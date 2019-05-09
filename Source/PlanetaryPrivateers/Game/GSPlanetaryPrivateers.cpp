// Fill out your copyright notice in the Description page of Project Settings.

#include "GSPlanetaryPrivateers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "OnlineSubsystemSessionSettings.h"

// ===================================================
AGSPlanetaryPrivateers::AGSPlanetaryPrivateers(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/**Bind functions for creating a session*/
	m_OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AGSPlanetaryPrivateers::OnCreateSessionComplete);
	m_OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &AGSPlanetaryPrivateers::OnStartOnlineGameComplete);

	/*Bind functions for Searching a session*/
	m_OnFindSessionCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &AGSPlanetaryPrivateers::OnFindSessionsComplete);

	/*Bind functions for Joining a session*/
	m_OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &AGSPlanetaryPrivateers::OnJoinSessionComplete);

	/*Bind Functions for Destroying a session*/
	m_OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &AGSPlanetaryPrivateers::OnDestroySessionComplete);

}

#pragma region HostSession
// ===================================================
bool AGSPlanetaryPrivateers::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	/*Get the online subsystem*/
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		/*Get the session interface*/
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid() && UserId.IsValid())
		{
			/*Fill in all session settings*/
			m_SessionSettings = MakeShareable(new FOnlineSessionSettings());

			m_SessionSettings->bIsDedicated = false;
			m_SessionSettings->bIsLANMatch = bIsLan;
			m_SessionSettings->bUsesPresence = bIsPresence;
			m_SessionSettings->NumPublicConnections = MaxNumPlayers;
			m_SessionSettings->NumPrivateConnections = 0;
			m_SessionSettings->bAllowInvites = true;
			m_SessionSettings->bAllowJoinInProgress = true;
			m_SessionSettings->bShouldAdvertise = true;
			m_SessionSettings->bAllowJoinViaPresence = true;
			m_SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			/*Set some extra variables that depends on the game*/
			m_SessionSettings->Set(SETTING_MAPNAME, m_LobbyMap.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

			/*Set the delegate to the handle of the session interface*/
			m_OnCreateSessionCompleteDelegateHandle = sessions->AddOnCreateSessionCompleteDelegate_Handle(m_OnCreateSessionCompleteDelegate);

			/*delegate should get called when this is complete*/
			return sessions->CreateSession(*UserId, SessionName, *m_SessionSettings);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::HostSession -->SessionInterface or UserId is Invalid!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::HostSession --> No onlline subsystem is found!")));
	}
	return false;
}

// ===================================================
void AGSPlanetaryPrivateers::OnCreateSessionComplete(FName SessionName, bool bWasSuccesfull)
{
	/*Tell the user the session name and wether it was created succesfull*/
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AGSPlanetaryPrivateers.cpp::OnCreateSessionComplete --> %s, %d"), *SessionName.ToString(), bWasSuccesfull));

	/*Get the online subsystem*/
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		//Get the session interface to start the session
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			/*Clear the SessionComplete delegate handle, since we finished this call*/
			sessions->ClearOnCreateSessionCompleteDelegate_Handle(m_OnCreateSessionCompleteDelegateHandle);

			if (bWasSuccesfull)
			{
				/*Set the start Session delegate handle*/
				m_OnStartSessionCompleteDelegateHandle = sessions->AddOnStartSessionCompleteDelegate_Handle(m_OnStartSessionCompleteDelegate);

				/*Call the StartsessionComplete delegate*/
				sessions->StartSession(SessionName);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnCreateSessionComplete --> The creation of the session: %s, was unsuccesfull!"), *SessionName.ToString()));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnCreateSessionComplete --> SessionInterface is Invalid!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnCreateSessionComplete --> No onlline subsystem is found!")));
	}
}

// ===================================================
void AGSPlanetaryPrivateers::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccesfull)
{
	/*Notify the user that the session was completed and give the name and wether it was succesfull*/
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AGSPlanetaryPrivateers.cpp::OnStartOnlineGameComplete --> OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccesfull));

	//Get the online subsystem
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		//Get the session interface
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			sessions->ClearOnStartSessionCompleteDelegate_Handle(m_OnStartSessionCompleteDelegateHandle);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnStartOnlineGameComplete --> SessionInterface is invalid!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnStartOnlineGameComplete --> No onlline subsystem is found!")));
	}

	//If the start was succesfull, we can open a newmap if we want, Make sure to pass llisten as a parameter (see ref in blueprint prototype)
	if (bWasSuccesfull)
	{
		UGameplayStatics::OpenLevel(GetWorld(), m_LobbyMap, true, "listen");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnStartOnlineGameComplete --> The start of the online session: %s, was unsuccesfull")));
	}
}
#pragma endregion 

#pragma region SearchSession
// ===================================================
void AGSPlanetaryPrivateers::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence)
{
	/**Get the online subsystem to work with*/
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		/**Get the session interface*/
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid() && UserId.IsValid())
		{
			/**Set up the searchsettings*/
			m_SessionSearch = MakeShareable(new FOnlineSessionSearch());

			m_SessionSearch->bIsLanQuery = bIsLan;
			m_SessionSearch->MaxSearchResults = m_MaxSearchSessions;
			m_SessionSearch->PingBucketSize = m_PingBucketSize;

			/**If we use presence set the presence query settings*/
			if (bIsPresence)
			{
				m_SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> searchSettingsRef = m_SessionSearch.ToSharedRef();

			/**Set the delegate to the handle of the search session function*/
			m_OnFindSessionsCompleteDelegateHandle = sessions->AddOnFindSessionsCompleteDelegate_Handle(m_OnFindSessionCompleteDelegate);

			/**Call the session interface delegate*/
			sessions->FindSessions(*UserId, searchSettingsRef);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::FindSessions --> The SessionInterface or UserId is invalid!")));
		}
	}
	else
	{
		/**Notify the user that there was a problem searching for sessions*/
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::FindSessions --> No onlline subsystem is found!")));

		/**Call the OnFindSessionComplete delegate with a false value*/
		OnFindSessionsComplete(false);
	}
}

