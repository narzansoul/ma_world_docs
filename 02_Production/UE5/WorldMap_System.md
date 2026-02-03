# Система Карт: Интеграция с Квестами и POI

## Архитектура Связей

```
┌─────────────────────────────────────────────────────────────────┐
│                    WORLD MAP SYSTEM                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   ┌─────────────┐      ┌─────────────┐      ┌─────────────┐     │
│   │    ZONES    │◄────►│    POIs     │◄────►│   QUESTS    │     │
│   │  (Зоны)     │      │(Точки Интерес)│    │  (Квесты)   │     │
│   └──────┬──────┘      └──────┬──────┘      └──────┬──────┘     │
│          │                     │                     │           │
│          ▼                     ▼                     ▼           │
│   ┌─────────────┐      ┌─────────────┐      ┌─────────────┐     │
│   │  LOCATIONS  │      │  DIALOGUES  │      │   REWARDS   │     │
│   │ (Локации)   │      │ (Диалоги)   │      │ (Награды)   │     │
│   └─────────────┘      └─────────────┘      └─────────────┘     │
│                                                                  │
│   Связи: Zone ► POI ► Quest ► Dialogue ► Reward                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 1. Структура Данных

### 1.1 Zone (Зона/Регион)

```json
{
  "zone_id": "zone_ark_of_creation",
  "display_name": "Арк Созидания",
  "description": "Плавающий город-цитадель, центр магии и технологий",
  "level_range": [5, 15],
  "coordinates": {
    "world_position": [0, 0, 0],
    "map_position": [0.5, 0.3]
  },
  "connections": {
    "north": "zone_frozen_wastes",
    "south": "zone_southern_plains",
    "east": "zone_eastern_reaches",
    "west": null
  },
  "pois": ["poi_market", "poi_temple", "poi_docks"],
  "ambient_events": ["event_market_busy", "event_temple_chant"],
  "music_cue": "music_ark_ambient",
  "loading_screen": "ls_ark_creation",
  "unlock_condition": "quest_main_001_completed"
}
```

### 1.2 Point of Interest (POI)

```json
{
  "poi_id": "poi_elara_shop",
  "zone_id": "zone_ark_of_creation",
  "display_name": "Лавка Элары",
  "poi_type": "merchant",
  "sub_type": "magic_shop",
  "coordinates": {
    "world_position": [1250, 340, -200],
    "map_icon": "icon_shop_magic"
  },
  "discovery": {
    "auto_discover": false,
    "reveal_radius": 50,
    "reveal_condition": "enter_area",
    "hidden_until": "quest_meet_elara_started"
  },
  "interactions": {
    "dialogue": "dlg_elara_shop",
    "shop_inventory": "inv_elara_tier1",
    "services": ["buy", "sell", "identify", "enchant"]
  },
  "linked_quests": {
    "starts": ["quest_elara_secret", "quest_rune_perception"],
    "progresses": ["quest_main_003"],
    "completes": ["quest_delivery_runes"]
  },
  "npcs_present": ["npc_elara", "npc_shop_guard"],
  "ambient": {
    "sounds": ["amb_shop_bells", "amb_magic_hum"],
    "particles": ["pfx_magic_dust"],
    "time_variants": {
      "night": {
        "lights": ["light_shop_lanterns"],
        "npcs": ["npc_elara_night"]
      }
    }
  },
  "fast_travel": {
    "enabled": true,
    "unlock_condition": "discover_poi",
    "cost": 0
  }
}
```

### 1.3 Quest Location Marker

```json
{
  "marker_id": "marker_find_mirror",
  "quest_id": "quest_mirror_of_truth",
  "stage": 2,
  "marker_type": "objective",
  "target_poi": "poi_old_well",
  "fallback_zone": "zone_ark_undercity",
  "visibility": {
    "show_on_map": true,
    "show_compass": true,
    "show_world_marker": true,
    "radius_indicator": 100
  },
  "dynamic_position": {
    "enabled": false,
    "update_trigger": "none"
  },
  "ui_data": {
    "icon": "icon_quest_main",
    "color": "gold",
    "label": "Зеркало Истины"
  }
}
```

---

## 2. Типы POI и Их Роли

### 2.1 Классификация POI

| Тип | Подтипы | Роль в Квестах | Пример |
|-----|---------|----------------|--------|
| **Settlement** | City, Village, Camp | Хаб для квестов, NPC | Арк Созидания |
| **Dungeon** | Cave, Ruins, Tower | Квестовые цели, лут | Глубины Арка |
| **Landmark** | Monument, Natural | Навигация, открытия | Старое Древо |
| **Resource** | Mine, Farm, Hunting | Сбор, крафт | Кристальные Шахты |
| **Service** | Shop, Inn, Temple | Сюжет, отдых | Лавка Элары |
| **Event** | Battlefield, Ritual | Динамичные квесты | Поле Битвы |
| **Secret** | Hidden, Locked | Опциональные квесты | Тайная Комната |

### 2.2 Состояния POI

```cpp
UENUM(BlueprintType)
enum class EPOIState : uint8
{
    Undiscovered    UMETA(DisplayName="Не открыт"),      // Нет на карте
    Discovered      UMETA(DisplayName="Открыт"),         // Виден на карте
    Active          UMETA(DisplayName="Активен"),        // Есть активный контент
    Completed       UMETA(DisplayName="Завершён"),       // Всё выполнено
    Inaccessible    UMETA(DisplayName="Недоступен"),     // Заблокирован
    Dangerous       UMETA(DisplayName="Опасен")          // Высокий уровень угрозы
};
```

---

## 3. Связи Квест-Карта

### 3.1 Quest Location Types

```cpp
UENUM(BlueprintType)
enum class EQuestLocationType : uint8
{
    // Статичные
    ExactLocation       UMETA(DisplayName="Точная точка"),
    POI                 UMETA(DisplayName="POI"),
    Zone                UMETA(DisplayName="Зона"),
    
