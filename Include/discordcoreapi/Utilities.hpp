/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// Utilities.cpp - Header file for the Utilities.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file Utilities.hpp

#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)
#pragma warning(disable : 4996)

#ifdef _WIN32
	#ifdef DiscordCoreAPI_EXPORTS
		#define DiscordCoreAPI_Dll __declspec(dllexport)
	#else
		#define DiscordCoreAPI_Dll __declspec(dllimport)
	#endif
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef WINRT_LEAN_AND_MEAN
		#define WINRT_LEAN_AND_MEAN
	#endif
#else
	#ifndef DiscordCoreAPI_Dll
		#define DiscordCoreAPI_Dll
	#endif
	#include <pthread.h>
	#include <sys/time.h>
	#include <time.h>
	#include <ctime>
#endif

#include <condition_variable>
#include <nlohmann/json.hpp>
#include <source_location>
#include <unordered_map>
#include <shared_mutex>
#include <functional>
#include <coroutine>
#include <semaphore>
#include <concepts>
#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <map>

namespace DiscordCoreInternal {

	using namespace std::literals;

	struct HttpsWorkloadData;
	class SoundCloudRequestBuilder;
	class YouTubeRequestBuilder;
	class VoiceSocketAgent;
	class BaseSocketAgent;
	class SoundCloudAPI;
	class HttpsClient;
	class YouTubeAPI;

	enum class WebSocketOpCode : int8_t { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

	enum class WebSocketState : int8_t { Connecting01 = 0, Connecting02 = 1, Connected = 2 };

	/// Websocket close codes. \brief Websocket close codes.
	enum class WebSocketCloseCode : uint16_t {
		Normal_Close = 1000,///< Normal close.
		Unknown_Error = 4000,///< We're not sure what went wrong. Try reconnecting?
		Unknown_Opcode = 4001,///< You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
		Decode_Error = 4002,///< You sent an invalid payload to us. Don't do that!
		Not_Authenticated = 4003,///< You sent us a payload prior to identifying.
		Authentication_Failed = 4004,///< The account token sent with your identify payload is incorrect.
		Already_Authenticated = 4005,///< You sent more than one identify payload. Don't do that!
		Invalid_Seq = 4007,///<	The sequence sent when resuming the session was invalid. Reconnect and start a new session.
		Rate_Limited = 4008,///< Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
		Session_Timed = 4009,///< Your session timed out. Reconnect and start a new one.
		Invalid_Shard = 4010,///< You sent us an invalid shard when identifying.
		Sharding_Required = 4011,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.
		Invalid_API_Version = 4012,///< You sent an invalid version for the gateway.
		Invalid_Intent = 4013,///< You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
		Disallowed_Intent = 4014,///< You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
	};

}// namespace DiscordCoreInternal

namespace DiscordCoreAPI {

	using namespace std::literals;

	struct DeleteInteractionResponseData;
	struct DeleteFollowUpMessageData;
	struct OnInteractionCreationData;
	struct GetGuildMemberRolesData;
	struct BaseFunctionArguments;
	struct GetRolesData;
	struct CommandData;
	struct File;

	class CreateEphemeralInteractionResponseData;
	class CreateDeferredInteractionResponseData;
	class ApplicationCommandOptionChoiceData;
	class CreateEphemeralFollowUpMessageData;
	class CreateInteractionResponseData;
	class EditInteractionResponseData;
	class CreateFollowUpMessageData;
	class RespondToInputEventData;
	class EditFollowUpMessageData;
	class SelectMenuCollector;
	class AllowedMentionsData;
	class DiscordCoreClient;
	class CreateMessageData;
	class SelectOptionData;
	class VoiceConnection;
	class EditMessageData;
	class ButtonCollector;
	class InteractionData;
	class ModalCollector;
	class ActionRowData;
	class Interactions;
	class EventManager;
	class EventHandler;
	class GuildMembers;
	class ChannelData;
	class InputEvents;
	class EventWaiter;
	class MessageData;
	class Permissions;
	class SendDMData;
	class Reactions;
	class EmbedData;
	class Messages;
	class WebHooks;
	class SongAPI;
	class BotUser;
	class Guilds;
	class Guild;
	class Roles;
	class Test;

	template<typename ReturnType, typename... ArgTypes> class EventDelegate;
	template<typename ReturnType, typename... ArgTypes> class Event;
	template<typename ReturnType> class CoRoutine;

