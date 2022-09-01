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
/// Utilities.cpp - Header file for the Utilities.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file Utilities.hpp

#pragma once

#pragma warning(push)
#pragma warning(disable : 4275)
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
	#include <WinSock2.h>
#else
	#ifndef DiscordCoreAPI_Dll
		#define DiscordCoreAPI_Dll
	#endif
	#include <cstdint>
	#include <cstring>
inline uint64_t ntohll(uint64_t x) {
	uint8_t data[8]{};
	memcpy(&data, &(x), sizeof(x));
	uint64_t theValue{};
	for (uint32_t y = 0; y < sizeof(uint64_t); ++y) {
		theValue |= static_cast<uint64_t>(data[y]) << 8 * (sizeof(uint64_t) - y - 1);
	}
	return theValue;
}
	#include <arpa/inet.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <time.h>
	#include <ctime>
#endif

#include <condition_variable>
#include <nlohmann/json.hpp>
#include <source_location>
#include <shared_mutex>
#include <functional>
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

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

/**
 * \defgroup main_endpoints Main Endpoints
 * \brief For all of the Discord API's endpoints.
 */

/**
 * \defgroup voice_connection Voice Connection
 * \brief For all of the voice connection related stuff.
 */

/**
 * \defgroup discord_events Discord Events
 * \brief For all of the events that could be sent by the Discord API.
 */

/**
 * \defgroup utilities Utilities
 * \brief For utility classes/functions.
 */

/**
 * \defgroup foundation_entities Foundation Entities
 * \brief For all of the building blocks of the main endpoints.
 */

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	struct ActivityData;
	/// For selecting the type of streamer that the given bot is, one must be one server and one of client per connection. \brief For selecting the type of streamer that the given bot is, one must be one server and one of client per connection.
	enum class StreamType { None = 0, Client = 1, Server = 2 };

	/// For connecting two bots to stream the VC contents between the two. \brief For connecting two bots to stream the VC contents between the two.
	struct StreamInfo {
		std::string address{};///< The address to connect to.
		std::string port{};///< The port to connect to.
	};

};

namespace DiscordCoreInternal {

	using namespace std::literals;
	using std::cout;
	using std::endl;

	class HttpsWorkloadData;
	class SoundCloudRequestBuilder;
	class YouTubeRequestBuilder;
	class WebSocketSSLShard;
	class BaseSocketAgent;
	class SoundCloudAPI;
	class HttpsClient;
	class YouTubeAPI;

	template<typename T> inline auto utCast(T x) -> std::enable_if_t<std::is_enum_v<T>, std::underlying_type_t<T>> {
		return static_cast<std::underlying_type_t<T>>(x);
	}

	enum class WebSocketOpCode : int8_t { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

	/// Websocket close codes. \brief Websocket close codes.
	class DiscordCoreAPI_Dll WebSocketClose {
	  public:
		/// Websocket close codes. \brief Websocket close codes.
		enum class WebSocketCloseCode : uint16_t {
			Unset = 1 << 0,///< Unset.
			Normal_Close = 1 << 1,///< Normal close.
			Unknown_Error = 1 << 2,///< We're not sure what went wrong. Try reconnecting?
			Unknown_Opcode = 1 << 3,///< You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
			Decode_Error = 1 << 4,///< You sent an invalid payload to us. Don't do that!
			Not_Authenticated = 1 << 5,///< You sent us a payload prior to identifying.
			Authentication_Failed = 1 << 6,///< The account token sent with your identify payload is incorrect.
			Already_Authenticated = 1 << 7,///< You sent more than one identify payload. Don't do that!
			Invalid_Seq = 1 << 8,///<	The sequence sent when resuming the session was invalid. Reconnect and start a new session.
			Rate_Limited = 1 << 9,///< Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
			Session_Timed = 1 << 10,///< Your session timed out. Reconnect and start a new one.
			Invalid_Shard = 1 << 11,///< You sent us an invalid shard when identifying.
			Sharding_Required = 1 << 12,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.
			Invalid_API_Version = 1 << 13,///< You sent an invalid version for the gateway.
			Invalid_Intent = 1 << 14,///< You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
			Disallowed_Intent =
				1 << 15,///< You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
			We_Do_Reconnect = Normal_Close | Unknown_Error | Unknown_Opcode | Decode_Error | Not_Authenticated | Already_Authenticated | Invalid_Seq | Rate_Limited | Session_Timed,
			We_Do_Not_Reconnect = Authentication_Failed | Invalid_Shard | Sharding_Required | Invalid_API_Version | Invalid_Intent | Disallowed_Intent
		};

