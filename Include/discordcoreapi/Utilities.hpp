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
#ifndef UTILITIES
	#define UTILITIES

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
	uint8_t theData[8]{};
	memcpy(&theData, &(x), sizeof(x));
	uint64_t theValue{};
	for (uint32_t y = 0; y < sizeof(uint64_t); ++y) {
		theValue |= static_cast<uint64_t>(theData[y]) << 8 * (sizeof(uint64_t) - y - 1);
	}
	return theValue;
}
		#include <arpa/inet.h>
		#include <pthread.h>
		#include <sys/time.h>
		#include <time.h>
		#include <ctime>
	#endif

	#include <source_location>
	#include <unordered_map>
	#include <unordered_set>
	#include <shared_mutex>
	#include <functional>
	#include <concepts>
	#include <iostream>
	#include <sstream>
	#include <iomanip>
	#include <vector>
	#include <atomic>
	#include <random>
	#include <string>
	#include <thread>
	#include <mutex>
	#include <queue>
	#include <array>
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

	using AtomicUint64 = std::atomic_uint64_t;
	using AtomicUint32 = std::atomic_uint32_t;
	using AtomicInt64 = std::atomic_int64_t;
	using AtomicInt32 = std::atomic_int32_t;
	using AtomicBool = std::atomic_bool;
	using StringStream = std::stringstream;
	using StringView = std::string_view;
	using String = std::string;
	using Uint64 = uint64_t;
	using Uint32 = uint32_t;
	using Uint16 = uint16_t;
	using Uint8 = uint8_t;
	using Int64 = int64_t;
	using Int32 = int32_t;
	using Int16 = int16_t;
	using Int8 = int8_t;
	using Float = float;
	using Double = double;
	using Snowflake = Uint64;
	using Bool = bool;

	enum class ValueType : Int8 { Null = 0, Null_Ext = 1, Object = 2, Array = 3, Float = 4, String = 5, Bool = 6, Int64 = 7, Uint64 = 8, Unset = 9 };

	template<typename TheType>
	concept IsEnum = std::is_enum<TheType>::value;

	template<typename TheType>
	concept IsString = std::same_as<TheType, String>;

	struct DiscordCoreAPI_Dll EnumConverter {
		template<IsEnum EnumType> EnumConverter(EnumType other) {
			this->thePtr = new Uint64{};
			*static_cast<Uint64*>(this->thePtr) = static_cast<Uint64>(other);
		};

		EnumConverter& operator=(EnumConverter&&) noexcept;

		EnumConverter(EnumConverter&&) noexcept;

		EnumConverter& operator=(EnumConverter&) noexcept = delete;

		EnumConverter(EnumConverter&) noexcept = delete;

		template<IsEnum EnumType> EnumConverter& operator=(std::vector<EnumType> other) {
			this->thePtr = new std::vector<Uint64>{};
			for (auto& value: other) {
				static_cast<std::vector<Uint64>*>(this->thePtr)->emplace_back(static_cast<Uint64>(value));
			}
			this->vectorType = true;
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(std::vector<EnumType> other) {
			*this = other;
		};

		operator std::vector<Uint64>();

		explicit operator Uint64();

		~EnumConverter();

	  protected:
		Bool vectorType{ false };
		void* thePtr{ nullptr };
	};

	class DiscordCoreAPI_Dll JsonObject {
	  public:
		using ObjectType = std::map<String, JsonObject, std::less<>, std::allocator<std::pair<const String, JsonObject>>>;
		using ArrayType = std::vector<JsonObject>;
		using StringType = String;
		using UintType = Uint64;
		using FloatType = Double;
		using IntType = Int64;
		using BoolType = Bool;

		ValueType theType{ ValueType::Null };

		union DiscordCoreAPI_Dll JsonValue {
			FloatType numberDouble;
			UintType numberUint;
			ObjectType* object;
			StringType* string;
			IntType numberInt;
			ArrayType* array;
			BoolType boolean;

			JsonValue& operator=(JsonValue&&) noexcept = delete;

			JsonValue(JsonValue&&) noexcept = delete;

			JsonValue& operator=(const JsonValue&) noexcept = delete;

			JsonValue(const JsonValue&) noexcept = delete;

			JsonValue& operator=(const StringType& theData) noexcept;

			JsonValue& operator=(StringType&& theData) noexcept;

			JsonValue& operator=(const char* theData) noexcept;

			JsonValue& operator=(Uint64 theData) noexcept;

			JsonValue& operator=(Uint32 theData) noexcept;

			JsonValue& operator=(Uint16 theData) noexcept;

			JsonValue& operator=(Uint8 theData) noexcept;

			JsonValue& operator=(Int64 theData) noexcept;

			JsonValue& operator=(Int32 theData) noexcept;

			JsonValue& operator=(Int16 theData) noexcept;

			JsonValue& operator=(Int8 theData) noexcept;

			JsonValue& operator=(Double theData) noexcept;

			JsonValue& operator=(Float theData) noexcept;

			JsonValue& operator=(Bool theData) noexcept;

			JsonValue& operator=(ValueType t) noexcept;
			JsonValue(ValueType t) noexcept;

			void destroy(ValueType theType);
		};

		JsonValue theValue{ ValueType::Null };

		JsonObject() noexcept = default;

		template<typename ObjectType> JsonObject& operator=(std::vector<ObjectType> theData) noexcept {
			this->theType = ValueType::Array;
			Int32 theIndex{};
			for (auto& value: theData) {
				this->theValue = ValueType::Array;
				this->theValue.array->push_back(JsonObject{ value });
				theIndex++;
			}
			return *this;
		}

		template<typename ObjectType> JsonObject(std::vector<ObjectType> theData) noexcept {
			*this = theData;
		}

		template<IsString KeyType, IsString ObjectType> JsonObject& operator=(std::unordered_map<KeyType, ObjectType> theData) noexcept {
			Int32 theIndex{};
			this->theType = ValueType::Array;

			for (auto& [key, value]: theData) {
				this->theValue = ValueType::Object;
				this->theValue.object->at(key) = JsonObject{ value };
			}
			theIndex++;
			return *this;
		}

		template<IsString KeyType, IsString ObjectType> JsonObject(std::unordered_map<KeyType, ObjectType> theData) noexcept {
			*this = theData;
		};

		JsonObject& operator=(EnumConverter theData) noexcept;
		JsonObject(EnumConverter) noexcept;

		JsonObject& operator=(const JsonObject& theKey) noexcept;
		JsonObject(const JsonObject& theKey) noexcept;

		JsonObject& operator=(JsonObject&& theKey) noexcept;
		JsonObject(JsonObject&& theKey) noexcept;

		JsonObject& operator=(const StringType& theData) noexcept;
		JsonObject(const StringType&) noexcept;

		JsonObject& operator=(StringType&& theData) noexcept;
		JsonObject(StringType&&) noexcept;

		JsonObject& operator=(const char* theData) noexcept;
		JsonObject(const char* theData) noexcept;

		JsonObject& operator=(Uint64 theData) noexcept;
		JsonObject(Uint64) noexcept;

		JsonObject& operator=(Uint32 theData) noexcept;
		JsonObject(Uint32) noexcept;

		JsonObject& operator=(Uint16 theData) noexcept;
		JsonObject(Uint16) noexcept;

		JsonObject& operator=(Uint8 theData) noexcept;
		JsonObject(Uint8) noexcept;

		JsonObject& operator=(Int64 theData) noexcept;
		JsonObject(Int64) noexcept;

		JsonObject& operator=(Int32 theData) noexcept;
		JsonObject(Int32) noexcept;

		JsonObject& operator=(Int16 theData) noexcept;
		JsonObject(Int16) noexcept;

		JsonObject& operator=(Int8 theData) noexcept;
		JsonObject(Int8) noexcept;

		JsonObject& operator=(Double theData) noexcept;
		JsonObject(Double) noexcept;

		JsonObject& operator=(Float theData) noexcept;
		JsonObject(Float) noexcept;

		JsonObject& operator=(Bool theData) noexcept;
		JsonObject(Bool) noexcept;

		JsonObject(const char*, ValueType) noexcept;

		JsonObject& operator=(ValueType) noexcept;

		JsonObject& operator[](Uint64 idx) const;
		JsonObject& operator[](Uint64 idx);

		JsonObject& operator[](const typename ObjectType::key_type& key) const;
		JsonObject& operator[](typename ObjectType::key_type key);

		operator String() noexcept;

		operator String() const noexcept;
		void pushBack(JsonObject& other) noexcept;
		void pushBack(JsonObject&& other) noexcept;

		~JsonObject() noexcept;
	};

	struct DiscordCoreAPI_Dll ActivityData;

	/// For selecting the type of streamer that the given bot is, one must be one server and one of client per connection. \brief For selecting the type of streamer that the given bot is, one must be one server and one of client per connection.
	enum class StreamType { None = 0, Client = 1, Server = 2 };

	/// For connecting two bots to stream the VC contents between the two. \brief For connecting two bots to stream the VC contents between the two.
	struct DiscordCoreAPI_Dll StreamInfo {
		String address{};///< The address to connect to.
		String port{};///< The port to connect to.
	};

};