	std::ostream& operator<<(std::ostream& outputSttream, const std::string& (*theFunction)( void ));

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// Input event response types. \brief Input event response types.
	enum class InputEventResponseType : int8_t {
		Unset = 0,///< Unset.
		Deferred_Response = 1,
		Ephemeral_Deferred_Response = 2,///< Deferred ephemeral response.
		Interaction_Response = 3,///< Interaction response.
		Ephemeral_Interaction_Response = 4,///< Ephemeral Interaction response.
		Edit_Interaction_Response = 5,///< Interaction response edit.
		Follow_Up_Message = 6,///< Follow-up Message.
		Ephemeral_Follow_Up_Message = 7,///< Ephemeral follow-up Message.
		Edit_Follow_Up_Message = 8,///< Follow-up Message edit.
		Application_Command_AutoComplete_Result = 9,///< Respond to an autocomplete interaction with suggested choices.
		Modal_Interaction_Response = 10,///< Respond to an interaction with a popup modal.
	};

	/// Gateway intents. \brief Gateway intents.
	enum class GatewayIntents : int32_t {
		Guilds = 1 << 0,///< Intent for receipt of Guild information.
		Guild_Members = 1 << 1,///< Intent for receipt of Guild members.
		Guild_Bans = 1 << 2,///< Intent for receipt of Guild bans.
		Guild_Emojis = 1 << 3,///< Intent for receipt of Guild emojis.
		Guild_Integrations = 1 << 4,///< Intent for receipt of Guild integrations.
		Guild_Webhooks = 1 << 5,///< Intent for receipt of Guild webhooks.
		Guild_Invites = 1 << 6,///< Intent for receipt of Guild invites.
		Guild_VoiceStates = 1 << 7,///< Intent for receipt of Guild voice states.
		Guild_Presences = 1 << 8,///< Intent for receipt of Guild presences.
		Guild_Messages = 1 << 9,///< Intent for receipt of Guild messages.
		Guild_Message_Reactions = 1 << 10,///< Intent for receipt of Guild message reactions.
		Guild_Message_Typing = 1 << 11,///< Intent for receipt of Guild message typing notifications.
		Direct_Messages = 1 << 12,///< Intent for receipt of direct messages (DMs).
		Direct_Message_Reactions = 1 << 13,///< Intent for receipt of direct message reactions.
		Direct_Message_Typing = 1 << 14,///< Intent for receipt of direct message typing notifications.
		Message_Content = 1 << 15,///< Intent for receipt of message content.
		Guild_Scheduled_Events = 1 << 16,///< Scheduled events.
		Default_Intents = Guilds | Guild_Bans | Guild_Emojis | Guild_Integrations | Guild_Webhooks | Guild_Invites | Guild_VoiceStates | Guild_Messages | Guild_Message_Reactions |
			Guild_Message_Typing | Direct_Messages | Direct_Message_Reactions | Direct_Message_Typing | Guild_Scheduled_Events,///< Default intents (all non-privileged intents).
		Privileged_Intents = Guild_Members | Guild_Presences | Message_Content,///< Privileged intents requiring ID.
		All_Intents = Default_Intents | Privileged_Intents///< Every single intent.
	};

	/// Function data for repeated functions to be loaded. \brief Function data for repeated functions to be loaded.
	struct DiscordCoreAPI_Dll RepeatedFunctionData {
		std::function<void(DiscordCoreClient*)> function{ nullptr };///< The std::function pointer to be loaded.
		uint32_t intervalInMs{ 0 };///< The time interval at which to call the std::function.
		bool repeated{ false };///< Whether or not the std::function is repeating.
		int64_t dummyArg{ 0 };
	};

	/// Represents which text format to use for websocket transfer. \brief Represents which text format to use for websocket transfer.
	enum class TextFormat : int8_t {
		Etf = 0x00,///< Etf format.
		Json = 0x01///< Json format.
	};

	/// Sharding options for the library. \brief Sharding options for the library.
	struct DiscordCoreAPI_Dll ShardingOptions {
		int32_t numberOfShardsForThisProcess{ 1 };///< The number of shards to launch on the current process.
		int32_t totalNumberOfShards{ 1 };///< The total number of shards that will be launched across all processes.
		int32_t startingShard{ 0 };///< The first shard to start on this process.
	};

