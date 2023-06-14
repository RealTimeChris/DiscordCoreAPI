/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
		if (!Globals::soundCloudAPIMap.contains(guildId.operator uint64_t())) {
			Globals::soundCloudAPIMap[guildId.operator uint64_t()] =
				std::make_unique<DiscordCoreInternal::SoundCloudAPI>(guild.discordCoreClient, guild.discordCoreClient->httpsClient.get(), guildId);
		}
		return Globals::soundCloudAPIMap[guildId.operator uint64_t()].get();
	}

	DiscordCoreInternal::YouTubeAPI* DiscordCoreClient::getYouTubeAPI(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!Globals::youtubeAPIMap.contains(guildId.operator uint64_t())) {
			Globals::youtubeAPIMap[guildId.operator uint64_t()] =
				std::make_unique<DiscordCoreInternal::YouTubeAPI>(guild.discordCoreClient, guild.discordCoreClient->httpsClient.get(), guildId);
		}
		return Globals::youtubeAPIMap[guildId.operator uint64_t()].get();
	}

	VoiceConnection* DiscordCoreClient::getVoiceConnection(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!Globals::voiceConnectionMap.contains(guildId.operator uint64_t())) {
			uint64_t theShardId{ (guildId.operator uint64_t() >> 22) % guild.discordCoreClient->configManager.getTotalShardCount() };
			uint64_t baseSocketIndex{ theShardId % guild.discordCoreClient->baseSocketAgentsMap.size() };
			auto baseSocketAgent = guild.discordCoreClient->baseSocketAgentsMap[baseSocketIndex].get();
			Globals::voiceConnectionMap[guildId.operator uint64_t()] = std::make_unique<VoiceConnection>(guild.discordCoreClient,
				static_cast<DiscordCoreInternal::WebSocketClient*>(baseSocketAgent->shardMap[theShardId].get()), &Globals::doWeQuit);
		}
		guild.voiceConnection = Globals::voiceConnectionMap[guildId.operator uint64_t()].get();
		return guild.voiceConnection;
	}

	SongAPI* DiscordCoreClient::getSongAPI(Snowflake guildId) {
		if (!Globals::songAPIMap.contains(guildId.operator uint64_t())) {
			Globals::songAPIMap[guildId.operator uint64_t()] = std::make_unique<SongAPI>(guildId);
		}
		return Globals::songAPIMap[guildId.operator uint64_t()].get();
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
		configManager = ConfigManager{ configData };
		if (!DiscordCoreInternal::SSLConnectionInterface::initialize()) {
			if (configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "Failed to initialize the SSL_CTX structure!" << reset() << endl << endl;
			}
			return;
		}
		if (sodium_init() == -1) {
			if (configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "LibSodium failed to initialize!" << reset() << endl << endl;
			}
			return;
		}
		httpsClient = std::make_unique<DiscordCoreInternal::HttpsClient>(&configManager);
		ApplicationCommands::initialize(httpsClient.get());
		AutoModerationRules::initialize(httpsClient.get());
		Channels::initialize(httpsClient.get(), &configManager);
		Guilds::initialize(httpsClient.get(), this, &configManager);
		GuildMembers::initialize(httpsClient.get(), &configManager);
		GuildScheduledEvents::initialize(httpsClient.get());
		Interactions::initialize(httpsClient.get());
		Messages::initialize(httpsClient.get());
		Reactions::initialize(httpsClient.get());
		Roles::initialize(httpsClient.get(), &configManager);
		Stickers::initialize(httpsClient.get());
		StageInstances::initialize(httpsClient.get());
		Threads::initialize(httpsClient.get());
		WebHooks::initialize(httpsClient.get());
		Users::initialize(httpsClient.get(), &configManager);
		didWeStartCorrectly = true;
	}

	ConfigManager& DiscordCoreClient::getConfigManager() {
		return configManager;
	}

	BotUser DiscordCoreClient::getBotUser() {
		return DiscordCoreClient::currentUser;
	}

	void DiscordCoreClient::runBot() {
		if (!didWeStartCorrectly) {
			return;
		}
		if (!instantiateWebSockets()) {
			Globals::doWeQuit.store(true);
			return;
		}
		while (!Globals::doWeQuit.load()) {
			std::this_thread::sleep_for(1ms);
		}
	}


	void DiscordCoreClient::registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction,
		CreateApplicationCommandData commandData, bool alwaysRegister) {
		commandData.alwaysRegister = alwaysRegister;
		commandController.registerFunction(functionNames, std::move(baseFunction));
		commandsToRegister.emplace_back(commandData);
	}

	CommandController& DiscordCoreClient::getCommandController() {
		return commandController;
	}

	EventManager& DiscordCoreClient::getEventManager() {
		return eventManager;
	}

	Milliseconds DiscordCoreClient::getTotalUpTime() {
		return std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch()) - startupTimeSinceEpoch;
	}

	void DiscordCoreClient::registerFunctionsInternal() {
		std::vector<ApplicationCommand> theCommands{
			ApplicationCommands::getGlobalApplicationCommandsAsync({ .applicationId = getBotUser().id, .withLocalizations = false }).get()
		};
		while (commandsToRegister.size() > 0) {
			CreateApplicationCommandData data = commandsToRegister.front();
			commandsToRegister.pop_front();
			data.applicationId = getBotUser().id;
			if (data.alwaysRegister) {
				if (data.guildId != 0) {
					ApplicationCommands::createGuildApplicationCommandAsync(*static_cast<CreateGuildApplicationCommandData*>(&data)).get();
				} else {
					ApplicationCommands::createGlobalApplicationCommandAsync(*static_cast<CreateGlobalApplicationCommandData*>(&data)).get();
				}
			} else {
				std::vector<ApplicationCommand> guildCommands{};
				if (data.guildId != 0) {
					guildCommands = ApplicationCommands::getGuildApplicationCommandsAsync(
						{ .applicationId = getBotUser().id, .withLocalizations = false, .guildId = data.guildId })
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
						ApplicationCommands::createGuildApplicationCommandAsync(*static_cast<CreateGuildApplicationCommandData*>(&data)).get();

					} else {
						ApplicationCommands::createGlobalApplicationCommandAsync(*static_cast<CreateGlobalApplicationCommandData*>(&data)).get();
					}
				}
			}
		}
	}

	GatewayBotData DiscordCoreClient::getGateWayBot() {
		try {
			DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Gateway_Bot };
			workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
			workload.relativePath = "/gateway/bot";
			workload.callStack = "DiscordCoreClient::getGateWayBot()";
			GatewayBotData data{};
			httpsClient->submitWorkloadAndGetResult<GatewayBotData>(workload, data);
			return data;
		} catch (...) {
			if (configManager.doWePrintGeneralErrorMessages()) {
				reportException("DiscordCoreClient::getGateWayBot()");
			}
		}
		return {};
	}

	bool DiscordCoreClient::instantiateWebSockets() {
		GatewayBotData gatewayData{ getGateWayBot() };

		if (gatewayData.url == "") {
			if (configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed()
					 << "Failed to collect the connection URL! Closing! Did you remember to "
						"properly set your bot token?"
					 << reset() << endl
					 << endl;
			}
			std::this_thread::sleep_for(5s);
			return false;
		}
		if (configManager.getStartingShard() + configManager.getShardCountForThisProcess() > configManager.getTotalShardCount()) {
			if (configManager.doWePrintGeneralErrorMessages()) {
				cout << shiftToBrightRed() << "Your sharding options are incorrect! Please fix it!" << reset() << endl << endl;
			}
			std::this_thread::sleep_for(5s);
			return false;
		}
		uint32_t theWorkerCount = configManager.getTotalShardCount() <= std::thread::hardware_concurrency() ? configManager.getTotalShardCount()
																											: std::thread::hardware_concurrency();
		if (configManager.getConnectionAddress() == "") {
			configManager.setConnectionAddress(gatewayData.url.substr(gatewayData.url.find("wss://") + std::string("wss://").size()));
		}
		if (configManager.getConnectionPort() == 0) {
			configManager.setConnectionPort(443);
		}
		for (uint32_t x = 0; x < configManager.getTotalShardCount(); ++x) {
			if (!baseSocketAgentsMap.contains(x % theWorkerCount)) {
				while (!connectionStopWatch01.hasTimePassed()) {
					std::this_thread::sleep_for(1ms);
				}
				connectionStopWatch01.resetTimer();
				baseSocketAgentsMap[x % theWorkerCount] =
					std::make_unique<DiscordCoreInternal::BaseSocketAgent>(this, &Globals::doWeQuit, x % theWorkerCount);
			}
			ConnectionPackage data{};
			data.currentShard = x;
			data.currentReconnectTries = 0;
			baseSocketAgentsMap[x % theWorkerCount]->shardMap[x] =
				std::make_unique<DiscordCoreInternal::WebSocketClient>(this, data.currentShard, &Globals::doWeQuit, false);
			baseSocketAgentsMap[x % theWorkerCount]->getClient(x).connections = std::make_unique<ConnectionPackage>(data);
		}

		for (auto& value: configManager.getFunctionsToExecute()) {
			if (value.repeated) {
				TimeElapsedHandlerNoArgs onSend = [=, this]() -> void {
					value.function(this);
				};
				ThreadPool::storeThread(onSend, value.intervalInMs);
			} else {
				ThreadPool::executeFunctionAfterTimePeriod(value.function, value.intervalInMs, false, this);
			}
		}
		startupTimeSinceEpoch = std::chrono::duration_cast<Milliseconds>(SysClock::now().time_since_epoch());
		return true;
	}

	DiscordCoreClient::~DiscordCoreClient() noexcept {
		for (auto& [key, value]: NewThreadAwaiterBase::threadPool.workerThreads) {
			if (value.thread.joinable()) {
				value.thread.request_stop();
				value.thread.detach();
			}
		}
	}

	BotUser DiscordCoreClient::currentUser{};
}
