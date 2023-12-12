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
/// WebHookEntities.hpp - the header file for web_hook_data relate stuff.
/// Dec 1, 2021
/// https://discordcoreapi.com
/// \file WebHookEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For creating a new web_hook_data.
	struct create_web_hook_data {
		snowflake channelId{};///< The channel_data within which to create the web_hook_data.
		jsonifier::string avatar{};///< Image for the default webhook avatar.
		jsonifier::string name{};///< Name of the webhook(1 - 80 characters).
	};

	/// @brief For executing a web_hook.
	class DiscordCoreAPI_Dll execute_web_hook_data {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class create_ephemeral_follow_up_message_data;
		friend class create_follow_up_message_data;
		friend class edit_follow_up_message_data;
		friend class interactions;
		friend class web_hooks;

		snowflake threadId{};///< Send a message to the specified thread within a webhook's channel_data. the thread will automatically be unarchived.
		bool wait{};///< Waits for server confirmation of message send before response, and returns the created message body.

		execute_web_hook_data() = default;

		execute_web_hook_data(const web_hook_data& dataNew);

		/// @brief Adds a button to the response message_data.
		/// @param disabled whether the button is active or not.
		/// @param customIdNew a custom id to give for identifying the button.
		/// @param buttonLabel a visible label for the button.
		/// @param buttonStyle the style of the button.
		/// @param emojiName an emoji name, if desired.
		/// @param emojiId an emoji id, if desired.
		/// @param url a url, if applicable.
		/// @return respond_to_input_event_data& a reference to this data structure.
		execute_web_hook_data& addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, button_style buttonStyle,
			jsonifier::string_view emojiName = "", snowflake emojiId = snowflake{}, jsonifier::string_view url = "");

		/// @brief Adds a select-menu to the response message_data.
		/// @param disabled whether the select-menu is active or not.
		/// @param customIdNew a custom id to give for identifying the select-menu.
		/// @param options a vector of select-menu-options to offer.
		/// @param placeholder custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues maximum number of selections that are possible.
		/// @param minValues minimum required number of selections that are required.
		/// @param type the type of select-menu that this is.
		/// @param channelTypes types of channels that can be accepted if this is of the type channel_type.
		/// @return respond_to_input_event_data& a reference to this data structure.
		execute_web_hook_data& addSelectMenu(bool disabled, jsonifier::string_view customIdNew, const jsonifier::vector<select_option_data>& options,
			jsonifier::string_view placeholder, int32_t maxValues, int32_t minValues, select_menu_type type,
			const jsonifier::vector<channel_type>& channelTypes = jsonifier::vector<channel_type>{});

		/// @brief Adds a modal to the response message_data.
		/// @param topTitleNew a title for the modal.
		/// @param topCustomIdNew a custom id to give for the modal.
		/// @param titleNew a title for the modal's individual input.
		/// @param customIdNew a custom id to give for the modal's individual input.
		/// @param required is it a required response?
		/// @param minLength minimum length.
		/// @param maxLength maximum length.
		/// @param inputStyle the input style.
		/// @param label a label for the modal.
		/// @param placeholder a placeholder for the modal.
		/// @return respond_to_input_event_data& a reference to this data structure.
		execute_web_hook_data& addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
			jsonifier::string_view customIdNew, bool required, int32_t minLength, int32_t maxLength, text_input_style inputStyle, jsonifier::string_view label = "",
			jsonifier::string_view placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile the file to be added.
		/// @return message_response_base& a reference to this data structure.
		execute_web_hook_data& addFile(const file& theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage an allowed_mentions_data structure.
		/// @return message_response_base& a reference to this data structure.
		execute_web_hook_data& addAllowedMentions(const allowed_mentions_data dataPackage);

		/// @brief For setting the components in a response.
		/// @param dataPackage an action_row_data structure.
		/// @return message_response_base& a reference to this data structure.
		execute_web_hook_data& addComponentRow(const action_row_data dataPackage);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage an embed_data structure.
		/// @return message_response_base& a reference to this data structure.
		execute_web_hook_data& addMessageEmbed(const embed_data dataPackage);

		/// @brief For setting the content in a response.
		/// @param dataPackage a string, containing the content.
		/// @return message_response_base& a reference to this data structure.
		execute_web_hook_data& addContent(jsonifier::string_view dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs a bool.
		/// @return message_response_base& a reference to this data structure.
		execute_web_hook_data& setTTSStatus(bool enabledTTs);

		void generateExcludedKeys();

		unordered_set<jsonifier::string> jsonifierExcludedKeys{};

	  protected:
		jsonifier::vector<attachment_data> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		jsonifier::vector<action_row_data> components{};///< Array of message component the components to include with the message.
		allowed_mentions_data allowedMentions{};///< Allowed mention object.
		jsonifier::vector<embed_data> embeds{};///< Array of up to 10 embed objects.
		jsonifier::vector<file> files{};///< File contents the contents of the file being sent.
		jsonifier::string webhookToken{};///< The web_hook_data token you would like to execute.
		jsonifier::string avatarUrl{};///< Override the default avatar of the webhook.
		jsonifier::string userName{};///< Override the default username of the webhook.
		jsonifier::string customId{};///< custom id for the modal.
		snowflake webHookId{};///< The web_hook_data you would like to execute.
		jsonifier::string content{};///< The message contents (up to 2000 characters)	one of content, file, embeds.
		jsonifier::string title{};///< Title for the modal.
		uint64_t flags{};///< Flags combined as a bitfield.
		bool tts{};///< True if this is a tts message.
	};

	/// @brief For editing a web_hook message_data.
	class DiscordCoreAPI_Dll edit_web_hook_data : public execute_web_hook_data {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class edit_interaction_response_data;
		friend class edit_follow_up_message_data;
		friend class interactions;
		friend class web_hooks;

		snowflake messageId{};///< The message snowflake to collect.
		snowflake threadId{};///< Send a message to the specified thread within a webhook's channel_data. the thread will automatically be unarchived.
		bool wait{};///< Waits for server confirmation of message send before response, and returns the created message body.

		execute_web_hook_data& setTTSStatus(bool) = delete;

		edit_web_hook_data() = default;

		edit_web_hook_data(const web_hook_data& dataNew);
	};

	/// @brief For collecting a list of web_hooks from a chosen channel_data.
	struct get_channel_web_hooks_data {
		snowflake channelId{};///< The channel_data from which to collect the web_hooks.
	};

	/// @brief For collecting a list of web_hooks from a chosen guild.
	struct get_guild_web_hooks_data {
		snowflake guildId{};///< The guild from which to collect the web_hooks.
	};

	/// @brief Collects a single web_hook_data.
	struct get_web_hook_data {
		snowflake webHookId{};///< snowflake of the desired web_hook_data to collect.
	};

	/// @brief Collects a single web_hook_data, using the token and snowflake.
	struct get_web_hook_with_token_data {
		jsonifier::string webhookToken{};///< Token of the desired web_hook_data.
		snowflake webHookId{};///< snowflake of the desired web_hook_data.
	};

	/// @brief For modifying a web_hook.
	struct modify_web_hook_data {
		snowflake channelId{};///< The new channel_data id this webhook should be moved to.
		snowflake webHookId{};///< The web_hook_data to be modified.
		jsonifier::string avatar{};///< Image responseData for the default webhook avatar.
		jsonifier::string name{};///< The default name of the webhook.
	};

	/// @brief For modifying a web_hook.
	struct modify_web_hook_with_token_data {
		jsonifier::string webhookToken{};///< Token of the desired web_hook_data.
		snowflake channelId{};///< The new channel_data id this webhook should be moved to.
		snowflake webHookId{};///< The web_hook_data to be modified.
		jsonifier::string avatar{};///< Image responseData for the default webhook avatar.
		jsonifier::string name{};///< The default name of the webhook.
	};

	/// @brief For deleting a web_hook.
	struct delete_web_hook_data {
		snowflake webHookId{};///< The desired web_hook_data to delete.
	};

	/// @brief For deleting a web_hook, using its token.
	struct delete_web_hook_with_token_data {
		jsonifier::string webhookToken{};///< Token of the desired web_hook_data.
		snowflake webHookId{};///< The desired web_hook_data to delete.
	};

	/// @brief For getting a web_hook message_data.
	struct get_web_hook_message_data {
		jsonifier::string webhookToken{};///< The web_hook_data token you would like to collect.
		snowflake webHookId{};///< The web_hook_data you would like to collect.
		snowflake messageId{};///< The message snowflake to collect.
		snowflake threadId{};///< The thread that the message is in.
	};

	/// @brief For deleting a web_hook message_data.
	struct delete_web_hook_message_data {
		jsonifier::string webhookToken{};///< The web_hook_data token you would like to collect.
		snowflake webHookId{};///< The web_hook_data you would like to collect.
		snowflake messageId{};///< The message snowflake to collect.
		snowflake threadId{};///< Send a message to the specified thread within a webhook's channel_data. the thread will automatically be unarchived.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the web_hook_data related discord endpoints;
	class DiscordCoreAPI_Dll web_hooks {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Creates a new web_hook_data.
		/// @param dataPackage a get_message_data structure.
		/// @return a co_routine containing a web_hook.
		static co_routine<web_hook_data> createWebHookAsync(create_web_hook_data dataPackage);

		/// @brief Collects a list of web_hooks from a chosen channel_data.
		/// @param dataPackage a get_channel_web_hooks_data structure.
		/// @return a co_routine containing a jsonifier::vector<web_hook_data>.
		static co_routine<jsonifier::vector<web_hook_data>> getChannelWebHooksAsync(get_channel_web_hooks_data dataPackage);

		/// @brief Collects a list of web_hooks from a chosen guild.
		/// @param dataPackage a get_guild_web_hooks_data structure.
		/// @return a co_routine containing a jsonifier::vector<web_hook_data>.
		static co_routine<jsonifier::vector<web_hook_data>> getGuildWebHooksAsync(get_guild_web_hooks_data dataPackage);

		/// @brief Collects a single web_hook_data.
		/// @param dataPackage a get_web_hook_data structure.
		/// @return a co_routine containing a web_hook.
		static co_routine<web_hook_data> getWebHookAsync(get_web_hook_data dataPackage);

		/// @brief Collects a single web_hook_data, using the token.
		/// @param dataPackage a get_web_hook_with_token_data structure.
		/// @return a co_routine containing a web_hook.
		static co_routine<web_hook_data> getWebHookWithTokenAsync(get_web_hook_with_token_data dataPackage);

		/// @brief Modifies a single web_hook_data.
		/// @param dataPackage a modify_web_hook_data structure.
		/// @return a co_routine containing a web_hook.
		static co_routine<web_hook_data> modifyWebHookAsync(modify_web_hook_data dataPackage);

		/// @brief Modifies a single web_hook_data, using its token.
		/// @param dataPackage a modify_web_hook_with_token_data structure.
		/// @return a co_routine containing a web_hook.
		static co_routine<web_hook_data> modifyWebHookWithTokenAsync(modify_web_hook_with_token_data dataPackage);

		/// @brief Deletes a single web_hook_data.
		/// @param dataPackage a delete_web_hook_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteWebHookAsync(delete_web_hook_data dataPackage);

		/// @brief Deletes a single web_hook_data, using its token.
		/// @param dataPackage a delete_web_hook_with_token_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteWebHookWithTokenAsync(delete_web_hook_with_token_data dataPackage);

		/// @brief Executes a single web_hook_data.
		/// @param dataPackage an execute_web_hook_data structure.
		/// @return a co_routine containing a message.
		static co_routine<message_data> executeWebHookAsync(execute_web_hook_data dataPackage);

		/// @brief Collects a web_hook message_data.
		/// @param dataPackage an get_web_hook_message_data structure.
		/// @return a co_routine containing a message.
		static co_routine<message_data> getWebHookMessageAsync(get_web_hook_message_data dataPackage);

		/// @brief Edits a web_hook message_data.
		/// @param dataPackage an edit_web_hook_data_message_data structure.
		/// @return a co_routine containing a message.
		static co_routine<message_data> editWebHookMessageAsync(edit_web_hook_data dataPackage);

		/// @brief Deletes a web_hook message_data.
		/// @param dataPackage an delete_web_hook_message_data structure.
		/// @return a co_routine containing a message.
		static co_routine<void> deleteWebHookMessageAsync(delete_web_hook_message_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};
	/**@}*/

}
