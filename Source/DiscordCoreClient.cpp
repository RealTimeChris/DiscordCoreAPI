/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++
   CoRoutines.

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

#include <atomic>
#include <csignal>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	VoiceConnectionsMap voiceConnectionMap{};
	SoundCloudAPIMap soundCloudAPIMap{};
	YouTubeAPIMap youtubeAPIMap{};
	SongAPIMap songAPIMap{};
	std::atomic_bool doWeQuit{};

	DiscordCoreInternal::SoundCloudAPI& DiscordCoreClient::getSoundCloudAPI(Snowflake guildId) {
		if (!soundCloudAPIMap.contains(guildId.operator const uint64_t&())) {
			soundCloudAPIMap[guildId.operator const uint64_t&()] = makeUnique<DiscordCoreInternal::SoundCloudAPI>(&getInstance()->configManager, guildId);
		}
		return *soundCloudAPIMap[guildId.operator const uint64_t&()].get();
	}

	DiscordCoreInternal::YouTubeAPI& DiscordCoreClient::getYouTubeAPI(Snowflake guildId) {
		if (!youtubeAPIMap.contains(guildId.operator const uint64_t&())) {
			youtubeAPIMap[guildId.operator const uint64_t&()] = makeUnique<DiscordCoreInternal::YouTubeAPI>(&getInstance()->configManager, guildId);
		}
		return *youtubeAPIMap[guildId.operator const uint64_t&()].get();
	}

	VoiceConnection& DiscordCoreClient::getVoiceConnection(Snowflake guildId) {
		if (!voiceConnectionMap.contains(guildId.operator const uint64_t&())) {
			uint64_t theShardId{ (guildId.operator const uint64_t&() >> 22) % getInstance()->configManager.getTotalShardCount() };
			uint64_t baseSocketIndex{ theShardId % getInstance()->baseSocketAgentsMap.size() };
			auto baseSocketAgent								   = getInstance()->baseSocketAgentsMap[baseSocketIndex].get();
			voiceConnectionMap[guildId.operator const uint64_t&()] = makeUnique<VoiceConnection>(&baseSocketAgent->shardMap[theShardId], &doWeQuit);
		}
		return *voiceConnectionMap[guildId.operator const uint64_t&()].get();
	}

	SongAPI& DiscordCoreClient::getSongAPI(Snowflake guildId) {
		if (!songAPIMap.contains(guildId.operator const uint64_t&())) {
			songAPIMap[guildId.operator const uint64_t&()] = makeUnique<SongAPI>(guildId);
		}
		return *songAPIMap[guildId.operator const uint64_t&()].get();
	}

	DiscordCoreClient* DiscordCoreClient::getInstance() {
		return instancePtr.get();
		;
	}

	void atexitHandler() {
		doWeQuit.store(true, std::memory_order_release);
	}

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
		} catch (const SIGINTError& error) {
			MessagePrinter::printError<PrintMessageType::General>(error.what());
			exit(EXIT_SUCCESS);
		} catch (...) {
			exit(EXIT_FAILURE);
		}
	}

	DiscordCoreClient::DiscordCoreClient(const DiscordCoreClientConfig& configData) : configManager{ configData } {
		instancePtr.reset(this);
		std::atexit(&atexitHandler);
		std::signal(SIGTERM, &signalHandler);
		std::signal(SIGSEGV, &signalHandler);
		std::signal(SIGINT, &signalHandler);
		std::signal(SIGILL, &signalHandler);
		std::signal(SIGABRT, &signalHandler);
		std::signal(SIGFPE, &signalHandler);
		MessagePrinter::initialize(configManager);
		if (!DiscordCoreInternal::SSLContextHolder::initialize()) {
			MessagePrinter::printError<PrintMessageType::General>("Failed to initialize the SSL_CTX structure!");
			return;
		}
		if (sodium_init() == -1) {
			MessagePrinter::printError<PrintMessageType::General>("LibSodium failed to initialize!");
			return;
		}
		httpsClient = makeUnique<DiscordCoreInternal::HttpsClient>(jsonifier::string{ configManager.getBotToken() });
		ApplicationCommands::initialize(httpsClient.get());
		AutoModerationRules::initialize(httpsClient.get());
		Channels::initialize(httpsClient.get(), &configManager);
		Guilds::initialize(httpsClient.get(), &configManager);
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

	const ConfigManager& DiscordCoreClient::getConfigManager() const {
		return configManager;
	}

	BotUser DiscordCoreClient::getBotUser() {
		return DiscordCoreClient::currentUser;
	}

	void DiscordCoreClient::runBot() {
		try {
			if (!instantiateWebSockets()) {
				doWeQuit.store(true, std::memory_order_release);
				return;
			}
			while (getBotUser().id == 0) {
				std::this_thread::sleep_for(1ms);
			}
			registerFunctionsInternal();
			while (!doWeQuit.load(std::memory_order_acquire)) {
				std::this_thread::sleep_for(1ms);
			}
		} catch (const DCAException& error) {
			MessagePrinter::printError<PrintMessageType::General>(error.what());
		}
	}

	void DiscordCoreClient::registerFunction(const jsonifier::vector<jsonifier::string>& functionNames, UniquePtr<BaseFunction> baseFunction, CreateApplicationCommandData commandData,
		bool alwaysRegister) {
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

		if (getBotUser().id != 0) {
			jsonifier::vector<ApplicationCommandData> theCommands{
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
					jsonifier::vector<ApplicationCommandData> guildCommands{};
					if (data.guildId != 0) {
						guildCommands =
							ApplicationCommands::getGuildApplicationCommandsAsync({ .applicationId = getBotUser().id, .withLocalizations = false, .guildId = data.guildId }).get();
					}
					bool doesItExist{};
					for (auto& value: theCommands) {
						if (value == data) {
							doesItExist = true;
							break;
						}
					}
					for (auto& value: guildCommands) {
						if (value == data) {
							doesItExist = true;
							break;
						}
					}
					try {
						if (!doesItExist) {
							if (data.guildId != 0) {
								ApplicationCommands::createGuildApplicationCommandAsync(*static_cast<CreateGuildApplicationCommandData*>(&data)).get();

							} else {
								ApplicationCommands::createGlobalApplicationCommandAsync(*static_cast<CreateGlobalApplicationCommandData*>(&data)).get();
							}
						}
					} catch (DCAException& error) {
						MessagePrinter::printError<PrintMessageType::Https>(error.what());
					}
				}
			}
		}
	}

	GatewayBotData DiscordCoreClient::getGateWayBot() {
		try {
			DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Gateway_Bot };
			workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
			workload.relativePath  = "/gateway/bot";
			workload.callStack	   = "DiscordCoreClient::getGateWayBot()";
			GatewayBotData data{};
			httpsClient->submitWorkloadAndGetResult(workload, data);
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
		uint64_t workerCount = configManager.getTotalShardCount() <= DiscordCoreInternal::ThreadWrapper::hardware_concurrency()
			? configManager.getTotalShardCount()
			: static_cast<uint64_t>(DiscordCoreInternal::ThreadWrapper::hardware_concurrency());

		if (configManager.getConnectionAddress() == "") {
			configManager.setConnectionAddress(gatewayData.url.substr(gatewayData.url.find("wss://") + jsonifier::string{ "wss://" }.size()));
		}
		if (configManager.getConnectionPort() == 0) {
			configManager.setConnectionPort(443);
		}
		areWeReadyToConnect.store(false, std::memory_order_release);
		baseSocketAgentsMap.reserve(workerCount);
		for (uint64_t x = 0; x < configManager.getTotalShardCount(); ++x) {
			if (baseSocketAgentsMap.size() < workerCount) {
				baseSocketAgentsMap[x] = makeUnique<DiscordCoreInternal::BaseSocketAgent>(&doWeQuit);
				baseSocketAgentsMap[x]->shardMap.reserve(configManager.getTotalShardCount() / workerCount);
			}
			baseSocketAgentsMap[x % workerCount]->shardMap[x] = DiscordCoreInternal::WebSocketClient{ x, &doWeQuit };
		}
		areWeReadyToConnect.store(true, std::memory_order_release);
		while (!areWeFullyConnected()) {
			std::this_thread::sleep_for(1ms);
		}
		for (auto& value: configManager.getFunctionsToExecute()) {
			executeFunctionAfterTimePeriod(value.function, value.intervalInMs, value.repeated, false, this);
		}
		startupTimeSinceEpoch = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch());
		return true;
	}

	bool DiscordCoreClient::areWeFullyConnected() {
		for (auto& [key, value]: baseSocketAgentsMap) {
			for (auto& [keyNew, valueNew]: value->shardMap) {
				if (!valueNew.areWeConnected()) {
					return false;
				}
			}
		}
		return true;
	}

	DiscordCoreClient::~DiscordCoreClient() {
		auto guildVector = Guilds::getAllGuildsAsync();
		for (auto& value: guildVector) {
			if (value.areWeConnected()) {
				value.disconnect();
			}
		}
		for (auto& [key, value]: NewThreadAwaiterBase::threadPool) {
			if (value->thread->joinable()) {
				value->thread->requestStop();
				value->thread->detach();
			}
		}
		instancePtr.release();
	}

	BotUser DiscordCoreClient::currentUser{};
}// namespace DiscordCoreAPI