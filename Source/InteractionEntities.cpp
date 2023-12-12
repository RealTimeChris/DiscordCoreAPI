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
/// InteractionEntities.cpp - Source file for the interaction_data related classes and structs.
/// May 28, 2021
/// https://discordcoreapi.com
/// \file InteractionEntities.cpp

#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::interaction_callback_data> {
		using value_type				 = discord_core_api::interaction_callback_data;
		static constexpr auto parseValue = createValue("attachments", &value_type::attachments, "choices", &value_type::choices, "components", &value_type::components, "content",
			&value_type::content, "custom_id", &value_type::customId, "embeds", &value_type::embeds, "files", &value_type::files, "flags", &value_type::flags, "tts",
			&value_type::tts, "allowed_mentions", &value_type::allowedMentions, "title", &value_type::title);
	};

	template<> struct core<discord_core_api::interaction_response_base> {
		using value_type				 = discord_core_api::interaction_response_base;
		static constexpr auto parseValue = createValue("type", &value_type::type, "data", &value_type::data);
	};

	template<> struct core<discord_core_api::create_interaction_response_data> {
		using value_type				 = discord_core_api::create_interaction_response_data;
		static constexpr auto parseValue = createValue("type", &value_type::type, "data", &value_type::data);
	};

	template<> struct core<discord_core_api::edit_interaction_response_data> {
		using value_type				 = discord_core_api::edit_interaction_response_data;
		static constexpr auto parseValue = createValue("content", &value_type::content, "embeds", &value_type::embeds, "allowed_mentions", &value_type::allowedMentions,
			"components", &value_type::components, "files", &value_type::files, "attachments", &value_type::attachments);
	};

	template<> struct core<discord_core_api::create_follow_up_message_data> {
		using value_type				 = discord_core_api::create_follow_up_message_data;
		static constexpr auto parseValue = createValue("content", &value_type::content, "username", &value_type::userName, "avatar_url", &value_type::avatarUrl, "tts",
			&value_type::tts, "embeds", &value_type::embeds, "allowed_mentions", &value_type::allowedMentions, "components", &value_type::components, "files", &value_type::files,
			"attachments", &value_type::attachments, "flags", &value_type::flags);
	};

	template<> struct core<discord_core_api::edit_follow_up_message_data> {
		using value_type				 = discord_core_api::edit_follow_up_message_data;
		static constexpr auto parseValue = createValue("content", &value_type::content, "username", &value_type::userName, "avatar_url", &value_type::avatarUrl, "tts",
			&value_type::tts, "embeds", &value_type::embeds, "allowed_mentions", &value_type::allowedMentions, "components", &value_type::components, "files", &value_type::files,
			"attachments", &value_type::attachments, "flags", &value_type::flags);
	};

}

namespace discord_core_api {