namespace DiscordCoreInternal {

	using AtomicUint64 = std::atomic_uint64_t;
	using AtomicUint32 = std::atomic_uint32_t;
	using AtomicInt64 = std::atomic_int64_t;
	using AtomicInt32 = std::atomic_int32_t;
	using AtomicBool = std::atomic_bool;
	using StringStream = std::stringstream;
	using StringView = std::string_view;
	using String = std::string;
	using Uint64 = uint64_t;
	using Uint32 = uint32_t;
	using Uint16 = uint16_t;
	using Uint8 = uint8_t;
	using Int64 = int64_t;
	using Int32 = int32_t;
	using Int16 = int16_t;
	using Int8 = int8_t;
	using Float = float;
	using Double = double;
	using Snowflake = Uint64;
	using Bool = bool;
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

	struct DiscordCoreAPI_Dll UpdatePresenceData;

	template<typename T> inline auto utCast(T x) -> std::enable_if_t<std::is_enum_v<T>, std::underlying_type_t<T>> {
		return static_cast<std::underlying_type_t<T>>(x);
	}

	enum class WebSocketOpCode : Int8 { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

	/// Websocket close codes. \brief Websocket close codes.
	class DiscordCoreAPI_Dll WebSocketClose {
	  public:
		/// Websocket close codes. \brief Websocket close codes.
		enum class WebSocketCloseCode : Uint16 {
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

		std::unordered_map<Uint16, WebSocketCloseCode> mappingValues{ { 0, WebSocketCloseCode::Unset }, { 1000, WebSocketCloseCode::Normal_Close },
			{ 4000, WebSocketCloseCode::Unknown_Error }, { 4001, WebSocketCloseCode::Unknown_Opcode }, { 4002, WebSocketCloseCode::Decode_Error },
			{ 4003, WebSocketCloseCode::Not_Authenticated }, { 4004, WebSocketCloseCode::Authentication_Failed }, { 4005, WebSocketCloseCode::Already_Authenticated },
			{ 4007, WebSocketCloseCode::Invalid_Seq }, { 4008, WebSocketCloseCode::Rate_Limited }, { 4009, WebSocketCloseCode::Session_Timed },
			{ 4010, WebSocketCloseCode::Invalid_Shard }, { 4011, WebSocketCloseCode::Sharding_Required }, { 4012, WebSocketCloseCode::Invalid_API_Version },
			{ 4013, WebSocketCloseCode::Invalid_Intent }, { 4014, WebSocketCloseCode::Disallowed_Intent } };

		WebSocketCloseCode theValue{};

		WebSocketClose& operator=(Uint16 theValueNew);

		explicit WebSocketClose(Uint16 theValueNew);

		operator Uint16();

		operator Bool();
	};

	/// For updating a User's presence. \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		std::vector<DiscordCoreAPI::ActivityData> activities{};///< A vector of activities.
		String status{};///< Current status.
		Int64 since{ 0 };///< When was the activity started?
		Bool afk{ false };///< Are we afk.

