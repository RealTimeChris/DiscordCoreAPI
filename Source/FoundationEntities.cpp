/*
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
/// FoundationEntities.cpp - Source file for the foundation entities.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file FoundationEntities.cpp

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

namespace DiscordCoreInternal {

	WebSocketResumeData::operator nlohmann::json() {
		nlohmann::json theData{};
		theData["d"]["seq"] = this->lastNumberReceived;
		theData["d"]["session_id"] = this->sessionId;
		theData["d"]["token"] = this->botToken;
		theData["op"] = 6;
		return theData;
	}

	WebSocketIdentifyData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["properties"]["browser"] = "DiscordCoreAPI";
		data["d"]["properties"]["device"] = "DiscordCoreAPI";
		data["d"]["shard"] = { this->currentShard, this->numberOfShards };
		data["d"]["large_threshold"] = 250;
		data["d"]["intents"] = this->intents;
		data["d"]["compress"] = false;
		data["d"]["token"] = this->botToken;
		data["op"] = 2;
#ifdef _WIN32
		data["d"]["properties"]["os"] = "Windows";
#else
		data["d"]["properties"]["os"] = "Linux";
#endif
		return data;
	}

	VoiceSocketProtocolPayloadData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["data"]["port"] = stol(this->voicePort);
		data["d"]["data"]["mode"] = this->voiceEncryptionMode;
		data["d"]["data"]["address"] = this->externalIp;
		data["d"]["protocol"] = "udp";
		data["op"] = 1;
		return data;
	}

	VoiceIdentifyData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["session_id"] = this->connectionData.sessionId;
		data["d"]["server_id"] = std::to_string(this->connectInitData.guildId);
		data["d"]["user_id"] = std::to_string(this->connectInitData.userId);
		data["d"]["token"] = this->connectionData.token;
		data["op"] = 0;
		return data;
	}

	SendSpeakingData::operator nlohmann::json() {
		nlohmann::json data{};
		data["d"]["speaking"] = this->type;
		data["d"]["delay"] = this->delay;
		data["d"]["ssrc"] = this->ssrc;
		data["op"] = 5;
		return data;
	}
}

namespace DiscordCoreAPI {

	InputEventData& InputEventData::operator=(const InputEventData& other) {
		if (this != &other) {
			*this->interactionData = *other.interactionData;
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(const InputEventData& other) {
		*this = other;
	}

	InputEventData& InputEventData::operator=(InputEventData& other) {
		if (this != &other) {
			*this->interactionData = *other.interactionData;
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(InputEventData& other) {
		*this = other;
	}

	InputEventData& InputEventData::operator=(InteractionData& other) {
		*this->interactionData = other;
		return *this;
	}

	InputEventData::InputEventData(InteractionData& interactionData) {
		*this = interactionData;
	}

	/// Returns the userName of the last User to trigger this input-event. \brief Returns the userName of the last User to trigger this input-event.
	/// \returns std::string A std::string containing the User name.
	std::string InputEventData::getUserName() {
		return this->interactionData->user.userName;
	}

	/// Gets the avatar Url of the last User to trigger this input-event. \brief Gets the avatar Url of the last User to trigger this input-event.
	/// \returns std::string A std::string containing the avatar Url.
	std::string InputEventData::getAvatarUrl() {
		return this->interactionData->user.avatar;
	}

	/// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
	/// \returns std::vector A std::vector containing the EmbedData.
	std::vector<EmbedData> InputEventData::getEmbeds() {
		return this->interactionData->message.embeds;
	}

	/// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
	/// \returns std::vector A std::vector containing ActionRowData.
	std::vector<ActionRowData> InputEventData::getComponents() {
		return this->interactionData->message.components;
	}

	/// Returns the User id of the last requester of this input-event. \brief Returns the User id of the last requester of this input-event.
	/// \returns uint64_t An uint64_t containing the author's id.
	uint64_t InputEventData::getAuthorId() {
		return this->interactionData->user.id;
	}

	/// Returns the Interaction id, if appplicable, of this input-event. \brief Returns the Interaction id, if appplicable, of this input-event.
	/// \returns uint64_t An uint64_t containing the Interaction id.
	uint64_t InputEventData::getInteractionId() {
		return this->interactionData->id;
	}

	/// Returns the application id. \brief Returns the application id.
	/// \returns uint64_t An uint64_t containing the application id.
	uint64_t InputEventData::getApplicationId() {
		return this->interactionData->applicationId;
	}

	/// Returns the Channel id of this input-event. \brief Returns the Channel id of this input-event.
	/// \returns uint64_t An uint64_t containing the Channel id.
	uint64_t InputEventData::getChannelId() {
		return this->interactionData->channelId;
	}

	/// Returns the Interaction token, if applicable, of this input-event. \brief Returns the Interaction token, if applicable, of this input-event.
	/// \returns std::string A std::string containing the Interaction token.
	std::string InputEventData::getInteractionToken() {
		return this->interactionData->token;
	}

	/// Returns the Guild id, of this input-event. \brief Returns the Guild id, of this input-event.
	/// \returns uint64_t An uint64_t containing the Guild id.
	uint64_t InputEventData::getGuildId() {
		return this->interactionData->guildId;
	}

	/// Returns the Message id, if applicable, of this input-event. \brief Returns the Message id, if applicable, of this input-event.
	/// \returns uint64_t An uint64_t containing the Message id.
	uint64_t InputEventData::getMessageId() {
		return this->interactionData->message.id;
	}

	/// Returns the Interaction data, if applicable, of this input-event. \brief Returns the InteractionData, if applicable, of this input-event.
	/// \returns InteractionData An InteractionData structure.
	InteractionData InputEventData::getInteractionData() {
		return *this->interactionData;
	}

	/// Returns the Message data, if applicable, of this input-event. \brief Returns the Message data, if applicable, of this input-event.
	/// \returns MessageData A MessageData structure.
	MessageData InputEventData::getMessageData() {
		return this->interactionData->message;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(InteractionData& dataPackage) {
		this->applicationId = dataPackage.applicationId;
		this->interactionToken = dataPackage.token;
		this->messageId = dataPackage.message.id;
		this->channelId = dataPackage.channelId;
		this->interactionId = dataPackage.id;
		this->eventType = dataPackage.type;
		return *this;
	};

	RespondToInputEventData::RespondToInputEventData(InteractionData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(InputEventData& dataPackage) {
		this->interactionToken = dataPackage.getInteractionToken();
		this->applicationId = dataPackage.getApplicationId();
		this->interactionId = dataPackage.getInteractionId();
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		return *this;
	}

	RespondToInputEventData::RespondToInputEventData(InputEventData& dataPackage) {
		*this = dataPackage;
	}

	/// Adds a button to the response Message. \brief Adds a button to the response Message.
	/// \param disabled Whether the button is active or not.
	/// \param customIdNew A custom id to give for identifying the button.
	/// \param buttonLabel A visible label for the button.
	/// \param buttonStyle The style of the button.
	/// \param emojiName An emoji name, if desired.
	/// \param emojiId An emoji id, if desired.
	/// \param url A url, if applicable.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
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
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = ComponentType::SelectMenu;
				componentData.placeholder = placeholder;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.customId = customIdNew;
				componentData.options = options;
				this->components[this->components.size() - 1].components.push_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
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
	RespondToInputEventData& RespondToInputEventData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
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
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
	/// \param theFile The file to be added.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addFile(File theFile) {
		this->files.push_back(theFile);
		return *this;
	}

	/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
	/// \param dataPackage An AllowedMentionsData structure.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	/// For setting the type of response to make. \brief For setting the type of response to make.
	/// \param typeNew An InputEventResponseType.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setResponseType(InputEventResponseType typeNew) {
		this->type = typeNew;
		return *this;
	}

	/// For setting the components in a response. \brief For setting the components in a response.
	/// \param dataPackage An ActionRowData structure.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addComponentRow(ActionRowData dataPackage) {
		this->components.push_back(dataPackage);
		return *this;
	}

	/// For setting the embeds in a response. \brief For setting the embeds in a response.
	/// \param dataPackage An EmbedData structure.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.push_back(dataPackage);
		return *this;
	}

	/// For setting the Message content in a response. \brief For setting the Message content in a response.
	/// \param dataPackage A std::string, containing the content.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	/// For setting the tts status of a response. \brief For setting the tts status of a response.
	/// \param enabledTTs A bool.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	/// For setting the choices of an autocomplete response. \brief For setting the choices of an autocomplete response.
	/// \param theValue An nlohmann::json value that is either a float, int32_t or a std::string.
	/// \param theName A std::string for the name of the choice.
	/// \param theNameLocalizations A std::unordered_map<std::string, std::string> for the name localizations.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(nlohmann::json theValue, const std::string& theName,
		std::unordered_map<std::string, std::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.value = theValue;
		choiceData.name = theName;
		this->choices.push_back(choiceData);
		return *this;
	}

	/// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
	/// \param targetUserIdNew A std::string, containing the target User's id.
	/// \returns RespondToInputEventData& A reference to this data structure.
	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const uint64_t& targetUserIdNew) {
		this->targetUserId = targetUserIdNew;
		return *this;
	}

	/// Adds a button to the response Message. \brief Adds a button to the response Message.
	/// \param disabled Whether the button is active or not.
	/// \param customIdNew A custom id to give for identifying the button.
	/// \param buttonLabel A visible label for the button.
	/// \param buttonStyle The style of the button.
	/// \param emojiName An emoji name, if desired.
	/// \param emojiId An emoji id, if desired.
	/// \param url A url, if applicable.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
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
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder,
		int32_t maxValues, int32_t minValues) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = ComponentType::SelectMenu;
				componentData.placeholder = placeholder;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.customId = customIdNew;
				componentData.options = options;
				this->components[this->components.size() - 1].components.push_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
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
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.push_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
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
				this->components[this->components.size() - 1].components.push_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
		}
		return *this;
	}

	/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
	/// \param theFile The file to be added.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addFile(File theFile) {
		this->files.push_back(theFile);
		return *this;
	}

	/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
	/// \param dataPackage An AllowedMentionsData structure.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	/// For setting the components in a response. \brief For setting the components in a response.
	/// \param dataPackage An ActionRowData structure.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addComponentRow(ActionRowData dataPackage) {
		this->components.push_back(dataPackage);
		return *this;
	}

	/// For setting the embeds in a response. \brief For setting the embeds in a response.
	/// \param dataPackage An EmbedData structure.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.push_back(dataPackage);
		return *this;
	}

	/// For setting the Message content in a response. \brief For setting the Message content in a response.
	/// \param dataPackage A std::string, containing the content.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	/// For setting the tts status of a response. \brief For setting the tts status of a response.
	/// \param enabledTTs A bool.
	/// \returns MessageResponseBase& A reference to this data structure.
	MessageResponseBase& MessageResponseBase::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	MoveThroughMessagePagesData moveThroughMessagePages(const std::string& userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const std::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
		MoveThroughMessagePagesData returnData{};
		uint32_t newCurrentPageIndex = currentPageIndex;
		std::unique_ptr<RespondToInputEventData> dataPackage{ std::make_unique<RespondToInputEventData>(originalEvent) };
		if (messageEmbeds.size() > 0) {
			dataPackage->addMessageEmbed(messageEmbeds[currentPageIndex]);
		}
		if (returnResult) {
			dataPackage->addButton(false, "select", "Select", ButtonStyle::Success, "✅");
		}
		dataPackage->addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
		dataPackage->addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
		dataPackage->addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
		dataPackage->setResponseType(InputEventResponseType::Edit_Interaction_Response);
		originalEvent = InputEvents::respondToInputEventAsync(*dataPackage).get();
		while (true) {
			std::unique_ptr<ButtonCollector> button{ std::make_unique<ButtonCollector>(originalEvent) };

			std::vector<ButtonResponseData> buttonIntData{ button->collectButtonData(false, waitForMaxMs, 1, stoull(userID)).get() };

			if (buttonIntData.size() == 0 || buttonIntData.at(0).buttonId == "empty" || buttonIntData.at(0).buttonId == "exit") {
				std::unique_ptr<RespondToInputEventData> dataPackage02{ std::make_unique<RespondToInputEventData>(originalEvent) };
				if (buttonIntData.at(0).buttonId == "empty") {
					*dataPackage02 = originalEvent;
				} else {
					std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(static_cast<InteractionData>(buttonIntData.at(0)));
					*dataPackage02 = RespondToInputEventData{ *interactionData };
				}

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				for (auto& value: originalEvent.getComponents()) {
					for (auto& value02: value.components) {
						value02.disabled = true;
					}
					dataPackage02->addComponentRow(value);
				}
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
					InputEvents::respondToInputEventAsync(*dataPackage02).get();
				}
				MoveThroughMessagePagesData dataPackage03{};
				dataPackage03.inputEventData = originalEvent;
				dataPackage03.buttonId = "exit";
				return dataPackage03;
			} else if (buttonIntData.at(0).buttonId == "forwards" || buttonIntData.at(0).buttonId == "backwards") {
				if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
					newCurrentPageIndex = 0;
				} else if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
					newCurrentPageIndex++;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex > 0)) {
					newCurrentPageIndex--;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex == 0)) {
					newCurrentPageIndex = static_cast<uint8_t>(messageEmbeds.size()) - 1;
				}
				std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(static_cast<InteractionData>(buttonIntData.at(0)));
				*dataPackage = RespondToInputEventData{ *interactionData };
				dataPackage->setResponseType(InputEventResponseType::Edit_Interaction_Response);
				for (auto& value: originalEvent.getComponents()) {
					dataPackage->addComponentRow(value);
				}
				dataPackage->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				InputEvents::respondToInputEventAsync(*dataPackage).get();
			} else if (buttonIntData.at(0).buttonId == "select") {
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(static_cast<InteractionData>(buttonIntData.at(0)));
					*dataPackage = RespondToInputEventData{ *interactionData };
					dataPackage->setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
					for (auto& value: originalEvent.getComponents()) {
						for (auto& value02: value.components) {
							value02.disabled = true;
						}
						dataPackage->addComponentRow(value);
					}
					InputEvents::respondToInputEventAsync(*dataPackage).get();
				}
				returnData.currentPageIndex = newCurrentPageIndex;
				returnData.inputEventData = originalEvent;
				returnData.buttonId = buttonIntData.at(0).buttonId;
				return returnData;
			}
		};
	};
};

namespace DiscordCoreInternal {

	std::unordered_map<HttpsWorkloadType, std::atomic_int64_t> HttpsWorkloadData::workloadIdsExternal{};
	std::unordered_map<HttpsWorkloadType, std::atomic_int64_t> HttpsWorkloadData::workloadIdsInternal{};
	std::mutex HttpsWorkloadData::accessMutex{};

}