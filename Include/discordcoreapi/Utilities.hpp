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
/// Utilities.cpp - Header file for the Utilities.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file Utilities.hpp

#pragma once

#include <jsonifier/Index.hpp>

#include <discordcoreapi/UnorderedSet.hpp>
#include <discordcoreapi/Etf.hpp>

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

/**
 * \defgroup discord_events Discord Events
 * \brief For all of events that could be sent by Discord's Websockets.
 */

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	template<typename ValueType>
	concept CopyableOrMovable = std::copyable<std::decay_t<ValueType>> || std::movable<std::decay_t<ValueType>>;

	/// \brief Time formatting methods.
	enum class TimeFormat : char {
		LongDate = 'D',///< "20 April 2021" - Long Date
		LongDateTime = 'F',///< "Tuesday, 20 April 2021 16:20" - Long Date/Time
		LongTime = 'T',///< "16:20:30" - Long Time
		ShortDate = 'd',///< "20/04/2021" - Short Date
		ShortDateTime = 'f',///< "20 April 2021 16:20" - Short Date/Time
		ShortTime = 't',///< "16:20" - Short Time
	};

	class DiscordCoreAPI_Dll Snowflake {
	  public:
		inline Snowflake() noexcept = default;

		inline Snowflake& operator=(const std::string& other) noexcept {
			for (auto& value: other) {
				if (!std::isdigit(static_cast<uint8_t>(value))) {
					return *this;
				}
			}
			if (other.size() == 0) {
				return *this;
			}
			id = stoull(other);
			return *this;
		}

		inline Snowflake(const std::string& other) noexcept {
			*this = other;
		}

		inline Snowflake& operator=(uint64_t other) noexcept {
			id = other;
			return *this;
		}

		inline Snowflake(uint64_t other) noexcept {
			*this = other;
		}

		inline operator std::string() const noexcept {
			return std::to_string(id);
		}

		inline explicit operator uint64_t() const noexcept {
			return id;
		}

		/// \brief Converts the
		/// snowflake-id into a time and date stamp. \returns std::string A
		/// std::string containing the timeStamp.
		std::string getCreatedAtTimeStamp(TimeFormat timeFormat);

		inline bool operator==(const Snowflake& rhs) const noexcept {
			return id == rhs.id;
		}

		friend inline std::string operator+(const std::string&, const Snowflake&) noexcept;

	  protected:
		uint64_t id{};
	};

	inline std::ostream& operator<<(std::ostream& os, Snowflake sf) {
		os << sf.operator std::string();
		return os;
	}

	inline std::string operator+(const std::string& lhs, const Snowflake& rhs) noexcept {
		std::string string{};
		string += lhs;
		string += std::to_string(rhs.id);
		return string;
	}

	template<typename TimeType> class StopWatch {
	  public:
		using HRClock = std::chrono::high_resolution_clock;

		inline StopWatch() = delete;

		inline StopWatch<TimeType>& operator=(StopWatch<TimeType>&& data) noexcept {
			maxNumberOfTimeUnits.store(data.maxNumberOfTimeUnits.load());
			startTime.store(data.startTime.load());
			return *this;
		}

		inline StopWatch(StopWatch<TimeType>&& other) noexcept = default;

		inline StopWatch(TimeType maxNumberOfMsNew) noexcept {
			maxNumberOfTimeUnits.store(maxNumberOfMsNew);
			resetTimer();
		}

		inline StopWatch(int64_t maxNumberOfMsNew) noexcept {
			maxNumberOfTimeUnits.store(TimeType{ maxNumberOfMsNew });
			resetTimer();
		}

		inline TimeType totalTimePassed() noexcept {
			return std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()) - startTime.load();
		}

		inline TimeType getTotalWaitTime() noexcept {
			return maxNumberOfTimeUnits.load();
		}

		inline bool hasTimePassed() noexcept {
			if (std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()) - startTime.load() >= maxNumberOfTimeUnits.load()) {
				return true;
			} else {
				return false;
			}
		}

		inline void resetTimer() noexcept {
			startTime.store(std::chrono::duration_cast<TimeType>(HRClock::now().time_since_epoch()));
		}

	  protected:
		std::atomic<TimeType> maxNumberOfTimeUnits{ TimeType{ 0 } };
		std::atomic<TimeType> startTime{ TimeType{ 0 } };
	};

	/// \brief Activity types.
	enum class ActivityType : uint8_t {
		Game = 0,///< Game.
		Streaming = 1,///< Streaming.
		Listening = 2,///< Listening.
		Watching = 3,///< Watching.
		Custom = 4,///< Custom.
		Competing = 5///< Competing.
	};

	/// \brief Activity data.
	struct DiscordCoreAPI_Dll ActivityData {
		std::string name{};///< Name of the activity.
		ActivityType type{};///< Activity data.
		std::string url{};///< Url associated with the activity.

		ActivityData() noexcept = default;

		virtual ~ActivityData() noexcept = default;
	};

	/// \brief For selecting the type of streamer that the given bot is, one must be one server and one of client per connection.
	enum class StreamType { None = 0, Client = 1, Server = 2 };

	/// \brief For connecting two bots to stream the VC contents between the two.
	struct DiscordCoreAPI_Dll StreamInfo {
		bool streamBotAudio{};///< Do we stream the audio coming from other bots?
		std::string address{};///< The address to connect to.
		StreamType type{};///< The type of streamer that this is. Set one to client and one to server.
		uint16_t port{};///< The port to connect to.
	};

};

namespace DiscordCoreInternal {

