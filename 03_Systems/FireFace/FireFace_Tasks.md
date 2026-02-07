# FireFace — Tasks & Checklist

> **Проект:** Magic Awakening: The Aron Heir  
> **Система:** FireFace Dialogue System  
> **Обновлено:** 2026-02-07

---

## 🔴 Phase 1: Foundation (Приоритет: Критичный)

### Core Architecture
- [ ] **Создать Dialogue Data Asset** (UDataAsset)
  - Структура: FDialogueEntry
  - Поля: ID, Speaker, Text, Choices, Events
  - Estimated: 4 часа

- [ ] **Dialogue Manager (C++)**
  - Singleton pattern
  - State machine: Idle → Playing → WaitingChoice → End
  - Event dispatcher (Quest triggers, Flag system)
  - Estimated: 8 часов

- [ ] **Integration with Game Mode**
  - Пауза игрового времени во время диалога (опционально)
  - Input mode: UI Only
  - Estimated: 2 часа

### UI Foundation
- [ ] **Dialogue Widget (UMG)**
  - Rich Text Block для текста
  - Scroll View для длинных диалогов
  - Typewriter effect с настройкой скорости
  - Estimated: 6 часов

- [ ] **Choice Widget**
  - Vertical Box с кнопками
  - Keyboard navigation (1-9)
  - Hover/Focus effects
  - Estimated: 4 часа

- [ ] **Speaker Portrait System**
  - Dynamic Material Instance для освещения
  - Fade in/out transitions
  - Estimated: 4 часа

**Phase 1 Total: ~28 часов**

---

## 🟡 Phase 2: Voice-Driven Mode (Mode B)

### TTS Integration
- [ ] **ElevenLabs API Setup**
  - HTTP Request module
  - API key management (секреты)
  - Voice ID mapping per character
  - Caching загруженных аудио
  - Estimated: 6 часов

- [ ] **Local TTS Fallback (Piper)**
  - Offline голоса для русского
  - Интеграция через процесс
  - Estimated: 4 часа

- [ ] **Audio Streaming**
  - USoundWaveProcedural или файловый кэш
  - Preload next line
  - Estimated: 4 часа

### Lip Sync System
- [ ] **Phoneme Extractor**
  - Rhubarb Lip Sync интеграция
  - Runtime phoneme analysis
  - Or: OVRLipSync plugin
  - Estimated: 8 часов

- [ ] **Blendshape Driver**
  - Mapping phonemes → MetaHuman blendshapes
  - Smooth interpolation
  - Viseme curves (JSON конфиг)
  - Estimated: 6 часов

### Emotion System (Mode B)
- [ ] **Emotion Presets**
  - Создать 8 базовых эмоций:
    - neutral, happy, sad, angry, surprised, disgusted, fearful, contempt
  - Blendshape weights per emotion
  - Estimated: 4 часа

- [ ] **Emotion Parser**
  - Парсинг тегов из текста: `[angry]` `[sad]`
  - Smooth transition между эмоциями
  - Layering: базовая + временная
  - Estimated: 4 часа

**Phase 2 Total: ~36 часов**

---

## 🟢 Phase 3: Cinematic Mode (Mode A)

### MetaHuman Pipeline
- [ ] **Face Graph Setup**
  - Проверка 52 blendshapes
  - Control Rig для микро-выражений
  - Material instances
  - Estimated: 4 часа

- [ ] **Audio2Face Workflow**
  - NVIDIA Omniverse Connector
  - Export settings (FBX, 60fps)
  - Import pipeline в UE5
  - Batch processing
  - Estimated: 8 часов

- [ ] **Live Link Face Alternative**
  - iOS app setup
  - Calibration profile
  - Real-time capture test
  - Estimated: 4 часа

### Animation Integration
- [ ] **Level Sequence Manager**
  - Camera cuts per dialogue line
  - Character positioning
  - Lighting setup
  - Estimated: 6 часов

- [ ] **Body Animation**
  - Motion Matching для idles
  - Gesture triggers (keyword-based)
  - Look At controller
  - Estimated: 8 часов

- [ ] **Cinematic Integration**
  - Master Sequence для главы
  - Dialogue triggers из Sequence
  - Skip/pause functionality
  - Estimated: 6 часов

**Phase 3 Total: ~36 часов**

---

## 🔵 Phase 4: Polish & Advanced Features

### Localization
- [ ] **Localization System**
  - CSV export/import
  - FText integration
  - Voiceover switching
  - Estimated: 6 часов

- [ ] **Subtitle System**
  - SRT parser
  - Styling (size, color, background)
  - Closed captions mode
  - Estimated: 4 часа

### Accessibility
- [ ] **Accessibility Features**
  - High contrast mode
  - Large text mode
  - Auto-advance option
  - Estimated: 4 часа

### Optimization
- [ ] **Memory Optimization**
  - Texture streaming для лиц
  - Audio compression (Ogg Vorbis)
  - LOD для distant NPCs
  - Estimated: 4 часа

- [ ] **Performance Profiling**
  - Unreal Insights
  - Stat commands
  - Budget validation
  - Estimated: 4 часа

**Phase 4 Total: ~22 часов**

---

## 📊 Summary

| Phase | Hours | Status |
|-------|-------|--------|
| Phase 1: Foundation | 28 | ⏳ Not Started |
| Phase 2: Voice-Driven | 36 | ⏳ Not Started |
| Phase 3: Cinematic | 36 | ⏳ Not Started |
| Phase 4: Polish | 22 | ⏳ Not Started |
| **TOTAL** | **122** | ⏳ |

---

## 🎯 Immediate Next Steps

1. **Сегодня:**
   - [ ] Создать FDialogueEntry struct
   - [ ] Базовый DialogueManager класс

2. **На этой неделе:**
   - [ ] UMG Widget для диалогов
   - [ ] Тестовая сцена с 3 репликами

3. **Следующий спринт:**
   - [ ] ElevenLabs интеграция
   - [ ] Lip sync prototype

---

## 📝 Notes

- Использовать **Enhanced Input** для навигации
- Сохранять dialogue state в **SaveGame**
- Поддержка **gamepad** с самого начала
- Планировать **DLSS/FSR** совместимость (camera effects)
