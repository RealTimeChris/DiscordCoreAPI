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
/// WebHookEntities.cpp - the source file for web_hook_data relate stuff.
/// Dec 1, 2021
/// https://discordcoreapi.com
/// \file WebHookEntities.cpp

#include <discordcoreapi/WebHookEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::execute_web_hook_data> {
		using value_type				 = discord_core_api::execute_web_hook_data;
		static constexpr auto parseValue = createValue("threadId", &value_type::threadId, "wait", &value_type::wait, "attachments", &value_type::attachments, "components",
			&value_type::components, "allowedMentions", &value_type::allowedMentions, "embeds", &value_type::embeds, "files", &value_type::files, "webhookToken",
			&value_type::webhookToken, "avatarUrl", &value_type::avatarUrl, "username", &value_type::userName, "customId", &value_type::customId, "webHookId",
			&value_type::webHookId, "content", &value_type::content, "title", &value_type::title, "flags", &value_type::flags, "tts", &value_type::tts);
	};

	template<> struct core<discord_core_api::create_web_hook_data> {
		using value_type				 = discord_core_api::create_web_hook_data;
		static constexpr auto parseValue = createValue("channelId", &value_type::channelId, "avatar", &value_type::avatar, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::modify_web_hook_data> {
		using value_type = discord_core_api::modify_web_hook_data;
		static constexpr auto parseValue =
			createValue("channelId", &value_type::channelId, "webHookId", &value_type::webHookId, "avatar", &value_type::avatar, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::modify_web_hook_with_token_data> {
		using value_type				 = discord_core_api::modify_web_hook_with_token_data;
		static constexpr auto parseValue = createValue("webhookToken", &value_type::webhookToken, "channelId", &value_type::channelId, "webHookId", &value_type::webHookId,
			"avatar", &value_type::avatar, "name", &value_type::name);
	};

	template<> struct core<discord_core_api::edit_web_hook_data> {
		using value_type = discord_core_api::edit_web_hook_data;
		static constexpr auto parseValue =
			createValue("threadId", &value_type::threadId, "wait", &value_type::wait, "attachments", &value_type::attachments, "components", &value_type::components,
				"allowedMentions", &value_type::allowedMentions, "embeds", &value_type::embeds, "files", &value_type::files, "webhookToken", &value_type::webhookToken, "avatarUrl",
				&value_type::avatarUrl, "username", &value_type::userName, "customId", &value_type::customId, "webHookId", &value_type::webHookId, "content", &value_type::content,
				"title", &value_type::title, "flags", &value_type::flags, "tts", &value_type::tts, "message_id", &value_type::messageId, "thread_id", &value_type::threadId);
	};
}


namespace discord_core_api {

	execute_web_hook_data::execute_web_hook_data(const web_hook_data& dataNew) {
		webhookToken = dataNew.token;
		webHookId	 = dataNew.id;
	}

	execute_web_hook_data& execute_web_hook_data::addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, button_style buttonStyle,
		jsonifier::string_view emojiName, snowflake emojiId, jsonifier::string_view url) {
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data component;
				component.type		 = component_type::Button;
				component.emoji.name = emojiName;
				component.label		 = buttonLabel;
				component.style		 = static_cast<uint64_t>(buttonStyle);
				component.customId	 = customIdNew;
				component.disabled	 = disabled;
				component.emoji.id	 = emojiId;
				component.url		 = url;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	execute_web_hook_data& execute_web_hook_data::addSelectMenu(bool disabled, jsonifier::string_view customIdNew, const jsonifier::vector<select_option_data>& options,
		jsonifier::string_view placeholder, int32_t maxValues, int32_t minValues, select_menu_type type, const jsonifier::vector<channel_type>& channelTypes) {
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data componentData;
				componentData.type		   = static_cast<component_type>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder  = placeholder;
				componentData.customId	   = customIdNew;
				componentData.maxValues	   = static_cast<uint64_t>(maxValues);
				componentData.minValues	   = static_cast<uint64_t>(minValues);
				componentData.disabled	   = disabled;
				componentData.options	   = options;
				components[components.size() - 1].components.emplace_back(componentData);
			} else if (components[components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	execute_web_hook_data& execute_web_hook_data::addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
		jsonifier::string_view customIdNew, bool required, int32_t minLength, int32_t maxLength, text_input_style inputStyle, jsonifier::string_view label,
		jsonifier::string_view placeholder) {
		title	 = topTitleNew;
		customId = topCustomIdNew;
		if (components.size() == 0) {
			action_row_data actionRowData;
			components.emplace_back(actionRowData);
		}
		if (components.size() < 5) {
			if (components[components.size() - 1].components.size() < 5) {
				component_data component{};
				component.type		  = component_type::Text_Input;
				component.customId	  = customIdNew;
				component.style		  = static_cast<uint64_t>(inputStyle);
				component.title		  = titleNew;
				component.maxLength	  = static_cast<uint64_t>(maxLength);
				component.minLength	  = static_cast<uint64_t>(minLength);
				component.label		  = label;
				component.required	  = required;
				component.placeholder = placeholder;
				components[components.size() - 1].components.emplace_back(component);
			} else if (components[components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	execute_web_hook_data& execute_web_hook_data::addFile(const file& theFile) {
		files.emplace_back(theFile);
		return *this;

	}
	execute_web_hook_data& execute_web_hook_data::addAllowedMentions(const allowed_mentions_data dataPackage) {
		allowedMentions = dataPackage;
		return *this;
	}

	execute_web_hook_data& execute_web_hook_data::addComponentRow(const action_row_data dataPackage) {
		components.emplace_back(dataPackage);
		return *this;
	}

	execute_web_hook_data& execute_web_hook_data::addMessageEmbed(const embed_data dataPackage) {
		embeds.emplace_back(dataPackage);
		return *this;
	}

	execute_web_hook_data& execute_web_hook_data::addContent(jsonifier::string_view dataPackage) {
		content = dataPackage;
		return *this;
	}

	execute_web_hook_data& execute_web_hook_data::setTTSStatus(bool enabledTTs) {
		tts = enabledTTs;
		return *this;
	}

	edit_web_hook_data::edit_web_hook_data(const web_hook_data& dataNew) {
		webhookToken = dataNew.token;
		webHookId	 = dataNew.id;
	}

	void execute_web_hook_data::generateExcludedKeys() {
		if (attachments.size() == 0) {
			jsonifierExcludedKeys.emplace("attachments");
		}
		if (allowedMentions.parse.size() == 0 && allowedMentions.roles.size() == 0 && allowedMentions.users.size() == 0) {
			jsonifierExcludedKeys.emplace("allowed_mentions");
		}
		if (components.size() == 0) {
			jsonifierExcludedKeys.emplace("components");
		}
		for (auto& value: components) {
			value.generateExcludedKeys();
		}
		if (embeds.size() == 0) {
			jsonifierExcludedKeys.emplace("embeds");
		}
		for (auto& value: embeds) {
			value.generateExcludedKeys();
		}
		if (files.size() == 0) {
			jsonifierExcludedKeys.emplace("files");
		}
		if (avatarUrl == "") {
			jsonifierExcludedKeys.emplace("avatar_url");
		}
		if (customId == "") {
			jsonifierExcludedKeys.emplace("custom_id");
		}
		if (userName == "") {
			jsonifierExcludedKeys.emplace("username");
		}
		if (title == "") {
			jsonifierExcludedKeys.emplace("title");
		}
		if (content == "") {
			jsonifierExcludedKeys.emplace("content");
		}
		return;
	}


	void discord_core_api::web_hooks::initialize(discord_core_internal::https_client* client) {
		discord_core_api::web_hooks::httpsClient = client;
	}

	co_routine<web_hook_data> discord_core_api::web_hooks::createWebHookAsync(create_web_hook_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Webhook };
		co_await newThreadAwaitable<web_hook_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/webhooks";
		workload.callStack	   = "discord_core_api::web_hooks::createWebHookAsync()";
		parser.serializeJson(dataPackage, workload.content);
		web_hook_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<web_hook_data>> discord_core_api::web_hooks::getChannelWebHooksAsync(get_channel_web_hooks_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Channel_Webhooks };
		co_await newThreadAwaitable<jsonifier::vector<web_hook_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/channels/" + dataPackage.channelId + "/webhooks";
		workload.callStack	   = "discord_core_api::web_hooks::getChannelWebHooksAsync()";
		jsonifier::vector<web_hook_data> returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<jsonifier::vector<web_hook_data>> discord_core_api::web_hooks::getGuildWebHooksAsync(get_guild_web_hooks_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Guild_Webhooks };
		co_await newThreadAwaitable<jsonifier::vector<web_hook_data>>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/guilds/" + dataPackage.guildId + "/webhooks";
		workload.callStack	   = "discord_core_api::web_hooks::getGuildWebHooksAsync()";
		jsonifier::vector<web_hook_data> returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<web_hook_data> discord_core_api::web_hooks::getWebHookAsync(get_web_hook_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Webhook };
		co_await newThreadAwaitable<web_hook_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId;
		workload.callStack	   = "discord_core_api::web_hooks::getWebHookAsync()";
		web_hook_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<web_hook_data> discord_core_api::web_hooks::getWebHookWithTokenAsync(get_web_hook_with_token_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Webhook_With_Token };
		co_await newThreadAwaitable<web_hook_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack	   = "discord_core_api::web_hooks::getWebHookWithTokenAsync()";
		web_hook_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<web_hook_data> discord_core_api::web_hooks::modifyWebHookAsync(modify_web_hook_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Webhook };
		co_await newThreadAwaitable<web_hook_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "discord_core_api::web_hooks::modifyWebHookAsync()";
		web_hook_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<web_hook_data> discord_core_api::web_hooks::modifyWebHookWithTokenAsync(modify_web_hook_with_token_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Webhook_With_Token };
		co_await newThreadAwaitable<web_hook_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "discord_core_api::web_hooks::modifyWebHookWithTokenAsync()";
		web_hook_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> discord_core_api::web_hooks::deleteWebHookAsync(delete_web_hook_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Webhook };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId;
		workload.callStack	   = "discord_core_api::web_hooks::deleteWebHookAsync()";
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<void> discord_core_api::web_hooks::deleteWebHookWithTokenAsync(delete_web_hook_with_token_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Webhook_With_Token };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack	   = "discord_core_api::web_hooks::deleteWebHookWithTokenAsync()";
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<message_data> discord_core_api::web_hooks::executeWebHookAsync(execute_web_hook_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Execute_Webhook };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken;
		workload.callStack	   = "discord_core_api::web_hooks::executeWebHookAsync()";
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
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
			parser.serializeJson(dataPackage, workload.content);
			workload.content = constructMultiPartData(workload.content, dataPackage.files);
		} else {
			parser.serializeJson(dataPackage, workload.content);
		}
		message_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> discord_core_api::web_hooks::getWebHookMessageAsync(get_web_hook_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Webhook_Message };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		workload.callStack = "discord_core_api::web_hooks::getWebHookMessageAsync()";
		message_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> discord_core_api::web_hooks::editWebHookMessageAsync(edit_web_hook_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Webhook_Message };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
			parser.serializeJson(dataPackage, workload.content);
			workload.content = constructMultiPartData(workload.content, dataPackage.files);
		} else {
			parser.serializeJson(dataPackage, workload.content);
		}
		workload.callStack = "discord_core_api::web_hooks::editWebHookMessageAsync()";
		message_data returnData{};
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> discord_core_api::web_hooks::deleteWebHookMessageAsync(delete_web_hook_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Webhook_Message };
		co_await newThreadAwaitable<void>();
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/webhooks/" + dataPackage.webHookId + "/" + dataPackage.webhookToken + "/messages/" + dataPackage.messageId;
		if (dataPackage.threadId != 0) {
			workload.relativePath += "?thread_id=" + dataPackage.threadId;
		}
		workload.callStack = "discord_core_api::web_hooks::deleteWebHookMessageAsync()";
		discord_core_api::web_hooks::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	discord_core_internal::https_client* discord_core_api::web_hooks::httpsClient{};
}