	/// Logging options for the library. \brief Loggin options for the library.
	struct DiscordCoreAPI_Dll LoggingOptions {
		bool logWebSocketSuccessMessages{ false };///< Do we log the websocket success messages to cout?
		bool logWebSocketErrorMessages{ false };///< Do we log the websocket error messages to cout?
		bool logGeneralSuccessMessages{ false };///< Do we log general success messages to cout?
		bool logFFMPEGSuccessMessages{ false };///< Do we log FFMPEG success messages to cout?
		bool logGeneralErrorMessages{ false };///< Do we log general error messages to cout?
		bool logHttpsSuccessMessages{ false };///< Do we log Http response success messages to cout?
		bool logFFMPEGErrorMessages{ false };///< Do we log FFMPEG error messages to cout?
		bool logHttpsErrorMessages{ false };///< Do we log Http response error messages to cout?
	};

	/// For selecting the caching style of the library. \brief For selecting the caching style of the library.
	struct DiscordCoreAPI_Dll CacheOptions {
		bool cacheGuildMembers{ true };///< Do we cache GuildMembers?
		bool cacheChannels{ true };///< Do we cache Channels?
		bool cacheGuilds{ true };///< Do we cache Guilds?
		bool cacheRoles{ true };///< Do we cache Roles?
		bool cacheUsers{ true };///< Do we cache Users?
	};

	/// Configuration data for the library's main class, DiscordCoreClient. \brief Configuration data for the library's main class, DiscordCoreClient.
	struct DiscordCoreAPI_Dll DiscordCoreClientConfig {
		GatewayIntents theIntents{ GatewayIntents::All_Intents };///< The gateway intents to be used for this instance.
		std::vector<RepeatedFunctionData> functionsToExecute{};///< Functions to execute after a timer, or on a repetition.
		TextFormat textFormat{ TextFormat::Etf };///< Use ETF or JSON format for websocket transfer?
		std::string connectionAddress{};///< A potentially alternative connection address for the websocket.
		ShardingOptions shardOptions{};///< Options for the sharding of your bot.
		std::string connectionPort{};///< A potentially alternative connection port for the websocket.
		LoggingOptions logOptions{};///< Options for the output/logging of the library.
		CacheOptions cacheOptions{};///< Options for the cache of the library.
		std::string botToken{};///< Your bot's token.
	};

	/**@}*/

	class DiscordCoreAPI_Dll ConfigManager {
	  public:
		ConfigManager() = default;

		ConfigManager(const DiscordCoreClientConfig&);

		const bool doWePrintWebSocketSuccessMessages();

		const bool doWePrintWebSocketErrorMessages();

		const bool doWePrintHttpsSuccessMessages();

		const bool doWePrintHttpsErrorMessages();

		const bool doWePrintFFMPEGSuccessMessages();

		const bool doWePrintFFMPEGErrorMessages();

		const bool doWePrintGeneralSuccessMessages();

		const bool doWePrintGeneralErrorMessages();

		const bool doWeCacheChannels();

		const bool doWeCacheUsers();

		const bool doWeCacheGuilds();

		const bool doWeCacheGuildMembers();

		const bool doWeCacheRoles();

		const std::string getBotToken();

		const int32_t getTotalShardCount();

		const int32_t getStartingShard();

		const int32_t getShardCountForThisProcess();

		const std::string getConnectionAddress();

		void setConnectionAddress(const std::string& connectionAddressNew);

		const std::string getConnectionPort();

		void setConnectionPort(const std::string& connectionPortNew);

		const std::vector<RepeatedFunctionData> getFunctionsToExecute();

		const TextFormat getTextFormat();

		const GatewayIntents getGatewayIntents();

	  protected:
		DiscordCoreClientConfig theConfig{};
	};

	template<typename ObjectType> class ReferenceCountingPtr {
	  public:
		class DiscordCoreAPI_Dll ObjectWrapper {
		  public:
			ObjectWrapper& operator=(ObjectType* other) {
				this->thePtr = other;
				return *this;
			}

			ObjectWrapper(ObjectType* other) {
				*this = other;
			}

			operator ObjectType*() {
				return this->thePtr;
			}

			ObjectWrapper() = default;

			ObjectType* get() {
				return this->thePtr;
			}

			void incrementCount() const {
				this->refCount++;
			}

			void release() const {
				assert(this->refCount > 0);
				this->refCount--;
				if (this->refCount == 0) {
					delete this;
				};
			}

			virtual ~ObjectWrapper() = default;

		  protected:
			ObjectType* thePtr{ nullptr };
			mutable int32_t refCount{ 0 };
		};

		ReferenceCountingPtr& operator=(ObjectType* ptr) {
			if (this->thePtr) {
				this->thePtr->release();
			}
			this->thePtr = new ObjectWrapper{ ptr };
			if (this->thePtr) {
				this->thePtr->incrementCount();
			}
			return *this;
		}

		ReferenceCountingPtr(ObjectType* ptr = nullptr) {
			*this = ptr;
		}

		ReferenceCountingPtr& operator=(const ReferenceCountingPtr& ptr) {
			this->thePtr = ptr.thePtr;
			return *this;
		}

		ReferenceCountingPtr(const ReferenceCountingPtr& ptr) {
			*this = ptr;
		}

		ReferenceCountingPtr(std::nullptr_t){};

		ObjectType* operator->() const {
			return this->thePtr->get();
		}

		ObjectType& operator*() const {
			return *this->thePtr->get();
		}

		ObjectType* get() const {
			return this->thePtr->get();
		}

		~ReferenceCountingPtr() {
			if (this->thePtr) {
				this->thePtr->release();
			}
		}

	  protected:
		ObjectWrapper* thePtr{ nullptr };
	};

