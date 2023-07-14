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
/// WebHookEntities.cpp - The source file for WebHookData relate stuff.
/// Dec 1, 2021
/// https://discordcoreapi.com
/// \file WebHookEntities.cpp

#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::ExecuteWebHookData> {
		using ValueType = DiscordCoreAPI::ExecuteWebHookData;
		static constexpr auto parseValue = object("threadId", &ValueType::threadId, "wait", &ValueType::wait, "attachments", &ValueType::attachments,
			"components", &ValueType::components, "allowedMentions", &ValueType::allowedMentions, "embeds", &ValueType::embeds, "files",
			&ValueType::files, "webhookToken", &ValueType::webhookToken, "avatarUrl", &ValueType::avatarUrl, "username", &ValueType::userName,
			"customId", &ValueType::customId, "webHookId", &ValueType::webHookId, "content", &ValueType::content, "title", &ValueType::title, "flags",
			&ValueType::flags, "tts", &ValueType::tts);
	};

	template<> struct Core<DiscordCoreAPI::CreateWebHookData> {
		using ValueType = DiscordCoreAPI::CreateWebHookData;
		static constexpr auto parseValue = object("channelId", &ValueType::channelId, "avatar", &ValueType::avatar, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::ModifyWebHookData> {
		using ValueType = DiscordCoreAPI::ModifyWebHookData;
		static constexpr auto parseValue =
			object("channelId", &ValueType::channelId, "webHookId", &ValueType::webHookId, "avatar", &ValueType::avatar, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::ModifyWebHookDataWithTokenData> {
		using ValueType = DiscordCoreAPI::ModifyWebHookDataWithTokenData;
		static constexpr auto parseValue = object("webhookToken", &ValueType::webhookToken, "channelId", &ValueType::channelId, "webHookId",
			&ValueType::webHookId, "avatar", &ValueType::avatar, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::EditWebHookData> {
		using ValueType = DiscordCoreAPI::EditWebHookData;
		static constexpr auto parseValue = object("threadId", &ValueType::threadId, "wait", &ValueType::wait, "attachments", &ValueType::attachments,
			"components", &ValueType::components, "allowedMentions", &ValueType::allowedMentions, "embeds", &ValueType::embeds, "files",
			&ValueType::files, "webhookToken", &ValueType::webhookToken, "avatarUrl", &ValueType::avatarUrl, "username", &ValueType::userName,
			"customId", &ValueType::customId, "webHookId", &ValueType::webHookId, "content", &ValueType::content, "title", &ValueType::title, "flags",
			&ValueType::flags, "tts", &ValueType::tts, "message_id", &ValueType::messageId, "thread_id", &ValueType::threadId);
	};
}


namespace DiscordCoreAPI {

	ExecuteWebHookData::ExecuteWebHookData(WebHookData dataNew) {
		webhookToken = dataNew.token;
		webHookId = dataNew.id;
	}

	ExecuteWebHookData& ExecuteWebHookData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type, std::vector<ChannelType> channelTypes) {
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label,
		const std::string& placeholder) {
		title = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			ActionRowData actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
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
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addFile(File theFile) {
		files.emplace_back(theFile);
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addAllowedMentions(AllowedMentionsData dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addComponentRow(ActionRowData dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addMessageEmbed(EmbedData dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addContent(const std::string& dataPackage) {
		content = dataPackage;
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	EditWebHookData::EditWebHookData(WebHookData dataNew) {
		webhookToken = dataNew.token;
		webHookId = dataNew.id;
	}

	void WebHooks::initialize(DiscordCoreInternal::HttpsClient* client) {
		WebHooks::httpsClient = client;
	}

	CoRoutine<WebHookData> WebHooks::createWebHookDataAsync(CreateWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Webhook };
		co_await NewThreadAwaitable<WebHookData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
		workload.callStack = "WebHooks::createWebHookDataAsync()";
		jsonifierCore.serializeJson(dataPackage, workload.content);
		WebHookData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<WebHookData>> WebHooks::getChannelWebHooksAsync(GetChannelWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Webhooks };
		co_await NewThreadAwaitable<std::vector<WebHookData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
		workload.callStack = "WebHooks::getChannelWebHooksAsync()";
		std::vector<WebHookData> returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<WebHookData>> WebHooks::getGuildWebHooksAsync(GetGuildWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Webhooks };
		co_await NewThreadAwaitable<std::vector<WebHookData>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/webhooks";
		workload.callStack = "WebHooks::getGuildWebHooksAsync()";
		std::vector<WebHookData> returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<WebHookData> WebHooks::getWebHookDataAsync(GetWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook };
		co_await NewThreadAwaitable<WebHookData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		workload.callStack = "WebHooks::getWebHookDataAsync()";
		WebHookData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<WebHookData> WebHooks::getWebHookDataWithTokenAsync(GetWebHookDataWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_With_Token };
		co_await NewThreadAwaitable<WebHookData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::getWebHookDataWithTokenAsync()";
		WebHookData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<WebHookData> WebHooks::modifyWebHookDataAsync(ModifyWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook };
		co_await NewThreadAwaitable<WebHookData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "WebHooks::modifyWebHookDataAsync()";
		WebHookData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<WebHookData> WebHooks::modifyWebHookDataWithTokenAsync(ModifyWebHookDataWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_With_Token };
		co_await NewThreadAwaitable<WebHookData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		jsonifierCore.serializeJson(dataPackage, workload.content);
		workload.callStack = "WebHooks::modifyWebHookDataWithTokenAsync()";
		WebHookData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> WebHooks::deleteWebHookDataAsync(DeleteWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		workload.callStack = "WebHooks::deleteWebHookDataAsync()";
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<void> WebHooks::deleteWebHookDataWithTokenAsync(DeleteWebHookDataWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_With_Token };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::deleteWebHookDataWithTokenAsync()";
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	CoRoutine<MessageData> WebHooks::executeWebHookDataAsync(ExecuteWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Execute_Webhook };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::executeWebHookDataAsync()";
		if (dataPackage.wait) {
			workload.relativePath += "?wait=true";
			if (dataPackage.threadId != 0) {
				workload.relativePath += "&thread_id=" + dataPackage.threadId;
			}
		}
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson(dataPackage, workload.content);
			workload.content = constructMultiPartData(workload.content, dataPackage.files);
		} else {
			jsonifierCore.serializeJson(dataPackage, workload.content);
		}
		MessageData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> WebHooks::getWebHookDataMessageAsync(GetWebHookDataMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		workload.callStack = "WebHooks::getWebHookDataMessageAsync()";
		MessageData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<MessageData> WebHooks::editWebHookDataMessageAsync(EditWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_Message };
		co_await NewThreadAwaitable<MessageData>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			jsonifierCore.serializeJson(dataPackage, workload.content);
			workload.content = constructMultiPartData(workload.content, dataPackage.files);
		} else {
			jsonifierCore.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "WebHooks::editWebHookDataMessageAsync()";
		MessageData returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	CoRoutine<void> WebHooks::deleteWebHookDataMessageAsync(DeleteWebHookDataMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		workload.callStack = "WebHooks::deleteWebHookDataMessageAsync()";
		WebHooks::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	DiscordCoreInternal::HttpsClient* WebHooks::httpsClient{};
}
