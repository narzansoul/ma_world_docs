// Copyright Magic Awakening. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MetaHumanFaceController.generated.h"

class USkeletalMeshComponent;
class UAudioComponent;

/**
 * Компонент управления лицевой мимикой и анимацией MetaHuman
 */
UCLASS(ClassGroup=(MetaHuman), meta=(BlueprintSpawnableComponent))
class MAGICA_API UMetaHumanFaceController : public UActorComponent
{
    GENERATED_BODY()

public:
    UMetaHumanFaceController();

    //~=============================================================================
    // Инициализация
    //~=============================================================================

    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void Initialize(USkeletalMeshComponent* InFaceMesh);

    //~=============================================================================
    // Эмоции и выражения
    //~=============================================================================

    /** Установить эмоцию (preset) */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetFacialExpression(FName ExpressionName, float BlendTime = 0.3f);

    /** Установить blend shape значение */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetBlendShape(FName BlendShapeName, float Value, float BlendTime = 0.0f);

    /** Установить несколько blend shapes */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetMultipleBlendShapes(const TMap<FName, float>& BlendShapes, float BlendTime = 0.3f);

    /** Сбросить все blend shapes к нейтральному */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void ResetToNeutral(float BlendTime = 0.5f);

    //~=============================================================================
    // Жесты и анимация
    //~=============================================================================

    /** Воспроизвести жест (Animation Montage) */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void PlayGesture(FName GestureID);

    /** Воспроизвести анимацию по имени */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void PlayAnimationByName(FName AnimationName);

    //~=============================================================================
    // Lip Sync
    //~=============================================================================

    /** Начать lip sync для голосовой линии */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void StartLipSync(FName VoiceLineID);

    /** Остановить lip sync */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void StopLipSync();

    /** Установить интенсивность lip sync */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetLipSyncIntensity(float Intensity);

    //~=============================================================================
    // Взгляд (Eye Tracking)
    //~=============================================================================

    /** Установить цель взгляда */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetLookAtTarget(AActor* TargetActor);

    /** Установить точку взгляда */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetLookAtLocation(const FVector& Location);

    /** Отключить отслеживание взгляда */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void ClearLookAtTarget();

    /** Установить интенсивность отслеживания (0-1) */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetLookAtIntensity(float Intensity);

    /** Включить/выключить моргание */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void SetBlinkingEnabled(bool bEnabled);

    //~=============================================================================
    // Пресеты эмоций (словарь)
    //~=============================================================================

    /** Добавить или обновить пресет эмоции */
    UFUNCTION(BlueprintCallable, Category="MetaHuman Face")
    void AddEmotionPreset(FName PresetName, const TMap<FName, float>& BlendShapeValues);

    /** Получить пресет эмоции */
    UFUNCTION(BlueprintPure, Category="MetaHuman Face")
    TMap<FName, float> GetEmotionPreset(FName PresetName) const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    /** Применить blend shapes к мешу */
    void ApplyBlendShapes(const TMap<FName, float>& Values);

    /** Интерполировать blend shapes */
    void InterpolateBlendShapes(float DeltaTime);

    /** Обновить взгляд */
    void UpdateEyeTracking(float DeltaTime);

    /** Обновить lip sync */
    void UpdateLipSync(float DeltaTime);

protected:
    /** Face mesh (skinned mesh component) */
    UPROPERTY()
    TObjectPtr<USkeletalMeshComponent> FaceMesh;

    /** Текущие значения blend shapes */
    UPROPERTY()
    TMap<FName, float> CurrentBlendShapes;

    /** Целевые значения blend shapes (для интерполяции) */
    UPROPERTY()
    TMap<FName, float> TargetBlendShapes;

    /** Скорость интерполяции */
    UPROPERTY(EditAnywhere, Category="MetaHuman Face")
    float BlendInterpolationSpeed = 5.0f;

    /** Пресеты эмоций */
    UPROPERTY(EditAnywhere, Category="MetaHuman Face")
    TMap<FName, TMap<FName, float>> EmotionPresets;

    /** Цель взгляда */
    UPROPERTY()
    TWeakObjectPtr<AActor> LookAtTarget;

    /** Точка взгляда */
    UPROPERTY()
    FVector LookAtLocation;

    /** Интенсивность взгляда */
    UPROPERTY()
    float LookAtIntensity = 1.0f;

    /** Текущая интенсивность lip sync */
    UPROPERTY()
    float CurrentLipSyncIntensity = 0.0f;

    /** Флаг активного lip sync */
    UPROPERTY()
    bool bIsLipSyncActive = false;

    /** Таймер моргания */
    UPROPERTY()
    float BlinkTimer = 0.0f;

    /** Интервал моргания */
    UPROPERTY(EditAnywhere, Category="MetaHuman Face")
    float BlinkInterval = 4.0f;

    /** Длительность моргания */
    UPROPERTY(EditAnywhere, Category="MetaHuman Face")
    float BlinkDuration = 0.15f;

    /** Флаг процесса моргания */
    UPROPERTY()
    bool bIsBlinking = false;
};
