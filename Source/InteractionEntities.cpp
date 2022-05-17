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
/// InteractionEntities.cpp - Source file for the interaction related classes and structs.
/// May 28, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file InteractionEntities.cpp

#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/ThreadPool.hpp>
#include <discordcoreapi/EventEntities.hpp>

namespace DiscordCoreAPI {

	void Interactions::initialize(DiscordCoreInternal::HttpClient* theClient) {
		Interactions::httpClient = theClient;
	}

	CoRoutine<Message> Interactions::createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Interaction_Response);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		if (dataPackage.data.data.files.size() > 0) {
			constructMultiPartData(workload, nlohmann::json::parse(DiscordCoreInternal::JSONIFY(dataPackage.data)), dataPackage.data.data.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage.data);
		}
		workload.callStack = "Interactions::createInteractionResponseAsync";
		DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Interactions::httpClient, workload);
		GetInteractionResponseData dataPackage01{};
		dataPackage01.applicationId = dataPackage.interactionPackage.applicationId;
		dataPackage01.interactionToken = dataPackage.interactionPackage.interactionToken;
		if (dataPackage.data.type != InteractionCallbackType::Application_Command_Autocomplete_Result) {
			co_return Interactions::getInteractionResponseAsync(dataPackage01).get();
		} else {
			co_return Message{};
		}
	}

	CoRoutine<Message> Interactions::getInteractionResponseAsync(GetInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Interaction_Response);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::getInteractionResponseAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
	}

	CoRoutine<Message> Interactions::editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Interaction_Response);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		if (dataPackage.data.files.size() > 0) {
			constructMultiPartData(workload, nlohmann::json::parse(DiscordCoreInternal::JSONIFY(dataPackage.data)), dataPackage.data.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage.data);
		}
		workload.callStack = "Interactions::editInteractionResponseAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
	}

	CoRoutine<void> Interactions::deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Interaction_Response);
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::deleteInteractionResponseAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Interactions::httpClient, workload);
	}

	CoRoutine<Message> Interactions::createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Post_Followup_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Followup_Message;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken;
		if (dataPackage.files.size() > 0) {
			constructMultiPartData(workload, nlohmann::json::parse(DiscordCoreInternal::JSONIFY(dataPackage)), dataPackage.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
		}
		workload.callStack = "Interactions::createFollowUpMessageAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
	}

	CoRoutine<Message> Interactions::getFollowUpMessageAsync(GetFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Get_Followup_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Followup_Message;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
		workload.callStack = "Interactions::getFollowUpMessageAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
	}

	CoRoutine<Message> Interactions::editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Patch_Followup_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Followup_Message;
		;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			dataPackage.messagePackage.messageId;
		if (dataPackage.data.files.size() > 0) {
			constructMultiPartData(workload, nlohmann::json::parse(DiscordCoreInternal::JSONIFY(dataPackage.data)), dataPackage.data.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage.data);
		}
		workload.callStack = "Interactions::editFollowUpMessageAsync";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
	}

	CoRoutine<void> Interactions::deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpWorkloadType::Delete_Followup_Message);
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
		workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Followup_Message;
		workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			dataPackage.messagePackage.messageId;
		workload.callStack = "Interactions::deleteFollowUpMessageToBeWrappe";
		co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Interactions::httpClient, workload);
	}

	SelectMenuCollector::SelectMenuCollector(InputEventData dataPackage) {
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		this->userId = dataPackage.getRequesterId();
		*this->interactionData = dataPackage.getInteractionData();
		this->bufferMapKey = this->channelId + this->messageId;
		SelectMenuCollector::selectMenuInteractionBufferMap.insert(std::make_pair(this->bufferMapKey, &this->selectMenuIncomingInteractionBuffer));
	}

	CoRoutine<std::vector<SelectMenuResponseData>> SelectMenuCollector::collectSelectMenuData(bool getSelectMenuDataForAllNew, int32_t maxWaitTimeInMsNew,
		int32_t maxCollectedSelectMenuCountNew, const std::string& targetUser) {
		co_await NewThreadAwaitable<std::vector<SelectMenuResponseData>>();
		if (targetUser == "" && !getSelectMenuDataForAllNew) {
			this->getSelectMenuDataForAll = true;
		} else {
			this->getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		}
		if (targetUser != "") {
			this->userId = targetUser;
		}
		this->maxCollectedSelectMenuCount = maxCollectedSelectMenuCountNew;
		this->getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseVector;
	}

	SelectMenuCollector::~SelectMenuCollector() {
		if (SelectMenuCollector::selectMenuInteractionBufferMap.contains(this->bufferMapKey)) {
			SelectMenuCollector::selectMenuInteractionBufferMap.erase(this->bufferMapKey);
		}
	}

	void SelectMenuCollector::run() {
		while (!this->doWeQuit) {
			if (this->getSelectMenuDataForAll == false) {
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
					this->responseVector.push_back(*response);
					break;
				}
				if (selectMenuInteractionData->user.id != this->userId) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
					auto embedData = std::make_unique<EmbedData>();
					embedData->setColor("FEFEFE");
					embedData->setTitle("__**Permission Issue:**__");
					embedData->setTimeStamp(getTimeAndDate());
					embedData->setDescription("Sorry, but that menu can only be selected by <@" + this->userId + ">!");
					createResponseData->data.data.embeds.push_back(*embedData);
					createResponseData->data.data.flags = 64;
					createResponseData->data.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				} else {
					*this->interactionData = *selectMenuInteractionData;
					this->selectMenuId = selectMenuInteractionData->data.componentData.customId;
					auto response = std::make_unique<SelectMenuResponseData>();
					response->selectionId = this->selectMenuId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = selectMenuInteractionData->user.id;
					response->values = this->interactionData->data.componentData.values;
					*response->interactionData = *selectMenuInteractionData;
					this->responseVector.push_back(*response);
					this->currentCollectedSelectMenuCount += 1;
					if (this->maxCollectedSelectMenuCount > 1 && this->currentCollectedSelectMenuCount < this->maxCollectedSelectMenuCount - 1) {
						auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
						createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (this->currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
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
					this->responseVector.push_back(*response);
					break;
				}
				*this->interactionData = *selectMenuInteractionData;
				this->selectMenuId = selectMenuInteractionData->data.componentData.customId;
				auto response = std::make_unique<SelectMenuResponseData>();
				response->selectionId = this->selectMenuId;
				response->channelId = this->channelId;
				response->messageId = this->messageId;
				response->userId = selectMenuInteractionData->user.id;
				*response->interactionData = *selectMenuInteractionData;
				response->values = this->interactionData->data.componentData.values;
				this->responseVector.push_back(*response);
				this->currentCollectedSelectMenuCount += 1;
				if (this->maxCollectedSelectMenuCount > 1 && this->currentCollectedSelectMenuCount < this->maxCollectedSelectMenuCount - 1) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*selectMenuInteractionData);
					createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (this->currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
					this->doWeQuit = true;
					for (auto& value: this->responseVector) {
						*value.interactionData = *selectMenuInteractionData;
					}
				}
			}
		}
		SelectMenuCollector::selectMenuInteractionBufferMap.erase(this->bufferMapKey);
	}

	ButtonCollector::ButtonCollector(InputEventData dataPackage) {
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		this->userId = dataPackage.getRequesterId();
		*this->interactionData = dataPackage.getInteractionData();
		ButtonCollector::buttonInteractionBufferMap.insert_or_assign(this->channelId + this->messageId, &this->buttonIncomingInteractionBuffer);
	}

	CoRoutine<std::vector<ButtonResponseData>> ButtonCollector::collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew, int32_t maxNumberOfPressesNew,
		 const std::string& targetUser) {
		co_await NewThreadAwaitable<std::vector<ButtonResponseData>>();
		if (targetUser == "" && !getButtonDataForAllNew) {
			throw std::runtime_error("You've failed to properly set the collectButtonData() parameters!");
		}
		if (targetUser != "") {
			this->userId = targetUser;
		}
		this->maxCollectedButtonCount = maxNumberOfPressesNew;
		this->getButtonDataForAll = getButtonDataForAllNew;
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseVector;
	}

	ButtonCollector::~ButtonCollector() {
		if (ButtonCollector::buttonInteractionBufferMap.contains(this->channelId + this->messageId)) {
			ButtonCollector::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
		}
	}

	void ButtonCollector::run() {
		while (!this->doWeQuit) {
			if (this->getButtonDataForAll == false) {
				auto buttonInteractionData = std::make_unique<InteractionData>();
				if (waitForTimeToPass(this->buttonIncomingInteractionBuffer, *buttonInteractionData.get(), this->maxTimeInMs)) {
					this->buttonId = "empty";
					auto response = std::make_unique<ButtonResponseData>();
					response->buttonId = this->buttonId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *this->interactionData;
					this->responseVector.push_back(*response);
					break;
				}
				if (buttonInteractionData->user.id != this->userId) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
					auto embedData = std::make_unique<EmbedData>();
					embedData->setColor("FEFEFE");
					embedData->setTitle("__**Permission Issue:**__");
					embedData->setTimeStamp(getTimeAndDate());
					embedData->setDescription("Sorry, but that button can only be pressed by <@" + this->userId + ">!");
					createResponseData->data.data.embeds.push_back(*embedData);
					createResponseData->data.data.flags = 64;
					createResponseData->data.type = InteractionCallbackType::Channel_Message_With_Source;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				} else {
					*this->interactionData = *buttonInteractionData;
					this->buttonId = buttonInteractionData->data.componentData.customId;
					auto response = std::make_unique<ButtonResponseData>();
					response->buttonId = this->buttonId;
					response->channelId = this->channelId;
					response->messageId = this->messageId;
					response->userId = buttonInteractionData->user.id;
					*response->interactionData = *buttonInteractionData;
					this->responseVector.push_back(*response);
					this->currentCollectedButtonCount += 1;
					if (this->maxCollectedButtonCount > 1 && this->currentCollectedButtonCount < this->maxCollectedButtonCount) {
						auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
						createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
						Interactions::createInteractionResponseAsync(*createResponseData).get();
					}
					if (this->currentCollectedButtonCount >= this->maxCollectedButtonCount) {
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
					this->responseVector.push_back(*response);
					break;
				}
				*this->interactionData = *buttonInteractionData;
				this->buttonId = buttonInteractionData->data.componentData.customId;
				auto response = std::make_unique<ButtonResponseData>();
				response->buttonId = this->buttonId;
				response->channelId = this->channelId;
				response->messageId = this->messageId;
				response->userId = buttonInteractionData->user.id;
				*response->interactionData = *buttonInteractionData;
				this->responseVector.push_back(*response);
				this->currentCollectedButtonCount += 1;
				if (this->maxCollectedButtonCount > 1 && this->currentCollectedButtonCount < this->maxCollectedButtonCount) {
					auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
					createResponseData->data.type = InteractionCallbackType::Deferred_Update_Message;
					Interactions::createInteractionResponseAsync(*createResponseData).get();
				}
				if (this->currentCollectedButtonCount >= this->maxCollectedButtonCount) {
					for (auto& value: this->responseVector) {
						*value.interactionData = *buttonInteractionData;
					}
					this->doWeQuit = true;
				}
			}

		}
		ButtonCollector::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
	}

	ModalCollector::ModalCollector(InputEventData dataPackage) {
		this->channelId = dataPackage.getChannelId();
		ModalCollector::modalInteractionBufferMap.insert_or_assign(this->channelId, &this->modalIncomingInteractionBuffer);
	}

	CoRoutine<ModalResponseData> ModalCollector::collectModalData(int32_t maxWaitTimeInMsNew) {
		co_await NewThreadAwaitable<ModalResponseData>();
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseData;
	}

	ModalCollector::~ModalCollector() {
		if (ModalCollector::modalInteractionBufferMap.contains(this->channelId)) {
			ModalCollector::modalInteractionBufferMap.erase(this->channelId);
		}
	}

	void ModalCollector::run() {
		while (!this->doWeQuit) {
			auto buttonInteractionData = std::make_unique<InteractionData>();
			if (waitForTimeToPass(this->modalIncomingInteractionBuffer, *buttonInteractionData.get(), this->maxTimeInMs)) {
				*this->responseData.interactionData = *buttonInteractionData;
				this->responseData.channelId = buttonInteractionData->channelId;
				this->responseData.customId = buttonInteractionData->data.modalData.customId;
				this->responseData.customIdSmall = buttonInteractionData->data.modalData.customIdSmall;
				this->responseData.userId = buttonInteractionData->user.id;
				this->responseData.value = buttonInteractionData->data.modalData.value;
				break;
			} else {
				*this->responseData.interactionData = *buttonInteractionData;
				this->responseData.channelId = buttonInteractionData->channelId;
				this->responseData.customId = buttonInteractionData->data.modalData.customId;
				this->responseData.customIdSmall = buttonInteractionData->data.modalData.customIdSmall;
				this->responseData.userId = buttonInteractionData->user.id;
				this->responseData.value = buttonInteractionData->data.modalData.value;
				break;
			}
		}

		ButtonCollector::buttonInteractionBufferMap.erase(this->channelId);
	}

	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> SelectMenuCollector::selectMenuInteractionBufferMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ButtonCollector::buttonInteractionBufferMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ModalCollector::modalInteractionBufferMap{};
	DiscordCoreInternal::HttpClient* Interactions::httpClient{ nullptr };
};
