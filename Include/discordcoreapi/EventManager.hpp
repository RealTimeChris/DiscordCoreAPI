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
/// EventManager.hpp - Header for the event manager class.
/// Apr 26, 2021
/// https://discordcoreapi.com
/// \file EventManager.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/ApplicationCommandEntities.hpp>
#include <discordcoreapi/AutoModerationEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/ReactionEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup discord_events
	 * @{
	 */

	template<typename value_type> struct event_data {
	  public:
		value_type value{};
	};

	template<typename value_type, typename old_value_type> struct updated_event_data {
	  public:
		old_value_type oldValue{};
		value_type value{};
	};

	struct guild_member_remove_data {
		snowflake guildId{};
		user_data user{};
	};

	struct guild_ban_add_data {
		snowflake guildId{};
		user_data user{};
	};

	struct guild_ban_remove_data {
		snowflake guildId{};
		user_data user{};
	};

	struct role_creation_data {
		snowflake guildId{};
		role_data role{};
	};

	struct role_update_data {
		snowflake guildId{};
		role_data role{};
	};

	struct role_deletion_data {
		snowflake guildId{};
		role_data role{};
	};

	struct guild_scheduled_event_user_add_data {
		snowflake guildScheduledEventId{};
		snowflake guildId{};
		snowflake userId{};
	};

	struct guild_scheduled_event_user_remove_data {
		snowflake guildScheduledEventId{};
		snowflake guildId{};
		snowflake userId{};
	};

	struct integration_creation_data {
		integration_data integrationData{};///< The new integration_data structure.
		snowflake guildId{};///< The id of the guild for which this integration was created.
	};

	struct integration_update_data {
		integration_data integrationData{};///< New integration_data structure.
		snowflake guildId{};///< The id of the guild for which the integration was updated.
	};

	struct integration_deletion_data {
		snowflake applicationId{};///< Application id of the current application.
		snowflake guildId{};///< The id of the guild for which the integration was deleted.
		snowflake id{};///< The id of the deleted integration.
	};

	struct invite_deletion_data {
		snowflake channelId{};///< The id of the channel for which the invite existed.
		snowflake guildId{};///< The id of the guild for which the invite existed.
		jsonifier::string code{};///< The code of the invite.
	};

	struct message_deletion_data {
		snowflake messageId{};///< The id of the message which was deleted.
		snowflake channelId{};///< The id of the channel from which the message was deleted.
		snowflake guildId{};///< The id of the guild from which the message was deleted.
	};

	struct message_deletion_bulk_data {
		jsonifier::vector<snowflake> ids{};///< A vector containing the list of deleted message ids.
		snowflake channelId{};///< The id of the channel from which the message was deleted.
		snowflake guildId{};///< The id of the guild from which the message was deleted.
	};

	struct reaction_remove_all_data {
		snowflake channelId{};///< The id of the channel from which the reactions were deleted.
		snowflake messageId{};///< The id of the message from which the reactions were deleted.
		snowflake guildId{};///< The id of the guild from which the reactions were deleted.
	};

	struct reaction_remove_emoji_data {
		snowflake messageId{};///< The id of the message from which the reactions were deleted.
		snowflake channelId{};///< The id of the channel from which the reactions were deleted.
		snowflake guildId{};///< The id of the guild from which the reactions were deleted.
		emoji_data emoji{};///< The id of the emoji which was removed from the message_data.
	};

	struct web_hook_update_data {
		snowflake channelId{};///< Id of the channel for which the web_hook update is occurring.
		snowflake guildId{};///< Id of the guild for which the web_hook update is occurring.
	};

	/// @brief Data that is received as part of a gateway ping event.
	struct on_gateway_ping_data : public event_data<int32_t> {
		int32_t timeUntilNextPing{};///< The number of milliseconds until the next ping.
	};

	/// @brief Data that is received as part of an input_event creation event.
	struct DiscordCoreAPI_Dll on_input_event_creation_data : public event_data<input_event_data> {
		on_input_event_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an application_command_permissions update event.
	struct DiscordCoreAPI_Dll on_application_command_permissions_update_data : public event_data<guild_application_command_permissions_data> {
		on_application_command_permissions_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an auto_moderation_rule_data creation event.
	struct DiscordCoreAPI_Dll on_auto_moderation_rule_creation_data : public event_data<auto_moderation_rule_data> {
		on_auto_moderation_rule_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an auto_moderation_rule_data update event.
	struct DiscordCoreAPI_Dll on_auto_moderation_rule_update_data : public event_data<auto_moderation_rule_data> {
		on_auto_moderation_rule_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an auto_moderation_rule_data delete event.
	struct DiscordCoreAPI_Dll on_auto_moderation_rule_deletion_data : public event_data<auto_moderation_rule_data> {
		on_auto_moderation_rule_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an auto_moderation_action execution event.
	struct DiscordCoreAPI_Dll on_auto_moderation_action_execution_data : public event_data<auto_moderation_action_execution_event_data> {
		on_auto_moderation_action_execution_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a channel creation event.
	struct DiscordCoreAPI_Dll on_channel_creation_data : public event_data<channel_data> {
		on_channel_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a channel update event.
	struct DiscordCoreAPI_Dll on_channel_update_data : public updated_event_data<channel_data, channel_data> {
		on_channel_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a channel deletion event.
	struct DiscordCoreAPI_Dll on_channel_deletion_data : public event_data<channel_data> {
		on_channel_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a channel pins update event.
	struct DiscordCoreAPI_Dll on_channel_pins_update_data : public event_data<channel_pins_update_event_data> {
		on_channel_pins_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a thread_data creation event.
	struct DiscordCoreAPI_Dll on_thread_creation_data : public event_data<thread_data> {
		on_thread_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a thread_data update event.
	struct DiscordCoreAPI_Dll on_thread_update_data : public event_data<thread_data> {
		on_thread_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a thread_data deletion event.
	struct DiscordCoreAPI_Dll on_thread_deletion_data : public event_data<thread_data> {
		on_thread_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a thread_data list sync event.
	struct DiscordCoreAPI_Dll on_thread_list_sync_data : public event_data<thread_list_sync_data> {
		on_thread_list_sync_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a thread_data member update event.
	struct DiscordCoreAPI_Dll on_thread_member_update_data : public event_data<thread_member_data> {
		on_thread_member_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a thread_data members update event.
	struct DiscordCoreAPI_Dll on_thread_members_update_data : public event_data<thread_members_update_data> {
		on_thread_members_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild creation event.
	struct DiscordCoreAPI_Dll on_guild_creation_data : public event_data<guild_data> {
		on_guild_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild update event.
	struct DiscordCoreAPI_Dll on_guild_update_data : public updated_event_data<guild_data, guild_data> {
		on_guild_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild deletion event.
	struct DiscordCoreAPI_Dll on_guild_deletion_data : public event_data<guild_data> {
		on_guild_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild ban add event.
	struct DiscordCoreAPI_Dll on_guild_ban_add_data : public event_data<guild_ban_add_data> {
		on_guild_ban_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild ban add event.
	struct DiscordCoreAPI_Dll on_guild_ban_remove_data : public event_data<guild_ban_remove_data> {
		on_guild_ban_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild emojis update event.
	struct DiscordCoreAPI_Dll on_guild_emojis_update_data : public event_data<guild_emojis_update_event_data> {
		on_guild_emojis_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild sticker update event.
	struct DiscordCoreAPI_Dll on_guild_stickers_update_data : public event_data<guild_stickers_update_event_data> {
		on_guild_stickers_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild integration update event.
	struct DiscordCoreAPI_Dll on_guild_integrations_update_data : public event_data<snowflake> {
		on_guild_integrations_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_member add event.
	struct DiscordCoreAPI_Dll on_guild_member_add_data : public event_data<guild_member_data> {
		on_guild_member_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_member update event.
	struct DiscordCoreAPI_Dll on_guild_member_update_data : public updated_event_data<guild_member_data, guild_member_data> {
		on_guild_member_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_member remove event.
	struct DiscordCoreAPI_Dll on_guild_member_remove_data : public event_data<guild_member_remove_data> {
		on_guild_member_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_members chunk event.
	struct DiscordCoreAPI_Dll on_guild_members_chunk_data : public event_data<guild_members_chunk_event_data> {
		on_guild_members_chunk_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a role_data creation event.
	struct DiscordCoreAPI_Dll on_role_creation_data : public event_data<role_creation_data> {
		on_role_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a role_data update event.
	struct DiscordCoreAPI_Dll on_role_update_data : public updated_event_data<role_update_data, role_data> {
		on_role_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a role_data deletion event.
	struct DiscordCoreAPI_Dll on_role_deletion_data : public event_data<role_deletion_data> {
		on_role_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_scheduled_event_data creation event.
	struct DiscordCoreAPI_Dll on_guild_scheduled_event_creation_data : public event_data<guild_scheduled_event_data> {
		on_guild_scheduled_event_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_scheduled_event_data update event.
	struct DiscordCoreAPI_Dll on_guild_scheduled_event_update_data : public event_data<guild_scheduled_event_data> {
		on_guild_scheduled_event_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_scheduled_event_data delete event.
	struct DiscordCoreAPI_Dll on_guild_scheduled_event_deletion_data : public event_data<guild_scheduled_event_data> {
		on_guild_scheduled_event_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_scheduled_event_data user add event.
	struct DiscordCoreAPI_Dll on_guild_scheduled_event_user_add_data : public event_data<guild_scheduled_event_user_add_data> {
		on_guild_scheduled_event_user_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a guild_scheduled_event_data user remove event.
	struct DiscordCoreAPI_Dll on_guild_scheduled_event_user_remove_data : public event_data<guild_scheduled_event_user_remove_data> {
		on_guild_scheduled_event_user_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an integration creation event.
	struct DiscordCoreAPI_Dll on_integration_creation_data : public event_data<integration_creation_data> {
		on_integration_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an integration update event.
	struct DiscordCoreAPI_Dll on_integration_update_data : public event_data<integration_update_data> {
		on_integration_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an integration deletion event.
	struct DiscordCoreAPI_Dll on_integration_deletion_data : public event_data<integration_deletion_data> {
		on_integration_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an invite creation event.
	struct DiscordCoreAPI_Dll on_invite_creation_data : public event_data<invite_data> {
		on_invite_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an invite deletion event.
	struct DiscordCoreAPI_Dll on_invite_deletion_data : public event_data<invite_deletion_data> {
		on_invite_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of an interaction creation event.
	struct DiscordCoreAPI_Dll on_interaction_creation_data : public event_data<interaction_data> {
		on_interaction_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a message creation event.
	struct DiscordCoreAPI_Dll on_message_creation_data : public event_data<message_data> {
		on_message_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a message update event.
	struct DiscordCoreAPI_Dll on_message_update_data : public event_data<message_data> {
		on_message_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a message deletion event.
	struct DiscordCoreAPI_Dll on_message_deletion_data : public event_data<message_deletion_data> {
		on_message_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a message delete bulk event.
	struct DiscordCoreAPI_Dll on_message_delete_bulk_data : public event_data<message_deletion_bulk_data> {
		on_message_delete_bulk_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a reaction add event.
	struct DiscordCoreAPI_Dll on_reaction_add_data : public event_data<reaction_data> {
		on_reaction_add_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a reaction remove event.
	struct DiscordCoreAPI_Dll on_reaction_remove_data : public event_data<reaction_remove_data> {
		reaction_remove_data reactionRemoveData{};///< The reaction_remove_data.
		on_reaction_remove_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a reaction remove all event.
	struct DiscordCoreAPI_Dll on_reaction_remove_all_data : public event_data<reaction_remove_all_data> {
		on_reaction_remove_all_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a reaction remove emoji event.
	struct DiscordCoreAPI_Dll on_reaction_remove_emoji_data : public event_data<reaction_remove_emoji_data> {
		on_reaction_remove_emoji_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a presence update event.
	struct DiscordCoreAPI_Dll on_presence_update_data : public event_data<presence_update_data> {
		on_presence_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a stage_instance_data creation event.
	struct DiscordCoreAPI_Dll on_stage_instance_creation_data : public event_data<stage_instance_data> {
		on_stage_instance_creation_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a stage_instance_data update event.
	struct DiscordCoreAPI_Dll on_stage_instance_update_data : public event_data<stage_instance_data> {
		on_stage_instance_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a stage_instance_data deletion event.
	struct DiscordCoreAPI_Dll on_stage_instance_deletion_data : public event_data<stage_instance_data> {
		on_stage_instance_deletion_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a typing start event.
	struct DiscordCoreAPI_Dll on_typing_start_data : public event_data<typing_start_data> {
		on_typing_start_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a user update event.
	struct DiscordCoreAPI_Dll on_user_update_data : public updated_event_data<user_data, user_data> {
		on_user_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received as part of a voice state update event.
	struct DiscordCoreAPI_Dll on_voice_state_update_data : public event_data<voice_state_data> {
		on_voice_state_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse, discord_core_internal::websocket_client* sslShard);
	};

	/// @brief Data that is received as part of a voice server update event.
	struct DiscordCoreAPI_Dll on_voice_server_update_data : public event_data<voice_server_update_data> {
		on_voice_server_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse, discord_core_internal::websocket_client* sslShard);
	};

	/// @brief Data that is received as part of a web_hook update event.
	struct DiscordCoreAPI_Dll on_webhook_update_data : public event_data<web_hook_update_data> {
		on_webhook_update_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Data that is received upon the bot receiving an autocomplete entry.
	struct DiscordCoreAPI_Dll on_auto_complete_entry_data : public event_data<input_event_data> {
		on_auto_complete_entry_data(jsonifier::jsonifier_core<false>& parserNew, jsonifier::string_view_base<uint8_t> dataToParse);
	};

	/// @brief Class for handling the assignment of event-handling functions.int32_t
	class DiscordCoreAPI_Dll event_manager {
	  public:
		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_gateway_ping_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGatewayPing(discord_core_internal::event_delegate<co_routine<void>, const on_gateway_ping_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGatewayPing(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_application_command_permissions_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onApplicationCommandsPermissionsUpdate(
			discord_core_internal::event_delegate<co_routine<void>, const on_application_command_permissions_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onApplicationCommandsPermissionsUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_auto_moderation_rule_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onAutoModerationRuleCreation(
			discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_rule_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onAutoModerationRuleCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_auto_moderation_rule_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onAutoModerationRuleUpdate(
			discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_rule_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onAutoModerationRuleUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_auto_moderation_rule_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onAutoModerationRuleDeletion(
			discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_rule_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onAutoModerationRuleDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_auto_moderation_action_execution_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onAutoModerationActionExecution(
			discord_core_internal::event_delegate<co_routine<void>, const on_auto_moderation_action_execution_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onAutoModerationActionExecution(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_input_event_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onAutoCompleteEntry(discord_core_internal::event_delegate<co_routine<void>, const on_auto_complete_entry_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onAutoCompleteEntry(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_input_event_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onInputEventCreation(discord_core_internal::event_delegate<co_routine<void>, const on_input_event_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onInputEventCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_channel_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onChannelCreation(discord_core_internal::event_delegate<co_routine<void>, const on_channel_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onChannelCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_channel_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onChannelUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_channel_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onChannelUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_channel_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onChannelDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_channel_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onChannelDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_channel_pins_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onChannelPinsUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_channel_pins_update_data&> handler);
		/// @brief For removing afunction from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onChannelPinsUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_thread_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onThreadCreation(discord_core_internal::event_delegate<co_routine<void>, const on_thread_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onThreadCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_thread_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onThreadUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_thread_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onThreadUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_thread_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onThreadDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_thread_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onThreadDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_thread_list_sync_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onThreadListSync(discord_core_internal::event_delegate<co_routine<void>, const on_thread_list_sync_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onThreadListSync(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_thread_member_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onThreadMemberUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_thread_member_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onThreadMemberUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_thread_members_update_data structure as an argument
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onThreadMembersUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_thread_members_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onThreadMembersUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildCreation(discord_core_internal::event_delegate<co_routine<void>, const on_guild_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_deletion_data structure as an argument
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_guild_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_ban_add_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildBanAdd(discord_core_internal::event_delegate<co_routine<void>, const on_guild_ban_add_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildBanAdd(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_ban_remove_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildBanRemove(discord_core_internal::event_delegate<co_routine<void>, const on_guild_ban_remove_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildBanRemove(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_emojis_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildEmojisUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_emojis_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildEmojisUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_stickers_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildStickersUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_stickers_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildStickersUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_integrations_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildIntegrationsUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_integrations_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildIntegrationsUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_members_chunk_data structure as an argument.
		/// @return an discord_core_internal::event_delegate_token, > for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildMembersChunk(discord_core_internal::event_delegate<co_routine<void>, const on_guild_members_chunk_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildMembersChunk(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_member_add_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildMemberAdd(discord_core_internal::event_delegate<co_routine<void>, const on_guild_member_add_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildMemberAdd(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_member_remove_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildMemberRemove(discord_core_internal::event_delegate<co_routine<void>, const on_guild_member_remove_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildMemberRemove(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_member_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildMemberUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_guild_member_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildMemberUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_role_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onRoleCreation(discord_core_internal::event_delegate<co_routine<void>, const on_role_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onRoleCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_role_update_data structure as an argument
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onRoleUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_role_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onRoleUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_role_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onRoleDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_role_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onRoleDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_scheduled_event_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildScheduledEventCreation(
			discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildScheduledEventCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_scheduled_event_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildScheduledEventUpdate(
			discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildScheduledEventUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_scheduled_event_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildScheduledEventDeletion(
			discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildScheduledEventDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_scheduled_event_user_add_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildScheduledEventUserAdd(
			discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_user_add_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildScheduledEventUserAdd(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_guild_scheduled_event_user_remove_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onGuildScheduledEventUserRemove(
			discord_core_internal::event_delegate<co_routine<void>, const on_guild_scheduled_event_user_remove_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onGuildScheduledEventUserRemove(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_integration_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onIntegrationCreation(discord_core_internal::event_delegate<co_routine<void>, const on_integration_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onIntegrationCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_integration_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onIntegrationUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_integration_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onIntegrationUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_integration_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onIntegrationDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_integration_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onIntegrationDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_interaction_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onInteractionCreation(discord_core_internal::event_delegate<co_routine<void>, const on_interaction_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onInteractionCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_invite_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onInviteCreation(discord_core_internal::event_delegate<co_routine<void>, const on_invite_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onInviteCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_invite_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onInviteDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_invite_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onInviteDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_message_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onMessageCreation(discord_core_internal::event_delegate<co_routine<void>, const on_message_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onMessageCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_message_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onMessageUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_message_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onMessageUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_message_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onMessageDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_message_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onMessageDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_message_delete_bulk_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onMessageDeleteBulk(discord_core_internal::event_delegate<co_routine<void>, const on_message_delete_bulk_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onMessageDeleteBulk(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_reaction_add_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onReactionAdd(discord_core_internal::event_delegate<co_routine<void>, const on_reaction_add_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onReactionAdd(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_reaction_remove_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onReactionRemove(discord_core_internal::event_delegate<co_routine<void>, const on_reaction_remove_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token,from the original event registration.
		void onReactionRemove(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_reaction_remove_all_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onReactionRemoveAll(discord_core_internal::event_delegate<co_routine<void>, const on_reaction_remove_all_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onReactionRemoveAll(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_reaction_remove_emoji_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onReactionRemoveEmoji(discord_core_internal::event_delegate<co_routine<void>, const on_reaction_remove_emoji_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onReactionRemoveEmoji(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_presence_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onPresenceUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_presence_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onPresenceUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_stage_instance_creation_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onStageInstanceCreation(discord_core_internal::event_delegate<co_routine<void>, const on_stage_instance_creation_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onStageInstanceCreation(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_stage_instance_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onStageInstanceUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_stage_instance_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onStageInstanceUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_stage_instance_deletion_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onStageInstanceDeletion(discord_core_internal::event_delegate<co_routine<void>, const on_stage_instance_deletion_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onStageInstanceDeletion(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_typing_start_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onTypingStart(discord_core_internal::event_delegate<co_routine<void>, const on_typing_start_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onTypingStart(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_user_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onUserUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_user_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onUserUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_voice_state_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onVoiceStateUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_voice_state_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onVoiceStateUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_voice_server_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onVoiceServerUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_voice_server_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onVoiceServerUpdate(discord_core_internal::event_delegate_token& token);

		/// @brief For adding a function to handle this event.
		/// @param handler a delegate taking an on_webhook_update_data structure as an argument.
		/// @return an event_token for later de-registering the event.
		discord_core_internal::event_delegate_token onWebhookUpdate(discord_core_internal::event_delegate<co_routine<void>, const on_webhook_update_data&> handler);
		/// @brief For removing a function from handling this event.
		/// @param token an discord_core_internal::event_delegate_token, from the original event registration.
		void onWebhookUpdate(discord_core_internal::event_delegate_token& token);

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_gateway_ping_data&> onGatewayPingEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_application_command_permissions_update_data&> onApplicationCommandPermissionsUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_auto_moderation_rule_creation_data&> onAutoModerationRuleCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_auto_moderation_rule_update_data&> onAutoModerationRuleUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_auto_moderation_rule_deletion_data&> onAutoModerationRuleDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_auto_moderation_action_execution_data&> onAutoModerationActionExecutionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_auto_complete_entry_data&> onAutoCompleteEntryEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_input_event_creation_data&> onInputEventCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_channel_creation_data&> onChannelCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_channel_update_data&> onChannelUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_channel_deletion_data&> onChannelDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_channel_pins_update_data&> onChannelPinsUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_thread_creation_data&> onThreadCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_thread_update_data&> onThreadUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_thread_deletion_data&> onThreadDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_thread_list_sync_data&> onThreadListSyncEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_thread_member_update_data&> onThreadMemberUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_thread_members_update_data&> onThreadMembersUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_creation_data&> onGuildCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_update_data&> onGuildUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_deletion_data&> onGuildDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_ban_add_data&> onGuildBanAddEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_ban_remove_data&> onGuildBanRemoveEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_emojis_update_data&> onGuildEmojisUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_stickers_update_data&> onGuildStickersUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_integrations_update_data&> onGuildIntegrationsUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_member_add_data&> onGuildMemberAddEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_member_remove_data&> onGuildMemberRemoveEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_member_update_data&> onGuildMemberUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_members_chunk_data&> onGuildMembersChunkEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_role_creation_data&> onRoleCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_role_update_data&> onRoleUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_role_deletion_data&> onRoleDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_scheduled_event_creation_data&> onGuildScheduledEventCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_scheduled_event_update_data&> onGuildScheduledEventUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_scheduled_event_deletion_data&> onGuildScheduledEventDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_scheduled_event_user_add_data&> onGuildScheduledEventUserAddEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_guild_scheduled_event_user_remove_data&> onGuildScheduledEventUserRemoveEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_integration_creation_data&> onIntegrationCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_integration_update_data&> onIntegrationUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_integration_deletion_data&> onIntegrationDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_interaction_creation_data&> onInteractionCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_invite_creation_data&> onInviteCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_invite_deletion_data&> onInviteDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_message_creation_data&> onMessageCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_message_update_data&> onMessageUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_message_deletion_data&> onMessageDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_message_delete_bulk_data&> onMessageDeleteBulkEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_reaction_add_data&> onReactionAddEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_reaction_remove_data&> onReactionRemoveEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_reaction_remove_all_data&> onReactionRemoveAllEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_reaction_remove_emoji_data&> onReactionRemoveEmojiEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_presence_update_data&> onPresenceUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_stage_instance_creation_data&> onStageInstanceCreationEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_stage_instance_update_data&> onStageInstanceUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_stage_instance_deletion_data&> onStageInstanceDeletionEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_typing_start_data&> onTypingStartEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_user_update_data&> onUserUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_voice_state_update_data&> onVoiceStateUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_voice_server_update_data&> onVoiceServerUpdateEvent{};

		DCA_INLINE static discord_core_internal::event<co_routine<void>, const on_webhook_update_data&> onWebhookUpdateEvent{};
	};
	/**@}*/

}// namespace discord_core_api
