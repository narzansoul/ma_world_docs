// Copyright Magic Awakening. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueTypes.h"
#include "DialogueManager.generated.h"

class UDialogueDataAsset;
class UMetaHumanFaceController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, const FString&, DialogueID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueNodeShown, const FString&, NodeID, const FDialogueNode&, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionSelected, int32, OptionIndex);

/**
 * Компонент-менеджер диалогов
 * Управляет логикой диалогов, состоянием и взаимодействием с MetaHuman
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAGICA_API UDialogueManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UDialogueManager();

    //~=============================================================================
    // Основные функции
    //~=============================================================================

    /** Начать диалог с указанным Dialogue Data Asset */
    UFUNCTION(BlueprintCallable, Category="Dialogue")
    void StartDialogue(UDialogueDataAsset* DialogueAsset);

    /** Закончить текущий диалог */
    UFUNCTION(BlueprintCallable, Category="Dialogue")
    void EndDialogue();

    /** Выбрать опцию по индексу */
    UFUNCTION(BlueprintCallable, Category="Dialogue")
    void SelectOption(int32 OptionIndex);

    /** Перейти к следующей ноде */
    UFUNCTION(BlueprintCallable, Category="Dialogue")
    void AdvanceToNode(const FString& NodeID);

    /** Пропустить текущую реплику */
    UFUNCTION(BlueprintCallable, Category="Dialogue")
    void SkipCurrentLine();

    //~=============================================================================
    // Геттеры
    //~=============================================================================

    UFUNCTION(BlueprintPure, Category="Dialogue")
    bool IsDialogueActive() const { return CurrentState != EDialogueState::Inactive; }

    UFUNCTION(BlueprintPure, Category="Dialogue")
    EDialogueState GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category="Dialogue")
    FString GetCurrentNodeID() const { return CurrentNodeID; }

    UFUNCTION(BlueprintPure, Category="Dialogue")
    FDialogueNode GetCurrentNode() const;

    UFUNCTION(BlueprintPure, Category="Dialogue")
    TArray<FDialogueOption> GetAvailableOptions() const;

    //~=============================================================================
    // События
    //~=============================================================================

    /** Диалог начался */
    UPROPERTY(BlueprintAssignable, Category="Dialogue|Events")
    FOnDialogueStarted OnDialogueStarted;

    /** Показана новая нода (текст, анимации) */
    UPROPERTY(BlueprintAssignable, Category="Dialogue|Events")
    FOnDialogueNodeShown OnDialogueNodeShown;

    /** Диалог завершён */
    UPROPERTY(BlueprintAssignable, Category="Dialogue|Events")
    FOnDialogueEnded OnDialogueEnded;

    /** Игрок выбрал опцию */
    UPROPERTY(BlueprintAssignable, Category="Dialogue|Events")
    FOnOptionSelected OnOptionSelected;

protected:
    //~=============================================================================
    // Lifecycle
    //~=============================================================================

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, 
                               FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    //~=============================================================================
    // Логика диалога
    //~=============================================================================

    /** Показать ноду */
    UFUNCTION()
    void ShowNode(const FString& NodeID);

    /** Обработать MetaHuman анимации для ноды */
    void ProcessMetaHumanAnimations(const FDialogueNode& Node);

    /** Применить эффекты от выбора опции */
    void ApplyEffects(const TArray<FString>& EffectStrings);

    /** Проверить условие доступности опции */
    bool CheckCondition(const FString& ConditionString) const;

    /** Парсить строку эффекта в структуру */
    FDialogueEffect ParseEffect(const FString& EffectString) const;

    /** Обработать таймаут */
    UFUNCTION()
    void HandleTimeout();

    /** Callback окончания голосовой линии */
    UFUNCTION()
    void OnVoiceLineFinished();

    //~=============================================================================
    // MetaHuman интеграция
    //~=============================================================================

    /** Получить контроллер лица для спикера */
    UMetaHumanFaceController* GetFaceController(const FString& CharacterID) const;

    /** Настроить камеру для диалога */
    void SetupCamera(const FName& CameraShotType);

    /** Восстановить камеру после диалога */
    void RestoreCamera();

    //~=============================================================================
    // Переменные
    //~=============================================================================

    /** Текущий диалог (Data Asset) */
    UPROPERTY()
    TObjectPtr<UDialogueDataAsset> CurrentDialogue;

    /** Текущая нода */
    UPROPERTY()
    FString CurrentNodeID;

    /** Состояние диалога */
    UPROPERTY()
    EDialogueState CurrentState = EDialogueState::Inactive;

    /** Таймер для авто-перехода/таймаута */
    UPROPERTY()
    float CurrentTimer = 0.0f;

    /** Флаг ожидания голосовой линии */
    UPROPERTY()
    bool bWaitingForVoice = false;

    /** Локальные переменные диалога */
    UPROPERTY()
    TMap<FString, FString> LocalVariables;

    /** Таймер handle для таймаутов */
    FTimerHandle TimeoutTimerHandle;

    /** Кэш камеры до начала диалога */
    UPROPERTY()
    TObjectPtr<AActor> CachedCamera;
};
