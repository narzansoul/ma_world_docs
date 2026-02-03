# Dialogue System for UE 5.7 + MetaHumans

Полноценная диалоговая система для Magic Awakening с интеграцией MetaHuman персонажей.

## 📋 Быстрый старт

```bash
# 1. Установка Python зависимостей
pip install -r Tools/requirements.txt

# 2. Конвертация диалога из Obsidian в JSON
python Tools/dialogue_converter.py \
    Examples/dlg_elara_obsidian_template.md \
    Output/dlg_elara.json

# 3. Импорт в UE через Editor Utility
# Content/Dialogue/Editor/EUW_DialogueImporter
```

## 🏗️ Архитектура

```
┌─────────────────────────────────────────────────────────────┐
│                        PIPELINE                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Obsidian (.md)  →  JSON  →  DataTable  →  Runtime          │
│       ↓              ↓          ↓            ↓              │
│   Писатель      Конвертер   UE Import   DialogueManager     │
│   Создаёт       Python      Editor      C++/BP              │
│   текст         скрипт      Utility     Логика              │
│                                                              │
│                    MetaHuman Integration                     │
│                         ↓                                   │
│         Facial Expressions / Gestures / Lip Sync            │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

## 📁 Структура

```
02_Production/UE5/Dialogue/
├── Source/                      # C++ код
│   ├── DialogueTypes.h         # Структуры данных
│   ├── DialogueManager.h       # Основной менеджер
│   └── MetaHumanFaceController.h  # Контроль мимики
├── Tools/                       # Python скрипты
│   ├── dialogue_converter.py   # Markdown → JSON
│   └── requirements.txt
├── Examples/                    # Примеры
│   ├── dlg_elara_obsidian_template.md
│   └── dlg_elara_complete_demo.json
├── dialogue-schema.json        # JSON Schema для валидации
└── README.md                   # Этот файл
```

## 🎮 Фичи

### Базовые
- ✅ Ветвящиеся диалоги с выбором
- ✅ Условия доступности опций (флаги, руны, квесты)
- ✅ Эффекты от выборов (изменение диспозиции, флаги, квесты)
- ✅ Таймауты с авто-выбором
- ✅ Локализация-ready

### MetaHuman интеграция
- ✅ **Facial Expressions** — 15+ эмоций через Blend Shapes
- ✅ **Gestures** — Animation Montages для жестов
- ✅ **Lip Sync** — синхронизация губ с аудио
- ✅ **Eye Tracking** — отслеживание взгляда
- ✅ **Camera Work** — динамические смены планов

### Инструменты
- ✅ Конвертер Markdown → JSON
- ✅ JSON Schema валидация
- ✅ Editor Utility Widget для UE

## 📝 Как писать диалоги

### 1. В Obsidian создай файл:

```markdown
---
dialogue_id: dlg_my_dialogue
participants: [player, npc_name]
---

## node_start
**@npc_name** *(curious, gesture_greeting)*
> Привет, путник!

- [Привет!](#node_friendly) → set_flag:met_npc
- [Кто ты?](node_who_are_you)
```

### 2. Конвертируй:

```bash
python Tools/dialogue_converter.py input.md output.json
```

### 3. Импортируй в UE:

- Помести JSON в `Content/Dialogue/Data/Raw/`
- Запусти Editor Utility Widget
- Нажми "Import to DataTable"

## 🎭 MetaHuman интеграция

### Настройка персонажа

1. **Добавь компоненты на MetaHuman BP:**
   ```
   UMetaHumanFaceController
   UDialogueParticipant
   ```

2. **Создай DataAsset для персонажа:**
   - Character ID
   - Привязка к MetaHuman mesh
   - Voice pack

3. **Настрой эмоции:**
   ```cpp
   // В MetaHumanFaceController
   AddEmotionPreset("curious", {
       {"brow_raise", 0.7f},
       {"head_tilt", 0.3f}
   });
   ```

### Использование в диалоге

```json
{
  "speaker": "elara",
  "text": "Приветствую!",
  "meta_human": {
    "facial_expression": "curious",
    "gesture": "gesture_greeting",
    "look_at": "player",
    "voice_line": "vo_elara_001",
    "camera_shot": "medium_shot"
  }
}
```

## 🧪 Тестирование

### Unit тесты
```bash
cd Tools
python -m pytest tests/
```

### В UE
1. Открой `L_DialogueTest`
2. Запусти PIE
3. Подойди к NPC и нажми E

## 📚 Документация

- [Полная архитектура](./Dialogue_System_Architecture.md)
- [JSON Schema](./dialogue-schema.json)
- [C++ API](./Source/)

## 🚀 Roadmap

- [ ] Yarn Spinner интеграция (опционально)
- [ ] Визуальный редактор нод в UE
- [ ] Live Lip Sync через OVRLipSync
- [ ] Динамические диалоги (AI-generated)
- [ ] Voice acted lines pipeline

## 💡 Best Practices

1. **Именование:**
   - `dlg_` prefix для dialogue_id
   - `node_` prefix для node IDs
   - `vo_` prefix для voice lines

2. **Структура:**
   - Один файл = один диалог
   - Разделяй диалоги по локациям/квестам
   - Используй entry_point для начала

3. **MetaHuman:**
   - Не перегружай blend shapes
   - Используй пресеты эмоций
   - Тестируй с разных ракурсов

4. **Производительность:**
   - Lazy load диалоги
   - Кэшируй DataAssets
   - Unload после завершения

## ❓ FAQ

**Q: Можно ли использовать без MetaHuman?**
A: Да, просто не заполняй `meta_human` секцию.

**Q: Как добавить новую эмоцию?**
A: В `MetaHumanFaceController` добавь пресет с blend shape значениями.

**Q: Поддерживается ли локализация?**
A: Да, используй `NSLOCTEXT` или `FText::FromStringTable`.

**Q: Можно ли менять диалоги runtime?**
A: Да, DialogueManager поддерживает hot-reload.

## 🔗 Связанные документы

- [Архитектура системы](./Dialogue_System_Architecture.md)
- [Пример диалога (JSON)](./Examples/dlg_elara_complete_demo.json)
- [Пример диалога (Markdown)](./Examples/dlg_elara_obsidian_template.md)
- [C++ Types](./Source/DialogueTypes.h)

---

**Готов к использованию!** Начни с примера `dlg_elara_obsidian_template.md`