		std::unordered_map<uint16_t, WebSocketCloseCode> mappingValues{ { 0, WebSocketCloseCode::Unset }, { 1000, WebSocketCloseCode::Normal_Close },
			{ 4000, WebSocketCloseCode::Unknown_Error }, { 4001, WebSocketCloseCode::Unknown_Opcode }, { 4002, WebSocketCloseCode::Decode_Error },
			{ 4003, WebSocketCloseCode::Not_Authenticated }, { 4004, WebSocketCloseCode::Authentication_Failed }, { 4005, WebSocketCloseCode::Already_Authenticated },
			{ 4007, WebSocketCloseCode::Invalid_Seq }, { 4008, WebSocketCloseCode::Rate_Limited }, { 4009, WebSocketCloseCode::Session_Timed },
			{ 4010, WebSocketCloseCode::Invalid_Shard }, { 4011, WebSocketCloseCode::Sharding_Required }, { 4012, WebSocketCloseCode::Invalid_API_Version },
			{ 4013, WebSocketCloseCode::Invalid_Intent }, { 4014, WebSocketCloseCode::Disallowed_Intent } };

		WebSocketCloseCode theValue{};

		WebSocketClose& operator=(uint16_t theValueNew);

		explicit WebSocketClose(uint16_t theValueNew);

		operator uint16_t();

		operator bool();
	};

	/// For updating a User's presence. \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		std::vector<DiscordCoreAPI::ActivityData> activities{};///< A vector of activities.
		std::string status{};///< Current status.
		int64_t since{ 0 };///< When was the activity started?
		bool afk{ false };///< Are we afk.

		operator nlohmann::json();
	};

}// namespace DiscordCoreInternal

namespace DiscordCoreAPI {

	/// For ids of DiscordEntities. \brief For ids of DiscordEntities.
	using Snowflake = uint64_t;
	using namespace std::literals;
	using std::cout;
	using std::endl;

	struct DeleteInteractionResponseData;
	struct DeleteFollowUpMessageData;
	struct OnInteractionCreationData;
	struct GetGuildMemberRolesData;
	struct BaseFunctionArguments;
	struct GetRolesData;
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
	class GuildMemberData;
	class ModalCollector;
	class ActionRowData;
	class Interactions;
	class EventManager;
	class GuildMembers;
	class GuildMember;
	class ChannelData;
	class InputEvents;
	class MessageData;
	class Permissions;
	class CommandData;
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
	class Users;

