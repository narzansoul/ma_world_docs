# Пример: Интеграция Карты, Квестов и POI

## Конкретный Пример для Magic Awakening

### Зона: Арк Созидания (`zone_ark_creation`)

```
┌─────────────────────────────────────────────────────────────────┐
│                     АРК СОЗИДАНИЯ                               │
│                      (Zone Level 5-15)                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│    🏛️  [POI] Храм Рун ───────┬─────── [POI] Рынок 🏪            │
│         (temple)              │          (market)                │
│                               │                                  │
│    📜 Quest: "Пробуждение"    │    📜 Quest: "Встреча с Эларой"  │
│       Start: Enter Temple     │       Start: Talk to Elara       │
│       Reward: Rune of Power   │       Next: Old Well             │
│                               │                                  │
│    🔮 [POI] Башня Магов       │    👤 NPC: Элара                 │
│       (tower, locked)         │       Merchant, Quest Giver      │
│       Unlock: quest_003_done  │       Dialogue: dlg_elara        │
│                               │                                  │
│         │                     │           │                      │
│         │                     │           │                      │
│    🌊 [POI] Доки ◄────────────┴──────────► [POI] Таверна 🍺     │
│       (docks)                              (inn)                │
│       Fast Travel Hub                      Rest, Info            │
│                               │                                  │
│                               ▼                                  │
│                    ⚫ [POI] Старое Дерево                        │
│                       (landmark, secret)                         │
│                       Hidden until quest_002                     │
│                                                                  │
│    ═══════════════════════════════════════════════════════      │
│    Подземный Уровень (отдельная зона):                          │
│    [POI] Глубины Арка ──────► [POI] Зеркало Истины             │
│    (dungeon)                    (quest_objective)                │
│    Access: Well at night        Boss Fight                       │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## JSON Структуры

### Zone: Арк Созидания

```json
{
  "zone_id": "zone_ark_creation",
  "display_name": "Арк Созидания",
  "level_range": [5, 15],
  "description": "Плавающая цитадель, последнее прибежище магии",
  
  "map_data": {
    "texture": "map_ark_creation",
    "bounds": {
      "min": [-2000, -2000],
      "max": [2000, 2000]
    },
    "zoom_levels": [1.0, 0.5, 0.25]
  },
  
  "pois": [
    "poi_rune_temple",
    "poi_market",
    "poi_mage_tower",
    "poi_docks",
    "poi_tavern",
    "poi_old_tree"
  ],
  
  "connections": {
    "underground": "zone_ark_depths"
  },
  
  "unlock_condition": "quest_main_001_completed"
}
```

### POI: Рынок + Лавка Элары

```json
{
  "poi_id": "poi_market",
  "zone_id": "zone_ark_creation",
  "display_name": "Рынок",
  "poi_type": "settlement",
  "sub_type": "market",
  
  "position": {
    "world": [0, 0, 100],
    "map": [0.5, 0.5]
  },
  
  "discovery": {
    "auto_discover": true,
    "reveal_radius": 200
  },
  
  "child_pois": [
    {
      "poi_id": "poi_elara_shop",
      "display_name": "Лавка Элары",
      "type": "merchant",
      "position_offset": [50, 0, 0],
      
      "npcs": ["npc_elara"],
      
      "services": {
        "shop": {
          "inventory": "inv_elara_magic",
          "currency": "gold",
          "buy_multiplier": 0.5,
          "sell_multiplier": 1.5
        },
        "identify": {
          "cost": 50,
          "requires_item": true
        }
      },
      
      "quest_links": {
        "starts": ["quest_meet_elara", "quest_elara_secret"],
        "progresses": ["quest_main_003", "quest_rune_power"],
        "completes": ["quest_delivery_scrolls"]
      },
      
      "dialogues": {
        "default": "dlg_elara_shop_default",
        "quest_active": {
          "quest_elara_secret": "dlg_elara_secret_meeting"
        }
      }
    }
  ],
  
  "fast_travel": {
    "enabled": true,
    "cost": 0,
    "name": "Рынок Арка"
  },
  
  "ambient": {
    "sounds": ["amb_market_crowd", "amb_magic_hum"],
    "music": "music_ark_market",
    "time_variants": {
      "night": {
        "sounds": ["amb_market_night"],
        "npcs_despawn": ["npc_merchants"],
        "lights": ["light_lanterns"]
      }
    }
  }
}
```

### POI: Старое Дерево (Секрет)

```json
{
  "poi_id": "poi_old_tree",
  "zone_id": "zone_ark_creation",
  "display_name": "Старое Дерево",
  "poi_type": "landmark",
  "sub_type": "secret",
  
  "position": {
    "world": [-800, 1200, 50]
  },
  
  "discovery": {
    "auto_discover": false,
    "hidden_until": {
      "condition": "quest_stage",
      "quest_id": "quest_elara_secret",
      "stage": 2
    },
    "reveal_trigger": "player_proximity",
    "reveal_radius": 100
  },
  
  "visual_states": {
    "undiscovered": {
      "mesh": "tree_old_dead",
      "interactable": false
    },
    "discovered": {
      "mesh": "tree_old_magical",
      "particles": "pfx_magic_aura",
      "interactable": true
    }
  },
  
  "interaction": {
    "type": "examine",
    "dialogue": "dlg_old_tree_examine",
    "reveals": ["poi_hidden_entrance"]
  },
  
  "quest_links": {
    "progresses": ["quest_elara_secret"]
  }
}
```

### POI: Старый Колодец (Квестовая Локация)

```json
{
  "poi_id": "poi_old_well",
  "zone_id": "zone_ark_creation",
  "display_name": "Старый Колодец",
  "poi_type": "landmark",
  
  "position": {
    "world": [500, -300, 0]
  },
  
  "discovery": {
    "auto_discover": false,
    "hidden_until": {
      "condition": "quest_started",
      "quest_id": "quest_elara_secret"
    }
  },
  
  "time_conditions": {
    "accessible": {
      "time": "night",
      "else_dialogue": "dlg_well_day_closed"
    }
  },
  
  "zone_transition": {
    "enabled": true,
    "target_zone": "zone_ark_depths",
    "target_spawn": "spawn_well_bottom",
    "condition": "time == night"
  },
  
  "quest_links": {
    "objectives": [
      {
        "quest_id": "quest_elara_secret",
        "stage": 2,
        "action": "investigate",
        "completes_stage": true
      }
    ]
  },
  
  "dynamic_marker": {
    "quest_marker": "quest_elara_secret",
    "icon": "icon_quest_main",
    "color": "gold",
    "visible": "quest_active"
  }
}
```

---

## Связь Квест → Карта

### Квест: "Секрет Элары" (`quest_elara_secret`)

```json
{
  "quest_id": "quest_elara_secret",
  "title": "Секрет Элары",
  
  "stages": [
    {
      "stage": 1,
      "title": "Поговорите с Эларой",
      "objectives": [
        {
          "type": "talk",
          "target_npc": "npc_elara",
          "location": {
            "type": "at_poi",
            "poi_id": "poi_elara_shop"
          }
        }
      ],
      "map_markers": [
        {
          "target": "poi_elara_shop",
          "icon": "icon_quest_talk",
          "label": "Элара"
        }
      ]
    },
    {
      "stage": 2,
      "title": "Исследуйте Старый Колодец",
      "description": "Элара сказала встретиться у Старого Колодца ночью",
      
      "conditions": {
        "time": "night"
      },
      
      "objectives": [
        {
          "type": "investigate",
          "target": "poi_old_well",
          "sub_conditions": {
            "time": "night",
            "enter_zone": "zone_ark_depths"
          }
        }
      ],
      
      "map_markers": [
        {
          "target": "poi_old_well",
          "icon": "icon_quest_investigate",
          "label": "Старый Колодец (ночью)",
          "dynamic": {
            "hidden_during_day": true,
            "radius_indicator": 150
          }
        }
      ],
      
      "stage_events": {
        "on_enter": {
          "reveal_poi": "poi_old_well",
          "unlock_dialogue": "dlg_elara_secret_night"
        }
      }
    },
    {
      "stage": 3,
      "title": "Найдите Зеркало Истины",
      "objectives": [
        {
          "type": "find_item",
          "item": "mirror_of_truth",
          "location": {
            "type": "in_zone",
            "zone_id": "zone_ark_depths",
            "specific_poi": "poi_mirror_chamber"
          }
        }
      ],
      "map_markers": [
        {
          "target": "zone_ark_depths",
          "icon": "icon_quest_dungeon",
          "label": "Глубины Арка",
          "area_highlight": true
        }
      ]
    },
    {
      "stage": 4,
      "title": "Вернитесь к Эларе",
      "objectives": [
        {
          "type": "talk",
          "target_npc": "npc_elara",
          "location": "poi_elara_shop"
        }
      ],
      "branching": {
        "choices": [
          {
            "condition": "has_item:mirror_of_truth",
            "dialogue": "dlg_elara_return_with_mirror",
            "reward": "exp:1000, item:rune_secret"
          },
          {
            "condition": "!has_item:mirror_of_truth",
            "dialogue": "dlg_elara_return_empty",
            "consequence": "disposition_elara:-20"
          }
        ]
      }
    }
  ]
}
```

---

## Flow Diagram

```
Начало Квеста
     │
     ▼