		operator String();
	};

}// namespace DiscordCoreInternal


namespace DiscordCoreAPI {

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

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const String& (*theFunction)( void ));

	/// Input event response types. \brief Input event response types.
	enum class InputEventResponseType : Int8 {
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
	enum class GatewayIntents : Int32 {
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
		Uint32 intervalInMs{ 0 };///< The time interval at which to call the std::function.
		Bool repeated{ false };///< Whether or not the std::function is repeating.
		Int64 dummyArg{ 0 };
	};

	/// Represents which text format to use for websocket transfer. \brief Represents which text format to use for websocket transfer.
	enum class TextFormat : Int8 {
		Etf = 0x00,///< Etf format.
		Json = 0x01///< Json format.
	};

	/// Sharding options for the library. \brief Sharding options for the library.
	struct DiscordCoreAPI_Dll ShardingOptions {
		Uint32 numberOfShardsForThisProcess{ 1 };///< The number of shards to launch on the current process.
		Uint32 totalNumberOfShards{ 1 };///< The total number of shards that will be launched across all processes.
		Uint32 startingShard{ 0 };///< The first shard to start on this process.
	};

	/**@}*/

	/// Logging options for the library. \brief Loggin options for the library.
	struct DiscordCoreAPI_Dll LoggingOptions {
		Bool logWebSocketSuccessMessages{ false };///< Do we log the websocket success messages to cout?
		Bool logWebSocketErrorMessages{ false };///< Do we log the websocket error messages to cout?
		Bool logGeneralSuccessMessages{ false };///< Do we log general success messages to cout?
		Bool logFFMPEGSuccessMessages{ false };///< Do we log FFMPEG success messages to cout?
		Bool logGeneralErrorMessages{ false };///< Do we log general error messages to cout?
		Bool logHttpsSuccessMessages{ false };///< Do we log Http response success messages to cout?
		Bool logFFMPEGErrorMessages{ false };///< Do we log FFMPEG error messages to cout?
		Bool logHttpsErrorMessages{ false };///< Do we log Http response error messages to cout?
	};

	/// For selecting the caching style of the library. \brief For selecting the caching style of the library.
	struct DiscordCoreAPI_Dll CacheOptions {
		Bool cacheChannels{ true };///< Do we cache Channels?
		Bool cacheGuilds{ true };///< Do we cache Guilds?
		Bool cacheRoles{ true };///< Do we cache Roles?
		Bool cacheUsers{ true };///< Do we cache Users/GuildMembers?
	};

	/// Configuration data for the library's main class, DiscordCoreClient. \brief Configuration data for the library's main class, DiscordCoreClient.
	struct DiscordCoreAPI_Dll DiscordCoreClientConfig {
		GatewayIntents theIntents{ GatewayIntents::All_Intents };///< The gateway intents to be used for this instance.
		DiscordCoreInternal::UpdatePresenceData presenceData{};///< Presence data to initialize your bot with.
		std::vector<RepeatedFunctionData> functionsToExecute{};///< Functions to execute after a timer, or on a repetition.
		TextFormat textFormat{ TextFormat::Etf };///< Use ETF or JSON format for websocket transfer?
		String connectionAddress{};///< A potentially alternative connection address for the websocket.
		ShardingOptions shardOptions{};///< Options for the sharding of your bot.
		String connectionPort{};///< A potentially alternative connection port for the websocket.
		LoggingOptions logOptions{};///< Options for the output/logging of the library.
		CacheOptions cacheOptions{};///< Options for the cache of the library.
		String botToken{};///< Your bot's token.
	};

	struct DiscordCoreAPI_Dll JsonStringValue {
		String theValue{};
		ValueType theType{};
	};

	class DiscordCoreAPI_Dll ConfigManager {
	  public:
		ConfigManager() noexcept = default;

		explicit ConfigManager(const DiscordCoreClientConfig&);

		const Bool doWePrintWebSocketSuccessMessages();

		const Bool doWePrintWebSocketErrorMessages();

		const Bool doWePrintHttpsSuccessMessages();

		const Bool doWePrintHttpsErrorMessages();

		const Bool doWePrintFFMPEGSuccessMessages();

		const Bool doWePrintFFMPEGErrorMessages();

		const Bool doWePrintGeneralSuccessMessages();

		const Bool doWePrintGeneralErrorMessages();

		const Bool doWeCacheChannels();

		const Bool doWeCacheUsers();

		const Bool doWeCacheGuilds();

		const Bool doWeCacheRoles();

		const DiscordCoreInternal::UpdatePresenceData getPresenceData();

		const String getBotToken();

		const Uint32 getTotalShardCount();

		const Uint32 getStartingShard();

		const Uint32 getShardCountForThisProcess();

		const String getConnectionAddress();

		void setConnectionAddress(const String& connectionAddressNew);

		const String getConnectionPort();

		void setConnectionPort(const String& connectionPortNew);

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

	template<typename ObjectType> class ObjectCache {
	  public:
		ObjectCache() noexcept {};

		void emplace(ObjectType&& theData) noexcept {
			std::unique_lock theLock{ this->theMutex };
			this->theMap.emplace(std::move(theData));
		}

		void emplace(ObjectType& theData) noexcept {
			std::unique_lock theLock{ this->theMutex };
			this->theMap.emplace(theData);
		}

		const ObjectType& readOnly(ObjectType& theKey) noexcept {
			std::shared_lock theLock{ this->theMutex };
			return *this->theMap.find(theKey);
		}

		ObjectType& at(ObjectType&& theKey) noexcept {
			std::shared_lock theLock{ this->theMutex };
			return ( ObjectType& )*this->theMap.find(theKey);
		}

		ObjectType& at(ObjectType& theKey) noexcept {
			std::shared_lock theLock{ this->theMutex };
			return ( ObjectType& )*this->theMap.find(theKey);
		}

		auto begin() {
			std::unique_lock theLock{ this->theMutex };
			return this->theMap.begin();
		}

		auto end() {
			std::unique_lock theLock{ this->theMutex };
			return this->theMap.end();
		}

		const Bool contains(ObjectType& theKey) noexcept {
			return this->theMap.contains(theKey);
		}

		void erase(ObjectType& theKey) {
			if (this->theMap.contains(theKey)) {
				std::unique_lock theLock{ this->theMutex };
				this->theMap.erase(theKey);
			}
		}

		ObjectType& operator[](ObjectType& theKey) {
			std::shared_lock theLock{ this->theMutex };
			return ( ObjectType& )*this->theMap.find(theKey);
		}

		ObjectType& operator[](ObjectType&& theKey) {
			std::shared_lock theLock{ this->theMutex };
			return ( ObjectType& )*this->theMap.find(theKey);
		}

		Uint64 size() noexcept {
			std::unique_lock theLock{ this->theMutex };
			return this->theMap.size();
		}

	  protected:
		std::unordered_set<ObjectType> theMap{};
		std::shared_mutex theMutex{};
	};