	using HRClock = std::chrono::high_resolution_clock;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;
	using SysClock = std::chrono::system_clock;
	using Seconds = std::chrono::seconds;
	using namespace std::literals;
	using std::cout;
	using std::endl;

	class DiscordCoreAPI_Dll SoundCloudRequestBuilder;
	class DiscordCoreAPI_Dll YouTubeRequestBuilder;
	class DiscordCoreAPI_Dll WebSocketClient;
	class DiscordCoreAPI_Dll BaseSocketAgent;
	class DiscordCoreAPI_Dll SoundCloudAPI;
	class DiscordCoreAPI_Dll YouTubeAPI;

	enum class WebSocketOpCode : uint8_t {
		Op_Continuation = 0x00,
		Op_Text = 0x01,
		Op_Binary = 0x02,
		Op_Close = 0x08,
		Op_Ping = 0x09,
		Op_Pong = 0x0a
	};

	/// \brief Websocket close codes.
	class DiscordCoreAPI_Dll WebSocketClose {
	  public:
		/// \brief Websocket close codes.
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
			Rate_Limited =
				1 << 9,///< Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
			Session_Timed = 1 << 10,///< Your session timed out. Reconnect and start a new one.
			Invalid_Shard = 1 << 11,///< You sent us an invalid shard when identifying.
			Sharding_Required =
				1 << 12,///< The session would have handled too many guilds - you are required to shard your connection in order to connect.
			Invalid_API_Version = 1 << 13,///< You sent an invalid version for the gateway.
			Invalid_Intent = 1 << 14,///< You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
			Disallowed_Intent = 1
				<< 15,///< You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
			We_Do_Reconnect = Normal_Close | Unknown_Error | Unknown_Opcode | Decode_Error | Not_Authenticated | Already_Authenticated | Invalid_Seq |
				Rate_Limited | Session_Timed,
			We_Do_Not_Reconnect = Authentication_Failed | Invalid_Shard | Sharding_Required | Invalid_API_Version | Invalid_Intent | Disallowed_Intent
		};

		inline static std::unordered_map<uint16_t, WebSocketCloseCode> mappingValues{ { 0, WebSocketCloseCode::Unset },
			{ 1000, WebSocketCloseCode::Normal_Close }, { 4000, WebSocketCloseCode::Unknown_Error }, { 4001, WebSocketCloseCode::Unknown_Opcode },
			{ 4002, WebSocketCloseCode::Decode_Error }, { 4003, WebSocketCloseCode::Not_Authenticated },
			{ 4004, WebSocketCloseCode::Authentication_Failed }, { 4005, WebSocketCloseCode::Already_Authenticated },
			{ 4007, WebSocketCloseCode::Invalid_Seq }, { 4008, WebSocketCloseCode::Rate_Limited }, { 4009, WebSocketCloseCode::Session_Timed },
			{ 4010, WebSocketCloseCode::Invalid_Shard }, { 4011, WebSocketCloseCode::Sharding_Required },
			{ 4012, WebSocketCloseCode::Invalid_API_Version }, { 4013, WebSocketCloseCode::Invalid_Intent },
			{ 4014, WebSocketCloseCode::Disallowed_Intent } };

		inline static std::unordered_map<WebSocketCloseCode, std::string> outputErrorValues{ {
																								 WebSocketCloseCode::Unknown_Error,
																								 "We're not sure what went wrong.",
																							 },
			{ WebSocketCloseCode::Unknown_Opcode, "You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!" },
			{ WebSocketCloseCode::Decode_Error, "You sent an invalid payload to Discord. Don't do that!" },
			{ WebSocketCloseCode::Not_Authenticated, "You sent us a payload prior to identifying." },
			{ WebSocketCloseCode::Authentication_Failed, "The account token sent with your identify payload is incorrect." },
			{ WebSocketCloseCode::Already_Authenticated, "You sent more than one identify payload. Don't do that!" },
			{ WebSocketCloseCode::Invalid_Seq, "The sequence sent when resuming the session was invalid. Reconnect and start a new session." },
			{ WebSocketCloseCode::Rate_Limited,
				"Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this." },
			{ WebSocketCloseCode::Session_Timed, "Your session timed out. Reconnect and start a new one." },
			{ WebSocketCloseCode::Invalid_Shard, "You sent us an invalid shard when identifying." },
			{ WebSocketCloseCode::Sharding_Required,
				"The session would have handled too many guilds - you are required to shard your connection in order to connect." },
			{ WebSocketCloseCode::Invalid_API_Version, "You sent an invalid version for the gateway." },
			{ WebSocketCloseCode::Invalid_Intent,
				"You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value." },
			{ WebSocketCloseCode::Disallowed_Intent,
				"You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not "
				"enabled or are "
				"not "
				"approved for." } };

		WebSocketCloseCode value{};

		WebSocketClose& operator=(uint16_t value);

		explicit WebSocketClose(uint16_t value);

		operator std::string();

