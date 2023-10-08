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
/// WebHookEntities.hpp - The header file for WebHookData relate stuff.
/// Dec 1, 2021
/// https://discordcoreapi.com
/// \file WebHookEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For creating a new WebHookData.
	struct CreateWebHookData {
		Snowflake channelId{};///< The ChannelData within which to create the WebHookData.
		jsonifier::string avatar{};///< Image for the default webhook avatar.
		jsonifier::string name{};///< Name of the webhook(1 - 80 characters).
	};

	/// @brief For executing a WebHook.
	class DiscordCoreAPI_Dll ExecuteWebHookData {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class CreateEphemeralFollowUpMessageData;
		friend class CreateFollowUpMessageData;
		friend class EditFollowUpMessageData;
		friend class Interactions;
		friend class WebHooks;

		Snowflake threadId{};///< Send a message to the specified thread within a webhook's ChannelData. The thread will automatically be unarchived.
		bool wait{};///< Waits for server confirmation of message send before response, and returns the created message body.

		ExecuteWebHookData() = default;

		ExecuteWebHookData(WebHookData dataNew);

		/// @brief Adds a button to the response MessageData.
		/// @param disabled Whether the button is active or not.
		/// @param customIdNew A custom id to give for identifying the button.
		/// @param buttonLabel A visible label for the button.
		/// @param buttonStyle The style of the button.
		/// @param emojiName An emoji name, if desired.
		/// @param emojiId An emoji id, if desired.
		/// @param url A url, if applicable.
		/// @return RespondToInputEventData& A reference to this data structure.
		ExecuteWebHookData& addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, ButtonStyle buttonStyle,
			jsonifier::string_view emojiName = "", Snowflake emojiId = Snowflake{}, jsonifier::string_view url = "");

		/// @brief Adds a select-menu to the response MessageData.
		/// @param disabled Whether the select-menu is active or not.
		/// @param customIdNew A custom id to give for identifying the select-menu.
		/// @param options A vector of select-menu-options to offer.
		/// @param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues Maximum number of selections that are possible.
		/// @param minValues Minimum required number of selections that are required.
		/// @param type The type of select-menu that this is.
		/// @param channelTypes Types of channels that can be accepted if this is of the type ChannelType.
		/// @return RespondToInputEventData& A reference to this data structure.
		ExecuteWebHookData& addSelectMenu(bool disabled, jsonifier::string_view customIdNew, jsonifier::vector<SelectOptionData> options, jsonifier::string_view placeholder,
			int32_t maxValues, int32_t minValues, SelectMenuType type, jsonifier::vector<ChannelType> channelTypes = jsonifier::vector<ChannelType>{});

		/// @brief Adds a modal to the response MessageData.
		/// @param topTitleNew A title for the modal.
		/// @param topCustomIdNew A custom id to give for the modal.
		/// @param titleNew A title for the modal's individual input.
		/// @param customIdNew A custom id to give for the modal's individual input.
		/// @param required Is it a required response?
		/// @param minLength Minimum length.
		/// @param maxLength Maximum length.
		/// @param inputStyle The input style.
		/// @param label A label for the modal.
		/// @param placeholder A placeholder for the modal.
		/// @return RespondToInputEventData& A reference to this data structure.
		ExecuteWebHookData& addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
			jsonifier::string_view customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, jsonifier::string_view label = "",
			jsonifier::string_view placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile The file to be added.
		/// @return MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& addFile(File theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage An AllowedMentionsData structure.
		/// @return MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& addAllowedMentions(AllowedMentionsData dataPackage);

		/// @brief For setting the components in a response.
		/// @param dataPackage An ActionRowData structure.
		/// @return MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& addComponentRow(ActionRowData dataPackage);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage An EmbedData structure.
		/// @return MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& addMessageEmbed(EmbedData dataPackage);

		/// @brief For setting the content in a response.
		/// @param dataPackage A string, containing the content.
		/// @return MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& addContent(jsonifier::string_view dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs A bool.
		/// @return MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& setTTSStatus(bool enabledTTs);

	  protected:
		jsonifier::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		jsonifier::vector<ActionRowData> components{};///< Array of message component the components to include with the message.
		AllowedMentionsData allowedMentions{};///< Allowed mention object.
		jsonifier::vector<EmbedData> embeds{};///< Array of up to 10 embed objects.
		jsonifier::vector<File> files{};///< File contents the contents of the file being sent.
		jsonifier::string webhookToken{};///< The WebHookData token you would like to execute.
		jsonifier::string avatarUrl{};///< Override the default avatar of the webhook.
		jsonifier::string userName{};///< Override the default username of the webhook.
		jsonifier::string customId{};///< Custom id for the modal.
		Snowflake webHookId{};///< The WebHookData you would like to execute.
		jsonifier::string content{};///< The message contents (up to 2000 characters)	one of content, file, embeds.
		jsonifier::string title{};///< Title for the modal.
		uint64_t flags{};///< Flags combined as a bitfield.
		bool tts{};///< True if this is a TTS message.
	};

	/// @brief For editing a WebHook MessageData.
	class DiscordCoreAPI_Dll EditWebHookData : public ExecuteWebHookData {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class EditInteractionResponseData;
		friend class EditFollowUpMessageData;
		friend class Interactions;
		friend class WebHooks;

		Snowflake messageId{};///< The Message Snowflake to collect.
		Snowflake threadId{};///< Send a message to the specified thread within a webhook's ChannelData. The thread will automatically be unarchived.
		bool wait{};///< Waits for server confirmation of message send before response, and returns the created message body.

		ExecuteWebHookData& setTTSStatus(bool) = delete;

		EditWebHookData() = default;

		EditWebHookData(WebHookData dataNew);
	};

	/// @brief For collecting a list of WebHooks from a chosen ChannelData.
	struct GetChannelWebHooksData {
		Snowflake channelId{};///< The ChannelData from which to collect the WebHooks.
	};

	/// @brief For collecting a list of WebHooks from a chosen Guild.
	struct GetGuildWebHooksData {
		Snowflake guildId{};///< The Guild from which to collect the WebHooks.
	};

	/// @brief Collects a single WebHookData.
	struct GetWebHookData {
		Snowflake webHookId{};///< Snowflake of the desired WebHookData to collect.
	};

	/// @brief Collects a single WebHookData, using the Token and Snowflake.
	struct GetWebHookDataWithTokenData {
		jsonifier::string webhookToken{};///< Token of the desired WebHookData.
		Snowflake webHookId{};///< Snowflake of the desired WebHookData.
	};

	/// @brief For modifying a WebHook.
	struct ModifyWebHookData {
		Snowflake channelId{};///< The new ChannelData id this webhook should be moved to.
		Snowflake webHookId{};///< The WebHookData to be modified.
		jsonifier::string avatar{};///< Image responseData for the default webhook avatar.
		jsonifier::string name{};///< The default name of the webhook.
	};

	/// @brief For modifying a WebHook.
	struct ModifyWebHookDataWithTokenData {
		jsonifier::string webhookToken{};///< Token of the desired WebHookData.
		Snowflake channelId{};///< The new ChannelData id this webhook should be moved to.
		Snowflake webHookId{};///< The WebHookData to be modified.
		jsonifier::string avatar{};///< Image responseData for the default webhook avatar.
		jsonifier::string name{};///< The default name of the webhook.
	};

	/// @brief For deleting a WebHook.
	struct DeleteWebHookData {
		Snowflake webHookId{};///< The desired WebHookData to delete.
	};

	/// @brief For deleting a WebHook, using its Token.
	struct DeleteWebHookDataWithTokenData {
		jsonifier::string webhookToken{};///< Token of the desired WebHookData.
		Snowflake webHookId{};///< The desired WebHookData to delete.
	};

	/// @brief For getting a WebHook MessageData.
	struct GetWebHookDataMessageData {
		jsonifier::string webhookToken{};///< The WebHookData token you would like to collect.
		Snowflake webHookId{};///< The WebHookData you would like to collect.
		Snowflake messageId{};///< The Message Snowflake to collect.
		Snowflake threadId{};///< The thread that the Message is in.
	};

	/// @brief For deleting a WebHook MessageData.
	struct DeleteWebHookDataMessageData {
		jsonifier::string webhookToken{};///< The WebHookData token you would like to collect.
		Snowflake webHookId{};///< The WebHookData you would like to collect.
		Snowflake messageId{};///< The Message Snowflake to collect.
		Snowflake threadId{};///< Send a message to the specified thread within a webhook's ChannelData. The thread will automatically be unarchived.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the WebHookData related Discord endpoints;
	class DiscordCoreAPI_Dll WebHooks {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Creates a new WebHookData.
		/// @param dataPackage A GetMessageData structure.
		/// @return A CoRoutine containing a WebHook.
		static CoRoutine<WebHookData> createWebHookDataAsync(CreateWebHookData dataPackage);

		/// @brief Collects a list of WebHooks from a chosen ChannelData.
		/// @param dataPackage A GetChannelWebHooksData structure.
		/// @return A CoRoutine containing a jsonifier::vector<WebHookData>.
		static CoRoutine<jsonifier::vector<WebHookData>> getChannelWebHooksAsync(GetChannelWebHooksData dataPackage);

		/// @brief Collects a list of WebHooks from a chosen Guild.
		/// @param dataPackage A GetGuildWebHooksData structure.
		/// @return A CoRoutine containing a jsonifier::vector<WebHookData>.
		static CoRoutine<jsonifier::vector<WebHookData>> getGuildWebHooksAsync(GetGuildWebHooksData dataPackage);

		/// @brief Collects a single WebHookData.
		/// @param dataPackage A GetWebHookData structure.
		/// @return A CoRoutine containing a WebHook.
		static CoRoutine<WebHookData> getWebHookDataAsync(GetWebHookData dataPackage);

		/// @brief Collects a single WebHookData, using the Token.
		/// @param dataPackage A GetWebHookDataWithTokenData structure.
		/// @return A CoRoutine containing a WebHook.
		static CoRoutine<WebHookData> getWebHookDataWithTokenAsync(GetWebHookDataWithTokenData dataPackage);

		/// @brief Modifies a single WebHookData.
		/// @param dataPackage A ModifyWebHookData structure.
		/// @return A CoRoutine containing a WebHook.
		static CoRoutine<WebHookData> modifyWebHookDataAsync(ModifyWebHookData dataPackage);

		/// @brief Modifies a single WebHookData, using its Token.
		/// @param dataPackage A ModifyWebHookDataWithTokenData structure.
		/// @return A CoRoutine containing a WebHook.
		static CoRoutine<WebHookData> modifyWebHookDataWithTokenAsync(ModifyWebHookDataWithTokenData dataPackage);

		/// @brief Deletes a single WebHookData.
		/// @param dataPackage A DeleteWebHookData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteWebHookDataAsync(DeleteWebHookData dataPackage);

		/// @brief Deletes a single WebHookData, using its Token.
		/// @param dataPackage A DeleteWebHookDataWithTokenData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteWebHookDataWithTokenAsync(DeleteWebHookDataWithTokenData dataPackage);

		/// @brief Executes a single WebHookData.
		/// @param dataPackage An ExecuteWebHookData structure.
		/// @return A CoRoutine containing a Message.
		static CoRoutine<MessageData> executeWebHookDataAsync(ExecuteWebHookData dataPackage);

		/// @brief Collects a WebHook MessageData.
		/// @param dataPackage An GetWebHookDataMessageData structure.
		/// @return A CoRoutine containing a Message.
		static CoRoutine<MessageData> getWebHookDataMessageAsync(GetWebHookDataMessageData dataPackage);

		/// @brief Edits a WebHook MessageData.
		/// @param dataPackage An EditWebHookDataMessageData structure.
		/// @return A CoRoutine containing a Message.
		static CoRoutine<MessageData> editWebHookDataMessageAsync(EditWebHookData dataPackage);

		/// @brief Deletes a WebHook MessageData.
		/// @param dataPackage An DeleteWebHookDataMessageData structure.
		/// @return A CoRoutine containing a Message.
		static CoRoutine<void> deleteWebHookDataMessageAsync(DeleteWebHookDataMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/

}
