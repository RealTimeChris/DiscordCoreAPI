/*
*
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
/// WebHookEntities.hpp - The header file for WebHook relate stuff.
/// Dec 1, 2021
/// https://discordcoreapi.com
/// \file WebHookEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/JSONIfier.hpp>
#include <discordcoreapi/MessageEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For creating a new WebHook. \brief For creating a new WebHook.
	struct DiscordCoreAPI_Dll CreateWebHookData {
		std::vector<uint8_t> avatar{};///< Image for the default webhook avatar.
		Snowflake channelId{};///< The Channel within which to create the WebHook.
		std::string name{};///< Name of the webhook(1 - 80 characters).
	};

	/// For executing a WebHook. \brief For executing a WebHook.
	class DiscordCoreAPI_Dll ExecuteWebHookData {
	  public:
		friend class CreateEphemeralFollowUpMessageData;
		friend class CreateFollowUpMessageData;
		friend class EditFollowUpMessageData;
		friend class Interactions;
		friend WebHooks;

		Snowflake threadId{};///< Send a message to the specified thread within a webhook's Channel. The thread will automatically be unarchived.
		bool wait{ false };///< Waits for server confirmation of message send before response, and returns the created message body.

		ExecuteWebHookData() = default;

		ExecuteWebHookData(WebHookData dataNew);

		operator std::string() {
			nlohmann::json data{};
			for (auto& value: this->attachments) {
				data["attachments"].push_back(value);
			}
			if (this->components.size() == 0) {
				data["components"] = nlohmann::json::array();
			} else {
				for (auto& value: this->components) {
					DiscordCoreAPI::ActionRowData theData{ value };
					data["components"].push_back(nlohmann::json{ theData });
				}
			}
			data["allowed_mentions"] = this->allowedMentions;
			if (this->embeds.size() == 0) {
				data["embeds"] = nlohmann::json::array();
			} else {
				for (auto& value: this->embeds) {
					data["embeds"].push_back(value);
				}
			}
			if (this->avatarUrl != "") {
				data["avatar_url"] = this->userName;
			}
			if (this->userName != "") {
				data["userName"] = this->userName;
			}
			if (this->content != "") {
				data["content"] = this->content;
			}
			data["flags"] = this->flags;
			data["tts"] = this->tts;
			return data.dump();
		}

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		ExecuteWebHookData& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle, const std::string& emojiName = "",
			Snowflake emojiId = 0, const std::string& url = "");

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A std::vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		ExecuteWebHookData addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues,
			int32_t minValues);

		/// Adds a modal to the response Message. \brief Adds a modal to the response Message.
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
		ExecuteWebHookData& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew, bool required,
			int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label = "", const std::string& placeholder = "");

		/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns MessageResponseBase& A reference to this data structure.
		ExecuteWebHookData& addFile(File theFile);

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		ExecuteWebHookData& addAllowedMentions(AllowedMentionsData dataPackage);

		/// For setting the components in a response. \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		ExecuteWebHookData& addComponentRow(ActionRowData dataPackage);

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		ExecuteWebHookData& addMessageEmbed(EmbedData dataPackage);

		/// For setting the Message content in a response. \brief For setting the content in a response.
		/// \param dataPackage A std::string, containing the content.
		ExecuteWebHookData& addContent(const std::string& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
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
		Snowflake webHookId{};///< The WebHook you would like to execute.
		std::string userName{};///< Override the default userName of the webhook.
		std::string customId{};///< Custom id for the modal.
		std::string content{};///< The message contents (up to 2000 characters)	one of content, file, embeds.
		std::string title{};///< Title for the modal.
		int32_t flags{ 0 };///< Flags combined as a bitfield.
		bool tts{ false };///< True if this is a TTS message.
	};

	/// For editing a WebHook Message. \brief For editing a WebHook Message.
	class DiscordCoreAPI_Dll EditWebHookData : public ExecuteWebHookData {
	  public:
		friend class EditInteractionResponseData;
		friend class EditFollowUpMessageData;
		friend class Interactions;
		friend WebHooks;

		Snowflake messageId{};///< The Message Id to collect.
		Snowflake threadId{};///< Send a message to the specified thread within a webhook's Channel. The thread will automatically be unarchived.
		bool wait{ false };///< Waits for server confirmation of message send before response, and returns the created message body.

		ExecuteWebHookData& setTTSStatus(bool) = delete;

		EditWebHookData() = default;

		EditWebHookData(WebHookData dataNew);

		operator std::string() {
			nlohmann::json data{};
			for (auto& value: this->attachments) {
				data["attachments"].push_back(DiscordCoreAPI::AttachmentData{ value });
			}
			if (this->components.size() == 0) {
				data["components"] = nlohmann::json::array();
			} else {
				for (auto& value: this->components) {
					DiscordCoreAPI::ActionRowData theData{ value };
					data["components"].push_back(nlohmann::json{ theData });
				}
			}
			data["allowed_mentions"] = DiscordCoreAPI::AllowedMentionsData{ this->allowedMentions };
			if (this->embeds.size() == 0) {
				data["embeds"] = nlohmann::json::array();
			} else {
				for (auto& value: this->embeds) {
					data["embeds"].push_back(DiscordCoreAPI::EmbedData{ value });
				}
			}
			if (this->content != "") {
				data["content"] = this->content;
			}
			return data.dump();
		}
	};

	/// For collecting a list of WebHooks from a chosen Channel. \brief For collecting a list of WebHooks from a chosen Channel.
	struct DiscordCoreAPI_Dll GetChannelWebHooksData {
		Snowflake channelId{};///< The Channel from which to collect the WebHooks.
	};

	/// For collecting a list of WebHooks from a chosen Guild. \brief For collecting a list of WebHooks from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildWebHooksData {
		Snowflake guildId{};///< The Guild from which to collect the WebHooks.
	};

	/// Collects a single WebHook. \brief Collects a single WebHook.
	struct DiscordCoreAPI_Dll GetWebHookData {
		Snowflake webHookId{};///< Id of the desired WebHook to collect.
	};

	/// Collects a single WebHook, using the Token and Id. \brief Collects a single WebHook, using the Token and Id.
	struct DiscordCoreAPI_Dll GetWebHookWithTokenData {
		std::string webhookToken{};///< Token of the desired WebHook.
		Snowflake webHookId{};///< Id of the desired WebHook.
	};

	/// For modifying a WebHook. \brief For modifying a WebHook.
	struct DiscordCoreAPI_Dll ModifyWebHookData {
		std::vector<uint8_t> avatar{};///< Image responseData for the default webhook avatar.
		Snowflake channelId{};///< The new Channel id this webhook should be moved to.
		Snowflake webHookId{};///< The WebHook to be modified.
		std::string name{};///< The default name of the webhook.
	};

	/// For modifying a WebHook. \brief For modifying a WebHook.
	struct DiscordCoreAPI_Dll ModifyWebHookWithTokenData {
		std::string webhookToken{};///< Token of the desired WebHook.
		std::vector<uint8_t> avatar{};///< Image responseData for the default webhook avatar.
		Snowflake channelId{};///< The new Channel id this webhook should be moved to.
		Snowflake webHookId{};///< The WebHook to be modified.
		std::string name{};///< The default name of the webhook.
	};

	/// For deleting a WebHook. \brief For deleting a WebHook.
	struct DiscordCoreAPI_Dll DeleteWebHookData {
		Snowflake webHookId{};///< The desired WebHook to delete.
	};

	/// For deleting a WebHook, using its Token. \brief For deleting a WebHook, using its Token.
	struct DiscordCoreAPI_Dll DeleteWebHookWithTokenData {
		std::string webhookToken{};///< Token of the desired WebHook.
		Snowflake webHookId{};///< The desired WebHook to delete.
	};

	/// For getting a WebHook Message. \brief For getting a WebHook Message.
	struct DiscordCoreAPI_Dll GetWebHookMessageData {
		std::string webhookToken{};///< The WebHook token you would like to collect.
		Snowflake webHookId{};///< The WebHook you would like to collect.
		Snowflake messageId{};///< The Message Id to collect.
		Snowflake threadId{};///< The thread that the Message is in.
	};

	/// For deleting a WebHook Message. \brief For deleting a WebHook Message.
	struct DiscordCoreAPI_Dll DeleteWebHookMessageData {
		std::string webhookToken{};///< The WebHook token you would like to collect.
		Snowflake webHookId{};///< The WebHook you would like to collect.
		Snowflake messageId{};///< The Message Id to collect.
		Snowflake threadId{};///< Send a message to the specified thread within a webhook's Channel. The thread will automatically be unarchived.
	};

	/// A single WebHook. \brief A single WebHook.
	class DiscordCoreAPI_Dll WebHook : public WebHookData, public DataParser<WebHook> {
	  public:
		WebHook() = default;

		WebHook& operator=(const nlohmann::json& jsonObjectData);

		WebHook(const nlohmann::json& jsonObjectData);

		virtual ~WebHook() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WebHook* pDataStructure);
	};

	class DiscordCoreAPI_Dll WebHookVector : public DataParser<WebHookVector> {
	  public:
		WebHookVector() = default;

		operator std::vector<WebHook>();

		WebHookVector& operator=(const nlohmann::json& jsonObjectData);

		WebHookVector(const nlohmann::json& jsonObjectData);

		virtual ~WebHookVector() = default;

	  protected:
		std::vector<WebHook> theWebHooks{};

		void parseObject(const nlohmann::json& jsonObjectData, WebHookVector* pDataStructure);
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the WebHook related Discord endpoints. \brief An interface class for the WebHook related Discord endpoints;
	class DiscordCoreAPI_Dll WebHooks {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// Creates a new WebHook. \brief Creates a new WebHook.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> createWebHookAsync(CreateWebHookData dataPackage);

		/// Collects a list of WebHooks from a chosen Channel. \brief Collects a list of WebHooks from a chosen Channel.
		/// \param dataPackage A GetChannelWebHooksData structure.
		/// \returns A CoRoutine containing a WebHookVector.
		static CoRoutine<std::vector<WebHook>> getChannelWebHooksAsync(GetChannelWebHooksData dataPackage);

		/// Collects a list of WebHooks from a chosen Guild. \brief Collects a list of WebHooks from a chosen Guild.
		/// \param dataPackage A GetGuildWebHooksData structure.
		/// \returns A CoRoutine containing a WebHookVector.
		static CoRoutine<std::vector<WebHook>> getGuildWebHooksAsync(GetGuildWebHooksData dataPackage);

		/// Collects a single WebHook. \brief Collects a single WebHook.
		/// \param dataPackage A GetWebHookData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> getWebHookAsync(GetWebHookData dataPackage);

		/// Collects a single WebHook, using the Token. \brief Collects a single WebHook, using the Token.
		/// \param dataPackage A GetWebHookWithTokenData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> getWebHookWithTokenAsync(GetWebHookWithTokenData dataPackage);

		/// Modifies a single WebHook. \brief Modifies a single WebHook.
		/// \param dataPackage A ModifyWebHookData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> modifyWebHookAsync(ModifyWebHookData dataPackage);

		/// Modifies a single WebHook, using its Token. \brief Modifies a single WebHook, using its Token.
		/// \param dataPackage A ModifyWebHookWithTokenData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> modifyWebHookWithTokenAsync(ModifyWebHookWithTokenData dataPackage);

		/// Deletes a single WebHook. \brief Deletes a single WebHook.
		/// \param dataPackage A DeleteWebHookData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteWebHookAsync(DeleteWebHookData dataPackage);

		/// Deletes a single WebHook, using its Token. \brief Deletes a single WebHook, using its Token.
		/// \param dataPackage A DeleteWebHookWithTokenData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteWebHookWithTokenAsync(DeleteWebHookWithTokenData dataPackage);

		/// Executes a single WebHook. \brief Executes a single WebHook.
		/// \param dataPackage An ExecuteWebHookData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> executeWebHookAsync(ExecuteWebHookData dataPackage);

		/// Collects a WebHook Message. \brief Collects a WebHook Message.
		/// \param dataPackage An GetWebHookMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> getWebHookMessageAsync(GetWebHookMessageData dataPackage);

		/// Edits a WebHook Message. \brief Edits a WebHook Message.
		/// \param dataPackage An EditWebHookMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> editWebHookMessageAsync(EditWebHookData dataPackage);

		/// Deletes a WebHook Message. \brief Deletes a WebHook Message.
		/// \param dataPackage An DeleteWebHookMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<void> deleteWebHookMessageAsync(DeleteWebHookMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/

}// namespace DiscordCoreAPI