		operator bool();
	};

	/// \brief Voice Websocket close codes.
	class DiscordCoreAPI_Dll VoiceWebSocketClose {
	  public:
		/// \brief Voice Websocket close codes.
		enum class VoiceWebSocketCloseCode : uint16_t {
			Unset = 1 << 0,///< Unset.
			Normal_Close = 1 << 1,///< Normal close.
			Unknown_Opcode = 1 << 2,///< You sent an invalid opcode.
			Failed_To_Decode = 1 << 3,///< You sent an invalid payload in your identifying to the Gateway.
			Not_Authenticated = 1 << 4,///< You sent a payload before identifying with the Gateway.
			Authentication_Failed = 1 << 5,///<	The token you sent in your identify payload is incorrect.
			Already_Authenticated = 1 << 6,///<	You sent more than one identify payload. Stahp.
			Session_No_Longer_Valid = 1 << 7,///< Your session is no longer valid.
			Session_Timeout = 1 << 8,///< Your session has timed out.
			Server_Not_Found = 1 << 9,///< We can't find the server you're trying to connect to.
			Unknown_Protocol = 1 << 10,///< We didn't recognize the protocol you sent.
			Disconnected = 1
				<< 11,///< Channel was deleted, you were kicked, voice server changed, or the main gateway session was dropped. Should not reconnect.
			Voice_Server_Crashed = 1 << 12,///< The server crashed. Our bad! Try resuming.
			Unknown_Encryption_Mode = 1 << 13///< We didn't recognize your encryption.
		};

		inline static std::unordered_map<uint16_t, VoiceWebSocketCloseCode> mappingValues{ { 0, VoiceWebSocketCloseCode::Unset },
			{ 1000, VoiceWebSocketCloseCode::Normal_Close }, { 4001, VoiceWebSocketCloseCode::Unknown_Opcode },
			{ 4002, VoiceWebSocketCloseCode::Failed_To_Decode }, { 4003, VoiceWebSocketCloseCode::Not_Authenticated },
			{ 4004, VoiceWebSocketCloseCode::Authentication_Failed }, { 4005, VoiceWebSocketCloseCode::Already_Authenticated },
			{ 4006, VoiceWebSocketCloseCode::Session_No_Longer_Valid }, { 4009, VoiceWebSocketCloseCode::Session_Timeout },
			{ 4011, VoiceWebSocketCloseCode::Server_Not_Found }, { 4012, VoiceWebSocketCloseCode::Unknown_Protocol },
			{ 4014, VoiceWebSocketCloseCode::Disconnected }, { 4015, VoiceWebSocketCloseCode::Voice_Server_Crashed },
			{ 4016, VoiceWebSocketCloseCode::Unknown_Encryption_Mode } };

		inline static std::unordered_map<VoiceWebSocketCloseCode, std::string> outputErrorValues{ { VoiceWebSocketCloseCode::Unset, "Unset." },
			{ VoiceWebSocketCloseCode::Normal_Close, "Normal close." }, { VoiceWebSocketCloseCode::Unknown_Opcode, "You sent an invalid opcode." },
			{ VoiceWebSocketCloseCode::Failed_To_Decode, "You sent an invalid payload in your identifying to the Gateway." },
			{ VoiceWebSocketCloseCode::Not_Authenticated, "You sent a payload before identifying with the Gateway." },
			{ VoiceWebSocketCloseCode::Authentication_Failed, "The token you sent in your identify payload is incorrect." },
			{ VoiceWebSocketCloseCode::Already_Authenticated, "You sent more than one identify payload. Stahp." },
			{ VoiceWebSocketCloseCode::Session_No_Longer_Valid, "Your session is no longer valid." },
			{ VoiceWebSocketCloseCode::Session_Timeout, "Your session has timed out." },
			{ VoiceWebSocketCloseCode::Server_Not_Found, "We can't find the server you're trying to connect to." },
			{ VoiceWebSocketCloseCode::Unknown_Protocol, "We didn't recognize the protocol you sent." },
			{ VoiceWebSocketCloseCode::Disconnected,
				"Channel was deleted, you were kicked, voice server changed, or the main gateway session was dropped. Should not "
				"reconnect." },
			{ VoiceWebSocketCloseCode::Voice_Server_Crashed, "The server crashed. Our bad! Try resuming." },
			{ VoiceWebSocketCloseCode::Unknown_Encryption_Mode, "We didn't recognize your encryption." } };

		VoiceWebSocketCloseCode value{};

		VoiceWebSocketClose& operator=(uint16_t value);

		VoiceWebSocketClose(uint16_t value);

		operator std::string();

		operator bool();
	};

	/// \brief Voice Websocket close codes.
	class DiscordCoreAPI_Dll HttpsResponseCode {
	  public:
		/// \brief Voice Websocket close codes.
		enum class HttpsResponseCodes : uint32_t {
			Ok = 200,///< The request completed successfully.
			Created = 201,///< The entity was created successfully.
			No_Content = 204,///< The request completed successfully but returned no content.
			Not_Modifies = 304,///< The entity was not modified (no action was taken).
			Bad_Request = 400,///< The request was improperly formatted, or the server couldn't understand it.
			Unauthorized = 401,///< The Authorization header was missing or invalid.
			Forbidden = 403,///< The Authorization token you passed did not have permission to the resource.
			Not_Found = 404,///< The resource at the location specified doesn't exist.
			Method_Not_Allowed = 405,///< The HTTPS method used is not valid for the location specified.
			Too_Many_Requests = 429,///< You are being rate limited, see Rate Limits.
			Gatewat_Unavailable = 502,///< There was not a gateway available to process your request. Wait a bit and retry.
		};

		inline static std::unordered_map<HttpsResponseCodes, std::string> outputErrorValues{ { static_cast<HttpsResponseCodes>(200),
																								 "The request completed successfully." },
			{ static_cast<HttpsResponseCodes>(201), "The entity was created successfully." },
			{ static_cast<HttpsResponseCodes>(204), "The request completed successfully but returned no content." },
			{ static_cast<HttpsResponseCodes>(304), "The entity was not modified (no action was taken)." },
			{ static_cast<HttpsResponseCodes>(400), "The request was improperly formatted, or the server couldn't understand it." },
			{ static_cast<HttpsResponseCodes>(401), "The Authorization header was missing or invalid." },
			{ static_cast<HttpsResponseCodes>(403), "The Authorization token you passed did not have permission to the resource." },
			{ static_cast<HttpsResponseCodes>(404), "The resource at the location specified doesn't exist." },
			{ static_cast<HttpsResponseCodes>(405), "The HTTPS method used is not valid for the location specified." },
			{ static_cast<HttpsResponseCodes>(429), "You are being rate limited, see Rate Limits." },
			{ static_cast<HttpsResponseCodes>(502), "There was not a gateway available to process your request.Wait a bit and retry." },
			{ static_cast<HttpsResponseCodes>(500), "The server had an error processing your request(these are rare)." } };

		HttpsResponseCodes value{};

		HttpsResponseCode& operator=(uint32_t value);

		HttpsResponseCode(uint32_t value);

		operator uint32_t();

		operator std::string();
	};

}// namespace DiscordCoreInternal


