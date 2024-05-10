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
/// DiscordCoreClient.hpp - Header file for the main/exposed discord_core_client class.
/// May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file DiscordCoreClient.hpp
#pragma once

#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/SongAPI.hpp>
#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/Utilities/WebSocketClient.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/YouTubeAPI.hpp>

namespace discord_core_api {

	class sigtermerror : public dca_exception {
	  public:
		DCA_INLINE sigtermerror(jsonifier::string_view message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
	};

	class sigsegverror : public dca_exception {
	  public:
		DCA_INLINE sigsegverror(jsonifier::string_view message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
	};

	class siginterror : public dca_exception {
	  public:
		DCA_INLINE siginterror(jsonifier::string_view message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
	};

	class sigillerror : public dca_exception {
	  public:
		DCA_INLINE sigillerror(jsonifier::string_view message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
	};

	class sigabrterror : public dca_exception {
	  public:
		DCA_INLINE sigabrterror(jsonifier::string_view message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
	};

	class sigfpeerror : public dca_exception {
	  public:
		DCA_INLINE sigfpeerror(jsonifier::string_view message, std::source_location location = std::source_location::current()) : dca_exception{ message, location } {};
	};

	using sound_cloud_api_map = unordered_map<uint64_t, unique_ptr<discord_core_internal::sound_cloud_api>>;

	using you_tube_api_map = unordered_map<uint64_t, unique_ptr<discord_core_internal::you_tube_api>>;

	using voice_connections_map = unordered_map<uint64_t, unique_ptr<voice_connection>>;

	using song_api_map = unordered_map<uint64_t, unique_ptr<song_api>>;

	template<typename... arg_types> using time_elapsed_handler = std::function<void(arg_types...)>;

	template<typename... arg_types, typename function_type>
	DCA_INLINE static co_routine<void, false> threadFunction(function_type timeElapsedHandler, bool repeated, int64_t timeInterval, arg_types... args) {
		auto threadHandle = co_await newThreadAwaitable<void, false>();
		stop_watch<milliseconds> stopWatch{ milliseconds{ timeInterval } };
		stopWatch.reset();
		do {
			std::this_thread::sleep_for(milliseconds{ static_cast<int64_t>(std::ceil(static_cast<float>(timeInterval) * 10.0f / 100.0f)) });
			while (!stopWatch.hasTimeElapsed() && !threadHandle.promise().stopRequested()) {
				std::this_thread::sleep_for(1ms);
			}
			stopWatch.reset();
			if (threadHandle.promise().stopRequested()) {
				co_return;
			}
			try {
				timeElapsedHandler(args...);
			} catch (const dca_exception& error) {
				message_printer::printError<print_message_type::general>(error.what());
			}
			if (threadHandle.promise().stopRequested()) {
				co_return;
			}
			std::this_thread::sleep_for(1ms);
		} while (repeated);
		co_return;
	};

	template<typename... arg_types, typename function_type> DCA_INLINE static void executeFunctionAfterTimePeriod(function_type timeElapsedHandler,
		int64_t timeDelay, bool repeated, bool blockForCompletion, arg_types... args) {
		auto newThread = threadFunction(timeElapsedHandler, repeated, timeDelay, args...);
		if (blockForCompletion) {
			newThread.get();
		}
	}

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief discord_core_client - the main class for this library.
	class DiscordCoreAPI_Dll discord_core_client {
	  public:
		friend class discord_core_internal::websocket_client;
		friend class discord_core_internal::base_socket_agent;
		friend class discord_core_internal::websocket_core;
		friend class voice_connection;
		friend class guild_data;
		friend class bot_user;
		friend class guilds;

		static discord_core_internal::sound_cloud_api& getSoundCloudAPI(snowflake guildId);

		static discord_core_internal::you_tube_api& getYouTubeAPI(snowflake guildId);

		static voice_connection& getVoiceConnection(snowflake guildId);

		static song_api& getSongAPI(snowflake guildId);

		static discord_core_client* getInstance();

		/// @brief discord_core_client constructor.
		/// @param configData a discord_core_client_config structure to select various library options.
		discord_core_client(const discord_core_client_config& configData);

		/// @brief For registering a function with the command_controller.
		/// @param functionNames a vector containing the possible names for activating this command/function.
		/// @param baseFunction a unique_ptr to the command to be registered.
		/// @param commandData a create_application_command_data structure describing the current function.
		/// @param alwaysRegister whether or not it gets registered every time the bot boots up, or only when it's missing from the bot's list of
		/// registered commands.
		void registerFunction(const jsonifier::vector<jsonifier::string>& functionNames, unique_ptr<base_function>&& baseFunction,
			const create_application_command_data& commandData, bool alwaysRegister = false);

		/// @brief For collecting a reference to the command_controller.
		/// @return command_controller& a reference to the command_controller.
		command_controller& getCommandController();

		/// @brief For collecting a reference to the config_manager.
		/// @return config_manager& a reference to the config_manager.
		const config_manager& getConfigManager() const;

		/// @brief For collecting a reference to the event_manager.
		/// @return event_manager& a reference to the event_manager.
		event_manager& getEventManager();

		/// @brief For collecting, the total time in milliseconds that this bot has been up for.
		/// @return milliseconds a size, in milliseconds, since the bot has come online.
		milliseconds getTotalUpTime();

		/// @brief For collecting a copy of the current bot's user_data.
		/// @return bot_user an instance of bot_user.
		static bot_user getBotUser();

		/// @brief Executes the library, and waits for completion.
		void runBot();

		~discord_core_client();

	  protected:
		DCA_INLINE static unique_ptr<discord_core_client> instancePtr{};
		static bot_user currentUser;

		discord_core_client& operator=(discord_core_client&&) = delete;
		discord_core_client(discord_core_client&&)			  = delete;

		discord_core_client& operator=(const discord_core_client&) = delete;
		discord_core_client(const discord_core_client&)			   = delete;

		unordered_map<uint64_t, unique_ptr<discord_core_internal::base_socket_agent>> baseSocketAgentsMap{};
		std::deque<create_application_command_data> commandsToRegister{};
		unique_ptr<discord_core_internal::https_client> httpsClient{};
		stop_watch<milliseconds> connectionStopWatch01{ 5000ms };
#if defined(_WIN32)
		discord_core_internal::wsadata_wrapper theWSAData{};
#endif
		std::atomic_uint64_t currentlyConnectingShard{};
		std::atomic_bool areWeReadyToConnect{ false };
		command_controller commandController{};
		milliseconds startupTimeSinceEpoch{};
		config_manager configManager{};
		event_manager eventManager{};///< An event-manager, for hooking into discord-api-events sent over the websockets.

		bool areWeFullyConnected();

		void registerFunctionsInternal();

		gateway_bot_data getGateWayBot();

		bool instantiateWebSockets();
	};
	/**@}*/
}// namespace discord_core_api
