/*
*
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
		DiscordCoreInternal::HttpsSSLClient::initialize();
		DiscordCoreInternal::WebSocketSSLShard::initialize();
		this->functionsToExecute = configData.functionsToExecute;
		this->loggingOptions = configData.logOptions;
		this->theAddress = configData.alternateConnectionAddress;
		this->thePort = configData.alternateConnectionPort;
		this->shardingOptions = configData.shardOptions;
		this->cacheOptions = configData.cacheOptions;
		this->theIntents = configData.theIntents;
		if (this->loggingOptions.logFFMPEGSuccessMessages) {
			av_log_set_level(AV_LOG_INFO);
		}
		if (this->loggingOptions.logFFMPEGErrorMessages) {
			av_log_set_level(AV_LOG_ERROR);
		}
		if (!this->loggingOptions.logFFMPEGErrorMessages && !this->loggingOptions.logFFMPEGSuccessMessages) {
			av_log_set_level(AV_LOG_QUIET);
		}
		if (sodium_init() == -1) {
			if (this->loggingOptions.logGeneralErrorMessages) {
				std::cout << shiftToBrightRed() << "LibSodium failed to initialize!" << reset() << std::endl << std::endl;
			}
		}
		this->eventManager.onChannelCreation(&EventHandler::onChannelCreation);
		this->eventManager.onChannelUpdate(&EventHandler::onChannelUpdate);
		this->eventManager.onChannelDeletion(&EventHandler::onChannelDeletion);
		this->eventManager.onGuildCreation(&EventHandler::onGuildCreation);
		this->eventManager.onGuildUpdate(&EventHandler::onGuildUpdate);
		this->eventManager.onGuildDeletion(&EventHandler::onGuildDeletion);
		this->eventManager.onGuildMemberAdd(&EventHandler::onGuildMemberAdd);
		this->eventManager.onGuildMemberRemove(&EventHandler::onGuildMemberRemove);
		this->eventManager.onGuildMemberUpdate(&EventHandler::onGuildMemberUpdate);
		this->eventManager.onRoleCreation(&EventHandler::onRoleCreation);
		this->eventManager.onRoleUpdate(&EventHandler::onRoleUpdate);
		this->eventManager.onRoleDeletion(&EventHandler::onRoleDeletion);
		this->eventManager.onUserUpdate(&EventHandler::onUserUpdate);
		this->eventManager.onVoiceStateUpdate(&EventHandler::onVoiceStateUpdate);
		EventHandler::initialize(this->cacheOptions);
		this->httpsClient =
			std::make_unique<DiscordCoreInternal::HttpsClient>(configData.botToken, this->loggingOptions.logHttpsSuccessMessages, this->loggingOptions.logHttpsErrorMessages,
				this->loggingOptions.logFFMPEGSuccessMessages, this->loggingOptions.logFFMPEGErrorMessages, this->loggingOptions.logWebSocketErrorMessages);
		ApplicationCommands::initialize(this->httpsClient.get());
		AutoModerationRules::initialize(this->httpsClient.get());
		Channels::initialize(this->httpsClient.get(), this->cacheOptions.cacheChannels);
		Guilds::initialize(this->httpsClient.get(), this, this->cacheOptions.cacheGuilds);
		GuildMembers::initialize(this->httpsClient.get(), this->cacheOptions.cacheGuildMembers);
		GuildScheduledEvents::initialize(this->httpsClient.get());
		Interactions::initialize(this->httpsClient.get());
		Messages::initialize(this->httpsClient.get());
		Reactions::initialize(this->httpsClient.get());
		Roles::initialize(this->httpsClient.get(), this->cacheOptions.cacheRoles);
		Stickers::initialize(this->httpsClient.get());
		StageInstances::initialize(this->httpsClient.get());
		Threads::initialize(this->httpsClient.get());
		Users::initialize(this->httpsClient.get(), this->cacheOptions.cacheUsers);
		WebHooks::initialize(this->httpsClient.get());
		this->botToken = configData.botToken;
		this->theFormat = configData.textFormat;
	}

	void DiscordCoreClient::registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		this->commandController.registerFunction(functionNames, std::move(baseFunction));
	}

	BotUser DiscordCoreClient::getBotUser() {
		return this->currentUser;
	}

	void DiscordCoreClient::runBot() {
		if (!this->instantiateWebSockets()) {
			return;
		}
		this->baseSocketAgentMap[std::to_string(this->shardingOptions.startingShard)]->getTheTask()->join();
	}

	GatewayBotData DiscordCoreClient::getGateWayBot() {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Gateway_Bot;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/gateway/bot";
		workload.callStack = "DiscordCoreClient::getGateWayBot";
		return this->httpsClient->submitWorkloadAndGetResult<GatewayBotData>(workload);
	}

	bool DiscordCoreClient::instantiateWebSockets() {
		GatewayBotData gatewayData = this->getGateWayBot();
		if (gatewayData.url == "") {
			if (this->loggingOptions.logGeneralErrorMessages) {
				std::cout << shiftToBrightRed()
						  << "Failed to collect the connection URL! Closing! Did you remember to "
							 "properly set your bot token?"
						  << reset() << std::endl
						  << std::endl;
			}
			std::this_thread::sleep_for(5s);
			return false;
		}
		if (this->shardingOptions.startingShard + this->shardingOptions.numberOfShardsForThisProcess > this->shardingOptions.totalNumberOfShards) {
			if (this->loggingOptions.logGeneralErrorMessages) {
				std::cout << shiftToBrightRed() << "Your sharding options are incorrect! Please fix it!" << reset() << std::endl << std::endl;
			}
			std::this_thread::sleep_for(5s);
			return false;
		}
		this->shardingOptions.numberOfShardsForThisProcess = this->shardingOptions.totalNumberOfShards;
		auto baseSocketAgentCount = static_cast<int32_t>((std::thread::hardware_concurrency()));
		int32_t shardsPerBaseSocketAgent{ static_cast<int32_t>(floor(static_cast<float>(this->shardingOptions.totalNumberOfShards) / static_cast<float>(baseSocketAgentCount))) };
		int32_t leftOverShards{ this->shardingOptions.totalNumberOfShards - (shardsPerBaseSocketAgent * baseSocketAgentCount) };

		std::vector<int32_t> shardsPerBaseSocketAgentVect{};
		for (int32_t x = 0; x < baseSocketAgentCount; x++) {
			int32_t newShardAmount{};
			shardsPerBaseSocketAgentVect.push_back(shardsPerBaseSocketAgent);
			if (leftOverShards == 0) {
				continue;
			}
			newShardAmount = static_cast<int32_t>(ceil(static_cast<float>(leftOverShards) / static_cast<float>(std::thread::hardware_concurrency())));
			shardsPerBaseSocketAgentVect[x] += newShardAmount;

			if (x == static_cast<int32_t>(std::thread::hardware_concurrency()) - 1) {
				shardsPerBaseSocketAgentVect[0] += leftOverShards;
			}
			leftOverShards -= newShardAmount;
		}
		int32_t currentShard{ 0 };
		if (this->theAddress == "") {
			this->theAddress = gatewayData.url.substr(gatewayData.url.find("wss://") + std::string("wss://").size());
		}
		if (this->thePort == "") {
			this->thePort = "443";
		}

		for (int32_t x = 0; x < shardsPerBaseSocketAgentVect.size(); x++) {
			auto thePtr = std::make_unique<DiscordCoreInternal::BaseSocketAgent>(this, &Globals::doWeQuit, x);
			this->baseSocketAgentMap[std::to_string(x)] = std::move(thePtr);
			for (int32_t y = 0; y < shardsPerBaseSocketAgentVect[x]; y++) {
				if (this->loggingOptions.logGeneralSuccessMessages) {
					std::cout << shiftToBrightBlue() << "Connecting Shard " + std::to_string(currentShard + 1) << " of " << this->shardingOptions.numberOfShardsForThisProcess
							  << std::string(" Shards for this process. (") + std::to_string(currentShard + 1) + " of " +
							std::to_string(this->shardingOptions.totalNumberOfShards) + std::string(" Shards total across all processes)")
							  << reset() << std::endl
							  << std::endl;
				}
				ConnectionPackage theData{};
				theData.currentShard = currentShard;
				theData.currentBaseSocketAgent = x;
				this->baseSocketAgentMap[std::to_string(x)]->connect(theData);
				currentShard++;
			}
		}
		this->currentUser = BotUser{ Users::getCurrentUserAsync().get(), this->baseSocketAgentMap[std::to_string(this->shardingOptions.startingShard)].get() };
		for (auto& value: this->functionsToExecute) {
			TimeElapsedHandler<int64_t> onSend = [=, this](int64_t theArg) -> void {
				value.function(this);
			};
			this->threadIds.push_back(ThreadPool::storeThread(onSend, value.intervalInMs, value.repeated, value.dummyArg));
		}
		if (this->loggingOptions.logGeneralSuccessMessages) {
			std::cout << shiftToBrightGreen() << "All of the shards are connected for the current process!" << reset() << std::endl << std::endl;
		}
		return true;
	}

}