    // Динамические
    NPCFollow           UMETA(DisplayName="Следовать за NPC"),
    PlayerProximity     UMETA(DisplayName="Рядом с игроком"),
    RandomInZone        UMETA(DisplayName="Случайно в зоне"),
    
    // Условные
    Conditional         UMETA(DisplayName="Условное место"),
    MultiLocation       UMETA(DisplayName="Несколько мест")
};
```

### 3.2 Примеры Связей

**Пример 1: Простой квест "Доставка"**
```
Quest: "Доставить письмо Эларе"
├─ Start: POI "City Gate" (автостарт при входе)
├─ Objective 1: MoveTo POI "Elara's Shop"
├─ Objective 2: TalkTo NPC "Elara" (внутри POI)
└─ Complete: POI "Elara's Shop"
```

**Пример 2: Исследование с секретом**
```
Quest: "Зеркало Истины"
├─ Start: POI "Market" (диалог с Эларой)
├─ Objective 1: Investigate POI "Old Well" (ночью)
├─ Objective 2: Find Secret POI "Hidden Chamber" 
│  └─ Condition: Требуется Руна Восприятия
├─ Objective 3: Interact Object "Mirror of Truth"
│  └─ Location: Inside Secret POI
└─ Complete: Return to POI "Market"
   └─ Alternative: POI "Temple" (если предал Элару)
```

**Пример 3: Динамический квест**
```
Quest: "Охота на монстра"
├─ Start: Zone "Wilderness" (случайная встреча)
├─ Objective 1: Track Monster
│  └─ LocationType: NPCFollow (монстр бродит)
├─ Objective 2: Defeat Monster
│  └─ Location: Current position
└─ Complete: Return to any POI типа "Settlement"
```

---

## 4. Техническая Реализация (UE 5)

### 4.1 C++ Классы

#### UWorldMapDataAsset
```cpp
UCLASS(BlueprintType)
class MAGICA_API UWorldMapDataAsset : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString WorldID;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<UZoneDataAsset*> Zones;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FZoneConnection> Connections;
    
    // Получить зону по ID
    UFUNCTION(BlueprintCallable)
    UZoneDataAsset* GetZoneByID(const FString& ZoneID) const;
    
    // Получить все доступные из зоны
    UFUNCTION(BlueprintCallable)
    TArray<UZoneDataAsset*> GetConnectedZones(UZoneDataAsset* FromZone) const;
};
```

#### UZoneDataAsset
```cpp
UCLASS(BlueprintType)
class MAGICA_API UZoneDataAsset : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ZoneID;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<UPOIDataAsset*> PointsOfInterest;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FIntPoint LevelRange;
    
    // Условие разблокировки
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString UnlockCondition;
    
    // Получить POI по ID
    UFUNCTION(BlueprintCallable)
    UPOIDataAsset* GetPOIByID(const FString& POIID) const;
    
    // Получить активные квесты в зоне
    UFUNCTION(BlueprintCallable)
    TArray<UQuestDataAsset*> GetActiveQuests() const;
};
```

#### UPoiDataAsset
```cpp
UCLASS(BlueprintType)
class MAGICA_API UPoiDataAsset : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString POIID;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TEnumAsByte<EPOIType> POIType;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector WorldPosition;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FPOIDiscoveryData DiscoveryData;
    
    // Связанные квесты
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FPOIQuestLinks QuestLinks;
    
    // Текущее состояние
    UPROPERTY(BlueprintReadOnly)
    TEnumAsByte<EPOIState> CurrentState = EPOIState::Undiscovered;
    
    // Открыть POI
    UFUNCTION(BlueprintCallable)
    void Discover(AMagicPlayerController* Player);
    
    // Проверить доступность
    UFUNCTION(BlueprintCallable)
    bool IsAccessible(const AMagicPlayerController* Player) const;
    
    // Получить активные маркеры квестов
    UFUNCTION(BlueprintCallable)
    TArray<FQuestMarkerData> GetQuestMarkers() const;
};
```

### 4.2 Компоненты на Уровне

```cpp
// На Level Blueprint или Actor
UCLASS(ClassGroup=(Magic), meta=(BlueprintSpawnableComponent))
class MAGICA_API UZoneVolumeComponent : public UBoxComponent
{
    GENERATED_BODY()
    
public:
    UZoneVolumeComponent();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UZoneDataAsset* ZoneData;
    