	/**
	 * \addtogroup utilities
	 * @{
	 */

	class DiscordCoreAPI_Dll StringWrapper {
	  public:
		StringWrapper() = default;

		StringWrapper& operator=(const std::string& theString);

		StringWrapper(const std::string& theString);

		StringWrapper& operator=(const StringWrapper& other);

		StringWrapper(const StringWrapper& other);

		StringWrapper& operator=(std::string& theString);

		StringWrapper(std::string& theString);

		StringWrapper& operator=(const char* theString);

		StringWrapper(const char* theString);

		StringWrapper& operator=(StringWrapper& other);

		StringWrapper(StringWrapper& other);

		operator std::basic_string<char, std::char_traits<char>, std::allocator<char>>();

		operator nlohmann::json();

		void push_back(char theChar);

		size_t size();

		char* data();

	  protected:
		std::unique_ptr<char[]> thePtr{};
	};

	inline std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, const StringWrapper& rhs) {
		for (auto& value: static_cast<std::string>(static_cast<StringWrapper>(rhs))) {
			lhs.put(value);
		}
		return lhs;
	}

	inline std::basic_string<char, std::char_traits<char>, std::allocator<char>> operator+(std::basic_string<char, std::char_traits<char>, std::allocator<char>> lhs,
		StringWrapper rhs) {
		std::stringstream theStream{};
		theStream << lhs << rhs;
		std::string theReturnString{};
		for (uint64_t x = 0; x < theStream.str().size(); x++) {
			theReturnString.push_back(theStream.str()[x]);
		}
		return theReturnString;
	}

	inline std::basic_string<char, std::char_traits<char>, std::allocator<char>> operator+(const char* lhs, StringWrapper rhs) {
		std::stringstream theStream{};
		theStream << lhs << rhs;
		std::string theReturnString{};
		for (uint64_t x = 0; x < theStream.str().size(); x++) {
			theReturnString.push_back(theStream.str()[x]);
		}
		return theReturnString;
	}

	inline bool operator!=(StringWrapper lhs, const char* rhs) {
		for (uint64_t x = 0; x < static_cast<std::string>(rhs).size(); x++) {
			if (static_cast<std::string>(lhs)[x] != static_cast<std::string>(rhs)[x]) {
				return false;
			}
		}
		return true;
	}

	inline bool operator==(std::string& lhs, StringWrapper& rhs) {
		for (uint64_t x = 0; x < static_cast<std::string>(rhs).size(); x++) {
			if (lhs[x] != static_cast<std::string>(rhs)[x]) {
				return false;
			}
		}
		return true;
	}

	inline bool operator==(StringWrapper lhs, const char* rhs) {
		if (std::string(lhs) == std::string(rhs)) {
			return true;
		} else {
			return false;
		}
	}

	template<typename ObjectType>
	concept Copyable = std::copyable<ObjectType>;

