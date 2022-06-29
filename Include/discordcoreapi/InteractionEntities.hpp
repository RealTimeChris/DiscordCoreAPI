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
/// InteractionEntities.hpp - Header for the interaction related classes and
/// structs. May 28, 2021 Chris M.
/// https://discordcoreapi.com
/// \file InteractionEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/JSONIfier.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/WebHookEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	using AutoCompleteEntryFunction = std::function<std::string(std::string)>;

	class DiscordCoreAPI_Dll InteractionResponse {
	  public:
		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		InteractionResponse& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle, const std::string& emojiName = "",
			Snowflake emojiId = 0, const std::string& url = "") {
			if (this->data.data.components.size() == 0) {
				ActionRowData actionRowData;
				this->data.data.components.push_back(actionRowData);
			}
			if (this->data.data.components.size() < 5) {
				if (this->data.data.components[this->data.data.components.size() - 1].components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = static_cast<int32_t>(buttonStyle);
					component.customId = customIdNew;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->data.data.components[this->data.data.components.size() - 1].components.push_back(component);
				} else if (this->data.data.components[this->data.data.components.size() - 1].components.size() == 5) {
					ActionRowData actionRowData;
					this->data.data.components.push_back(actionRowData);
				}
			}
			return *this;
		}

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A std::vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		InteractionResponse& addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues,
			int32_t minValues) {
			if (this->data.data.components.size() == 0) {
				ActionRowData actionRowData;
				this->data.data.components.push_back(actionRowData);
			}
			if (this->data.data.components.size() < 5) {
				if (this->data.data.components[this->data.data.components.size() - 1].components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customIdNew;
					componentData.options = options;
					this->data.data.components[this->data.data.components.size() - 1].components.push_back(componentData);
				} else if (this->data.data.components[this->data.data.components.size() - 1].components.size() == 5) {
					ActionRowData actionRowData;
					this->data.data.components.push_back(actionRowData);
				}
			}
			return *this;
		}

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
		InteractionResponse& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew, bool required,
			int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label = "", const std::string& placeholder = "") {
			this->data.data.title = topTitleNew;
			this->data.data.customId = topCustomIdNew;
			if (this->data.data.components.size() == 0) {
				ActionRowData actionRowData;
				this->data.data.components.push_back(actionRowData);
			}
			if (this->data.data.components.size() < 5) {
				if (this->data.data.components[this->data.data.components.size() - 1].components.size() < 5) {
					ComponentData component{};
					component.type = ComponentType::TextInput;
					component.customId = customIdNew;
					component.style = static_cast<int32_t>(inputStyle);
					component.title = titleNew;
					component.maxLength = maxLength;
					component.minLength = minLength;
					component.label = label;
					component.required = required;
					component.placeholder = placeholder;
					this->data.data.components[this->data.data.components.size() - 1].components.push_back(component);
				} else if (this->data.data.components[this->data.data.components.size() - 1].components.size() == 5) {
					ActionRowData actionRowData;
					this->data.data.components.push_back(actionRowData);
				}
			}
			return *this;
		}

		/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns MessageResponseBase& A reference to this data structure.
		InteractionResponse& addFile(const File& theFile) {
			this->data.data.files.push_back(theFile);
			return *this;
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		InteractionResponse& addAllowedMentions(const AllowedMentionsData& dataPackage) {
			this->data.data.allowedMentions = dataPackage;
			return *this;
		}

		/// For setting the components in a response. \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		InteractionResponse& addComponentRow(const ActionRowData& dataPackage) {
			this->data.data.components.push_back(dataPackage);
			return *this;
		}

		/// Sets the response type of the current Message. \brief Sets the response type of the current Message.
		/// \param type Interaction callback type.
		InteractionResponse& setResponseType(InteractionCallbackType type) {
			this->data.type = type;
			return *this;
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		InteractionResponse& addMessageEmbed(const EmbedData& dataPackage) {
			this->data.data.embeds.push_back(dataPackage);
			return *this;
		}

		/// For setting the Message content in a response. \brief For setting the content in a response.
		/// \param dataPackage A std::string, containing the content.
		InteractionResponse& addContent(const std::string& dataPackage) {
			this->data.data.content = dataPackage;
			return *this;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		InteractionResponse& setTTSStatus(bool enabledTTs) {
			this->data.data.tts = enabledTTs;
			return *this;
		}

		virtual ~InteractionResponse() = default;

		InteractionResponseData getInteractionResponseData() {
			return this->data;
		}

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		InteractionResponseData data{};
	};

	/// For creating an ephemeral Interaction response. \brief For creating an ephemeral Interaction response.
	class DiscordCoreAPI_Dll CreateEphemeralInteractionResponseData : public InteractionResponse {
	  public:
		friend CreateInteractionResponseData;
		friend Interactions;
		friend InputEvents;

		CreateEphemeralInteractionResponseData(const RespondToInputEventData& dataPackage) {
			this->data = dataPackage;
			if (dataPackage.eventType == InteractionType::Message_Component) {
				this->data.type = InteractionCallbackType::Update_Message;
			} else {
				this->data.type = InteractionCallbackType::Channel_Message_With_Source;
			}
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->data.data.flags = 64;
		}

		CreateEphemeralInteractionResponseData(RespondToInputEventData& dataPackage) {
			this->data = dataPackage;
			if (dataPackage.eventType == InteractionType::Message_Component) {
				this->data.type = InteractionCallbackType::Update_Message;
			} else {
				this->data.type = InteractionCallbackType::Channel_Message_With_Source;
			}
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->data.data.flags = 64;
		}

		virtual ~CreateEphemeralInteractionResponseData() = default;
	};

	/// For creating a deferred Interaction response. \brief For creating a deferred Interaction response.
	class DiscordCoreAPI_Dll CreateDeferredInteractionResponseData {
	  public:
		friend CreateInteractionResponseData;
		friend Interactions;
		friend InputEvents;

		CreateDeferredInteractionResponseData(RespondToInputEventData& dataPackage) {
			this->data = dataPackage;
			if (dataPackage.eventType == InteractionType::Message_Component) {
				this->data.type = InteractionCallbackType::Deferred_Update_Message;
			} else {
				this->data.type = InteractionCallbackType::Deferred_Channel_Message_With_Source;
			}
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
		}

		CreateDeferredInteractionResponseData(const RespondToInputEventData& dataPackage) {
			this->data = dataPackage;
			if (dataPackage.eventType == InteractionType::Message_Component) {
				this->data.type = InteractionCallbackType::Deferred_Update_Message;
			} else {
				this->data.type = InteractionCallbackType::Deferred_Channel_Message_With_Source;
			}
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
		}

		virtual ~CreateDeferredInteractionResponseData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
		InteractionResponseData data{};
	};

	/// For creating an Interaction response. \brief For creating an Interaction response.
	class DiscordCoreAPI_Dll CreateInteractionResponseData : public InteractionResponse {
	  public:
		friend SelectMenuCollector;
		friend ButtonCollector;
		friend ModalCollector;
		friend Interactions;
		friend InputEvents;

		CreateInteractionResponseData(const CreateDeferredInteractionResponseData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			this->data.data.components = dataPackage.data.data.components;
			this->data.type = dataPackage.data.type;
			this->data = dataPackage.data;
		}

		CreateInteractionResponseData(CreateDeferredInteractionResponseData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			this->data.data.components = dataPackage.data.data.components;
			this->data.type = dataPackage.data.type;
			this->data = dataPackage.data;
		}

		CreateInteractionResponseData(const CreateEphemeralInteractionResponseData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			this->data.data.components = dataPackage.data.data.components;
			this->data.type = dataPackage.data.type;
			this->data = dataPackage.data;
			this->data.data.flags = 64;
		}

		CreateInteractionResponseData(CreateEphemeralInteractionResponseData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
			this->data.data.components = dataPackage.data.data.components;
			this->data.type = dataPackage.data.type;
			this->data = dataPackage.data;
			this->data.data.flags = 64;
		}

		CreateInteractionResponseData(RespondToInputEventData& dataPackage) {
			this->data = dataPackage;
			if (dataPackage.eventType == InteractionType::Message_Component && dataPackage.type == InputEventResponseType::Deferred_Response) {
				this->data.type = InteractionCallbackType::Deferred_Update_Message;
			} else if (dataPackage.eventType == InteractionType::Message_Component) {
				this->data.type = InteractionCallbackType::Update_Message;
			} else if (dataPackage.eventType == InteractionType::Application_Command_Autocomplete ||
				dataPackage.type == InputEventResponseType::Application_Command_AutoComplete_Result) {
				this->data.type = InteractionCallbackType::Application_Command_Autocomplete_Result;
			} else {
				this->data.type = InteractionCallbackType::Channel_Message_With_Source;
			}
			if (dataPackage.type == InputEventResponseType::Modal_Interaction_Response || dataPackage.title != "") {
				this->data.type = InteractionCallbackType::Modal;
			}
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
		}

		CreateInteractionResponseData(InteractionData& dataPackage) {
			if (dataPackage.type == InteractionType::Message_Component) {
				this->data.type = InteractionCallbackType::Update_Message;
			} else {
				this->data.type = InteractionCallbackType::Channel_Message_With_Source;
			}
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionToken = dataPackage.token;
			this->interactionPackage.interactionId = dataPackage.id;
		}

		virtual ~CreateInteractionResponseData() = default;
	};

	/// For getting an Interaction response. \brief For getting an Interaction response.
	struct DiscordCoreAPI_Dll GetInteractionResponseData {
		std::string interactionToken{};///< Interaction token.
		Snowflake applicationId{};///< application id.
	};

	/// For editing an Interaction response. \brief For editing an Interaction response.
	class DiscordCoreAPI_Dll EditInteractionResponseData {
	  public:
		friend Interactions;
		friend InputEvents;

		EditInteractionResponseData(const RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->data.allowedMentions = dataPackage.allowedMentions;
			this->data.components = dataPackage.components;
			this->data.content = dataPackage.content;
			this->data.embeds = dataPackage.embeds;
			this->data.title = dataPackage.title;
			this->data.flags = dataPackage.flags;
			this->data.tts = dataPackage.tts;
		}

		EditInteractionResponseData(RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->data.allowedMentions = dataPackage.allowedMentions;
			this->data.components = dataPackage.components;
			this->data.content = dataPackage.content;
			this->data.embeds = dataPackage.embeds;
			this->data.title = dataPackage.title;
			this->data.flags = dataPackage.flags;
			this->data.tts = dataPackage.tts;
		}

		virtual ~EditInteractionResponseData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
		EditWebHookData data{};
	};

	/// For deleting an Interaction response. \brief For deleting an Interaction response.
	struct DiscordCoreAPI_Dll DeleteInteractionResponseData {
		friend Interactions;
		friend InputEvents;

		DeleteInteractionResponseData(RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
		}

	  protected:
		InteractionPackageData interactionPackage{};
		uint32_t timeDelay{ 0 };
	};

	/// For creating an ephemeral follow up Message. \brief For creating an ephemeral follow up Message.
	class DiscordCoreAPI_Dll CreateEphemeralFollowUpMessageData : public ExecuteWebHookData {
	  public:
		friend CreateFollowUpMessageData;
		friend Interactions;
		friend InputEvents;

		CreateEphemeralFollowUpMessageData(const RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->allowedMentions = dataPackage.allowedMentions;
			this->components = dataPackage.components;
			this->content = dataPackage.content;
			this->embeds = dataPackage.embeds;
			this->files = dataPackage.files;
			this->tts = dataPackage.tts;
			this->flags = 64;
		}

		CreateEphemeralFollowUpMessageData(RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->allowedMentions = dataPackage.allowedMentions;
			this->components = dataPackage.components;
			this->content = dataPackage.content;
			this->embeds = dataPackage.embeds;
			this->files = dataPackage.files;
			this->tts = dataPackage.tts;
			this->flags = 64;
		}

		virtual ~CreateEphemeralFollowUpMessageData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
	};

	/// For creating a follow up Message. \brief For creating a follow up Message.
	class DiscordCoreAPI_Dll CreateFollowUpMessageData : public ExecuteWebHookData {
	  public:
		friend SelectMenuCollector;
		friend ButtonCollector;
		friend Interactions;
		friend InputEvents;

		CreateFollowUpMessageData(const CreateEphemeralFollowUpMessageData& dataPackage) {
			this->interactionPackage = dataPackage.interactionPackage;
			this->allowedMentions = dataPackage.allowedMentions;
			this->components = dataPackage.components;
			this->content = dataPackage.content;
			this->embeds = dataPackage.embeds;
			this->flags = dataPackage.flags;
			this->files = dataPackage.files;
			this->tts = dataPackage.tts;
			this->flags = 64;
		}

		CreateFollowUpMessageData(CreateEphemeralFollowUpMessageData& dataPackage) {
			this->interactionPackage = dataPackage.interactionPackage;
			this->allowedMentions = dataPackage.allowedMentions;
			this->components = dataPackage.components;
			this->content = dataPackage.content;
			this->embeds = dataPackage.embeds;
			this->flags = dataPackage.flags;
			this->files = dataPackage.files;
			this->tts = dataPackage.tts;
			this->flags = 64;
		}

		CreateFollowUpMessageData(const RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->allowedMentions = dataPackage.allowedMentions;
			this->components = dataPackage.components;
			this->content = dataPackage.content;
			this->embeds = dataPackage.embeds;
			this->flags = dataPackage.flags;
			this->files = dataPackage.files;
			this->tts = dataPackage.tts;
		}

		CreateFollowUpMessageData(RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->allowedMentions = dataPackage.allowedMentions;
			this->components = dataPackage.components;
			this->content = dataPackage.content;
			this->embeds = dataPackage.embeds;
			this->flags = dataPackage.flags;
			this->files = dataPackage.files;
			this->tts = dataPackage.tts;
		}

		virtual ~CreateFollowUpMessageData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
	};

	/// For getting a follow-up Message. \brief For getting a follow-up Message.
	struct DiscordCoreAPI_Dll GetFollowUpMessageData {
		std::string interactionToken{};///< Interaction token.
		Snowflake applicationId{};///< application id.
		Snowflake messageId{};///< Message id.
	};

	/// For editing a follow up Message. \brief For editing a follow up Message.
	class DiscordCoreAPI_Dll EditFollowUpMessageData {
	  public:
		friend Interactions;
		friend InputEvents;

		EditFollowUpMessageData(const RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->data.allowedMentions = dataPackage.allowedMentions;
			this->messagePackage.channelId = dataPackage.channelId;
			this->messagePackage.messageId = dataPackage.messageId;
			this->data.components = dataPackage.components;
			this->data.content = dataPackage.content;
			this->data.embeds = dataPackage.embeds;
			this->data.files = dataPackage.files;
		}

		EditFollowUpMessageData(RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->data.allowedMentions = dataPackage.allowedMentions;
			this->messagePackage.channelId = dataPackage.channelId;
			this->messagePackage.messageId = dataPackage.messageId;
			this->data.components = dataPackage.components;
			this->data.content = dataPackage.content;
			this->data.embeds = dataPackage.embeds;
			this->data.files = dataPackage.files;
		}

		virtual ~EditFollowUpMessageData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		EditWebHookData data{};
	};

	/// For deleting a follow up Message. \brief For deleting a follow up Message.
	struct DiscordCoreAPI_Dll DeleteFollowUpMessageData {
		friend Interactions;
		friend InputEvents;

		DeleteFollowUpMessageData(RespondToInputEventData& dataPackage) {
			this->interactionPackage.interactionToken = dataPackage.interactionToken;
			this->interactionPackage.applicationId = dataPackage.applicationId;
			this->interactionPackage.interactionId = dataPackage.interactionId;
			this->messagePackage.messageId = dataPackage.messageId;
		}

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		uint32_t timeDelay{ 0 };
	};

	/// A single Interaction.
	struct DiscordCoreAPI_Dll Interaction : public InteractionData {
	  public:
		Interaction() = default;

		virtual ~Interaction() = default;
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// An interface class for the Interaction related Discord endpoints. \brief An interface class for the Interaction related Discord endpoints.
	class DiscordCoreAPI_Dll Interactions {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend DiscordCoreClient;
		friend EventHandler;
		friend EventManager;

		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// Creates a response to an input Interaction. \brief Creates a response to an input Interaction.
		/// \param dataPackage A CreateInteractionResponseData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> createInteractionResponseAsync(CreateInteractionResponseData dataPackage);

		/// Collects an Interaction response. \brief Collects an Interaction response.
		/// \param dataPackage A GetInteractionResponseData structure.
		/// \returns A CoRoutine containing an InteractionResponseData.
		static CoRoutine<Message> getInteractionResponseAsync(GetInteractionResponseData dataPackage);

		/// Edits an Interaction response. \brief Edits an Interaction response.
		/// \param dataPackage A EditInteractionResponseData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> editInteractionResponseAsync(EditInteractionResponseData dataPackage);

		/// Deletes an Interaction respnose. \brief Deletes an Interaction respnose.
		/// \param dataPackage A DeleteInteractionResponseData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage);

		/// Creates a follow up Message to an input Interaction. \brief Creates a follow up Message to an input Interaction.
		/// \param dataPackage A CreateFollowUpMessageData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage);

		/// Creates a follow up Message to an input Interaction. \brief Creates a follow up Message to an input Interaction.
		/// \param dataPackage A CreateFollowUpMessageData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> getFollowUpMessageAsync(GetFollowUpMessageData dataPackage);

		/// Edits a follow up Message. \brief Edits a follow up Message.
		/// \param dataPackage A EditFollowUpMessageData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage);

		/// Deletes a follow up Message. \brief Deletes a follow up Message.
		/// \param dataPackage A DeleteFollowUpMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// Select menu response data. \brief Select menu response data.
	struct DiscordCoreAPI_Dll SelectMenuResponseData {
		operator InteractionData() {
			return *this->interactionData;
		}

		SelectMenuResponseData& operator=(const SelectMenuResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->selectionId = other.selectionId;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->values = other.values;
				this->userId = other.userId;
			}
			return *this;
		}

		SelectMenuResponseData(const SelectMenuResponseData& other) {
			*this = other;
		}

		SelectMenuResponseData& operator=(SelectMenuResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->selectionId = other.selectionId;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->values = other.values;
				this->userId = other.userId;
			}
			return *this;
		}

		SelectMenuResponseData(SelectMenuResponseData& other) {
			*this = other;
		}

		SelectMenuResponseData() = default;

		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };///< Interaction data.
		std::vector<std::string> values{};///< A std::vector of the chosen values.
		std::string selectionId{};///< Selection id.
		Snowflake channelId{};///< The Channel id where it took place.
		Snowflake messageId{};///< The Message id where it took place.
		Snowflake userId{};///< The User id who selected the menu options.
	};

	/// SelectMenuCollector, for collecting select-menu input from one or more
	/// Users. \brief SelectMenuCollector, for collecting select-menu input from one
	/// or more Users.
	class DiscordCoreAPI_Dll SelectMenuCollector {
	  public:
		friend DiscordCoreClient;

		static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> selectMenuInteractionBufferMap;

		/// Constructor. \brief Constructor.
		/// \param dataPackage An InputEventData structure, from the response that came from the submitted select-menu.
		SelectMenuCollector(InputEventData& dataPackage);

		/// Used to collect the select-menu inputs from one or more users. \brief Used to collect the select-menu inputs from one or more users.
		/// \param getSelectMenuDataForAllNew Whether or not to collect select-menu input from a single target User or all potential users.
		/// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// \param maxCollectedSelectMenuCountNew The maximum number of inputs to collect before stopping.
		/// \param targetUserId The id of the single User to collect inputs from, if getSelectMenuDataForAllNew is set to false.
		/// \returns A std::vector of SelectMenuResponseData.
		CoRoutine<std::vector<SelectMenuResponseData>> collectSelectMenuData(bool getSelectMenuDataForAllNew, int32_t maxWaitTimeInMsNew, int32_t maxCollectedSelectMenuCountNew,
			Snowflake targetUserId = 0);

		~SelectMenuCollector();

	  protected:
		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
		UnboundedMessageBlock<InteractionData> selectMenuIncomingInteractionBuffer{};
		std::vector<SelectMenuResponseData> responseVector{};
		int32_t currentCollectedSelectMenuCount{ 0 };
		int32_t maxCollectedSelectMenuCount{ 0 };
		bool getSelectMenuDataForAll{ false };
		std::vector<std::string> values{};
		std::string bufferMapKey{};
		std::string selectMenuId{};
		uint32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake userId{};

		void run();
	};

	/// Button response data. \brief Button response data.
	struct DiscordCoreAPI_Dll ButtonResponseData {
		operator InteractionData() {
			return *this->interactionData;
		}

		ButtonResponseData& operator=(const ButtonResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->emojiName = other.emojiName;
				this->buttonId = other.buttonId;
				this->userId = other.userId;
			}
			return *this;
		}

		ButtonResponseData(const ButtonResponseData& other) {
			*this = other;
		}

		ButtonResponseData& operator=(ButtonResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->emojiName = other.emojiName;
				this->buttonId = other.buttonId;
				this->userId = other.userId;
			}
			return *this;
		}

		ButtonResponseData(ButtonResponseData& other) {
			*this = other;
		}

		ButtonResponseData() = default;

		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };///< Interaction data.
		std::string emojiName{};///< The emoji name, if applicable.
		std::string buttonId{};///< The id of the button, for identification.
		Snowflake channelId{};///< The Channel id where it took place.
		Snowflake messageId{};///< The Message id where it took place.
		Snowflake userId{};///< The User id who selected the menu options.
	};

	/// ButtonCollector, for collecting button input from one or more Users. \brief ButtonCollector, for collecting button input from one or more Users.
	class DiscordCoreAPI_Dll ButtonCollector {
	  public:
		friend DiscordCoreClient;

		static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> buttonInteractionBufferMap;

		/// Constructor. \brief Constructor.
		/// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
		ButtonCollector(InputEventData& dataPackage);

		/// Used to collect the button inputs from one or more users. \brief Used to collect the button inputs from one or more users.
		/// \param getButtonDataForAllNew Whether or not to collect input from a single target User or all potential users.
		/// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// \param maxNumberOfPressesNew The maximum number of inputs to collect before stopping.
		/// \param targetUserId The id of the single User to collect inputs from, if getButtonDataForAllNew is set to false.
		/// \returns A std::vector of ButtonResponseData.
		CoRoutine<std::vector<ButtonResponseData>> collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew, int32_t maxNumberOfPressesNew,
			Snowflake targetUserId = 0);

		~ButtonCollector();

	  protected:
		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
		UnboundedMessageBlock<InteractionData> buttonIncomingInteractionBuffer{};
		std::vector<ButtonResponseData> responseVector{};
		int32_t currentCollectedButtonCount{ 0 };
		int32_t maxCollectedButtonCount{ 0 };
		bool getButtonDataForAll{ false };
		std::vector<std::string> values{};
		std::string bufferMapKey{};
		uint32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		std::string buttonId{};
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake userId{};

		void run();
	};

	/// Button response data. \brief Button response data.
	struct DiscordCoreAPI_Dll ModalResponseData {
		operator InteractionData() {
			return *this->interactionData;
		}

		ModalResponseData& operator=(const ModalResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->customIdSmall = other.customIdSmall;
				this->channelId = other.channelId;
				this->customId = other.customId;
				this->userId = other.userId;
				this->value = other.value;
			}
			return *this;
		}

		ModalResponseData(const ModalResponseData& other) {
			*this = other;
		}

		ModalResponseData& operator=(ModalResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->customIdSmall = other.customIdSmall;
				this->channelId = other.channelId;
				this->customId = other.customId;
				this->userId = other.userId;
				this->value = other.value;
			}
			return *this;
		}

		ModalResponseData(ModalResponseData& other) {
			*this = other;
		}

		ModalResponseData() = default;

		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };///< Interaction data.
		std::string customIdSmall{};///< The customId of the particular input.
		std::string customId{};///< The customId of the modal component.
		Snowflake channelId{};///< The Channel id where it took place.
		std::string value{};/// The input value of the modal component.
		Snowflake userId{};///< The User id who selected the menu options.
	};

	/// ModalCollector, for collecting modal text input from one or more Users. \brief ModalCollector, for collecting modal text input from one or more Users.
	class DiscordCoreAPI_Dll ModalCollector {
	  public:
		friend DiscordCoreClient;

		static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> modalInteractionBufferMap;

		/// Constructor. \brief Constructor.
		/// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
		ModalCollector(InputEventData& dataPackage);

		/// Used to collect the button inputs from one or more users. \brief Used to collect the button inputs from one or more users.
		/// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// \returns A std::vector of ButtonResponseData.
		CoRoutine<ModalResponseData> collectModalData(int32_t maxWaitTimeInMsNew);

		~ModalCollector();

	  protected:
		UnboundedMessageBlock<InteractionData> modalIncomingInteractionBuffer{};
		int32_t currentCollectedButtonCount{ 0 };
		ModalResponseData responseData{};
		uint32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		Snowflake channelId{};

		void run();
	};

	/**@}*/
};// namespace DiscordCoreAPI