    // POI внутри этой зоны
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<UPOIDataAsset*> ContainedPOIs;
    
protected:
    UFUNCTION()
    void OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, 
                       AActor* OtherActor,
                       UPrimitiveComponent* OtherComp,
                       int32 OtherBodyIndex,
                       bool bFromSweep,
                       const FHitResult& SweepResult);
    
    UFUNCTION()
    void OnPlayerExit(UPrimitiveComponent* OverlappedComponent,
                      AActor* OtherActor,
                      UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex);
};
```

### 4.3 POI Actor в Мире

```cpp
UCLASS()
class MAGICA_API APointOfInterest : public AActor
{
    GENERATED_BODY()
    
public:
    APointOfInterest();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POI")
    UPoiDataAsset* POIData;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POI")
    USphereComponent* DiscoveryVolume;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POI")
    UBillboardComponent* MapIcon;
    
    // Визуальное состояние
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
    TArray<UStaticMesh*> StateMeshes;
    
    // Инициализация
    virtual void BeginPlay() override;
    
    // Обновить визуал по состоянию
    UFUNCTION(BlueprintCallable)
    void UpdateVisualState(EPOIState NewState);
    
    // Взаимодействие
    UFUNCTION(BlueprintCallable)
    void OnInteract(AMagicCharacter* Interactor);
    
    // Получить диалог для этого POI
    UFUNCTION(BlueprintCallable)
    UDialogueDataAsset* GetCurrentDialogue() const;
};
```

---

## 5. UI/UX Интеграция

### 5.1 Карта Мира

```cpp
UCLASS()
class MAGICA_API UWorldMapWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UPROPERTY(meta=(BindWidget))
    UCanvasPanel* MapCanvas;
    
    UPROPERTY(meta=(BindWidget))
    UImage* ZoneMapImage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UPOIWidget> POIWidgetClass;
    
    // Показать зону
    UFUNCTION(BlueprintCallable)
    void DisplayZone(UZoneDataAsset* Zone);
    
    // Добавить POI на карту
    UFUNCTION(BlueprintCallable)
    UPOIWidget* AddPOIToMap(UPOIDataAsset* POI);
    
    // Обновить маркеры квестов
    UFUNCTION(BlueprintCallable)
    void UpdateQuestMarkers();
    
    // Фильтры
    UFUNCTION(BlueprintCallable)
    void SetFilter(EPoiType Type, bool bVisible);
    
    // Фаст-травел
    UFUNCTION(BlueprintCallable)
    void ShowFastTravelDialog(UPOIDataAsset* TargetPOI);
};
```

### 5.2 Компас

```cpp
UCLASS()
class MAGICA_API UCompassWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    // Обновление маркеров на компасе
    UFUNCTION(BlueprintCallable)
    void UpdateMarkers(const FVector& PlayerLocation, const FRotator& PlayerRotation);
    
    // Добавить маркер квеста
    UFUNCTION(BlueprintCallable)
    void AddQuestMarker(const FQuestMarkerData& Marker);
    
    // Добавить POI маркер
    UFUNCTION(BlueprintCallable)
    void AddPOIMarker(UPOIDataAsset* POI);
    
    // Удалить маркер
    UFUNCTION(BlueprintCallable)
    void RemoveMarker(const FString& MarkerID);
};
```

---

## 6. Динамические Системы

### 6.1 Энкаунтеры близ POI

```json
{
  "poi_id": "poi_forest_cabin",
  "random_encounters": {
    "enabled": true,
    "trigger_radius": 500,
    "encounters": [
      {
        "id": "enc_bandits",
        "weight": 40,
        "condition": "time == night",
        "enemies": ["bandit_archer", "bandit_melee"]
      },
      {
        "id": "enc_merchant",
        "weight": 20,
        "condition": "day && !quest_active_001",
        "npc": "npc_traveling_merchant"
      }
    ]
  }
}
```

### 6.2 Временные События

```json
{
  "poi_id": "poi_market",
  "time_events": {
    "morning": {
      "npcs": ["npc_merchant_morning"],
      "shops_open": false,
      "ambient": "amb_market_setup"
    },
    "day": {
      "npcs": ["npc_merchants_all"],
      "shops_open": true,
      "ambient": "amb_market_busy",
      "special": "event_daily_deal"
    },
    "night": {
      "npcs": ["npc_guard_night"],
      "shops_open": false,
      "ambient": "amb_market_empty",
      "danger_level": 2
    }
  }
}
```

---

## 7. Диалоговая Интеграция

### 7.1 POI-Условия в Диалогах

```json
{
  "dialogue_id": "dlg_elara_secret",
  "conditions": {
    "location": {
      "type": "at_poi",
      "poi_id": "poi_old_well",
      "time": "night"
    }
  },
  "options": [
    {
      "text": "Показать Зеркало",
      "condition": "has_item:mirror_of_truth && at_poi:poi_hidden_chamber",
      "next_node": "node_reveal_truth"
    }
  ]
}
```

### 7.2 Динамические Диалоги по Локации

```cpp
// В DialogueManager
UFUNCTION(BlueprintCallable)
UDialogueDataAsset* SelectDialogueForPOI(UPOIDataAsset* POI, AMagicCharacter* Player)
{
    // Проверить квестовые диалоги
    for (auto Quest : POI->QuestLinks.Starts)
    {
        if (Quest->CanStart(Player))
            return Quest->StartDialogue;
    }
    
    // Проверить прогресс квестов
    for (auto Quest : POI->QuestLinks.Progresses)
    {
        if (Player->QuestManager->HasActiveQuest(Quest))
            return Quest->GetCurrentStageDialogue();
    }
    
    // Стандартный диалог POI
    return POI->DefaultDialogue;
}
```

---

## 8. Примеры Использования

### 8.1 Flow: Игрок получает квест

```
1. Игрок входит в Zone "Ark of Creation"
   └─► ZoneVolume вызывает OnPlayerEnter()
   
