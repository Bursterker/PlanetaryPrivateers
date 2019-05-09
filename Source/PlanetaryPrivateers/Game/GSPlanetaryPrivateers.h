// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "Online.h"
#include "GSPlanetaryPrivateers.generated.h"

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API AGSPlanetaryPrivateers : public AGameSession
{
	GENERATED_UCLASS_BODY()

public:
	/**Variables to hold all SessionSettings*/
	TSharedPtr<class FOnlineSessionSettings> m_SessionSettings;
	FName m_LobbyMap = "Phase1_2";
	FName m_MainMenuMap = "MainMenu";

public:
#pragma region HostSession
	/**
	* Function to host a new session
	* @Param							UserId							User that initializes the request
	* @Param							SessionName				Name of the session
	* @Param							bIsLan							True if the game is hosted over LAN
	* @Param							bIsPresence					True if this is a presence session
	* @Param							MaxNumPlayers			Max number of players in this session
	*
	* @Return							bool								True if succesfull
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);

	/**Delegate called when a session is created*/
	FOnCreateSessionCompleteDelegate m_OnCreateSessionCompleteDelegate;

	/**Delegate called when a session is started*/
	FOnStartSessionCompleteDelegate m_OnStartSessionCompleteDelegate;

	/**Handles to register the hosting delegates*/
	FDelegateHandle m_OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle m_OnStartSessionCompleteDelegateHandle;

	/**
	*Function fired when a new session creation request is called
	*@Param							SessionName			Name of the session
	*@Param							bWasSuccesfull		True if the async action completed without error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccesfull);

	/**
	*Function fired when a new session start request is called
	*@Param							SessionName			Name of the session
	*@Param							bWasSuccesfull		True if the async action completed without error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccesfull);
#pragma endregion 

#pragma region SearchSession
	/**
	* Find a session
	* @Param				UserId				User that initiates the request
	* @Param				bIsLan				Searching for a LAN match?
	* @Param				bIsPresence		Are we searching for a presence session?
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence);

	/**Delegate for searching a session*/
	FOnFindSessionsCompleteDelegate m_OnFindSessionCompleteDelegate;

	/**Handle to register delegate for searching a session*/
	FDelegateHandle m_OnFindSessionsCompleteDelegateHandle;

	/**Variable to hold our searchsettings*/
	TSharedPtr<class FOnlineSessionSearch> m_SessionSearch;

	/**Variable to hold maximum ammount of sessions to search for*/
	int m_MaxSearchSessions = 20;

	/**Variable to store the PingBucketSize*/
	int m_PingBucketSize = 50;

	/**
	* Delegate fired when a search query has completed
	* @Param				bWasSuccesfull				true if the async action completed
	*/
	void OnFindSessionsComplete(bool bWasSuccesfull);
#pragma endregion 

#pragma region JoinSession
	/**
	* Joins a session via a seaarch result
	* @Param				UserId							Id of the user
	* @Param				SessionName				Name of the session
	* @Param				SearchResult				The session to join
	*
	* @Return				bool								True if succesfull, false if not
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/**Delegate to join the session*/
	FOnJoinSessionCompleteDelegate m_OnJoinSessionCompleteDelegate;

	/**Handle to register the delegate to join a session*/
	FDelegateHandle m_OnJoinSessionCompleteDelegateHandle;

	/**
	* Delegate fired when a session join request has completed
	* @Param				SessionName				The name of the session the user joined
	* @Param				Result							Result of joining a session
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	APlayerController* m_InvokingPlayerController = nullptr;
#pragma endregion 

#pragma region DestroySession
	/**Delegate for destroying the session*/
	FOnDestroySessionCompleteDelegate m_OnDestroySessionCompleteDelegate;

	/**Handle to register the session destroy delegate*/
	FDelegateHandle m_OnDestroySessionCompleteDelegateHandle;

	/**
	* Delegate fired when a destroying online session has completed
	* @Param					SessionName						Name of the session being destroyed
	* @Param					bWasSuccesfull					True if the async action completed without error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccesfull);
#pragma endregion

#pragma region DedicatedServer
	/**
	* Called when this instance is starting up as a dedicated server
	*/
	virtual void RegisterServer() override;
#pragma endregion
};
