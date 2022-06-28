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
/// InteractionEntities.cpp - Source file for the interaction related classes and structs.
/// May 28, 2021
/// https://discordcoreapi.com
/// \file InteractionEntities.cpp

#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/ThreadPool.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	void Interactions::initialize(DiscordCoreInternal::HttpsClient* theClient) {
		Interactions::httpsClient = theClient;
	}

	CoRoutine<Message> Interactions::createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Interaction_Response);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath =
			"/interactions/" + std::to_string(dataPackage.interactionPackage.interactionId) + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
		if (dataPackage.data.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(nlohmann::json::parse(DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage.data)), dataPackage.data.data.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage.data);
		}
		workload.callStack = "Interactions::createInteractionResponseAsync";
		Interactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
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
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Interaction_Response);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.applicationId) + "/" + dataPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::getInteractionResponseAsync";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Interactions::editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Interaction_Response);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath =
			"/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(nlohmann::json::parse(DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage.data)), dataPackage.data.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage.data);
		}
		workload.callStack = "Interactions::editInteractionResponseAsync";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<void> Interactions::deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Interaction_Response);
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Interaction_Response;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath =
			"/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
		workload.callStack = "Interactions::deleteInteractionResponseAsync";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	CoRoutine<Message> Interactions::createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Post_Followup_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Post_Followup_Message;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Post;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken;
		if (dataPackage.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(nlohmann::json::parse(DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage)), dataPackage.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage);
		}
		workload.callStack = "Interactions::createFollowUpMessageAsync";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Interactions::getFollowUpMessageAsync(GetFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Get_Followup_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Get_Followup_Message;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Get;
		workload.relativePath =
			"/webhooks/" + std::to_string(dataPackage.applicationId) + "/" + dataPackage.interactionToken + "/messages/" + std::to_string(dataPackage.messageId);
		workload.callStack = "Interactions::getFollowUpMessageAsync";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<Message> Interactions::editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Patch_Followup_Message);
		co_await NewThreadAwaitable<Message>();
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Patch_Followup_Message;
		;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Patch;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			std::to_string(dataPackage.messagePackage.messageId);
		if (dataPackage.data.files.size() > 0) {
			workload.payloadType = DiscordCoreInternal::PayloadType::Multipart_Form;
			workload.content = constructMultiPartData(nlohmann::json::parse(DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage.data)), dataPackage.data.files);
		} else {
			workload.content = DiscordCoreInternal::JSONIfier::JSONIFY(dataPackage.data);
		}
		workload.callStack = "Interactions::editFollowUpMessageAsync";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<Message>(workload);
	}

	CoRoutine<void> Interactions::deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
		DiscordCoreInternal::HttpsWorkloadData workload{};
		workload.thisWorkerId = DiscordCoreInternal::HttpsWorkloadData::getAndIncrementWorkloadId(DiscordCoreInternal::HttpsWorkloadType::Delete_Followup_Message);
		co_await NewThreadAwaitable<void>();
		std::this_thread::sleep_for(std::chrono::milliseconds{ dataPackage.timeDelay });
		workload.workloadType = DiscordCoreInternal::HttpsWorkloadType::Delete_Followup_Message;
		workload.workloadClass = DiscordCoreInternal::HttpsWorkloadClass::Delete;
		workload.relativePath = "/webhooks/" + std::to_string(dataPackage.interactionPackage.applicationId) + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" +
			std::to_string(dataPackage.messagePackage.messageId);
		workload.callStack = "Interactions::deleteFollowUpMessageToBeWrappe";
		co_return Interactions::httpsClient->submitWorkloadAndGetResult<void>(workload);
	}

	SelectMenuCollector::SelectMenuCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		*this->interactionData = dataPackage.getInteractionData();
		this->bufferMapKey = std::to_string(this->channelId) + std::to_string(this->messageId);
		SelectMenuCollector::selectMenuInteractionBufferMap[this->bufferMapKey] = &this->selectMenuIncomingInteractionBuffer;
	}

	CoRoutine<std::vector<SelectMenuResponseData>> SelectMenuCollector::collectSelectMenuData(bool getSelectMenuDataForAllNew, int32_t maxWaitTimeInMsNew,
		int32_t maxCollectedSelectMenuCountNew, uint64_t targetUser) {
		co_await NewThreadAwaitable<std::vector<SelectMenuResponseData>>();
		if (targetUser == 0 && !getSelectMenuDataForAllNew) {
			this->getSelectMenuDataForAll = true;
		} else {
			this->getSelectMenuDataForAll = getSelectMenuDataForAllNew;
		}
		if (targetUser != 0) {
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
		StopWatch theStopWatch{ std::chrono::milliseconds{ this->maxTimeInMs } };
		while (!this->doWeQuit && !theStopWatch.hasTimePassed()) {
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
					embedData->setDescription("Sorry, but that menu can only be selected by <@" + std::to_string(this->userId) + ">!");
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
					this->currentCollectedSelectMenuCount++;
					theStopWatch.resetTimer();
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
				this->currentCollectedSelectMenuCount++;
				theStopWatch.resetTimer();
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

	ButtonCollector::ButtonCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelId();
		this->messageId = dataPackage.getMessageId();
		*this->interactionData = dataPackage.getInteractionData();
		this->bufferMapKey = std::to_string(this->channelId) + std::to_string(this->messageId);
		ButtonCollector::buttonInteractionBufferMap[this->bufferMapKey] = &this->buttonIncomingInteractionBuffer;
	}

	CoRoutine<std::vector<ButtonResponseData>> ButtonCollector::collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew, int32_t maxNumberOfPressesNew,
		uint64_t targetUser) {
		co_await NewThreadAwaitable<std::vector<ButtonResponseData>>();
		if (targetUser == 0 && !getButtonDataForAllNew) {
			throw std::runtime_error{ "ButtonCollector::collectButtonData(), You've failed to "
									  "properly set the collectButtonData() parameters!" };
		}
		if (targetUser != 0) {
			this->userId = targetUser;
		}
		this->maxCollectedButtonCount = maxNumberOfPressesNew;
		this->getButtonDataForAll = getButtonDataForAllNew;
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseVector;
	}

	ButtonCollector::~ButtonCollector() {
		if (ButtonCollector::buttonInteractionBufferMap.contains(this->bufferMapKey)) {
			ButtonCollector::buttonInteractionBufferMap.erase(this->bufferMapKey);
		}
	}

	void ButtonCollector::run() {
		StopWatch theStopWatch{ std::chrono::milliseconds{ this->maxTimeInMs } };
		while (!this->doWeQuit && !theStopWatch.hasTimePassed()) {
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
					embedData->setDescription("Sorry, but that button can only be pressed by <@" + std::to_string(this->userId) + ">!");
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
					this->currentCollectedButtonCount++;
					theStopWatch.resetTimer();
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
				this->currentCollectedButtonCount++;
				theStopWatch.resetTimer();
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
		ButtonCollector::buttonInteractionBufferMap.erase(this->bufferMapKey);
	}

	ModalCollector::ModalCollector(InputEventData& dataPackage) {
		this->channelId = dataPackage.getChannelId();
		ModalCollector::modalInteractionBufferMap[std::to_string(this->channelId)] = &this->modalIncomingInteractionBuffer;
	}

	CoRoutine<ModalResponseData> ModalCollector::collectModalData(int32_t maxWaitTimeInMsNew) {
		co_await NewThreadAwaitable<ModalResponseData>();
		this->maxTimeInMs = maxWaitTimeInMsNew;
		this->run();
		co_return this->responseData;
	}

	ModalCollector::~ModalCollector() {
		if (ModalCollector::modalInteractionBufferMap.contains(std::to_string(this->channelId))) {
			ModalCollector::modalInteractionBufferMap.erase(std::to_string(this->channelId));
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

		ModalCollector::modalInteractionBufferMap.erase(std::to_string(this->channelId));
	}

	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> SelectMenuCollector::selectMenuInteractionBufferMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ButtonCollector::buttonInteractionBufferMap{};
	std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ModalCollector::modalInteractionBufferMap{};
	DiscordCoreInternal::HttpsClient* Interactions::httpsClient{ nullptr };
};