	/// A thread-safe messaging block for data-structures. \brief A thread-safe messaging block for data-structures.
	/// \tparam ObjectType The type of object that will be sent over the message block.
	template<Copyable ObjectType> class UnboundedMessageBlock {
	  public:
		UnboundedMessageBlock<ObjectType>& operator=(UnboundedMessageBlock<ObjectType>&& other) noexcept {
			if (this != &other) {
				this->theQueue = std::move(other.theQueue);
				other.theQueue = std::queue<ObjectType>{};
			}
			return *this;
		}

		UnboundedMessageBlock(UnboundedMessageBlock<ObjectType>&& other) noexcept {
			*this = std::move(other);
		}

		UnboundedMessageBlock<ObjectType>& operator=(const UnboundedMessageBlock<ObjectType>&) = delete;

		UnboundedMessageBlock(const UnboundedMessageBlock<ObjectType>&) = delete;

		UnboundedMessageBlock<ObjectType>& operator=(UnboundedMessageBlock<ObjectType>&) = delete;

		UnboundedMessageBlock(UnboundedMessageBlock<ObjectType>&) = delete;

		UnboundedMessageBlock() = default;

		/// Sends an object of type ObjectType to the "recipient". \brief Sends an object of type ObjectType to the "recipient".
		/// \param theObject An object of ObjectType.
		void send(const ObjectType&& theObject) {
			std::lock_guard theLock{ this->accessMutex };
			this->theQueue.push(theObject);
		}

		/// Sends an object of type ObjectType to the "recipient". \brief Sends an object of type ObjectType to the "recipient".
		/// \param theObject An object of ObjectType.
		void send(ObjectType&& theObject) {
			std::lock_guard theLock{ this->accessMutex };
			this->theQueue.push(std::move(theObject));
		}

		/// Sends an object of type ObjectType to the "recipient". \brief Sends an object of type ObjectType to the "recipient".
		/// \param theObject An object of ObjectType.
		void send(const ObjectType& theObject) {
			std::lock_guard theLock{ this->accessMutex };
			this->theQueue.push(theObject);
		}

		/// Sends an object of type ObjectType to the "recipient". \brief Sends an object of type ObjectType to the "recipient".
		/// \param theObject An object of ObjectType.
		void send(ObjectType& theObject) {
			std::lock_guard theLock{ this->accessMutex };
			this->theQueue.push(theObject);
		}

		/// Clears the contents of the messaging block. \brief Clears the contents of the messaging block.
		void clearContents() {
			std::lock_guard theLock{ this->accessMutex };
			this->theQueue = std::queue<ObjectType>{};
		}

		/// Tries to receive an object of type ObjectType to be placed into a reference. \brief Tries to receive an object of type ObjectType to be placed into a reference.
		/// \param theObject A reference of type ObjectType for placing the potentially received object.
		/// \returns bool A bool, denoting whether or not we received an object.
		bool tryReceive(ObjectType& theObject) {
			std::lock_guard theLock{ this->accessMutex };
			if (this->theQueue.size() > 0) {
				theObject = std::move(this->theQueue.front());
				this->theQueue.pop();
				return true;
			} else {
				return false;
			}
		}

	  protected:
		std::queue<ObjectType> theQueue{};
		std::mutex accessMutex{};
	};

	/**@}*/

	template<typename TimeType> class StopWatch {
	  public:
		StopWatch<TimeType>& operator=(StopWatch<TimeType>&& other) noexcept {
			if (this != &other) {
				this->maxNumberOfMs.store(other.maxNumberOfMs.load());
				this->startTime.store(other.startTime.load());
			}
			return *this;
		}

		StopWatch(StopWatch<TimeType>&& other) noexcept {
			*this = std::move(other);
		}

		StopWatch<TimeType>& operator=(StopWatch<TimeType>& other) noexcept {
			if (this != &other) {
				this->maxNumberOfMs.store(other.maxNumberOfMs.load());
				this->startTime.store(other.startTime.load());
			}
			return *this;
		}

		StopWatch(StopWatch<TimeType>& other) noexcept {
			*this = other;
		}

		StopWatch() = delete;

		StopWatch(TimeType maxNumberOfMsNew) {
			this->maxNumberOfMs.store(maxNumberOfMsNew.count());
			this->startTime.store(static_cast<uint64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count()));
		}

		uint64_t totalTimePassed() {
			uint64_t currentTime = static_cast<uint64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
			uint64_t elapsedTime = currentTime - this->startTime.load();
			return elapsedTime;
		}

		bool hasTimePassed() {
			uint64_t currentTime = static_cast<uint64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
			uint64_t elapsedTime = currentTime - this->startTime.load();
			if (elapsedTime >= this->maxNumberOfMs.load()) {
				return true;
			} else {
				return false;
			}
		}

