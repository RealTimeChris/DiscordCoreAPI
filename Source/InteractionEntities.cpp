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
/// InteractionEntities.cpp - Source file for the interaction related classes and structs.
/// May 28, 2021
/// https://discordcoreapi.com
/// \file InteractionEntities.cpp

#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::InteractionCallbackData> {
		using ValueType = DiscordCoreAPI::InteractionCallbackData;
		static constexpr auto parseValue =
			object("attachments", &ValueType::attachments, "choices", &ValueType::choices, "components", &ValueType::components, "content",
				&ValueType::content, "custom_id", &ValueType::customId, "embeds", &ValueType::embeds, "files", &ValueType::files, "flags",
				&ValueType::flags, "tts", &ValueType::tts, "allowed_mentions", &ValueType::allowedMentions, "title", &ValueType::title);
	};

	template<> struct Core<DiscordCoreAPI::InteractionResponseBase> {
		using ValueType = DiscordCoreAPI::InteractionResponseBase;
		static constexpr auto parseValue = object("type", &ValueType::type, "data", &ValueType::data);
	};

	template<> struct Core<DiscordCoreAPI::CreateInteractionResponseData> {
		using ValueType = DiscordCoreAPI::CreateInteractionResponseData;
		static constexpr auto parseValue = object("type", &ValueType::type, "data", &ValueType::data);
	};

	template<> struct Core<DiscordCoreAPI::EditInteractionResponseData> {
		using ValueType = DiscordCoreAPI::EditInteractionResponseData;
		static constexpr auto parseValue = object("content", &ValueType::content, "embeds", &ValueType::embeds, "allowed_mentions",
			&ValueType::allowedMentions, "components", &ValueType::components, "files", &ValueType::files, "attachments", &ValueType::attachments);
	};

	template<> struct Core<DiscordCoreAPI::CreateFollowUpMessageData> {
		using ValueType = DiscordCoreAPI::CreateFollowUpMessageData;
		static constexpr auto parseValue = object("content", &ValueType::content, "username", &ValueType::userName, "avatar_url",
			&ValueType::avatarUrl, "tts", &ValueType::tts, "embeds", &ValueType::embeds, "allowed_mentions", &ValueType::allowedMentions,
			"components", &ValueType::components, "files", &ValueType::files, "attachments", &ValueType::attachments, "flags", &ValueType::flags);
	};

	template<> struct Core<DiscordCoreAPI::EditFollowUpMessageData> {
		using ValueType = DiscordCoreAPI::EditFollowUpMessageData;
		static constexpr auto parseValue = object("content", &ValueType::content, "username", &ValueType::userName, "avatar_url",
			&ValueType::avatarUrl, "tts", &ValueType::tts, "embeds", &ValueType::embeds, "allowed_mentions", &ValueType::allowedMentions,
			"components", &ValueType::components, "files", &ValueType::files, "attachments", &ValueType::attachments, "flags", &ValueType::flags);
	};

}

namespace DiscordCoreAPI {

