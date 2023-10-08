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
/// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
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

namespace DiscordCoreAPI {

	class SIGTERMError : public DCAException {
	  public:
		inline SIGTERMError(jsonifier::string_view message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
	};

	class SIGSEGVError : public DCAException {
	  public:
		inline SIGSEGVError(jsonifier::string_view message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
	};

	class SIGINTError : public DCAException {
	  public:
		inline SIGINTError(jsonifier::string_view message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
	};

	class SIGILLError : public DCAException {
	  public:
		inline SIGILLError(jsonifier::string_view message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
	};

	class SIGABRTError : public DCAException {
	  public:
		inline SIGABRTError(jsonifier::string_view message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
	};

	class SIGFPEError : public DCAException {
	  public:
		inline SIGFPEError(jsonifier::string_view message, std::source_location location = std::source_location::current()) : DCAException{ message, location } {};
	};

	using SoundCloudAPIMap = UnorderedMap<uint64_t, UniquePtr<DiscordCoreInternal::SoundCloudAPI>>;

	using YouTubeAPIMap = UnorderedMap<uint64_t, UniquePtr<DiscordCoreInternal::YouTubeAPI>>;

	using VoiceConnectionsMap = UnorderedMap<uint64_t, UniquePtr<VoiceConnection>>;

	using SongAPIMap = UnorderedMap<uint64_t, UniquePtr<SongAPI>>;

	template<typename... ArgTypes> using TimeElapsedHandler = std::function<void(ArgTypes...)>;

	template<typename... ArgTypes>
	inline static CoRoutine<void, false> threadFunction(TimeElapsedHandler<ArgTypes...> timeElapsedHandler, bool repeated, int64_t timeInterval, ArgTypes... args) {
		auto threadHandle = co_await NewThreadAwaitable<void, false>();
		StopWatch<Milliseconds> stopWatch{ Milliseconds{ timeInterval } };
		stopWatch.reset();
		do {
			std::this_thread::sleep_for(Milliseconds{ static_cast<int64_t>(std::ceil(static_cast<float>(timeInterval) * 10.0f / 100.0f)) });
			while (!stopWatch.hasTimeElapsed() && !threadHandle.promise().stopRequested()) {
				std::this_thread::sleep_for(1ms);
			}
			stopWatch.reset();
			if (threadHandle.promise().stopRequested()) {
				co_return;
			}
			try {
				timeElapsedHandler(args...);
			} catch (const DCAException& error) {
				MessagePrinter::printError<PrintMessageType::General>(error.what());
			}
			if (threadHandle.promise().stopRequested()) {
				co_return;
			}
			std::this_thread::sleep_for(1ms);
		} while (repeated);
		co_return;
	};

	template<typename... ArgTypes> inline static void executeFunctionAfterTimePeriod(TimeElapsedHandler<ArgTypes...> timeElapsedHandler, int64_t timeDelay, bool repeated,
		bool blockForCompletion, ArgTypes... args) {
		auto newThread = threadFunction<ArgTypes...>(timeElapsedHandler, repeated, timeDelay, args...);
		if (blockForCompletion) {
			newThread.get();
		}
	}

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief DiscordCoreClient - The main class for this library.
	class DiscordCoreAPI_Dll DiscordCoreClient {
	  public:
		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreInternal::WebSocketCore;
		friend class VoiceConnection;
		friend class GuildData;
		friend class BotUser;
		friend class Guilds;

		static DiscordCoreInternal::SoundCloudAPI& getSoundCloudAPI(Snowflake guildId);

		static DiscordCoreInternal::YouTubeAPI& getYouTubeAPI(Snowflake guildId);

		static VoiceConnection& getVoiceConnection(Snowflake guildId);

		static SongAPI& getSongAPI(Snowflake guildId);

		static DiscordCoreClient* getInstance();

		/// @brief DiscordCoreClient constructor.
		/// @param configData A DiscordCoreClientConfig structure to select various library options.
		DiscordCoreClient(const DiscordCoreClientConfig& configData);

		/// @brief For registering a function with the CommandController.
		/// @param functionNames A vector containing the possible names for activating this command/function.
		/// @param baseFunction A unique_ptr to the command to be registered.
		/// @param commandData A CreateApplicationCommandData structure describing the current function.
		/// @param alwaysRegister Whether or not it gets registered every time the bot boots up, or only when it's missing from the bot's list of
		/// registered commands.
		void registerFunction(const jsonifier::vector<jsonifier::string>& functionNames, UniquePtr<BaseFunction> baseFunction, CreateApplicationCommandData commandData,
			bool alwaysRegister = false);

		/// @brief For collecting a reference to the CommandController.
		/// @return CommandController& A reference to the CommandController.
		CommandController& getCommandController();

		/// @brief For collecting a reference to the ConfigManager.
		/// @return ConfigManager& A reference to the ConfigManager.
		const ConfigManager& getConfigManager() const;

		/// @brief For collecting a reference to the EventManager.
		/// @return EventManager& A reference to the EventManager.
		EventManager& getEventManager();

		/// @brief For collecting, the total time in milliseconds that this bot has been up for.
		/// @return Milliseconds A size, in milliseconds, since the bot has come online.
		Milliseconds getTotalUpTime();

		/// @brief For collecting a copy of the current bot's UserData.
		/// @return BotUser An instance of BotUser.
		static BotUser getBotUser();

		/// @brief Executes the library, and waits for completion.
		void runBot();

		~DiscordCoreClient();

	  protected:
		inline static UniquePtr<DiscordCoreClient> instancePtr{};
		static BotUser currentUser;

		DiscordCoreClient& operator=(DiscordCoreClient&&) = delete;
		DiscordCoreClient(DiscordCoreClient&&)			  = delete;

		DiscordCoreClient& operator=(const DiscordCoreClient&) = delete;
		DiscordCoreClient(const DiscordCoreClient&)			   = delete;

		UnorderedMap<uint64_t, UniquePtr<DiscordCoreInternal::BaseSocketAgent>> baseSocketAgentsMap{};
		std::deque<CreateApplicationCommandData> commandsToRegister{};
		UniquePtr<DiscordCoreInternal::HttpsClient> httpsClient{};
		StopWatch<Milliseconds> connectionStopWatch01{ 5000ms };
#ifdef _WIN32
		DiscordCoreInternal::WSADataWrapper theWSAData{};
#endif
		std::atomic_uint64_t currentlyConnectingShard{};
		std::atomic_bool areWeReadyToConnect{ false };
		CommandController commandController{};
		Milliseconds startupTimeSinceEpoch{};
		ConfigManager configManager{};
		EventManager eventManager{};///< An event-manager, for hooking into Discord-API-Events sent over the Websockets.

		bool areWeFullyConnected();

		void registerFunctionsInternal();

		GatewayBotData getGateWayBot();

		bool instantiateWebSockets();
	};
	/**@}*/
}// namespace DiscordCoreAPI