namespace DiscordCoreAPI {

	using HRClock = std::chrono::high_resolution_clock;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;
	using SysClock = std::chrono::system_clock;
	using Seconds = std::chrono::seconds;
	using namespace std::literals;
	using std::cout;
	using std::endl;

	struct DiscordCoreAPI_Dll OnVoiceServerUpdateData;
	struct DiscordCoreAPI_Dll OnVoiceStateUpdateData;
	struct DiscordCoreAPI_Dll File;

	class DiscordCoreAPI_Dll DiscordCoreClient;
	class DiscordCoreAPI_Dll VoiceConnection;
	class DiscordCoreAPI_Dll GuildMember;
	class DiscordCoreAPI_Dll ChannelData;
	class DiscordCoreAPI_Dll Reactions;
	class DiscordCoreAPI_Dll BotUser;

	template<typename RTy> class CoRoutine;

	/// \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		std::vector<ActivityData> activities{};///< A vector of activities.
		std::string status{};///< Current status.
		int64_t since{};///< When was the activity started?
		bool afk{};///< Are we afk.

		operator DiscordCoreInternal::EtfSerializer();
	};

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*function)( void ));

	/// \brief Input event response types.
	enum class InputEventResponseType : uint8_t {
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

	/// \brief Gateway intents.
	enum class GatewayIntents : uint32_t {
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
		Default_Intents = Guilds | Guild_Bans | Guild_Emojis | Guild_Integrations | Guild_Webhooks | Guild_Invites | Guild_VoiceStates |
			Guild_Messages | Guild_Message_Reactions | Guild_Message_Typing | Direct_Messages | Direct_Message_Reactions | Direct_Message_Typing |
			Guild_Scheduled_Events,///< Default intents (all non-privileged intents).
		Privileged_Intents = Guild_Members | Guild_Presences | Message_Content,///< Privileged intents requiring ID.
		All_Intents = Default_Intents | Privileged_Intents///< Every single intent.
	};

	/// \brief Function data for repeated functions to be loaded.
	struct DiscordCoreAPI_Dll RepeatedFunctionData {
		std::function<void(DiscordCoreClient*)> function{};///< The std::function char* to be loaded.
		uint32_t intervalInMs{};///< The time interval at which to call the std::function.
		bool repeated{};///< Whether or not the std::function is repeating.
		int64_t dummyArg{};
	};

	/// \brief Represents which text format to use for websocket transfer.
	enum class TextFormat : uint8_t {
		Etf = 0x00,///< Etf format.
		Json = 0x01///< Json format.
	};

	/// \brief Sharding options for the library.
	struct DiscordCoreAPI_Dll ShardingOptions {
		uint32_t numberOfShardsForThisProcess{ 1 };///< The number of shards to launch on the current process.
		uint32_t totalNumberOfShards{ 1 };///< The total number of shards that will be launched across all processes.
		uint32_t startingShard{};///< The first shard to start on this process.
	};

	/**@}*/

	/// \brief Loggin options for the library.
	struct DiscordCoreAPI_Dll LoggingOptions {
		bool logWebSocketSuccessMessages{};///< Do we log the websocket success messages to cout?
		bool logWebSocketErrorMessages{};///< Do we log the websocket error messages to cout?
		bool logGeneralSuccessMessages{};///< Do we log general success messages to cout?
		bool logGeneralErrorMessages{};///< Do we log general error messages to cout?
		bool logHttpsSuccessMessages{};///< Do we log Https response success messages to cout?
		bool logHttpsErrorMessages{};///< Do we log Https response error messages to cout?
	};

	/// \brief For selecting the caching style of the library.
	struct DiscordCoreAPI_Dll CacheOptions {
		bool cacheChannels{ true };///< Do we cache Channels?
		bool cacheGuilds{ true };///< Do we cache Guilds?
		bool cacheRoles{ true };///< Do we cache Roles?
		bool cacheUsers{ true };///< Do we cache Users/GuildMembers?
	};

	/// \brief Configuration data for the library's main class, DiscordCoreClient.
	struct DiscordCoreAPI_Dll DiscordCoreClientConfig {
		std::vector<RepeatedFunctionData> functionsToExecute{};///< Functions to execute after a timer, or on a repetition.
		GatewayIntents intents{ GatewayIntents::All_Intents };///< The gateway intents to be used for this instance.
		TextFormat textFormat{ TextFormat::Etf };///< Use ETF or JSON format for websocket transfer?
		UpdatePresenceData presenceData{};///< Presence data to initialize your bot with.
		std::string connectionAddress{};///< A potentially alternative connection address for the websocket.
		ShardingOptions shardOptions{};///< Options for the sharding of your bot.
		LoggingOptions logOptions{};///< Options for the output/logging of the library.
		CacheOptions cacheOptions{};///< Options for the cache of the library.
		uint16_t connectionPort{};///< A potentially alternative connection port for the websocket.
		std::string botToken{};///< Your bot's token.
	};

	struct DiscordCoreAPI_Dll JsonStringValue {
		DiscordCoreInternal::JsonType type{};
		Jsonifier::RawJsonData value{};
	};

	class DiscordCoreAPI_Dll ConfigManager {
	  public:
		ConfigManager() noexcept = default;

		explicit ConfigManager(const DiscordCoreClientConfig&);

		const bool doWePrintWebSocketSuccessMessages() const;

		const bool doWePrintWebSocketErrorMessages() const;

		const bool doWePrintHttpsSuccessMessages() const;

		const bool doWePrintHttpsErrorMessages() const;

		const bool doWePrintGeneralSuccessMessages() const;

		const bool doWePrintGeneralErrorMessages() const;

		const bool doWeCacheChannels() const;

		const bool doWeCacheUsers() const;

		const bool doWeCacheGuilds() const;

		const bool doWeCacheRoles() const;

		const UpdatePresenceData getPresenceData() const;

		const std::string getBotToken() const;

		const uint32_t getTotalShardCount() const;

		const uint32_t getStartingShard() const;

		const uint32_t getShardCountForThisProcess() const;

		const std::string getConnectionAddress() const;

		void setConnectionAddress(const std::string& connectionAddressNew);

		const uint16_t getConnectionPort() const;

		void setConnectionPort(const uint16_t connectionPortNew);

		const std::vector<RepeatedFunctionData> getFunctionsToExecute() const;

		const TextFormat getTextFormat() const;

		const GatewayIntents getGatewayIntents();

	  protected:
		DiscordCoreClientConfig config{};
	};

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// \brief Color constants for use in the EmbedData color values.
	namespace Colors {
		const std::string White = "FFFFFF",///< White.
			DiscordWhite = "FFFFFE",///< Discord white.
			LightGray = "C0C0C0",///< Light gray.
			Gray = "808080",///< Gray.
			DarkGray = "404040",///< Dark gray.
			Black = "000000",///< Black.
			DiscordBlack = "000001",///< Discord black.
			Red = "FF0000",///< Red.
			Pink = "FFAFAF",///< Pink.
			Orange = "FFC800",///< Orange.
			Yellow = "FFFF00",///< Yellow.
			Green = "00FF00",///< Green.
			Magenta = "FF00FF",///< Magenta.
			Cyan = "00FFFF",///< Cyan.
			Blue = "0000FF",///< Blue.
			LightSeaGreen = "1ABC9C",///< Light sea green.
			MediumSeaGreen = "2ECC71",///< Medium sea green.
			SummerSky = "3498DB",///< Summer skye.
			DeepLilac = "9B59B6",///< Deep lilac.
			Ruby = "E91E63",///< Ruby.
			MoonYellow = "F1C40F",///< Moon yellow.
			TahitiGold = "E67E22",///< Tahiti gold.
			Cinnabar = "E74C3C",///< Cinnabar.
			Submarine = "95A5A6",///< Submarine.
			BlueAquamarine = "607D8B",///< Blue aquamarine.
			DeepSea = "11806A",///< Deep sea.
			SeaGreen = "1F8B4C",///< Sea green.
			Endeavour = "206694",///< Endeavor.
			VividViolet = "71368A",///< Vivid violet.
			JazzberryJam = "AD1457",///< Jazzberry jam.
			DarkGoldenrod = "C27C0E",///< Dark goldenrod.
			Rust = "A84300",///< Rust.
			Brown = "992D22",///< Brown.
			GrayChateau = "979C9F",///< Gray chateau.
			Bismark = "546E7A",///< Bismark.
			StiBlue = "0E4BEF",///< Sti blue.
			WrxBlue = "00247D",///< Wrx blue.
			RalliArtCrimson = "E60012",///< Ralliart crimson.
			Lime = "00FF00",///< Lime.
			ForestGreen = "228B22",///< Forest green.
			CadmiumGreen = "097969",///< Cadmium green.
			Aquamarine = "7FFFD4",///< Aquamarine.
			BlueGreen = "088F8F",///< Blue green.
			Raspberry = "E30B5C",///< Raspberry.
			ScarletRed = "FF2400";///< Scarlet red.
	};

	/**@}*/

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// \brief Audio frame types.
	enum class AudioFrameType : uint8_t {
		Unset = 0,///< Unset.
		RawPCM = 1,///< Raw PCM.
		Encoded = 2,///< Encoded audio data.
	};

	/// \brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll AudioFrameData {
		AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
		std::vector<uint8_t> data{};///< The audio data.
		int64_t currentSize{ -5 };///< The current size of the allocated memory.
		uint64_t guildMemberId{};///< GuildMemberId for the sending GuildMember.

		AudioFrameData() noexcept = default;

		AudioFrameData& operator=(AudioFrameData&&) noexcept = default;

		AudioFrameData(AudioFrameData&&) noexcept = default;

		AudioFrameData& operator=(const AudioFrameData&) noexcept = default;

		AudioFrameData(const AudioFrameData&) noexcept = default;

		AudioFrameData& operator+=(std::basic_string_view<uint8_t>) noexcept;

		AudioFrameData& operator+=(uint8_t);

		friend bool operator==(const AudioFrameData& lhs, const AudioFrameData& rhs);

		void clearData() noexcept;
	};

	/// For connecting to a voice-channel. "streamInfo" is used when a socket is created to connect this bot to another bot, for transmitting audio back and forth.
	/// \brief For connecting to a voice-channel. "streamInfo" is used when a socket is created to connect this bot to another bot, for transmitting audio back and forth.
	struct DiscordCoreAPI_Dll VoiceConnectInitData {
		StreamInfo streamInfo{};///< The info for the stream-socekt, if applicable.
		int32_t currentShard{};///< The current websocket shard, if applicable.
		Snowflake channelId{};///< The channel id to connect to.
		Snowflake guildId{};///< The guild id to connect to.
		Snowflake userId{};///< This bot's user id.
		bool selfDeaf{};///< Self-deafen the bot?
		bool selfMute{};///< Self-mute the bot?
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename RTy> RTy inline fromString(const std::string& string, std::ios_base& (*type)( std::ios_base& )) {
		RTy value{};
		std::istringstream stream(string);
		stream >> type, stream >> value;
		return value;
	}

	template<typename RTy> std::string inline toHex(RTy inputValue) {
		std::stringstream stream{};
		stream << std::setfill('0') << std::setw(sizeof(RTy) * 2) << std::hex << inputValue;
		return stream.str();
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
		friend struct Jsonifier::Core<ColorValue>;

		ColorValue(std::string hexColorValue);

		ColorValue(uint32_t colorValue);

		RGBColorValue getRgbColorValue();

		HexColorValue getHexColorValue();

		uint32_t getIntColorValue();

	  protected:
		uint32_t color{};
	};

	enum class HashType {
		User_Avatar = 0,
		Channel_Icon = 1,
		GuildMember_Avatar = 2,
		Guild_Icon = 3,
		Guild_Splash = 4,
		Guild_Banner = 5,
		Guild_Discovery = 6
	};

	class DiscordCoreAPI_Dll IconHash {
	  public:
		IconHash() noexcept = default;

		IconHash& operator=(const std::string& string);

		IconHash(const std::string& string) noexcept;

		bool operator==(const IconHash& other);

		operator std::string() noexcept;

		~IconHash() noexcept = default;

	  protected:
		uint64_t highBits{};
		uint64_t lowBits{};
	};

	/**@}*/

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief Permission values, for a given Channel, by Role or GuildMember.
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

	/// \brief Permissions class, for representing and manipulating Permission values.
	class DiscordCoreAPI_Dll Permissions {
	  public:
		friend struct JsonifierInternal::ParseWithKeys;
		friend struct JsonifierInternal::ParseNoKeys;

		Permissions() noexcept = default;

		Permissions& operator=(Permission&& other);

		Permissions& operator=(const Permission& other);

		explicit Permissions(const Permission& permsNew);

		Permissions& operator=(std::string&& other);

		explicit Permissions(std::string&& permsNew);

		Permissions& operator=(const std::string& other);

		explicit Permissions(const std::string& permsNew);

		Permissions& operator=(const uint64_t other);

		explicit Permissions(const uint64_t permsNew);

		explicit operator uint64_t();

		explicit operator std::string();

		/// \brief Returns a string containing all of a given User's Permissions for a given Channel.
		/// \param guildMember The GuildMember who's Permissions to analyze.
		/// \param channel The Channel withint which to check for Permissions.
		/// \returns std::string A string containing the final Permission's value for a given Channel.
		static std::string getCurrentChannelPermissions(const GuildMember& guildMember, const ChannelData& channel);

		/// \brief Checks for a given Permission in a chosen Channel, for a specific User.
		/// \param guildMember The GuildMember who to check the Permissions of.
		/// \param channel The Channel within which to check for the Permission's presence.
		/// \param permission A Permission to check the current Channel for.
		/// \returns bool A bool suggesting the presence of the chosen Permission.
		bool checkForPermission(const GuildMember& guildMember, const ChannelData& channel, Permission permission);

		/// \brief Returns a string containing the currently held Permissions in a given Guild.
		/// \param guildMember The GuildMember who's Permissions are to be evaluated.
		/// \returns std::string A string containing the current Permissions.
		static std::string getCurrentGuildPermissions(const GuildMember& guildMember);

		/// \brief Removes one or more Permissions from the current Permissions value.
		/// \param permissionsToRemove A vector containing the Permissions you wish to remove.
		void removePermissions(const std::vector<Permission>& permissionsToRemove);

		/// \brief Adds one or more Permissions to the current Permissions value.
		/// \param permissionsToAdd A vector containing the Permissions you wish to add.
		void addPermissions(const std::vector<Permission>& permissionsToAdd);

		/// \brief Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format.
		/// \returns std::vector A vector full of strings of the Permissions that are in the input std::string's value.
		std::vector<std::string> displayPermissions();

		/// \brief Returns a string containing the currently held Permissions.
		/// \returns std::string A string containing the current Permissions.
		std::string getCurrentPermissionString();

		/// \brief Returns a string containing ALL of the possible Permissions.
		/// \returns std::string A string containing all of the possible Permissions.
		static std::string getAllPermissions();

	  protected:
		uint64_t permissions{};

		static std::string computeOverwrites(const std::string& basePermissions, const GuildMember& guildMember, const ChannelData& channel);

		static std::string computePermissions(const GuildMember& guildMember, const ChannelData& channel);

		static std::string computeBasePermissions(const GuildMember& guildMember);
	};

	/// \brief Prints the current file, line, and column from which the function is being called - typically from within an exception's "catch" block.
	/// \param currentFunctionName A string to display the current function's name.
	/// \param location For deriving the current file, line, and column - do not set this value.
	/// \param ptr A pointer to the current exception.
	DiscordCoreAPI_Dll void reportException(const std::string& currentFunctionName, std::source_location location = std::source_location::current(),
		std::exception_ptr ptr = nullptr);

	DiscordCoreAPI_Dll void rethrowException(const std::string& currentFunctionName, std::source_location location = std::source_location::current());

	DiscordCoreAPI_Dll std::string constructMultiPartData(const std::string& data, const std::vector<File>& files);

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

	/// \brief Acquires a timeStamp with the current time and date - suitable for use in message-embeds.
	/// \returns std::string A string containing the current date-time stamp.
	DiscordCoreAPI_Dll std::string getTimeAndDate();

	/// \brief Class for representing a timeStamp, as well as working with time-related values.
	class DiscordCoreAPI_Dll TimeStamp {
	  public:
		TimeStamp(TimeFormat formatNew = TimeFormat::LongDateTime);

		TimeStamp(std::string year, std::string month, std::string day, std::string hour, std::string minute, std::string second,
			TimeFormat formatNew);

		operator std::string();

		operator uint64_t();

		TimeStamp& operator=(std::string&& originalTimeStampNew);

		TimeStamp(std::string&& originalTimeStampNew);

		TimeStamp& operator=(std::string& originalTimeStampNew);

		TimeStamp(std::string& originalTimeStampNew);

		TimeStamp& operator=(const TimeStamp& other) {
			timeStampInTimeUnits = other.timeStampInTimeUnits;
			return *this;
		}

		TimeStamp(const TimeStamp& other) {
			*this = other;
		}

		TimeStamp(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, TimeFormat formatNew);

		TimeStamp(uint64_t timeInTimeUnits, TimeFormat formatNew);

		static std::string convertToFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd,
			int32_t yearsToAdd, TimeFormat formatNew);

		static std::string convertToCurrentISO8601TimeStamp(TimeFormat timeFormat);

		bool hasTimeElapsed(uint64_t days, uint64_t hours, uint64_t minutes);

		static std::string convertMsToDurationString(uint64_t durationInMs);

	  protected:
		uint64_t timeStampInTimeUnits{};

		void getTimeSinceEpoch(int64_t year, int64_t month, int64_t day, int64_t hour, int64_t minute, int64_t second);

		void convertTimeStampToTimeUnits(TimeFormat formatNew, std::string originalTimeStamp);

		std::string getISO8601TimeStamp(TimeFormat timeFormat);
	};

	template<typename ITy>
	concept IsInteger = std::is_integral<ITy>::value;

	template<IsInteger StoredAs, DiscordCoreInternal::EnumT ValueType>
	inline auto setBool(StoredAs theFlags, ValueType theFlagToSet, bool enabled) {
		auto theValue{ theFlags };
		if (enabled) {
			theValue |= std::to_underlying(theFlagToSet);
		} else {
			theValue &= ~std::to_underlying(theFlagToSet);
		}
		return theValue;
	}

	template<DiscordCoreInternal::EnumT ValueType> inline bool getBool(ValueType theFlags, ValueType theFlagToCheckFor) {
		return std::to_underlying(theFlags) & std::to_underlying(theFlagToCheckFor);
	}

	template<typename KTy, typename ValueType> class ObjectCache {
	  public:
		using key_type = KTy;
		using mapped_type = ValueType;
		using reference = mapped_type&;
		using pointer = mapped_type*;

		inline ObjectCache& operator=(ObjectCache&& other) noexcept {
			if (this != &other) {
				std::unique_lock lock01{ other.cacheMutex };
				std::unique_lock lock02{ cacheMutex };
				std::swap(cacheMap, other.cacheMap);
			}
			return *this;
		}

		inline ObjectCache(ObjectCache&& other) noexcept {
			*this = std::move(other);
		}

		inline ObjectCache& operator=(const ObjectCache& other) noexcept {
			if (this != &other) {
				cacheMap = other.cacheMap;
			}
			return *this;
		}

		inline ObjectCache(const ObjectCache& other) noexcept {
			*this = other;
		}

		inline constexpr ObjectCache(){};

		inline constexpr void emplace(mapped_type&& object) {
			std::unique_lock lock(cacheMutex);
			cacheMap.emplace(std::forward<mapped_type>(object));
		}

		inline constexpr void emplace(const mapped_type& object) {
			std::unique_lock lock(cacheMutex);
			cacheMap.emplace(object);
		}

		inline constexpr reference operator[](key_type&& key) {
			return find(std::forward<key_type>(key));
		}

		inline constexpr reference operator[](const key_type& key) {
			return find(key);
		}

		inline constexpr void erase(key_type key) {
			std::unique_lock lock(cacheMutex);
			auto existing = cacheMap.find(key);
			if (existing != cacheMap.end()) {
				cacheMap.erase(key);
			}
		}

		inline constexpr auto begin() const {
			return cacheMap.begin();
		}

		inline constexpr auto end() const {
			return cacheMap.end();
		}

		inline constexpr auto begin() {
			return cacheMap.begin();
		}

		inline constexpr auto end() {
			return cacheMap.end();
		}

		inline constexpr bool contains(key_type key) {
			return cacheMap.contains(key);
		}

		inline constexpr reference find(key_type key) {
			std::shared_lock lock(cacheMutex);
			auto r = cacheMap.find(key);
			if (r != cacheMap.end()) {
				return *r;
			}
			return *end();
		}

		inline constexpr uint64_t count() {
			std::shared_lock lock(cacheMutex);
			return cacheMap.size();
		}

		inline constexpr ~ObjectCache(){};

	  protected:
		UnorderedSet<key_type, mapped_type> cacheMap{};
		std::shared_mutex cacheMutex{};
	};

	/// \brief A thread-safe messaging block for data-structures.
	/// \tparam ValueType The type of object that will be sent over
	///  message block.
	template<CopyableOrMovable ValueType> class UnboundedMessageBlock {
	  public:
		inline UnboundedMessageBlock() noexcept {};

		inline UnboundedMessageBlock<std::decay_t<ValueType>>& operator=(UnboundedMessageBlock<std::decay_t<ValueType>>&& other) noexcept {
			if (this != &other) {
				std::lock(accessMutex, other.accessMutex);
				std::lock_guard lock{ accessMutex, std::adopt_lock };
				std::lock_guard otherLock{ other.accessMutex, std::adopt_lock };
				queue = std::move(other.queue);
			}
			return *this;
		}

		inline UnboundedMessageBlock(UnboundedMessageBlock&& other) noexcept {
			std::lock_guard lock{ other.accessMutex };
			queue = std::move(other.queue);
		}

		inline UnboundedMessageBlock<std::decay_t<ValueType>>& operator=(const UnboundedMessageBlock<std::decay_t<ValueType>>&) = delete;
		inline UnboundedMessageBlock(const UnboundedMessageBlock&) = delete;

		inline ~UnboundedMessageBlock() = default;

		inline void send(ValueType&& object) {
			std::lock_guard lock{ accessMutex };
			queue.emplace_back(std::move(object));
		}

		inline void clearContents() {
			std::lock_guard lock{ accessMutex };
			queue.clear();
		}

		inline bool tryReceive(ValueType& object) {
			std::lock_guard lock{ accessMutex };
			if (queue.size() > 0) {
				object = std::move(queue.front());
				queue.pop_front();
				return true;
			} else {
				return false;
			}
		}

		inline uint64_t size() const {
			std::lock_guard lock{ accessMutex };
			return queue.size();
		}

	  protected:
		std::deque<std::decay_t<ValueType>> queue{};
		mutable std::mutex accessMutex{};
	};

	template<typename RTy> class NewThreadAwaiter;

	/// \brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// \tparam RTy The type of value returned by the containing CoRoutine.
	template<typename RTy> inline auto NewThreadAwaitable() {
		return NewThreadAwaiter<RTy>{};
	}

	/// \brief Typedef for the message filter.
	template<typename ValueType> using ObjectFilter = std::function<bool(ValueType)>;

	template<typename ValueType>
	inline bool waitForTimeToPass(UnboundedMessageBlock<std::decay_t<ValueType>>& outBuffer, ValueType& argOne, int32_t timeInMsNew) {
		StopWatch stopWatch{ Milliseconds{ timeInMsNew } };
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

	enum class RingBufferAccessType { Read = 0, Write = 1 };

	template<typename ValueType, uint64_t Size> class RingBufferInterface {
	  public:
		template<typename ValueType2, uint64_t SliceCount> friend class RingBuffer;

		inline constexpr void modifyReadOrWritePosition(RingBufferAccessType type, uint64_t size) noexcept {
			if (type == RingBufferAccessType::Read) {
				tail += size;
			} else {
				head += size;
			}
		}

		inline constexpr bool isItEmpty() {
			return tail == head;
		}

		inline constexpr bool isItFull() {
			return getUsedSpace() == Size;
		}

		inline constexpr size_t getUsedSpace() {
			return head - tail;
		}

		inline constexpr ValueType* getCurrentTail() noexcept {
			return arrayValue.data() + (tail % Size);
		}

		inline constexpr ValueType* getCurrentHead() noexcept {
			return arrayValue.data() + (head % Size);
		}

		inline constexpr virtual void clear() noexcept {
			tail = 0;
			head = 0;
		}

	  protected:
		std::array<std::decay_t<ValueType>, Size> arrayValue{};
		uint64_t tail{};
		uint64_t head{};
	};

	template<typename ValueType, uint64_t SliceCount> class RingBuffer
		: public RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount> {
	  public:
		inline constexpr void clear() noexcept {
			for (uint64_t x = 0; x < SliceCount; ++x) {
				RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::arrayValue[x].tail = 0;
				RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::arrayValue[x].head = 0;
			}
			RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::tail = 0;
			RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::head = 0;
		}

		inline constexpr void modifyReadOrWritePosition(RingBufferAccessType type, uint64_t size) noexcept {
			RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::modifyReadOrWritePosition(type, size);
		}

		inline constexpr uint64_t getUsedSpace() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getUsedSpace();
		}

		inline constexpr auto getCurrentTail() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentTail();
		}

		inline constexpr auto isItFull() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::isItFull();
		}

		inline constexpr auto getCurrentHead() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentHead();
		}

		inline constexpr void writeData(std::basic_string_view<std::decay_t<ValueType>> data) {
			if (isItFull() || data.size() > 16384 - getCurrentHead()->getUsedSpace()) {
				getCurrentTail()->clear();
				modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
			}
			uint64_t writeSize{ data.size() };
			std::memcpy(getCurrentHead()->getCurrentHead(), data.data(), data.size());
			getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, writeSize);
			modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
		}

		inline constexpr std::basic_string_view<std::decay_t<ValueType>> readData() {
			std::basic_string_view<std::decay_t<ValueType>> returnData{};
			if (getCurrentTail()->getUsedSpace() > 0) {
				returnData = std::basic_string_view<std::decay_t<ValueType>>{ getCurrentTail()->getCurrentTail(), getCurrentTail()->getUsedSpace() };
				getCurrentTail()->clear();
				modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
			}
			return returnData;
		}
	};
}
