// Fill out your copyright notice in the Description page of Project Settings.

#include "GIPlanetaryPrivateers.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GSPlanetaryPrivateers.h"
// ===================================================
UGIPlanetaryPrivateers::UGIPlanetaryPrivateers(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

// ===================================================
AGSPlanetaryPrivateers* UGIPlanetaryPrivateers::GetGameSession()
{
	UWorld* world = GetWorld();

	if (world)
	{
		AGameModeBase* gameMode = world->GetAuthGameMode();

		if (gameMode)
		{
			return Cast<AGSPlanetaryPrivateers>(gameMode->GameSession);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::GetGameSession --> Could not get the GameMode")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::GetGameSession --> Could not get the world")));
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::GetGameSession --> Failed to retrieve the GameSession, returning nullptr")));
	return nullptr;
}

// ===================================================
void UGIPlanetaryPrivateers::StartGame(bool bIsLan, int MaxNumPlayers)
{
	//Get the local player
	ULocalPlayer* const localPlayer = GetFirstGamePlayer();

	if (GetGameSession() && localPlayer)
	{
		//Host a session using this player
		if(GetGameSession()->HostSession(localPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, bIsLan, true, MaxNumPlayers))
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::StartGame --> Succesfully Hosted a session!")));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::StartGame --> Could not host a session!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::StartGame --> GetGameSession returns nullptr and/or localplayer is nullptr")));
	}
}

// ===================================================
void UGIPlanetaryPrivateers::FindGames(bool bIsLan)
{
	//Get the local player
	ULocalPlayer* const localPlayer = GetFirstGamePlayer();

	if (GetGameSession() && localPlayer)
	{
		//Find a session
		GetGameSession()->FindSessions(localPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), bIsLan, true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::FindGames --> GetGameSession returns nullptr and/or localplayer is nullptr")));
	}
}

// ===================================================
void UGIPlanetaryPrivateers::JoinGame()
{
	//Get the local player
	ULocalPlayer* const localPlayer = GetFirstGamePlayer();

	if (GetGameSession() && localPlayer)
	{
		//Set the correct player controller
		GetGameSession()->m_InvokingPlayerController = GetFirstLocalPlayerController();

		//Tempory store the found search result for code convenience
		TArray<FOnlineSessionSearchResult> tempFoundSessions = GetGameSession()->m_SessionSearch->SearchResults;

		//Variable to store the searchResult
		FOnlineSessionSearchResult searchResult;

		if (tempFoundSessions.Num() > 0)
		{
			for (int32 i = 0; i < tempFoundSessions.Num(); i++)
			{
				//Filter out our own sessions
				if (tempFoundSessions[i].Session.OwningUserId != localPlayer->GetPreferredUniqueNetId().GetUniqueNetId())
				{
					searchResult = GetGameSession()->m_SessionSearch->SearchResults[i];

					//Join the found session
					if(GetGameSession()->JoinSession(localPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, searchResult))
					{
						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::JoinGame --> Succesfully joined a session!")));
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::JoinGame --> Could not join the session!")));
					}
					break;
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::JoinGame --> Reached the session we are hosting, so skip it")));
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::JoinGame --> tempFoundSessions has no stored values")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GIPlanetaryPrivateers.cpp::JoinGame --> GetGameSession returns nullptr and/or localplayer is nullptr")));
	}
}

// ===================================================
void UGIPlanetaryPrivateers::LeaveGame()
{
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if(onlineSub)
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			sessions->AddOnDestroySessionCompleteDelegate_Handle(GetGameSession()->m_OnDestroySessionCompleteDelegate);
			sessions->DestroySession(GameSessionName);
		}
	}
}