	template<typename ReturnType, typename... ArgTypes> class EventDelegate;
	template<typename ReturnType, typename... ArgTypes> class Event;
	template<typename ReturnType> class CoRoutine;

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*theFunction)( void ));

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
		uint32_t numberOfShardsForThisProcess{ 1 };///< The number of shards to launch on the current process.
		uint32_t totalNumberOfShards{ 1 };///< The total number of shards that will be launched across all processes.
		uint32_t startingShard{ 0 };///< The first shard to start on this process.
	};

	/**@}*/

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
		DiscordCoreInternal::UpdatePresenceData presenceData{};///< Presence data to initialize your bot with.
		TextFormat textFormat{ TextFormat::Etf };///< Use ETF or JSON format for websocket transfer?
		std::string connectionAddress{};///< A potentially alternative connection address for the websocket.
		ShardingOptions shardOptions{};///< Options for the sharding of your bot.
		std::string connectionPort{};///< A potentially alternative connection port for the websocket.
		LoggingOptions logOptions{};///< Options for the output/logging of the library.
		CacheOptions cacheOptions{};///< Options for the cache of the library.
		std::string botToken{};///< Your bot's token.
	};

	class DiscordCoreAPI_Dll ConfigManager {
	  public:
		ConfigManager() noexcept = default;

		explicit ConfigManager(const DiscordCoreClientConfig&);

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

		const DiscordCoreInternal::UpdatePresenceData getPresenceData();

		const std::string getBotToken();

		const uint32_t getTotalShardCount();

		const uint32_t getStartingShard();

		const uint32_t getShardCountForThisProcess();

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

	/**
	 * \addtogroup utilities
	 * @{
	 */

	class DiscordCoreAPI_Dll StringWrapper {
	  public:
		StringWrapper() noexcept = default;

		StringWrapper& operator=(StringWrapper&& other) noexcept;

		StringWrapper(StringWrapper&& other) noexcept;

		StringWrapper& operator=(const StringWrapper& other);

		StringWrapper(const StringWrapper& other);

		StringWrapper& operator=(const std::string& theString);

		explicit StringWrapper(const std::string& theString);

		StringWrapper& operator=(const char* theString);

		StringWrapper(const char* theString);

		operator std::basic_string<char, std::char_traits<char>, std::allocator<char>>();

		operator nlohmann::json();

		void push_back(char theChar);

		size_t size();

		const char* data();

	  protected:
		std::unique_ptr<char[]> thePtr{};
	};

	inline std::basic_ostream<char>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, const StringWrapper& rhs) {
		for (auto& value: static_cast<std::string>(static_cast<StringWrapper>(rhs))) {
			lhs.put(value);
		}
		return lhs;
	}

	inline std::basic_string<char> operator+(const std::basic_string<char, std::char_traits<char>, std::allocator<char>>& lhs, StringWrapper rhs) {
		std::stringstream theStream{};
		theStream << lhs << rhs;
		return theStream.str();
	}

	inline std::basic_string<char> operator+(const char* lhs, StringWrapper rhs) {
		std::stringstream theStream{};
		theStream << lhs << rhs;
		return theStream.str();
	}

	inline bool operator==(StringWrapper lhs, const char* rhs) {
		if (std::string(lhs) == std::string(rhs)) {
			return true;
		} else {
			return false;
		}
	}

	inline bool operator!=(StringWrapper lhs, const char* rhs) {
		if (static_cast<std::string>(lhs) == rhs) {
			return false;
		}
		return true;
	}

	inline bool operator==(std::string& lhs, StringWrapper& rhs) {
		if (lhs == rhs) {
			return true;
		}
		return true;
	}

	/**@}*/

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// Audio frame types. \brief Audio frame types.
	enum class AudioFrameType : int8_t {
		Unset = 0,///< Unset.
		Encoded = 1,///< Encoded.
		RawPCM = 2,///< Raw PCM.
		Skip = 3///< Skip.
	};

	/// Represents a single frame of audio data. \brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll AudioFrameData {
		AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
		int64_t sampleCount{ -1ll };///< The number of samples per this frame.
		std::vector<uint8_t> data{};///< The audio data.
		uint64_t guildMemberId{ 0 };///< GuildMemberId for the sending GuildMember.

		AudioFrameData() noexcept = default;

		AudioFrameData& operator=(AudioFrameData&&) noexcept = default;

		AudioFrameData(AudioFrameData&&) noexcept = default;

		AudioFrameData& operator=(const AudioFrameData&) noexcept = default;

		AudioFrameData(const AudioFrameData&) noexcept = default;

		void clearData() noexcept;
	};

	/**@}*/

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

	template<typename ReturnType> DiscordCoreAPI_Dll void parseObject(nlohmann::json* jsonObjectData, ReturnType& theData);

	DiscordCoreAPI_Dll uint8_t getUint8(nlohmann::json* jsonData, const char* keyName);

	DiscordCoreAPI_Dll uint16_t getUint16(nlohmann::json* jsonData, const char* keyName);

	DiscordCoreAPI_Dll uint32_t getUint32(nlohmann::json* jsonData, const char* keyName);

	DiscordCoreAPI_Dll uint64_t getUint64(nlohmann::json* jsonData, const char* keyName);

	DiscordCoreAPI_Dll bool getBool(nlohmann::json* jsonData, const char* keyName);

	DiscordCoreAPI_Dll std::string getString(nlohmann::json* jsonData, const char* keyName);

	DiscordCoreAPI_Dll uint64_t strtoull(std::string&& theString);

	template<typename ReturnType> ReturnType fromString(const std::string& string, std::ios_base& (*type)( std::ios_base& )) {
		ReturnType theValue{};
		std::istringstream theStream(string);
		theStream >> type, theStream >> theValue;
		return theValue;
	}

	template<typename ReturnType> std::string toHex(ReturnType inputValue) {
		std::stringstream theStream{};
		theStream << std::setfill('0') << std::setw(sizeof(ReturnType) * 2) << std::hex << inputValue;
		return theStream.str();
	}

	class DiscordCoreAPI_Dll RGBColorValue {
	  public:
		uint8_t green{};
		uint8_t blue{};
		uint8_t red{};
	};

	using HexColorValue = std::string;

	class DiscordCoreAPI_Dll ColorValue {
	  public:
		ColorValue(std::string theHexColorValue);

		ColorValue(uint32_t theColorValue);

		RGBColorValue getRgbColorValue();

		HexColorValue getHexColorValue();

		uint32_t getIntColorValue();

	  protected:
		uint32_t theColor{};
	};

	enum class HashType { User_Avatar = 0, Channel_Icon = 1, GuildMember_Avatar = 2, Guild_Icon = 3, Guild_Splash = 4, Guild_Banner = 5, Guild_Discovery = 6 };

	class DiscordCoreAPI_Dll IconHash {
	  public:
		IconHash() noexcept = default;

		IconHash& operator=(const std::string theString);

		IconHash(const std::string theString) noexcept;

		bool operator==(const IconHash& other);

		std::string getIconHash() noexcept;

		~IconHash() noexcept = default;

	  protected:
		uint64_t highBits{};
		uint64_t lowBits{};
	};

	template<typename TimeType>
	/// Class for representing a timestamp, as well as working with time-related values. \brief Class for representing a timestamp, as well as working with time-related values.
	class TimeStamp {
	  public:
		explicit TimeStamp(TimeFormat theFormatNew = TimeFormat::LongDateTime) {
			this->timeStampInTimeUnits = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		TimeStamp(std::string year, std::string month, std::string day, std::string hour, std::string minute, std::string second, TimeFormat theFormatNew) {
			this->getTimeSinceEpoch(stoull(year), stoull(month), stoull(day), stoull(hour), stoull(minute), stoull(second));
		}

		operator std::string() {
			return getISO8601TimeStamp(TimeFormat::LongDateTime);
		}

		operator uint64_t() {
			return this->timeStampInTimeUnits;
		}

		TimeStamp<TimeType>& operator=(std::string&& originalTimeStampNew) {
			std::string theString = originalTimeStampNew;
			this->convertTimeStampToTimeUnits(TimeFormat::LongDateTime, theString);
			this->getISO8601TimeStamp(TimeFormat::LongDateTime);
			return *this;
		}

		explicit TimeStamp(std::string&& originalTimeStampNew) {
			*this = std::move(originalTimeStampNew);
		}

		TimeStamp<TimeType>& operator=(std::string& originalTimeStampNew) {
			std::string theString = originalTimeStampNew;
			this->convertTimeStampToTimeUnits(TimeFormat::LongDateTime, theString);
			this->getISO8601TimeStamp(TimeFormat::LongDateTime);
			return *this;
		}

		explicit TimeStamp(std::string& originalTimeStampNew) {
			*this = originalTimeStampNew;
		}

		TimeStamp<TimeType>& operator=(const TimeStamp& other) {
			this->timeStampInTimeUnits = other.timeStampInTimeUnits;
			return *this;
		}

		TimeStamp(const TimeStamp& other) {
			*this = other;
		}

		TimeStamp(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, TimeFormat theFormatNew) {
			this->getTimeSinceEpoch(year, month, day, hour, minute, second);
		};

		TimeStamp(uint64_t timeInTimeUnits, TimeFormat theFormatNew) {
			this->timeStampInTimeUnits = TimeType{ timeInTimeUnits }.count();
			this->getISO8601TimeStamp(theFormatNew);
		}

		static std::string convertToFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd, int32_t yearsToAdd,
			TimeFormat theFormatNew) {
			std::time_t result = std::time(nullptr);
			int32_t secondsPerMinute{ 60 };
			int32_t minutesPerHour{ 60 };
			int32_t secondsPerHour{ minutesPerHour * secondsPerMinute };
			int32_t hoursPerDay{ 24 };
			int32_t secondsPerDay{ secondsPerHour * hoursPerDay };
			int32_t daysPerMonth{ 30 };
			int32_t secondsPerMonth{ secondsPerDay * daysPerMonth };
			int32_t daysPerYear{ 365 };
			int32_t secondsPerYear{ secondsPerDay * daysPerYear };
			int32_t secondsToAdd =
				(yearsToAdd * secondsPerYear) + (monthsToAdd * secondsPerMonth) + (daysToAdd * secondsPerDay) + (hoursToAdd * secondsPerHour) + (minutesToAdd * secondsPerMinute);
			result += secondsToAdd;
			auto resultTwo = std::localtime(&result);
			std::string theReturnString{};
			if (resultTwo->tm_isdst) {
				if (resultTwo->tm_hour + 4 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), theFormatNew };
				theTimeStamp.getISO8601TimeStamp(theFormatNew);
				theReturnString = static_cast<std::string>(theTimeStamp);
			} else {
				if (resultTwo->tm_hour + 5 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), theFormatNew };
				theTimeStamp.getISO8601TimeStamp(theFormatNew);
				theReturnString = static_cast<std::string>(theTimeStamp);
			}
			return theReturnString;
		}

		static std::string convertToCurrentISO8601TimeStamp(TimeFormat timeFormat) {
			std::time_t result = std::time(nullptr);
			auto resultTwo = std::localtime(&result);
			std::string theReturnString{};
			if (resultTwo->tm_isdst) {
				if (resultTwo->tm_hour + 4 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), timeFormat };
				theTimeStamp.getISO8601TimeStamp(timeFormat);
				theReturnString = static_cast<std::string>(theTimeStamp);
			} else {
				if (resultTwo->tm_hour + 5 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), timeFormat };
				theTimeStamp.getISO8601TimeStamp(timeFormat);
				theReturnString = static_cast<std::string>(theTimeStamp);
			}
			return theReturnString;
		}

		bool hasTimeElapsed(uint64_t days, uint64_t hours, uint64_t minutes) {
			if (this->timeStampInTimeUnits == 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
			uint64_t startTimeRaw = this->timeStampInTimeUnits;
			auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			uint64_t secondsPerMinute = 60;
			uint64_t secondsPerHour = secondsPerMinute * 60;
			uint64_t secondsPerDay = secondsPerHour * 24;
			auto targetElapsedTime = ((days * secondsPerDay) + (hours * secondsPerHour) + (minutes * secondsPerMinute)) * 1000;
			auto actualElapsedTime = currentTime - startTimeRaw;
			if (actualElapsedTime >= targetElapsedTime) {
				return true;
			} else {
				return false;
			}
		}

		static std::string convertMsToDurationString(uint64_t durationInMs) {
			std::string newString{};
			uint64_t msPerSecond{ 1000 };
			uint64_t secondsPerMinute{ 60 };
			uint64_t minutesPerHour{ 60 };
			uint64_t msPerMinute{ msPerSecond * secondsPerMinute };
			uint64_t msPerHour{ msPerMinute * minutesPerHour };
			uint64_t hoursLeft = static_cast<uint64_t>(trunc(durationInMs / msPerHour));
			uint64_t minutesLeft = static_cast<uint64_t>(trunc((durationInMs % msPerHour) / msPerMinute));
			uint64_t secondsLeft = static_cast<uint64_t>(trunc(((durationInMs % msPerHour) % msPerMinute) / msPerSecond));
			if (hoursLeft >= 1) {
				newString += std::to_string(hoursLeft) + " Hours, ";
				newString += std::to_string(minutesLeft) + " Minutes, ";
				newString += std::to_string(secondsLeft) + " Seconds.";
			} else if (minutesLeft >= 1) {
				newString += std::to_string(minutesLeft) + " Minutes, ";
				newString += std::to_string(secondsLeft) + " Seconds.";
			} else {
				newString += std::to_string(secondsLeft) + " Seconds.";
			}
			return newString;
		}

	  protected:
		uint64_t timeStampInTimeUnits{};

		void getTimeSinceEpoch(int64_t year, int64_t month, int64_t day, int64_t hour, int64_t minute, int64_t second) {
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
			TimeType theValue{};
			for (int32_t x = 1970; x < year; ++x) {
				theValue += TimeType{ secondsInJan };
				theValue += TimeType{ secondsInFeb };
				theValue += TimeType{ secondsInMar };
				theValue += TimeType{ secondsInApr };
				theValue += TimeType{ secondsInMay };
				theValue += TimeType{ secondsInJun };
				theValue += TimeType{ secondsInJul };
				theValue += TimeType{ secondsInAug };
				theValue += TimeType{ secondsInSep };
				theValue += TimeType{ secondsInOct };
				theValue += TimeType{ secondsInNov };
				theValue += TimeType{ secondsInDec };
				if (x % 4 == 0) {
					theValue += TimeType{ secondsPerDay };
				}
			}
			if (month > 0) {
				theValue += TimeType{ static_cast<uint64_t>((day - 1) * secondsPerDay) };
				theValue += TimeType{ static_cast<uint64_t>(hour * secondsPerHour) };
				theValue += TimeType{ static_cast<uint64_t>(minute * secondsPerMinute) };
				theValue += TimeType{ second };
			}
			if (month > 1) {
				theValue += TimeType{ secondsInJan };
			}
			if (month > 2) {
				theValue += TimeType{ secondsInFeb };
			}
			if (month > 3) {
				theValue += TimeType{ secondsInMar };
			}
			if (month > 4) {
				theValue += TimeType{ secondsInApr };
			}
			if (month > 5) {
				theValue += TimeType{ secondsInMay };
			}
			if (month > 6) {
				theValue += TimeType{ secondsInJun };
			}
			if (month > 7) {
				theValue += TimeType{ secondsInJul };
			}
			if (month > 8) {
				theValue += TimeType{ secondsInAug };
			}
			if (month > 9) {
				theValue += TimeType{ secondsInSep };
			}
			if (month > 10) {
				theValue += TimeType{ secondsInOct };
			}
			if (month > 11) {
				theValue += TimeType{ secondsInNov };
			}
			this->timeStampInTimeUnits = std::chrono::duration_cast<std::chrono::milliseconds>(theValue).count() * 1000;
		}

		void convertTimeStampToTimeUnits(TimeFormat theFormatNew, std::string originalTimeStamp) {
			try {
				if (originalTimeStamp != "") {
					TimeStamp<TimeType> timeValue = TimeStamp{ stoi(originalTimeStamp.substr(0, 4)), stoi(originalTimeStamp.substr(5, 6)), stoi(originalTimeStamp.substr(8, 9)),
						stoi(originalTimeStamp.substr(11, 12)), stoi(originalTimeStamp.substr(14, 15)), stoi(originalTimeStamp.substr(17, 18)), theFormatNew };
					this->timeStampInTimeUnits = TimeType{ static_cast<uint64_t>(timeValue) }.count();
				} else {
					this->timeStampInTimeUnits = std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count();
				}
			} catch (...) {
			}
		}

		std::string getISO8601TimeStamp(TimeFormat timeFormat) {
			if (this->timeStampInTimeUnits == 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
			uint64_t timeValue = (std::chrono::duration_cast<std::chrono::milliseconds>(TimeType{ this->timeStampInTimeUnits }).count()) / 1000;
			time_t rawTime(timeValue);
			tm timeInfo = *localtime(&rawTime);
			std::string timeStamp{};
			timeStamp.resize(48);
			switch (timeFormat) {
				case TimeFormat::LongDate: {
					size_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongDateTime: {
					size_t sizeResponse = strftime(timeStamp.data(), 48, "%a %b %d %Y %X", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongTime: {
					size_t sizeResponse = strftime(timeStamp.data(), 48, "%T", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDate: {
					size_t sizeResponse = strftime(timeStamp.data(), 48, "%d/%m/%g", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDateTime: {
					size_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G %R", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortTime: {
					size_t sizeResponse = strftime(timeStamp.data(), 48, "%R", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				default: {
					break;
				}
			}
			return timeStamp;
		}
	};

	/**@}*/

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

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
	class DiscordCoreAPI_Dll Permissions {
	  public:
		Permissions() noexcept = default;

		Permissions& operator=(Permission&& other);

		Permissions(Permission&& permsNew);

		Permissions& operator=(const Permission& other);

		explicit Permissions(const Permission& permsNew);

		Permissions& operator=(std::string&& other);

		explicit Permissions(std::string&& permsNew);

		Permissions& operator=(const std::string& other);

		explicit Permissions(const std::string& permsNew);

		Permissions& operator=(const uint64_t other);

		explicit Permissions(const uint64_t permsNew);

		operator uint64_t();

		operator std::string();

		/// Returns a string containing all of a given User's Permissions for a given Channel. \brief Returns a string containing all of a given User's Permissions for a given Channel.
		/// \param guildMember The GuildMember who's Permissions to analyze.
		/// \param channel The Channel withint which to check for Permissions.
		/// \returns std::string A string containing the final Permission's value for a given Channel.
		static std::string getCurrentChannelPermissions(const GuildMember& guildMember, ChannelData& channel);

		/// Checks for a given Permission in a chosen Channel, for a specific User. \brief Checks for a given Permission in a chosen Channel, for a specific User.
		/// \param guildMember The GuildMember who to check the Permissions of.
		/// \param channel The Channel within which to check for the Permission's presence.
		/// \param permission A Permission to check the current Channel for.
		/// \returns bool A bool suggesting the presence of the chosen Permission.
		bool checkForPermission(const GuildMember& guildMember, ChannelData& channel, Permission permission);

		/// Returns a string containing the currently held Permissions in a given Guild. \brief Returns a string containing the currently held Permissions in a given Guild.
		/// \param guildMember The GuildMember who's Permissions are to be evaluated.
		/// \returns std::string A string containing the current Permissions.
		static std::string getCurrentGuildPermissions(const GuildMember& guildMember);

		/// Removes one or more Permissions from the current Permissions value. \brief Removes one or more Permissions from the current Permissions value.
		/// \param permissionsToRemove A vector containing the Permissions you wish to remove.
		void removePermissions(const std::vector<Permission>& permissionsToRemove);

		/// Adds one or more Permissions to the current Permissions value. \brief Adds one or more Permissions to the current Permissions value.
		/// \param permissionsToAdd A vector containing the Permissions you wish to add.
		void addPermissions(const std::vector<Permission>& permissionsToAdd);

		/// Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format. \brief Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format.
		/// \returns std::vector A vector full of strings of the Permissions that are in the input std::string's value.
		std::vector<std::string> displayPermissions();

		/// Returns a string containing the currently held Permissions. \brief Returns a string containing the currently held Permissions.
		/// \returns std::string A string containing the current Permissions.
		std::string getCurrentPermissionString();

		/// Returns a string containing ALL of the possible Permissions. \brief Returns a string containing ALL of the possible Permissions.
		/// \returns std::string A string containing all of the possible Permissions.
		static std::string getAllPermissions();

	  protected:
		uint64_t thePermissions{};

		static std::string computeOverwrites(const std::string& basePermissions, const GuildMember& guildMember, ChannelData& channel);

		static std::string computePermissions(const GuildMember& guildMember, ChannelData& channel);

		static std::string computeBasePermissions(const GuildMember& guildMember);
	};

	/// Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block. \brief Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block.
	/// \param currentFunctionName A string to display the current function's name.
	/// \param theLocation For deriving the current file, line, and column - do not set this value.
	DiscordCoreAPI_Dll void reportException(const std::string& currentFunctionName, std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll void rethrowException(const std::string& currentFunctionName, std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll std::string constructMultiPartData(nlohmann::json theData, const std::vector<File>& files);

	DiscordCoreAPI_Dll std::string convertToLowerCase(const std::string& stringToConvert);

	DiscordCoreAPI_Dll std::string base64Encode(const std::string&, bool = false);

	DiscordCoreAPI_Dll std::string loadFileContents(const std::string& filePath);

	DiscordCoreAPI_Dll std::string utf8MakeValid(const std::string& inputString);

	DiscordCoreAPI_Dll std::string urlEncode(const std::string& inputString);

	DiscordCoreAPI_Dll void spinLock(uint64_t timeInNsToSpinLockFor);

	DiscordCoreAPI_Dll std::string generateBase64EncodedKey();

	DiscordCoreAPI_Dll std::string shiftToBrightGreen();

	DiscordCoreAPI_Dll std::string shiftToBrightBlue();

	DiscordCoreAPI_Dll std::string shiftToBrightRed();

	DiscordCoreAPI_Dll bool nanoSleep(int64_t ns);

	DiscordCoreAPI_Dll std::string reset();

	/// Acquires a timestamp with the current time and date - suitable for use in message-embeds. \brief Acquires a timestamp with the current time and date - suitable for use in message-embeds.
	/// \returns std::string A string containing the current date-time stamp.
	DiscordCoreAPI_Dll std::string getTimeAndDate();

	template<typename ReturnType> ReturnType reverseByteOrder(const ReturnType net) {
		switch (sizeof(ReturnType)) {
			case 1: {
				return net;
			}
			case 2: {
				return ntohs(static_cast<u_short>(net));
			}
			case 4: {
				return ntohl(static_cast<u_long>(net));
			}
			case 8: {
				return ntohll(static_cast<unsigned long long>(net));
			}
		}
		return ReturnType{};
	}

	template<typename ReturnType> void storeBits(std::string& to, ReturnType num) {
		const uint8_t byteSize{ 8 };
		ReturnType newValue = reverseByteOrder<ReturnType>(num);
		for (uint32_t x = 0; x < sizeof(ReturnType); ++x) {
			to.push_back(static_cast<uint8_t>(newValue >> (byteSize * x)));
		}
	}

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

	template<typename ObjectType>
	concept CopyableOrMovable = std::copyable<ObjectType> || std::movable<ObjectType>;

	/// A thread-safe messaging block for data-structures. \brief A thread-safe messaging block for data-structures.
	/// \tparam ObjectType The type of object that will be sent over the message block.
	template<CopyableOrMovable ObjectType> class UnboundedMessageBlock {
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

		UnboundedMessageBlock() noexcept = default;

		/// Sends an object of type ObjectType to the "recipient". \brief Sends an object of type ObjectType to the "recipient".
		/// \param theObject An object of ObjectType.
		void send(ObjectType&& theObject) {
			std::unique_lock theLock{ this->accessMutex };
			this->theQueue.emplace_back(std::move(theObject));
		}

		/// Sends an object of type ObjectType to the "recipient". \brief Sends an object of type ObjectType to the "recipient".
		/// \param theObject An object of ObjectType.
		void send(ObjectType& theObject) {
			std::unique_lock theLock{ this->accessMutex };
			this->theQueue.emplace_back(theObject);
		}

		/// Clears the contents of the messaging block. \brief Clears the contents of the messaging block.
		void clearContents() {
			std::unique_lock theLock{ this->accessMutex };
			this->theQueue.clear();
			this->theQueue = std::deque<ObjectType>{};
		}

		/// Tries to receive an object of type ObjectType to be placed into a reference. \brief Tries to receive an object of type ObjectType to be placed into a reference.
		/// \param theObject A reference of type ObjectType for placing the potentially received object.
		/// \returns bool A bool, denoting whether or not we received an object.
		bool tryReceive(ObjectType& theObject) {
			std::unique_lock theLock{ this->accessMutex };
			if (this->theQueue.size() > 0) {
				theObject = std::move(this->theQueue.front());
				this->theQueue.pop_front();
				return true;
			} else {
				return false;
			}
		}

	  protected:
		std::deque<ObjectType> theQueue{};
		std::mutex accessMutex{};
	};

	template<typename TimeType> class StopWatch {
	  public:
		using DoubleTimeDuration = std::chrono::duration<double, std::nano>;

		using DoubleTimePoint = std::chrono::time_point<std::chrono::system_clock, DoubleTimeDuration>;

		StopWatch<TimeType>& operator=(StopWatch<TimeType>&& other) noexcept {
			if (this != &other) {
				this->maxNumberOfMs = std::move(other.maxNumberOfMs);
				this->startTime = std::move(other.startTime);
			}
			return *this;
		}

		StopWatch(StopWatch<TimeType>&& other) noexcept {
			*this = std::move(other);
			this->resetTimer();
		}

		StopWatch<TimeType>& operator=(const StopWatch<TimeType>& other) noexcept {
			if (this != &other) {
				this->maxNumberOfMs = other.maxNumberOfMs;
				this->startTime = other.startTime;
			}
			return *this;
		}

		StopWatch(const StopWatch<TimeType>& other) noexcept {
			*this = other;
			this->resetTimer();
		}

		StopWatch() noexcept {
			this->resetTimer();
		}

		StopWatch<TimeType>& operator=(TimeType maxNumberOfMsNew) {
			std::unique_lock theLock{ this->theMutex };
			this->maxNumberOfMs = DoubleTimePoint{ maxNumberOfMsNew };
			this->startTime = std::chrono::system_clock::now();
			return *this;
		}

		explicit StopWatch(TimeType maxNumberOfMsNew) {
			*this = maxNumberOfMsNew;
			this->resetTimer();
		}

		uint64_t totalTimePassed() {
			std::unique_lock theLock{ this->theMutex };
			auto elapsedTime = std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()) -
				std::chrono::duration_cast<TimeType>(this->startTime.time_since_epoch());
			return elapsedTime.count();
		}

		bool hasTimePassed() {
			std::unique_lock theLock{ this->theMutex };
			DoubleTimeDuration elapsedTime = std::chrono::system_clock::now() - this->startTime;
			if (elapsedTime >= this->maxNumberOfMs.time_since_epoch()) {
				return true;
			} else {
				return false;
			}
		}

		void resetTimer(uint64_t theNewTime = 0) {
			std::unique_lock theLock{ this->theMutex };
			if (theNewTime != 0) {
				this->maxNumberOfMs = DoubleTimePoint{ TimeType{ theNewTime } };
			}
			this->startTime = std::chrono::system_clock::now();
		}

	  protected:
		DoubleTimePoint maxNumberOfMs{};
		DoubleTimePoint startTime{};
		std::mutex theMutex{};
	};

	template<typename ObjectType> bool waitForTimeToPass(UnboundedMessageBlock<ObjectType>& outBuffer, ObjectType& argOne, int32_t timeInMsNew) {
		StopWatch stopWatch{ std::chrono::milliseconds{ timeInMsNew } };
		while (!outBuffer.tryReceive(argOne)) {
			std::this_thread::sleep_for(1ms);
			if (stopWatch.hasTimePassed()) {
				return true;
			}
		};
		return false;
	}

	/**@}*/
};