	interaction_response_base& interaction_response_base::addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, button_style buttonStyle,
		jsonifier::string_view emojiName, snowflake emojiId, jsonifier::string_view url) {
		if (data.components.size() == 0) {
			action_row_data actionRowData;
			data.components.emplace_back(actionRowData);
		}
		if (data.components.size() < 5) {
			if (data.components[data.components.size() - 1].components.size() < 5) {
				component_data component;
				component.type		 = component_type::Button;
				component.emoji.name = emojiName;
				component.label		 = buttonLabel;
				component.style		 = static_cast<uint64_t>(buttonStyle);
				component.customId	 = customIdNew;
				component.disabled	 = disabled;
				component.emoji.id	 = emojiId;
				component.url		 = url;
				data.components[data.components.size() - 1].components.emplace_back(component);
			} else if (data.components[data.components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	interaction_response_base& interaction_response_base::addSelectMenu(bool disabled, jsonifier::string_view customIdNew, jsonifier::vector<select_option_data> options,
		jsonifier::string_view placeholder, uint64_t maxValues, uint64_t minValues, select_menu_type typeNew, jsonifier::vector<channel_type> channelTypes) {
		if (data.components.size() == 0) {
			action_row_data actionRowData;
			data.components.emplace_back(actionRowData);
		}
		if (data.components.size() < 5) {
			if (data.components[data.components.size() - 1].components.size() < 5) {
				component_data componentData;
				componentData.type		   = static_cast<component_type>(typeNew);
				componentData.channelTypes = channelTypes;
				componentData.placeholder  = placeholder;
				componentData.customId	   = customIdNew;
				componentData.maxValues	   = maxValues;
				componentData.minValues	   = minValues;
				componentData.disabled	   = disabled;
				componentData.options	   = options;
				data.components[data.components.size() - 1].components.emplace_back(componentData);
			} else if (data.components[data.components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	interaction_response_base& interaction_response_base::addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
		jsonifier::string_view customIdNew, bool required, uint64_t minLength, uint64_t maxLength, text_input_style inputStyle, jsonifier::string_view label,
		jsonifier::string_view placeholder) {
		data.title	  = topTitleNew;
		data.customId = topCustomIdNew;
		if (data.components.size() == 0) {
			action_row_data actionRowData;
			data.components.emplace_back(actionRowData);
		}
		if (data.components.size() < 5) {
			if (data.components[data.components.size() - 1].components.size() < 5) {
				component_data component{};
				component.type		  = component_type::Text_Input;
				component.customId	  = customIdNew;
				component.style		  = static_cast<uint64_t>(inputStyle);
				component.title		  = titleNew;
				component.maxLength	  = maxLength;
				component.minLength	  = minLength;
				component.label		  = label;
				component.required	  = required;
				component.placeholder = placeholder;
				data.components[data.components.size() - 1].components.emplace_back(component);
			} else if (data.components[data.components.size() - 1].components.size() == 5) {
				action_row_data actionRowData;
				data.components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	interaction_response_base& interaction_response_base::addFile(const file& theFile) {
		data.files.emplace_back(theFile);
		return *this;
	}

	interaction_response_base& interaction_response_base::addAllowedMentions(const allowed_mentions_data dataPackage) {
		data.allowedMentions = dataPackage;
		return *this;
	}

	interaction_response_base& interaction_response_base::addComponentRow(const action_row_data dataPackage) {
		data.components.emplace_back(dataPackage);
		return *this;
	}

	interaction_response_base& interaction_response_base::setResponseType(interaction_callback_type typeNew) {
		type = typeNew;
		return *this;
	}

	void interaction_response_base::generateExcludedKeys() {
		data.jsonifierExcludedKeys.clear();
		data.generateExcludedKeys();
		return;
	}

	interaction_response_base& interaction_response_base::addMessageEmbed(const embed_data dataPackage) {
		data.embeds.emplace_back(dataPackage);
		return *this;
	}

	interaction_response_base& interaction_response_base::addContent(jsonifier::string_view dataPackage) {
		data.content = dataPackage;
		return *this;
	}

	interaction_response_base& interaction_response_base::setTTSStatus(bool enabledTTs) {
		data.tts = enabledTTs;
		return *this;
	}

	interaction_response_base& interaction_response_base::setFlags(uint64_t flagsNew) {
		data.flags = flagsNew;
		return *this;
	}

	interaction_response_data interaction_response_base::getInteractionResponseData() {
		interaction_response_data returnData;
		returnData.data = data;
		returnData.type = type;
		return returnData;
	}

	void interactions::initialize(discord_core_internal::https_client* client) {
		interactions::httpsClient = client;
	}

	create_ephemeral_interaction_response_data::create_ephemeral_interaction_response_data(const respond_to_input_event_data dataPackage) {
		data = dataPackage;
		if (dataPackage.eventType == interaction_type::Message_Component) {
			type = interaction_callback_type::Update_Message;
		} else {
			type = interaction_callback_type::Channel_Message_With_Source;
		}
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
		data.flags							= 64;
	}

	create_deferred_interaction_response_data::create_deferred_interaction_response_data(const respond_to_input_event_data dataPackage) {
		data = dataPackage;
		if (dataPackage.eventType == interaction_type::Message_Component) {
			type = interaction_callback_type::Deferred_Update_Message;
		} else {
			type = interaction_callback_type::Deferred_Channel_Message_With_Source;
		}
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
	}

	create_interaction_response_data::create_interaction_response_data(const create_deferred_interaction_response_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.interactionPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionPackage.interactionId;
		data.components						= dataPackage.data.components;
		type								= dataPackage.type;
		data								= dataPackage.data;
	}

	create_interaction_response_data::create_interaction_response_data(const create_ephemeral_interaction_response_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.interactionPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionPackage.interactionId;
		data.components						= dataPackage.data.components;
		type								= dataPackage.type;
		data								= dataPackage.data;
		data.flags							= 64;
	}

	create_interaction_response_data::create_interaction_response_data(const respond_to_input_event_data dataPackage) {
		data = dataPackage;
		if (dataPackage.eventType == interaction_type::Message_Component && dataPackage.type == input_event_response_type::Deferred_Response) {
			type = interaction_callback_type::Deferred_Update_Message;
		} else if (dataPackage.eventType == interaction_type::Message_Component) {
			type = interaction_callback_type::Update_Message;
		} else if (dataPackage.eventType == interaction_type::Application_Command_Autocomplete ||
			dataPackage.type == input_event_response_type::Application_Command_AutoComplete_Result) {
			type = interaction_callback_type::Application_Command_Autocomplete_Result;
		} else {
			type = interaction_callback_type::Channel_Message_With_Source;
		}
		if (dataPackage.type == input_event_response_type::Modal_Interaction_Response || dataPackage.title != "") {
			type = interaction_callback_type::Modal;
		}
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
		data.files							= dataPackage.files;
	}

	create_interaction_response_data::create_interaction_response_data(const interaction_data dataPackage) {
		if (dataPackage.type == interaction_type::Message_Component) {
			type = interaction_callback_type::Update_Message;
		} else {
			type = interaction_callback_type::Channel_Message_With_Source;
		}
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionToken = dataPackage.token;
		interactionPackage.interactionId	= dataPackage.id;
	}

	edit_interaction_response_data::edit_interaction_response_data(const respond_to_input_event_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
		allowedMentions						= dataPackage.allowedMentions;
		components							= dataPackage.components;
		content								= dataPackage.content;
		embeds								= dataPackage.embeds;
		title								= dataPackage.title;
		flags								= dataPackage.flags;
		files								= dataPackage.files;
		tts									= dataPackage.tts;
	}

	create_ephemeral_follow_up_message_data::create_ephemeral_follow_up_message_data(const respond_to_input_event_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
		allowedMentions						= dataPackage.allowedMentions;
		components							= dataPackage.components;
		content								= dataPackage.content;
		embeds								= dataPackage.embeds;
		files								= dataPackage.files;
		tts									= dataPackage.tts;
		flags								= 64;
	}

	create_follow_up_message_data::create_follow_up_message_data(const create_ephemeral_follow_up_message_data dataPackage) {
		interactionPackage = dataPackage.interactionPackage;
		allowedMentions	   = dataPackage.allowedMentions;
		components		   = dataPackage.components;
		content			   = dataPackage.content;
		embeds			   = dataPackage.embeds;
		flags			   = dataPackage.flags;
		files			   = dataPackage.files;
		tts				   = dataPackage.tts;
		flags			   = 64;
	}

	create_follow_up_message_data::create_follow_up_message_data(const respond_to_input_event_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
		allowedMentions						= dataPackage.allowedMentions;
		components							= dataPackage.components;
		content								= dataPackage.content;
		embeds								= dataPackage.embeds;
		flags								= dataPackage.flags;
		files								= dataPackage.files;
		tts									= dataPackage.tts;
	}

	edit_follow_up_message_data::edit_follow_up_message_data(const respond_to_input_event_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		allowedMentions						= dataPackage.allowedMentions;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
		messagePackage.channelId			= dataPackage.channelId;
		messagePackage.messageId			= dataPackage.messageId;
		components							= dataPackage.components;
		content								= dataPackage.content;
		embeds								= dataPackage.embeds;
		files								= dataPackage.files;
	}

	delete_follow_up_message_data::delete_follow_up_message_data(const respond_to_input_event_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
		messagePackage.channelId			= dataPackage.channelId;
		messagePackage.messageId			= dataPackage.messageId;
	}

	delete_interaction_response_data::delete_interaction_response_data(const respond_to_input_event_data dataPackage) {
		interactionPackage.interactionToken = dataPackage.interactionToken;
		interactionPackage.applicationId	= dataPackage.applicationId;
		interactionPackage.interactionId	= dataPackage.interactionId;
	}

	co_routine<message_data> interactions::createInteractionResponseAsync(create_interaction_response_data dataPackageNew) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Interaction_Response };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		auto dataPackage{ dataPackageNew };
		workload.relativePath = "/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		dataPackage.generateExcludedKeys();
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::createInteractionResponseAsync()";
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		get_interaction_response_data dataPackage01{};
		dataPackage01.applicationId	   = dataPackage.interactionPackage.applicationId;
		dataPackage01.interactionToken = dataPackage.interactionPackage.interactionToken;
		if (dataPackage.type != interaction_callback_type::Application_Command_Autocomplete_Result) {
			co_return interactions::getInteractionResponseAsync(dataPackage01).get();
		} else {
			co_return message_data{};
		}
	}

	message_data interactions::getInteractionResponse(get_interaction_response_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Interaction_Response };
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
		workload.callStack	   = "interactions::getInteractionResponseAsync()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		return returnData;
	}


	co_routine<message_data> interactions::getInteractionResponseAsync(get_interaction_response_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Interaction_Response };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
		workload.callStack	   = "interactions::getInteractionResponseAsync()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> interactions::editInteractionResponseAsync(edit_interaction_response_data dataPackageNew) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Interaction_Response };
		co_await newThreadAwaitable<message_data>();
		auto dataPackage{ dataPackageNew };
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::editInteractionResponseAsync()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> interactions::deleteInteractionResponseAsync(delete_interaction_response_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Interaction_Response };
		co_await newThreadAwaitable<void>();
		std::this_thread::sleep_for(milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		workload.callStack	   = "interactions::deleteInteractionResponseAsync()";
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	co_routine<message_data> interactions::createFollowUpMessageAsync(create_follow_up_message_data dataPackageNew) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Followup_Message };
		co_await newThreadAwaitable<message_data>();
		auto dataPackage{ dataPackageNew };
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken;
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::createFollowUpMessageAsync()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> interactions::getFollowUpMessageAsync(get_follow_up_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Get_Followup_Message };
		co_await newThreadAwaitable<message_data>();
		workload.workloadClass = discord_core_internal::https_workload_class::Get;
		workload.relativePath  = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
		workload.callStack	   = "interactions::getFollowUpMessageAsync()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<message_data> interactions::editFollowUpMessageAsync(edit_follow_up_message_data dataPackageNew) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Followup_Message };
		co_await newThreadAwaitable<message_data>();
		auto dataPackage{ dataPackageNew };
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			dataPackage.messagePackage.messageId;
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::editFollowUpMessageAsync()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		co_return returnData;
	}

	co_routine<void> interactions::deleteFollowUpMessageAsync(delete_follow_up_message_data dataPackage) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Delete_Followup_Message };
		co_await newThreadAwaitable<void>();
		std::this_thread::sleep_for(milliseconds{ dataPackage.timeDelay });
		workload.workloadClass = discord_core_internal::https_workload_class::Delete;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			dataPackage.messagePackage.messageId;
		workload.callStack = "interactions::deleteFollowUpMessageAsync()";
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		co_return;
	}

	message_data interactions::createInteractionResponse(create_interaction_response_data dataPackageNew) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Interaction_Response };
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		auto dataPackage{ dataPackageNew };
		workload.relativePath  = "/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		dataPackage.generateExcludedKeys();
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::createInteractionResponse()";
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload));
		get_interaction_response_data dataPackage01{};
		dataPackage01.applicationId	   = dataPackage.interactionPackage.applicationId;
		dataPackage01.interactionToken = dataPackage.interactionPackage.interactionToken;
		if (dataPackage.type != interaction_callback_type::Application_Command_Autocomplete_Result) {
			return interactions::getInteractionResponse(dataPackage01);
		} else {
			return {};
		}
	}

	message_data interactions::editInteractionResponse(edit_interaction_response_data dataPackageNew) {
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Interaction_Response };
		auto dataPackage{ dataPackageNew };
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::editInteractionResponse()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		return returnData;
	}

	message_data interactions::createFollowUpMessage(create_follow_up_message_data dataPackageNew) {
		auto dataPackage{ dataPackageNew };
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Post_Followup_Message };
		workload.workloadClass = discord_core_internal::https_workload_class::Post;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken;
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::createFollowUpMessage()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		return returnData;
	}

	message_data interactions::editFollowUpMessage(edit_follow_up_message_data dataPackageNew) {
		auto dataPackage{ dataPackageNew };
		discord_core_internal::https_workload_data workload{ discord_core_internal::https_workload_type::Patch_Followup_Message };
		workload.workloadClass = discord_core_internal::https_workload_class::Patch;
		workload.relativePath  = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			dataPackage.messagePackage.messageId;
		dataPackage.generateExcludedKeys();
		if (dataPackage.files.size() > 0) {
			workload.payloadType = discord_core_internal::payload_type::Multipart_Form;
		}
		parser.serializeJson(dataPackage, workload.content);
		workload.callStack = "interactions::editFollowUpMessage()";
		message_data returnData{};
		interactions::httpsClient->submitWorkloadAndGetResult(std::move(workload), returnData);
		return returnData;
	}

	select_menu_collector::select_menu_collector(input_event_data dataPackage) {
		channelId		 = dataPackage.getInteractionData().channelId;
		messageId		 = dataPackage.getMessageData().id;
		*interactionData = dataPackage.getInteractionData();
		buffersMapKey	 = channelId.operator jsonifier::string() + messageId.operator jsonifier::string();
	}

	co_routine<jsonifier::vector<select_menu_response_data>, false> select_menu_collector::collectSelectMenuData(bool getSelectMenuDataForAllNew, uint32_t maxWaitTimeInMsNew,
		uint32_t maxCollectedSelectMenuCountNew, create_interaction_response_data errorMessageDataNew, snowflake targetUser) {
		co_await newThreadAwaitable<jsonifier::vector<select_menu_response_data>, false>();
		select_menu_collector::selectMenuInteractionBuffersMap[buffersMapKey] = &selectMenuIncomingInteractionBuffer;
		if (targetUser == 0 && !getSelectMenuDataForAllNew) {
			getSelectMenuDataForAll = true;
		} else {
			getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		}
		if (targetUser != 0) {
			userId = targetUser;
		}
		maxCollectedSelectMenuCount = maxCollectedSelectMenuCountNew;
		getSelectMenuDataForAll		= getSelectMenuDataForAllNew;
		errorMessageData			= errorMessageDataNew;
		maxTimeInMs					= maxWaitTimeInMsNew;
		run();
		co_return std::move(responseVector);
	}

	void select_menu_collector::collectSelectMenuData(std::function<bool(interaction_data)> triggerFunctionNew,
		discord_core_internal::trigger_event_delegate<void, interaction_data> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		select_menu_collector::selectMenuInteractionEventsMap.add(std::move(functionNew));
	}

	select_menu_collector::~select_menu_collector() {
		if (select_menu_collector::selectMenuInteractionBuffersMap.contains(buffersMapKey)) {
			select_menu_collector::selectMenuInteractionBuffersMap.erase(buffersMapKey);
		}
	}

	void select_menu_collector::run() {
		stop_watch<milliseconds> stopWatch{ milliseconds{ maxTimeInMs } };
		stopWatch.reset();
		while (!doWeQuit && !stopWatch.hasTimeElapsed()) {
			if (!getSelectMenuDataForAll) {
				auto selectMenuInteractionData = makeUnique<interaction_data>();
				if (waitForTimeToPass(selectMenuIncomingInteractionBuffer, *selectMenuInteractionData.get(), maxTimeInMs)) {
					selectMenuId			   = "empty";
					auto response			   = makeUnique<select_menu_response_data>();
					response->selectionId	   = selectMenuId;
					response->channelId		   = channelId;
					response->messageId		   = messageId;
					response->userId		   = selectMenuInteractionData->user.id;
					*response->interactionData = *interactionData;
					response->values		   = jsonifier::vector<jsonifier::string>{ "empty" };
					responseVector.emplace_back(*response);
					break;
				}
				if (selectMenuInteractionData->member.user.id != userId) {
					errorMessageData.interactionPackage.applicationId	 = selectMenuInteractionData->applicationId;
					errorMessageData.interactionPackage.interactionId	 = selectMenuInteractionData->id;
					errorMessageData.interactionPackage.interactionToken = selectMenuInteractionData->token;
					errorMessageData.messagePackage.messageId			 = selectMenuInteractionData->message.id;
					errorMessageData.messagePackage.channelId			 = selectMenuInteractionData->message.channelId;
					errorMessageData.type								 = interaction_callback_type::Channel_Message_With_Source;
					interactions::createInteractionResponseAsync(errorMessageData).get();
				} else {
					*interactionData		   = *selectMenuInteractionData;
					selectMenuId			   = selectMenuInteractionData->data.customId;
					auto response			   = makeUnique<select_menu_response_data>();
					response->selectionId	   = selectMenuId;
					response->channelId		   = channelId;
					response->messageId		   = messageId;
					response->userId		   = selectMenuInteractionData->user.id;
					response->values		   = interactionData->data.values;
					*response->interactionData = *selectMenuInteractionData;
					responseVector.emplace_back(*response);
					++currentCollectedSelectMenuCount;
					stopWatch.reset();
					if (maxCollectedSelectMenuCount > 1 && currentCollectedSelectMenuCount < maxCollectedSelectMenuCount - 1) {
						auto createResponseData	 = makeUnique<create_interaction_response_data>(*selectMenuInteractionData);
						createResponseData->type = interaction_callback_type::Deferred_Update_Message;
						interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (currentCollectedSelectMenuCount >= maxCollectedSelectMenuCount) {
						for (auto& value: responseVector) {
							*value.interactionData = *selectMenuInteractionData;
						}
						doWeQuit = true;
					}
				}
			} else {
				auto selectMenuInteractionData = makeUnique<interaction_data>();
				if (waitForTimeToPass(selectMenuIncomingInteractionBuffer, *selectMenuInteractionData.get(), maxTimeInMs)) {
					selectMenuId			   = "empty";
					auto response			   = makeUnique<select_menu_response_data>();
					response->selectionId	   = selectMenuId;
					response->channelId		   = channelId;
					response->messageId		   = messageId;
					response->userId		   = selectMenuInteractionData->user.id;
					*response->interactionData = *interactionData;
					response->values		   = jsonifier::vector<jsonifier::string>{ "empty" };
					responseVector.emplace_back(*response);
					break;
				}
				*interactionData		   = *selectMenuInteractionData;
				selectMenuId			   = selectMenuInteractionData->data.customId;
				auto response			   = makeUnique<select_menu_response_data>();
				response->selectionId	   = selectMenuId;
				response->channelId		   = channelId;
				response->messageId		   = messageId;
				response->userId		   = selectMenuInteractionData->user.id;
				*response->interactionData = *selectMenuInteractionData;
				response->values		   = interactionData->data.values;
				responseVector.emplace_back(*response);
				++currentCollectedSelectMenuCount;
				stopWatch.reset();
				if (maxCollectedSelectMenuCount > 1 && currentCollectedSelectMenuCount < maxCollectedSelectMenuCount - 1) {
					auto createResponseData	 = makeUnique<create_interaction_response_data>(*selectMenuInteractionData);
					createResponseData->type = interaction_callback_type::Deferred_Update_Message;
					interactions::createInteractionResponseAsync(*createResponseData).get();
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
		select_menu_collector::selectMenuInteractionBuffersMap.erase(buffersMapKey);
	}

	button_collector::button_collector(input_event_data dataPackage) {
		channelId													 = dataPackage.getInteractionData().channelId;
		messageId													 = dataPackage.getMessageData().id;
		*interactionData											 = dataPackage.getInteractionData();
		buffersMapKey												 = channelId.operator jsonifier::string() + messageId.operator jsonifier::string();
		button_collector::buttonInteractionBuffersMap[buffersMapKey] = &buttonIncomingInteractionBuffer;
	}

	co_routine<jsonifier::vector<button_response_data>, false> button_collector::collectButtonData(bool getButtonDataForAllNew, uint32_t maxWaitTimeInMsNew,
		uint32_t maxNumberOfPressesNew, create_interaction_response_data errorMessageDataNew, snowflake targetUser) {
		co_await newThreadAwaitable<jsonifier::vector<button_response_data>, false>();
		if (targetUser == 0 && !getButtonDataForAllNew) {
			throw dca_exception{ "button_collector::collectButtonData(), you've failed to "
								 "properly set the collectButtonData() parameters!" };
		}
		if (targetUser != 0) {
			userId = targetUser;
		}
		maxCollectedButtonCount = maxNumberOfPressesNew;
		getButtonDataForAll		= getButtonDataForAllNew;
		errorMessageData		= errorMessageDataNew;
		maxTimeInMs				= maxWaitTimeInMsNew;
		run();
		co_return std::move(responseVector);
	}

	void button_collector::collectButtonData(std::function<bool(interaction_data)> triggerFunctionNew,
		discord_core_internal::trigger_event_delegate<void, interaction_data> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		button_collector::buttonInteractionEventsMap.add(std::move(functionNew));
	}

	button_collector::~button_collector() {
		if (button_collector::buttonInteractionBuffersMap.contains(buffersMapKey)) {
			button_collector::buttonInteractionBuffersMap.erase(buffersMapKey);
		}
	}

	void button_collector::run() {
		stop_watch<milliseconds> stopWatch{ milliseconds{ maxTimeInMs } };
		stopWatch.reset();
		while (!doWeQuit && !stopWatch.hasTimeElapsed()) {
			if (!getButtonDataForAll) {
				auto buttonInteractionData = makeUnique<interaction_data>();
				if (waitForTimeToPass(buttonIncomingInteractionBuffer, *buttonInteractionData.get(), maxTimeInMs)) {
					buttonId				   = "empty";
					auto response			   = makeUnique<button_response_data>();
					response->buttonId		   = buttonId;
					response->channelId		   = channelId;
					response->messageId		   = messageId;
					response->userId		   = buttonInteractionData->user.id;
					*response->interactionData = *interactionData;
					responseVector.emplace_back(*response);
					break;
				}
				if (buttonInteractionData->member.user.id != userId) {
					errorMessageData.interactionPackage.applicationId	 = buttonInteractionData->applicationId;
					errorMessageData.interactionPackage.interactionId	 = buttonInteractionData->id;
					errorMessageData.interactionPackage.interactionToken = buttonInteractionData->token;
					errorMessageData.messagePackage.messageId			 = buttonInteractionData->message.id;
					errorMessageData.messagePackage.channelId			 = buttonInteractionData->message.channelId;
					errorMessageData.type								 = interaction_callback_type::Channel_Message_With_Source;
					interactions::createInteractionResponseAsync(errorMessageData).get();
				} else {
					*interactionData		   = *buttonInteractionData;
					buttonId				   = buttonInteractionData->data.customId;
					auto response			   = makeUnique<button_response_data>();
					response->buttonId		   = buttonId;
					response->channelId		   = channelId;
					response->messageId		   = messageId;
					response->emojiName		   = buttonInteractionData->message.components[0].components[0].emoji.name;
					response->userId		   = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					responseVector.emplace_back(*response);
					++currentCollectedButtonCount;
					stopWatch.reset();
					if (maxCollectedButtonCount > 1 && currentCollectedButtonCount < maxCollectedButtonCount) {
						auto createResponseData	 = makeUnique<create_interaction_response_data>(*buttonInteractionData);
						createResponseData->type = interaction_callback_type::Deferred_Update_Message;
						interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (currentCollectedButtonCount >= maxCollectedButtonCount) {
						for (auto& value: responseVector) {
							*value.interactionData = *buttonInteractionData;
						}
						doWeQuit = true;
					}
				}
			} else {
				auto buttonInteractionData = makeUnique<interaction_data>();
				if (waitForTimeToPass(buttonIncomingInteractionBuffer, *buttonInteractionData.get(), maxTimeInMs)) {
					buttonId				   = "empty";
					auto response			   = makeUnique<button_response_data>();
					response->buttonId		   = buttonId;
					response->channelId		   = channelId;
					response->messageId		   = messageId;
					response->emojiName		   = buttonInteractionData->message.components[0].components[0].emoji.name;
					response->userId		   = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					responseVector.emplace_back(*response);
					break;
				}
				*interactionData		   = *buttonInteractionData;
				buttonId				   = buttonInteractionData->data.customId;
				auto response			   = makeUnique<button_response_data>();
				response->buttonId		   = buttonId;
				response->channelId		   = channelId;
				response->messageId		   = messageId;
				response->emojiName		   = buttonInteractionData->message.components[0].components[0].emoji.name;
				response->userId		   = buttonInteractionData->user.id;
				*response->interactionData = *buttonInteractionData;
				responseVector.emplace_back(*response);
				++currentCollectedButtonCount;
				stopWatch.reset();
				if (maxCollectedButtonCount > 1 && currentCollectedButtonCount < maxCollectedButtonCount) {
					auto createResponseData	 = makeUnique<create_interaction_response_data>(*buttonInteractionData);
					createResponseData->type = interaction_callback_type::Deferred_Update_Message;
					interactions::createInteractionResponseAsync(*createResponseData).get();
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
		button_collector::buttonInteractionBuffersMap.erase(buffersMapKey);
	}

	modal_collector::modal_collector(input_event_data dataPackage) {
		channelId											   = dataPackage.getInteractionData().channelId;
		modal_collector::modalInteractionBuffersMap[channelId.operator jsonifier::string()] = &modalIncomingInteractionBuffer;
	}

	co_routine<modal_response_data, false> modal_collector::collectModalData(uint32_t maxWaitTimeInMsNew) {
		co_await newThreadAwaitable<modal_response_data, false>();
		maxTimeInMs = maxWaitTimeInMsNew;
		run();
		co_return std::move(responseData);
	}

	void modal_collector::collectModalData(std::function<bool(interaction_data)> triggerFunctionNew,
		discord_core_internal::trigger_event_delegate<void, interaction_data> functionNew) {
		functionNew.setTestFunction(triggerFunctionNew);
		modal_collector::modalInteractionEventsMap.add(std::move(functionNew));
	}

	modal_collector::~modal_collector() {
		if (modal_collector::modalInteractionBuffersMap.contains(channelId.operator jsonifier::string())) {
			modal_collector::modalInteractionBuffersMap.erase(channelId.operator jsonifier::string());
		}
	}

	void modal_collector::run() {
		stop_watch<milliseconds> stopWatch{ milliseconds{ maxTimeInMs } };
		stopWatch.reset();
		while (!doWeQuit && !stopWatch.hasTimeElapsed()) {
			auto buttonInteractionData = makeUnique<interaction_data>();
			if (waitForTimeToPass(modalIncomingInteractionBuffer, *buttonInteractionData.get(), maxTimeInMs)) {
				*responseData.interactionData = *buttonInteractionData;
				responseData.channelId		  = buttonInteractionData->channelId;
				responseData.customId		  = buttonInteractionData->data.customId;
				responseData.userId			  = buttonInteractionData->user.id;
				responseData.values			  = buttonInteractionData->data.values;
				break;
			} else {
				*responseData.interactionData = *buttonInteractionData;
				responseData.channelId		  = buttonInteractionData->channelId;
				responseData.customId		  = buttonInteractionData->data.customId;
				responseData.userId			  = buttonInteractionData->user.id;
				responseData.values			  = buttonInteractionData->data.values;
				break;
			}
		}

		modal_collector::modalInteractionBuffersMap.erase(channelId.operator jsonifier::string());
	}

	unordered_map<jsonifier::string, unbounded_message_block<interaction_data>*> select_menu_collector::selectMenuInteractionBuffersMap{};
	unordered_map<jsonifier::string, unbounded_message_block<interaction_data>*> button_collector::buttonInteractionBuffersMap{};
	unordered_map<jsonifier::string, unbounded_message_block<interaction_data>*> modal_collector::modalInteractionBuffersMap{};
	discord_core_internal::trigger_event<void, interaction_data> select_menu_collector::selectMenuInteractionEventsMap{};
	discord_core_internal::trigger_event<void, interaction_data> button_collector::buttonInteractionEventsMap{};
	discord_core_internal::trigger_event<void, interaction_data> modal_collector::modalInteractionEventsMap{};
	discord_core_internal::https_client* interactions::httpsClient{};
};
