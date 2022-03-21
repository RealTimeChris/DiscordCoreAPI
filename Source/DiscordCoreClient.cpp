/*
*
	Copyright 2021, 2022 Chris M.

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
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file DiscordCoreClient.cpp

#include <DiscordCoreClient.hpp>
#include <csignal>

namespace DiscordCoreAPI {

	namespace Globals {
		std::unordered_map<std::string, std::unique_ptr<VoiceConnection>> voiceConnectionMap{};
		std::unordered_map<std::string, std::unique_ptr<SoundCloudAPI>> soundCloudAPIMap{};
		std::unordered_map<std::string, std::unique_ptr<YouTubeAPI>> youtubeAPIMap{};
		std::unordered_map<std::string, std::unique_ptr<SongAPI>> songAPIMap{};
		std::atomic<bool> doWeQuit{ false };
	}	
	
	std::unordered_map<std::string, std::unique_ptr<VoiceConnection>>& getVoiceConnectionMap() {
		return Globals::voiceConnectionMap;
	}

	std::unordered_map<std::string, std::unique_ptr<SoundCloudAPI>>& getSoundCloudAPIMap() {
		return Globals::soundCloudAPIMap;
	}

	std::unordered_map<std::string, std::unique_ptr<YouTubeAPI>>& getYouTubeAPIMap() {
		return Globals::youtubeAPIMap;
	}

	std::unordered_map<std::string, std::unique_ptr<SongAPI>>& getSongAPIMap() {
		return Globals::songAPIMap;
	}

	DiscordCoreClient::DiscordCoreClient(std::string botTokenNew, std::vector<RepeatedFunctionData> functionsToExecuteNew, CacheOptions cacheOptionsNew, ShardingOptions shardingOptionsNew, LoggingOptions loggingOptionsNew) {
		std::signal(SIGTERM, &signalHandler);
		std::signal(SIGABRT, &signalHandler);
		std::signal(SIGILL, &signalHandler);
		std::signal(SIGINT, &signalHandler);
		this->loggingOptions = loggingOptionsNew;
		this->shardingOptions = shardingOptionsNew;
		this->cacheOptions = cacheOptionsNew;
		this->functionsToExecute = functionsToExecuteNew;
		this->eventManager = EventManager{};
		this->commandController = CommandController{ this };
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
		this->httpClient = std::make_unique<DiscordCoreInternal::HttpClient>(botTokenNew, this->loggingOptions.logHttpMessages, this->loggingOptions.logFFMPEGMessages);
		ApplicationCommands::initialize(this->httpClient.get());
		Channels::initialize(this->httpClient.get());
		Guilds::initialize(this->httpClient.get(), this);
		GuildMembers::initialize(this->httpClient.get());
		GuildScheduledEvents::initialize(this->httpClient.get());
		Interactions::initialize(this->httpClient.get());
		Messages::initialize(this->httpClient.get());
		Reactions::initialize(this->httpClient.get());
		Roles::initialize(this->httpClient.get());
		Stickers::initialize(this->httpClient.get());
		StageInstances::initialize(this->httpClient.get());
		Threads::initialize(this->httpClient.get());
		Users::initialize(this->httpClient.get());
		WebHooks::initialize(this->httpClient.get());
		GatewayBotData gatewayData = this->getGateWayBot();
		if (this->shardingOptions.startingShard + this->shardingOptions.numberOfShardsForThisProcess > this->shardingOptions.totalNumberOfShards) {
			std::cout << shiftToBrightRed() << "Your sharding options are incorrect! Please fix it!" << reset() << std::endl;
			return;
		}
		int32_t shardGroupCount{ static_cast<int32_t>(ceil(static_cast<double>(this->shardingOptions.numberOfShardsForThisProcess) / static_cast<double>(gatewayData.sessionStartLimit.maxConcurrency))) };
		int32_t shardsPerGroup{ static_cast<int32_t>(ceil(static_cast<double>(this->shardingOptions.numberOfShardsForThisProcess) / static_cast<double>(shardGroupCount))) };
		for (int32_t x = 0; x < shardGroupCount; x += 1) {
			for (int32_t y = 0; y < shardsPerGroup; y += 1) {
				std::cout << shiftToBrightBlue << "Connecting Shard " + std::to_string(x * shardsPerGroup + y + 1) << " of " << this->shardingOptions.numberOfShardsForThisProcess << std::string(" Shards for this process. (") + std::to_string(x * shardsPerGroup + y + 1 + this->shardingOptions.startingShard) + " of " + std::to_string(this->shardingOptions.totalNumberOfShards) + std::string(" Shards total across all processes.)") << std::endl;
				this->theWebSockets.insert_or_assign(std::to_string(x * shardsPerGroup + y + this->shardingOptions.startingShard), std::make_unique<DiscordCoreInternal::BaseSocketAgent>(botTokenNew, gatewayData.url.substr(gatewayData.url.find("wss://") + std::string("wss://").size()), &this->eventManager, this, &this->commandController, &Globals::doWeQuit, this->loggingOptions.logWebSocketMessages, x * shardsPerGroup + y + this->shardingOptions.startingShard, this->shardingOptions.totalNumberOfShards));
			}
			if (shardGroupCount > 1 && x <  shardGroupCount - 1) {
				std::cout << "Waiting to connect the subsequent group of shards..." << std::endl << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds{ 20000 });
			}			
		}
		this->currentUser = BotUser{ Users::getCurrentUserAsync().get(), this->theWebSockets.at(std::to_string(this->shardingOptions.startingShard)).get() };
		std::cout << shiftToBrightGreen() << "All of the shards are connected for the current process!" << reset() << std::endl << std::endl;
		for (auto& value : this->functionsToExecute) {
			if (value.repeated) {
				TimeElapsedHandler onSend = [&](void)->void {
					value.function(this);
				};
				this->threadPoolTimers.push_back(ThreadPoolTimer::createPeriodicTimer(onSend, value.intervalInMs));
			}
			else {
				ThreadPoolTimer::executeFunctionAfterTimePeriod(value.function, value.intervalInMs, this).detachThread();
			}
		}
		this->didWeStartFine = true;
	}

	DiscordCoreClient::DiscordCoreClient(nullptr_t) {};

	void DiscordCoreClient::registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		this->commandController.registerFunction(functionNames, std::move(baseFunction));
	}

	BotUser DiscordCoreClient::getBotUser() {
		return this->currentUser;
	}

	void DiscordCoreClient::runBot() {
		if (!this->didWeStartFine) {
			return;
		}
		this->theWebSockets.at(std::to_string(this->shardingOptions.startingShard))->theTask->get();
	}

	GatewayBotData DiscordCoreClient::getGateWayBot() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Gateway_Bot;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/gateway/bot";
			workload.callStack = "DiscordCoreClient::getGateWayBot";
			return DiscordCoreInternal::submitWorkloadAndGetResult<GatewayBotData>(*this->httpClient, workload);
		}
		catch (...) {
			reportException("DiscordCoreClient::getGateWayBot()");
		}
		return GatewayBotData{};
	}

	DiscordCoreClient::~DiscordCoreClient() {
		if (this != nullptr) {
			curl_global_cleanup();
		}
	}
}

void signalHandler(int32_t)
{
	DiscordCoreAPI::Globals::doWeQuit.store(true, std::memory_order_seq_cst);
}