	class DiscordCoreAPI_Dll StringWrapper {
	  public:
		StringWrapper() noexcept = default;

		StringWrapper& operator=(StringWrapper&& other) noexcept;

		StringWrapper(StringWrapper&& other) noexcept;

		StringWrapper& operator=(const StringWrapper& other);

		StringWrapper(const StringWrapper& other);

		StringWrapper& operator=(const String& theString);

		explicit StringWrapper(const String& theString);

		StringWrapper& operator=(const char* theString);

		StringWrapper(const char* theString);

		operator std::basic_string<char, std::char_traits<char>, std::allocator<char>>();

		void emplace_back(char theChar);

		Uint64 size();

		const char* data();

	  protected:
		std::unique_ptr<char[]> thePtr{};
	};

	inline std::basic_ostream<char>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, const StringWrapper& rhs) {
		for (auto& value: static_cast<String>(static_cast<StringWrapper>(rhs))) {
			lhs.put(value);
		}
		return lhs;
	}

	inline std::basic_string<char> operator+(const std::basic_string<char, std::char_traits<char>, std::allocator<char>>& lhs, StringWrapper rhs) {
		StringStream theStream{};
		theStream << lhs << rhs;
		return theStream.str();
	}

	inline std::basic_string<char> operator+(const char* lhs, StringWrapper rhs) {
		StringStream theStream{};
		theStream << lhs << rhs;
		return theStream.str();
	}

	inline Bool operator==(StringWrapper lhs, const char* rhs) {
		if (String(lhs) == String(rhs)) {
			return true;
		} else {
			return false;
		}
	}

	inline Bool operator!=(StringWrapper lhs, const char* rhs) {
		if (static_cast<String>(lhs) == rhs) {
			return false;
		}
		return true;
	}

