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
/// WebHookEntities.cpp - The source file for WebHook relate stuff.
/// Dec 1, 2021
/// https://discordcoreapi.com
/// \file WebHookEntities.cpp

#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/Https.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::WebHook> {
		using ValueType = DiscordCoreAPI::WebHook;
		static constexpr auto parseValue = Jsonifier::object("id", &ValueType::id, "type", &ValueType::type, "guild_id", &ValueType::guildId,
			"channel_id", &ValueType::channelId, "name", &ValueType::name, "token", &ValueType::token, "avatar", &ValueType::avatar, "source_guild",
			&ValueType::sourceGuild, "source_channel", &ValueType::sourceChannel, "user", &ValueType::user, "url", &ValueType::url);
	};

	template<> struct Core<DiscordCoreAPI::ExecuteWebHookData> {
		using ValueType = DiscordCoreAPI::ExecuteWebHookData;
		static constexpr auto parseValue = object("threadId", &ValueType::threadId, "wait", &ValueType::wait, "attachments", &ValueType::attachments,
			"components", &ValueType::components, "allowedMentions", &ValueType::allowedMentions, "embeds", &ValueType::embeds, "files",
			&ValueType::files, "webhookToken", &ValueType::webhookToken, "avatarUrl", &ValueType::avatarUrl, "userName", &ValueType::userName,
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

	template<> struct Core<DiscordCoreAPI::ModifyWebHookWithTokenData> {
		using ValueType = DiscordCoreAPI::ModifyWebHookWithTokenData;
		static constexpr auto parseValue = object("webhookToken", &ValueType::webhookToken, "channelId", &ValueType::channelId, "webHookId",
			&ValueType::webHookId, "avatar", &ValueType::avatar, "name", &ValueType::name);
	};

	template<> struct Core<DiscordCoreAPI::EditWebHookData> {
		using ValueType = DiscordCoreAPI::EditWebHookData;
		static constexpr auto parseValue = object("threadId", &ValueType::threadId, "wait", &ValueType::wait, "attachments", &ValueType::attachments,
			"components", &ValueType::components, "allowedMentions", &ValueType::allowedMentions, "embeds", &ValueType::embeds, "files",
			&ValueType::files, "webhookToken", &ValueType::webhookToken, "avatarUrl", &ValueType::avatarUrl, "userName", &ValueType::userName,
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

	CoRoutine<WebHook> WebHooks::createWebHookAsync(CreateWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Webhook };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
		workload.callStack = "WebHooks::createWebHookAsync()";
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		WebHook returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getChannelWebHooksAsync(GetChannelWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Webhooks };
		co_await NewThreadAwaitable<std::vector<WebHook>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
		workload.callStack = "WebHooks::getChannelWebHooksAsync()";
		WebHookVector returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<WebHookVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getGuildWebHooksAsync(GetGuildWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Webhooks };
		co_await NewThreadAwaitable<std::vector<WebHook>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/webhooks";
		workload.callStack = "WebHooks::getGuildWebHooksAsync()";
		WebHookVector returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<WebHookVector>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<WebHook> WebHooks::getWebHookAsync(GetWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		workload.callStack = "WebHooks::getWebHookAsync()";
		WebHook returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<WebHook> WebHooks::getWebHookWithTokenAsync(GetWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_With_Token };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::getWebHookWithTokenAsync()";
		WebHook returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookAsync(ModifyWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "WebHooks::modifyWebHookAsync()";
		WebHook returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookWithTokenAsync(ModifyWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_With_Token };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		Jsonifier::JsonifierCore parser{};
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "WebHooks::modifyWebHookWithTokenAsync()";
		WebHook returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> WebHooks::deleteWebHookAsync(DeleteWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		workload.callStack = "WebHooks::deleteWebHookAsync()";
		WebHooks::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<void> WebHooks::deleteWebHookWithTokenAsync(DeleteWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_With_Token };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::deleteWebHookWithTokenAsync()";
		WebHooks::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	CoRoutine<Message> WebHooks::executeWebHookAsync(ExecuteWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Post_Execute_Webhook };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::executeWebHookAsync()";
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
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
			workload.content = constructMultiPartData(workload.content, dataPackage.files);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		}
		Message returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> WebHooks::getWebHookMessageAsync(GetWebHookMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		workload.callStack = "WebHooks::getWebHookMessageAsync()";
		Message returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<Message> WebHooks::editWebHookMessageAsync(EditWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_Message };
		co_await NewThreadAwaitable<Message>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
			workload.content = constructMultiPartData(workload.content, dataPackage.files);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "WebHooks::editWebHookMessageAsync()";
		Message returnData{};
		WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
		co_return returnData;
	}

	CoRoutine<void> WebHooks::deleteWebHookMessageAsync(DeleteWebHookMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_Message };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		workload.callStack = "WebHooks::deleteWebHookMessageAsync()";
		WebHooks::httpsClient->submitWorkloadAndGetResult<void>(workload);
		co_return;
	}

	DiscordCoreInternal::HttpsClient* WebHooks::httpsClient{};
}
