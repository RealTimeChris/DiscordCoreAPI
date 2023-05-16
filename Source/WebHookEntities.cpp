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
/*
template<> struct Jsonifier::Core<DiscordCoreAPI::WebHook> {
	using OTy2 = DiscordCoreAPI::WebHook;
	static constexpr auto parseValue =
		Jsonifier::object("id", &OTy2::id, "type", &OTy2::type, "guild_id", &OTy2::guildId, "channel_id", &OTy2::channelId, "name", &OTy2::name, "token", &OTy2::token,
			"avatar", &OTy2::avatar, "source_guild", &OTy2::sourceGuild, "source_channel", &OTy2::sourceChannel, "user", &OTy2::user, "url", &OTy2::url);
};

namespace DiscordCoreAPI {

	ExecuteWebHookData::ExecuteWebHookData(WebHookData dataNew) {
		this->webhookToken = dataNew.token;
		this->webHookId = dataNew.id;
	}

	ExecuteWebHookData::operator EtfSerializer() {
		EtfSerializer data{};
		if (this->allowedMentions.parse.size() > 0 || this->allowedMentions.roles.size() > 0 || this->allowedMentions.users.size() > 0) {
			data["allowed_mentions"] = this->allowedMentions.operator EtfSerializer();
		}
		for (auto& value: this->attachments) {
			data["attachments"].emplaceBack(value.operator EtfSerializer());
		}
		if (this->components.size() == 0) {
			data["components"].emplaceBack(ActionRowData{});
			data["components"].getValue<EtfSerializer::ArrayType>().clear();
		} else {
			for (auto& value: this->components) {
				data["components"].emplaceBack(value.operator EtfSerializer());
			}
		}
		if (this->embeds.size() == 0) {
			data["embeds"].emplaceBack(EmbedData{});
			data["embeds"].getValue<EtfSerializer::ArrayType>().clear();
		} else {
			for (auto& value: this->embeds) {
				data["embeds"].emplaceBack(value.operator EtfSerializer());
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
		return data;
	}

	ExecuteWebHookData& ExecuteWebHookData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
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
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type, std::vector<ChannelType> channelTypes) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				this->components[this->components.size() - 1].components.emplace_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label,
		const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
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
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addFile(File theFile) {
		this->files.emplace_back(theFile);
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addComponentRow(ActionRowData dataPackage) {
		this->components.emplace_back(dataPackage);
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.emplace_back(dataPackage);
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	ExecuteWebHookData& ExecuteWebHookData::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	EditWebHookData::EditWebHookData(WebHookData dataNew) {
		this->webhookToken = dataNew.token;
		this->webHookId = dataNew.id;
	}

	EditWebHookData::operator EtfSerializer() {
		EtfSerializer data{};
		if (this->allowedMentions.parse.size() > 0 || this->allowedMentions.roles.size() > 0 || this->allowedMentions.users.size() > 0) {
			data["allowed_mentions"] = this->allowedMentions.operator EtfSerializer();
		}
		for (auto& value: this->attachments) {
			data["attachments"].emplaceBack(value.operator EtfSerializer());
		}
		if (this->components.size() == 0) {
			data["components"].emplaceBack(ActionRowData{});
			data["components"].getValue<EtfSerializer::ArrayType>().clear();
		} else {
			for (auto& value: this->components) {
				data["components"].emplaceBack(value.operator EtfSerializer());
			}
		}
		if (this->embeds.size() == 0) {
			data["embeds"].emplaceBack(EmbedData{});
			data["embeds"].getValue<EtfSerializer::ArrayType>().clear();
		} else {
			for (auto& value: this->embeds) {
				data["embeds"].emplaceBack(value.operator EtfSerializer());
			}
		}
		if (this->content != "") {
			data["content"] = this->content;
		}
		return data;
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
		EtfSerializer responseData{};
		if (dataPackage.avatar.size() > 0) {
			responseData["avatar"] = dataPackage.avatar;
		}
		if (dataPackage.name != "") {
			responseData["name"] = dataPackage.name;
		}
		responseData.refreshString(SerializerSerializeType::Etf);
		workload.content = responseData.operator std::string();
		WebHook returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getChannelWebHooksAsync(GetChannelWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Channel_Webhooks };
		co_await NewThreadAwaitable<std::vector<WebHook>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
		workload.callStack = "WebHooks::getChannelWebHooksAsync()";
		WebHookVector returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHookVector>(workload, returnData);
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getGuildWebHooksAsync(GetGuildWebHooksData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Guild_Webhooks };
		co_await NewThreadAwaitable<std::vector<WebHook>>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/guilds/" + dataPackage.guildId + "/webhooks";
		workload.callStack = "WebHooks::getGuildWebHooksAsync()";
		WebHookVector returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHookVector>(workload, returnData);
	}

	CoRoutine<WebHook> WebHooks::getWebHookAsync(GetWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		workload.callStack = "WebHooks::getWebHookAsync()";
		WebHook returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
	}

	CoRoutine<WebHook> WebHooks::getWebHookWithTokenAsync(GetWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Get_Webhook_With_Token };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::getWebHookWithTokenAsync()";
		WebHook returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookAsync(ModifyWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		EtfSerializer responseData{};
		if (dataPackage.avatar.size() > 0) {
			responseData["avatar"] = dataPackage.avatar;
		}
		if (dataPackage.name != "") {
			responseData["name"] = dataPackage.name;
		}
		if (dataPackage.channelId != 0) {
			responseData["channel_id"] = dataPackage.channelId;
		}
		responseData.refreshString(SerializerSerializeType::Etf);
		workload.content = responseData.operator std::string();
		workload.callStack = "WebHooks::modifyWebHookAsync()";
		WebHook returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookWithTokenAsync(ModifyWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Patch_Webhook_With_Token };
		co_await NewThreadAwaitable<WebHook>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		EtfSerializer responseData{};
		if (dataPackage.avatar.size() > 0) {
			responseData["avatar"] = dataPackage.avatar;
		}
		if (dataPackage.name != "") {
			responseData["name"] = dataPackage.name;
		}
		if (dataPackage.channelId != 0) {
			responseData["channel_id"] = dataPackage.channelId;
		}
		responseData.refreshString(SerializerSerializeType::Etf);
		workload.content = responseData.operator std::string();
		workload.callStack = "WebHooks::modifyWebHookWithTokenAsync()";
		WebHook returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<WebHook>(workload, returnData);
	}

	CoRoutine<void> WebHooks::deleteWebHookAsync(DeleteWebHookData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId;
		workload.callStack = "WebHooks::deleteWebHookAsync()";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult(workload);
	}

	CoRoutine<void> WebHooks::deleteWebHookWithTokenAsync(DeleteWebHookWithTokenData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{ DiscordCoreInternal::HttpsWorkloadType::Delete_Webhook_With_Token };
		co_await NewThreadAwaitable<void>();
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack = "WebHooks::deleteWebHookWithTokenAsync()";
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult(workload);
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
			parser.serializeJson(dataPackage, workload.content);workload.content = constructMultiPartData(serializer.operator std::string(), dataPackage.files);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		}
		Message returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
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
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
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
			parser.serializeJson(dataPackage, workload.content);workload.content = constructMultiPartData(serializer.operator std::string(), dataPackage.files);
		} else {
			Jsonifier::JsonifierCore parser{};
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "WebHooks::editWebHookMessageAsync()";
		Message returnData{};
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult<Message>(workload, returnData);
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
		co_return WebHooks::httpsClient->submitWorkloadAndGetResult(workload);
	}
	DiscordCoreInternal::HttpsClient* WebHooks::httpsClient{ nullptr };
}*/
