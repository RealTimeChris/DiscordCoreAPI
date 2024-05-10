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
/// DiscordCoreClient.cpp - Source file for the main/exposed discord_core_client class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file DiscordCoreClient.cpp

#include <atomic>
#include <csignal>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace discord_core_api {

	voice_connections_map voiceConnectionMap{};
	sound_cloud_api_map soundCloudAPIMap{};
	you_tube_api_map youtubeAPIMap{};
	song_api_map songAPIMap{};
	std::atomic_bool doWeQuit{};

	discord_core_internal::sound_cloud_api& discord_core_client::getSoundCloudAPI(snowflake guildId) {
		if (!soundCloudAPIMap.contains(guildId.operator const uint64_t&())) {
			soundCloudAPIMap[guildId.operator const uint64_t&()] = makeUnique<discord_core_internal::sound_cloud_api>(&getInstance()->configManager, guildId);
		}
		return *soundCloudAPIMap[guildId.operator const uint64_t&()].get();
	}

	discord_core_internal::you_tube_api& discord_core_client::getYouTubeAPI(snowflake guildId) {
		if (!youtubeAPIMap.contains(guildId.operator const uint64_t&())) {
			youtubeAPIMap[guildId.operator const uint64_t&()] = makeUnique<discord_core_internal::you_tube_api>(&getInstance()->configManager, guildId);
		}
		return *youtubeAPIMap[guildId.operator const uint64_t&()].get();
	}

	voice_connection& discord_core_client::getVoiceConnection(snowflake guildId) {
		if (!voiceConnectionMap.contains(guildId.operator const uint64_t&())) {
			uint64_t theShardId{ (guildId.operator const uint64_t&() >> 22) % getInstance()->configManager.getTotalShardCount() };
			uint64_t baseSocketIndex{ theShardId % getInstance()->baseSocketAgentsMap.size() };
			voiceConnectionMap[guildId.operator const uint64_t&()] =
				makeUnique<voice_connection>(&getInstance()->baseSocketAgentsMap[baseSocketIndex]->shardMap[theShardId], &doWeQuit);
		}
		return *voiceConnectionMap[guildId.operator const uint64_t&()].get();
	}

	song_api& discord_core_client::getSongAPI(snowflake guildId) {
		if (!songAPIMap.contains(guildId.operator const uint64_t&())) {
			songAPIMap[guildId.operator const uint64_t&()] = makeUnique<song_api>(guildId);
		}
		return *songAPIMap[guildId.operator const uint64_t&()].get();
	}

	discord_core_client* discord_core_client::getInstance() {
		return instancePtr.get();
		;
	}

	void atexitHandler() noexcept {
		doWeQuit.store(true, std::memory_order_release);
	}

	void signalHandler(int32_t value) noexcept {
		switch (value) {
			case SIGTERM: {
				message_printer::printError<print_message_type::general>("SIGTERM ERROR.");
				exit(EXIT_FAILURE);
			}
			case SIGSEGV: {
				message_printer::printError<print_message_type::general>("SIGSEGV ERROR.");
				exit(EXIT_FAILURE);
			}
			case SIGINT: {
				message_printer::printError<print_message_type::general>("SIGINT ERROR.");
				exit(EXIT_SUCCESS);
			}
			case SIGILL: {
				message_printer::printError<print_message_type::general>("SIGILL ERROR.");
				exit(EXIT_FAILURE);
			}
			case SIGABRT: {
				message_printer::printError<print_message_type::general>("SIGABRT ERROR.");
				exit(EXIT_FAILURE);
			}
			case SIGFPE: {
				message_printer::printError<print_message_type::general>("SIGFPE ERROR.");
				exit(EXIT_FAILURE);
			}
		}
	}

	discord_core_client::discord_core_client(const discord_core_client_config& configData) : configManager{ configData } {
		instancePtr.reset(this);
		std::atexit(&atexitHandler);
		std::signal(SIGTERM, &signalHandler);
		std::signal(SIGSEGV, &signalHandler);
		std::signal(SIGINT, &signalHandler);
		std::signal(SIGILL, &signalHandler);
		std::signal(SIGABRT, &signalHandler);
		std::signal(SIGFPE, &signalHandler);
		message_printer::initialize(configManager);
		if (!discord_core_internal::ssl_context_holder::initialize()) {
			message_printer::printError<print_message_type::general>("Failed to initialize the SSL_CTX structure!");
			return;
		}
		if (sodium_init() == -1) {
			message_printer::printError<print_message_type::general>("Lib_sodium failed to initialize!");
			return;
		}
		httpsClient = makeUnique<discord_core_internal::https_client>(jsonifier::string{ configManager.getBotToken() });
		application_commands::initialize(httpsClient.get());
		auto_moderation_rules::initialize(httpsClient.get());
		channels::initialize(httpsClient.get(), &configManager);
		guilds::initialize(httpsClient.get(), &configManager);
		guild_members::initialize(httpsClient.get(), &configManager);
		guild_scheduled_events::initialize(httpsClient.get());
		interactions::initialize(httpsClient.get());
		messages::initialize(httpsClient.get());
		reactions::initialize(httpsClient.get());
		roles::initialize(httpsClient.get(), &configManager);
		stickers::initialize(httpsClient.get());
		stage_instances::initialize(httpsClient.get());
		threads::initialize(httpsClient.get());
		web_hooks::initialize(httpsClient.get());
		users::initialize(httpsClient.get(), &configManager);
	}

	const config_manager& discord_core_client::getConfigManager() const {
		return configManager;
	}

	bot_user discord_core_client::getBotUser() {
		return discord_core_client::currentUser;
	}

	void discord_core_client::runBot() {
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
		} catch (const dca_exception& error) {
			message_printer::printError<print_message_type::general>(error.what());
		}
	}

	void discord_core_client::registerFunction(const jsonifier::vector<jsonifier::string>& functionNames, unique_ptr<base_function>&& baseFunction,
		const create_application_command_data& commandDataNew, bool alwaysRegister) {
		create_application_command_data commandData{ commandDataNew };
		commandData.alwaysRegister = alwaysRegister;
		commandController.registerFunction(functionNames, std::move(baseFunction));
		commandsToRegister.emplace_back(commandData);
	}

	command_controller& discord_core_client::getCommandController() {
		return commandController;
	}

	event_manager& discord_core_client::getEventManager() {
		return eventManager;
	}

	milliseconds discord_core_client::getTotalUpTime() {
		return std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch()) - startupTimeSinceEpoch;
	}

	void discord_core_client::registerFunctionsInternal() {
		if (getBotUser().id != 0) {
			jsonifier::vector<application_command_data> theCommands{
				application_commands::getGlobalApplicationCommandsAsync({ .applicationId = getBotUser().id, .withLocalizations = false }).get()
			};
			while (commandsToRegister.size() > 0) {
				create_application_command_data data = commandsToRegister.front();
				commandsToRegister.pop_front();
				data.applicationId = getBotUser().id;
				if (data.alwaysRegister) {
					if (data.guildId != 0) {
						application_commands::createGuildApplicationCommandAsync(*static_cast<create_guild_application_command_data*>(&data)).get();
					} else {
						application_commands::createGlobalApplicationCommandAsync(*static_cast<create_global_application_command_data*>(&data)).get();
					}
				} else {
					jsonifier::vector<application_command_data> guildCommands{};
					if (data.guildId != 0) {
						guildCommands =
							application_commands::getGuildApplicationCommandsAsync({ .applicationId = getBotUser().id, .withLocalizations = false, .guildId = data.guildId }).get();
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
								application_commands::createGuildApplicationCommandAsync(*static_cast<create_guild_application_command_data*>(&data)).get();

							} else {
								application_commands::createGlobalApplicationCommandAsync(*static_cast<create_global_application_command_data*>(&data)).get();
							}
						}
					} catch (dca_exception& error) {
						message_printer::printError<print_message_type::https>(error.what());
					}
				}
			}
		}
	}

	gateway_bot_data discord_core_client::getGateWayBot() {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Gateway_Bot };
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/gateway/bot";
		workload.callStack	   = "discord_core_client::getGateWayBot()";
		gateway_bot_data data{};
		httpsClient->submitWorkloadAndGetResult(workload, data);
		return data;
	}

	bool discord_core_client::instantiateWebSockets() {
		gateway_bot_data gatewayData{};
		try {
			gatewayData = getGateWayBot();
		} catch (const discord_core_internal::https_error& error) {
			message_printer::printError<print_message_type::general>(error.what());
			return false;
		}

		if (gatewayData.url == "") {
			message_printer::printError<print_message_type::general>("Failed to collect the connection url! closing! did you remember to "
																	 "properly set your bot token?");
			std::this_thread::sleep_for(5s);
			return false;
		}
		if (configManager.getStartingShard() + configManager.getShardCountForThisProcess() > configManager.getTotalShardCount()) {
			message_printer::printError<print_message_type::general>("your sharding options are incorrect! please fix it!");
			std::this_thread::sleep_for(5s);
			return false;
		}
		uint64_t workerCount = configManager.getTotalShardCount() <= std::jthread::hardware_concurrency() ? configManager.getTotalShardCount()
																										  : static_cast<uint64_t>(std::jthread::hardware_concurrency());

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
				baseSocketAgentsMap[x] = makeUnique<discord_core_internal::base_socket_agent>(&doWeQuit);
				baseSocketAgentsMap[x]->shardMap.reserve(configManager.getTotalShardCount() / workerCount);
			}
			baseSocketAgentsMap[x % workerCount]->shardMap[x] = discord_core_internal::websocket_client{ x, &doWeQuit };
		}
		areWeReadyToConnect.store(true, std::memory_order_release);
		while (!areWeFullyConnected()) {
			std::this_thread::sleep_for(1ms);
		}
		for (auto& value: configManager.getFunctionsToExecute()) {
			executeFunctionAfterTimePeriod(value.function, value.intervalInMs, value.repeated, false, this);
		}
		startupTimeSinceEpoch = std::chrono::duration_cast<milliseconds>(sys_clock::now().time_since_epoch());
		return true;
	}

	bool discord_core_client::areWeFullyConnected() {
		for (auto& [key, value]: baseSocketAgentsMap) {
			for (auto& [keyNew, valueNew]: value->shardMap) {
				if (!valueNew.areWeConnected()) {
					return false;
				}
			}
		}
		return true;
	}

	discord_core_client::~discord_core_client() {
		instancePtr.release();
	}

	bot_user discord_core_client::currentUser{};
}// namespace discord_core_api
