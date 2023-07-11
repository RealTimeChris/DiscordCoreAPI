/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// DiscordCoreClient01.cpp - Source file for the main/exposed DiscordCoreClient class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file DiscordCoreClient.cpp

#include <discordcoreapi/DiscordCoreClient.hpp>
#include <csignal>
#include <atomic>

namespace DiscordCoreAPI {

	VoiceConnectionsMap voiceConnectionMap{};
	SoundCloudAPIMap soundCloudAPIMap{};
	YouTubeAPIMap youtubeAPIMap{};
	SongAPIMap songAPIMap{};
	std::atomic_bool doWeQuit{};

	DiscordCoreInternal::SoundCloudAPI& DiscordCoreClient::getSoundCloudAPI(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!soundCloudAPIMap.contains(guildId.operator uint64_t())) {
			soundCloudAPIMap[guildId.operator uint64_t()] =
				makeUnique<DiscordCoreInternal::SoundCloudAPI>(&guild.discordCoreClient->configManager, guildId);
		}
		return *soundCloudAPIMap[guildId.operator uint64_t()].get();
	}

	DiscordCoreInternal::YouTubeAPI& DiscordCoreClient::getYouTubeAPI(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!youtubeAPIMap.contains(guildId.operator uint64_t())) {
			youtubeAPIMap[guildId.operator uint64_t()] =
				makeUnique<DiscordCoreInternal::YouTubeAPI>(&guild.discordCoreClient->configManager, guildId);
		}
		return *youtubeAPIMap[guildId.operator uint64_t()].get();
	}

	VoiceConnection& DiscordCoreClient::getVoiceConnection(Snowflake guildId) {
		GuildData guild = Guilds::getCachedGuild({ .guildId = guildId });
		if (!voiceConnectionMap.contains(guildId.operator uint64_t())) {
			uint64_t theShardId{ (guildId.operator uint64_t() >> 22) % guild.discordCoreClient->configManager.getTotalShardCount() };
			uint64_t baseSocketIndex{ theShardId % guild.discordCoreClient->baseSocketAgentsMap.size() };
			auto baseSocketAgent = guild.discordCoreClient->baseSocketAgentsMap[baseSocketIndex].get();
			voiceConnectionMap[guildId.operator uint64_t()] = makeUnique<VoiceConnection>(guild.discordCoreClient,
				static_cast<DiscordCoreInternal::WebSocketClient*>(&baseSocketAgent->shardMap[theShardId]), &doWeQuit);
		}
		return *voiceConnectionMap[guildId.operator uint64_t()].get();
	}

	SongAPI& DiscordCoreClient::getSongAPI(Snowflake guildId) {
		if (!songAPIMap.contains(guildId.operator uint64_t())) {
			songAPIMap[guildId.operator uint64_t()] = makeUnique<SongAPI>(guildId);
		}
		return *songAPIMap[guildId.operator uint64_t()].get();
	}

	void atexitHandler() {
		doWeQuit.store(true);
	}

	SIGTERMError::SIGTERMError(const std::string& message, std::source_location location) : DCAException(message, location){};

	SIGSEGVError::SIGSEGVError(const std::string& message, std::source_location location) : DCAException(message, location){};

	SIGINTError::SIGINTError(const std::string& message, std::source_location location) : DCAException(message, location){};

	SIGILLError::SIGILLError(const std::string& message, std::source_location location) : DCAException(message, location){};

	SIGABRTError::SIGABRTError(const std::string& message, std::source_location location) : DCAException(message, location){};

	SIGFPEError::SIGFPEError(const std::string& message, std::source_location location) : DCAException(message, location){};

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
			doWeQuit.store(true);
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
		MessagePrinter::initialize(configManager);
		if (!DiscordCoreInternal::SSLContextHolder::initialize()) {
			MessagePrinter::printError<PrintMessageType::General>("Failed to initialize the SSL_CTX structure!");
			return;
		}
		if (sodium_init() == -1) {
			MessagePrinter::printError<PrintMessageType::General>("LibSodium failed to initialize!");
			return;
		}
		httpsClient = makeUnique<DiscordCoreInternal::HttpsClient>(configManager.getBotToken());
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
	}

	ConfigManager& DiscordCoreClient::getConfigManager() {
		return configManager;
	}

	BotUser DiscordCoreClient::getBotUser() {
		return DiscordCoreClient::currentUser;
	}

	void DiscordCoreClient::runBot() {
		if (!instantiateWebSockets()) {
			doWeQuit.store(true);
			return;
		}
		while (!doWeQuit.load()) {
			std::this_thread::sleep_for(1ms);
		}
	}

	void DiscordCoreClient::registerFunction(const std::vector<std::string>& functionNames, UniquePtr<BaseFunction> baseFunction,
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
		return std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()) - startupTimeSinceEpoch;
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
			httpsClient->submitWorkloadAndGetResult<GatewayBotData>(std::move(workload), data);
			return data;
		} catch (const DiscordCoreInternal::HttpsError& error) {
			MessagePrinter::printError<PrintMessageType::Https>(error.what());
		}
		return {};
	}

	bool DiscordCoreClient::instantiateWebSockets() {
		GatewayBotData gatewayData{ getGateWayBot() };

		if (gatewayData.url == "") {
			MessagePrinter::printError<PrintMessageType::General>("Failed to collect the connection URL! Closing! Did you remember to "
																  "properly set your bot token?");
			std::this_thread::sleep_for(5s);
			return false;
		}
		if (configManager.getStartingShard() + configManager.getShardCountForThisProcess() > configManager.getTotalShardCount()) {
			MessagePrinter::printError<PrintMessageType::General>("Your sharding options are incorrect! Please fix it!");
			std::this_thread::sleep_for(5s);
			return false;
		}
		int32_t theWorkerCount = configManager.getTotalShardCount() <= std::thread::hardware_concurrency() ? configManager.getTotalShardCount()
																										   : std::thread::hardware_concurrency();
		if (configManager.getConnectionAddress() == "") {
			configManager.setConnectionAddress(gatewayData.url.substr(gatewayData.url.find("wss://") + std::string_view{ "wss://" }.size()));
		}
		if (configManager.getConnectionPort() == 0) {
			configManager.setConnectionPort(443);
		}

		for (int32_t x = 0; x < configManager.getTotalShardCount(); ++x) {
			if (!baseSocketAgentsMap.contains(x % theWorkerCount)) {
				while (!connectionStopWatch01.hasTimePassed()) {
					std::this_thread::sleep_for(1ms);
				}
				connectionStopWatch01.resetTimer();
				baseSocketAgentsMap[x % theWorkerCount] = makeUnique<DiscordCoreInternal::BaseSocketAgent>(this, &doWeQuit, x % theWorkerCount);
			}
			baseSocketAgentsMap[x % theWorkerCount]->shardMap[x] = DiscordCoreInternal::WebSocketClient{ this, x, &doWeQuit };
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
		startupTimeSinceEpoch = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
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
