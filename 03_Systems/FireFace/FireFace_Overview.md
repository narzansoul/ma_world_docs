# FireFace — Dialogue System for Magic Awakening

> **Версия:** 0.1.0-alpha  
> **Статус:** Проектирование  
> **Целевой движок:** Unreal Engine 5.7

---

## Что такое FireFace

FireFace — это модульная система диалогов для **Magic Awakening: The Aron Heir**, объединяющая:
- Визуальную новелльную подачу
- Продвнутую анимацию лиц (MetaHuman + Audio Driven Animation)
- Распознавание эмоций в реальном времени
- Два режима работы: полная кинематографичность и быстрая встроенная речь

---

## Архитектура системы

```
┌─────────────────────────────────────────────────────────────┐
│                    FireFace Core                            │
├─────────────────────────────────────────────────────────────┤
│  Dialogue Manager                                           │
│  ├── Dialogue Data Table (Scriptable Objects)              │
│  ├── State Machine (Conversation Flow)                     │
│  └── Event Dispatcher (Quest Triggers, Flags)              │
├─────────────────────────────────────────────────────────────┤
│  Рендер-режимы                                              │
│  ├── Mode A: Cinematic Face Motion                          │
│  │   ├── MetaHuman Face Graph                               │
│  │   ├── Audio2Face (NVIDIA) / Live Link Face               │
│  │   └── Full Body Animation (Motion Matching)              │
│  └── Mode B: Voice-Driven Built-in                          │
│      ├── TTS (Text-to-Speech)                               │
│      ├── Phoneme-based Lip Sync                             │
│      └── Simplified Emotion Presets                         │
├─────────────────────────────────────────────────────────────┤
│  UI Layer                                                   │
│  ├── Dialogue Widget (Rich Text, Choices)                   │
│  ├── Speaker Portrait (Dynamic Lighting)                    │
│  └── Subtitle System (Multi-language)                       │
└─────────────────────────────────────────────────────────────┘
```

---

## Два режима диалогов

### Mode A: Cinematic Face Motion
**Когда использовать:** Ключевые сюжетные сцены, важные NPC, эмоциональные моменты

**Pipeline:**
1. **Подготовка контента**
   - Текст диалога → Разбивка на реплики
   - Аудиозапись (голосовой актёр) → WAV 48kHz
   - Анализ аудио → Phonemes + Emotion markers

2. **Face Animation**
   - **Option 1:** Audio2Face (NVIDIA Omniverse) → FBX sequence
   - **Option 2:** Live Link Face (iPhone) → Real-time capture
   - **Option 3:** Manual keyframing в Face Graph

3. **Body Animation**
   - Motion Matching для idles
   - Жесты привязаны к ключевым словам (метки в тексте)

4. **Интеграция в UE5**
   - Level Sequence с Camera Cuts
   - Control Rig для микро-выражений
   - Post-process (depth of field, eye focus)

**Ограничения:**
- Длительная подготовка (1-2 дня на сцену)
- Требует голосовых файлов заранее
- Высокие требования к памяти (Face Graph textures)

---

### Mode B: Voice-Driven Built-in
**Когда использовать:** Второстепенные NPC, процедурные диалоги, быстрые итерации

**Pipeline:**
1. **TTS Generation**
   - ElevenLabs API / Azure TTS / Local Piper
   - Runtime генерация речи из текста

2. **Real-time Lip Sync**
   - **Slate-based:** OVRLipSync / LipSync Pro
   - **Phoneme-based:** Runtime phoneme extraction → Blendshapes

3. **Emotion System**
   - Теги в тексте: `[angry]`, `[sad]`, `[whisper]`
   - Preset curves для бровей, глаз, рта
   - Smooth transitions между состояниями

4. **Fallback визуал**
   - Static portrait + анимированный рот
   - Или упрощённый MetaHuman без tessellation

**Преимущества:**
- Мгновенные изменения текста
- Не требует записи голоса
- Меньшее потребление памяти

**Недостатки:**
- Менее естественная анимация
- Ограниченная эмоциональная глубина

---

## Структура диалога

```yaml
DialogueEntry:
  id: "dlg_01_03_keira_intro"
  speaker: "keira"
  mode: "cinematic"  # или "voice_driven"
  
  # Текст с тегами
  text: "Ты проспал дежурство. Снова."
  localized:
    en: "You overslept your shift. Again."
    ru: "Ты проспал дежурство. Снова."
  
  # Аудио (для Mode A)
  audio_path: "/Audio/Dialogues/Chapter01/keira_01_03.wav"
  
  # Face Animation (для Mode A)
  face_clip: "/Animation/Face/keira_01_03_face.fbx"
  
  # Emotion preset (для Mode B)
  emotion: "annoyed"
  intensity: 0.7
  
  # Выборы игрока
  choices:
    - text: "Отпуск закончился три дня назад..."
      next_id: "dlg_01_03_keira_response_1"
      condition: ""
      flags_set: ["keira_sarcastic"]
    - text: "Прости, я..."
      next_id: "dlg_01_03_keira_response_2"
      flags_set: ["keira_apologetic"]
  
  # Автопереход если нет выборов
  next_id: "dlg_01_03_keira_continue"
  
  # События
  events:
    on_start: "fade_in_dialogue"
    on_end: "trigger_quest_update"
  
  # Камера
  camera: 
    type: "close_up"
    target: "speaker"
    duration: 4.5
```

---

## Технические требования

### MetaHuman Integration
- **UE 5.7+** (требуется Audio Driven Animation)
- **MetaHuman Plugin** включён
- **Face Graph** настроен (52 blendshapes)
- **Control Rig** для микро-выражений

### Audio Pipeline
```
Input Audio (48kHz, mono)
    ↓
[Audio Preprocessor] — нормализация, шумоподавление
    ↓
[Phoneme Extractor] — OpenAI Whisper / Rhubarb
    ↓
[Emotion Analyzer] — тональность, темп
    ↓
[Face Generator] — Blendshape weights per frame
    ↓
[MetaHuman Face Graph] — Real-time application
```

### Performance Budget
| Режим | CPU | GPU | Memory |
|-------|-----|-----|--------|
| Mode A (Cinematic) | 2ms/frame | 5ms/frame | 150MB per character |
| Mode B (Voice) | 0.5ms/frame | 1ms/frame | 50MB per character |

---

## Roadmap

### Phase 1: Foundation (Current)
- [ ] Система хранения диалогов (Data Tables)
- [ ] Базовый Dialogue Manager
- [ ] UI виджет для выборов

### Phase 2: Mode B Implementation
- [ ] TTS интеграция (ElevenLabs)
- [ ] Runtime lip sync
- [ ] Emotion presets

### Phase 3: Mode A Implementation
- [ ] MetaHuman Face Graph pipeline
- [ ] Audio2Face workflow
- [ ] Level Sequence integration

### Phase 4: Polish
- [ ] Multi-language support
- [ ] Subtitle system
- [ ] Accessibility (screen reader support)

---

## Связанные документы

- [[FireFace_Tasks]] — Задачи и чеклисты
- [[FireFace_Best_Practices]] — Рекомендации по диалогам
- [[MetaHuman_Dialogue_System_UE57]] — Технические детали MetaHuman
