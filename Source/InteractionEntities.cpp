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
/// InteractionEntities.cpp - Source file for the interaction related classes and structs.
/// May 28, 2021
/// https://discordcoreapi.com
/// \file InteractionEntities.cpp

#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/ThreadPool.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::InteractionCallbackData> {
		using OTy2 = DiscordCoreAPI::InteractionCallbackData;
		static constexpr auto parseValue = object("attachments", &OTy2::attachments, "choices", &OTy2::choices, "components", &OTy2::components,
			"content", &OTy2::content, "custom_id", &OTy2::customId, "embeds", &OTy2::embeds, "files", &OTy2::files, "flags", &OTy2::flags, "tts",
			&OTy2::tts, "allowed_mentions", &OTy2::allowedMentions, "title", &OTy2::title);
	};

	template<> struct Core<DiscordCoreAPI::InteractionResponseBase> {
		using OTy2 = DiscordCoreAPI::InteractionResponseBase;
		static constexpr auto parseValue = object("type", &OTy2::type, "data", &OTy2::data);
	};

	template<> struct Core<DiscordCoreAPI::CreateInteractionResponseData> {
		using OTy2 = DiscordCoreAPI::CreateInteractionResponseData;
		static constexpr auto parseValue = object("type", &OTy2::type, "data", &OTy2::data);
	};

	template<> struct Core<DiscordCoreAPI::EditInteractionResponseData> {
		using OTy2 = DiscordCoreAPI::EditInteractionResponseData;
		static constexpr auto parseValue = object("content", &OTy2::content, "embeds", &OTy2::embeds, "allowed_mentions", &OTy2::allowedMentions,
			"components", &OTy2::components, "files", &OTy2::files, "attachments", &OTy2::attachments);
	};

	template<> struct Core<DiscordCoreAPI::CreateFollowUpMessageData> {
		using OTy2 = DiscordCoreAPI::CreateFollowUpMessageData;
		static constexpr auto parseValue = object("content", &OTy2::content, "username", &OTy2::userName, "avatar_url", &OTy2::avatarUrl, "tts",
			&OTy2::tts, "embeds", &OTy2::embeds, "allowed_mentions", &OTy2::allowedMentions, "components", &OTy2::components, "files", &OTy2::files,
			"attachments", &OTy2::attachments, "flags", &OTy2::flags);
	};

	template<> struct Core<DiscordCoreAPI::EditFollowUpMessageData> {
		using OTy2 = DiscordCoreAPI::EditFollowUpMessageData;
		static constexpr auto parseValue = object("content", &OTy2::content, "username", &OTy2::userName, "avatar_url", &OTy2::avatarUrl, "tts",
			&OTy2::tts, "embeds", &OTy2::embeds, "allowed_mentions", &OTy2::allowedMentions, "components", &OTy2::components, "files", &OTy2::files,
			"attachments", &OTy2::attachments, "flags", &OTy2::flags);
	};

}

namespace DiscordCoreAPI {