// ===================================================
void AGSPlanetaryPrivateers::OnFindSessionsComplete(bool bWasSuccesfull)
{
	/**Notify a user wether the sessions search was succesfull*/
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AGSPlanetaryPrivateers.cpp:: OnFindSessionComplete --> bSucces: %d"), bWasSuccesfull));

	/**Get the online subsystem to work with*/
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		/**Get the online interface*/
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			//Clear the delegate handle
			sessions->ClearOnFindSessionsCompleteDelegate_Handle(m_OnFindSessionsCompleteDelegateHandle);

			//Check if we found atleast 1 session
			if (m_SessionSearch->SearchResults.Num() > 0)
			{
				for (int32 i = 0; i < m_SessionSearch->SearchResults.Num(); i++)
				{
					//Notify the user about all the found sessions
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("AGSPlanetaryPrivateers.cpp::OnFindSessionsComplete --> Session number: %d, Session Name: %s"), i + 1, *(m_SessionSearch->SearchResults[i].Session.OwningUserName)));
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnFindSessionComplete --> No sessions are being hosted!")));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnFindSessionComplete --> The sessionInterface was invalid!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnFindSessionComplete --> No onlline subsystem is found!")));
	}
}
#pragma endregion 

#pragma region JoinSession
// ===================================================
bool AGSPlanetaryPrivateers::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bsuccesfull = false;

	/**Get the online subsystem*/
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		//Get the online session interface
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid() && UserId.IsValid())
		{
			/**Set the join session handle*/
			m_OnJoinSessionCompleteDelegateHandle = sessions->AddOnJoinSessionCompleteDelegate_Handle(m_OnJoinSessionCompleteDelegate);

			/**Call the build-in join session function*/
			bsuccesfull = sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::JoinSession --> SessionInterface or UserId is invalid!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::JoinSession --> No onlline subsystem is found!")));
	}
	return bsuccesfull;
}

// ===================================================
void AGSPlanetaryPrivateers::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	/**Notify the user if they succesfully joined a session*/
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AGSPlanetaryPrivateers.cpp::OnJoinSessionComplete --> Joined session: %s, with result: %d"), *SessionName.ToString(), static_cast<int32>(Result)));

	/*Get the online subsystem*/
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		/*Get the online session interface*/
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			/**Clear the delegate*/
			sessions->ClearOnJoinSessionCompleteDelegate_Handle(m_OnJoinSessionCompleteDelegateHandle);

			FString travelURL;

			if (m_InvokingPlayerController && sessions->GetResolvedConnectString(SessionName, travelURL))
			{
				//Call clientravel to put the player on the server
				m_InvokingPlayerController->ClientTravel(travelURL, ETravelType::TRAVEL_Absolute);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnJoinSessionComplete --> The invokingPlayer is nullptr or Could not resolve the connection string!")));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnJoinSessionComplete --> The SessionInterface is invalid!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnJoinSessionComplete --> No onlline subsystem is found!")));
	}
}
#pragma endregion 

#pragma region DestroySession
// ===================================================
void AGSPlanetaryPrivateers::OnDestroySessionComplete(FName SessionName, bool bWasSuccesfull)
{
	/**Notify the user of the session that is being destroyed and wether it was succesfull*/
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AGSPlanetaryPrivateers.cpp::OnDestroySessionComplete --> Session being destroyed: %s, succesfull?: %d"), *SessionName.ToString(), bWasSuccesfull));

	/**Get the online subsystem*/
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		//Get the session interface
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			//Clear the delegate
			sessions->ClearOnDestroySessionCompleteDelegate_Handle(m_OnDestroySessionCompleteDelegateHandle);

			//If the session is succesfully destroyed load another level that is local
			if (bWasSuccesfull)
			{
				UGameplayStatics::OpenLevel(GetWorld(), m_MainMenuMap, true);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnDestroySessionComplete --> Destroying the session was unsuccesfull")));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnDestroySessionComplete --> The sessionInterface was invalid!")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GSPlanetaryPrivateers.cpp::OnDestroySessionComplete --> No onlline subsystem is found!")));
	}
}
#pragma endregion

#pragma region DedicatedServer
// ===================================================
void AGSPlanetaryPrivateers::RegisterServer()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInt = Online::GetSessionInterface();
		if (SessionInt.IsValid())
		{
			m_SessionSettings = MakeShareable(new FOnlineSessionSettings());

			/**VARIABLES THAT REQUIRED SOMEWAY OF SET METHOD*/
			m_SessionSettings->bIsLANMatch = false;
			m_SessionSettings->NumPublicConnections = 16;

			/**VARABLES FOR ALL INSTANCES*/
			m_SessionSettings->bIsDedicated = true;
			m_SessionSettings->NumPrivateConnections = 0;
			m_SessionSettings->bUsesPresence = false;
			m_SessionSettings->bAllowInvites = true;
			m_SessionSettings->bAllowJoinInProgress = true;
			m_SessionSettings->bShouldAdvertise = true;
			m_SessionSettings->bAllowJoinViaPresence = true;
			m_SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			m_OnCreateSessionCompleteDelegateHandle = SessionInt->AddOnCreateSessionCompleteDelegate_Handle(m_OnCreateSessionCompleteDelegate);
			SessionInt->CreateSession(0, SessionName, *m_SessionSettings);
		}
	}
}
#pragma endregion