┌─────────────────────────────────────────┐
│ Игрок заходит в зону "Ark of Creation"  │
│ [ZoneVolume OnBeginOverlap]             │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Игрок подходит к POI "Elara's Shop"    │
│ • POI появляется на компасе            │
│ • Иконка на карте (если открыта)       │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Взаимодействие с NPC "Элара"            │
│ DialogueManager проверяет:              │
│ • Есть активные квесты?                 │
│ • Доступен старт quest_elara_secret?    │
│ → Показывает диалог dlg_elara_shop      │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Игрок принимает квест                   │
│ QuestManager:                           │
│ • Добавляет quest_elara_secret          │
│ • Stage = 1                             │
│ • Создаёт MapMarker к poi_elara_shop    │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Поговорил с Эларой → Stage 2            │
│ • Новый MapMarker: poi_old_well         │
│ • Скрыт до наступления ночи             │
│ • Компас показывает направление         │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Игрок подходит к колодцу ДНЁМ           │
│ TimeCondition: fail                     │
│ → Показывает dlg_well_day_closed        │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Игрок подходит к колодцу НОЧЬЮ          │
│ • TimeCondition: pass                   │
│ • Открывается переход в zone_ark_depths │
│ • POI меняет состояние на "active"      │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Игрок находит Зеркало                   │
│ • Обновляет Stage 3 → 4                 │
│ • Маркер возврата к Эларе               │
└─────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────┐
│ Возврат к Эларе                         │
│ • Проверка: есть Зеркало?               │
│ • Да → dlg_elara_success + награда      │
│ • Нет → dlg_elara_fail                  │
└─────────────────────────────────────────┘
```

---

## Визуальная Иерархия

```
World (Magic Awakening)
│
├── Zone: Ark of Creation
│   ├── POI: Market (Settlement)
│   │   └── Child: Elara's Shop
│   │       ├── NPC: Elara
│   │       ├── Shop Inventory
│   │       └── Quest Start: quest_elara_secret
│   │
│   ├── POI: Rune Temple (Landmark)
│   │   └── Quest Start: quest_awakening
│   │
│   ├── POI: Old Well (Landmark)
│   │   ├── Time Gate: Night only
│   │   ├── Zone Transition → Ark Depths
│   │   └── Quest Progress: quest_elara_secret (Stage 2)
│   │
│   └── POI: Old Tree (Secret)
│       ├── Hidden until: quest_elara_secret Stage 2
│       └── Interaction reveals: Hidden Entrance
│
└── Zone: Ark Depths (Dungeon)
    ├── POI: Well Bottom (Spawn)
    ├── POI: Mirror Chamber (Objective)
    │   └── Item: Mirror of Truth
    └── POI: Ancient Cache (Secret)
        └── Requires: Rune of Perception
