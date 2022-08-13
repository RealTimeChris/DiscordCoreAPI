/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// DiscordCoreClient01.cpp - Source file for the main/exposed DiscordCoreClient class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file DiscordCoreClient.cpp

#include <discordcoreapi/DiscordCoreClient.hpp>
#include <csignal>
#include <atomic>
#include <concepts>

namespace DiscordCoreAPI {

	namespace Globals {
		VoiceConnectionMap voiceConnectionMap{};
		SoundCloudAPIMap soundCloudAPIMap{};
		YouTubeAPIMap youtubeAPIMap{};
		SongAPIMap songAPIMap{};
		std::atomic_bool doWeQuit{ false };
	}

	VoiceConnectionMap& getVoiceConnectionMap() {
		return Globals::voiceConnectionMap;
	}

	SoundCloudAPIMap& getSoundCloudAPIMap() {
		return Globals::soundCloudAPIMap;
	}

	YouTubeAPIMap& getYouTubeAPIMap() {
		return Globals::youtubeAPIMap;
	}

	SongAPIMap& getSongAPIMap() {
		return Globals::songAPIMap;
	}

	void atexitHandler() {
		Globals::doWeQuit.store(true);
	}

	void signalHandler(int32_t) {
		std::exit(EXIT_SUCCESS);
	}

