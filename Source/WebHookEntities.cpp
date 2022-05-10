/*
*
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
/// Chris M.
/// https://discordcoreapi.com
/// \file WebHookEntities.cpp

#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	void WebHooks::initialize(DiscordCoreInternal::HttpClient* theClient) {
		WebHooks::httpClient = theClient;
	}

	CoRoutine<WebHook> WebHooks::createWebHookAsync(CreateWebHookData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Webhook);
			co_await NewThreadAwaitable<WebHook>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
			workload.callStack = "WebHooks::createWebHookAsync";
			nlohmann::json responseData = { { "name", dataPackage.name } };
			if (dataPackage.avatar.size() > 0) {
				responseData.update({ { "avatar", dataPackage.avatar } });
			}
			workload.content = responseData.dump();
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::createWebHookAsync()");
		}
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getChannelWebHooksAsync(GetChannelWebHooksData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Channel_Webhooks);
			co_await NewThreadAwaitable<std::vector<WebHook>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Channel_Webhooks;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/webhooks";
			workload.callStack = "WebHooks::getChannelWebHooksAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<WebHook>>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::getChannelWebHooksAsync()");
		}
	}

	CoRoutine<std::vector<WebHook>> WebHooks::getGuildWebHooksAsync(GetGuildWebHooksData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Guild_Webhooks);
			co_await NewThreadAwaitable<std::vector<WebHook>>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Webhooks;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/webhooks";
			workload.callStack = "WebHooks::getGuildWebHooksAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<WebHook>>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::getGuildWebHooksAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::getWebHookAsync(GetWebHookData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Webhook);
			co_await NewThreadAwaitable<WebHook>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId;
			workload.callStack = "WebHooks::getWebHookAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::getWebHookAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::getWebHookWithTokenAsync(GetWebHookWithTokenData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Webhook_With_Token);
			co_await NewThreadAwaitable<WebHook>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Webhook_With_Token;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken;
			workload.callStack = "WebHooks::getWebHookWithTokenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::getWebHookWithTokenAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookAsync(ModifyWebHookData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Webhook);
			co_await NewThreadAwaitable<WebHook>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId;
			nlohmann::json responseData{};
			if (dataPackage.avatar.size() > 0) {
				responseData.update({ { "avatar", dataPackage.avatar } });
			}
			if (dataPackage.name != "") {
				responseData.update({ { "name", dataPackage.name } });
			}
			if (dataPackage.channelId != "") {
				responseData.update({ { "channel_id", dataPackage.channelId } });
			}
			workload.content = responseData.dump();
			workload.callStack = "WebHooks::modifyWebHookAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::modifyWebHookAsync()");
		}
	}

	CoRoutine<WebHook> WebHooks::modifyWebHookWithTokenAsync(ModifyWebHookWithTokenData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Webhook_With_Token);
			co_await NewThreadAwaitable<WebHook>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Webhook_With_Token;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken;
			nlohmann::json responseData{};
			if (dataPackage.avatar.size() > 0) {
				responseData.update({ { "avatar", dataPackage.avatar } });
			}
			if (dataPackage.name != "") {
				responseData.update({ { "name", dataPackage.name } });
			}
			if (dataPackage.channelId != "") {
				responseData.update({ { "channel_id", dataPackage.channelId } });
			}
			workload.content = responseData.dump();
			workload.callStack = "WebHooks::modifyWebHookWithTokenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<WebHook>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::modifyWebHookWithTokenAsync()");
		}
	}

	CoRoutine<void> WebHooks::deleteWebHookAsync(DeleteWebHookData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Webhook);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Webhook;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId;
			workload.callStack = "WebHooks::deleteWebHookAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::deleteWebHookAsync()");
		}
	}

	CoRoutine<void> WebHooks::deleteWebHookWithTokenAsync(DeleteWebHookWithTokenData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Webhook_With_Token);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Webhook_With_Token;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken;
			workload.callStack = "WebHooks::deleteWebHookWithTokenAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::deleteWebHookWithTokenAsync()");
		}
	}

	CoRoutine<Message> WebHooks::executeWebHookAsync(ExecuteWebHookData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Execute_Webhook);
			co_await NewThreadAwaitable<Message>();
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
			if (dataPackage.files.size() > 0) {
				constructMultiPartData(workload, nlohmann::json::parse(DiscordCoreInternal::JSONIFY(dataPackage)), dataPackage.files);
			} else {
				workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::executeWebHookAsync()");
		}
	}

	CoRoutine<Message> WebHooks::getWebHookMessageAsync(GetWebHookMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Webhook_Message);
			co_await NewThreadAwaitable<Message>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Webhook_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
			if (dataPackage.threadId != "") {
				workload.relativePath += "?thread_id=" + dataPackage.threadId;
			}
			workload.callStack = "WebHooks::getWebHookMessageAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::getWebHookMessageAsync()");
		}
	}

	CoRoutine<Message> WebHooks::editWebHookMessageAsync(EditWebHookData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Webhook_Message);
			co_await NewThreadAwaitable<Message>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Webhook_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
			if (dataPackage.threadId != "") {
				workload.relativePath += "?thread_id=" + dataPackage.threadId;
			}
			if (dataPackage.files.size() > 0) {
				constructMultiPartData(workload, nlohmann::json::parse(DiscordCoreInternal::JSONIFY(dataPackage)), dataPackage.files);
			} else {
				workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
			}
			workload.callStack = "WebHooks::editWebHookMessageAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::editWebHookMessageAsync()");
		}
	}

	CoRoutine<void> WebHooks::deleteWebHookMessageAsync(DeleteWebHookMessageData dataPackage) {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Webhook_Message);
			co_await NewThreadAwaitable<void>();
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Webhook_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/webhooks/" + dataPackage.webhookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
			if (dataPackage.threadId != "") {
				workload.relativePath += "?thread_id=" + dataPackage.threadId;
			}
			workload.callStack = "WebHooks::deleteWebHookMessageAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*WebHooks::httpClient, workload);
		} catch (...) {
			reportException("WebHooks::deleteWebHookMessageAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* WebHooks::httpClient{ nullptr };
}
