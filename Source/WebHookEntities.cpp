/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// WebHookEntities.cpp - The source file for WebHook relate stuff.
/// Dec 1, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file WebHookEntities.cpp

#include <WebHookEntities.hpp>
#include <Http.hpp>
#include <DataParsingFunctions.hpp>
#include <CoRoutine.hpp>

namespace DiscordCoreAPI {

	WebHook::WebHook(WebHookData dataNew) {
		this->sourceChannel = dataNew.sourceChannel;
		this->applicationId = dataNew.applicationId;
		this->sourceGuild = dataNew.sourceGuild;
		this->channelId = dataNew.channelId;
		this->guildId = dataNew.guildId;
		this->avatar = dataNew.avatar;
		this->token = dataNew.token;
		this->name = dataNew.name;
		this->type = dataNew.type;
		this->user = dataNew.user;
		this->url = dataNew.url;
		this->id = dataNew.id;
	}

	void WebHooks::initialize(DiscordCoreInternal::HttpClient*theClient) {
		WebHooks::httpClient = theClient;
	}

    CoRoutine<WebHook> WebHooks::createWebHookAsync(CreateWebHookData dataPackage) {
		try {
			co_await NewThreadAwaitable<WebHook>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
			workload.callStack = "WebHooks::createWebHookAsync";
			nlohmann::json responseData = { {"name",dataPackage.name} };
			if (dataPackage.avatar.size() > 0) {
				responseData.update({ {"avatar", dataPackage.avatar} });
			}
			workload.content = responseData.dump();
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::createWebHookAsync()");
		}
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getChannelWebHooksAsync(GetChannelWebHooksData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<WebHook>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Channel_Webhooks;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
			workload.callStack = "WebHooks::getChannelWebHooksAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<WebHook>>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::getChannelWebHooksAsync()");
		}
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getGuildWebHooksAsync(GetGuildWebHooksData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<WebHook>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Webhooks;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/webhooks";
			workload.callStack = "WebHooks::getGuildWebHooksAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<WebHook>>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::getGuildWebHooksAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::getWebHookAsync(GetWebHookData dataPackage) {
		try {
			co_await NewThreadAwaitable<WebHook>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId;
			workload.callStack = "WebHooks::getWebHookAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::getWebHookAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::getWebHookWithTokenAsync(GetWebHookWithTokenData dataPackage) {
		try {
			co_await NewThreadAwaitable<WebHook>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Webhook_With_Token;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken;
			workload.callStack = "WebHooks::getWebHookWithTokenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::getWebHookWithTokenAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookAsync(ModifyWebHookData dataPackage) {
		try {
			co_await NewThreadAwaitable<WebHook>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId;
			nlohmann::json responseData{};
			if (dataPackage.avatar.size() > 0) {
				responseData.update({ {"avatar",dataPackage.avatar} });
			}
			if (dataPackage.name != "") {
				responseData.update({ {"name",dataPackage.name} });
			}
			if (dataPackage.channelId!= "") {
				responseData.update({ {"channel_id",dataPackage.channelId} });
			}
			workload.content = responseData.dump();
			workload.callStack = "WebHooks::modifyWebHookAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::modifyWebHookAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookWithTokenAsync(ModifyWebHookWithTokenData dataPackage) {
		try {
			co_await NewThreadAwaitable<WebHook>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Webhook_With_Token;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken;
			nlohmann::json responseData{};
			if (dataPackage.avatar.size() > 0) {
				responseData.update({ {"avatar",dataPackage.avatar} });
			}
			if (dataPackage.name != "") {
				responseData.update({ {"name",dataPackage.name} });
			}
			if (dataPackage.channelId != "") {
				responseData.update({ {"channel_id",dataPackage.channelId} });
			}
			workload.content = responseData.dump();
			workload.callStack = "WebHooks::modifyWebHookWithTokenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::modifyWebHookWithTokenAsync()");
		}
	}

	CoRoutine<void> WebHooks::deleteWebHookAsync(DeleteWebHookData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId;
			workload.callStack = "WebHooks::deleteWebHookAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::deleteWebHookAsync()");
		}
	}

	CoRoutine<void> WebHooks::deleteWebHookWithTokenAsync(DeleteWebHookWithTokenData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Webhook_With_Token;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken;
			workload.callStack = "WebHooks::deleteWebHookWithTokenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::deleteWebHookWithTokenAsync()");
		}
	}

	CoRoutine<Message> WebHooks::executeWebHookAsync(ExecuteWebHookData dataPackage) {
		try {
			co_await NewThreadAwaitable<Message>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Execute_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken;
			workload.callStack = "WebHooks::executeWebHookAsync";
			if (dataPackage.wait) {
				workload.relativePath += "?wait=true";
				if (dataPackage.threadId != "") {
					workload.relativePath += "&thread_id=" + dataPackage.threadId;
				}
			}
			if (dataPackage.threadId != "") {
				workload.relativePath += "?thread_id=" + dataPackage.threadId;
			}
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::executeWebHookAsync()");
		}
	}

	CoRoutine<Message> WebHooks::getWebHookMessageAsync(GetWebHookMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<Message>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Webhook_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
			if (dataPackage.threadId != "") {
				workload.relativePath += "?thread_id=" + dataPackage.threadId;
			}
			workload.callStack = "WebHooks::getWebHookMessageAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::getWebHookMessageAsync()");
		}
	}

	CoRoutine<Message> WebHooks::editWebHookMessageAsync(EditWebHookMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<Message>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Webhook_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
			if (dataPackage.threadId != "") {
				workload.relativePath += "?thread_id=" + dataPackage.threadId;
			}
			workload.callStack = "WebHooks::editWebHookMessageAsync";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::editWebHookMessageAsync()");
		}
	}

	CoRoutine<void> WebHooks::deleteWebHookMessageAsync(DeleteWebHookMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Webhook_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
			if (dataPackage.threadId != "") {
				workload.relativePath += "?thread_id=" + dataPackage.threadId;
			}
			workload.callStack = "WebHooks::deleteWebHookMessageAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*WebHooks::httpClient, workload);
		}
		catch (...) {
			reportException("WebHooks::deleteWebHookMessageAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* WebHooks::httpClient{ nullptr };
}