		void resetTimer(uint64_t theNewTime = 0) {
			if (theNewTime != 0) {
				this->maxNumberOfMs.store(theNewTime);
			}
			this->startTime.store(static_cast<uint64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count()));
		}

	  protected:
		std::atomic_uint64_t maxNumberOfMs{ 0 };
		std::atomic_uint64_t startTime{ 0 };
	};

	template<typename ObjectType> bool waitForTimeToPass(UnboundedMessageBlock<ObjectType>& outBuffer, ObjectType& argOne, int32_t timeInMsNew) {
		StopWatch stopWatch{ std::chrono::milliseconds{ timeInMsNew } };
		bool didTimePass{ false };
		while (!outBuffer.tryReceive(argOne)) {
			std::this_thread::sleep_for(1ms);
			if (stopWatch.hasTimePassed()) {
				didTimePass = true;
				break;
			}
		};
		return didTimePass;
	}

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// Time formatting methods. \brief Time formatting methods.
	enum class TimeFormat : char {
		LongDate = 'D',///< "20 April 2021" - Long Date
		LongDateTime = 'F',///< "Tuesday, 20 April 2021 16:20" - Long Date/Time
		LongTime = 'T',///< "16:20:30" - Long Time
		ShortDate = 'd',///< "20/04/2021" - Short Date
		ShortDateTime = 'f',///< "20 April 2021 16:20" - Short Date/Time
		ShortTime = 't',///< "16:20" - Short Time
	};

	/// Class for representing a timestamp, as well as working with time-related values. \brief Class for representing a timestamp, as well as working with time-related values.
	class DiscordCoreAPI_Dll TimeStamp {
	  public:
		operator std::string();

		TimeStamp& operator=(std::string&& originalTimeStampNew);

		TimeStamp(std::string&& originalTimeStampNew);

		TimeStamp& operator=(std::string& originalTimeStampNew);

		TimeStamp(std::string& originalTimeStampNew);

		TimeStamp& operator=(const TimeStamp& other);

		TimeStamp(const TimeStamp& other);

		TimeStamp& operator=(TimeStamp& other);

		TimeStamp(TimeStamp& other);

		TimeStamp(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second);

		TimeStamp(uint64_t timeInMs);

		static std::string getISO8601TimeStamp(const std::string& year, const std::string& month, const std::string& day, const std::string& hour, const std::string& minute,
			const std::string& second);

		/// Collects a timestamp that is a chosen number of minutes ahead of the current time. \brief Collects a timestamp that is a chosen number of minutes ahead of the current time.
		/// \param minutesToAdd An int32_t containing the number of minutes to increment the timestamp forward for.
		/// \param hoursToAdd An int32_t containing the number of hours to increment the timestamp forward for.
		/// \param daysToAdd An int32_t containing the number of days to increment the timestamp forward for.
		/// \param monthsToAdd An int32_t containing the number of months to increment the timestamp forward for.
		/// \param yearsToAdd An int32_t containing the number of years to increment the timestamp forward for.
		/// \returns std::string A string containing the new ISO8601 timestamp.
		static std::string getFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd = 0, int32_t daysToAdd = 0, int32_t monthsToAdd = 0, int32_t yearsToAdd = 0);

		/// Deduces whether or not a chosen period of time has passed, for a chosen timestamp. \brief Deduces whether or not a chosen period of time has passed, for a chosen timestamp.
		/// \param days An int64_t representing the number of days to check for.
		/// \param hours An int64_t representing the number of hours to check for.
		/// \param minutes An int64_t representing the number of minutes to check for.
		/// \returns bool A bool denoting whether or not the input period of time has elapsed since the supplied timestamp.
		bool hasTimeElapsed(uint64_t days = 0, uint64_t hours = 0, uint64_t minutes = 0);

		static std::string convertMsToDurationString(uint64_t durationInMs);

		std::string convertTimeInMsToDateTimeString(TimeFormat timeFormat);

		/// Collects a timestamp using the format TimeFormat, as a string. \brief Collects a timestamp using the format TimeFormat, as a string.
		/// \param timeFormat A TimeFormat value, for selecting the output type.
		/// \returns std::string A string containing the returned timestamp.
		std::string getDateTimeStamp(TimeFormat timeFormat);

		std::string getCurrentISO8601TimeStamp();

		uint64_t convertTimestampToMsInteger();

		/// Returns the original timestamp, from a Discord entity. \brief Returns the original timestamp, from a Discord entity.
		/// \returns std::string A string containing the returned timestamp.
		std::string getOriginalTimeStamp();

		uint64_t getTime();

	  protected:
		StringWrapper originalTimeStamp{};
		uint64_t timeStampInMs{ 0 };
		uint64_t year{ 0 };
		uint64_t month{ 0 };
		uint64_t day{ 0 };
		uint64_t hour{ 0 };
		uint64_t minute{ 0 };
		uint64_t second{ 0 };
		const uint32_t secondsInJan{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInFeb{ 28 * 24 * 60 * 60 };
		const uint32_t secondsInMar{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInApr{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInMay{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInJun{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInJul{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInAug{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInSep{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInOct{ 31 * 24 * 60 * 60 };
		const uint32_t secondsInNov{ 30 * 24 * 60 * 60 };
		const uint32_t secondsInDec{ 31 * 24 * 60 * 60 };
		const uint32_t secondsPerMinute{ 60 };
		const uint32_t secondsPerHour{ 60 * 60 };
		const uint32_t secondsPerDay{ 60 * 60 * 24 };
	};

	class GuildMember;
	class ChannelData;

	/// Permission values, for a given Channel, by Role or GuildMember. \brief Permission values, for a given Channel, by Role or GuildMember.
	enum class Permission : uint64_t {
		Create_Instant_Invite = 1ull << 0,///< Create Instant Invite.
		Kick_Members = 1ull << 1,///< Kick Members.
		Ban_Members = 1ull << 2,///< Ban Members.
		Administrator = 1ull << 3,///< Administrator.
		Manage_Channels = 1ull << 4,///< Manage Channels.
		Manage_Guild = 1ull << 5,///< Manage Guild.
		Add_Reactions = 1ull << 6,///< Add Reactions.
		View_Audit_Log = 1ull << 7,///< View Audit Log.
		Priority_Speaker = 1ull << 8,///< Priority Speaker.
		Stream = 1ull << 9,///< Stream.
		View_Channel = 1ull << 10,///< View Channel.
		Send_Messages = 1ull << 11,///< Send Messages.
		Send_Tts_Messages = 1ull << 12,///< Send TTS Messages.
		Manage_Messages = 1ull << 13,///< Manage Messages.
		Embed_Links = 1ull << 14,///< Embed Links.
		Attach_Files = 1ull << 15,///< Attach Files.
		Read_Message_History = 1ull << 16,///< Read Message History.
		Mention_Everyone = 1ull << 17,///< Mention Everyone.
		Use_External_Emojis = 1ull << 18,///< Use External Emoji.
		View_Guild_Insights = 1ull << 19,///< View Guild Insights.
		Connect = 1ull << 20,///< Connect.
		Speak = 1ull << 21,///< Speak.
		Mute_Members = 1ull << 22,///< Mute Members.
		Deafen_Members = 1ull << 23,///< Deafen Members.
		Move_Members = 1ull << 24,///< Move Members.
		Use_Vad = 1ull << 25,///< Use VAD.
		Change_Nickname = 1ull << 26,///< Change Nickname.
		Manage_Nicknames = 1ull << 27,///< Manage Nicknames.
		Manage_Roles = 1ull << 28,///< Manage Roles.
		Manage_Webhooks = 1ull << 29,///< Manage Webhooks.
		Manage_Emojis_And_Stickers = 1ull << 30,///< Manage Emojis And Stickers.
		Use_Application_Commands = 1ull << 31,///< Use Application Commands.
		Request_To_Speak = 1ull << 32,///< Request To Speak.
		Manage_Events = 1ull << 33,///< Manage Events.
		Manage_Threads = 1ull << 34,///< Manage Threads.
		Create_Public_Threads = 1ull << 35,///< Create Public Threads.
		Create_Private_Threads = 1ull << 36,///< Create Private Threads.
		Use_External_Stickers = 1ull << 37,///< Use External Stickers.
		Send_Messages_In_Threads = 1ull << 38,///< Send Messages In Threads.
		Start_Embedded_Activities = 1ull << 39,///< Start Embedded Activities.
		Moderate_Members = 1ull << 40///< Moderate Members.
	};

	/// Permissions class, for representing and manipulating Permission values. \brief Permissions class, for representing and manipulating Permission values.
	class DiscordCoreAPI_Dll Permissions : public StringWrapper {
	  public:
		Permissions() = default;

		Permissions& operator=(std::string&& other);

		Permissions(std::string&& permsNew);

		Permissions& operator=(std::string& other);

		Permissions(std::string& permsNew);

		operator const char*();

		/// Adds one or more Permissions to the current Permissions value. \brief Adds one or more Permissions to the current Permissions value.
		/// \param permissionsToAdd A std::vector containing the Permissions you wish to add.
		void addPermissions(const std::vector<Permission>& permissionsToAdd);

		/// Removes one or more Permissions from the current Permissions value. \brief Removes one or more Permissions from the current Permissions value.
		/// \param permissionsToRemove A std::vector containing the Permissions you wish to remove.
		void removePermissions(const std::vector<Permission>& permissionsToRemove);

		/// Displays the currently present Permissions in a std::string, and returns a std::vector with each of them stored in std::string format. \brief Displays the currently present Permissions in a std::string, and returns a std::vector with each of them stored in std::string format.
		/// \returns std::vector A std::vector full of strings of the Permissions that are in the input std::string's value.
		std::vector<std::string> displayPermissions();

		/// Returns a std::string containing ALL of the possible Permissions. \brief Returns a std::string containing ALL of the possible Permissions.
		/// \returns std::string A std::string containing all of the possible Permissions.
		static std::string getAllPermissions();

		/// Returns a std::string containing the currently held Permissions. \brief Returns a std::string containing the currently held Permissions.
		/// \returns std::string A std::string containing the current Permissions.
		std::string getCurrentPermissionString();

		/// Returns a std::string containing the currently held Permissions in a given Guild. \brief Returns a std::string containing the currently held Permissions in a given Guild.
		/// \param guildMember The GuildMember who's Permissions are to be evaluated.
		/// \returns std::string A std::string containing the current Permissions.
		static std::string getCurrentGuildPermissions(const GuildMember& guildMember);

		/// Returns a std::string containing all of a given User's Permissions for a given Channel. \brief Returns a std::string containing all of a given User's Permissions for a given Channel.
		/// \param guildMember The GuildMember who's Permissions to analyze.
		/// \param channel The Channel withint which to check for Permissions.
		/// \returns std::string A std::string containing the final Permission's value for a given Channel.
		static std::string getCurrentChannelPermissions(const GuildMember& guildMember, ChannelData& channel);

		/// Checks for a given Permission in a chosen Channel, for a specific User. \brief Checks for a given Permission in a chosen Channel, for a specific User.
		/// \param guildMember The GuildMember who to check the Permissions of.
		/// \param channel The Channel within which to check for the Permission's presence.
		/// \param permission A Permission to check the current Channel for.
		/// \returns bool A bool suggesting the presence of the chosen Permission.
		bool checkForPermission(const GuildMember& guildMember, ChannelData& channel, Permission permission);

	  protected:
		static std::string computeBasePermissions(const GuildMember& guildMember);

		static std::string computeOverwrites(const std::string& basePermissions, const GuildMember& guildMember, ChannelData& channel);

		static std::string computePermissions(const GuildMember& guildMember, ChannelData& channel);
	};

	/// Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block. \brief Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block.
	/// \param currentFunctionName A std::string to display the current function's name.
	/// \param theLocation For deriving the current file, line, and column - do not set this value.
	DiscordCoreAPI_Dll void reportException(const std::string& currentFunctionName, std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll std::string constructMultiPartData(nlohmann::json theData, const std::vector<File>& files);

	DiscordCoreAPI_Dll std::string convertToLowerCase(const std::string& stringToConvert);

	DiscordCoreAPI_Dll std::string base64Encode(const std::string&, bool = false);

	DiscordCoreAPI_Dll std::string loadFileContents(const std::string& filePath);

	DiscordCoreAPI_Dll std::string utf8MakeValid(const std::string& inputString);

	DiscordCoreAPI_Dll std::string urlEncode(const std::string& inputString);

	DiscordCoreAPI_Dll void spinLock(int64_t timeInNsToSpinLockFor);

	DiscordCoreAPI_Dll std::string generateBase64EncodedKey();

	DiscordCoreAPI_Dll std::string shiftToBrightGreen();

	DiscordCoreAPI_Dll std::string shiftToBrightBlue();

	DiscordCoreAPI_Dll std::string shiftToBrightRed();

	DiscordCoreAPI_Dll bool nanoSleep(int64_t ns);

	DiscordCoreAPI_Dll std::string reset();

	/// Acquires a timestamp with the current time and date - suitable for use in message-embeds. \brief Acquires a timestamp with the current time and date - suitable for use in message-embeds.
	/// \returns std::string A string containing the current date-time stamp.
	DiscordCoreAPI_Dll std::string getTimeAndDate();

	template<typename StoredAsType, typename FlagType> StoredAsType setBool(StoredAsType inputFlag, FlagType theFlag, bool enabled) {
		if (enabled) {
			inputFlag |= static_cast<StoredAsType>(theFlag);
			return inputFlag;
		} else {
			inputFlag &= ~static_cast<StoredAsType>(theFlag);
			return inputFlag;
		}
	}

	template<typename StoredAsType, typename FlagType> bool getBool(StoredAsType inputFlag, FlagType theFlag) {
		return static_cast<StoredAsType>(inputFlag) & static_cast<StoredAsType>(theFlag);
	}

};