```

---

## Компоненты для Реализации

### 1. Map Marker Component
```cpp
UCLASS(ClassGroup=(Magic))
class UMapMarkerComponent : public UActorComponent
{
    UPROPERTY(EditAnywhere)
    EMapMarkerType MarkerType;
    
    UPROPERTY(EditAnywhere)
    bool bDynamic; // Движется ли маркер?
    
    UPROPERTY(EditAnywhere)
    FString LinkedQuestID; // Привязка к квесту
    
    // Обновить положение на UI карты
    UFUNCTION()
    void UpdateMapPosition();
};
```

### 2. Quest Location Volume
```cpp
UCLASS()
class AQuestLocationVolume : public ATriggerVolume
{
    UPROPERTY(EditAnywhere)
    FQuestLocationData LocationData;
    
    UFUNCTION()
    void OnPlayerEnter(ACharacter* Player);
    
    // Автоматически прогрессирует квест
    UPROPERTY(EditAnywhere)
    bool bAutoProgressQuest;
};
```

### 3. Time Gate Component
```cpp
UCLASS()
class UTimeGateComponent : public UActorComponent
{
    UPROPERTY(EditAnywhere)
    ETimeOfDay AllowedTime; // Morning, Day, Evening, Night
    
    UPROPERTY(EditAnywhere)
    FString BlockedDialogueID; // Что сказать если не то время
    
    UFUNCTION()
    bool CanInteract() const;
};
```

---

## Чеклист для Дизайнера

### Создание Зоны
- [ ] Определить границы зоны
- [ ] Настроить Level Bounds
- [ ] Добавить ZoneVolume
- [ ] Создать Zone Data Asset
- [ ] Настроить карту (текстура, зум)

### Создание POI
- [ ] Разместить POI Actor в мире
- [ ] Настроить Discovery (авто/ручное)
- [ ] Добавить визуальные состояния
- [ ] Настроить Fast Travel (если нужно)
- [ ] Связать с Quest Data Assets

### Интеграция Квеста
- [ ] Добавить Quest Location в стадии
- [ ] Настроить Map Markers
- [ ] Проверить условия (время, флаги)
- [ ] Протестировать полный flow
- [ ] Проверить edge cases (днём/ночью, без квеста)

---

**Связанные документы:**
- [Dialogue System](./Dialogue_System_Architecture.md)
- [Quest System](./Quest_System.md) (TODO)
- [Save System](./Save_System.md) (TODO)