	inline Bool operator==(String& lhs, StringWrapper& rhs) {
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
	enum class AudioFrameType : Int8 {
		Unset = 0,///< Unset.
		Encoded = 1,///< Encoded.
		RawPCM = 2,///< Raw PCM.
		Skip = 3///< Skip.
	};

	/// Represents a single frame of audio data. \brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll AudioFrameData {
		AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
		Int64 sampleCount{ -1ll };///< The number of samples per this frame.
		Uint64 guildMemberId{ 0 };///< GuildMemberId for the sending GuildMember.
		std::vector<Uint8> data{};///< The audio data.

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

	DiscordCoreAPI_Dll Uint64 strtoull(const String& theString);

	template<typename ReturnType> ReturnType fromString(const String& string, std::ios_base& (*type)( std::ios_base& )) {
		ReturnType theValue{};
		std::istringstream theStream(string);
		theStream >> type, theStream >> theValue;
		return theValue;
	}

	template<typename ReturnType> String toHex(ReturnType inputValue) {
		StringStream theStream{};
		theStream << std::setfill('0') << std::setw(sizeof(ReturnType) * 2) << std::hex << inputValue;
		return theStream.str();
	}

	class DiscordCoreAPI_Dll RGBColorValue {
	  public:
		Uint8 green{};
		Uint8 blue{};
		Uint8 red{};
	};

	using HexColorValue = String;

	class DiscordCoreAPI_Dll ColorValue {
	  public:
		ColorValue(String theHexColorValue);

		ColorValue(Uint32 theColorValue);

		RGBColorValue getRgbColorValue();

		HexColorValue getHexColorValue();

		Uint32 getIntColorValue();

	  protected:
		Uint32 theColor{};
	};

	enum class HashType { User_Avatar = 0, Channel_Icon = 1, GuildMember_Avatar = 2, Guild_Icon = 3, Guild_Splash = 4, Guild_Banner = 5, Guild_Discovery = 6 };

	class DiscordCoreAPI_Dll IconHash {
	  public:
		IconHash() noexcept = default;

		IconHash& operator=(const String theString);

		IconHash(const String theString) noexcept;

		Bool operator==(const IconHash& other);

		String getIconHash() noexcept;

		~IconHash() noexcept = default;

	  protected:
		Uint64 highBits{};
		Uint64 lowBits{};
	};

	template<typename TimeType>
	/// Class for representing a timestamp, as well as working with time-related values. \brief Class for representing a timestamp, as well as working with time-related values.
	class DiscordCoreAPI_Dll TimeStamp {
	  public:
		explicit TimeStamp(TimeFormat theFormatNew = TimeFormat::LongDateTime) {
			this->timeStampInTimeUnits = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		TimeStamp(String year, String month, String day, String hour, String minute, String second, TimeFormat theFormatNew) {
			this->getTimeSinceEpoch(stoull(year), stoull(month), stoull(day), stoull(hour), stoull(minute), stoull(second));
		}

		operator String() {
			return getISO8601TimeStamp(TimeFormat::LongDateTime);
		}

		operator Uint64() {
			return this->timeStampInTimeUnits;
		}

		TimeStamp<TimeType>& operator=(String&& originalTimeStampNew) {
			String theString = originalTimeStampNew;
			this->convertTimeStampToTimeUnits(TimeFormat::LongDateTime, theString);
			this->getISO8601TimeStamp(TimeFormat::LongDateTime);
			return *this;
		}

		explicit TimeStamp(String&& originalTimeStampNew) {
			*this = std::move(originalTimeStampNew);
		}

		TimeStamp<TimeType>& operator=(String& originalTimeStampNew) {
			String theString = originalTimeStampNew;
			this->convertTimeStampToTimeUnits(TimeFormat::LongDateTime, theString);
			this->getISO8601TimeStamp(TimeFormat::LongDateTime);
			return *this;
		}

		explicit TimeStamp(String& originalTimeStampNew) {
			*this = originalTimeStampNew;
		}

		TimeStamp<TimeType>& operator=(const TimeStamp& other) {
			this->timeStampInTimeUnits = other.timeStampInTimeUnits;
			return *this;
		}

		TimeStamp(const TimeStamp& other) {
			*this = other;
		}

		TimeStamp(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 minute, Int32 second, TimeFormat theFormatNew) {
			this->getTimeSinceEpoch(year, month, day, hour, minute, second);
		};

		TimeStamp(Uint64 timeInTimeUnits, TimeFormat theFormatNew) {
			this->timeStampInTimeUnits = TimeType{ timeInTimeUnits }.count();
			this->getISO8601TimeStamp(theFormatNew);
		}

		static String convertToFutureISO8601TimeStamp(Int32 minutesToAdd, Int32 hoursToAdd, Int32 daysToAdd, Int32 monthsToAdd, Int32 yearsToAdd, TimeFormat theFormatNew) {
			std::time_t result = std::time(nullptr);
			Int32 secondsPerMinute{ 60 };
			Int32 minutesPerHour{ 60 };
			Int32 secondsPerHour{ minutesPerHour * secondsPerMinute };
			Int32 hoursPerDay{ 24 };
			Int32 secondsPerDay{ secondsPerHour * hoursPerDay };
			Int32 daysPerMonth{ 30 };
			Int32 secondsPerMonth{ secondsPerDay * daysPerMonth };
			Int32 daysPerYear{ 365 };
			Int32 secondsPerYear{ secondsPerDay * daysPerYear };
			Int32 secondsToAdd =
				(yearsToAdd * secondsPerYear) + (monthsToAdd * secondsPerMonth) + (daysToAdd * secondsPerDay) + (hoursToAdd * secondsPerHour) + (minutesToAdd * secondsPerMinute);
			result += secondsToAdd;
			auto resultTwo = std::localtime(&result);
			String theReturnString{};
			if (resultTwo->tm_isdst) {
				if (resultTwo->tm_hour + 4 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), theFormatNew };
				theTimeStamp.getISO8601TimeStamp(theFormatNew);
				theReturnString = static_cast<String>(theTimeStamp);
			} else {
				if (resultTwo->tm_hour + 5 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), theFormatNew };
				theTimeStamp.getISO8601TimeStamp(theFormatNew);
				theReturnString = static_cast<String>(theTimeStamp);
			}
			return theReturnString;
		}

		static String convertToCurrentISO8601TimeStamp(TimeFormat timeFormat) {
			std::time_t result = std::time(nullptr);
			auto resultTwo = std::localtime(&result);
			String theReturnString{};
			if (resultTwo->tm_isdst) {
				if (resultTwo->tm_hour + 4 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 4), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), timeFormat };
				theReturnString = theTimeStamp.getISO8601TimeStamp(timeFormat);
			} else {
				if (resultTwo->tm_hour + 5 >= 24) {
					resultTwo->tm_hour = resultTwo->tm_hour - 24;
					resultTwo->tm_mday++;
				}
				TimeStamp theTimeStamp{ std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon + 1), std::to_string(resultTwo->tm_mday),
					std::to_string(resultTwo->tm_hour + 5), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec), timeFormat };
				theReturnString = theTimeStamp.getISO8601TimeStamp(timeFormat);
			}
			return theReturnString;
		}

		Bool hasTimeElapsed(Uint64 days, Uint64 hours, Uint64 minutes) {
			if (this->timeStampInTimeUnits == 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
			Uint64 startTimeRaw = this->timeStampInTimeUnits;
			auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			Uint64 secondsPerMinute = 60;
			Uint64 secondsPerHour = secondsPerMinute * 60;
			Uint64 secondsPerDay = secondsPerHour * 24;
			auto targetElapsedTime = ((days * secondsPerDay) + (hours * secondsPerHour) + (minutes * secondsPerMinute)) * 1000;
			auto actualElapsedTime = currentTime - startTimeRaw;
			if (actualElapsedTime >= targetElapsedTime) {
				return true;
			} else {
				return false;
			}
		}

		static String convertMsToDurationString(Uint64 durationInMs) {
			String newString{};
			Uint64 msPerSecond{ 1000 };
			Uint64 secondsPerMinute{ 60 };
			Uint64 minutesPerHour{ 60 };
			Uint64 msPerMinute{ msPerSecond * secondsPerMinute };
			Uint64 msPerHour{ msPerMinute * minutesPerHour };
			Uint64 hoursLeft = static_cast<Uint64>(trunc(durationInMs / msPerHour));
			Uint64 minutesLeft = static_cast<Uint64>(trunc((durationInMs % msPerHour) / msPerMinute));
			Uint64 secondsLeft = static_cast<Uint64>(trunc(((durationInMs % msPerHour) % msPerMinute) / msPerSecond));
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
		Uint64 timeStampInTimeUnits{};

		void getTimeSinceEpoch(Int64 year, Int64 month, Int64 day, Int64 hour, Int64 minute, Int64 second) {
			const Uint32 secondsInJan{ 31 * 24 * 60 * 60 };
			const Uint32 secondsInFeb{ 28 * 24 * 60 * 60 };
			const Uint32 secondsInMar{ 31 * 24 * 60 * 60 };
			const Uint32 secondsInApr{ 30 * 24 * 60 * 60 };
			const Uint32 secondsInMay{ 31 * 24 * 60 * 60 };
			const Uint32 secondsInJun{ 30 * 24 * 60 * 60 };
			const Uint32 secondsInJul{ 31 * 24 * 60 * 60 };
			const Uint32 secondsInAug{ 31 * 24 * 60 * 60 };
			const Uint32 secondsInSep{ 30 * 24 * 60 * 60 };
			const Uint32 secondsInOct{ 31 * 24 * 60 * 60 };
			const Uint32 secondsInNov{ 30 * 24 * 60 * 60 };
			const Uint32 secondsInDec{ 31 * 24 * 60 * 60 };
			const Uint32 secondsPerMinute{ 60 };
			const Uint32 secondsPerHour{ 60 * 60 };
			const Uint32 secondsPerDay{ 60 * 60 * 24 };
			TimeType theValue{};
			for (Int32 x = 1970; x < year; ++x) {
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
				theValue += TimeType{ static_cast<Uint64>((day - 1) * secondsPerDay) };
				theValue += TimeType{ static_cast<Uint64>(hour * secondsPerHour) };
				theValue += TimeType{ static_cast<Uint64>(minute * secondsPerMinute) };
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

		void convertTimeStampToTimeUnits(TimeFormat theFormatNew, String originalTimeStamp) {
			try {
				if (originalTimeStamp != "" && originalTimeStamp != "0") {
					TimeStamp<TimeType> timeValue = TimeStamp{ stoi(originalTimeStamp.substr(0, 4)), stoi(originalTimeStamp.substr(5, 6)), stoi(originalTimeStamp.substr(8, 9)),
						stoi(originalTimeStamp.substr(11, 12)), stoi(originalTimeStamp.substr(14, 15)), stoi(originalTimeStamp.substr(17, 18)), theFormatNew };
					this->timeStampInTimeUnits = TimeType{ static_cast<Uint64>(timeValue) }.count();
				} else {
					this->timeStampInTimeUnits = std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count();
				}
			} catch (...) {
			}
		}

		String getISO8601TimeStamp(TimeFormat timeFormat) {
			if (this->timeStampInTimeUnits == 0) {
				this->timeStampInTimeUnits = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
			Uint64 timeValue = (std::chrono::duration_cast<std::chrono::milliseconds>(TimeType{ this->timeStampInTimeUnits }).count()) / 1000;
			time_t rawTime(timeValue);
			tm timeInfo = *localtime(&rawTime);
			String timeStamp{};
			timeStamp.resize(48);
			switch (timeFormat) {
				case TimeFormat::LongDate: {
					Uint64 sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongDateTime: {
					Uint64 sizeResponse = strftime(timeStamp.data(), 48, "%FT%T", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::LongTime: {
					Uint64 sizeResponse = strftime(timeStamp.data(), 48, "%T", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDate: {
					Uint64 sizeResponse = strftime(timeStamp.data(), 48, "%d/%m/%g", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortDateTime: {
					Uint64 sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G %R", &timeInfo);
					timeStamp.resize(sizeResponse);
					break;
				}
				case TimeFormat::ShortTime: {
					Uint64 sizeResponse = strftime(timeStamp.data(), 48, "%R", &timeInfo);
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
	enum class Permission : Uint64 {
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

		Permissions& operator=(String&& other);

		explicit Permissions(String&& permsNew);

		Permissions& operator=(const String& other);

		explicit Permissions(const String& permsNew);

		Permissions& operator=(const Uint64 other);

		explicit Permissions(const Uint64 permsNew);

		operator Uint64();

		operator String();

		/// Returns a string containing all of a given User's Permissions for a given Channel. \brief Returns a string containing all of a given User's Permissions for a given Channel.
		/// \param guildMember The GuildMember who's Permissions to analyze.
		/// \param channel The Channel withint which to check for Permissions.
		/// \returns String A string containing the final Permission's value for a given Channel.
		static String getCurrentChannelPermissions(const GuildMember& guildMember, ChannelData& channel);

		/// Checks for a given Permission in a chosen Channel, for a specific User. \brief Checks for a given Permission in a chosen Channel, for a specific User.
		/// \param guildMember The GuildMember who to check the Permissions of.
		/// \param channel The Channel within which to check for the Permission's presence.
		/// \param permission A Permission to check the current Channel for.
		/// \returns Bool A Bool suggesting the presence of the chosen Permission.
		Bool checkForPermission(const GuildMember& guildMember, ChannelData& channel, Permission permission);

		/// Returns a string containing the currently held Permissions in a given Guild. \brief Returns a string containing the currently held Permissions in a given Guild.
		/// \param guildMember The GuildMember who's Permissions are to be evaluated.
		/// \returns String A string containing the current Permissions.
		static String getCurrentGuildPermissions(const GuildMember& guildMember);

		/// Removes one or more Permissions from the current Permissions value. \brief Removes one or more Permissions from the current Permissions value.
		/// \param permissionsToRemove A vector containing the Permissions you wish to remove.
		void removePermissions(const std::vector<Permission>& permissionsToRemove);

		/// Adds one or more Permissions to the current Permissions value. \brief Adds one or more Permissions to the current Permissions value.
		/// \param permissionsToAdd A vector containing the Permissions you wish to add.
		void addPermissions(const std::vector<Permission>& permissionsToAdd);

		/// Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format. \brief Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format.
		/// \returns std::vector A vector full of strings of the Permissions that are in the input String's value.
		std::vector<String> displayPermissions();

		/// Returns a string containing the currently held Permissions. \brief Returns a string containing the currently held Permissions.
		/// \returns String A string containing the current Permissions.
		String getCurrentPermissionString();

		/// Returns a string containing ALL of the possible Permissions. \brief Returns a string containing ALL of the possible Permissions.
		/// \returns String A string containing all of the possible Permissions.
		static String getAllPermissions();

	  protected:
		Uint64 thePermissions{};

		static String computeOverwrites(const String& basePermissions, const GuildMember& guildMember, ChannelData& channel);

		static String computePermissions(const GuildMember& guildMember, ChannelData& channel);

		static String computeBasePermissions(const GuildMember& guildMember);
	};

	/// Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block. \brief Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block.
	/// \param currentFunctionName A string to display the current function's name.
	/// \param theLocation For deriving the current file, line, and column - do not set this value.
	DiscordCoreAPI_Dll void reportException(const String& currentFunctionName, std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll void rethrowException(const String& currentFunctionName, std::source_location theLocation = std::source_location::current());

	DiscordCoreAPI_Dll String constructMultiPartData(String theData, const std::vector<File>& files);

	DiscordCoreAPI_Dll String convertToLowerCase(const String& stringToConvert);

	DiscordCoreAPI_Dll String base64Encode(const String&, Bool = false);

	DiscordCoreAPI_Dll String loadFileContents(const String& filePath);

	DiscordCoreAPI_Dll String utf8MakeValid(const String& inputString);

	DiscordCoreAPI_Dll String urlEncode(const String& inputString);

	DiscordCoreAPI_Dll void spinLock(Uint64 timeInNsToSpinLockFor);

	DiscordCoreAPI_Dll String generateBase64EncodedKey();

	DiscordCoreAPI_Dll String shiftToBrightGreen();

	DiscordCoreAPI_Dll String shiftToBrightBlue();

	DiscordCoreAPI_Dll String shiftToBrightRed();

	DiscordCoreAPI_Dll Bool nanoSleep(Int64 ns);

	DiscordCoreAPI_Dll String escapeCharacters(StringView theString);

	DiscordCoreAPI_Dll String reset();

	/// Acquires a timestamp with the current time and date - suitable for use in message-embeds. \brief Acquires a timestamp with the current time and date - suitable for use in message-embeds.
	/// \returns String A string containing the current date-time stamp.
	DiscordCoreAPI_Dll String getTimeAndDate();

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

	template<typename ReturnType> void storeBits(String& to, ReturnType num) {
		const Uint8 byteSize{ 8 };
		ReturnType newValue = reverseByteOrder<ReturnType>(num);
		for (Uint32 x = 0; x < sizeof(ReturnType); ++x) {
			to.push_back(static_cast<Uint8>(newValue >> (byteSize * x)));
		}
	}

	template<typename StoredAsType, typename FlagType> StoredAsType setBool(StoredAsType inputFlag, FlagType theFlag, Bool enabled) {
		if (enabled) {
			inputFlag |= static_cast<StoredAsType>(theFlag);
			return inputFlag;
		} else {
			inputFlag &= ~static_cast<StoredAsType>(theFlag);
			return inputFlag;
		}
	}

	template<typename StoredAsType, typename FlagType> Bool getBool(StoredAsType inputFlag, FlagType theFlag) {
		return static_cast<StoredAsType>(inputFlag) & static_cast<StoredAsType>(theFlag);
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
		/// \returns Bool A Bool, denoting whether or not we received an object.
		Bool tryReceive(ObjectType& theObject) {
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

	template<typename ReturnType> class NewThreadAwaiter;

	/// An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution. \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam ReturnType The type of value returned by the containing CoRoutine.
	template<typename ReturnType> auto NewThreadAwaitable() {
		return NewThreadAwaiter<ReturnType>{};
	}

	/// Typedef for the message filter. \brief Typedef for the message filter.
	template<typename Object> using ObjectFilter = std::function<Bool(Object)>;

	/// ObjectCollectorReturnData responseData. \brief ObjectCollectorReturnData responseData.
	template<typename Object> struct DiscordCoreAPI_Dll ObjectCollectorReturnData {
		std::vector<Object> objects{};///< A vector of collected Objects.
	};

	/// ObjectCollector, for collecting Objects from a Channel. \brief Object collector, for collecting Objects from a Channel.
	template<typename Object> class DiscordCoreAPI_Dll ObjectCollector {
	  public:
		static std::unordered_map<String, UnboundedMessageBlock<Object>*> objectsBufferMap;

		ObjectCollector() noexcept = default;

		/// Begin waiting for Objects. \brief Begin waiting for Objects.
		/// \param quantityToCollect Maximum quantity of Objects to collect before returning the results.
		/// \param msToCollectForNew Maximum number of std::chrono::milliseconds to wait for Objects before returning the results.
		/// \param filteringFunctionNew A filter function to apply to new Objects, where returning "true" from the function results in a Object being stored.
		/// \returns A ObjectCollectorReturnData structure.
		CoRoutine<ObjectCollectorReturnData<Object>> collectObjects(Int32 quantityToCollect, Int32 msToCollectForNew, ObjectFilter<Object> filteringFunctionNew) {
			co_await NewThreadAwaitable<ObjectCollectorReturnData<Object>>();
			this->quantityOfObjectToCollect = quantityToCollect;
			this->filteringFunction = filteringFunctionNew;
			this->msToCollectFor = msToCollectForNew;
			this->collectorId = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
			ObjectCollector::objectsBufferMap[this->collectorId] = &this->messagesBuffer;
			this->run();
			co_return this->messageReturnData;
		}

		void run() {
			Int64 startingTime = static_cast<Int64>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
			Int64 elapsedTime{ 0 };
			while (elapsedTime < this->msToCollectFor) {
				Object message{};
				waitForTimeToPass<Object>(this->messagesBuffer, message, static_cast<Int32>(this->msToCollectFor - elapsedTime));
				if (this->filteringFunction(message)) {
					this->messageReturnData.objects.emplace_back(message);
				}
				if (static_cast<Int32>(this->messageReturnData.objects.size()) >= this->quantityOfObjectToCollect) {
					break;
				}

				elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - startingTime;
			}
		}

		~ObjectCollector() {
			if (ObjectCollector::objectsBufferMap.contains(this->collectorId)) {
				ObjectCollector::objectsBufferMap.erase(this->collectorId);
			}
		}

	  protected:
		UnboundedMessageBlock<Object> messagesBuffer{};
		ObjectCollectorReturnData<Object> messageReturnData{};
		ObjectFilter<Object> filteringFunction{ nullptr };
		Int32 quantityOfObjectToCollect{ 0 };
		String collectorId{};
		Int32 msToCollectFor{ 0 };
	};

	template<typename TimeType> class StopWatch {
	  public:
		StopWatch<TimeType>& operator=(StopWatch<TimeType>&& other) noexcept {
			if (this != &other) {
				this->maxNumberOfTimeUnits.store(other.maxNumberOfTimeUnits.load());
				this->startTime.store(other.startTime.load());
			}
			return *this;
		}

		StopWatch(StopWatch<TimeType>&& other) noexcept {
			*this = std::move(other);
			this->resetTimer();
		}

		StopWatch<TimeType>& operator=(StopWatch<TimeType>& other) noexcept {
			if (this != &other) {
				this->maxNumberOfTimeUnits.store(other.maxNumberOfTimeUnits.load());
				this->startTime.store(other.startTime.load());
			}
			return *this;
		}

		StopWatch(StopWatch<TimeType>& other) noexcept {
			*this = other;
			this->resetTimer();
		}

		StopWatch() noexcept {
			this->resetTimer();
		}

		StopWatch<TimeType>& operator=(TimeType maxNumberOfTimeUnitsNew) {
			this->maxNumberOfTimeUnits.store(maxNumberOfTimeUnitsNew.count());
			this->startTime.store(std::chrono::duration_cast<TimeType>(std::chrono::steady_clock::now().time_since_epoch()).count());
			return *this;
		}

		explicit StopWatch(TimeType maxNumberOfTimeUnitsNew) {
			*this = maxNumberOfTimeUnitsNew;
			this->resetTimer();
		}

		Uint64 totalTimePassed() {
			auto elapsedTime = std::chrono::duration_cast<TimeType>(std::chrono::steady_clock::now().time_since_epoch()).count() - this->startTime.load();
			return elapsedTime;
		}

		Bool hasTimePassed() {
			auto elapsedTime = std::chrono::duration_cast<TimeType>(std::chrono::steady_clock::now().time_since_epoch()).count() - this->startTime.load();
			if (elapsedTime >= this->maxNumberOfTimeUnits.load()) {
				return true;
			} else {
				return false;
			}
		}

		void resetTimer(Uint64 theNewTime = 0) {
			if (theNewTime != 0) {
				this->maxNumberOfTimeUnits.store(TimeType{ theNewTime }.count());
			}
			this->startTime.store(std::chrono::duration_cast<TimeType>(std::chrono::steady_clock::now().time_since_epoch()).count());
		}

	  protected:
		AtomicUint64 maxNumberOfTimeUnits{};
		AtomicUint64 startTime{};
	};

	template<typename ObjectType> Bool waitForTimeToPass(UnboundedMessageBlock<ObjectType>& outBuffer, ObjectType& argOne, Int32 timeInMsNew) {
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

namespace DiscordCoreInternal {

	struct LengthData {
		Uint64 offSet{};
		Uint64 length{};
	};

	class DiscordCoreAPI_Dll StringBuffer {
	  public:
		StringBuffer() noexcept;

		StringView operator[](LengthData);

		void writeData(const char*, Uint64);

		operator StringView();

		void erase(Uint64, Uint64);

		char operator[](Uint64);

		Uint64 size();

		auto begin() {
			if (this->whichOneAreWeOn == 0) {
				return this->theString01.begin();
			} else {
				return this->theString02.begin();
			}
		}

		auto end() {
			if (this->whichOneAreWeOn == 0) {
				return this->theString01.end();
			} else {
				return this->theString02.end();
			}
		}

		void clear();

		char* data();

	  protected:
		Uint64 whichOneAreWeOn{ 0 };
		std::string theString01{};
		std::string theString02{};
		Uint64 theSize{};
	};

	enum class RingBufferAccessType { Read = 0, Write = 1 };

	template<typename ObjectType, Uint64 TheSize> class RingBufferInterface {
	  public:
		void modifyReadOrWritePosition(RingBufferAccessType theType, Uint64 theSize) noexcept {
			if (theType == RingBufferAccessType::Read) {
				this->tail += theSize;
				this->areWeFull = false;
			} else {
				this->head += theSize;
				if (this->head == this->tail) {
					this->areWeFull = true;
				}
				if (this->head != this->tail) {
					this->areWeFull = false;
				}
			}
		}

		Uint64 getUsedSpace() noexcept {
			if (this->areWeFull) {
				return this->theArray.size();
			}
			if ((this->head % this->theArray.size()) >= (this->tail % this->theArray.size())) {
				Uint64 freeSpace = this->theArray.size() - ((this->head % this->theArray.size()) - (this->tail % this->theArray.size()));
				return this->theArray.size() - freeSpace;
			} else {
				Uint64 freeSpace = (this->tail % this->theArray.size()) - (this->head % this->theArray.size());
				return this->theArray.size() - freeSpace;
			}
		}

		Uint64 getFreeSpace() noexcept {
			return this->theArray.size() - this->getUsedSpace();
		}

		ObjectType* getCurrentTail() noexcept {
			return (this->theArray.data() + (this->tail % (this->theArray.size())));
		}

		ObjectType* getCurrentHead() noexcept {
			return (this->theArray.data() + (this->head % (this->theArray.size())));
		}

		Bool isItFull() noexcept {
			return this->areWeFull;
		}

		virtual void clear() noexcept {
			this->areWeFull = false;
			this->tail = 0;
			this->head = 0;
		}

	  protected:
		std::array<ObjectType, TheSize> theArray{};
		Bool areWeFull{ false };
		Uint64 tail{};
		Uint64 head{};
	};

	class DiscordCoreAPI_Dll RingBufferSlice : public RingBufferInterface<char, 1024 * 16> {};

	template<Uint64 TheSliceCount> class RingBuffer : public RingBufferInterface<RingBufferSlice, TheSliceCount> {
	  public:
		void clear() noexcept {
			for (Uint64 x = 0; x < this->theArray.size(); ++x) {
				this->theArray[x].clear();
			}
			this->areWeFull = false;
			this->tail = 0;
			this->head = 0;
		}
	};

}

#endif