2. Игрок подходит к POI "Elara's Shop"
   └─► DiscoveryVolume срабатывает
   └─► POI переходит в Discovered
   └─► Добавляется на карту
   
3. Игрок взаимодействует с NPC
   └─► DialogueManager проверяет:
       - Активные квесты в POI
       - Доступные старты квестов
       - Условия (время, флаги)
   
4. Диалог даёт квест "Mirror of Truth"
   └─► QuestManager создаёт квест
   └─► MapMarker добавляется на карту
   └─► Compass показывает направление
   
5. Игрок следует маркеру к POI "Old Well"
   └─► При приближении проверяется время
   └─► Если ночь — доступна опция квеста
   └─► Если день — Элара просит прийти ночью
```

### 8.2 Flow: Быстрое путешествие

```
1. Игрок открывает карту (M)
2. Выбирает открытый POI с fast_travel.enabled
3. Система проверяет:
   - Достаточно ли ресурсов?
   - Нет ли врагов рядом?
   - Не в бою ли игрок?
4. Подтверждение → Loading Screen
5. Спавн игрока у POI
```

---

## 9. Инструменты для Дизайнеров

### 9.1 Editor Utility Widget

```cpp
UCLASS()
class MAGICA_API UMapEditorWidget : public UEditorUtilityWidget
{
    // Визуальное редактирование связей
    // Drag-and-drop POI на карту
    // Автоматическая генерация JSON
    // Валидация связей
};
```

### 9.2 Валидация

```cpp
// Проверить целостность мира
bool ValidateWorldMap(UWorldMapDataAsset* World)
{
    // Все POI имеют валидные zone_id?
    // Все квестовые ссылки ведут к существующим POI?
    // Нет дубликатов ID?
    // Все зоны связаны (нет изолированных)?
}
```

---

## 10. Чеклист Внедрения

- [ ] Создать структуру зон (Zones)
- [ ] Расставить POI Actor'ы на уровнях
- [ ] Настроить связи Zone ↔ POI
- [ ] Добавить Quest Links к POI
- [ ] Создать UI карты
- [ ] Настроить компас
- [ ] Реализовать Fast Travel
- [ ] Интегрировать с Dialogue System
- [ ] Добавить Discovery System
- [ ] Тестировать с разными состояниями

---

## Ссылки на Реализацию

- C++ классы: `Source/WorldMap/`
- Data Assets: `Content/World/`
- Blueprints: `Content/World/BP_`
- UI Widgets: `Content/UI/Map/`
