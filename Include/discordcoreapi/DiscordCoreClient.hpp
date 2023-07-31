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
/// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
/// May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file DiscordCoreClient.hpp

#pragma once

#include <discordcoreapi/Utilities/WebSocketClient.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/SongAPI.hpp>
#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/StageInstanceEntities.hpp>
#include <discordcoreapi/StickerEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/YouTubeAPI.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>

namespace DiscordCoreAPI {

	class SIGTERMError : public DCAException {
	  public:
		inline SIGTERMError(const std::string& message, std::source_location location = std::source_location::current())
			: DCAException{ message, location } {};
	};

	class SIGSEGVError : public DCAException {
	  public:
		inline SIGSEGVError(const std::string& message, std::source_location location = std::source_location::current())
			: DCAException{ message, location } {};
	};

	class SIGINTError : public DCAException {
	  public:
		inline SIGINTError(const std::string& message, std::source_location location = std::source_location::current())
			: DCAException{ message, location } {};
	};

	class SIGILLError : public DCAException {
	  public:
		inline SIGILLError(const std::string& message, std::source_location location = std::source_location::current())
			: DCAException{ message, location } {};
	};

	class SIGABRTError : public DCAException {
	  public:
		inline SIGABRTError(const std::string& message, std::source_location location = std::source_location::current())
			: DCAException{ message, location } {};
	};

	class SIGFPEError : public DCAException {
	  public:
		inline SIGFPEError(const std::string& message, std::source_location location = std::source_location::current())
			: DCAException{ message, location } {};
	};

	using SoundCloudAPIMap = UnorderedMap<uint64_t, UniquePtr<DiscordCoreInternal::SoundCloudAPI>>;

	using YouTubeAPIMap = UnorderedMap<uint64_t, UniquePtr<DiscordCoreInternal::YouTubeAPI>>;

	using VoiceConnectionsMap = UnorderedMap<uint64_t, UniquePtr<VoiceConnection>>;

	using SongAPIMap = UnorderedMap<uint64_t, UniquePtr<SongAPI>>;

	template<typename... ArgTypes> using TimeElapsedHandler = std::function<void(ArgTypes...)>;

	template<typename... ArgTypes> inline static CoRoutine<void, false> threadFunction(TimeElapsedHandler<ArgTypes...> timeElapsedHandler,
		bool repeated, int64_t timeInterval, ArgTypes... args) {
		auto threadHandle = co_await NewThreadAwaitable<void, false>();
		StopWatch<std::chrono::milliseconds> stopWatch{ Milliseconds{ timeInterval } };
		do {
			stopWatch.resetTimer();
			std::this_thread::sleep_for(Milliseconds{ static_cast<int64_t>(std::ceil(static_cast<double>(timeInterval) * 10.0f / 100.0f)) });
			while (!stopWatch.hasTimePassed() && !threadHandle.promise().areWeStopped()) {
				std::this_thread::sleep_for(1ms);
			}
			if (threadHandle.promise().areWeStopped()) {
				co_return;
			}
			try {
				timeElapsedHandler(args...);
			} catch (const DCAException& error) {
				MessagePrinter::printError<PrintMessageType::General>(error.what());
			}
			if (threadHandle.promise().areWeStopped()) {
				co_return;
			}
			std::this_thread::sleep_for(1ms);
		} while (repeated);
		co_return;
	};

	template<typename... ArgTypes> inline static void executeFunctionAfterTimePeriod(TimeElapsedHandler<ArgTypes...> timeElapsedHandler,
		int64_t timeDelay, bool repeated, bool blockForCompletion, ArgTypes... args) {
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
		/// \param configData A DiscordCoreClientConfig structure to select various library options.
		DiscordCoreClient(DiscordCoreClientConfig configData);

		/// @brief For registering a function with the CommandController.
		/// \param functionNames A vector containing the possible names for activating this command/function.
		/// \param baseFunction A unique_ptr to the command to be registered.
		/// \param commandData A CreateApplicationCommandData structure describing the current function.
		/// \param alwaysRegister Whether or not it gets registered every time the bot boots up, or only when it's missing from the bot's list of registered commands.
		void registerFunction(const std::vector<std::string>& functionNames, UniquePtr<BaseFunction> baseFunction,
			CreateApplicationCommandData commandData, bool alwaysRegister = false);

		/// @brief For collecting a reference to the CommandController.
		/// @return CommandController& A reference to the CommandController.
		CommandController& getCommandController();

		/// @brief For collecting a reference to the ConfigManager.
		/// @return ConfigManager& A reference to the ConfigManager.
		ConfigManager& getConfigManager();

		/// @brief For collecting a reference to the EventManager.
		/// @return EventManager& A reference to the EventManager.
		EventManager& getEventManager();

		/// @brief For collecting, the total time in milliseconds that this bot has been up for.
		/// @return Milliseconds A count, in milliseconds, since the bot has come online.
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
		DiscordCoreClient(DiscordCoreClient&&) = delete;

		DiscordCoreClient& operator=(const DiscordCoreClient&) = delete;
		DiscordCoreClient(const DiscordCoreClient&) = delete;

		UnorderedMap<uint64_t, UniquePtr<DiscordCoreInternal::BaseSocketAgent>> baseSocketAgentsMap{};
		StopWatch<std::chrono::milliseconds> connectionStopWatch01{ 5000ms };
		std::deque<CreateApplicationCommandData> commandsToRegister{};
		UniquePtr<DiscordCoreInternal::HttpsClient> httpsClient{};
#ifdef _WIN32
		DiscordCoreInternal::WSADataWrapper theWSAData{};
#endif
		std::counting_semaphore<1> theSemaphore{ 1 };
		CommandController commandController{ this };
		Milliseconds startupTimeSinceEpoch{};
		ConfigManager configManager{};
		std::mutex connectionMutex{};
		EventManager eventManager{};///< An event-manager, for hooking into Discord-API-Events sent over the Websockets.

		void registerFunctionsInternal();

		GatewayBotData getGateWayBot();

		bool instantiateWebSockets();
	};
	/**@}*/
}
