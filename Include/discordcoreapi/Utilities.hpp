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
/// Utilities.cpp - Header file for the Utilities.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file Utilities.hpp

#pragma once

#include <discordcoreapi/Utilities/Base.hpp>
#include <discordcoreapi/Utilities/UnorderedSet.hpp>
#include <discordcoreapi/Utilities/UnorderedMap.hpp>
#include <discordcoreapi/Utilities/ObjectCache.hpp>
#include <discordcoreapi/Utilities/UnboundedMessageBlock.hpp>
#include <discordcoreapi/Utilities/Etf.hpp>
#include <coroutine>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief Activity types.
	enum class ActivityType : uint8_t {
		Game	  = 0,///< Game.
		Streaming = 1,///< Streaming.
		Listening = 2,///< Listening.
		Watching  = 3,///< Watching.
		Custom	  = 4,///< Custom.
		Competing = 5///< Competing.
	};

	/// @brief Activity data.
	struct ActivityData {
		UnorderedSet<std::string> excludedKeys{};
		Snowflake applicationId{};///< Application ID for the game.
		std::string created_at{};///< Unix timestamp(in milliseconds) of when the activity was added to the user's session.
		std::string details{};///< What the player is currently doing.
		ActivityType type{};///< Activity's type.
		std::string state{};///< User's current party status, or text used for a custom status.
		std::string name{};///< Name of the activity.
		std::string url{};///< Stream URL, is validated when type is 1.

		ActivityData() = default;

		~ActivityData() = default;
	};

	/// @brief For connecting two bots to stream the VC contents between the two.
	struct StreamInfo {
		bool streamBotAudio{};///< Do we stream the audio coming from other bots?
		std::string address{};///< The address to connect to.
		StreamType type{};///< The type of streamer that this is. Set one to client and one to server.
		uint16_t port{};///< The port to connect to.
	};

	namespace DiscordCoreInternal {

		class SoundCloudRequestBuilder;
		class YouTubeRequestBuilder;
		class WebSocketClient;
		class BaseSocketAgent;
		class SoundCloudAPI;
		class YouTubeAPI;

	}// namespace DiscordCoreInternal

	struct OnVoiceServerUpdateData;
	struct OnVoiceStateUpdateData;
	struct File;

	class DiscordCoreClient;
	class CommandController;
	class VoiceConnection;
	class GuildMemberData;
	class GuildMembers;
	class ChannelData;
	class Reactions;
	class RoleData;
	class BotUser;

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename ReturnType, bool timeOut = true> class CoRoutine;

	/**@}*/

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	enum class PresenceUpdateState { Online = 0, Do_Not_Disturb = 1, Idle = 2, Invisible = 3, Offline = 4 };

	/// @brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		template<typename ValueType> friend struct jsonifier::core;
		jsonifier::vector<ActivityData> activities{};///< A vector of activities.
		UnorderedSet<std::string> excludedKeys{};
		PresenceUpdateState status{};///< Current status.
		int64_t since{};///< When was the activity started?
		bool afk{};///< Are we afk.

		inline UpdatePresenceData() = default;
		UpdatePresenceData(PresenceUpdateState updateState);

		operator DiscordCoreInternal::EtfSerializer();

	  protected:
		std::string statusReal{};
	};

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, const std::string& (*function)( void ));

	/// @brief Input event response types.
	enum class InputEventResponseType : uint8_t {
		Unset									= 0,///< Unset.
		Deferred_Response						= 1,
		Ephemeral_Deferred_Response				= 2,///< Deferred ephemeral response.
		Interaction_Response					= 3,///< Interaction response.
		Ephemeral_Interaction_Response			= 4,///< Ephemeral Interaction response.
		Edit_Interaction_Response				= 5,///< Interaction response edit.
		Follow_Up_Message						= 6,///< Follow-up MessageData.
		Ephemeral_Follow_Up_Message				= 7,///< Ephemeral follow-up MessageData.
		Edit_Follow_Up_Message					= 8,///< Follow-up Message edit.
		Application_Command_AutoComplete_Result = 9,///< Respond to an autocomplete interaction with suggested choices.
		Modal_Interaction_Response				= 10,///< Respond to an interaction with a popup modal.
	};

	/// @brief Gateway intents.
	enum class GatewayIntents : uint32_t {
		Guilds						  = 1 << 0,///< Intent for receipt of Guild information.
		Guild_Members				  = 1 << 1,///< Intent for receipt of Guild members.
		Guild_Bans					  = 1 << 2,///< Intent for receipt of Guild bans.
		Guild_Emojis				  = 1 << 3,///< Intent for receipt of Guild emojis.
		Guild_Integrations			  = 1 << 4,///< Intent for receipt of Guild integrations.
		Guild_Webhooks				  = 1 << 5,///< Intent for receipt of Guild webhooks.
		Guild_Invites				  = 1 << 6,///< Intent for receipt of Guild invites.
		Guild_VoiceStates			  = 1 << 7,///< Intent for receipt of Guild voice states.
		Guild_Presences				  = 1 << 8,///< Intent for receipt of Guild presences.
		Guild_Messages				  = 1 << 9,///< Intent for receipt of Guild messages.
		Guild_Message_Reactions		  = 1 << 10,///< Intent for receipt of Guild message reactions.
		Guild_Message_Typing		  = 1 << 11,///< Intent for receipt of Guild message typing notifications.
		Direct_Messages				  = 1 << 12,///< Intent for receipt of direct messages (DMs).
		Direct_Message_Reactions	  = 1 << 13,///< Intent for receipt of direct message reactions.
		Direct_Message_Typing		  = 1 << 14,///< Intent for receipt of direct message typing notifications.
		Message_Content				  = 1 << 15,///< Intent for receipt of message content.
		Guild_Scheduled_Events		  = 1 << 16,///< Scheduled events.
		Auto_Moderation_Configuration = 1 << 20,/// Auto moderation configuration.
		Auto_Moderation_Execution	  = 1 << 21,///< Auto moderation execution.
		Default_Intents = Guilds | Guild_Bans | Guild_Emojis | Guild_Integrations | Guild_Webhooks | Guild_Invites | Guild_VoiceStates | Guild_Messages | Guild_Message_Reactions |
			Guild_Message_Typing | Direct_Messages | Direct_Message_Reactions | Direct_Message_Typing | Guild_Scheduled_Events | Auto_Moderation_Configuration |
			Auto_Moderation_Execution,///< Default intents (all non-privileged intents).
		Privileged_Intents = Guild_Members | Guild_Presences | Message_Content,///< Privileged intents requiring ID.
		All_Intents		   = Default_Intents | Privileged_Intents///< Every single intent.
	};

	/// @brief Function data for repeated functions to be loaded.
	struct RepeatedFunctionData {
		std::function<void(DiscordCoreClient*)> function{};///< The std::function char* to be loaded.
		uint32_t intervalInMs{};///< The time interval at which to call the std::function.
		bool repeated{};///< Whether or not the std::function is repeating.
		int64_t dummyArg{};
	};

	/// @brief Represents which text format to use for websocket transfer.
	enum class TextFormat : uint8_t {
		Etf	 = 0x00,///< Etf format.
		Json = 0x01///< Json format.
	};

	/// @brief Sharding options for the library.
	struct ShardingOptions {
		uint32_t numberOfShardsForThisProcess{ 1 };///< The number of shards to launch on the current process.
		uint32_t totalNumberOfShards{ 1 };///< The total number of shards that will be launched across all processes.
		uint32_t startingShard{};///< The first shard to start on this process.
	};

	/// @brief Loggin options for the library.
	struct LoggingOptions {
		bool logWebSocketSuccessMessages{};///< Do we log the websocket success messages to std::cout?
		bool logWebSocketErrorMessages{};///< Do we log the websocket error messages to std::cout?
		bool logGeneralSuccessMessages{};///< Do we log general success messages to std::cout?
		bool logGeneralErrorMessages{};///< Do we log general error messages to std::cout?
		bool logHttpsSuccessMessages{};///< Do we log Https response success messages to std::cout?
		bool logHttpsErrorMessages{};///< Do we log Https response error messages to std::cout?
	};

	/// @brief For selecting the caching style of the library.
	struct CacheOptions {
		bool cacheGuildMembers{ true };///< Do we cache GuildMembers?
		bool cacheVoiceStates{ true };///< Do we cache Voices States?
		bool cacheChannels{ true };///< Do we cache Channels?
		bool cacheGuilds{ true };///< Do we cache Guilds?
		bool cacheRoles{ true };///< Do we cache Roles?
		bool cacheUsers{ true };///< Do we cache Users?
	};

	/// @brief Configuration data for the library's main class, DiscordCoreClient.
	struct DiscordCoreClientConfig {
		UpdatePresenceData presenceData{ PresenceUpdateState::Online };///< Presence data to initialize your bot with.
		jsonifier::vector<RepeatedFunctionData> functionsToExecute{};///< Functions to execute after a timer, or on a repetition.
		GatewayIntents intents{ GatewayIntents::All_Intents };///< The gateway intents to be used for this instance.
		TextFormat textFormat{ TextFormat::Etf };///< Use ETF or JSON format for websocket transfer?
		std::string connectionAddress{};///< A potentially alternative connection address for the websocket.
		ShardingOptions shardOptions{};///< Options for the sharding of your bot.
		LoggingOptions logOptions{};///< Options for the output/logging of the library.
		CacheOptions cacheOptions{};///< Options for the cache of the library.
		uint16_t connectionPort{};///< A potentially alternative connection port for the websocket.
		std::string botToken{};///< Your bot's token.
	};

	struct JsonStringValue {
		DiscordCoreInternal::JsonType type{};
		jsonifier::raw_json_data value{};
	};

	class DiscordCoreAPI_Dll ConfigManager {
	  public:
		ConfigManager() = default;

		explicit ConfigManager(const DiscordCoreClientConfig&);

		bool doWePrintWebSocketSuccessMessages() const;

		bool doWePrintWebSocketErrorMessages() const;

		bool doWePrintHttpsSuccessMessages() const;

		bool doWePrintHttpsErrorMessages() const;

		bool doWePrintGeneralSuccessMessages() const;

		bool doWePrintGeneralErrorMessages() const;

		bool doWeCacheGuildMembers() const;

		bool doWeCacheVoiceStates() const;

		bool doWeCacheChannels() const;

		bool doWeCacheUsers() const;

		bool doWeCacheGuilds() const;

		bool doWeCacheRoles() const;

		UpdatePresenceData getPresenceData() const;

		std::string getBotToken() const;

		uint32_t getTotalShardCount() const;

		uint32_t getStartingShard() const;

		uint32_t getShardCountForThisProcess() const;

		std::string getConnectionAddress() const;

		void setConnectionAddress(const std::string& connectionAddressNew);

		uint16_t getConnectionPort() const;

		void setConnectionPort(const uint16_t connectionPortNew);

		jsonifier::vector<RepeatedFunctionData> getFunctionsToExecute() const;

		TextFormat getTextFormat() const;

		GatewayIntents getGatewayIntents();

	  protected:
		DiscordCoreClientConfig config{};
	};

	/// @brief Color constants for use in the EmbedData color values.
	namespace Colors {
		static constexpr std::string_view White{ "FFFFFF" },///< White.
			DiscordWhite{ "FFFFFE" },///< Discord white.
			LightGray{ "C0C0C0" },///< Light gray.
			Gray{ "808080" },///< Gray.
			DarkGray{ "404040" },///< Dark gray.
			Black{ "000000" },///< Black.
			DiscordBlack{ "000001" },///< Discord black.
			Red{ "FF0000" },///< Red.
			Pink{ "FFAFAF" },///< Pink.
			Orange{ "FFC800" },///< Orange.
			Yellow{ "FFFF00" },///< Yellow.
			Green{ "00FF00" },///< Green.
			Magenta{ "FF00FF" },///< Magenta.
			Cyan{ "00FFFF" },///< Cyan.
			Blue{ "0000FF" },///< Blue.
			LightSeaGreen{ "1ABC9C" },///< Light sea green.
			MediumSeaGreen{ "2ECC71" },///< Medium sea green.
			SummerSky{ "3498DB" },///< Summer skye.
			DeepLilac{ "9B59B6" },///< Deep lilac.
			Ruby{ "E91E63" },///< Ruby.
			MoonYellow{ "F1C40F" },///< Moon yellow.
			TahitiGold{ "E67E22" },///< Tahiti gold.
			Cinnabar{ "E74C3C" },///< Cinnabar.
			Submarine{ "95A5A6" },///< Submarine.
			BlueAquamarine{ "607D8B" },///< Blue aquamarine.
			DeepSea{ "11806A" },///< Deep sea.
			SeaGreen{ "1F8B4C" },///< Sea green.
			Endeavour{ "206694" },///< Endeavor.
			VividViolet{ "71368A" },///< Vivid violet.
			JazzberryJam{ "AD1457" },///< Jazzberry jam.
			DarkGoldenrod{ "C27C0E" },///< Dark goldenrod.
			Rust{ "A84300" },///< Rust.
			Brown{ "992D22" },///< Brown.
			GrayChateau{ "979C9F" },///< Gray chateau.
			Bismark{ "546E7A" },///< Bismark.
			StiBlue{ "0E4BEF" },///< Sti blue.
			WrxBlue{ "00247D" },///< Wrx blue.
			RalliArtCrimson{ "E60012" },///< Ralliart crimson.
			Lime{ "00FF00" },///< Lime.
			ForestGreen{ "228B22" },///< Forest green.
			CadmiumGreen{ "097969" },///< Cadmium green.
			Aquamarine{ "7FFFD4" },///< Aquamarine.
			BlueGreen{ "088F8F" },///< Blue green.
			Raspberry{ "E30B5C" },///< Raspberry.
			ScarletRed{ "FF2400" };///< Scarlet red.
	};

	/**@}*/

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// @brief Audio frame types.
	enum class AudioFrameType : uint8_t {
		Unset	= 0,///< Unset.
		RawPCM	= 1,///< Raw PCM.
		Encoded = 2,///< Encoded audio data.
	};

	/// @brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll AudioFrameData {
		AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
		jsonifier::vector<uint8_t> data{};///< The audio data.
		int64_t currentSize{ -5 };///< The current size of the allocated memory.
		uint64_t guildMemberId{};///< GuildMemberId for the sending GuildMemberData.

		AudioFrameData() = default;

		AudioFrameData(AudioFrameType frameType);

		AudioFrameData& operator+=(std::basic_string_view<uint8_t>);

		inline bool operator==(const AudioFrameData& rhs) const {
			return currentSize == rhs.currentSize && data == rhs.data;
		}

		void clearData();
	};

	/// For connecting to a voice-channel. "streamInfo" is used when a socket is created to connect this bot to another bot, for transmitting audio back and forth.
	/// @brief For connecting to a voice-channel. "streamInfo" is used when a socket is created to connect this bot to another bot, for transmitting audio back and forth.
	struct VoiceConnectInitData {
		StreamInfo streamInfo{};///< The info for the stream-socket, if applicable.
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

	template<typename ReturnType> inline ReturnType fromString(const std::string& string, std::ios_base& (*type)( std::ios_base& )) {
		ReturnType value{};
		std::istringstream stream(string);
		stream >> type, stream >> value;
		return value;
	}

	template<typename ReturnType> inline std::string toHex(ReturnType inputValue) {
		std::stringstream stream{};
		stream << std::setfill('0') << std::setw(sizeof(ReturnType) * 2) << std::hex << inputValue;
		return stream.str();
	}

	class RGBColorValue {
	  public:
		uint8_t green{};
		uint8_t blue{};
		uint8_t red{};
	};

	using HexColorValue = std::string;

	class DiscordCoreAPI_Dll ColorValue {
	  public:
		template<typename ValueType> friend struct jsonifier::core;

		ColorValue(std::string hexColorValue);

		ColorValue(uint32_t colorValue);

		RGBColorValue getRgbColorValue();

		HexColorValue getHexColorValue();

		uint32_t getIntColorValue();

	  protected:
		uint32_t color{};
	};

	enum class HashType { User_Avatar = 0, Channel_Icon = 1, GuildMember_Avatar = 2, Guild_Icon = 3, Guild_Splash = 4, Guild_Banner = 5, Guild_Discovery = 6 };

	class DiscordCoreAPI_Dll IconHash {
	  public:
		IconHash() = default;

		IconHash& operator=(const std::string& string);

		IconHash(const std::string& string);

		bool operator==(const std::string& rhs) const;

		bool operator==(const IconHash& rhs) const;

		friend std::string operator+(const IconHash& lhs, std::string rhs);

		operator std::string() const;

	  protected:
		uint64_t highBits{};
		uint64_t lowBits{};
	};

	/**@}*/

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief Permission values, for a given Channel, by RoleData or GuildMemberData.
	enum class Permission : uint64_t {
		Create_Instant_Invite				= 0x0000000000000001,///< Allows creation of instant invites.
		Kick_Members						= 0x0000000000000002,///< Allows kicking members.
		Ban_Members							= 0x0000000000000004,///< Allows banning members.
		Administrator						= 0x0000000000000008,///< Allows all permissions and bypasses channel permission overwrites.
		Manage_Channels						= 0x0000000000000010,///< Allows management and editing of channels.
		Manage_Guild						= 0x0000000000000020,///< Allows management and editing of the guild.
		Add_Reactions						= 0x0000000000000040,///< Allows for the addition of reactions to messages.
		View_Audit_Log						= 0x0000000000000080,///< Allows for viewing of audit logs.
		Priority_Speaker					= 0x0000000000000100,///< Allows for using priority speaker in a voice channel.
		Stream								= 0x0000000000000200,///< Allows the user to go live.
		View_Channel						= 0x0000000000000400,///< Allows guild members to view a channel, which includes reading messages in text channels.
		Send_Messages						= 0x0000000000000800,///< Allows for sending messages in a channel and creating threads in a forum.
		Send_TTS_Messages					= 0x0000000000001000,///< Allows for sending of /tts messages.
		Manage_Messages						= 0x0000000000002000,///< Allows for deletion of other users messages.
		Embed_Links							= 0x0000000000004000,///< Links sent by users with this permission will be auto-embedded.
		Attach_Files						= 0x0000000000008000,///< Allows for uploading images and files.
		Read_Message_History				= 0x0000000000010000,///< Allows for reading of message history.
		Mention_Everyone					= 0x0000000000020000,///< Allows for using the AT-everyone tag to notify all users in a channel.
		Use_External_Emojis					= 0x0000000000040000,///< Allows the usage of custom emojis from other servers.
		View_Guild_Insights					= 0x0000000000080000,///< Allows for viewing guild insights.
		Connect								= 0x0000000000100000,///< Allows for joining of a voice channel.
		Speak								= 0x0000000000200000,///< Allows for speaking in a voice channel.
		Mute_Members						= 0x0000000000400000,///< Allows for muting members in a voice channel.
		Deafen_Members						= 0x0000000000800000,///< Allows for deafening of members in a voice channel.
		Move_Members						= 0x0000000001000000,///< Allows for moving of members between voice channels.
		Use_VAD								= 0x0000000002000000,///< Allows for using voice-activity-detection in a voice channel.
		Change_Nickname						= 0x0000000004000000,///< Allows for modification of own nickname.
		Manage_Nicknames					= 0x0000000008000000,///< Allows for modification of other users nicknames.
		Manage_Roles						= 0x0000000010000000,///< Allows management and editing of roles.
		Manage_Webhooks						= 0x0000000020000000,///< Allows management and editing of webhooks.
		Manage_Guild_Expressions			= 0x0000000040000000,///< Allows management and editing of emojis, stickers, and soundboard sounds.
		Use_Application_Commands			= 0x0000000080000000,///< Allows members to use application commands, including slash commands and context menu.
		Request_to_Speak					= 0x0000000100000000,///< Allows for requesting to speak in stage channels. (This permission is under active development).
		Manage_Events						= 0x0000000200000000,///< Allows for creating, editing, and deleting scheduled events.
		Manage_Threads						= 0x0000000400000000,///< Allows for deleting and archiving threads, and viewing all protected threads.
		Create_Public_Threads				= 0x0000000800000000,///< Allows for creating public and announcement threads.
		Create_Private_Threads				= 0x0000001000000000,///< Allows for creating protected threads.
		Use_External_Stickers				= 0x0000002000000000,///< Allows the usage of custom stickers from other servers.
		Send_Messages_in_Threads			= 0x0000004000000000,///< Allows for sending messages in threads.
		Use_Embedded_Activities				= 0x0000008000000000,///< Allows for using Activities (applications with the EMBEDDED flag) in a voice channel.
		Moderate_Members					= 0x0000010000000000,///< Allows for timing out users to prevent them from sending or reacting to messages in chat.
		View_Creator_Monetization_Analytics = 0x0000020000000000,///< Allows for viewing role subscription insights.
		Use_Soundboard						= 0x0000040000000000,///< Allows for using soundboard in a voice channel.
		Use_External_Sounds					= 0x0000200000000000,///< Allows the usage of custom soundboard sounds from other servers.
		Send_Voice_Messages					= 0x0000400000000000,///< Allows sending voice messages.
	};

	/// @brief PermissionsBase class, for representing and manipulating Permission values.
	template<typename ValueType> class PermissionsBase {
	  public:
		friend class jsonifier_internal::parser;

		inline bool operator==(const std::string& other) const {
			return *static_cast<const ValueType*>(this) == other;
		}

		/// @brief Returns a string containing all of a given User's PermissionsBase for a given Channel.
		/// @param guildMember The GuildMemberData who's PermissionsBase to analyze.
		/// @param channel The ChannelData withint which to check for PermissionsBase.
		/// @return std::string A string containing the final Permission's value for a given Channel.
		inline static std::string getCurrentChannelPermissions(const GuildMemberData& guildMember, const ChannelData& channel) {
			return computePermissions(guildMember, channel);
		}

		/// @brief Checks for a given Permission in a chosen ChannelData, for a specific UserData.
		/// @param guildMember The GuildMemberData who to check the PermissionsBase of.
		/// @param channel The ChannelData within which to check for the Permission's presence.
		/// @param permission A Permission to check the current ChannelData for.
		/// @return bool A bool suggesting the presence of the chosen Permission.
		inline bool checkForPermission(const GuildMemberData& guildMember, const ChannelData& channel, Permission permission) {
			if ((stoull(computePermissions(guildMember, channel)) & static_cast<uint64_t>(permission)) == static_cast<uint64_t>(permission)) {
				return true;
			} else {
				return false;
			}
		}

		/// @brief Returns a string containing the currently held PermissionsBase in a given Guild.
		/// @param guildMember The GuildMemberData who's PermissionsBase are to be evaluated.
		/// @return std::string A string containing the current PermissionsBase.
		inline static std::string getCurrentGuildPermissions(const GuildMemberData& guildMember) {
			PermissionsBase setValue(computeBasePermissions(guildMember));
			return static_cast<ValueType*>(setValue);
		}

		/// @brief Removes one or more PermissionsBase from the current PermissionsBase value.
		/// @param permissionsToRemove A vector containing the PermissionsBase you wish to remove.
		inline void removePermissions(const jsonifier::vector<Permission>& permissionsToRemove) {
			uint64_t permissionsInteger = *static_cast<ValueType*>(this);
			for (auto valueNew: permissionsToRemove) {
				permissionsInteger &= ~static_cast<uint64_t>(valueNew);
			}
			std::stringstream sstream{};
			sstream << permissionsInteger;
			*static_cast<ValueType*>(this) = sstream.str();
		}

		/// @brief Adds one or more PermissionsBase to the current PermissionsBase value.
		/// @param permissionsToAdd A vector containing the PermissionsBase you wish to add.
		inline void addPermissions(const jsonifier::vector<Permission>& permissionsToAdd) {
			uint64_t permissionsInteger = *static_cast<ValueType*>(this);
			for (auto valueNew: permissionsToAdd) {
				permissionsInteger |= static_cast<uint64_t>(valueNew);
			}
			std::stringstream sstream{};
			sstream << permissionsInteger;
			*static_cast<ValueType*>(this) = sstream.str();
		}

		/// @brief Displays the currently present PermissionsBase in a string, and returns a vector with each of them stored in string format.
		/// @return jsonifier::vector A vector full of strings of the PermissionsBase that are in the input std::string's value.
		inline jsonifier::vector<std::string> displayPermissions() {
			jsonifier::vector<std::string> returnVector{};
			uint64_t permissionsInteger = *static_cast<ValueType*>(this);
			if (permissionsInteger & (1ll << 3)) {
				for (int64_t x = 0; x < 46; ++x) {
					permissionsInteger |= 1ll << x;
				}
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Create_Instant_Invite)) {
				returnVector.emplace_back("Create Instant Invite");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Kick_Members)) {
				returnVector.emplace_back("Kick Members");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Ban_Members)) {
				returnVector.emplace_back("Ban Members");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Administrator)) {
				returnVector.emplace_back("Administrator");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Channels)) {
				returnVector.emplace_back("Manage Channels");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Guild)) {
				returnVector.emplace_back("Manage Guild");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Add_Reactions)) {
				returnVector.emplace_back("Add Reactions");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::View_Audit_Log)) {
				returnVector.emplace_back("View Audit Log");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Priority_Speaker)) {
				returnVector.emplace_back("Priority Speaker");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Stream)) {
				returnVector.emplace_back("Stream");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::View_Channel)) {
				returnVector.emplace_back("View Channel");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Send_Messages)) {
				returnVector.emplace_back("Send Messages");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Send_TTS_Messages)) {
				returnVector.emplace_back("Send TTS Messages");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Messages)) {
				returnVector.emplace_back("Manage Messages");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Embed_Links)) {
				returnVector.emplace_back("Embed Links");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Attach_Files)) {
				returnVector.emplace_back("Attach Files");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Read_Message_History)) {
				returnVector.emplace_back("Read Message History");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Mention_Everyone)) {
				returnVector.emplace_back("Mention Everyone");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Use_External_Emojis)) {
				returnVector.emplace_back("Use External Emoji");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::View_Guild_Insights)) {
				returnVector.emplace_back("View Guild Insights");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Connect)) {
				returnVector.emplace_back("Connect");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Speak)) {
				returnVector.emplace_back("Speak");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Mute_Members)) {
				returnVector.emplace_back("Mute Members");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Deafen_Members)) {
				returnVector.emplace_back("Deafen Members");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Move_Members)) {
				returnVector.emplace_back("Move Members");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Use_VAD)) {
				returnVector.emplace_back("Use VAD");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Change_Nickname)) {
				returnVector.emplace_back("Change Nickname");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Nicknames)) {
				returnVector.emplace_back("Manage Nicknames");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Roles)) {
				returnVector.emplace_back("Manage Roles");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Webhooks)) {
				returnVector.emplace_back("Manage Webhooks");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Guild_Expressions)) {
				returnVector.emplace_back("Manage Emojis And Stickers");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Use_Application_Commands)) {
				returnVector.emplace_back("Use Application Commands");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Request_to_Speak)) {
				returnVector.emplace_back("Request To Speak");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Events)) {
				returnVector.emplace_back("Manage Events");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Manage_Threads)) {
				returnVector.emplace_back("Manage Threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Create_Public_Threads)) {
				returnVector.emplace_back("Create Public Threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Create_Private_Threads)) {
				returnVector.emplace_back("Create Private Threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Use_External_Stickers)) {
				returnVector.emplace_back("Use External Stickers");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Send_Messages_in_Threads)) {
				returnVector.emplace_back("Send Messages In Threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Use_Embedded_Activities)) {
				returnVector.emplace_back("Start Embedded Activities");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Moderate_Members)) {
				returnVector.emplace_back("Moderate Members");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::View_Creator_Monetization_Analytics)) {
				returnVector.emplace_back("View Creator Monetization Analytics");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Use_Soundboard)) {
				returnVector.emplace_back("Use Soundboard");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Use_External_Sounds)) {
				returnVector.emplace_back("Use External Sounds");
			}
			if (permissionsInteger & static_cast<uint64_t>(Permission::Send_Voice_Messages)) {
				returnVector.emplace_back("Send Voice Messages");
			}
			return returnVector;
		}

		/// @brief Returns a string containing the currently held PermissionsBase.
		/// @return std::string A string containing the current PermissionsBase.
		inline std::string getCurrentPermissionString() {
			std::string returnString = *static_cast<ValueType*>(this);
			return returnString;
		}

		/// @brief Returns a string containing ALL of the possible PermissionsBase.
		/// @return std::string A string containing all of the possible PermissionsBase.
		inline static std::string getAllPermissions() {
			uint64_t allPerms{};
			for (int64_t x = 0; x < 46; ++x) {
				allPerms |= 1ll << x;
			}
			std::stringstream stream{};
			stream << allPerms;
			return stream.str();
		}

	  protected:
		inline PermissionsBase() = default;

		DiscordCoreAPI_Dll static std::string computeOverwrites(const std::string& basePermissions, const GuildMemberData& guildMember, const ChannelData& channel);

		inline static std::string computePermissions(const GuildMemberData& guildMember, const ChannelData& channel) {
			std::string permissions = computeBasePermissions(guildMember);
			permissions				= computeOverwrites(permissions, guildMember, channel);
			return permissions;
		}

		DiscordCoreAPI_Dll static std::string computeBasePermissions(const GuildMemberData& guildMember);
	};

	class PermissionsParse : public PermissionsBase<PermissionsParse>, public std::string {
	  public:
		template<typename ValueType> friend class PermissionsBase;

		inline PermissionsParse() = default;

		inline PermissionsParse& operator=(const std::string& valueNew) {
			resize(valueNew.size());
			std::memcpy(data(), valueNew.data(), size());
			return *this;
		}

		inline PermissionsParse(const std::string& valueNew) {
			*this = valueNew;
		}

		inline PermissionsParse& operator=(std::string&& valueNew) {
			resize(valueNew.size());
			std::memcpy(data(), valueNew.data(), size());
			return *this;
		}

		inline PermissionsParse(std::string&& valueNew) {
			*this = std::move(valueNew);
		}

		inline PermissionsParse& operator=(uint64_t valueNew) {
			*this = std::to_string(valueNew);
			return *this;
		}

		inline PermissionsParse(uint64_t valueNew) {
			*this = valueNew;
		}

		inline PermissionsParse substr(uint64_t offset, uint64_t count) const {
			return substr(offset, count);
		}

		inline uint64_t size() const {
			return std::string::size();
		}

		inline char* data() {
			return std::string::data();
		}

		inline operator uint64_t() const {
			return std::stoull(*this);
		}
	};

	class Permissions : public PermissionsBase<Permissions> {
	  public:
		template<typename ValueType> friend class PermissionsBase;

		inline Permissions() = default;

		inline Permissions& operator=(const PermissionsParse& other) {
			value = other.operator uint64_t();
			return *this;
		}

		inline Permissions(const PermissionsParse& other) {
			*this = other;
		}

		inline Permissions& operator=(const std::string& valueNew) {
			value = stoull(valueNew);
			return *this;
		}

		inline Permissions(const std::string& valueNew) {
			*this = valueNew;
		}

		inline Permissions& operator=(std::string&& valueNew) {
			value = stoull(valueNew);
			return *this;
		}

		inline Permissions(std::string&& valueNew) {
			*this = std::move(valueNew);
		}

		inline Permissions& operator=(uint64_t valueNew) {
			value = valueNew;
			return *this;
		}

		inline Permissions(uint64_t valueNew) {
			*this = valueNew;
		}

		inline operator uint64_t() const {
			return value;
		}

		inline operator std::string() const {
			return std::to_string(value);
		}

	  protected:
		uint64_t value{};
	};

	DiscordCoreAPI_Dll std::string constructMultiPartData(const std::string& data, const jsonifier::vector<File>& files);

	DiscordCoreAPI_Dll std::string convertToLowerCase(const std::string& stringToConvert);

	DiscordCoreAPI_Dll std::string base64Encode(const std::string&, bool = false);

	DiscordCoreAPI_Dll std::string loadFileContents(const std::string& filePath);

	DiscordCoreAPI_Dll std::string utf8MakeValid(const std::string& inputString);

	DiscordCoreAPI_Dll std::string urlEncode(const std::string& inputString);

	DiscordCoreAPI_Dll void spinLock(uint64_t timeInNsToSpinLockFor);

	DiscordCoreAPI_Dll std::string generateBase64EncodedKey();

	DiscordCoreAPI_Dll bool nanoSleep(int64_t ns);

	/// @brief Acquires a timeStamp with the current time and date - suitable for use in message-embeds.
	/// @return std::string A string containing the current date-time stamp.
	DiscordCoreAPI_Dll std::string getTimeAndDate();

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */
	template<typename ReturnType, bool timeOut = true> class NewThreadAwaiter;

	/// @brief An awaitable that can be used to launch the CoRoutine onto a new thread - as well as return the handle for stoppping its execution.
	/// @tparam ReturnType The type of value returned by the containing CoRoutine.
	/// @tparam timeOut Whether or not to time out the CoRoutine's execution after a period of time.
	/// @return NewThreadAwaiter<ReturnType, timeOut> A NewThreadAwaiter for suspendint the current CoRoutine's execution.
	template<typename ReturnType, bool timeOut = true> inline auto NewThreadAwaitable() {
		return NewThreadAwaiter<ReturnType, timeOut>{};
	}

	/**@}*/
};
