# MetaHumans в Unreal Engine 5.7: Диалоговая Система

_Практическое руководство по созданию диалогов с реалистичной мимикой_

---

## Что такое MetaHumans

MetaHumans — это высокодетализированные цифровые люди от Epic Games. В UE 5.7 они стали ещё мощнее:

- **Face Graph** — система контроля мимики через 52+ фейшл-блендшейпа
- **MetaHuman Animator** — создание анимации лица из видео или аудио
- **Audio Driven Animation** — автоматическая мимика под голос
- **Nanite + Lumen** — кинематографическое качество в реальном времени

---

## Установка и Настройка

### 1. Плагины (обязательно)

В `Edit → Plugins` включите:
```
☑ MetaHuman Plugin
☑ MetaHuman Animator
☑ MetaHuman SDK
☑ Live Link (для анимации в реальном времени)
☑ Apple ARKit (если используете iPhone для face capture)
☑ Serialization
```

Перезагрузите движок после активации.

### 2. Скачивание MetaHuman

**MetaHuman Creator** (в браузере):
1. Идите на [metahuman.unrealengine.com](https://metahuman.unrealengine.com)
2. Создайте персонажа или выберите готового
3. Нажмите **Quixel Bridge** → `Send to Unreal Engine`

**В Quixel Bridge** (внутри UE):
```
Window → Quixel Bridge
→ Выберите MetaHuman
→ Download & Export to Project
→ Сохраните в папку Content/MetaHumans/
```

**Важно для UE 5.7**: Выберите версию skeletal mesh для **UE 5** (не 4.26-4.27).

---

## Структура MetaHuman

### Основные компоненты

```
BP_MetaHumanCharacter (Blueprint)
├── Mesh (Body)
├── FaceMesh (Head)
├── Face_AnimBP (Face Animation Blueprint)
├── Control Rig (для тела)
└── Face Graph Component
```

### Face Graph — ключ к мимике

**Face Graph** — это материал-основа для управления лицом. Находится:
```
Content/MetaHumans/[Имя]/Face/Face_Graph
```

**52 Blendshapes** контролируют:
- **Jaw** — челюсть (jaw_open, jaw_left, jaw_right)
- **Mouth** — рот (mouth_open, mouth_smile, mouth_frown)
- **Eyes** — глаза (blink_left, blink_right, squint)
- **Brows** — брови (brow_up, brow_down)
- **Cheeks/Nose** — щёки и нос

---

## Создание Диалоговой Системы

### Архитектура системы

```
DialogueSystem
├── DialogueManager (Game Instance или singleton)
├── DialogueWidget (UI)
├── DialogueDataTable (таблица реплик)
├── MetaHumanSpeaker (Actor с MetaHuman)
└── AudioComponent (для воспроизведения голоса)
```

### Шаг 1: Data Table для диалогов

**Создайте структуру** (`Blueprint → Structure`):
```cpp
FDialogueLine
├── SpeakerID (String)          → "NPC_Elara"
├── SpeakerName (String)        → "Элара"
├── DialogueText (Text)         → "Привет, путник."
├── AudioFile (SoftObjectPath)  → SoundCue
├── FacialAnimation (AnimSequence) → Мимика
├── Duration (Float)            → 3.5 сек
├── bAutoAdvance (Bool)         → автопереход
└── NextLineID (String)         → "line_002"
```

**Data Table** (`Content/Tables/DT_Dialogues`):
| Row Name | SpeakerID | DialogueText | AudioFile | FacialAnim |
|----------|-----------|--------------|-----------|------------|
| Line_001 | NPC_Elara | Привет... | SC_Greeting | A_Elara_Smile |
| Line_002 | NPC_Elara | Я ждала тебя. | SC_Waiting | A_Elara_Serious |

### Шаг 2: MetaHuman Speaker Blueprint

**Создайте Blueprint Actor** `BP_MetaHumanSpeaker`:

#### Components:
```
DefaultSceneRoot
├── MetaHumanMesh (Skeletal Mesh Component)
│   └── Mesh = SKM_[MetaHumanName]_Face
├── AudioComponent
└── Arrow (для ориентации камеры)
```

#### Event Graph — ключевые ноды:

**Play Dialogue Line**:
```
Event PlayDialogue (DialogueRow Input)
│
├─ Play Sound at Component (AudioComponent)
│  └─ Sound = Input.AudioFile
│
├─ Play Animation (FaceMesh)
│  └─ Anim = Input.FacialAnimation
│  └─ Looping = false
│
├─ Set Timer by Function Name
│  └─ Function = "OnDialogueEnd"
│  └─ Time = Input.Duration
│
└─ Show Subtitle Widget
```

**Facial Animation Control**:
```
Event SetFacialExpression (ExpressionName String, Weight Float)
│
├─ Get FaceMesh → Get Anim Instance
├─ Cast to Face_AnimBP
└─ Set Blendshape Value
   ├─ Blendshape Name = ExpressionName
   └─ Value = Weight (0.0 - 1.0)
```

### Шаг 3: Face Animation Blueprint

**Откройте** `Face_AnimBP` в вашем MetaHuman:

#### Anim Graph

**Base Layer** → **Additive Layer** → **Output Pose**

**Face Control** (в Event Graph):
```
Event Update Animation
│
├─ Update Blendshape Values
│  ├─ jaw_open = Get Curve Value("JawOpen")
│  ├─ mouth_smile = Get Curve Value("MouthSmile")
│  └─ ... (52 параметра)
│
└─ Apply to Face Graph
```

**Live Link Integration** (если используете iPhone capture):
```
Event Blueprint Update Animation
│
├─ Get Live Link Data
│  └─ Subject Name = "iPhoneFace"
│
├─ Extract Blendshapes
│  └─ jawOpen, mouthClose, eyeBlinkLeft...
│
└─ Set in Face Graph
```

---

## Методы Анимации Лица

### Метод 1: Audio Driven Animation (UE 5.7)

**Автоматическая мимика под голос** — флагманская фича 5.7:

**Настройка**:
```
1. Content/MetaHumanAnimator/ — папка плагина
2. Выберите Audio file (WAV, 44.1kHz, mono)
3. ПКМ → Generate MetaHuman Animation
4. Target = ваш MetaHuman
5. Language = Russian/English/...
6. Generate
```

**Результат**: `AnimSequence` с ключами для всех 52 блендшейпов.

**Использование в Blueprint**:
```
Event PlayVoiceLine (SoundCue Voice)
│
├─ Get MetaHuman Animator
├─ Process Audio (Voice)
│  └─ Return AnimSequence
├─ Play Anim Sequence (FaceMesh)
└─ Play Sound (AudioComponent)
```

### Метод 2: Manual Blendshape Control

**Для точного контроля в диалогах**:

**Create Animation Curve** в Sequencer:
```
Add Track → Animation → Face Graph
├─ 0.0s: jaw_open = 0.0
├─ 0.2s: jaw_open = 0.6  (начало слога)
├─ 0.4s: jaw_open = 0.3  (звук "и")
└─ 0.6s: jaw_open = 0.0  (конец)
```

**Emotion Presets** (создайте Enum):
```cpp
UENUM()
enum class EFacialExpression : uint8
{
    Neutral     UMETA(DisplayName = "Нейтральное"),
    Happy       UMETA(DisplayName = "Радость"),
    Sad         UMETA(DisplayName = "Грусть"),
    Angry       UMETA(DisplayName = "Злость"),
    Surprised   UMETA(DisplayName = "Удивление"),
    Disgusted   UMETA(DisplayName = "Отвращение"),
    Fearful     UMETA(DisplayName = "Страх")
};
```

**Apply Emotion** в Blueprint:
```
Event ApplyEmotion (EFacialExpression Emotion, Float Intensity)
│
├─ Switch on Enum
├─ Happy → Set Curves:
│  ├─ mouth_smile = 0.8 * Intensity
│  ├─ cheek_puff = 0.3 * Intensity
│  └─ eye_blink = 0.1
│
└─ Sad → Set Curves:
   ├─ mouth_frown = 0.7 * Intensity
   ├─ brow_down = 0.5 * Intensity
   └─ eye_squint = 0.2
```

### Метод 3: MetaHuman Animator + Video

**Съёмка своего лица**:
```
1. iPhone с Face ID
2. Приложение Live Link Face (App Store)
3. Подключение по WiFi:
   Settings → Live Link → IP вашего ПК
4. В UE: Window → Live Link → Add Source → iPhone
```

**Запись**:
```
Sequencer → Add Facial Animation Track
→ Record (красная кнопка)
→ Говорите в iPhone
→ Stop
→ Bake to Control Rig
```

---

## Полная Диалоговая Система

### Диалог Manager (Blueprint)

**BP_DialogueManager** (разместите в Level):

```
Variables:
├── CurrentDialogueTable (DataTable)
├── CurrentRow (Name)
├── ActiveSpeaker (BP_MetaHumanSpeaker)
├── PlayerController (PC_Dialogue)
└── DialogueQueue (Array of Names)

Functions:
├── StartDialogue (DataTable, StartRow)
├── AdvanceDialogue ()
├── EndDialogue ()
└── SkipLine ()
```

**Start Dialogue**:
```
Event StartDialogue
│
├─ Set CurrentDialogueTable
├─ Set CurrentRow = StartRow
├─ Get Data Table Row
│  └─ Row Name = CurrentRow
├─ Find Speaker Actor
│  └─ SpeakerID == Row.SpeakerID
├─ Set ActiveSpeaker
├─ Call ActiveSpeaker.PlayDialogue(Row)
└─ Show UI Widget
```

**Advance Dialogue**:
```
Event AdvanceDialogue
│
├─ Get Current Row
├─ If bAutoAdvance = true:
│  ├─ Set CurrentRow = Row.NextLineID
│  ├─ Get New Row
│  └─ PlayDialogue(NewRow)
│
└─ Else:
   └─ Wait for Input
```

### UI Widget (DialogueWidget)

**W_DialogueOverlay**:
```
Canvas Panel
├── DialogueBox (Vertical Box)
│   ├── SpeakerName (Text Block) — "Элара"
│   ├── DialogueText (Text Block) — "Привет..."
│   └── SkipHint (Text) — "[Space] Пропустить"
│
├── Portrait (Image) — аватар говорящего
└── ChoicesPanel (Vertical Box) — для выбора ответов
```

**Typewriter Effect**:
```
Event SetDialogueText (Text FullText)
│
├─ Set DisplayedText = ""
├─ For Each Character in FullText:
│  ├─ Append to DisplayedText
│  ├─ Set DialogueText = DisplayedText
│  └─ Delay 0.03 сек
└─ End
```

### Trigger диалога

**BP_DialogueTrigger** (Box Collision):
```
Event OnBeginOverlap (Player)
│
├─ Get Dialogue Manager
├─ Call StartDialogue
│  ├─ Table = DT_MainQuest_01
│  └─ StartRow = "MQ01_Greeting"
└─ Destroy Actor (или Set DoOnce)
```

---

## Интеграция с Quest System

**Структура квеста**:
```cpp
FQuestData
├── QuestID
├── DialogueStartRow
├── RequiredQuest (зависимость)
└─ Rewards
```

**Условия диалога**:
```
Event CanStartDialogue → Return Bool
│
├─ Get Quest Status
├─ Check Prerequisites
├─ Check Time of Day (если нужно)
└─ Return true/false
```

**Ветвление диалога**:
```
Event ChooseDialogueBranch (Int ChoiceIndex)
│
├─ Switch on ChoiceIndex
├─ 0: → CurrentRow = "Choice_Yes"
├─ 1: → CurrentRow = "Choice_No"
└─ 2: → CurrentRow = "Choice_Maybe"
```

---

## Производительность и Оптимизация

### Level of Detail (LOD)

**MetaHuman LOD System**:
```
Viewport → LOD Distance
├── LOD 0: 0-2m (полная детализация)
├── LOD 1: 2-5m (упрощённые шейдеры)
└── LOD 2: 5m+ (только тело, без Face Graph)
```

**Настройка**:
```
MetaHuman → LODSettings
├── LOD0_Hysteresis = 100cm
├── LOD1_Hysteresis = 200cm
└── Disable Face Graph on LOD 2+ = true
```

### Culling

**Enable Culling**:
```
MetaHumanMesh → Cull Distance Volume
└─ End Cull Distance = 10000 (100m)
```

**Audio Optimization**:
```
AudioComponent
├── Attenuation Settings = Voice_Attenuation
├── Occlusion Refresh Interval = 0.5
└── Override Attenuation = true
```

---

## Практический Пример: NPC Говорит

### Шаг 1: Создать NPC

1. **Content Browser** → ПКМ → Blueprint Class
2. Parent Class = `Character`
3. Name = `BP_NPC_Elara`

### Шаг 2: Добавить MetaHuman Mesh

```
Components:
├── Mesh (Inherited)
│   └── Skeletal Mesh Asset = SKM_Elara_Full
├── FaceMesh (Skeletal Mesh)
│   └── Parent Socket = head
│   └── Mesh = SKM_Elara_Face
└── VoiceAudio (Audio Component)
```

### Шаг 3: Создать анимацию реплики

**Для фразы "Привет, путник"**:
```
1. Откройте MetaHuman Animator
2. Import Audio: "privet_putnik.wav"
3. Generate Animation
4. Save as: A_Elara_Greeting
5. Preview — лицо должно двигаться синхронно
```

### Шаг 4: Blueprint логика

**Event Graph BP_NPC_Elara**:
```
Custom Event GreetPlayer
│
├─ Play Animation (FaceMesh, A_Elara_Greeting)
├─ Play Sound (VoiceAudio, SC_Elara_Greeting)
├─ Set Timer (3.5s) → EndGreeting
└─ Show Subtitle ("Привет, путник.")

Event EndGreeting
├─ Stop Animation
├─ Return to Idle
└─ Enable Player Input
```

### Шаг 5: Триггер в мире

```
Event OnPlayerApproach
├─ IF Distance < 200cm:
│  ├─ Rotate NPC to Player
│  ├─ Call GreetPlayer
│  └─ Show Interaction Prompt
└─ ELSE: Hide Prompt
```

---

## Дополнительные Возможности UE 5.7

### Neural Network Inference (NNI)

**Для более плавной анимации**:
```
Project Settings → Plugins → Neural Network Inference
├── Enable NNI = true
├── Model = FaceAnimation_5.7.onnx
└─ Use GPU = true
```

### Chaos Physics для волос/одежды

**Secondary Animation**:
```
MetaHuman → Groom (волосы)
├── Enable Physics = true
├── Wind Influence = 0.3
└─ Collision = Head, Shoulders
```

### Nanite для Face Mesh

**Включите Nanite** на SKM_Face:
```
Static Mesh → Nanite Settings
├── Enable Nanite = true
├── Position Precision = 0.001
└─ Keep Triangle Percent = 50%
```

---

## Чек-лист Для Тестирования

- [ ] MetaHuman импортирован корректно
- [ ] Face Graph работает (тест в Animation Preview)
- [ ] Аудио синхронизировано с мимикой
- [ ] LOD переключается без видимых скачков
- [ ] Диалог прерывается при отходе игрока
- [ ] UI отображается на всех аспектах
- [ ] Производительность > 60 FPS (5+ NPC)

---

## Полезные Команды Консоли

```
stat metahuman          # Статистика MetaHuman
showflag.metahuman 0    # Отключить отображение
showflag.facegraph 1    # Показать debug face graph
slomo 0.5               # Замедление для проверки синхронизации
```

---

## Ссылки

- [MetaHuman Documentation](https://docs.metahuman.unrealengine.com)
- [MetaHuman Animator Guide](https://docs.unrealengine.com/metahuman-animator)
- [Live Link Face App](https://apps.apple.com/us/live-link-face)
- [Quixel Bridge](https://quixel.com/bridge)

---

_Создано: 2026-02-06_  
_Версия UE: 5.7_  
_Статус: Рабочий шаблон для проекта Magic Awakening_
