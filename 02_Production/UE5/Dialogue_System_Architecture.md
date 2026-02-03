# Диалоговая Система для UE 5.7 + MetaHumans

## Обзор Архитектуры

```
┌─────────────────────────────────────────────────────────────┐
│                    DIALOGUE PIPELINE                        │
├─────────────────────────────────────────────────────────────┤
│  1. WRITING    →   2. DATA    →   3. UE IMPORT   →  4. RUNTIME │
│  (Obsidian/    │   (JSON/     │   (DataTables/   │   (Dialogue    │
│   Markdown)    │    YAML)     │    DataAssets)   │    Manager)    │
└─────────────────────────────────────────────────────────────┘
```

---

## 1. Формат Данных: JSON vs Альтернативы

### Рекомендация: **JSON** (с расширением для MetaHuman интеграции)

**Почему JSON:**
- ✅ Нативная поддержка в UE (`FJsonObject`, `FJsonSerializer`)
- ✅ DataTable импорт из JSON
- ✅ Версионируется в Git
- ✅ Можно редактировать вне UE
- ✅ Поддержка схем (JSON Schema для валидации)

**Альтернативы и почему отклонены:**
- **YAML** — нет нативной поддержки в UE, нужен плагин
- **XML** — многословен, сложно ручное редактирование
- **Ink/Yarn** — хороши для визуальных новелл, но ограничены для MetaHuman интеграции
- **ScriptableObjects** — сложно версионировать, нет возможности редактировать вне UE

### Структура JSON

```json
{
  "dialogue_id": "dlg_tutorial_meet_elara",
  "version": "1.0",
  "meta": {
    "title": "Встреча с Эларой",
    "location": "ark_market",
    "time_of_day": "morning",
    "participants": ["player", "elara"]
  },
  "entry_point": "node_001",
  "nodes": {
    "node_001": {
      "speaker": "elara",
      "text": "Приветствую, путник. Я вижу по твоим глазам — ты не из этих мест.",
      "meta_human": {
        "facial_expression": "curious",
        "gesture": "gesture_greeting",
        "look_at": "player",
        "voice_line": "vo_elara_001"
      },
      "options": [
        {
          "text": "Ты права. Я прибыл с... далека.",
          "next_node": "node_002",
          "condition": null,
          "effects": ["set_flag:met_elara"]
        },
        {
          "text": "Это тебя не касается.",
          "next_node": "node_003",
          "condition": null,
          "effects": ["change_disposition:elara:-10"]
        },
        {
          "text": "[Требуется: Руна Восприятия] Что ты видишь во мне?",
          "next_node": "node_004",
          "condition": "has_rune:perception",
          "effects": ["unlock_dialogue:elara_secret"]
        }
      ],
      "timeout": {
        "duration": 10.0,
        "default_option": 0,
        "on_timeout_animation": "impatient"
      }
    },
    "node_002": {
      "speaker": "elara",
      "text": "Далеко — это относительно понятие в нашем мире.",
      "meta_human": {
        "facial_expression": "intrigued",
        "gesture": "gesture_shrug",
        "camera_shot": "close_up"
      },
      "options": [
        {
          "text": "Продолжить...",
          "next_node": "node_005"
        }
      ]
    },
    "node_003": {
      "speaker": "elara",
      "text": "*холодно* Как скажешь. Но не жди помощи в Арке.",
      "meta_human": {
        "facial_expression": "cold",
        "gesture": "gesture_dismiss",
        "turn_away": true
      },
      "options": [
        {
          "text": "Подождите!",
          "next_node": "node_006"
        },
        {
          "text": "[Уйти]",
          "next_node": "END_DIALOGUE",
          "effects": ["close_dialogue"]
        }
      ]
    }
  },
  "variables": {
    "local": {
      "elara_mood": "neutral"
    },
    "global_refs": [
      "player_reputation",
      "main_quest_stage"
    ]
  }
}
```

---

## 2. Механики Диалогов

### Базовые Механики