	DiscordCoreClient::DiscordCoreClient(DiscordCoreClientConfig configData) {
		std::atexit(&atexitHandler);
		std::signal(SIGTERM, &signalHandler);
		std::signal(SIGSEGV, &signalHandler);
		std::signal(SIGINT, &signalHandler);
		std::signal(SIGILL, &signalHandler);
		std::signal(SIGABRT, &signalHandler);
		std::signal(SIGFPE, &signalHandler);
		this->configManager = ConfigManager{ configData };
		if (this->configManager.doWePrintFFMPEGSuccessMessages()) {
			av_log_set_level(AV_LOG_INFO);
		}
		if (this->configManager.doWePrintFFMPEGErrorMessages()) {
			av_log_set_level(AV_LOG_ERROR);
		}
		if (!this->configManager.doWePrintFFMPEGErrorMessages() && !this->configManager.doWePrintFFMPEGSuccessMessages()) {
			av_log_set_level(AV_LOG_QUIET);
		}
		if (sodium_init() == -1) {
			if (this->configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "LibSodium failed to initialize!" << reset() << endl << endl;
			}
		}
		this->eventManager.onChannelCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnChannelCreationData>>{ EventHandler::onChannelCreation });
		this->eventManager.onChannelUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnChannelUpdateData>>{ EventHandler::onChannelUpdate });
		this->eventManager.onChannelDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnChannelDeletionData>>{ EventHandler::onChannelDeletion });
		this->eventManager.onGuildMemberAdd(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnGuildMemberAddData>>{ EventHandler::onGuildMemberAdd });
		this->eventManager.onGuildMemberRemove(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnGuildMemberRemoveData>>{ EventHandler::onGuildMemberRemove });
		this->eventManager.onGuildMemberUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnGuildMemberUpdateData>>{ EventHandler::onGuildMemberUpdate });
		this->eventManager.onRoleCreation(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnRoleCreationData>>{ EventHandler::onRoleCreation });
		this->eventManager.onRoleUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnRoleUpdateData>>{ EventHandler::onRoleUpdate });
		this->eventManager.onRoleDeletion(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnRoleDeletionData>>{ EventHandler::onRoleDeletion });
		this->eventManager.onUserUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnUserUpdateData>>{ EventHandler::onUserUpdate });
		this->eventManager.onVoiceStateUpdate(DiscordCoreInternal::EventDelegate<CoRoutine<void>, std::unique_ptr<OnVoiceStateUpdateData>>{ EventHandler::onVoiceStateUpdate });
		EventHandler::initialize(&this->configManager);
		this->httpsClient = std::make_unique<DiscordCoreInternal::HttpsClient>(&this->configManager);
		ApplicationCommands::initialize(this->httpsClient.get());
		AutoModerationRules::initialize(this->httpsClient.get());
		Channels::initialize(this->httpsClient.get(), &this->configManager);
		Guilds::initialize(this->httpsClient.get(), this, &this->configManager);
		GuildMembers::initialize(this->httpsClient.get(), &this->configManager);
		GuildScheduledEvents::initialize(this->httpsClient.get());
		Interactions::initialize(this->httpsClient.get());
		Messages::initialize(this->httpsClient.get());
		Reactions::initialize(this->httpsClient.get());
		Roles::initialize(this->httpsClient.get(), &this->configManager);
		Stickers::initialize(this->httpsClient.get());
		StageInstances::initialize(this->httpsClient.get());
		Threads::initialize(this->httpsClient.get());
		Users::initialize(this->httpsClient.get(), &this->configManager);
		WebHooks::initialize(this->httpsClient.get());
	}

	void DiscordCoreClient::registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		this->commandController.registerFunction(functionNames, std::move(baseFunction));
	}

	CommandController& DiscordCoreClient::getCommandController() {
		return this->commandController;
	}

	EventManager& DiscordCoreClient::getEventManager() {
		return this->eventManager;
	}

	BotUser DiscordCoreClient::getBotUser() {
		return this->currentUser;
	}

	void DiscordCoreClient::runBot() {
		/*
		std::jthread theThread = std::jthread{ [this]() {
			while (!Globals::doWeQuit.load()) {
				for (auto& [key, value]: this->baseSocketAgentMap) {
					if (value && value->sslShard) {
						StopWatch<std::chrono::microseconds> theStopWatch{};
						theStopWatch.resetTimer();
						value->sslShard->parseMessage(value->sslShard.get(), theStopWatch);
					}
				}
				std::this_thread::sleep_for(1ms);
			};
		} };
		theThread.detach();
		*/
		if (!this->instantiateWebSockets()) {
			Globals::doWeQuit.store(true);
			return;
		}
		while (!Globals::doWeQuit.load()) {
			std::this_thread::sleep_for(1ms);
		}
		if (this->baseSocketAgentMap.contains(std::to_string(this->configManager.getStartingShard())) &&
			this->baseSocketAgentMap[std::to_string(this->configManager.getStartingShard())]->getTheTask()) {
			if (this->baseSocketAgentMap[std::to_string(this->configManager.getStartingShard())]->getTheTask()->joinable()) {
				this->baseSocketAgentMap[std::to_string(this->configManager.getStartingShard())]->getTheTask()->join();
			}
		}
	}

	void DiscordCoreClient::insertEvent(std::unique_ptr<EventData> theEventData) {
		std::unique_lock theLock{ this->theAccessMutex };
		this->theEventQueue.emplace_back(std::move(theEventData));
	}

	GatewayBotData DiscordCoreClient::getGateWayBot() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Gateway_Bot };
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/gateway/bot";
		workload.callStack = "DiscordCoreClient::getGateWayBot()";
		GatewayBotData theData{};
		try {
			theData = this->httpsClient->submitWorkloadAndGetResult<GatewayBotData>(workload);
		} catch (...) {
			return theData;
		}
		return theData;
	}

	bool DiscordCoreClient::instantiateWebSockets() {
		GatewayBotData gatewayData = this->getGateWayBot();
		if (gatewayData.url == "") {
			if (this->configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed()
					 << "Failed to collect the connection URL! Closing! Did you remember to "
						"properly set your bot token?"
					 << reset() << endl
					 << endl;
			}
			std::this_thread::sleep_for(5s);
			return false;
		}
		if (this->configManager.getStartingShard() + this->configManager.getShardCountForThisProcess() > this->configManager.getTotalShardCount()) {
			if (this->configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "Your sharding options are incorrect! Please fix it!" << reset() << endl << endl;
			}
			std::this_thread::sleep_for(5s);
			return false;
		}

		int32_t currentShard{ 0 };
		if (this->configManager.getConnectionAddress() == "") {
			this->configManager.setConnectionAddress(gatewayData.url.substr(gatewayData.url.find("wss://") + std::string("wss://").size()));
		}
		if (this->configManager.getConnectionPort() == "") {
			this->configManager.setConnectionPort("443");
		}

		for (int32_t x = 0; x < this->configManager.getTotalShardCount(); ++x) {
			auto thePtr = std::make_unique<DiscordCoreInternal::BaseSocketAgent>(this, &Globals::doWeQuit, x);
			this->baseSocketAgentMap[std::to_string(currentShard)] = std::move(thePtr);
			if (true) {
				cout << shiftToBrightBlue() << "Connecting Shard " + std::to_string(currentShard + 1) << " of " << this->configManager.getShardCountForThisProcess()
					 << std::string(" Shards for this process. (") + std::to_string(currentShard + 1) + " of " + std::to_string(this->configManager.getTotalShardCount()) +
						std::string(" Shards total across all processes)")
					 << reset() << endl
					 << endl;
			}
			ConnectionPackage theData{};
			theData.currentShard = currentShard;
			this->baseSocketAgentMap[std::to_string(currentShard)]->connect(theData);
			currentShard++;
		}
		this->currentUser = BotUser{ Users::getCurrentUserAsync().get(), this->baseSocketAgentMap[std::to_string(this->configManager.getStartingShard())].get() };
		for (auto& value: this->configManager.getFunctionsToExecute()) {
			if (value.repeated) {
				TimeElapsedHandlerNoArgs onSend = [=, this]() -> void {
					value.function(this);
				};
				ThreadPool::storeThread(onSend, value.intervalInMs);
			} else {
				TimeElapsedHandler<void*> onSend = [=, this](void*) -> void {
					value.function(this);
				};
				ThreadPool::executeFunctionAfterTimePeriod(onSend, value.intervalInMs, false, static_cast<void*>(&onSend));
			}
		}
		if (this->configManager.doWePrintGeneralSuccessMessages()) {
			cout << shiftToBrightGreen() << "All of the shards are connected for the current process!" << reset() << endl << endl;
		}
		return true;
	}

	void DiscordCoreClient::dispatchEvent(std::unique_ptr<EventData> theEvent) {
		if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnApplicationCommandPermissionsUpdateData*>(theEvent.release()));
			this->eventManager.onApplicationCommandPermissionsUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnAutoModerationRuleCreationData*>(theEvent.release()));
			this->eventManager.onAutoModerationRuleCreationEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnAutoModerationRuleUpdateData*>(theEvent.release()));
			this->eventManager.onAutoModerationRuleUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnAutoModerationRuleDeletionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnAutoModerationRuleDeletionData*>(theEvent.release()));
			this->eventManager.onAutoModerationRuleDeletionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnAutoModerationActionExecutionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnAutoModerationActionExecutionData*>(theEvent.release()));
			this->eventManager.onAutoModerationActionExecutionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnChannelCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnChannelCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnChannelCreationData*>(theEvent.release()));
			this->eventManager.onChannelCreationEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnChannelUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnChannelUpdateData*>(theEvent.release()));
			this->eventManager.onChannelUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnChannelDeletionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnChannelDeletionData*>(theEvent.release()));
			this->eventManager.onChannelDeletionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnChannelPinsUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnChannelPinsUpdateData*>(theEvent.release()));
			this->eventManager.onChannelPinsUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnThreadCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnThreadCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnThreadCreationData*>(theEvent.release()));
			this->eventManager.onThreadCreationEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnThreadUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnThreadUpdateData*>(theEvent.release()));
			this->eventManager.onThreadUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnThreadDeletionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnThreadDeletionData*>(theEvent.release()));
			this->eventManager.onThreadDeletionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnThreadListSyncData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnThreadListSyncData*>(theEvent.release()));
			this->eventManager.onThreadListSyncEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnThreadMemberUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnThreadMemberUpdateData*>(theEvent.release()));
			this->eventManager.onThreadMemberUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnThreadMembersUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnThreadMembersUpdateData*>(theEvent.release()));
			this->eventManager.onThreadMembersUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildCreationData*>(theEvent.release()));
			this->eventManager.onGuildCreationEvent(std::move(thePtr));
		}  else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildUpdateData*>(theEvent.release()));
			this->eventManager.onGuildUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildDeletionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildDeletionData*>(theEvent.release()));
			this->eventManager.onGuildDeletionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildBanAddData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildBanAddData*>(theEvent.release()));
			this->eventManager.onGuildBanAddEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildBanRemoveData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildBanRemoveData*>(theEvent.release()));
			this->eventManager.onGuildBanRemoveEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildEmojisUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildEmojisUpdateData*>(theEvent.release()));
			this->eventManager.onGuildEmojisUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildStickersUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildStickersUpdateData*>(theEvent.release()));
			this->eventManager.onGuildStickersUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildIntegrationsUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildIntegrationsUpdateData*>(theEvent.release()));
			this->eventManager.onGuildIntegrationsUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildMemberAddData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildMemberAddData*>(theEvent.release()));
			this->eventManager.onGuildMemberAddEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildMemberRemoveData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildMemberRemoveData*>(theEvent.release()));
			this->eventManager.onGuildMemberRemoveEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildMemberUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildMemberUpdateData*>(theEvent.release()));
			this->eventManager.onGuildMemberUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnGuildMembersChunkData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnGuildMembersChunkData*>(theEvent.release()));
			this->eventManager.onGuildMembersChunkEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnRoleCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnRoleCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnRoleCreationData*>(theEvent.release()));
			this->eventManager.onRoleCreationEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnRoleUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnRoleUpdateData*>(theEvent.release()));
			this->eventManager.onRoleUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnRoleDeletionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnRoleDeletionData*>(theEvent.release()));
			this->eventManager.onRoleDeletionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnIntegrationCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnIntegrationCreationData*>(theEvent.release()));
			this->eventManager.onIntegrationCreationEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnIntegrationUpdateData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnIntegrationUpdateData*>(theEvent.release()));
			this->eventManager.onIntegrationUpdateEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnIntegrationDeletionData*>(theEvent.release()));
			this->eventManager.onIntegrationDeletionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnIntegrationDeletionData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnIntegrationDeletionData*>(theEvent.release()));
			this->eventManager.onIntegrationDeletionEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnInteractionCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnInteractionCreationData*>(theEvent.release()));
			this->eventManager.onInteractionCreationEvent(std::move(thePtr));
		} else if (std::is_same<EventType, std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData>>().value) {
			std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> thePtr{};
			thePtr.reset(static_cast<DiscordCoreAPI::OnInputEventCreationData*>(theEvent.release()));
			this->eventManager.onInputEventCreationEvent(std::move(thePtr));
		} /*
		
		} else if (payloadNew["t"] == "INVITE_CREATE") {
			std::unique_ptr<DiscordCoreAPI::OnInviteCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteCreationData>() };
			dataPackage->invite = payloadNew["d"];
			this->eventManager.onInviteCreationEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "INVITE_DELETE") {
			std::unique_ptr<DiscordCoreAPI::OnInviteDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnInviteDeletionData>() };
			if (payloadNew["d"].contains("channel_id") && !payloadNew["d"]["channel_id"].is_null()) {
				dataPackage->channelId = stoull(payloadNew["d"]["channel_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("guild_id") && !payloadNew["d"]["guild_id"].is_null()) {
				dataPackage->guildId = stoull(payloadNew["d"]["guild_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("code") && !payloadNew["d"]["code"].is_null()) {
				dataPackage->code = payloadNew["d"]["code"];
			}
			this->eventManager.onInviteDeletionEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "MESSAGE_CREATE") {
			std::unique_ptr<DiscordCoreAPI::Message> message{ std::make_unique<DiscordCoreAPI::Message>() };
			*message = payloadNew["d"];
			std::unique_ptr<DiscordCoreAPI::OnMessageCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageCreationData>() };
			dataPackage->message = *message;
			for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
				value->send(*message);
			}
			this->eventManager.onMessageCreationEvent(std::move(theEvent.release()));
			if (message->content.find("!registerapplicationcommands") != std::string::npos) {
				std::unique_ptr<DiscordCoreAPI::CommandData> commandData{ std::make_unique<DiscordCoreAPI::CommandData>() };
				commandData->commandName = "registerapplicationcommands";
				DiscordCoreAPI::CommandData commandDataNew = *commandData;
				this->discordCoreClient->commandController.checkForAndRunCommand(commandDataNew);
			}
			std::unique_ptr<DiscordCoreAPI::OnInputEventCreationData> eventCreationData{ std::make_unique<DiscordCoreAPI::OnInputEventCreationData>() };
		} else if (payloadNew["t"] == "MESSAGE_UPDATE") {
			std::unique_ptr<DiscordCoreAPI::OnMessageUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageUpdateData>() };
			dataPackage->messageNew = payloadNew["d"];
			for (auto& [key, value]: DiscordCoreAPI::MessageCollector::messagesBufferMap) {
				value->send(dataPackage->messageNew);
			}
			this->eventManager.onMessageUpdateEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "MESSAGE_DELETE") {
			std::unique_ptr<DiscordCoreAPI::OnMessageDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeletionData>() };
			if (payloadNew["d"].contains("guild_id") && !payloadNew["d"]["guild_id"].is_null()) {
				dataPackage->guildId = stoull(payloadNew["d"]["guild_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("channel_id") && !payloadNew["d"]["channel_id"].is_null()) {
				dataPackage->channelId = stoull(payloadNew["d"]["channel_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("id") && !payloadNew["d"]["id"].is_null()) {
				dataPackage->messageId = stoull(payloadNew["d"]["id"].get<std::string>());
			}
			this->eventManager.onMessageDeletionEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "MESSAGE_DELETE_BULK") {
			std::unique_ptr<DiscordCoreAPI::OnMessageDeleteBulkData> dataPackage{ std::make_unique<DiscordCoreAPI::OnMessageDeleteBulkData>() };
			if (payloadNew["d"].contains("channel_id") && !payloadNew["d"]["channel_id"].is_null()) {
				dataPackage->channelId = stoull(payloadNew["d"]["channel_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("guild_id") && !payloadNew["d"]["guild_id"].is_null()) {
				dataPackage->guildId = stoull(payloadNew["d"]["guild_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("ids") && !payloadNew["d"]["ids"].is_null()) {
				for (auto& value: payloadNew["d"]["ids"]) {
					dataPackage->ids.push_back(stoull(value.get<std::string>()));
				}
			}
			this->eventManager.onMessageDeleteBulkEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "MESSAGE_REACTION_ADD") {
			std::unique_ptr<DiscordCoreAPI::OnReactionAddData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionAddData>() };
			dataPackage->reaction = payloadNew["d"];
			this->eventManager.onReactionAddEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "MESSAGE_REACTION_REMOVE") {
			std::unique_ptr<DiscordCoreAPI::OnReactionRemoveData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveData>() };
			dataPackage->reactionRemoveData = payloadNew["d"];
			this->eventManager.onReactionRemoveEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "MESSAGE_REACTION_REMOVE_ALL") {
			std::unique_ptr<DiscordCoreAPI::OnReactionRemoveAllData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveAllData>() };
			if (payloadNew["d"].contains("channel_id") && !payloadNew["d"]["channel_id"].is_null()) {
				dataPackage->channelId = stoull(payloadNew["d"]["channel_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("message_id") && !payloadNew["d"]["message_id"].is_null()) {
				dataPackage->messageId = stoull(payloadNew["d"]["message_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("guild_id") && !payloadNew["d"]["guild_id"].is_null()) {
				dataPackage->guildId = stoull(payloadNew["d"]["guild_id"].get<std::string>());
			}
			this->eventManager.onReactionRemoveAllEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "MESSAGE_REACTION_REMOVE_EMOJI") {
			std::unique_ptr<DiscordCoreAPI::OnReactionRemoveEmojiData> dataPackage{ std::make_unique<DiscordCoreAPI::OnReactionRemoveEmojiData>() };
			if (payloadNew["d"].contains("channel_id") && !payloadNew["d"]["channel_id"].is_null()) {
				dataPackage->channelId = stoull(payloadNew["d"]["channel_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("message_id") && !payloadNew["d"]["message_id"].is_null()) {
				dataPackage->messageId = stoull(payloadNew["d"]["message_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("guild_id") && !payloadNew["d"]["guild_id"].is_null()) {
				dataPackage->guildId = stoull(payloadNew["d"]["guild_id"].get<std::string>());
			}
			dataPackage->emoji = payloadNew["d"]["emoji"];
			this->eventManager.onReactionRemoveEmojiEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "PRESENCE_UPDATE") {
			std::unique_ptr<DiscordCoreAPI::OnPresenceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnPresenceUpdateData>() };
			dataPackage->presenceData = payloadNew["d"];
			this->eventManager.onPresenceUpdateEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "STAGE_INSTANCE_CREATE") {
			std::unique_ptr<DiscordCoreAPI::OnStageInstanceCreationData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceCreationData>() };
			dataPackage->stageInstance = payloadNew["d"];
			this->eventManager.onStageInstanceCreationEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "STAGE_INSTANCE_UPDATE") {
			std::unique_ptr<DiscordCoreAPI::OnStageInstanceUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceUpdateData>() };
			dataPackage->stageInstance = payloadNew["d"];
			this->eventManager.onStageInstanceUpdateEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "STAGE_INSTANCE_DELETE") {
			std::unique_ptr<DiscordCoreAPI::OnStageInstanceDeletionData> dataPackage{ std::make_unique<DiscordCoreAPI::OnStageInstanceDeletionData>() };
			dataPackage->stageInstance = payloadNew["d"];
			this->eventManager.onStageInstanceDeletionEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "TYPING_START") {
			std::unique_ptr<DiscordCoreAPI::OnTypingStartData> dataPackage{ std::make_unique<DiscordCoreAPI::OnTypingStartData>() };
			dataPackage->typingStartData = payloadNew["d"];
			this->eventManager.onTypingStartEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "USER_UPDATE") {
			std::unique_ptr<DiscordCoreAPI::OnUserUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnUserUpdateData>() };
			dataPackage->userNew = DiscordCoreAPI::User(payloadNew["d"]["user"]);
			dataPackage->userOld = DiscordCoreAPI::User(DiscordCoreAPI::Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get());
			this->eventManager.onUserUpdateEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "VOICE_STATE_UPDATE") {
			if (this->areWeCollectingData && !this->stateUpdateCollected && !this->serverUpdateCollected &&
				payloadNew["d"]["member"]["user"]["id"] == std::to_string(this->userId)) {
				this->voiceConnectionData = VoiceConnectionData{};
				this->voiceConnectionData.sessionId = payloadNew["d"]["session_id"].get<std::string>();
				this->stateUpdateCollected = true;
			} else if (this->areWeCollectingData && !this->stateUpdateCollected) {
				this->voiceConnectionData.sessionId = payloadNew["d"]["session_id"].get<std::string>();
				if (this->voiceConnectionDataBufferMap.contains(stoull(payloadNew["d"]["guild_id"].get<std::string>()))) {
					this->voiceConnectionDataBufferMap[stoull(payloadNew["d"]["guild_id"].get<std::string>())]->send(this->voiceConnectionData);
				}
				this->serverUpdateCollected = false;
				this->stateUpdateCollected = false;
				this->areWeCollectingData = false;
			}
			std::unique_ptr<DiscordCoreAPI::OnVoiceStateUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceStateUpdateData>() };
			dataPackage->voiceStateData = payloadNew["d"];
			this->eventManager.onVoiceStateUpdateEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "VOICE_SERVER_UPDATE") {
			if (this->areWeCollectingData && !this->serverUpdateCollected && !this->stateUpdateCollected) {
				this->voiceConnectionData = VoiceConnectionData{};
				this->voiceConnectionData.endPoint = payloadNew["d"]["endpoint"].get<std::string>();
				this->voiceConnectionData.token = payloadNew["d"]["token"].get<std::string>();
				this->serverUpdateCollected = true;
			} else if (this->areWeCollectingData && !this->serverUpdateCollected) {
				this->voiceConnectionData.endPoint = payloadNew["d"]["endpoint"].get<std::string>();
				this->voiceConnectionData.token = payloadNew["d"]["token"].get<std::string>();
				if (this->voiceConnectionDataBufferMap.contains(stoull(payloadNew["d"]["guild_id"].get<std::string>()))) {
					this->voiceConnectionDataBufferMap[stoull(payloadNew["d"]["guild_id"].get<std::string>())]->send(this->voiceConnectionData);
				}
				this->serverUpdateCollected = false;
				this->stateUpdateCollected = false;
				this->areWeCollectingData = false;
			}
			std::unique_ptr<DiscordCoreAPI::OnVoiceServerUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnVoiceServerUpdateData>() };
			if (payloadNew["d"].contains("endpoint") && !payloadNew["d"]["endpoint"].is_null()) {
				dataPackage->endpoint = payloadNew["d"]["endpoint"];
			}
			if (payloadNew["d"].contains("guild_id") && !payloadNew["d"]["guild_id"].is_null()) {
				dataPackage->guildId = stoull(payloadNew["d"]["guild_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("token") && !payloadNew["d"]["token"].is_null()) {
				dataPackage->token = payloadNew["d"]["token"];
			}
			this->eventManager.onVoiceServerUpdateEvent(std::move(theEvent.release()));
		} else if (payloadNew["t"] == "WEBHOOKS_UPDATE") {
			std::unique_ptr<DiscordCoreAPI::OnWebhookUpdateData> dataPackage{ std::make_unique<DiscordCoreAPI::OnWebhookUpdateData>() };
			if (payloadNew["d"].contains("channel_id") && !payloadNew["d"]["channel_id"].is_null()) {
				dataPackage->channelId = stoull(payloadNew["d"]["channel_id"].get<std::string>());
			}
			if (payloadNew["d"].contains("guild_id") && !payloadNew["d"]["guild_id"].is_null()) {
				dataPackage->guildId = stoull(payloadNew["d"]["guild_id"].get<std::string>());
			}
			this->eventManager.onWebhookUpdateEvent(std::move(theEvent.release()));
		}*/
	}

	void DiscordCoreClient::dispatchEventWrapper() {
		if (this->theEventQueue.size() > 0) {
			std::unique_lock theLock{ this->theAccessMutex };
			auto theEvent = std::move(this->theEventQueue.front());
			this->theEventQueue.pop_front();
			theLock.unlock();
			this->dispatchEvent(std::move(theEvent));
		}
	}	

}
