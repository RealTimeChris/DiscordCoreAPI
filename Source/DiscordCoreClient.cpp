/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
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

	DiscordCoreInternal::SoundCloudAPI* DiscordCoreClient::getSoundCloudAPI(Snowflake guildId) {
		GuildData theData{};
		theData.id = guildId;
		GuildData* theGuild = &Guilds::cache.at(theData);
		if (!Globals::soundCloudAPIMap.contains(guildId)) {
			Globals::soundCloudAPIMap[guildId] =
				std::make_unique<DiscordCoreInternal::SoundCloudAPI>(&theGuild->discordCoreClient->configManager, theGuild->discordCoreClient->httpsClient.get(), guildId);
		}
		return Globals::soundCloudAPIMap[guildId].get();
	}

	DiscordCoreInternal::YouTubeAPI* DiscordCoreClient::getYouTubeAPI(Snowflake guildId) {
		GuildData theData{};
		theData.id = guildId;
		GuildData* theGuild = &Guilds::cache.at(theData);
		if (!Globals::youtubeAPIMap.contains(guildId)) {
			Globals::youtubeAPIMap[guildId] =
				std::make_unique<DiscordCoreInternal::YouTubeAPI>(&theGuild->discordCoreClient->configManager, theGuild->discordCoreClient->httpsClient.get(), guildId);
		}
		return Globals::youtubeAPIMap[guildId].get();
	}

	VoiceConnection* DiscordCoreClient::getVoiceConnection(Snowflake guildId) {
		GuildData theData{};
		theData.id = guildId;
		GuildData* theGuild = &Guilds::cache.at(theData);
		if (!Globals::voiceConnectionMap.contains(guildId)) {
			uint64_t theShardId{ (guildId >> 22) % theGuild->discordCoreClient->configManager.getTotalShardCount() };
			uint64_t baseSocketIndex{ theShardId % theGuild->discordCoreClient->baseSocketAgentMap.size() };
			auto baseSocketAgent = theGuild->discordCoreClient->baseSocketAgentMap[baseSocketIndex].get();
			Globals::voiceConnectionMap[guildId] = std::make_unique<VoiceConnection>(baseSocketAgent, baseSocketAgent->theShardMap[theShardId].get(),
				DiscordCoreInternal::VoiceConnectInitData{}, &theGuild->discordCoreClient->configManager, &Globals::doWeQuit, StreamType::None);
		}
		theGuild->voiceConnectionPtr = Globals::voiceConnectionMap[guildId].get();
		return theGuild->voiceConnectionPtr;
	}

	SongAPI* DiscordCoreClient::getSongAPI(Snowflake guildId) {
		if (!Globals::songAPIMap.contains(guildId)) {
			Globals::songAPIMap[guildId] = std::make_unique<SongAPI>(guildId);
		}
		return Globals::songAPIMap[guildId].get();
	}

	void atexitHandler() {
		Globals::doWeQuit.store(true);
	}

	SIGTERMError::SIGTERMError(std::string theString) : std::runtime_error(theString){};

	SIGSEGVError::SIGSEGVError(std::string theString) : std::runtime_error(theString){};

	SIGINTError::SIGINTError(std::string theString) : std::runtime_error(theString){};

	SIGILLError::SIGILLError(std::string theString) : std::runtime_error(theString){};

	SIGABRTError::SIGABRTError(std::string theString) : std::runtime_error(theString){};

	SIGFPEError::SIGFPEError(std::string theString) : std::runtime_error(theString){};

	void signalHandler(int32_t theValue) {
		try {
			switch (theValue) {
				case SIGTERM: {
					throw SIGTERMError{ "Exiting for: SIGTERM.\n" };
				}
				case SIGSEGV: {
					throw SIGSEGVError{ "Exiting for: SIGSEGV.\n" };
				}
				case SIGINT: {
					throw SIGINTError{ "Exiting for: SIGINT.\n" };
				}
				case SIGILL: {
					throw SIGILLError{ "Exiting for: SIGILL.\n" };
				}
				case SIGABRT: {
					throw SIGABRTError{ "Exiting for: SIGABRT.\n" };
				}
				case SIGFPE: {
					throw SIGFPEError{ "Exiting for: SIGFPE.\n" };
				}
			}
		} catch (SIGINTError&) {
			reportException("signalHandler()");
			std::exit(EXIT_SUCCESS);
		} catch (...) {
			reportException("signalHandler()");
		}
		std::exit(EXIT_FAILURE);
	}

	DiscordCoreClient::DiscordCoreClient(DiscordCoreClientConfig configData) {
		std::atexit(&atexitHandler);
		std::signal(SIGTERM, &signalHandler);
		std::signal(SIGSEGV, &signalHandler);
		std::signal(SIGINT, &signalHandler);
		std::signal(SIGILL, &signalHandler);
		std::signal(SIGABRT, &signalHandler);
		std::signal(SIGFPE, &signalHandler);
		if (!DiscordCoreInternal::SSLConnectionInterface::initialize()) {
			if (this->configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "Failed to initialize the SSL_CTX structure!" << reset() << endl << endl;
			}
			return;
		}
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
			return;
		}
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
		this->didWeStartCorrectly = true;
	}

	void DiscordCoreClient::registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction, CreateApplicationCommandData commandData,
		bool alwaysRegister) {
		commandData.alwaysRegister = alwaysRegister;
		this->commandController.registerFunction(functionNames, std::move(baseFunction));
		this->commandsToRegister.emplace_back(commandData);
	}

	void DiscordCoreClient::registerFunctionsInternal() {
		std::vector<ApplicationCommand> theCommands{};
		try {
			theCommands = ApplicationCommands::getGlobalApplicationCommandsAsync({ .withLocalizations = false, .applicationId = this->getBotUser().id }).get();
		} catch (...) {
			reportException("DiscordCoreClient::registerFunctionsInternal()");
			return;
		}
		while (this->commandsToRegister.size() > 0) {
			CreateApplicationCommandData theData = this->commandsToRegister.front();
			this->commandsToRegister.pop_front();
			theData.applicationId = this->getBotUser().id;
			if (theData.alwaysRegister) {
				if (theData.guildId != 0) {
					ApplicationCommands::createGuildApplicationCommandAsync(*static_cast<CreateGuildApplicationCommandData*>(&theData)).get();
				} else {
					ApplicationCommands::createGlobalApplicationCommandAsync(*static_cast<CreateGlobalApplicationCommandData*>(&theData)).get();
				}
			} else {
				std::vector<ApplicationCommand> theGuildCommands{};
				if (theData.guildId != 0) {
					theGuildCommands =
						ApplicationCommands::getGuildApplicationCommandsAsync({ .withLocalizations = false, .applicationId = this->getBotUser().id, .guildId = theData.guildId })
							.get();
				}
				bool doesItExist{ false };
				for (auto& value: theCommands) {
					if (value.name == theData.name) {
						doesItExist = true;
					}
				}
				for (auto& value: theGuildCommands) {
					if (value.name == theData.name) {
						doesItExist = true;
					}
				}
				if (!doesItExist) {
					if (theData.guildId != 0) {
						ApplicationCommands::createGuildApplicationCommandAsync(*static_cast<CreateGuildApplicationCommandData*>(&theData)).get();
					} else {
						ApplicationCommands::createGlobalApplicationCommandAsync(*static_cast<CreateGlobalApplicationCommandData*>(&theData)).get();
					}
				}
			}
		}
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
		if (!this->didWeStartCorrectly) {
			return;
		}
		if (!this->instantiateWebSockets()) {
			Globals::doWeQuit.store(true);
			return;
		}
		while (!Globals::doWeQuit.load()) {
			if (this->theConnections.size() > 0 && this->theConnectionStopWatch.hasTimePassed()) {
				this->theConnectionStopWatch.resetTimer();
				auto theData = this->theConnections.front();
				this->theConnections.pop_front();
				this->baseSocketAgentMap[theData.currentShard % this->baseSocketAgentMap.size()]->connect(theData);
				if (this->theConnections.size() == 0) {
					if (this->configManager.doWePrintGeneralSuccessMessages()) {
						cout << shiftToBrightGreen() << "All of the shards are connected for the current process!" << reset() << endl << endl;
					}
					this->registerFunctionsInternal();
				}
			}
			std::this_thread::sleep_for(1ms);
		}
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
			reportException("getGateWatBot()");
			return theData;
		}
		return theData;
	}

	bool DiscordCoreClient::instantiateWebSockets() {
		GatewayBotData gatewayData{};
		try {
			gatewayData = this->getGateWayBot();
		} catch (...) {
			reportException("DiscordCoreClient::instantiateWebSockets()");
		}

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
		uint32_t theWorkerCount =
			this->configManager.getTotalShardCount() <= std::thread::hardware_concurrency() ? this->configManager.getTotalShardCount() : std::thread::hardware_concurrency();
		if (this->configManager.getConnectionAddress() == "") {
			this->configManager.setConnectionAddress(gatewayData.url.substr(gatewayData.url.find("wss://") + std::string("wss://").size()));
		}
		if (this->configManager.getConnectionPort() == "") {
			this->configManager.setConnectionPort("443");
		}
		for (uint32_t x = 0; x < this->configManager.getTotalShardCount(); ++x) {
			if (!this->baseSocketAgentMap.contains(x % theWorkerCount)) {
				this->baseSocketAgentMap[x % theWorkerCount] = std::make_unique<DiscordCoreInternal::BaseSocketAgent>(this, &Globals::doWeQuit, x % theWorkerCount);
			}
			ConnectionPackage theData{};
			theData.currentShard = x;
			theData.currentReconnectTries = 0;
			this->baseSocketAgentMap[x % theWorkerCount]->theShardMap[x] =
				std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(this, &this->theConnections, x, &Globals::doWeQuit);
			this->theConnections.emplace_back(theData);
		}
		try {
			this->currentUser = BotUser{ Users::getCurrentUserAsync().get(), this->baseSocketAgentMap[this->configManager.getStartingShard()].get() };
		} catch (...) {
			reportException("DiscordCoreClient::instantiateWebSockets()");
		}

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
		return true;
	}

	DiscordCoreClient::~DiscordCoreClient() noexcept {
		for (auto& [key, value]: CoRoutineBase::threadPool.workerThreads) {
			if (value.theThread.joinable()) {
				value.theThread.request_stop();
				value.theThread.join();
			}
		}
	}
}
