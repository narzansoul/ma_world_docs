// Copyright Magic Awakening. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueTypes.generated.h"

// Forward declarations
USTRUCT(BlueprintType)
struct FDialogueOption;

USTRUCT(BlueprintType)
struct FMetaHumanExpression;

USTRUCT(BlueprintType)
struct FDialogueTimeout;

/**
 * Структура ноды диалога
 */
USTRUCT(BlueprintType)
struct MAGICA_API FDialogueNode
{
    GENERATED_BODY()

    /** ID спикера (character_id) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Speaker;

    /** Текст реплики */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true))
    FText Text;

    /** MetaHuman настройки */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMetaHumanExpression MetaHumanSettings;

    /** Опции выбора */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueOption> Options;

    /** Таймаут (если есть) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDialogueTimeout Timeout;

    /** Авто-переход после окончания голосовой линии */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAutoAdvance = false;

    /** Следующая нода по умолчанию (если нет опций или auto_advance) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString NextNode;
};

/**
 * Опция диалога (выбор игрока)
 */
USTRUCT(BlueprintType)
struct MAGICA_API FDialogueOption
{
    GENERATED_BODY()

    /** Текст опции */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Text;

    /** ID следующей ноды или "END_DIALOGUE" */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString NextNode;

    /** Условие доступности (строка для парсинга, например "has_flag:met_elara") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Condition;

    /** Эффекты при выборе */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Effects;

    /** Скрытая опция (не показывается, используется для default/timout) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHidden = false;
};

/**
 * MetaHuman выражения и анимации
 */
USTRUCT(BlueprintType)
struct MAGICA_API FMetaHumanExpression
{
    GENERATED_BODY()

    /** Эмоция (facial expression preset) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName FacialExpression = TEXT("neutral");

    /** Жест (Animation Montage ID) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Gesture;

    /** Цель взгляда */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName LookAtTarget;

    /** ID голосовой линии */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName VoiceLine;

    /** Тип камерного плана */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CameraShot;

    /** Blend shapes для тонкой настройки */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> BlendShapes;
};

/**
 * Таймаут для ноды
 */
USTRUCT(BlueprintType)
struct MAGICA_API FDialogueTimeout
{
    GENERATED_BODY()

    /** Длительность в секундах */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f;

    /** Индекс опции по умолчанию при таймауте */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DefaultOption = 0;

    /** Анимация при таймауте */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName OnTimeoutAnimation;
};

/**
 * Типы эффектов диалога
 */
UENUM(BlueprintType)
enum class EDialogueEffectType : uint8
{
    None            UMETA(DisplayName="None"),
    SetFlag         UMETA(DisplayName="Set Flag"),
    ClearFlag       UMETA(DisplayName="Clear Flag"),
    ChangeStat      UMETA(DisplayName="Change Stat"),
    ChangeDisposition UMETA(DisplayName="Change Disposition"),
    StartQuest      UMETA(DisplayName="Start Quest"),
    CompleteQuest   UMETA(DisplayName="Complete Quest"),
    GiveItem        UMETA(DisplayName="Give Item"),
    RemoveItem      UMETA(DisplayName="Remove Item"),
    PlaySound       UMETA(DisplayName="Play Sound"),
    TriggerEvent    UMETA(DisplayName="Trigger Event"),
    Custom          UMETA(DisplayName="Custom")
};

/**
 * Структура эффекта диалога (распарсенная)
 */
USTRUCT(BlueprintType)
struct MAGICA_API FDialogueEffect
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueEffectType Type = EDialogueEffectType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Target;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 IntValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValue = 0.0f;
};

/**
 * Состояние диалога
 */
UENUM(BlueprintType)
enum class EDialogueState : uint8
{
    Inactive        UMETA(DisplayName="Inactive"),
    Starting        UMETA(DisplayName="Starting"),
    ShowingText     UMETA(DisplayName="Showing Text"),
    WaitingForInput UMETA(DisplayName="Waiting For Input"),
    Transitioning   UMETA(DisplayName="Transitioning"),
    Ending          UMETA(DisplayName="Ending")
};

/**
 * Результат проверки условия
 */
USTRUCT(BlueprintType)
struct MAGICA_API FConditionResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bPassed = false;

    UPROPERTY(BlueprintReadOnly)
    FString Reason;

    static FConditionResult Passed() 
    { 
        FConditionResult R; 
        R.bPassed = true; 
        return R; 
    }

    static FConditionResult Failed(const FString& InReason) 
    { 
        FConditionResult R; 
        R.bPassed = false; 
        R.Reason = InReason; 
        return R; 
    }
};
