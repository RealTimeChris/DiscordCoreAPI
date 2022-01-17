// WebHookEntities.hpp - The header file for WebHook relate stuff.
// Dec 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "JSONIfier.hpp"
#include "MessageEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For creating a new WebHook. \brief For creating a new WebHook.
	struct DiscordCoreAPI_Dll CreateWebHookData{
		vector<uint8_t>avatar{};///< Image for the default webhook avatar.
		string channelId{ "" };///< The Channel within which to create the WebHook.
		string name{ "" };///< Name of the webhook(1 - 80 characters).
	};

	/// For collecting a list of WebHooks from a chosen Channel. \brief For collecting a list of WebHooks from a chosen Channel.
	struct DiscordCoreAPI_Dll GetChannelWebHooksData {
		string channelId{ "" };///< The Channel from which to collect the WebHooks.
	};

	/// For collecting a list of WebHooks from a chosen Guild. \brief For collecting a list of WebHooks from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildWebHooksData {
		string guildId{ "" };///< The Guild from which to collect the WebHooks.
	};

	/// Collects a single WebHook. \brief Collects a single WebHook.
	struct DiscordCoreAPI_Dll GetWebHookData {
		string webhookId{ "" };///< Id of the desired WebHook to collect.
	};

	/// Collects a single WebHook, using the Token and Id. \brief Collects a single WebHook, using the Token and Id.
	struct DiscordCoreAPI_Dll GetWebHookWithTokenData {
		string webhookToken{ "" };///< Token of the desired WebHook.
		string webhookId{ "" };///< Id of the desired WebHook.
	};
		
	/// For modifying a WebHook. \brief For modifying a WebHook.
	struct DiscordCoreAPI_Dll ModifyWebHookData {
		vector<uint8_t> avatar{};///< Image responseData for the default webhook avatar.
		string channelId{ "" };///< The new channel id this webhook should be moved to.
		string webhookId{ "" };///< The WebHook to be modified.
		string name{ "" };///< The default name of the webhook.
	};

	/// For modifying a WebHook. \brief For modifying a WebHook.
	struct DiscordCoreAPI_Dll ModifyWebHookWithTokenData {
		string webhookToken{ "" };///< Token of the desired WebHook.
		vector<uint8_t> avatar{};///< Image responseData for the default webhook avatar.
		string channelId{ "" };///< The new channel id this webhook should be moved to.
		string webhookId{ "" };///< The WebHook to be modified.
		string name{ "" };///< The default name of the webhook.
	};

	/// For deleting a WebHook. \brief For deleting a WebHook.
	struct DiscordCoreAPI_Dll DeleteWebHookData {
		string webhookId{ "" };///< The desired WebHook to delete.
	};

	/// For deleting a WebHook, using its Token. \brief For deleting a WebHook, using its Token.
	struct DiscordCoreAPI_Dll DeleteWebHookWithTokenData {
		string webhookToken{ "" };///< Token of the desired WebHook.
		string webhookId{ "" };///< The desired WebHook to delete.
	};

	/// For executing a WebHook. \brief For executing a WebHook.
	class DiscordCoreAPI_Dll ExecuteWebHookData {
	public:
		
		friend string DiscordCoreInternal::JSONIFY(ExecuteWebHookData dataPackage);
		friend class WebHooks;

		string threadId{ "" };///< Send a message to the specified thread within a webhook's channel. The thread will automatically be unarchived.
		bool wait{ false };///< Waits for server confirmation of message send before response, and returns the created message body(defaults to false; when false a message that is not saved does not return an error).

		ExecuteWebHookData(WebHookData dataNew) {
			this->webhookToken = dataNew.token;
			this->webhookId = dataNew.id;
		}

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customId A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.        
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		void addButton(bool disabled, string customId, string buttonLabel, ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customId A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int32_t maxValues, int32_t minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:

		AllowedMentionsData allowedMentions{};///< Allowed mentions for the message.
		vector<ActionRowData> components{};///< Array of message component	the components to include with the message.
		vector<EmbedData>embeds{};///< Array of up to 10 embed objects.
		string webhookToken{ "" };///< The WebHook token you would like to execute.
		string avatarUrl{ "" };///< The default avatar of the webhook.
		string webhookId{ "" };///< The WebHook you would like to execute.
		string username{ "" };///< The default username of the webhook.
		string content{ "" };///< The message contents (up to 2000 characters).
		bool tts{ false };///< True if this is a TTS message.
	};

	/// For getting a WebHook Message. \brief For getting a WebHook Message.
	struct DiscordCoreAPI_Dll GetWebHookMessageData {
		string webhookToken{ "" };///< The WebHook token you would like to collect.
		string webhookId{ "" };///< The WebHook you would like to collect.
		string messageId{ "" };///< The Message Id to collect.
		string threadId{ "" };///< The thread that the Message is in.
	};

	class DiscordCoreAPI_Dll EditWebHookMessageData {
	public:
		 
		friend string DiscordCoreInternal::JSONIFY(EditWebHookMessageData dataPackage);
		friend class WebHooks;

		string messageId{ "" };///< The Message Id to collect.
		string threadId{ "" };///< Send a message to the specified thread within a webhook's channel. The thread will automatically be unarchived.
		bool wait{ false };///< Waits for server confirmation of message send before response, and returns the created message body(defaults to false; when false a message that is not saved does not return an error).

		EditWebHookMessageData(WebHookData dataNew) {
			this->webhookToken = dataNew.token;
			this->webhookId = dataNew.id;
		}

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customId A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.        
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		void addButton(bool disabled, string customId, string buttonLabel, ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customId A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int32_t maxValues, int32_t minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

	protected:

		AllowedMentionsData allowedMentions{};///< Allowed mention object.
		vector<ActionRowData>components{};///< Array of message component.
		vector<EmbedData> embeds{};///< Array of up to 10 embed objects.
		string webhookToken{ "" };///< The WebHook token you would like to collect.
		string webhookId{ "" };///< The WebHook you would like to collect.
		string content{ "" };///< The message contents(up to 2000 characters).
	};

	/// For deleting a WebHook Message. \brief For deleting a WebHook Message.
	struct DiscordCoreAPI_Dll DeleteWebHookMessageData {
		string webhookToken{ "" };///< The WebHook token you would like to collect.
		string webhookId{ "" };///< The WebHook you would like to collect.
		string messageId{ "" };///< The Message Id to collect.
		string threadId{ "" };///< Send a message to the specified thread within a webhook's channel. The thread will automatically be unarchived.
	};

	/// A single WebHook. \brief A single WebHook.
	class DiscordCoreAPI_Dll WebHook : public WebHookData {
	public:
		WebHook();

		WebHook(WebHookData dataNew);
	};
	/**@}*/

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class for the WebHook related Discord endpoints. \brief An interface class for the WebHook related Discord endpoints;
	class DiscordCoreAPI_Dll WebHooks {
	public:

		static shared_ptr<DiscordCoreInternal::HttpClient> httpClient;
		static void initialize(shared_ptr<DiscordCoreInternal::HttpClient>);

		/// Creates a new WebHook. \brief Creates a new WebHook.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> createWebHookAsync(CreateWebHookData dataPackage);

		/// Collects a list of WebHooks from a chosen Channel. \brief Collects a list of WebHooks from a chosen Channel.
		/// \param dataPackage A GetChannelWebHooksData structure.
		/// \returns A CoRoutine containing a vector<WebHook>.
		static CoRoutine<vector<WebHook>> getChannelWebHooksAsync(GetChannelWebHooksData dataPackage);

		/// Collects a list of WebHooks from a chosen Guild. \brief Collects a list of WebHooks from a chosen Guild.
		/// \param dataPackage A GetGuildWebHooksData structure.
		/// \returns A CoRoutine containing a vector<WebHook>.
		static CoRoutine<vector<WebHook>> getGuildWebHooksAsync(GetGuildWebHooksData dataPackage);

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
		static CoRoutine<Message> editWebHookMessageAsync(EditWebHookMessageData dataPackage);

		/// Deletes a WebHook Message. \brief Deletes a WebHook Message.
		/// \param dataPackage An DeleteWebHookMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<void> deleteWebHookMessageAsync(DeleteWebHookMessageData dataPackage);

	};
	/**@}*/
	
}
