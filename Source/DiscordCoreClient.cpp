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
		SongAPIMap songAPIMap{};
		VoiceConnectionsMap voiceConnectionMap{};
		SoundCloudAPIMap soundCloudAPIMap{};
		YouTubeAPIMap youtubeAPIMap{};
		std::atomic_bool doWeQuit{};
	}

	DiscordCoreInternal::SoundCloudAPI* DiscordCoreClient::getSoundCloudAPI(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!Globals::soundCloudAPIMap.contains(guildId.operator size_t())) {
			Globals::soundCloudAPIMap[guildId.operator size_t()] = std::make_unique<DiscordCoreInternal::SoundCloudAPI>(
				&guild.discordCoreClient->configManager, guild.discordCoreClient->httpsClient.get(), guildId);
		}
		return Globals::soundCloudAPIMap[guildId.operator size_t()].get();
	}

	DiscordCoreInternal::YouTubeAPI* DiscordCoreClient::getYouTubeAPI(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!Globals::youtubeAPIMap.contains(guildId.operator size_t())) {
			Globals::youtubeAPIMap[guildId.operator size_t()] = std::make_unique<DiscordCoreInternal::YouTubeAPI>(
				&guild.discordCoreClient->configManager, guild.discordCoreClient->httpsClient.get(), guildId);
		}
		return Globals::youtubeAPIMap[guildId.operator size_t()].get();
	}

	VoiceConnection* DiscordCoreClient::getVoiceConnection(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!Globals::voiceConnectionMap.contains(guildId.operator size_t())) {
			uint64_t theShardId{ (guildId.operator size_t() >> 22) % guild.discordCoreClient->configManager.getTotalShardCount() };
			uint64_t baseSocketIndex{ theShardId % guild.discordCoreClient->baseSocketAgentsMap.size() };
			auto baseSocketAgent = guild.discordCoreClient->baseSocketAgentsMap[baseSocketIndex].get();
			Globals::voiceConnectionMap[guildId.operator size_t()] =
				std::make_unique<VoiceConnection>(guild.discordCoreClient, baseSocketAgent->shardMap[theShardId].get(), &Globals::doWeQuit);
		}
		guild.voiceConnectionPtr = Globals::voiceConnectionMap[guildId.operator size_t()].get();
		return guild.voiceConnectionPtr;
	}

	SongAPI* DiscordCoreClient::getSongAPI(Snowflake guildId) {
		if (!Globals::songAPIMap.contains(guildId.operator size_t())) {
			Globals::songAPIMap[guildId.operator size_t()] = std::make_unique<SongAPI>(guildId);
		}
		return Globals::songAPIMap[guildId.operator size_t()].get();
	}

	void atexitHandler() {
		Globals::doWeQuit.store(true);
	}

	SIGTERMError::SIGTERMError(const std::string& string) : DCAException(string){};

	SIGSEGVError::SIGSEGVError(const std::string& string) : DCAException(string){};

	SIGINTError::SIGINTError(const std::string& string) : DCAException(string){};

	SIGILLError::SIGILLError(const std::string& string) : DCAException(string){};

	SIGABRTError::SIGABRTError(const std::string& string) : DCAException(string){};

	SIGFPEError::SIGFPEError(const std::string& string) : DCAException(string){};

	void signalHandler(int32_t value) {
		try {
			switch (value) {
				case SIGTERM: {
					throw SIGTERMError{ "Exiting for: SIGTERM." };
				}
				case SIGSEGV: {
					throw SIGSEGVError{ "Exiting for: SIGSEGV." };
				}
				case SIGINT: {
					throw SIGINTError{ "Exiting for: SIGINT." };
				}
				case SIGILL: {
					throw SIGILLError{ "Exiting for: SIGILL." };
				}
				case SIGABRT: {
					throw SIGABRTError{ "Exiting for: SIGABRT." };
				}
				case SIGFPE: {
					throw SIGFPEError{ "Exiting for: SIGFPE." };
				}
			}
		} catch (SIGINTError&) {
			reportException("signalHandler()");
			Globals::doWeQuit.store(true);
		} catch (...) {
			reportException("signalHandler()");
			std::exit(EXIT_FAILURE);
		}
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
		if (!DiscordCoreInternal::SSLConnectionInterface::initialize()) {
			if (this->configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "Failed to initialize the SSL_CTX structure!" << reset() << endl << endl;
			}
			return;
		}
		if (this->configManager.doWePrintFFMPEGSuccessMessages()) {
			av_log_set_level(AV_LOG_INFO);
		} else if (this->configManager.doWePrintFFMPEGErrorMessages()) {
			av_log_set_level(AV_LOG_ERROR);
		} else if (!this->configManager.doWePrintFFMPEGErrorMessages() && !this->configManager.doWePrintFFMPEGSuccessMessages()) {
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

	void DiscordCoreClient::registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction,
		CreateApplicationCommandData commandData, bool alwaysRegister) {
		commandData.alwaysRegister = alwaysRegister;
		this->commandController.registerFunction(functionNames, std::move(baseFunction));
		this->commandsToRegister.emplace_back(commandData);
	}

	CommandController& DiscordCoreClient::getCommandController() {
		return this->commandController;
	}

	ConfigManager& DiscordCoreClient::getConfigManager() {
		return this->configManager;
	}

	EventManager& DiscordCoreClient::getEventManager() {
		return this->eventManager;
	}

	Milliseconds DiscordCoreClient::getTotalUpTime() {
		return std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()) - this->startupTimeSinceEpoch;
	}

	BotUser DiscordCoreClient::getBotUser() {
		return DiscordCoreClient::currentUser;
	}

	void DiscordCoreClient::runBot() {
		if (!this->didWeStartCorrectly) {
			return;
		}
		if (!this->instantiateWebSockets()) {
			Globals::doWeQuit.store(true);
			return;
		}
		this->registerFunctionsInternal();
		while (!Globals::doWeQuit.load()) {
			std::this_thread::sleep_for(1ms);
		}
	}

	void DiscordCoreClient::registerFunctionsInternal() {
		std::vector<ApplicationCommand> theCommands{ ApplicationCommands::getGlobalApplicationCommandsAsync(
			{ .applicationId = this->getBotUser().id, .withLocalizations = false })
														 .get() };
		while (this->commandsToRegister.size() > 0) {
			CreateApplicationCommandData data = this->commandsToRegister.front();
			this->commandsToRegister.pop_front();
			data.applicationId = this->getBotUser().id;
			if (data.alwaysRegister) {
				if (data.guildId != 0) {
					ApplicationCommands::createGuildApplicationCommandAsync(*static_cast<CreateGuildApplicationCommandData*>(&data)).get();
				} else {
					ApplicationCommands::createGlobalApplicationCommandAsync(*static_cast<CreateGlobalApplicationCommandData*>(&data))
						.get();
				}
			} else {
				std::vector<ApplicationCommand> guildCommands{};
				if (data.guildId != 0) {
					guildCommands = ApplicationCommands::getGuildApplicationCommandsAsync(
						{ .applicationId = this->getBotUser().id, .withLocalizations = false, .guildId = data.guildId })
										.get();
				}
				bool doesItExist{};
				for (auto& value: theCommands) {
					if (*static_cast<ApplicationCommandData*>(&value) == *static_cast<ApplicationCommandData*>(&data)) {
						doesItExist = true;
					}
				}
				for (auto& value: guildCommands) {
					if (*static_cast<ApplicationCommandData*>(&value) == *static_cast<ApplicationCommandData*>(&data)) {
						doesItExist = true;
					}
				}
				if (!doesItExist) {
					if (data.guildId != 0) {
						ApplicationCommands::createGuildApplicationCommandAsync(*static_cast<CreateGuildApplicationCommandData*>(&data))
							.get();

					} else {
						ApplicationCommands::createGlobalApplicationCommandAsync(*static_cast<CreateGlobalApplicationCommandData*>(&data))
							.get();
					}
				}
			}
		}
	}

	GatewayBotData DiscordCoreClient::getGateWayBot() {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Gateway_Bot };
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/gateway/bot";
		workload.callStack = "DiscordCoreClient::getGateWayBot()";
		GatewayBotData data{};
		return this->httpsClient->submitWorkloadAndGetResult<GatewayBotData>(workload, &data);
	}

	bool DiscordCoreClient::instantiateWebSockets() {
		GatewayBotData gatewayData{ this->getGateWayBot() };

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
		if (this->configManager.getStartingShard() + this->configManager.getShardCountForThisProcess() >
			this->configManager.getTotalShardCount()) {
			if (this->configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "Your sharding options are incorrect! Please fix it!" << reset() << endl << endl;
			}
			std::this_thread::sleep_for(5s);
			return false;
		}
		uint32_t theWorkerCount = this->configManager.getTotalShardCount() <= std::thread::hardware_concurrency()
			? this->configManager.getTotalShardCount()
			: std::thread::hardware_concurrency();
		if (this->configManager.getConnectionAddress() == "") {
			this->configManager.setConnectionAddress(gatewayData.url.substr(gatewayData.url.find("wss://") + std::string("wss://").size()));
		}
		if (this->configManager.getConnectionPort() == 0) {
			this->configManager.setConnectionPort(443);
		}
		for (uint32_t x = 0; x < this->configManager.getTotalShardCount(); ++x) {
			if (!this->baseSocketAgentsMap.contains(x % theWorkerCount)) {
				while (!this->connectionStopWatch01.hasTimePassed()) {
					std::this_thread::sleep_for(1ms);
				}
				this->connectionStopWatch01.resetTimer();
				this->baseSocketAgentsMap[x % theWorkerCount] =
					std::make_unique<DiscordCoreInternal::BaseSocketAgent>(this, &Globals::doWeQuit, x % theWorkerCount);
			}
			ConnectionPackage data{};
			data.currentShard = x;
			data.currentReconnectTries = 0;
			this->baseSocketAgentsMap[x % theWorkerCount]->shardMap[x] =
				std::make_unique<DiscordCoreInternal::WebSocketClient>(this, data.currentShard, &Globals::doWeQuit);
			this->baseSocketAgentsMap[x % theWorkerCount]->shardMap[x]->connections = std::make_unique<ConnectionPackage>(data);
		}
		DiscordCoreClient::currentUser =
			BotUser{ Users::getCurrentUserAsync().get(), this->baseSocketAgentsMap[this->configManager.getStartingShard()].get() };

		for (auto& value: this->configManager.getFunctionsToExecute()) {
			if (value.repeated) {
				TimeElapsedHandlerNoArgs onSend = [=, this]() -> void {
					value.function(this);
				};
				ThreadPool::storeThread(onSend, value.intervalInMs);
			} else {
				ThreadPool::executeFunctionAfterTimePeriod(value.function, value.intervalInMs, false, this);
			}
		}
		this->startupTimeSinceEpoch = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch());
		return true;
	}

	DiscordCoreClient::~DiscordCoreClient() noexcept {
		for (auto& value: Guilds::getCache()) {
			Guild guild = value;
			if (guild.areWeConnected()) {
				guild.disconnect();
			}
		}
		for (auto& [key, value]: NewThreadAwaiterBase::threadPool.workerThreads) {
			if (value.thread.joinable()) {
				value.thread.request_stop();
				value.thread.join();
			}
		}
	}

	BotUser DiscordCoreClient::currentUser{};
}
