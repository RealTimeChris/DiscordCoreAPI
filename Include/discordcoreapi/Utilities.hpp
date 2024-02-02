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
/// Utilities.hpp - Header file for the utilities.
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

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief Activity types.
	enum class activity_type : uint8_t {
		game	  = 0,///< Game.
		streaming = 1,///< Streaming.
		listening = 2,///< Listening.
		watching  = 3,///< Watching.
		custom	  = 4,///< custom.
		competing = 5///< competing.
	};

	struct time_stamps {
		uint64_t start{};
		uint64_t end{};
	};

	/// @brief Activity data.
	struct activity_data {
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};
		jsonifier::string details{};///< What the player is currently doing.
		snowflake applicationId{};///< Application id for the game.
		jsonifier::string state{};///< User's current party status, or text used for a custom status.
		jsonifier::string name{};///< Name of the activity.
		jsonifier::string url{};///< Stream url, is validated when type is 1.
		time_stamps timestamps{};///< Timestamps object unix timestamps for start and / or end of the game.
		uint64_t createdAt{};///< Unix timestamp(in milliseconds) of when the activity was added to the user's session.
		activity_type type{};///< Activity's type.

		activity_data() = default;

		~activity_data() = default;
	};

	/// @brief For connecting two bots to stream the vc contents between the two.
	struct stream_info {
		bool streamBotAudio{};///< Do we stream the audio coming from other bots?
		jsonifier::string address{};///< The address to connect to.
		stream_type type{};///< The type of streamer that this is. set one to client and one to server.
		uint16_t port{};///< The port to connect to.
	};

	namespace discord_core_internal {

		class sound_cloud_request_builder;
		class you_tube_request_builder;
		class websocket_client;
		class base_socket_agent;
		class sound_cloud_api;
		class you_tube_api;

	}// namespace discord_core_internal

	struct on_voice_server_update_data;
	struct on_voice_state_update_data;
	struct file;

	class discord_core_client;
	class command_controller;
	class voice_connection;
	class guild_member_data;
	class guild_members;
	class channel_data;
	class reactions;
	class role_data;
	class bot_user;

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename return_type, bool timeOut = true> class co_routine;

	/**@}*/

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	enum class presence_update_state { online = 0, Do_Not_Disturb = 1, idle = 2, invisible = 3, offline = 4 };

	/// @brief For updating a user's presence.
	struct DiscordCoreAPI_Dll update_presence_data {
		template<typename value_type> friend struct jsonifier::core;
		jsonifier::vector<activity_data> activities{};///< A vector of activities.
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};
		presence_update_state status{};///< current status.
		int64_t since{};///< When was the activity started?
		bool afk{};///< Are we afk.

		DCA_INLINE update_presence_data() = default;
		update_presence_data(presence_update_state updateState);

		operator discord_core_internal::etf_serializer();

	  protected:
		jsonifier::string statusReal{};
	};

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& outputSttream, jsonifier::string_view (*function)(void));

	/// @brief Input event response types.
	enum class input_event_response_type : uint8_t {
		Unset									= 0,///< Unset.
		Deferred_Response						= 1,
		Ephemeral_Deferred_Response				= 2,///< Deferred ephemeral response.
		Interaction_Response					= 3,///< Interaction response.
		Ephemeral_Interaction_Response			= 4,///< Ephemeral interaction response.
		Edit_Interaction_Response				= 5,///< Interaction response edit.
		Follow_Up_Message						= 6,///< Follow-up message_data.
		Ephemeral_Follow_Up_Message				= 7,///< Ephemeral follow-up message_data.
		Edit_Follow_Up_Message					= 8,///< Follow-up message edit.
		Application_Command_AutoComplete_Result = 9,///< Respond to an autocomplete interaction with suggested choices.
		Modal_Interaction_Response				= 10,///< Respond to an interaction with a popup modal.
	};

	/// @brief Gateway intents.
	enum class gateway_intents : uint32_t {
		guilds						  = 1 << 0,///< Intent for receipt of guild information.
		Guild_Members				  = 1 << 1,///< Intent for receipt of guild members.
		Guild_Bans					  = 1 << 2,///< Intent for receipt of guild bans.
		Guild_Emojis				  = 1 << 3,///< Intent for receipt of guild emojis.
		Guild_Integrations			  = 1 << 4,///< Intent for receipt of guild integrations.
		Guild_Webhooks				  = 1 << 5,///< Intent for receipt of guild webhooks.
		Guild_Invites				  = 1 << 6,///< Intent for receipt of guild invites.
		Guild_VoiceStates			  = 1 << 7,///< Intent for receipt of guild voice states.
		Guild_Presences				  = 1 << 8,///< Intent for receipt of guild presences.
		Guild_Messages				  = 1 << 9,///< Intent for receipt of guild messages.
		Guild_Message_Reactions		  = 1 << 10,///< Intent for receipt of guild message reactions.
		Guild_Message_Typing		  = 1 << 11,///< Intent for receipt of guild message typing notifications.
		Direct_Messages				  = 1 << 12,///< Intent for receipt of direct messages (dms).
		Direct_Message_Reactions	  = 1 << 13,///< Intent for receipt of direct message reactions.
		Direct_Message_Typing		  = 1 << 14,///< Intent for receipt of direct message typing notifications.
		Message_Content				  = 1 << 15,///< Intent for receipt of message content.
		Guild_Scheduled_Events		  = 1 << 16,///< Scheduled events.
		Auto_Moderation_Configuration = 1 << 20,/// auto moderation configuration.
		Auto_Moderation_Execution	  = 1 << 21,///< Auto moderation execution.
		Default_Intents = guilds | Guild_Bans | Guild_Emojis | Guild_Integrations | Guild_Webhooks | Guild_Invites | Guild_VoiceStates | Guild_Messages | Guild_Message_Reactions |
			Guild_Message_Typing | Direct_Messages | Direct_Message_Reactions | Direct_Message_Typing | Guild_Scheduled_Events | Auto_Moderation_Configuration |
			Auto_Moderation_Execution,///< Default intents (all non-privileged intents).
		Privileged_Intents = Guild_Members | Guild_Presences | Message_Content,///< Privileged intents requiring id.
		All_Intents		   = Default_Intents | Privileged_Intents///< Every single intent.
	};

	/// @brief Function data for repeated functions to be loaded.
	struct repeated_function_data {
		std::function<void(discord_core_client*)> function{};///< The std::function char* to be loaded.
		uint32_t intervalInMs{};///< The time interval at which to call the std::function.
		bool repeated{};///< Whether or not the std::function is repeating.
		int64_t dummyArg{};
	};

	/// @brief Represents which text format to use for websocket transfer.
	enum class text_format : uint8_t {
		etf	 = 0x00,///< Etf format.
		json = 0x01///< Json format.
	};

	/// @brief Sharding options for the library.
	struct sharding_options {
		uint32_t numberOfShardsForThisProcess{ 1 };///< The number of shards to launch on the current process.
		uint32_t totalNumberOfShards{ 1 };///< The total number of shards that will be launched across all processes.
		uint32_t startingShard{};///< The first shard to start on this process.
	};

	/// @brief Loggin options for the library.
	struct logging_options {
		std::ostream* outputStream{ &std::cout };
		std::ostream* errorStream{ &std::cerr };
		bool logWebSocketSuccessMessages{};///< Do we log the websocket success messages to std::cout?
		bool logWebSocketErrorMessages{};///< Do we log the websocket error messages to std::cout?
		bool logGeneralSuccessMessages{};///< Do we log general success messages to std::cout?
		bool logGeneralErrorMessages{};///< Do we log general error messages to std::cout?
		bool logHttpsSuccessMessages{};///< Do we log https response success messages to std::cout?
		bool logHttpsErrorMessages{};///< Do we log https response error messages to std::cout?
	};

	/// @brief For selecting the caching style of the library.
	struct cache_options {
		bool cacheGuildMembers{ true };///< Do we cache guild_members?
		bool cacheVoiceStates{ true };///< Do we cache voices states?
		bool cacheChannels{ true };///< Do we cache channels?
		bool cacheGuilds{ true };///< Do we cache guilds?
		bool cacheRoles{ true };///< Do we cache roles?
		bool cacheUsers{ true };///< Do we cache users?
	};

	/// @brief Configuration data for the library's main class, discord_core_client.
	struct discord_core_client_config {
		update_presence_data presenceData{ presence_update_state::online };///< Presence data to initialize your bot with.
		jsonifier::vector<repeated_function_data> functionsToExecute{};///< Functions to execute after a timer, or on a repetition.
		gateway_intents intents{ gateway_intents::All_Intents };///< The gateway intents to be used for this instance.
		text_format textFormat{ text_format::etf };///< Use etf or json format for websocket transfer?
		jsonifier::string connectionAddress{};///< A potentially alternative connection address for the websocket.
		sharding_options shardOptions{};///< Options for the sharding of your bot.
		jsonifier::string botToken{};///< Your bot's token.
		logging_options logOptions{};///< Options for the output/logging of the library.
		cache_options cacheOptions{};///< Options for the cache of the library.
		uint16_t connectionPort{};///< A potentially alternative connection port for the websocket.
	};

	class DiscordCoreAPI_Dll config_manager {
	  public:
		config_manager() = default;

		explicit config_manager(const discord_core_client_config&);

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

		update_presence_data getPresenceData() const;

		jsonifier::string getBotToken() const;

		uint64_t getTotalShardCount() const;

		std::ostream* getOutputStream() const;

		std::ostream* getErrorStream() const;

		uint64_t getStartingShard() const;

		uint64_t getShardCountForThisProcess() const;

		jsonifier::string getConnectionAddress() const;

		void setConnectionAddress(jsonifier::string_view connectionAddressNew);

		uint16_t getConnectionPort() const;

		void setConnectionPort(const uint16_t connectionPortNew);

		jsonifier::vector<repeated_function_data> getFunctionsToExecute() const;

		text_format getTextFormat() const;

		gateway_intents getGatewayIntents();

	  protected:
		discord_core_client_config config{};
	};

	/// @brief Color constants for use in the embed_data color values.
	namespace colors {
		static constexpr jsonifier::string_view white{ "ffffff" },///< White.
			discord_white{ "fffffe" },///< Discord white.
			light_gray{ "c0_c0_c0" },///< Light gray.
			gray{ "808080" },///< Gray.
			dark_gray{ "404040" },///< Dark gray.
			black{ "000000" },///< Black.
			discord_black{ "000001" },///< Discord black.
			red{ "ff0000" },///< Red.
			pink{ "ffafaf" },///< Pink.
			orange{ "ffc800" },///< Orange.
			yellow{ "ffff00" },///< Yellow.
			green{ "00FF00" },///< Green.
			magenta{ "ff00_ff" },///< Magenta.
			cyan{ "00FFFF" },///< cyan.
			blue{ "0000FF" },///< Blue.
			light_sea_green{ "1ABC9C" },///< Light sea green.
			medium_sea_green{ "2ECC71" },///< Medium sea green.
			summer_sky{ "3498DB" },///< Summer skye.
			deep_lilac{ "9B59B6" },///< Deep lilac.
			ruby{ "e91_e63" },///< Ruby.
			moon_yellow{ "f1_c40_f" },///< Moon yellow.
			tahiti_gold{ "e67_e22" },///< Tahiti gold.
			cinnabar{ "e74_c3_c" },///< cinnabar.
			submarine{ "95A5A6" },///< Submarine.
			blue_aquamarine{ "607D8B" },///< Blue aquamarine.
			deep_sea{ "11806A" },///< Deep sea.
			sea_green{ "1F8B4C" },///< Sea green.
			endeavour{ "206694" },///< Endeavor.
			vivid_violet{ "71368A" },///< Vivid violet.
			jazzberry_jam{ "ad1457" },///< Jazzberry jam.
			dark_goldenrod{ "c27_c0_e" },///< Dark goldenrod.
			rust{ "a84300" },///< Rust.
			brown{ "992D22" },///< Brown.
			gray_chateau{ "979C9F" },///< Gray chateau.
			bismark{ "546E7A" },///< Bismark.
			sti_blue{ "0E4BEF" },///< Sti blue.
			wrx_blue{ "00247D" },///< Wrx blue.
			ralli_art_crimson{ "e60012" },///< Ralliart crimson.
			lime{ "00FF00" },///< Lime.
			forest_green{ "228B22" },///< Forest green.
			cadmium_green{ "097969" },///< cadmium green.
			aquamarine{ "7FFFD4" },///< Aquamarine.
			blue_green{ "088F8F" },///< Blue green.
			raspberry{ "e30_b5_c" },///< Raspberry.
			scarlet_red{ "ff2400" };///< Scarlet red.
	};

	/**@}*/

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// @brief Audio frame types.
	enum class audio_frame_type : uint8_t {
		Unset	= 0,///< Unset.
		raw_pcm = 1,///< Raw pcm.
		encoded = 2,///< Encoded audio data.
	};

	/// @brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll audio_frame_data {
		audio_frame_type type{ audio_frame_type::Unset };///< The type of audio frame.
		jsonifier::vector<uint8_t> data{};///< The audio data.
		int64_t currentSize{ -5 };///< The current size of the allocated memory.

		audio_frame_data() = default;

		audio_frame_data(audio_frame_type frameType);

		audio_frame_data& operator+=(jsonifier::string_view_base<uint8_t>);

		audio_frame_data& operator+=(jsonifier::vector<uint8_t>);

		DCA_INLINE bool operator==(const audio_frame_data& rhs) const {
			return currentSize == rhs.currentSize && data == rhs.data;
		}

		void clearData();
	};

	/// for connecting to a voice-channel. "streamInfo" is used when a SOCKET is created to connect this bot to another bot, for transmitting audio back and forth.
	/// @brief For connecting to a voice-channel. "streamInfo" is used when a SOCKET is created to connect this bot to another bot, for transmitting audio back and forth.
	struct voice_connect_init_data {
		stream_info streamInfo{};///< The info for the stream-SOCKET, if applicable.
		int32_t currentShard{};///< The current websocket shard, if applicable.
		snowflake channelId{};///< The channel id to connect to.
		snowflake guildId{};///< The guild id to connect to.
		snowflake userId{};///< This bot's user id.
		bool selfDeaf{};///< Self-deafen the bot?
		bool selfMute{};///< Self-mute the bot?
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename return_type> DCA_INLINE return_type fromString(jsonifier::string_view string, std::ios_base& (*type)( std::ios_base& )) {
		return_type value{};
		std::istringstream stream(std::string{ string });
		stream >> type, stream >> value;
		return value;
	}

	template<typename return_type> DCA_INLINE jsonifier::string toHex(return_type inputValue) {
		std::stringstream stream{};
		stream << std::setfill('0') << std::setw(sizeof(return_type) * 2) << std::hex << inputValue;
		return jsonifier::string{ stream.str() };
	}

	class rgbcolor_value {
	  public:
		uint8_t green{};
		uint8_t blue{};
		uint8_t red{};
	};

	using hex_color_value = jsonifier::string;

	class DiscordCoreAPI_Dll color_value {
	  public:
		template<typename value_type> friend struct jsonifier::core;

		color_value(jsonifier::string_view hexColorValue);

		rgbcolor_value getRgbColorValue();

		hex_color_value getHexColorValue();

		color_value(uint32_t colorValue);

		uint32_t getIntColorValue();

	  protected:
		uint32_t color{};
	};

	enum class hash_type { User_Avatar = 0, Channel_Icon = 1, GuildMember_Avatar = 2, Guild_Icon = 3, Guild_Splash = 4, Guild_Banner = 5, Guild_Discovery = 6 };

	class DiscordCoreAPI_Dll icon_hash {
	  public:
		icon_hash() = default;

		icon_hash& operator=(jsonifier::string_view string);

		icon_hash(jsonifier::string_view string);

		bool operator==(jsonifier::string_view rhs) const;

		bool operator==(const icon_hash& rhs) const;

		friend jsonifier::string operator+(const icon_hash& lhs, jsonifier::string rhs);

		operator jsonifier::string() const;

	  protected:
		uint64_t highBits{};
		uint64_t lowBits{};
	};

	/**@}*/

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief Permission values, for a given channel, by role_data or guild_member_data.
	enum class permission : uint64_t {
		Create_Instant_Invite				= 0x0000000000000001,///< Allows creation of instant invites.
		Kick_Members						= 0x0000000000000002,///< Allows kicking members.
		Ban_Members							= 0x0000000000000004,///< Allows banning members.
		administrator						= 0x0000000000000008,///< Allows all permissions and bypasses channel permission overwrites.
		Manage_Channels						= 0x0000000000000010,///< Allows management and editing of channels.
		Manage_Guild						= 0x0000000000000020,///< Allows management and editing of the guild.
		Add_Reactions						= 0x0000000000000040,///< Allows for the addition of reactions to messages.
		View_Audit_Log						= 0x0000000000000080,///< Allows for viewing of audit logs.
		Priority_Speaker					= 0x0000000000000100,///< Allows for using priority speaker in a voice channel.
		stream								= 0x0000000000000200,///< Allows the user to go live.
		View_Channel						= 0x0000000000000400,///< Allows guild members to view a channel, which includes reading messages in text channels.
		Send_Messages						= 0x0000000000000800,///< Allows for sending messages in a channel and creating threads in a forum.
		Send_TTS_Messages					= 0x0000000000001000,///< Allows for sending of /tts messages.
		Manage_Messages						= 0x0000000000002000,///< Allows for deletion of other users messages.
		Embed_Links							= 0x0000000000004000,///< Links sent by users with this permission will be auto-embedded.
		Attach_Files						= 0x0000000000008000,///< Allows for uploading images and files.
		Read_Message_History				= 0x0000000000010000,///< Allows for reading of message history.
		Mention_Everyone					= 0x0000000000020000,///< Allows for using the at-everyone tag to notify all users in a channel.
		Use_External_Emojis					= 0x0000000000040000,///< Allows the usage of custom emojis from other servers.
		View_Guild_Insights					= 0x0000000000080000,///< Allows for viewing guild insights.
		connect								= 0x0000000000100000,///< Allows for joining of a voice channel.
		speak								= 0x0000000000200000,///< Allows for speaking in a voice channel.
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
		Request_to_Speak					= 0x0000000100000000,///< Allows for requesting to speak in stage channels. (this permission is under active development).
		Manage_Events						= 0x0000000200000000,///< Allows for creating, editing, and deleting scheduled events.
		Manage_Threads						= 0x0000000400000000,///< Allows for deleting and archiving threads, and viewing all protected threads.
		Create_Public_Threads				= 0x0000000800000000,///< Allows for creating public and announcement threads.
		Create_Private_Threads				= 0x0000001000000000,///< Allows for creating protected threads.
		Use_External_Stickers				= 0x0000002000000000,///< Allows the usage of custom stickers from other servers.
		Send_Messages_in_Threads			= 0x0000004000000000,///< Allows for sending messages in threads.
		Use_Embedded_Activities				= 0x0000008000000000,///< Allows for using activities (applications with the embedded flag) in a voice channel.
		Moderate_Members					= 0x0000010000000000,///< Allows for timing out users to prevent them from sending or reacting to messages in chat.
		View_Creator_Monetization_Analytics = 0x0000020000000000,///< Allows for viewing role subscription insights.
		Use_Soundboard						= 0x0000040000000000,///< Allows for using soundboard in a voice channel.
		Use_External_Sounds					= 0x0000200000000000,///< Allows the usage of custom soundboard sounds from other servers.
		Send_Voice_Messages					= 0x0000400000000000,///< Allows sending voice messages.
	};

	/// @brief Permissions_base class, for representing and manipulating permission values.
	template<typename value_type> class permissions_base {
	  public:

		/// @brief Returns a string containing all of a given user's permissions_base for a given channel.
		/// @param guildMember the guild_member_data who's permissions_base to analyze.
		/// @param channel the channel_data withint which to check for permissions_base.
		/// @return jsonifier::string a string containing the final permission's value for a given channel.
		DCA_INLINE static jsonifier::string getCurrentChannelPermissions(const guild_member_data& guildMember, const channel_data& channel) {
			return computePermissions(guildMember, channel);
		}

		/// @brief Checks for a given permission in a chosen channel_data, for a specific user_data.
		/// @param guildMember the guild_member_data who to check the permissions_base of.
		/// @param channel the channel_data within which to check for the permission's presence.
		/// @param permission a permission to check the current channel_data for.
		/// @return bool a bool suggesting the presence of the chosen permission.
		DCA_INLINE bool checkForPermission(const guild_member_data& guildMember, const channel_data& channel, permission permission) {
			if ((jsonifier::strToUint64(computePermissions(guildMember, channel).data()) & static_cast<uint64_t>(permission)) == static_cast<uint64_t>(permission)) {
				return true;
			} else {
				return false;
			}
		}

		/// @brief Returns a string containing the currently held permissions_base in a given guild.
		/// @param guildMember the guild_member_data who's permissions_base are to be evaluated.
		/// @return jsonifier::string a string containing the current permissions_base.
		DCA_INLINE static jsonifier::string getCurrentGuildPermissions(const guild_member_data& guildMember) {
			permissions_base setValue(computeBasePermissions(guildMember));
			return static_cast<value_type*>(setValue);
		}

		/// @brief Removes one or more permissions_base from the current permissions_base value.
		/// @param permissionsToRemove a vector containing the permissions_base you wish to remove.
		DCA_INLINE void removePermissions(const jsonifier::vector<permission>& permissionsToRemove) {
			uint64_t permissionsInteger = *static_cast<value_type*>(this);
			for (auto valueNew: permissionsToRemove) {
				permissionsInteger &= ~static_cast<uint64_t>(valueNew);
			}
			std::stringstream sstream{};
			sstream << permissionsInteger;
			*static_cast<value_type*>(this) = jsonifier::string{ sstream.str() };
		}

		/// @brief Adds one or more permissions_base to the current permissions_base value.
		/// @param permissionsToAdd a vector containing the permissions_base you wish to add.
		DCA_INLINE void addPermissions(const jsonifier::vector<permission>& permissionsToAdd) {
			uint64_t permissionsInteger = *static_cast<value_type*>(this);
			for (auto valueNew: permissionsToAdd) {
				permissionsInteger |= static_cast<uint64_t>(valueNew);
			}
			std::stringstream sstream{};
			sstream << permissionsInteger;
			*static_cast<value_type*>(this) = jsonifier::string{ sstream.str() };
		}

		/// @brief Displays the currently present permissions_base in a string, and returns A vector with each of them stored in string format.
		/// @return jsonifier::vector a vector full of strings of the permissions_base that are in the input jsonifier::string's value.
		DCA_INLINE jsonifier::vector<jsonifier::string> displayPermissions() {
			jsonifier::vector<jsonifier::string> returnVector{};
			uint64_t permissionsInteger = *static_cast<value_type*>(this);
			if (permissionsInteger & (1ll << 3)) {
				for (int64_t x = 0; x < 46; ++x) {
					permissionsInteger |= 1ll << x;
				}
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Create_Instant_Invite)) {
				returnVector.emplace_back("create instant invite");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Kick_Members)) {
				returnVector.emplace_back("kick members");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Ban_Members)) {
				returnVector.emplace_back("ban members");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::administrator)) {
				returnVector.emplace_back("administrator");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Channels)) {
				returnVector.emplace_back("manage channels");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Guild)) {
				returnVector.emplace_back("manage guild");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Add_Reactions)) {
				returnVector.emplace_back("add reactions");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::View_Audit_Log)) {
				returnVector.emplace_back("view audit log");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Priority_Speaker)) {
				returnVector.emplace_back("priority speaker");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::stream)) {
				returnVector.emplace_back("stream");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::View_Channel)) {
				returnVector.emplace_back("view channel");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Send_Messages)) {
				returnVector.emplace_back("send messages");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Send_TTS_Messages)) {
				returnVector.emplace_back("send tts messages");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Messages)) {
				returnVector.emplace_back("manage messages");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Embed_Links)) {
				returnVector.emplace_back("embed links");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Attach_Files)) {
				returnVector.emplace_back("attach files");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Read_Message_History)) {
				returnVector.emplace_back("read message history");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Mention_Everyone)) {
				returnVector.emplace_back("mention everyone");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Use_External_Emojis)) {
				returnVector.emplace_back("use external emoji");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::View_Guild_Insights)) {
				returnVector.emplace_back("view guild insights");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::connect)) {
				returnVector.emplace_back("connect");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::speak)) {
				returnVector.emplace_back("speak");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Mute_Members)) {
				returnVector.emplace_back("mute members");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Deafen_Members)) {
				returnVector.emplace_back("deafen members");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Move_Members)) {
				returnVector.emplace_back("move members");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Use_VAD)) {
				returnVector.emplace_back("use vad");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Change_Nickname)) {
				returnVector.emplace_back("change nickname");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Nicknames)) {
				returnVector.emplace_back("manage nicknames");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Roles)) {
				returnVector.emplace_back("manage roles");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Webhooks)) {
				returnVector.emplace_back("manage webhooks");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Guild_Expressions)) {
				returnVector.emplace_back("manage emojis and stickers");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Use_Application_Commands)) {
				returnVector.emplace_back("use application commands");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Request_to_Speak)) {
				returnVector.emplace_back("request to speak");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Events)) {
				returnVector.emplace_back("manage events");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Manage_Threads)) {
				returnVector.emplace_back("manage threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Create_Public_Threads)) {
				returnVector.emplace_back("create public threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Create_Private_Threads)) {
				returnVector.emplace_back("create private threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Use_External_Stickers)) {
				returnVector.emplace_back("use external stickers");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Send_Messages_in_Threads)) {
				returnVector.emplace_back("send messages in threads");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Use_Embedded_Activities)) {
				returnVector.emplace_back("start embedded activities");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Moderate_Members)) {
				returnVector.emplace_back("moderate members");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::View_Creator_Monetization_Analytics)) {
				returnVector.emplace_back("view creator monetization analytics");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Use_Soundboard)) {
				returnVector.emplace_back("use soundboard");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Use_External_Sounds)) {
				returnVector.emplace_back("use external sounds");
			}
			if (permissionsInteger & static_cast<uint64_t>(permission::Send_Voice_Messages)) {
				returnVector.emplace_back("send voice messages");
			}
			return returnVector;
		}

		/// @brief Returns a string containing the currently held permissions_base.
		/// @return jsonifier::string a string containing the current permissions_base.
		DCA_INLINE jsonifier::string getCurrentPermissionString() {
			jsonifier::string returnString = *static_cast<value_type*>(this);
			return returnString;
		}

		/// @brief Returns a string containing all of the possible permissions_base.
		/// @return jsonifier::string a string containing all of the possible permissions_base.
		DCA_INLINE static jsonifier::string getAllPermissions() {
			uint64_t allPerms{};
			for (int64_t x = 0; x < 46; ++x) {
				allPerms |= 1ll << x;
			}
			std::stringstream stream{};
			stream << allPerms;
			return jsonifier::string{ stream.str() };
		}

	  protected:
		DCA_INLINE permissions_base() = default;

		DiscordCoreAPI_Dll static jsonifier::string computeOverwrites(jsonifier::string_view basePermissions, const guild_member_data& guildMember, const channel_data& channel);

		DCA_INLINE static jsonifier::string computePermissions(const guild_member_data& guildMember, const channel_data& channel) {
			jsonifier::string permissions = computeBasePermissions(guildMember);
			permissions					  = computeOverwrites(permissions, guildMember, channel);
			return permissions;
		}

		DiscordCoreAPI_Dll static jsonifier::string computeBasePermissions(const guild_member_data& guildMember);
	};

	class permissions_parse : public permissions_base<permissions_parse>, public jsonifier::string {
	  public:
		template<typename value_type> friend class permissions_base;

		DCA_INLINE permissions_parse() = default;

		DCA_INLINE permissions_parse& operator=(jsonifier::string_view valueNew) {
			resize(valueNew.size());
			std::memcpy(data(), valueNew.data(), size());
			return *this;
		}

		DCA_INLINE permissions_parse(jsonifier::string_view valueNew) {
			*this = valueNew;
		}

		DCA_INLINE permissions_parse& operator=(uint64_t valueNew) {
			*this = jsonifier::toString(valueNew);
			return *this;
		}

		DCA_INLINE permissions_parse(uint64_t valueNew) {
			*this = valueNew;
		}

		DCA_INLINE permissions_parse substr(uint64_t offset, uint64_t count) const {
			return substr(offset, count);
		}

		DCA_INLINE uint64_t size() const {
			return jsonifier::string::size();
		}

		DCA_INLINE char* data() const {
			return jsonifier::string::data();
		}

		DCA_INLINE operator uint64_t() const {
			return jsonifier::strToUint64(data());
		}
	};

	class permissions : public permissions_base<permissions> {
	  public:
		template<typename value_type> friend class permissions_base;

		DCA_INLINE permissions() = default;

		DCA_INLINE permissions& operator=(const permissions_parse& other) {
			value = other.operator uint64_t();
			return *this;
		}

		DCA_INLINE permissions(const permissions_parse& other) {
			*this = other;
		}

		DCA_INLINE permissions& operator=(jsonifier::string_view valueNew) {
			value = jsonifier::strToUint64(valueNew.data());
			return *this;
		}

		DCA_INLINE permissions(jsonifier::string_view valueNew) {
			*this = valueNew;
		}

		DCA_INLINE permissions& operator=(jsonifier::string&& valueNew) {
			value = jsonifier::strToUint64(valueNew.data());
			return *this;
		}

		DCA_INLINE permissions(jsonifier::string&& valueNew) {
			*this = std::move(valueNew);
		}

		DCA_INLINE permissions& operator=(uint64_t valueNew) {
			value = valueNew;
			return *this;
		}

		DCA_INLINE permissions(uint64_t valueNew) {
			*this = valueNew;
		}

		DCA_INLINE operator uint64_t() const {
			return value;
		}

		DCA_INLINE operator jsonifier::string() const {
			return jsonifier::toString(value);
		}

	  protected:
		uint64_t value{};
	};

	DiscordCoreAPI_Dll jsonifier::string constructMultiPartData(jsonifier::string_view data, const jsonifier::vector<file>& files);

	DiscordCoreAPI_Dll jsonifier::string convertToLowerCase(jsonifier::string_view stringToConvert);

	DiscordCoreAPI_Dll jsonifier::string base64Encode(jsonifier::string_view, bool = false);

	DiscordCoreAPI_Dll jsonifier::string loadFileContents(jsonifier::string_view filePath);

	DiscordCoreAPI_Dll jsonifier::string utf8MakeValid(jsonifier::string_view inputString);

	DiscordCoreAPI_Dll jsonifier::string urlEncode(jsonifier::string_view inputString);

	DiscordCoreAPI_Dll void spinLock(uint64_t timeInNsToSpinLockFor);

	DiscordCoreAPI_Dll jsonifier::string generateBase64EncodedKey();

	DiscordCoreAPI_Dll bool nanoSleep(int64_t ns);

	/// @brief Acquires a timeStamp with the current time and date - suitable for use in message-embeds.
	/// @return jsonifier::string a string containing the current date-time stamp.
	DiscordCoreAPI_Dll jsonifier::string getTimeAndDate();

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */
	template<typename return_type, bool timeOut = true> class new_thread_awaiter;

	/// @brief An awaitable that can be used to launch the co_routine onto a new thread - as well as return the handle for stoppping its execution.
	/// @tparam return_type the type of value returned by the containing co_routine.
	/// @tparam timeOut whether or not to time out the co_routine's execution after a period of time.
	/// @return new_thread_awaiter<return_type, timeOut> a new_thread_awaiter for suspendint the current co_routine's execution.
	template<typename return_type, bool timeOut = true> DCA_INLINE auto newThreadAwaitable() {
		return new_thread_awaiter<return_type, timeOut>{};
	}

	/**@}*/
};
