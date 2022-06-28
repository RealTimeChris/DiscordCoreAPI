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
/// WebHookEntities.cpp - The source file for WebHook relate stuff.
/// Dec 1, 2021
/// https://discordcoreapi.com
/// \file WebHookEntities.cpp

#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	ExecuteWebHookData::ExecuteWebHookData(WebHookData dataNew) {
		this->webhookToken = dataNew.token;
		this->webhookId = dataNew.id;
	}

	/// Adds a button to the response Message. \brief Adds a button to the response Message.
	/// \param disabled Whether the button is active or not.
	/// \param customIdNew A custom id to give for identifying the button.
	/// \param buttonLabel A visible label for the button.
	/// \param buttonStyle The style of the button.
	/// \param emojiName An emoji name, if desired.
	/// \param emojiId An emoji id, if desired.
	/// \param url A url, if applicable.
	ExecuteWebHookData& ExecuteWebHookData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
		const std::string& emojiName, uint64_t emojiId, const std::string& url) {
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
	ExecuteWebHookData ExecuteWebHookData::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder,
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
	/// \returns RespondToInputEventData& A reference to this data structure.
	ExecuteWebHookData& ExecuteWebHookData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew,
		bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label, const std::string& placeholder) {
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
	ExecuteWebHookData& ExecuteWebHookData::addFile(File theFile) {
		this->files.push_back(theFile);
		return *this;
	}

	/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
	/// \param dataPackage An AllowedMentionsData structure.
	ExecuteWebHookData& ExecuteWebHookData::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	/// For setting the components in a response. \brief For setting the components in a response.
	/// \param dataPackage An ActionRowData structure.
	ExecuteWebHookData& ExecuteWebHookData::addComponentRow(ActionRowData dataPackage) {
		this->components.push_back(dataPackage);
		return *this;
	}

	/// For setting the embeds in a response. \brief For setting the embeds in a response.
	/// \param dataPackage An EmbedData structure.
	ExecuteWebHookData& ExecuteWebHookData::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.push_back(dataPackage);
		return *this;
	}

	/// For setting the Message content in a response. \brief For setting the content in a response.
	/// \param dataPackage A std::string, containing the content.
	ExecuteWebHookData& ExecuteWebHookData::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	/// For setting the tts status of a response. \brief For setting the tts status of a response.
	/// \param enabledTTs A bool.
	ExecuteWebHookData& ExecuteWebHookData::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	EditWebHookData::EditWebHookData(WebHookData dataNew) {
		this->webhookToken = dataNew.token;
		this->webhookId = dataNew.id;
	}

	WebHook& WebHook::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	WebHook::WebHook(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	WebHookVector::operator std::vector<WebHook>() {
		return this->theWebHooks;
	}

	WebHookVector& WebHookVector::operator=(const nlohmann::json& jsonObjectData) {
		this->parseObject(jsonObjectData, this);
		return *this;
	}

	WebHookVector::WebHookVector(const nlohmann::json& jsonObjectData) {
		*this = jsonObjectData;
	}

	void WebHooks::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		WebHooks::httpsClient = theClient;
	}

	CoRoutine<WebHook> WebHooks::createWebHookAsync(CreateWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Webhook);
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Webhook;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/webhooks";
		workload.callStack = "WebHooks::createWebHookAsync";
		nlohmann::json responseData = { { "name", dataPackage.name } };
		if (dataPackage.avatar.size() > 0) {
			responseData.update({ { "avatar", dataPackage.avatar } });
		}
		workload.content = responseData.dump();
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload);
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getChannelWebHooksAsync(GetChannelWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Webhooks);
		co_await NewThreadAwaitable<std::vector<WebHook>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Webhooks;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + std::to_string(dataPackage.channelId) + "/webhooks";
		workload.callStack = "WebHooks::getChannelWebHooksAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHookVector>(workload);
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getGuildWebHooksAsync(GetGuildWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Webhooks);
		co_await NewThreadAwaitable<std::vector<WebHook>>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Webhooks;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + std::to_string(dataPackage.guildId) + "/webhooks";
		workload.callStack = "WebHooks::getGuildWebHooksAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHookVector>(workload);
	}

	CoRoutine<WebHook> WebHooks::getWebHookAsync(GetWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Webhook);
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Webhook;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId);
		workload.callStack = "WebHooks::getWebHookAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload);
	}

	CoRoutine<WebHook> WebHooks::getWebHookWithTokenAsync(GetWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_With_Token);
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_With_Token;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId) + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::getWebHookWithTokenAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload);
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookAsync(ModifyWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook);
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId);
		nlohmann::json responseData{};
		if (dataPackage.avatar.size() > 0) {
			responseData.update({ { "avatar", dataPackage.avatar } });
		}
		if (dataPackage.name != "") {
			responseData.update({ { "name", dataPackage.name } });
		}
		if (dataPackage.channelId != 0) {
			responseData.update({ { "channel_id", std::to_string(dataPackage.channelId) } });
		}
		workload.content = responseData.dump();
		workload.callStack = "WebHooks::modifyWebHookAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload);
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookWithTokenAsync(ModifyWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_With_Token);
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_With_Token;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId) + "/" + dataPackage.webhookToken;
		nlohmann::json responseData{};
		if (dataPackage.avatar.size() > 0) {
			responseData.update({ { "avatar", dataPackage.avatar } });
		}
		if (dataPackage.name != "") {
			responseData.update({ { "name", dataPackage.name } });
		}
		if (dataPackage.channelId != 0) {
			responseData.update({ { "channel_id", std::to_string(dataPackage.channelId) } });
		}
		workload.content = responseData.dump();
		workload.callStack = "WebHooks::modifyWebHookWithTokenAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload);
	}

	CoRoutine<void> WebHooks::deleteWebHookAsync(DeleteWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId);
		workload.callStack = "WebHooks::deleteWebHookAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<void> WebHooks::deleteWebHookWithTokenAsync(DeleteWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_With_Token);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_With_Token;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId) + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::deleteWebHookWithTokenAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<Message> WebHooks::executeWebHookAsync(ExecuteWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Execute_Webhook);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Execute_Webhook;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId) + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::executeWebHookAsync";
		if (dataPackage.wait) {
			workload.relativePath += "?wait=true";
			if (dataPackage.threadId != 0) {
				workload.relativePath += "&thread_id=" + std::to_string(dataPackage.threadId);
			}
		}
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + std::to_string(dataPackage.threadId);
		}
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(nlohmann::json::parse(DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage)), dataPackage.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage);
		}
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> WebHooks::getWebHookMessageAsync(GetWebHookMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_Message;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId) + "/" + dataPackage.webhookToken + "/messages/" + std::to_string(dataPackage.messageId);
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + std::to_string(dataPackage.threadId);
		}
		workload.callStack = "WebHooks::getWebHookMessageAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> WebHooks::editWebHookMessageAsync(EditWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_Message;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId) + "/" + dataPackage.webhookToken + "/messages/" + std::to_string(dataPackage.messageId);
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + std::to_string(dataPackage.threadId);
		}
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(nlohmann::json::parse(DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage)), dataPackage.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage);
		}
		workload.callStack = "WebHooks::editWebHookMessageAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<void> WebHooks::deleteWebHookMessageAsync(DeleteWebHookMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_Message);
		co_await NewThreadAwaitable<void>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_Message;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.webhookId) + "/" + dataPackage.webhookToken + "/messages/" + std::to_string(dataPackage.messageId);
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + std::to_string(dataPackage.threadId);
		}
		workload.callStack = "WebHooks::deleteWebHookMessageAsync";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}
	DiscordCoreInternal::HttpsClient* WebHooks::httpsClient{ nullptr };
}
