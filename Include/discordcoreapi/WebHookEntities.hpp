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
/// WebHookEntities.hpp - The header file for WebHook relate stuff.
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

	/// \brief For creating a new WebHook.
	struct DiscordCoreAPI_Dll CreateWebHookData {
		Snowflake channelId{};///< The Channel within which to create the WebHook.
		std::string avatar{};///< Image for the default webhook avatar.
		std::string name{};///< Name of the webhook(1 - 80 characters).
	};

	/// \brief For executing a WebHook.
	class DiscordCoreAPI_Dll ExecuteWebHookData {
	  public:
		friend class CreateEphemeralFollowUpMessageData;
		friend class CreateFollowUpMessageData;
		friend class EditFollowUpMessageData;
		friend class Interactions;
		friend class WebHooks;

		Snowflake threadId{};///< Send a message to the specified thread within a webhook's Channel. The thread will automatically be unarchived.
		bool wait{};///< Waits for server confirmation of message send before response, and returns the created message body.

		ExecuteWebHookData() noexcept = default;

		ExecuteWebHookData(WebHookData dataNew);

		/// \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		ExecuteWebHookData& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
			const std::string& emojiName = "", Snowflake emojiId = Snowflake{}, const std::string& url = "");

		/// \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \param type The type of select-menu that this is.
		/// \param channelTypes Types of channels that can be accepted if this is of the type ChannelType.
		/// \returns RespondToInputEventData& A reference to this data structure.
		ExecuteWebHookData& addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
			const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type,
			std::vector<ChannelType> channelTypes = std::vector<ChannelType>{});

		/// \brief Adds a modal to the response Message.
		/// \param topTitleNew A title for the modal.
		/// \param topCustomIdNew A custom id to give for the modal.
		/// \param titleNew A title for the modal's individual input.
		/// \param customIdNew A custom id to give for the modal's individual input.
		/// \param required Is it a required response?
		/// \param minLength Minimum length.
		/// \param maxLength Maximum length.
		/// \param inputStyle The input style.
		/// \param label A label for the modal.
		/// \param placeholder A placeholder for the modal.
		/// \returns RespondToInputEventData& A reference to this data structure.
		ExecuteWebHookData& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
			const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle,
			const std::string& label = "", const std::string& placeholder = "");

		/// \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& addFile(File theFile);

		/// \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		ExecuteWebHookData& addAllowedMentions(AllowedMentionsData dataPackage);

		/// \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		ExecuteWebHookData& addComponentRow(ActionRowData dataPackage);

		/// \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		ExecuteWebHookData& addMessageEmbed(EmbedData dataPackage);

		/// \brief For setting the content in a response.
		/// \param dataPackage A string, containing the content.
		ExecuteWebHookData& addContent(const std::string& dataPackage);

		/// \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		ExecuteWebHookData& setTTSStatus(bool enabledTTs);

	  protected:
		std::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		std::vector<ActionRowData> components{};///< Array of message component the components to include with the message.
		AllowedMentionsData allowedMentions{};///< Allowed mention object.
		std::vector<EmbedData> embeds{};///< Array of up to 10 embed objects.
		std::string webhookToken{};///< The WebHook token you would like to execute.
		std::vector<File> files{};///< File contents the contents of the file being sent.
		std::string avatarUrl{};///< Override the default avatar of the webhook.
		std::string userName{};///< Override the default userName of the webhook.
		std::string customId{};///< Custom id for the modal.
		Snowflake webHookId{};///< The WebHook you would like to execute.
		std::string content{};///< The message contents (up to 2000 characters)	one of content, file, embeds.
		std::string title{};///< Title for the modal.
		int32_t flags{};///< Flags combined as a bitfield.
		bool tts{};///< True if this is a TTS message.
	};

	/// \brief For editing a WebHook Message.
	class DiscordCoreAPI_Dll EditWebHookData : public ExecuteWebHookData {
	  public:
		friend class EditInteractionResponseData;
		friend class EditFollowUpMessageData;
		friend class Interactions;
		friend class WebHooks;

		Snowflake messageId{};///< The Message Id to collect.
		Snowflake threadId{};///< Send a message to the specified thread within a webhook's Channel. The thread will automatically be unarchived.
		bool wait{};///< Waits for server confirmation of message send before response, and returns the created message body.

		ExecuteWebHookData& setTTSStatus(bool) = delete;

		EditWebHookData() noexcept = default;

		EditWebHookData(WebHookData dataNew);
	};

	/// \brief For collecting a list of WebHooks from a chosen Channel.
	struct DiscordCoreAPI_Dll GetChannelWebHooksData {
		Snowflake channelId{};///< The Channel from which to collect the WebHooks.
	};

	/// \brief For collecting a list of WebHooks from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildWebHooksData {
		Snowflake guildId{};///< The Guild from which to collect the WebHooks.
	};

	/// \brief Collects a single WebHook.
	struct DiscordCoreAPI_Dll GetWebHookData {
		Snowflake webHookId{};///< Id of the desired WebHook to collect.
	};

	/// \brief Collects a single WebHook, using the Token and Id.
	struct DiscordCoreAPI_Dll GetWebHookWithTokenData {
		std::string webhookToken{};///< Token of the desired WebHook.
		Snowflake webHookId{};///< Id of the desired WebHook.
	};

	/// \brief For modifying a WebHook.
	struct DiscordCoreAPI_Dll ModifyWebHookData {
		Snowflake channelId{};///< The new Channel id this webhook should be moved to.
		Snowflake webHookId{};///< The WebHook to be modified.
		std::string avatar{};///< Image responseData for the default webhook avatar.
		std::string name{};///< The default name of the webhook.
	};

	/// \brief For modifying a WebHook.
	struct DiscordCoreAPI_Dll ModifyWebHookWithTokenData {
		std::string webhookToken{};///< Token of the desired WebHook.
		Snowflake channelId{};///< The new Channel id this webhook should be moved to.
		Snowflake webHookId{};///< The WebHook to be modified.
		std::string avatar{};///< Image responseData for the default webhook avatar.
		std::string name{};///< The default name of the webhook.
	};

	/// \brief For deleting a WebHook.
	struct DiscordCoreAPI_Dll DeleteWebHookData {
		Snowflake webHookId{};///< The desired WebHook to delete.
	};

	/// \brief For deleting a WebHook, using its Token.
	struct DiscordCoreAPI_Dll DeleteWebHookWithTokenData {
		std::string webhookToken{};///< Token of the desired WebHook.
		Snowflake webHookId{};///< The desired WebHook to delete.
	};

	/// \brief For getting a WebHook Message.
	struct DiscordCoreAPI_Dll GetWebHookMessageData {
		std::string webhookToken{};///< The WebHook token you would like to collect.
		Snowflake webHookId{};///< The WebHook you would like to collect.
		Snowflake messageId{};///< The Message Id to collect.
		Snowflake threadId{};///< The thread that the Message is in.
	};

	/// \brief For deleting a WebHook Message.
	struct DiscordCoreAPI_Dll DeleteWebHookMessageData {
		std::string webhookToken{};///< The WebHook token you would like to collect.
		Snowflake webHookId{};///< The WebHook you would like to collect.
		Snowflake messageId{};///< The Message Id to collect.
		Snowflake threadId{};///< Send a message to the specified thread within a webhook's Channel. The thread will automatically be unarchived.
	};

	/// \brief A single WebHook.
	class DiscordCoreAPI_Dll WebHook : public WebHookData {
	  public:
		WebHook() noexcept = default;

		virtual ~WebHook() noexcept = default;
	};

	using WebHookVector = std::vector<WebHook>;

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the WebHook related Discord endpoints;
	class DiscordCoreAPI_Dll WebHooks {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Creates a new WebHook.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> createWebHookAsync(CreateWebHookData dataPackage);

		/// \brief Collects a list of WebHooks from a chosen Channel.
		/// \param dataPackage A GetChannelWebHooksData structure.
		/// \returns A CoRoutine containing a WebHookVector.
		static CoRoutine<std::vector<WebHook>> getChannelWebHooksAsync(GetChannelWebHooksData dataPackage);

		/// \brief Collects a list of WebHooks from a chosen Guild.
		/// \param dataPackage A GetGuildWebHooksData structure.
		/// \returns A CoRoutine containing a WebHookVector.
		static CoRoutine<std::vector<WebHook>> getGuildWebHooksAsync(GetGuildWebHooksData dataPackage);

		/// \brief Collects a single WebHook.
		/// \param dataPackage A GetWebHookData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> getWebHookAsync(GetWebHookData dataPackage);

		/// \brief Collects a single WebHook, using the Token.
		/// \param dataPackage A GetWebHookWithTokenData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> getWebHookWithTokenAsync(GetWebHookWithTokenData dataPackage);

		/// \brief Modifies a single WebHook.
		/// \param dataPackage A ModifyWebHookData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> modifyWebHookAsync(ModifyWebHookData dataPackage);

		/// \brief Modifies a single WebHook, using its Token.
		/// \param dataPackage A ModifyWebHookWithTokenData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> modifyWebHookWithTokenAsync(ModifyWebHookWithTokenData dataPackage);

		/// \brief Deletes a single WebHook.
		/// \param dataPackage A DeleteWebHookData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteWebHookAsync(DeleteWebHookData dataPackage);

		/// \brief Deletes a single WebHook, using its Token.
		/// \param dataPackage A DeleteWebHookWithTokenData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteWebHookWithTokenAsync(DeleteWebHookWithTokenData dataPackage);

		/// \brief Executes a single WebHook.
		/// \param dataPackage An ExecuteWebHookData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> executeWebHookAsync(ExecuteWebHookData dataPackage);

		/// \brief Collects a WebHook Message.
		/// \param dataPackage An GetWebHookMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> getWebHookMessageAsync(GetWebHookMessageData dataPackage);

		/// \brief Edits a WebHook Message.
		/// \param dataPackage An EditWebHookMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> editWebHookMessageAsync(EditWebHookData dataPackage);

		/// \brief Deletes a WebHook Message.
		/// \param dataPackage An DeleteWebHookMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<void> deleteWebHookMessageAsync(DeleteWebHookMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/

}