	InteractionResponseBase& InteractionResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (data.components.size() == 0) {
			ActionRowData actionRowData;
			data.components.emplace_back(actionRowData);
		}
		if (data.components.size() < 5) {
			if (data.components[data.components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				data.components[data.components.size() - 1].components.emplace_back(component);
			} else if (data.components[data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew,
		std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType typeNew,
		std::vector<ChannelType> channelTypes) {
		if (data.components.size() == 0) {
			ActionRowData actionRowData;
			data.components.emplace_back(actionRowData);
		}
		if (data.components.size() < 5) {
			if (data.components[data.components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(typeNew);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				data.components[data.components.size() - 1].components.emplace_back(componentData);
			} else if (data.components[data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew,
		const std::string& titleNew, const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle,
		const std::string& label, const std::string& placeholder) {
		data.title = topTitleNew;
		data.customId = topCustomIdNew;
		if (data.components.size() == 0) {
			ActionRowData actionRowData;
			data.components.emplace_back(actionRowData);
		}
		if (data.components.size() < 5) {
			if (data.components[data.components.size() - 1].components.size() < 5) {
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
				data.components[data.components.size() - 1].components.emplace_back(component);
			} else if (data.components[data.components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addFile(const File& theFile) {
		data.files.emplace_back(theFile);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addAllowedMentions(const AllowedMentionsData& dataPackage) {
		data.allowedMentions = dataPackage;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addComponentRow(const ActionRowData& dataPackage) {
		data.components.emplace_back(dataPackage);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setResponseType(InteractionCallbackType typeNew) {
		type = typeNew;
		return *this;
	}

	void InteractionResponseBase::generateExcludedKeys() {
		data.excludedKeys.clear();
		data.generateExcludedKeys();
		return;
	}

	void EditInteractionResponseData::generateExcludedKeys() {
		if (attachments.size() == 0) {
			excludedKeys.emplace("attachments");
		}
		if (allowedMentions.parse.size() == 0 && allowedMentions.roles.size() == 0 && allowedMentions.users.size() == 0) {
			excludedKeys.emplace("allowed_mentions");
		}
		if (components.size() == 0) {
			excludedKeys.emplace("components");
		}
		for (auto& value: components) {
			for (auto& value02: value.components) {
				value02.generateExcludedKeys();
			}
		}
		if (embeds.size() == 0) {
			excludedKeys.emplace("embeds");
		}
		for (auto& value: embeds) {
			value.generateExcludedKeys();
		}
		if (files.size() == 0) {
			excludedKeys.emplace("files");
		}
		if (customId == "") {
			excludedKeys.emplace("custom_id");
		}
		if (title == "") {
			excludedKeys.emplace("title");
		}
		if (content == "") {
			excludedKeys.emplace("content");
		}
		return;
	}

	InteractionResponseBase& InteractionResponseBase::addMessageEmbed(const EmbedData& dataPackage) {
		data.embeds.emplace_back(dataPackage);
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::addContent(const std::string& dataPackage) {
		data.content = dataPackage;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setTTSStatus(bool enabledTTs) {
		data.tts = enabledTTs;
		return *this;
	}

	InteractionResponseBase& InteractionResponseBase::setFlags(int64_t flagsNew) {
		data.flags = flagsNew;
		return *this;
	}

	InteractionResponseData InteractionResponseBase::getInteractionResponseData() {
		InteractionResponseData returnData;
		returnData.data = data;
		returnData.type = type;
		return returnData;
	}

	void Interactions::initialize(DiscordCoreInternal::HttpsClient* client) {
		Interactions::httpsClient = client;
	}

	CreateEphemeralInteractionResponseData::CreateEphemeralInteractionResponseData(const RespondToInputEventData& dataPackage) {
		data = dataPackage;
		if (dataPackage.eventType == InteractionType::Message_Component) {
			type = InteractionCallbackType::Update_Message;
		} else {
			type = InteractionCallbackType::Channel_Message_With_Source;
		}
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
		data.flags = 64;
	}

	CreateDeferredInteractionResponseData::CreateDeferredInteractionResponseData(const RespondToInputEventData& dataPackage) {
		data = dataPackage;
		if (dataPackage.eventType == InteractionType::Message_Component) {
			type = InteractionCallbackType::Deferred_Update_Message;
		} else {
			type = InteractionCallbackType::Deferred_Channel_Message_With_Source;
		}
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const CreateDeferredInteractionResponseData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		data.components = dataPackage.data.components;
		type = dataPackage.type;
		data = dataPackage.data;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const CreateEphemeralInteractionResponseData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
		data.components = dataPackage.data.components;
		type = dataPackage.type;
		data = dataPackage.data;
		data.flags = 64;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const RespondToInputEventData& dataPackage) {
		data = dataPackage;
		if (dataPackage.eventType == InteractionType::Message_Component && dataPackage.type == InputEventResponseType::Deferred_Response) {
			type = InteractionCallbackType::Deferred_Update_Message;
		} else if (dataPackage.eventType == InteractionType::Message_Component) {
			type = InteractionCallbackType::Update_Message;
		} else if (dataPackage.eventType == InteractionType::Application_Command_Autocomplete ||
			dataPackage.type == InputEventResponseType::Application_Command_AutoComplete_Result) {
			type = InteractionCallbackType::Application_Command_Autocomplete_Result;
		} else {
			type = InteractionCallbackType::Channel_Message_With_Source;
		}
		if (dataPackage.type == InputEventResponseType::Modal_Interaction_Response || dataPackage.title != "") {
			type = InteractionCallbackType::Modal;
		}
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
		data.files = dataPackage.files;
	}

	CreateInteractionResponseData::CreateInteractionResponseData(const InteractionData& dataPackage) {
		if (dataPackage.type == InteractionType::Message_Component) {
			type = InteractionCallbackType::Update_Message;
		} else {
			type = InteractionCallbackType::Channel_Message_With_Source;
		}
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionToken = dataPackage.token;
		interactionPackage.interactionId = dataPackage.id;
	}

	EditInteractionResponseData::EditInteractionResponseData(const RespondToInputEventData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
		allowedMentions = dataPackage.allowedMentions;
		components = dataPackage.components;
		content = dataPackage.content;
		embeds = dataPackage.embeds;
		title = dataPackage.title;
		flags = dataPackage.flags;
		files = dataPackage.files;
		tts = dataPackage.tts;
	}

	CreateEphemeralFollowUpMessageData::CreateEphemeralFollowUpMessageData(const RespondToInputEventData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
		allowedMentions = dataPackage.allowedMentions;
		components = dataPackage.components;
		content = dataPackage.content;
		embeds = dataPackage.embeds;
		files = dataPackage.files;
		tts = dataPackage.tts;
		flags = 64;
	}

	CreateFollowUpMessageData::CreateFollowUpMessageData(const CreateEphemeralFollowUpMessageData& dataPackage) {
		interactionPackage = dataPackage.interactionPackage;
		allowedMentions = dataPackage.allowedMentions;
		components = dataPackage.components;
		content = dataPackage.content;
		embeds = dataPackage.embeds;
		flags = dataPackage.flags;
		files = dataPackage.files;
		tts = dataPackage.tts;
		flags = 64;
	}

	CreateFollowUpMessageData::CreateFollowUpMessageData(const RespondToInputEventData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
		allowedMentions = dataPackage.allowedMentions;
		components = dataPackage.components;
		content = dataPackage.content;
		embeds = dataPackage.embeds;
		flags = dataPackage.flags;
		files = dataPackage.files;
		tts = dataPackage.tts;
	}

	void CreateFollowUpMessageData::generateExcludedKeys() {
		if (attachments.size() == 0) {
			excludedKeys.emplace("attachments");
		}
		if (allowedMentions.parse.size() == 0 && allowedMentions.roles.size() == 0 && allowedMentions.users.size() == 0) {
			excludedKeys.emplace("allowed_mentions");
		}
		if (components.size() == 0) {
			excludedKeys.emplace("components");
		}
		for (auto& value: components) {
			for (auto& value02: value.components) {
				value02.generateExcludedKeys();
			}
		}
		if (embeds.size() == 0) {
			excludedKeys.emplace("embeds");
		} else {
			for (auto& value: embeds) {
				value.generateExcludedKeys();
			}
		}
		if (files.size() == 0) {
			excludedKeys.emplace("files");
		}
		if (avatarUrl == "") {
			excludedKeys.emplace("avatar_url");
		}
		if (customId == "") {
			excludedKeys.emplace("custom_id");
		}
		if (userName == "") {
			excludedKeys.emplace("username");
		}
		if (title == "") {
			excludedKeys.emplace("title");
		}
		if (content == "") {
			excludedKeys.emplace("content");
		}
		return;
	}

	EditFollowUpMessageData::EditFollowUpMessageData(const RespondToInputEventData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
		allowedMentions = dataPackage.allowedMentions;
		messagePackage.channelId = dataPackage.channelId;
		messagePackage.messageId = dataPackage.messageId;
		components = dataPackage.components;
		content = dataPackage.content;
		embeds = dataPackage.embeds;
		files = dataPackage.files;
	}

	DeleteFollowUpMessageData::DeleteFollowUpMessageData(const RespondToInputEventData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
		messagePackage.channelId = dataPackage.channelId;
		messagePackage.messageId = dataPackage.messageId;
	}

	DeleteInteractionResponseData::DeleteInteractionResponseData(const RespondToInputEventData& dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId = dataPackage.applicationId;
		interactionPackage.interactionId = dataPackage.interactionId;
	}

	CoRoutine<MessageData> Interactions::createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Interaction_Response };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath =
			"/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		dataPackage.generateExcludedKeys();
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::createInteractionResponseAsync()";
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		GetInteractionResponseData dataPackage01{};
		dataPackage01.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackage01.interactionToken = dataPackage.interactionPackage.interactionToken;
		if (dataPackage.type != InteractionCallbackType::Application_Command_Autocomplete_Result) {
			co_return Interactions::getInteractionResponseAsync(dataPackage01).get();
		} else {
			co_return {};
		}
	}

	MessageData Interactions::createInteractionResponse(CreateInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Interaction_Response };
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath =
			"/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		dataPackage.generateExcludedKeys();
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::createInteractionResponseAsync()";
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		GetInteractionResponseData dataPackage01{};
		dataPackage01.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackage01.interactionToken = dataPackage.interactionPackage.interactionToken;
		if (dataPackage.type != InteractionCallbackType::Application_Command_Autocomplete_Result) {
			return Interactions::getInteractionResponseAsync(dataPackage01).get();
		} else {
			return {};
		}
	}

	CoRoutine<MessageData> Interactions::getInteractionResponseAsync(GetInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Interaction_Response };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::getInteractionResponseAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> Interactions::editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Interaction_Response };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/@original";
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::editInteractionResponseAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	MessageData Interactions::editInteractionResponse(EditInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Interaction_Response };
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/@original";
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::editInteractionResponseAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		return returnData;
	}

	CoRoutine<void> Interactions::deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Interaction_Response };
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(Milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/@original";
		workload.callStack = "Interactions::deleteInteractionResponseAsync()";
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<MessageData> Interactions::createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Followup_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken;
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::createFollowUpMessageAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	MessageData Interactions::createFollowUpMessage(CreateFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Followup_Message };
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken;
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::createFollowUpMessageAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		return returnData;
	}

	CoRoutine<MessageData> Interactions::getFollowUpMessageAsync(GetFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Followup_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
		workload.callStack = "Interactions::getFollowUpMessageAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> Interactions::editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Followup_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/" + dataPackage.messagePackage.messageId;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::editFollowUpMessageAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	MessageData Interactions::editFollowUpMessage(EditFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Followup_Message };
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/" + dataPackage.messagePackage.messageId;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		} else {
			jsonifierCore.serializeJson<true>(dataPackage, workload.content);
		}
		workload.callStack = "Interactions::editFollowUpMessageAsync()";
		MessageData returnData{};
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		return returnData;
	}

	CoRoutine<void> Interactions::deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Followup_Message };
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(Milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken +
			"/messages/" + dataPackage.messagePackage.messageId;
		workload.callStack = "Interactions::deleteFollowUpMessageToBeWrappe()";
		Interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	SelectMenuCollector::SelectMenuCollector(InputEventData& dataPackage) {
		channelId = dataPackage.getChannelData().id;
		messageId = dataPackage.getMessageData().id;
		*interactionData = dataPackage.getInteractionData();
		buffersMapKey = channelId + messageId;
	}

	CoRoutine<std::vector<SelectMenuResponseData>, false> SelectMenuCollector::collectSelectMenuData(bool getSelectMenuDataForAllNew,
		int32_t maxWaitTimeInMsNew, int32_t maxCollectedSelectMenuCountNew, CreateInteractionResponseData errorMessageDataNew, Snowflake targetUser) {
		co_await NewThreadAwaitable<std::vector<SelectMenuResponseData>, false>();
		SelectMenuCollector::selectMenuInteractionBuffersMap[buffersMapKey] = &selectMenuIncomingInteractionBuffer;
		if (targetUser == 0 && !getSelectMenuDataForAllNew) {
			getSelectMenuDataForAll = true;
		} else {
			getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		}
		if (targetUser != 0) {
			userId = targetUser;
		}
		maxCollectedSelectMenuCount = maxCollectedSelectMenuCountNew;
		getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		errorMessageData = errorMessageDataNew;
		maxTimeInMs = maxWaitTimeInMsNew;
		run();
		co_return std::move(responseVector);
	}

	void SelectMenuCollector::collectSelectMenuData(std::function<bool(InteractionData)> triggerFunctionNew,
		DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		SelectMenuCollector::selectMenuInteractionEventsMap.add(std::move(functionNew));
	}

	SelectMenuCollector::~SelectMenuCollector() {
		if (SelectMenuCollector::selectMenuInteractionBuffersMap.contains(buffersMapKey)) {
			SelectMenuCollector::selectMenuInteractionBuffersMap.erase(buffersMapKey);
		}
	}

	void SelectMenuCollector::run() {
		StopWatch stopWatch{ Milliseconds{ maxTimeInMs } };
		while (!doWeQuit && !stopWatch.hasTimePassed()) {
			if (!getSelectMenuDataForAll) {
				auto selectMenuInteractionData = makeUnique<InteractionData>();
				if (waitForTimeToPass(selectMenuIncomingInteractionBuffer, *selectMenuInteractionData.get(), maxTimeInMs)) {
					selectMenuId = "empty";
					auto response = makeUnique<SelectMenuResponseData>();
					response->selectionId = selectMenuId;
					response->channelId = channelId;
					response->messageId = messageId;
					response->userId = selectMenuInteractionData->user.id;
					*response->interactionData = *interactionData;
					response->values = std::vector<std::string>{ "empty" };
					responseVector.emplace_back(*response);
					break;
				}
				if (selectMenuInteractionData->member.user.id != userId) {
					errorMessageData.interactionPackage.applicationId = selectMenuInteractionData->applicationId;
					errorMessageData.interactionPackage.interactionId = selectMenuInteractionData->id;
					errorMessageData.interactionPackage.interactionToken = selectMenuInteractionData->token;
					errorMessageData.messagePackage.messageId = selectMenuInteractionData->message.id;
					errorMessageData.messagePackage.channelId = selectMenuInteractionData->message.channelId;
					errorMessageData.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(errorMessageData).get();
				} else {
					*interactionData = *selectMenuInteractionData;
					selectMenuId = selectMenuInteractionData->data.customId;
					auto response = makeUnique<SelectMenuResponseData>();
					response->selectionId = selectMenuId;
					response->channelId = channelId;
					response->messageId = messageId;
					response->userId = selectMenuInteractionData->user.id;
					response->values = interactionData->data.values;
					*response->interactionData = *selectMenuInteractionData;
					responseVector.emplace_back(*response);
					++currentCollectedSelectMenuCount;
					stopWatch.resetTimer();
					if (maxCollectedSelectMenuCount > 1 && currentCollectedSelectMenuCount < maxCollectedSelectMenuCount - 1) {
						auto createResponseData = makeUnique<CreateInteractionResponseData>(*selectMenuInteractionData);
						createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (currentCollectedSelectMenuCount >= maxCollectedSelectMenuCount) {
						for (auto& value: responseVector) {
							*value.interactionData = *selectMenuInteractionData;
						}
						doWeQuit = true;
					}
				}
			} else {
				auto selectMenuInteractionData = makeUnique<InteractionData>();
				if (waitForTimeToPass(selectMenuIncomingInteractionBuffer, *selectMenuInteractionData.get(), maxTimeInMs)) {
					selectMenuId = "empty";
					auto response = makeUnique<SelectMenuResponseData>();
					response->selectionId = selectMenuId;
					response->channelId = channelId;
					response->messageId = messageId;
					response->userId = selectMenuInteractionData->user.id;
					*response->interactionData = *interactionData;
					response->values = std::vector<std::string>{ "empty" };
					responseVector.emplace_back(*response);
					break;
				}
				*interactionData = *selectMenuInteractionData;
				selectMenuId = selectMenuInteractionData->data.customId;
				auto response = makeUnique<SelectMenuResponseData>();
				response->selectionId = selectMenuId;
				response->channelId = channelId;
				response->messageId = messageId;
				response->userId = selectMenuInteractionData->user.id;
				*response->interactionData = *selectMenuInteractionData;
				response->values = interactionData->data.values;
				responseVector.emplace_back(*response);
				++currentCollectedSelectMenuCount;
				stopWatch.resetTimer();
				if (maxCollectedSelectMenuCount > 1 && currentCollectedSelectMenuCount < maxCollectedSelectMenuCount - 1) {
					auto createResponseData = makeUnique<CreateInteractionResponseData>(*selectMenuInteractionData);
					createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (currentCollectedSelectMenuCount >= maxCollectedSelectMenuCount) {
					doWeQuit = true;
					for (auto& value: responseVector) {
						*value.interactionData = *selectMenuInteractionData;
					}
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		SelectMenuCollector::selectMenuInteractionBuffersMap.erase(buffersMapKey);
	}

	ButtonCollector::ButtonCollector(InputEventData& dataPackage) {
		channelId = dataPackage.getChannelData().id;
		messageId = dataPackage.getMessageData().id;
		*interactionData = dataPackage.getInteractionData();
		buffersMapKey = channelId + messageId;
		ButtonCollector::buttonInteractionBuffersMap[buffersMapKey] = &buttonIncomingInteractionBuffer;
	}

	CoRoutine<std::vector<ButtonResponseData>, false> ButtonCollector::collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew,
		int32_t maxNumberOfPressesNew, CreateInteractionResponseData errorMessageDataNew, Snowflake targetUser) {
		co_await NewThreadAwaitable<std::vector<ButtonResponseData>, false>();
		if (targetUser == 0 && !getButtonDataForAllNew) {
			throw DCAException{ "ButtonCollector::collectButtonData(), You've failed to "
								"properly set the collectButtonData() parameters!" };
		}
		if (targetUser != 0) {
			userId = targetUser;
		}
		maxCollectedButtonCount = maxNumberOfPressesNew;
		getButtonDataForAll = getButtonDataForAllNew;
		errorMessageData = errorMessageDataNew;
		maxTimeInMs = maxWaitTimeInMsNew;
		run();
		co_return std::move(responseVector);
	}

	void ButtonCollector::collectButtonData(std::function<bool(InteractionData)> triggerFunctionNew,
		DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		ButtonCollector::buttonInteractionEventsMap.add(std::move(functionNew));
	}

	ButtonCollector::~ButtonCollector() {
		if (ButtonCollector::buttonInteractionBuffersMap.contains(buffersMapKey)) {
			ButtonCollector::buttonInteractionBuffersMap.erase(buffersMapKey);
		}
	}

	void ButtonCollector::run() {
		StopWatch stopWatch{ Milliseconds{ maxTimeInMs } };
		while (!doWeQuit && !stopWatch.hasTimePassed()) {
			if (!getButtonDataForAll) {
				auto buttonInteractionData = makeUnique<InteractionData>();
				if (waitForTimeToPass(buttonIncomingInteractionBuffer, *buttonInteractionData.get(), maxTimeInMs)) {
					buttonId = "empty";
					auto response = makeUnique<ButtonResponseData>();
					response->buttonId = buttonId;
					response->channelId = channelId;
					response->messageId = messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *interactionData;
					responseVector.emplace_back(*response);
					break;
				}
				if (buttonInteractionData->member.user.id != userId) {
					errorMessageData.interactionPackage.applicationId = buttonInteractionData->applicationId;
					errorMessageData.interactionPackage.interactionId = buttonInteractionData->id;
					errorMessageData.interactionPackage.interactionToken = buttonInteractionData->token;
					errorMessageData.messagePackage.messageId = buttonInteractionData->message.id;
					errorMessageData.messagePackage.channelId = buttonInteractionData->message.channelId;
					errorMessageData.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(errorMessageData).get();
				} else {
					*interactionData = *buttonInteractionData;
					buttonId = buttonInteractionData->data.customId;
					auto response = makeUnique<ButtonResponseData>();
					response->buttonId = buttonId;
					response->channelId = channelId;
					response->messageId = messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					responseVector.emplace_back(*response);
					++currentCollectedButtonCount;
					stopWatch.resetTimer();
					if (maxCollectedButtonCount > 1 && currentCollectedButtonCount < maxCollectedButtonCount) {
						auto createResponseData = makeUnique<CreateInteractionResponseData>(*buttonInteractionData);
						createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (currentCollectedButtonCount >= maxCollectedButtonCount) {
						for (auto& value: responseVector) {
							*value.interactionData = *buttonInteractionData;
						}
						doWeQuit = true;
					}
				}
			} else {
				auto buttonInteractionData = makeUnique<InteractionData>();
				if (waitForTimeToPass(buttonIncomingInteractionBuffer, *buttonInteractionData.get(), maxTimeInMs)) {
					buttonId = "empty";
					auto response = makeUnique<ButtonResponseData>();
					response->buttonId = buttonId;
					response->channelId = channelId;
					response->messageId = messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					responseVector.emplace_back(*response);
					break;
				}
				*interactionData = *buttonInteractionData;
				buttonId = buttonInteractionData->data.customId;
				auto response = makeUnique<ButtonResponseData>();
				response->buttonId = buttonId;
				response->channelId = channelId;
				response->messageId = messageId;
				response->userId = buttonInteractionData->user.id;
				*response->interactionData = *buttonInteractionData;
				responseVector.emplace_back(*response);
				++currentCollectedButtonCount;
				stopWatch.resetTimer();
				if (maxCollectedButtonCount > 1 && currentCollectedButtonCount < maxCollectedButtonCount) {
					auto createResponseData = makeUnique<CreateInteractionResponseData>(*buttonInteractionData);
					createResponseData->type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (currentCollectedButtonCount >= maxCollectedButtonCount) {
					for (auto& value: responseVector) {
						*value.interactionData = *buttonInteractionData;
					}
					doWeQuit = true;
				}
			}
			std::this_thread::sleep_for(1ms);
		}
		ButtonCollector::buttonInteractionBuffersMap.erase(buffersMapKey);
	}

	ModalCollector::ModalCollector(InputEventData& dataPackage) {
		channelId = dataPackage.getChannelData().id;
		ModalCollector::modalInteractionBuffersMap[channelId] = &modalIncomingInteractionBuffer;
	}

	CoRoutine<ModalResponseData, false> ModalCollector::collectModalData(int32_t maxWaitTimeInMsNew) {
		co_await NewThreadAwaitable<ModalResponseData, false>();
		maxTimeInMs = maxWaitTimeInMsNew;
		run();
		co_return std::move(responseData);
	}

	void ModalCollector::collectModalData(std::function<bool(InteractionData)> triggerFunctionNew,
		DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		ModalCollector::modalInteractionEventsMap.add(std::move(functionNew));
	}

	ModalCollector::~ModalCollector() {
		if (ModalCollector::modalInteractionBuffersMap.contains(channelId)) {
			ModalCollector::modalInteractionBuffersMap.erase(channelId);
		}
	}

	void ModalCollector::run() {
		StopWatch stopWatch{ Milliseconds{ maxTimeInMs } };
		while (!doWeQuit && !stopWatch.hasTimePassed()) {
			auto buttonInteractionData = makeUnique<InteractionData>();
			if (waitForTimeToPass(modalIncomingInteractionBuffer, *buttonInteractionData.get(), maxTimeInMs)) {
				*responseData.interactionData = *buttonInteractionData;
				responseData.channelId = buttonInteractionData->channelId;
				responseData.customId = buttonInteractionData->data.customId;
				responseData.customIdSmall = buttonInteractionData->data.customIdSmall;
				responseData.userId = buttonInteractionData->user.id;
				responseData.value = buttonInteractionData->data.value;
				break;
			} else {
				*responseData.interactionData = *buttonInteractionData;
				responseData.channelId = buttonInteractionData->channelId;
				responseData.customId = buttonInteractionData->data.customId;
				responseData.customIdSmall = buttonInteractionData->data.customIdSmall;
				responseData.userId = buttonInteractionData->user.id;
				responseData.value = buttonInteractionData->data.value;
				break;
			}
		}

		ModalCollector::modalInteractionBuffersMap.erase(channelId);
	}

	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> SelectMenuCollector::selectMenuInteractionBuffersMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ButtonCollector::buttonInteractionBuffersMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ModalCollector::modalInteractionBuffersMap{};
	DiscordCoreInternal::TriggerEvent<void, InteractionData> SelectMenuCollector::selectMenuInteractionEventsMap{};
	DiscordCoreInternal::TriggerEvent<void, InteractionData> ButtonCollector::buttonInteractionEventsMap{};
	DiscordCoreInternal::TriggerEvent<void, InteractionData> ModalCollector::modalInteractionEventsMap{};
	DiscordCoreInternal::HttpsClient* Interactions::httpsClient{};
};
