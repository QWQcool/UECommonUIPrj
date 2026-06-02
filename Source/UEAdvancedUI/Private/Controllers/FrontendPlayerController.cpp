// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FrontendPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"

void AFrontendPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), FoundCameras);
	if (FoundCameras.Num() > 0)
	{
		// Set the first camera as active
		SetViewTarget(FoundCameras[0]);
	}
}
