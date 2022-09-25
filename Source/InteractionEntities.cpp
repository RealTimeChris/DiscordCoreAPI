/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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
/// InteractionEntities.cpp - Source file for the interaction related classes and structs.
/// May 28, 2021
/// https://discordcoreapi.com
/// \file InteractionEntities.cpp

#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/ThreadPool.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	InteractionResponseBase& InteractionResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->data.data.components.size() == 0) {
			ActionRowData actionRowData;
			this->data.data.components.emplace_back(actionRowData);
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
				this->data.data.components[this->data.data.components.size() - 1].components.emplace_back(component);
			} else if (this->data.data.components[this->data.data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->data.data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues) {
		if (this->data.data.components.size() == 0) {
			ActionRowData actionRowData;
			this->data.data.components.emplace_back(actionRowData);
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
				this->data.data.components[this->data.data.components.size() - 1].components.emplace_back(componentData);
			} else if (this->data.data.components[this->data.data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->data.data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
		this->data.data.title = topTitleNew;
		this->data.data.customId = topCustomIdNew;
		if (this->data.data.components.size() == 0) {
			ActionRowData actionRowData;
			this->data.data.components.emplace_back(actionRowData);
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
				this->data.data.components[this->data.data.components.size() - 1].components.emplace_back(component);
			} else if (this->data.data.components[this->data.data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->data.data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addFile(const File& theFile) {
		this->data.data.files.emplace_back(theFile);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addAllowedMentions(const AllowedMentionsData& dataPackage) {
		this->data.data.allowedMentions = dataPackage;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addComponentRow(const ActionRowData& dataPackage) {
		this->data.data.components.emplace_back(dataPackage);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setResponseType(InteractionCallbackType type) {
		this->data.type = type;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addMessageEmbed(const EmbedData& dataPackage) {
		this->data.data.embeds.emplace_back(dataPackage);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addContent(const std::string& dataPackage) {
		this->data.data.content = dataPackage;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setTTSStatus(bool enabledTTs) {
		this->data.data.tts = enabledTTs;
		return *this;
	}

	InteractionResponseData InteractionResponseBase::getInteractionResponseData() {
		return this->data;
	}

	void Interactions::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		Interactions::httpsClient = theClient;
	}

	CreateEphemeralInteractionResponseData::CreateEphemeralInteractionResponseData(const RespondToInputEventData& dataPackage) {
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

	CreateEphemeralInteractionResponseData::CreateEphemeralInteractionResponseData(RespondToInputEventData& dataPackage) {
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

	CreateDeferredInteractionResponseData::CreateDeferredInteractionResponseData(RespondToInputEventData& dataPackage) {
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

	CreateDeferredInteractionResponseData::CreateDeferredInteractionResponseData(const RespondToInputEventData& dataPackage) {
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

	CreateInteractionResponseData::CreateInteractionResponseData(const CreateDeferredInteractionResponseData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		this->data.data.components = dataPackage.data.data.components;
		this->data.type = dataPackage.data.type;
		this->data = dataPackage.data;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(CreateDeferredInteractionResponseData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		this->data.data.components = dataPackage.data.data.components;
		this->data.type = dataPackage.data.type;
		this->data = dataPackage.data;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const CreateEphemeralInteractionResponseData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		this->data.data.components = dataPackage.data.data.components;
		this->data.type = dataPackage.data.type;
		this->data = dataPackage.data;
		this->data.data.flags = 64;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(CreateEphemeralInteractionResponseData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		this->data.data.components = dataPackage.data.data.components;
		this->data.type = dataPackage.data.type;
		this->data = dataPackage.data;
		this->data.data.flags = 64;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(RespondToInputEventData& dataPackage) {
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
		this->data.data.files = dataPackage.files;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(InteractionData& dataPackage) {
		if (dataPackage.type == InteractionType::Message_Component) {
			this->data.type = InteractionCallbackType::Update_Message;
		} else {
			this->data.type = InteractionCallbackType::Channel_Message_With_Source;
		}
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionToken = dataPackage.token;
		this->interactionPackage.interactionId = dataPackage.id;
	}

	EditInteractionResponseData::EditInteractionResponseData(const RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->data.allowedMentions = dataPackage.allowedMentions;
		this->data.components = dataPackage.components;
		this->data.content = dataPackage.content;
		this->data.embeds = dataPackage.embeds;
		this->data.title = dataPackage.title;
		this->data.flags = dataPackage.flags;
		this->data.files = dataPackage.files;
		this->data.tts = dataPackage.tts;
	}

	EditInteractionResponseData::EditInteractionResponseData(RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->data.allowedMentions = dataPackage.allowedMentions;
		this->data.components = dataPackage.components;
		this->data.content = dataPackage.content;
		this->data.embeds = dataPackage.embeds;
		this->data.title = dataPackage.title;
		this->data.flags = dataPackage.flags;
		this->data.files = dataPackage.files;
		this->data.tts = dataPackage.tts;
	}

	DeleteInteractionResponseData::DeleteInteractionResponseData(RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
	}


	CreateEphemeralFollowUpMessageData::CreateEphemeralFollowUpMessageData(const RespondToInputEventData& dataPackage) {
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

	CreateEphemeralFollowUpMessageData::CreateEphemeralFollowUpMessageData(RespondToInputEventData& dataPackage) {
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

	CreateFollowUpMessageData::CreateFollowUpMessageData(const CreateEphemeralFollowUpMessageData& dataPackage) {
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

	CreateFollowUpMessageData::CreateFollowUpMessageData(CreateEphemeralFollowUpMessageData& dataPackage) {
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

	CreateFollowUpMessageData::CreateFollowUpMessageData(const RespondToInputEventData& dataPackage) {
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

	CreateFollowUpMessageData::CreateFollowUpMessageData(RespondToInputEventData& dataPackage) {
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

	EditFollowUpMessageData::EditFollowUpMessageData(const RespondToInputEventData& dataPackage) {
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

	EditFollowUpMessageData::EditFollowUpMessageData(RespondToInputEventData& dataPackage) {
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

	DeleteFollowUpMessageData::DeleteFollowUpMessageData(RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->messagePackage.messageId = dataPackage.messageId;
	}

	CoRoutine<Message> Interactions::createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Interaction_Response };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath =
			"/interactions/" + std::to_string(dataPackage.interactionPackage.interactionId) + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		if (dataPackage.data.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(dataPackage.data.operator DiscordCoreAPI::JsonObject(), dataPackage.data.data.files);
		} else {
			workload.content = dataPackage.data.operator JsonObject();
		}
		workload.callStack = "Interactions::createInteractionResponseAsync()";
		Interactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
		GetInteractionResponseData dataPackage01{};
		dataPackage01.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackage01.interactionToken = dataPackage.interactionPackage.interactionToken;
		if (dataPackage.data.type != InteractionCallbackType::Application_Command_Autocomplete_Result) {
			co_return Interactions::getInteractionResponseAsync(dataPackage01).get();
		} else {
			co_return Message{};
		}
	}

	CoRoutine<Message> Interactions::getInteractionResponseAsync(GetInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Interaction_Response };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.applicationId) + "/" + dataPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::getInteractionResponseAsync()";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Interactions::editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Interaction_Response };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath =
			"/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(dataPackage.data.operator DiscordCoreAPI::JsonObject(), dataPackage.data.files);
		} else {
			workload.content = dataPackage.data.operator DiscordCoreAPI::JsonObject();
		}
		workload.callStack = "Interactions::editInteractionResponseAsync()";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<void> Interactions::deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Interaction_Response };
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath =
			"/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::deleteInteractionResponseAsync()";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<Message> Interactions::createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Followup_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(dataPackage.operator DiscordCoreAPI::JsonObject(), dataPackage.files);
		} else {
			workload.content = dataPackage.operator DiscordCoreAPI::JsonObject();
		}
		workload.callStack = "Interactions::createFollowUpMessageAsync()";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Interactions::getFollowUpMessageAsync(GetFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Followup_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath =
			"/webhooks/" + std::to_string(dataPackage.applicationId) + "/" + dataPackage.interactionToken + "/messages/" + std::to_string(dataPackage.messageId);
		workload.callStack = "Interactions::getFollowUpMessageAsync()";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Interactions::editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Followup_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			std::to_string(dataPackage.messagePackage.messageId);
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(dataPackage.data.operator DiscordCoreAPI::JsonObject(), dataPackage.data.files);
		} else {
			workload.content = dataPackage.data.operator DiscordCoreAPI::JsonObject();
		}
		workload.callStack = "Interactions::editFollowUpMessageAsync()";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<void> Interactions::deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Followup_Message };
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			std::to_string(dataPackage.messagePackage.messageId);
		workload.callStack = "Interactions::deleteFollowUpMessageToBeWrappe()";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	SelectMenuCollector::SelectMenuCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		*this->interactionData = dataPackage.getInteractionData();
		this->bufferMapKey = std::to_string(this->channelId) + std::to_string(this->messageId);
		SelectMenuCollector::selectMenuInteractionBufferMap[this->bufferMapKey] = &this->selectMenuIncomingInteractionBuffer;
	}

	CoRoutine<std::vector<SelectMenuResponseData>> SelectMenuCollector::collectSelectMenuData(bool getSelectMenuDataForAllNew, int32_t maxWaitTimeInMsNew,
		int32_t maxCollectedSelectMenuCountNew, Snowflake targetUser) {
		co_await NewThreadAwaitable<std::vector<SelectMenuResponseData>>();
		if (targetUser.operator size_t() == 0 && !getSelectMenuDataForAllNew) {
			this->getSelectMenuDataForAll = true;
		} else {
			this->getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		}
		if (targetUser.operator size_t() != 0) {
			this->userId = targetUser;
		}
		this->maxCollectedSelectMenuCount = maxCollectedSelectMenuCountNew;
		this->getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseVector;
	}

	SelectMenuCollector::~SelectMenuCollector() {
		if (SelectMenuCollector::selectMenuInteractionBufferMap.contains(this->bufferMapKey)) {
			SelectMenuCollector::selectMenuInteractionBufferMap.erase(this->bufferMapKey);
		}
	}

	void SelectMenuCollector::run() {
		StopWatch theStopWatch{ std::chrono::milliseconds{ this->maxTimeInMs } };
		while (!this->doWeQuit && !theStopWatch.hasTimePassed()) {
			if (this->getSelectMenuDataForAll == false) {
				auto selectMenuInteractionData = std::make_unique<InteractionData>();
				if (waitForTimeToPass(this->selectMenuIncomingInteractionBuffer, *selectMenuInteractionData.get(), this->maxTimeInMs)) {
					this->selectMenuId = "empty";
					auto response = std::make_unique<SelectMenuResponseData>();
					response->selectionId = this->selectMenuId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = selectMenuInteractionData->user.id;
					*response->interactionData = *this->interactionData;
					response->values = std::vector<std::string>{ "empty" };
					this->responseVector.emplace_back(*response);
					break;
				}
				if (selectMenuInteractionData->user.id != this->userId) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
					auto embedData = std::make_unique<EmbedData>();
					embedData->setColor("FEFEFE");
					embedData->setTitle("__**PermissionTypes Issue:**__");
					embedData->setTimeStamp(getTimeAndDate());
					embedData->setDescription("Sorry, but that menu can only be selected by <@" + std::to_string(this->userId) + ">!");
					createResponseData->data.data.embeds.emplace_back(*embedData);
					createResponseData->data.data.flags = 64;
					createResponseData->data.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				} else {
					*this->interactionData = *selectMenuInteractionData;
					this->selectMenuId = selectMenuInteractionData->data.componentData.customId;
					auto response = std::make_unique<SelectMenuResponseData>();
					response->selectionId = this->selectMenuId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = selectMenuInteractionData->user.id;
					response->values = this->interactionData->data.componentData.values;
					*response->interactionData = *selectMenuInteractionData;
					this->responseVector.emplace_back(*response);
					this->currentCollectedSelectMenuCount++;
					theStopWatch.resetTimer();
					if (this->maxCollectedSelectMenuCount > 1 && this->currentCollectedSelectMenuCount < this->maxCollectedSelectMenuCount - 1) {
						auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
						createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (this->currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
						for (auto& value: this->responseVector) {
							*value.interactionData = *selectMenuInteractionData;
						}
						doWeQuit = true;
					}
				}
			} else {
				auto selectMenuInteractionData = std::make_unique<InteractionData>();
				if (waitForTimeToPass(this->selectMenuIncomingInteractionBuffer, *selectMenuInteractionData.get(), this->maxTimeInMs)) {
					this->selectMenuId = "empty";
					auto response = std::make_unique<SelectMenuResponseData>();
					response->selectionId = this->selectMenuId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = selectMenuInteractionData->user.id;
					*response->interactionData = *this->interactionData;
					response->values = std::vector<std::string>{ "empty" };
					this->responseVector.emplace_back(*response);
					break;
				}
				*this->interactionData = *selectMenuInteractionData;
				this->selectMenuId = selectMenuInteractionData->data.componentData.customId;
				auto response = std::make_unique<SelectMenuResponseData>();
				response->selectionId = this->selectMenuId;
				response->channelId = this->channelId;
				response->messageId = this->messageId;
				response->userId = selectMenuInteractionData->user.id;
				*response->interactionData = *selectMenuInteractionData;
				response->values = this->interactionData->data.componentData.values;
				this->responseVector.emplace_back(*response);
				this->currentCollectedSelectMenuCount++;
				theStopWatch.resetTimer();
				if (this->maxCollectedSelectMenuCount > 1 && this->currentCollectedSelectMenuCount < this->maxCollectedSelectMenuCount - 1) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
					createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (this->currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
					this->doWeQuit = true;
					for (auto& value: this->responseVector) {
						*value.interactionData = *selectMenuInteractionData;
					}
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		SelectMenuCollector::selectMenuInteractionBufferMap.erase(this->bufferMapKey);
	}

	ButtonCollector::ButtonCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		*this->interactionData = dataPackage.getInteractionData();
		this->bufferMapKey = std::to_string(this->channelId) + std::to_string(this->messageId);
		ButtonCollector::buttonInteractionBufferMap[this->bufferMapKey] = &this->buttonIncomingInteractionBuffer;
	}

	CoRoutine<std::vector<ButtonResponseData>> ButtonCollector::collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew, int32_t maxNumberOfPressesNew,
		Snowflake targetUser) {
		co_await NewThreadAwaitable<std::vector<ButtonResponseData>>();
		if (targetUser.operator size_t() == 0 && !getButtonDataForAllNew) {
			throw std::runtime_error{ "ButtonCollector::collectButtonData(), You've failed to "
									  "properly set the collectButtonData() parameters!\n\n" };
		}
		if (targetUser.operator size_t() != 0) {
			this->userId = targetUser;
		}
		this->maxCollectedButtonCount = maxNumberOfPressesNew;
		this->getButtonDataForAll = getButtonDataForAllNew;
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseVector;
	}

	ButtonCollector::~ButtonCollector() {
		if (ButtonCollector::buttonInteractionBufferMap.contains(this->bufferMapKey)) {
			ButtonCollector::buttonInteractionBufferMap.erase(this->bufferMapKey);
		}
	}

	void ButtonCollector::run() {
		StopWatch theStopWatch{ std::chrono::milliseconds{ this->maxTimeInMs } };
		while (!this->doWeQuit && !theStopWatch.hasTimePassed()) {
			if (this->getButtonDataForAll == false) {
				auto buttonInteractionData = std::make_unique<InteractionData>();
				if (waitForTimeToPass(this->buttonIncomingInteractionBuffer, *buttonInteractionData.get(), this->maxTimeInMs)) {
					this->buttonId = "empty";
					auto response = std::make_unique<ButtonResponseData>();
					response->buttonId = this->buttonId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *this->interactionData;
					this->responseVector.emplace_back(*response);
					break;
				}
				if (buttonInteractionData->user.id != this->userId) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
					auto embedData = std::make_unique<EmbedData>();
					embedData->setColor("FEFEFE");
					embedData->setTitle("__**PermissionTypes Issue:**__");
					embedData->setTimeStamp(getTimeAndDate());
					embedData->setDescription("Sorry, but that button can only be pressed by <@" + std::to_string(this->userId) + ">!");
					createResponseData->data.data.embeds.emplace_back(*embedData);
					createResponseData->data.data.flags = 64;
					createResponseData->data.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				} else {
					*this->interactionData = *buttonInteractionData;
					this->buttonId = buttonInteractionData->data.componentData.customId;
					auto response = std::make_unique<ButtonResponseData>();
					response->buttonId = this->buttonId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					this->responseVector.emplace_back(*response);
					this->currentCollectedButtonCount++;
					theStopWatch.resetTimer();
					if (this->maxCollectedButtonCount > 1 && this->currentCollectedButtonCount < this->maxCollectedButtonCount) {
						auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
						createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (this->currentCollectedButtonCount >= this->maxCollectedButtonCount) {
						for (auto& value: this->responseVector) {
							*value.interactionData = *buttonInteractionData;
						}
						doWeQuit = true;
					}
				}
			} else {
				auto buttonInteractionData = std::make_unique<InteractionData>();
				if (waitForTimeToPass(this->buttonIncomingInteractionBuffer, *buttonInteractionData.get(), this->maxTimeInMs)) {
					this->buttonId = "empty";
					auto response = std::make_unique<ButtonResponseData>();
					response->buttonId = this->buttonId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					this->responseVector.emplace_back(*response);
					break;
				}
				*this->interactionData = *buttonInteractionData;
				this->buttonId = buttonInteractionData->data.componentData.customId;
				auto response = std::make_unique<ButtonResponseData>();
				response->buttonId = this->buttonId;
				response->channelId = this->channelId;
				response->messageId = this->messageId;
				response->userId = buttonInteractionData->user.id;
				*response->interactionData = *buttonInteractionData;
				this->responseVector.emplace_back(*response);
				this->currentCollectedButtonCount++;
				theStopWatch.resetTimer();
				if (this->maxCollectedButtonCount > 1 && this->currentCollectedButtonCount < this->maxCollectedButtonCount) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
					createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (this->currentCollectedButtonCount >= this->maxCollectedButtonCount) {
					for (auto& value: this->responseVector) {
						*value.interactionData = *buttonInteractionData;
					}
					this->doWeQuit = true;
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		ButtonCollector::buttonInteractionBufferMap.erase(this->bufferMapKey);
	}

	ModalCollector::ModalCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelId();
		ModalCollector::modalInteractionBufferMap[std::to_string(this->channelId)] = &this->modalIncomingInteractionBuffer;
	}

	CoRoutine<ModalResponseData> ModalCollector::collectModalData(int32_t maxWaitTimeInMsNew) {
		co_await NewThreadAwaitable<ModalResponseData>();
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseData;
	}

	ModalCollector::~ModalCollector() {
		if (ModalCollector::modalInteractionBufferMap.contains(std::to_string(this->channelId))) {
			ModalCollector::modalInteractionBufferMap.erase(std::to_string(this->channelId));
		}
	}

	void ModalCollector::run() {
		while (!this->doWeQuit) {
			auto buttonInteractionData = std::make_unique<InteractionData>();
			if (waitForTimeToPass(this->modalIncomingInteractionBuffer, *buttonInteractionData.get(), this->maxTimeInMs)) {
				*this->responseData.interactionData = *buttonInteractionData;
				this->responseData.channelId = buttonInteractionData->channelId;
				this->responseData.customId = buttonInteractionData->data.modalData.customId;
				this->responseData.customIdSmall = buttonInteractionData->data.modalData.customIdSmall;
				this->responseData.userId = buttonInteractionData->user.id;
				this->responseData.value = buttonInteractionData->data.modalData.value;
				break;
			} else {
				*this->responseData.interactionData = *buttonInteractionData;
				this->responseData.channelId = buttonInteractionData->channelId;
				this->responseData.customId = buttonInteractionData->data.modalData.customId;
				this->responseData.customIdSmall = buttonInteractionData->data.modalData.customIdSmall;
				this->responseData.userId = buttonInteractionData->user.id;
				this->responseData.value = buttonInteractionData->data.modalData.value;
				break;
			}
			std::this_thread::sleep_for(1ms);
		}

		ModalCollector::modalInteractionBufferMap.erase(std::to_string(this->channelId));
	}

	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> SelectMenuCollector::selectMenuInteractionBufferMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ButtonCollector::buttonInteractionBufferMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ModalCollector::modalInteractionBufferMap{};
	DiscordCoreInternal::HttpsClient* Interactions::httpsClient{ nullptr };
};