	InteractionResponseBase& InteractionResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->data.components.size() == 0) {
			ActionRowData actionRowData;
			this->data.components.emplace_back(actionRowData);
		}
		if (this->data.components.size() < 5) {
			if (this->data.components[this->data.components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				this->data.components[this->data.components.size() - 1].components.emplace_back(component);
			} else if (this->data.components[this->data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew,
		std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type,
		std::vector<ChannelType> channelTypes) {
		if (this->data.components.size() == 0) {
			ActionRowData actionRowData;
			this->data.components.emplace_back(actionRowData);
		}
		if (this->data.components.size() < 5) {
			if (this->data.components[this->data.components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				this->data.components[this->data.components.size() - 1].components.emplace_back(componentData);
			} else if (this->data.components[this->data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew,
		const std::string& titleNew, const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle,
		const std::string& label, const std::string& placeholder) {
		this->data.title = topTitleNew;
		this->data.customId = topCustomIdNew;
		if (this->data.components.size() == 0) {
			ActionRowData actionRowData;
			this->data.components.emplace_back(actionRowData);
		}
		if (this->data.components.size() < 5) {
			if (this->data.components[this->data.components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type = ComponentType::Text_Input;
				component.customId = customIdNew;
				component.style = static_cast<int32_t>(inputStyle);
				component.title = titleNew;
				component.maxLength = maxLength;
				component.minLength = minLength;
				component.label = label;
				component.required = required;
				component.placeholder = placeholder;
				this->data.components[this->data.components.size() - 1].components.emplace_back(component);
			} else if (this->data.components[this->data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addFile(const File& theFile) {
		this->data.files.emplace_back(theFile);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addAllowedMentions(const AllowedMentionsData& dataPackage) {
		this->data.allowedMentions = dataPackage;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addComponentRow(const ActionRowData& dataPackage) {
		this->data.components.emplace_back(dataPackage);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setResponseType(InteractionCallbackType type) {
		this->type = type;
		return *this;
	}

	void InteractionResponseBase::generateExcludedKeys() noexcept {
		this->data.generateExcludedKeys();
		return;
	}

	void EditInteractionResponseData::generateExcludedKeys() noexcept {
		if (this->attachments.size() == 0) {
			this->excludedKeys.emplace("attachments");
		}
		if (this->allowedMentions.parse.size() == 0 && this->allowedMentions.roles.size() == 0 && this->allowedMentions.users.size() == 0) {
			this->excludedKeys.emplace("allowed_mentions");
		}
		if (this->components.size() == 0) {
			this->excludedKeys.emplace("components");
		}
		for (auto& value: components) {
			for (auto& value02: value.components) {
				value02.generateExcludedKeys();
			}
		}
		if (this->embeds.size() == 0) {
			this->excludedKeys.emplace("embeds");
		}
		for (auto& value: this->embeds) {
			value.generateExcludedKeys();
		}
		if (this->files.size() == 0) {
			this->excludedKeys.emplace("files");
		}
		if (this->customId == "") {
			this->excludedKeys.emplace("custom_id");
		}
		if (this->title == "") {
			this->excludedKeys.emplace("title");
		}
		if (this->content == "") {
			this->excludedKeys.emplace("content");
		}
		return;
	}

	InteractionResponseBase& InteractionResponseBase::addMessageEmbed(const EmbedData& dataPackage) {
		this->data.embeds.emplace_back(dataPackage);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addContent(const std::string& dataPackage) {
		this->data.content = dataPackage;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setTTSStatus(bool enabledTTs) {
		this->data.tts = enabledTTs;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setFlags(int64_t flagsNew) {
		this->data.flags = flagsNew;
		return *this;
	}

	InteractionResponseData InteractionResponseBase::getInteractionResponseData() {
		InteractionResponseData returnData;
		returnData.data = this->data;
		returnData.type = this->type;
		return returnData;
	}

	void Interactions::initialize(DiscordCoreInternal::HttpsClient* client) {
		Interactions::httpsClient = client;
	}

	CreateEphemeralInteractionResponseData::CreateEphemeralInteractionResponseData(const RespondToInputEventData& dataPackage) {
		this->data = dataPackage;
		if (dataPackage.eventType == InteractionType::Message_Component) {
			this->type = InteractionCallbackType::Update_Message;
		} else {
			this->type = InteractionCallbackType::Channel_Message_With_Source;
		}
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->data.flags = 64;
	}

	CreateDeferredInteractionResponseData::CreateDeferredInteractionResponseData(const RespondToInputEventData& dataPackage) {
		this->data = dataPackage;
		if (dataPackage.eventType == InteractionType::Message_Component) {
			this->type = InteractionCallbackType::Deferred_Update_Message;
		} else {
			this->type = InteractionCallbackType::Deferred_Channel_Message_With_Source;
		}
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const CreateDeferredInteractionResponseData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		this->data.components = dataPackage.data.components;
		this->type = dataPackage.type;
		this->data = dataPackage.data;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const CreateEphemeralInteractionResponseData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		this->data.components = dataPackage.data.components;
		this->type = dataPackage.type;
		this->data = dataPackage.data;
		this->data.flags = 64;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const RespondToInputEventData& dataPackage) {
		this->data = dataPackage;
		if (dataPackage.eventType == InteractionType::Message_Component && dataPackage.type == InputEventResponseType::Deferred_Response) {
			this->type = InteractionCallbackType::Deferred_Update_Message;
		} else if (dataPackage.eventType == InteractionType::Message_Component) {
			this->type = InteractionCallbackType::Update_Message;
		} else if (dataPackage.eventType == InteractionType::Application_Command_Autocomplete ||
			dataPackage.type == InputEventResponseType::Application_Command_AutoComplete_Result) {
			this->type = InteractionCallbackType::Application_Command_Autocomplete_Result;
		} else {
			this->type = InteractionCallbackType::Channel_Message_With_Source;
		}
		if (dataPackage.type == InputEventResponseType::Modal_Interaction_Response || dataPackage.title != "") {
			this->type = InteractionCallbackType::Modal;
		}
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->data.files = dataPackage.files;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const InteractionData& dataPackage) {
		if (dataPackage.type == InteractionType::Message_Component) {
			this->type = InteractionCallbackType::Update_Message;
		} else {
			this->type = InteractionCallbackType::Channel_Message_With_Source;
		}
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionToken = dataPackage.token;
		this->interactionPackage.interactionId = dataPackage.id;
	}

	EditInteractionResponseData::EditInteractionResponseData(const RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->allowedMentions = dataPackage.allowedMentions;
		this->components = dataPackage.components;
		this->content = dataPackage.content;
		this->embeds = dataPackage.embeds;
		this->title = dataPackage.title;
		this->flags = dataPackage.flags;
		this->files = dataPackage.files;
		this->tts = dataPackage.tts;
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

	void CreateFollowUpMessageData::generateExcludedKeys() noexcept {
		if (this->attachments.size() == 0) {
			this->excludedKeys.emplace("attachments");
		}
		if (this->allowedMentions.parse.size() == 0 && this->allowedMentions.roles.size() == 0 && this->allowedMentions.users.size() == 0) {
			this->excludedKeys.emplace("allowed_mentions");
		}
		if (this->components.size() == 0) {
			this->excludedKeys.emplace("components");
		}
		for (auto& value: components) {
			for (auto& value02: value.components) {
				value02.generateExcludedKeys();
			}
		}
		if (this->embeds.size() == 0) {
			this->excludedKeys.emplace("embeds");
		}
		for (auto& value: this->embeds) {
			value.generateExcludedKeys();
		}
		if (this->files.size() == 0) {
			this->excludedKeys.emplace("files");
		}
		if (this->avatarUrl == "") {
			this->excludedKeys.emplace("avatar_url");
		}
		if (this->customId == "") {
			this->excludedKeys.emplace("custom_id");
		}
		if (this->userName == "") {
			this->excludedKeys.emplace("username");
		}
		if (this->title == "") {
			this->excludedKeys.emplace("title");
		}
		if (this->content == "") {
			this->excludedKeys.emplace("content");
		}
		return;
	}

	EditFollowUpMessageData::EditFollowUpMessageData(const RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->allowedMentions = dataPackage.allowedMentions;
		this->messagePackage.channelId = dataPackage.channelId;
		this->messagePackage.messageId = dataPackage.messageId;
		this->components = dataPackage.components;
		this->content = dataPackage.content;
		this->embeds = dataPackage.embeds;
		this->files = dataPackage.files;
	}

	DeleteFollowUpMessageData::DeleteFollowUpMessageData(const RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
		this->messagePackage.channelId = dataPackage.channelId;
		this->messagePackage.messageId = dataPackage.messageId;
	}

	DeleteInteractionResponseData::DeleteInteractionResponseData(const RespondToInputEventData& dataPackage) {
		this->interactionPackage.interactionToken = dataPackage.interactionToken;
		this->interactionPackage.applicationId = dataPackage.applicationId;
		this->interactionPackage.interactionId = dataPackage.interactionId;
	}

	CoRoutine<Message> Interactions::createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Interaction_Response };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath =
			"/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		dataPackage.generateExcludedKeys();
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::createInteractionResponseAsync()";
		Interactions::httpsClient->submitWorkloadAndGetResult(workload);
		GetInteractionResponseData dataPackage01{};
		dataPackage01.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackage01.interactionToken = dataPackage.interactionPackage.interactionToken;
		if (dataPackage.type != InteractionCallbackType::Application_Command_Autocomplete_Result) {
			co_return Interactions::getInteractionResponseAsync(dataPackage01).get();
		} else {
			co_return {};
		}
	}

	CoRoutine<Message> Interactions::getInteractionResponseAsync(GetInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Interaction_Response };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::getInteractionResponseAsync()";
		Message returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> Interactions::editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Interaction_Response };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/@original";
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::editInteractionResponseAsync()";
		Message returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> Interactions::deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Interaction_Response };
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(Milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/@original";
		workload.callStack = "Interactions::deleteInteractionResponseAsync()";
		Interactions::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	CoRoutine<Message> Interactions::createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Followup_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken;
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::createFollowUpMessageAsync()";
		Message returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> Interactions::getFollowUpMessageAsync(GetFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Followup_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
		workload.callStack = "Interactions::getFollowUpMessageAsync()";
		Message returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> Interactions::editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Followup_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/" + dataPackage.messagePackage.messageId;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::editFollowUpMessageAsync()";
		Message returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> Interactions::deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Followup_Message };
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(Milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/" + dataPackage.messagePackage.messageId;
		workload.callStack = "Interactions::deleteFollowUpMessageToBeWrappe()";
		Interactions::httpsClient->submitWorkloadAndGetResult(workload);
		co_return;
	}

	SelectMenuCollector::SelectMenuCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelData().id;
		this->messageId = dataPackage.getMessageData().id;
		*this->interactionData = dataPackage.getInteractionData();
		this->buffersMapKey = this->channelId + this->messageId;
	}

	CoRoutine<std::vector<SelectMenuResponseData>> SelectMenuCollector::collectSelectMenuData(bool getSelectMenuDataForAllNew,
		int32_t maxWaitTimeInMsNew, int32_t maxCollectedSelectMenuCountNew, CreateInteractionResponseData errorMessageDataNew, Snowflake targetUser) {
		co_await NewThreadAwaitable<std::vector<SelectMenuResponseData>>();
		SelectMenuCollector::selectMenuInteractionBuffersMap[this->buffersMapKey] = &this->selectMenuIncomingInteractionBuffer;
		if (targetUser == 0 && !getSelectMenuDataForAllNew) {
			this->getSelectMenuDataForAll = true;
		} else {
			this->getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		}
		if (targetUser != 0) {
			this->userId = targetUser;
		}
		this->maxCollectedSelectMenuCount = maxCollectedSelectMenuCountNew;
		this->getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		this->errorMessageData = errorMessageDataNew;
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return std::move(this->responseVector);
	}

	void SelectMenuCollector::collectSelectMenuData(std::function<bool(InteractionData)> triggerFunctionNew,
		DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		SelectMenuCollector::selectMenuInteractionEventsMap.add(std::move(functionNew));
	}

	SelectMenuCollector::~SelectMenuCollector() {
		if (SelectMenuCollector::selectMenuInteractionBuffersMap.contains(this->buffersMapKey)) {
			SelectMenuCollector::selectMenuInteractionBuffersMap.erase(this->buffersMapKey);
		}
	}

	void SelectMenuCollector::run() {
		int32_t currentCollectedSelectMenuCount{};
		StopWatch stopWatch{ Milliseconds{ this->maxTimeInMs } };
		while (!this->doWeQuit && !stopWatch.hasTimePassed()) {
			if (!this->getSelectMenuDataForAll) {
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
				if (selectMenuInteractionData->member.user.id != this->userId) {
					this->errorMessageData.interactionPackage.applicationId = selectMenuInteractionData->applicationId;
					this->errorMessageData.interactionPackage.interactionId = selectMenuInteractionData->id;
					this->errorMessageData.interactionPackage.interactionToken = selectMenuInteractionData->token;
					this->errorMessageData.messagePackage.messageId = selectMenuInteractionData->message.id;
					this->errorMessageData.messagePackage.channelId = selectMenuInteractionData->message.channelId;
					this->errorMessageData.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(this->errorMessageData).get();
				} else {
					*this->interactionData = *selectMenuInteractionData;
					this->selectMenuId = selectMenuInteractionData->data.customId;
					auto response = std::make_unique<SelectMenuResponseData>();
					response->selectionId = this->selectMenuId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = selectMenuInteractionData->user.id;
					response->values = this->interactionData->data.values;
					*response->interactionData = *selectMenuInteractionData;
					this->responseVector.emplace_back(*response);
					++currentCollectedSelectMenuCount;
					stopWatch.resetTimer();
					if (this->maxCollectedSelectMenuCount > 1 && currentCollectedSelectMenuCount < this->maxCollectedSelectMenuCount - 1) {
						auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
						createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
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
				this->selectMenuId = selectMenuInteractionData->data.customId;
				auto response = std::make_unique<SelectMenuResponseData>();
				response->selectionId = this->selectMenuId;
				response->channelId = this->channelId;
				response->messageId = this->messageId;
				response->userId = selectMenuInteractionData->user.id;
				*response->interactionData = *selectMenuInteractionData;
				response->values = this->interactionData->data.values;
				this->responseVector.emplace_back(*response);
				++currentCollectedSelectMenuCount;
				stopWatch.resetTimer();
				if (this->maxCollectedSelectMenuCount > 1 && currentCollectedSelectMenuCount < this->maxCollectedSelectMenuCount - 1) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
					createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
					this->doWeQuit = true;
					for (auto& value: this->responseVector) {
						*value.interactionData = *selectMenuInteractionData;
					}
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		SelectMenuCollector::selectMenuInteractionBuffersMap.erase(this->buffersMapKey);
	}

	ButtonCollector::ButtonCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelData().id;
		this->messageId = dataPackage.getMessageData().id;
		*this->interactionData = dataPackage.getInteractionData();
		this->buffersMapKey = this->channelId + this->messageId;
		ButtonCollector::buttonInteractionBuffersMap[this->buffersMapKey] = &this->buttonIncomingInteractionBuffer;
	}

	CoRoutine<std::vector<ButtonResponseData>> ButtonCollector::collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew,
		int32_t maxNumberOfPressesNew, CreateInteractionResponseData errorMessageDataNew, Snowflake targetUser) {
		co_await NewThreadAwaitable<std::vector<ButtonResponseData>>();
		if (targetUser == 0 && !getButtonDataForAllNew) {
			throw DCAException{ "ButtonCollector::collectButtonData(), You've failed to "
								"properly set the collectButtonData() parameters!" };
		}
		if (targetUser != 0) {
			this->userId = targetUser;
		}
		this->maxCollectedButtonCount = maxNumberOfPressesNew;
		this->getButtonDataForAll = getButtonDataForAllNew;
		this->errorMessageData = errorMessageDataNew;
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return std::move(this->responseVector);
	}

	void ButtonCollector::collectButtonData(std::function<bool(InteractionData)> triggerFunctionNew,
		DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		ButtonCollector::buttonInteractionEventsMap.add(std::move(functionNew));
	}

	ButtonCollector::~ButtonCollector() {
		if (ButtonCollector::buttonInteractionBuffersMap.contains(this->buffersMapKey)) {
			ButtonCollector::buttonInteractionBuffersMap.erase(this->buffersMapKey);
		}
	}

	void ButtonCollector::run() {
		int64_t currentCollectedButtonCount{};
		StopWatch stopWatch{ Milliseconds{ this->maxTimeInMs } };
		while (!this->doWeQuit && !stopWatch.hasTimePassed()) {
			if (!this->getButtonDataForAll) {
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
				if (buttonInteractionData->member.user.id != this->userId) {
					this->errorMessageData.interactionPackage.applicationId = buttonInteractionData->applicationId;
					this->errorMessageData.interactionPackage.interactionId = buttonInteractionData->id;
					this->errorMessageData.interactionPackage.interactionToken = buttonInteractionData->token;
					this->errorMessageData.messagePackage.messageId = buttonInteractionData->message.id;
					this->errorMessageData.messagePackage.channelId = buttonInteractionData->message.channelId;
					this->errorMessageData.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(this->errorMessageData).get();
				} else {
					*this->interactionData = *buttonInteractionData;
					this->buttonId = buttonInteractionData->data.customId;
					auto response = std::make_unique<ButtonResponseData>();
					response->buttonId = this->buttonId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					this->responseVector.emplace_back(*response);
					++currentCollectedButtonCount;
					stopWatch.resetTimer();
					if (this->maxCollectedButtonCount > 1 && currentCollectedButtonCount < this->maxCollectedButtonCount) {
						auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
						createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (currentCollectedButtonCount >= this->maxCollectedButtonCount) {
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
				this->buttonId = buttonInteractionData->data.customId;
				auto response = std::make_unique<ButtonResponseData>();
				response->buttonId = this->buttonId;
				response->channelId = this->channelId;
				response->messageId = this->messageId;
				response->userId = buttonInteractionData->user.id;
				*response->interactionData = *buttonInteractionData;
				this->responseVector.emplace_back(*response);
				++currentCollectedButtonCount;
				stopWatch.resetTimer();
				if (this->maxCollectedButtonCount > 1 && currentCollectedButtonCount < this->maxCollectedButtonCount) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
					createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (currentCollectedButtonCount >= this->maxCollectedButtonCount) {
					for (auto& value: this->responseVector) {
						*value.interactionData = *buttonInteractionData;
					}
					this->doWeQuit = true;
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		ButtonCollector::buttonInteractionBuffersMap.erase(this->buffersMapKey);
	}

	ModalCollector::ModalCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelData().id;
		ModalCollector::modalInteractionBuffersMap[this->channelId] = &this->modalIncomingInteractionBuffer;
	}

	CoRoutine<ModalResponseData> ModalCollector::collectModalData(int32_t maxWaitTimeInMsNew) {
		co_await NewThreadAwaitable<ModalResponseData>();
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return std::move(this->responseData);
	}

	void ModalCollector::collectModalData(std::function<bool(InteractionData)> triggerFunctionNew,
		DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		ModalCollector::modalInteractionEventsMap.add(std::move(functionNew));
	}

	ModalCollector::~ModalCollector() {
		if (ModalCollector::modalInteractionBuffersMap.contains(this->channelId)) {
			ModalCollector::modalInteractionBuffersMap.erase(this->channelId);
		}
	}

	void ModalCollector::run() {
		StopWatch stopWatch{ Milliseconds{ this->maxTimeInMs } };
		while (!this->doWeQuit && !stopWatch.hasTimePassed()) {
			auto buttonInteractionData = std::make_unique<InteractionData>();
			if (waitForTimeToPass(this->modalIncomingInteractionBuffer, *buttonInteractionData.get(), this->maxTimeInMs)) {
				*this->responseData.interactionData = *buttonInteractionData;
				this->responseData.channelId = buttonInteractionData->channelId;
				this->responseData.customId = buttonInteractionData->data.customId;
				this->responseData.customIdSmall = buttonInteractionData->data.customIdSmall;
				this->responseData.userId = buttonInteractionData->user.id;
				this->responseData.value = buttonInteractionData->data.value;
				break;
			} else {
				*this->responseData.interactionData = *buttonInteractionData;
				this->responseData.channelId = buttonInteractionData->channelId;
				this->responseData.customId = buttonInteractionData->data.customId;
				this->responseData.customIdSmall = buttonInteractionData->data.customIdSmall;
				this->responseData.userId = buttonInteractionData->user.id;
				this->responseData.value = buttonInteractionData->data.value;
				break;
			}
			std::this_thread::sleep_for(1ms);
		}

		ModalCollector::modalInteractionBuffersMap.erase(this->channelId);
	}

	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> SelectMenuCollector::selectMenuInteractionBuffersMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ButtonCollector::buttonInteractionBuffersMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ModalCollector::modalInteractionBuffersMap{};
	DiscordCoreInternal::TriggerEvent<void, InteractionData> SelectMenuCollector::selectMenuInteractionEventsMap{};
	DiscordCoreInternal::TriggerEvent<void, InteractionData> ButtonCollector::buttonInteractionEventsMap{};
	DiscordCoreInternal::TriggerEvent<void, InteractionData> ModalCollector::modalInteractionEventsMap{};
	DiscordCoreInternal::HttpsClient* Interactions::httpsClient{ nullptr };
};