| Механика | Описание | Реализация |
|----------|----------|------------|
| **Ветвление** | Выбор реплик влияет на ход диалога | `options[].next_node` |
| **Условия** | Доступность опций зависит от состояния игры | `options[].condition` |
| **Эффекты** | Изменение переменных при выборе | `options[].effects` |
| **Таймауты** | Автовыбор если игрок молчит | `timeout` |

### MetaHuman-Специфичные Механики

```json
{
  "meta_human": {
    "facial_expression": "emotion_name",
    "gesture": "animation_montage",
    "look_at": "target_actor",
    "voice_line": "audio_cue",
    "camera_shot": "shot_type",
    "blend_shapes": {
      "brow_raise": 0.7,
      "smile": 0.3
    }
  }
}
```

| Механика | Описание |
|----------|----------|
| **Facial Expressions** | Эмоции через Blend Shapes ( sadness, anger, joy, etc.) |
| **Gestures** | Полноценные анимации жестов через Animation Montages |
| **Lip Sync** | Синхронизация губ с аудио (Live Link Face или预录) |
| **Eye Contact** | Отслеживание взгляда (Look At Target) |
| **Camera Work** | Динамические смены планов (Close-up, Two-shot, etc.) |
| **Idle Variations** | Рандомные idle-анимации между репликами |

### Продвинутые Механики

```json
{
  "advanced_features": {
    "interruptible": true,
    "bark_system": {
      "enabled": true,
      "barks": ["bark_idle_01", "bark_idle_02"]
    },
    "dynamic_responses": {
      "context_aware": true,
      "time_sensitive": true,
      "weather_reactive": false
    },
    "multi_participant": {
      "enabled": true,
      "participants": ["elara", "merchant", "guard"]
    }
  }
}
```

---

## 3. Структура Проекта

### Директории

```
Content/
├── Dialogue/
│   ├── Data/
│   │   ├── Tables/           # DataTables (импорт из JSON)
│   │   ├── Characters/       # DataAssets персонажей
│   │   └── Localization/     # .csv файлы
│   ├── Blueprints/
│   │   ├── BP_DialogueManager.uasset
│   │   ├── BP_DialogueUI.uasset
│   │   ├── BP_MetaHumanController.uasset
│   │   └── Interfaces/
│   │       ├── BPI_DialogueParticipant.uasset
│   │       └── BPI_MetaHumanFace.uasset
│   └── Widgets/
│       ├── W_DialogueMain.uasset
│       ├── W_DialogueOption.uasset
│       └── W_DialoguePortrait.uasset
├── MetaHumans/
│   ├── Common/
│   │   ├── Animations/
│   │   │   ├── Gestures/     # Montages жестов
│   │   │   ├── Emotions/     # Blend shape presets
│   │   │   └── LipSync/      # Анимации губ
│   │   └── Materials/
│   └── Characters/
│       ├── Elara/
│       ├── Merchant/
│       └── etc.
└── Input/
    └── DialogueInputs/       # IA_DialogueNext, IA_DialogueChoice
```

### Ключевые Классы (C++ / Blueprint)

#### UDialogueDataAsset
```cpp
UCLASS(BlueprintType)
class MAGICA_API UDialogueDataAsset : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DialogueID;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FString, FDialogueNode> Nodes;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString EntryPoint;
};
```

#### UDialogueManager
```cpp
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAGICA_API UDialogueManager : public UActorComponent
{
    // Основной менеджер диалогов
    // - Загружает Dialogue Data
    // - Управляет состоянием (текущий node)
    // - Обрабатывает ввод игрока
    // - Триггерит MetaHuman анимации
};
```

#### UMetaHumanFaceController
```cpp
UCLASS()
class MAGICA_API UMetaHumanFaceController : public UActorComponent
{
    // Контроль лицевой мимики
    // - Blend shape interpolation
    // - Eye tracking
    // - Lip sync
};
```

---

## 4. Пайплайн Создания Диалогов

### Шаг 1: Написание (Obsidian)

Создай файл в Obsidian с использованием специального синтаксиса:

```markdown
---
dialogue_id: dlg_elara_intro
participants: [player, elara]
location: ark_market
---

# Встреча с Эларой

## node_start
**@elara** *(curious, gesture_greeting)*
> Приветствую, путник.

- [Ты права, я не отсюда](#node_truth)
- [Это не твоё дело](#node_rude) → disposition:-10
- [[Требуется Руна Восприятия]] Что ты видишь? → node_perception

## node_truth
**@elara** *(intrigued)*
> Далеко — это относительно...

- [Продолжить](#END)
```

### Шаг 2: Конвертация в JSON

Python скрипт конвертирует Markdown → JSON:

```python
# tools/dialogue_converter.py
import markdown
import json
import re

def parse_dialogue_md(md_content):
    """Парсит Markdown диалога в JSON структуру"""
    # ... реализация парсера
    return dialogue_json

# CLI: python dialogue_converter.py input.md output.json
```

### Шаг 3: Импорт в UE

1. JSON файл кладётся в `Content/Dialogue/Data/Raw/`
2. Editor Utility Widget импортирует в DataTable
3. Проверка схемы (JSON Schema validation)

### Шаг 4: Настройка MetaHuman

Для каждого персонажа создаётся:
- **Character Data Asset** — связь MetaHuman mesh ↔ Dialogue ID
- **Emotion Mapping** — emotion name → blend shape values
- **Gesture Library** — gesture ID → Animation Montage

---

## 5. Интеграция MetaHumans

### Компоненты на MetaHuman Character

```cpp
// На BP_MetaHumanCharacter
UActorComponent
├── UMetaHumanFaceController
│   ├── FaceMesh (skinned mesh)
   ├── BlendShapeTargets
   └── EyeTrackingComponent
├── UDialogueParticipant
│   ├── CharacterID
│   ├── VoiceComponent
│   └── GestureSlot (animation slot)
└── ULookAtController
    └── HeadTracking (bone control)
```

### Sequence диалога

```
1. DialogueManager::StartDialogue()
   ↓
2. Spawn/Focus Camera
   ↓
3. MetaHuman::PlayFacialExpression("curious", blend_time=0.3)
   ↓
4. MetaHuman::PlayGesture("greeting")
   ↓
5. AudioComponent::PlayVoiceLine("vo_elara_001")
   ↓
6. LipSync::StartSync("vo_elara_001")
   ↓
7. UI::ShowDialogueText("Приветствую...")
   ↓
8. WaitForInput() / Timeout
   ↓
9. TransitionToNextNode()
```

---

## 6. Инструменты

### Editor Utility Widget (UE)

Создать визуальный редактор диалогов:
- Node-based graph view
- Preview с MetaHuman
- Валидация связей
- Экспорт в JSON

### VS Code Extension

Для редактирования JSON:
- Схема валидации (JSON Schema)
- Автодополнение emotion/gesture IDs
- Preview диалога

---

## 7. Пример Полного Диалога

Смотри: `Examples/dlg_complete_example.json`

Ключевые особенности примера:
- 15+ нод
- Все типы условий
- MetaHuman анимации
- Ветвление с последствиями
- Интеграция с квестовой системой

---

## 8. Чеклист Интеграции

- [ ] Настроить C++ проект (или BP-only)
- [ ] Создать базовые DataAsset классы
- [ ] Импортировать MetaHuman
- [ ] Настроить Blend Shapes для эмоций
- [ ] Создать Animation Montages для жестов
- [ ] Настроить Lip Sync (OVRLipSync или родной)
- [ ] Создать DialogueManager
- [ ] Создать UI Widgets
- [ ] Написать конвертер Markdown→JSON
- [ ] Протестировать полный цикл

---

## Следующие Шаги

1. **Создать базовый C++ класс** DialogueManager
2. **Настроить MetaHuman** с базовыми эмоциями
3. **Создать тестовый диалог** в JSON
4. **Импортировать в UE** и протестировать

Готов помочь с любым из шагов! Какой хочешь начать?
