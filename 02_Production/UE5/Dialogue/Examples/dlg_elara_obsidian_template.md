---
dialogue_id: dlg_elara_market_meeting
version: "1.0"
participants: [player, elara]
location: ark_market
time_of_day: morning
---

# Встреча с Эларой на рынке

## node_greeting

**@elara** *(curious, gesture_greeting_open, vo_elara_greeting_01)*
> Приветствую, путник. Я вижу по твоим глазам — ты не из этих мест. Что привело тебя в Арк Созидания?

- [Я ищу ответы о своём происхождении. Говорят, здесь хранятся древние знания.](#node_origin_truth) → set_flag:elara_seeks_knowledge, change_disposition:elara:+5
- [Это не твоё дело. Просто ответь на мои вопросы.](#node_rude_response) → change_disposition:elara:-15, set_flag:elara_dislikes_player
- [[Требуется: Руна Восприятия]] [Что ты скрываешь? Я вижу тревогу в твоих глазах.](#node_perception_reveal) → unlock_knowledge:elara_secret, change_disposition:elara:+10
- [[met_elara]] [Рада снова тебя видеть, Элара.](#node_reunion)

## node_origin_truth

**@elara** *(warm, gesture_contemplate, vo_elara_reflective_01, close_up)*
> *мягко улыбается* Ответы... Все мы их ищем. Но знания в Арке имеют цену. Иногда слишком высокую.

- [Какую цену? Я готов заплатить.](#node_price_discussion)
- [Ты предупреждаешь или угрожаешь?](#node_defensive) → change_disposition:elara:-5

## node_rude_response

**@elara** *(cold, gesture_dismiss, vo_elara_cold_01, profile)*
> *холодно* Как скажешь, чужак. Но помни — в Арке никто не обязан тебе ничего. *отворачивается*

- [Подожди! Я... я извиняюсь. Просто я в отчаянии.](#node_apology_accept) → change_disposition:elara:+5
- [Уйти](END_DIALOGUE) → set_flag:elara_ignored

## node_perception_reveal

**@elara** *(shocked, gesture_shield_face, vo_elara_whisper_01, close_up)*
> *вздрогнув* Ты... ты видишь слишком много. *тихо* Приходи ночью, когда рынок пуст. Не здесь. Не сейчас.

- [Чего ты боишься?](#node_fear_reveal)
- [Хорошо. Где встретимся?](#node_meeting_arrange) → unlock_quest:elara_secret_meeting, set_flag:elara_trusts_player

## node_reunion

**@elara** *(happy, gesture_welcome, vo_elara_happy_01)*
> *радостно* Ты вернулся! Я надеялась... то есть, я рада, что ты в порядке.

- [У меня есть новости о том, что ты просила узнать.](#node_quest_progress)
- [Просто хотел поблагодарить за совет.](END_DIALOGUE) → change_disposition:elara:+10

## node_apology_accept

**@elara** *(sympathetic, gesture_invite, vo_elara_soften_01, medium_shot)*
> *оборачивается* Отчаяние... Да, оно знакомо мне. *мягче* Хорошо. Я выслушаю тебя.

- [Продолжить...](#node_origin_truth)

## node_defensive

**@elara** *(tired, gesture_shrug, vo_elara_tired_01)*
> *вздыхает* Ни то, ни другое. Просто констатирую факты. Продолжать разговор?

- [Да, прошу прощения за резкость.](#node_origin_truth)
- [Нет, я найду ответы сам.](END_DIALOGUE) → set_flag:refused_elara_help

## node_fear_reveal

**@elara** *(fearful, gesture_nervous_glance, vo_elara_fear_01)*
> *шёпотом* Кулькт. Они всюду. У них есть... глаза в стенах.

- [Продолжить...](#node_meeting_arrange)

## node_meeting_arrange

**@elara** *(neutral, gesture_farewell_quick)*
> У Старого Колодца, после заката. Приходи один.

- [Я буду там.](END_DIALOGUE)

## node_price_discussion

**@elara** *(intrigued, gesture_weigh_options, vo_elara_serious_01, two_shot)*
> Цена... *пауза* Некоторые знания меняют тебя навсегда. Ты готов рискнуть своей человечностью?

*timeout: 8s, default: 0*

- [Я готов на всё, чтобы узнать правду.](#node_serious_commit) → change_stat:wisdom:+1, change_stat:humanity:-5
- [Что ты имеешь в виду под 'человечностью'?](#node_clarification)
- *[Таймаут] *замолчать*](#node_silence) *hidden

## node_serious_commit

**@elara** *(serious, gesture_lean_in, vo_elara_urgent_01, close_up)*
> *серьёзно* Тогда слушай внимательно. В Глубинах Арка есть Зеркало Истин. Но охраняет его... нечто древнее.

*auto_advance: true* → node_quest_offer

## node_quest_offer

**@elara** *(neutral, gesture_farewell, vo_elara_quest_01)*
> Если ты действительно хочешь узнать правду — найди меня после заката. У Старого Колодца. И приготовь руну защиты.

- [Я буду там.](END_DIALOGUE) → start_quest:mirror_of_truth, set_flag:elara_meeting_scheduled

## node_clarification

**@elara** *(sad, gesture_hand_on_heart)*
> Я имею в виду, что знание нельзя отнять. Оно останется с тобой, даже когда станет бременем.

- [Продолжить...](#node_price_discussion)

## node_silence

**@elara** *(disappointed, gesture_turn_away)*
> *недовольно* Не люблю, когда меня заставляют ждать. Ты ещё не готов к ответам.

- [Уйти](END_DIALOGUE) → change_disposition:elara:-5, set_flag:elara_impatient

## node_quest_progress

**@elara** *(intrigued, gesture_lean_in, close_up)*
> *зажигается интересом* Говори! Что ты узнал?

- [Закончить разговор](END_DIALOGUE)

---

## Примечания для писателя

### Синтаксис Obsidian для диалогов:

1. **Фронтматтер** — метаданные между `---`
2. **Ноды** — заголовки `## node_id`
3. **Спикер** — `**@character_id**`
4. **MetaHuman** — выражения в `*(...)*` после спикера:
   - Эмоции: `curious`, `happy`, `sad`, `angry`, etc.
   - Жесты: `gesture_*`
   - Голос: `vo_*`
   - Камера: `close_up`, `medium_shot`, etc.
5. **Текст** — строки начинающиеся с `>`
6. **Опции** — `- [Текст](#node_id)`
7. **Условия** — `[[condition]]` перед опцией
8. **Эффекты** — `→ effect1, effect2` после опции
9. **Таймаут** — `*timeout: Xs, default: N*` в ноде
10. **Auto-advance** — `*auto-advance: true*` или `→ node_id`
