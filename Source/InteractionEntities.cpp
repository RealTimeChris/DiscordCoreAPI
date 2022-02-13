// Interactions.cpp - Source file for the interaction related classes and structs.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "InteractionEntities.hpp"
#include "CoRoutine.hpp"
#include "Http.hpp"
#include "JSONIfier.hpp"
#include "EventEntities.hpp"
#include "DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

    Interaction::Interaction(InteractionData dataPackage) {
        this->applicationId = dataPackage.applicationId;
        this->requesterId = dataPackage.requesterId;
        this->channelId = dataPackage.channelId;
        this->guildId = dataPackage.guildId;
        this->message = dataPackage.message;
        this->rawData = dataPackage.rawData;
        this->version = dataPackage.version;
        this->member = dataPackage.member;
        this->token = dataPackage.token;
        this->data = dataPackage.data;
        this->type = dataPackage.type;
        this->user = dataPackage.user;
        this->id = dataPackage.id;
    }

    void Interactions::initialize(DiscordCoreInternal::HttpClient*theClient) {
        Interactions::httpClient = theClient;
    }

     CoRoutine<Message> Interactions::createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
         try {
             co_await NewThreadAwaitable<Message>();
             Interactions::collectMessageDataBuffers.insert(std::make_pair(dataPackage.interactionPackage.interactionId, std::make_unique<UnboundedMessageBlock<MessageData>>()));
             DiscordCoreInternal::HttpWorkloadData workload{};
             workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Interaction_Response;
             workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
             workload.relativePath = "/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
             workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
             workload.callStack = "Interactions::createInteractionResponseAsync";
             DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Interactions::httpClient, workload);
             if (dataPackage.data.type == InteractionCallbackType::ChannelMessageWithSource) {
                 if (Interactions::collectMessageDataBuffers.contains(dataPackage.interactionPackage.interactionId)) {
                     Message messageData{};
                     StopWatch<std::chrono::milliseconds> stopWatch(std::chrono::milliseconds(1500));
                     while (!Interactions::collectMessageDataBuffers.at(dataPackage.interactionPackage.interactionId)->tryReceive(messageData)) {
                         std::this_thread::sleep_for(std::chrono::milliseconds{1});
                         if (stopWatch.hasTimePassed()) {
                             break;
                         }
                     }
                     Interactions::collectMessageDataBuffers.erase(dataPackage.interactionPackage.interactionId);
                     co_return messageData;
                 }
             }
         }
         catch (...) {
             reportException("Interactions::createInteractionResponseAsync()");
         }
     }

    CoRoutine<void> Interactions::createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData dataPackage) {
        try {
            co_await NewThreadAwaitable<void>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Interaction_Response;
            dataPackage.data.data.flags = 64;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
            workload.relativePath = "/interactions/" + dataPackage.interactionPackage.interactionId + "/" + dataPackage.interactionPackage.interactionToken + "/callback";
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "Interactions::createDeferredInteractionResponseAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::createDeferredInteractionResponseAsync()");
        }
    }

    CoRoutine<InteractionResponseData> Interactions::getInteractionResponseAsync(GetInteractionResponseData dataPackage) {
        try {
            co_await NewThreadAwaitable<InteractionResponseData>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Interaction_Response;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.callStack = "Interactions::getInteractionResponseAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<InteractionResponseData>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::getInteractionResponseAsync()");
        }
    }

    CoRoutine<Message> Interactions::editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
        try {
            co_await NewThreadAwaitable<Message>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Interaction_Response;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
            workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "Interactions::editInteractionResponseAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::editInteractionResponseAsync()");
        }
    }

    void Interactions::deleteInteractionResponseToBeWrapped(DeleteInteractionResponseData dataPackage) {
        try {
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Interaction_Response;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
            workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/@original";
            workload.callStack = "Interactions::deleteInteractionResponseAsync";
            return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::deleteInteractionResponseToBeWrapped()");
        }
    }

    CoRoutine<void> Interactions::deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
        try {
            co_await NewThreadAwaitable<void>();
            if (dataPackage.timeDelay > 0) {
                TimeElapsedHandler onSend = [=]() {
                    Interactions::deleteInteractionResponseToBeWrapped(dataPackage);
                };
                ThreadPoolTimer::executeFunctionAfterTimePeriod(onSend, dataPackage.timeDelay);
            }
            else {
                Interactions::deleteInteractionResponseToBeWrapped(dataPackage);
            }
            co_return;
        }
        catch (...) {
            reportException("Interactions::deleteInteractionResponseAsync()");
        }
    }

    CoRoutine<Message> Interactions::createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
        try {
            co_await NewThreadAwaitable<Message>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Followup_Message;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
            workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken;
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "Interactions::createFollowUpMessageAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::createFollowUpMessageAsync()");
        }
    }

    CoRoutine<Message> Interactions::getFollowUpMessageAsync(GetFollowUpMessageData dataPackage) {
        try {
            co_await NewThreadAwaitable<Message>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Followup_Message;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
            workload.callStack = "Interactions::getFollowUpMessageAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::getFollowUpMessageAsync()");
        }
    }

    CoRoutine<Message> Interactions::editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
        try {
            co_await NewThreadAwaitable<Message>();
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Followup_Message;;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
            workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" + dataPackage.messagePackage.messageId;
            workload.content = DiscordCoreInternal::JSONIFY(dataPackage);
            workload.callStack = "Interactions::editFollowUpMessageAsync";
            co_return DiscordCoreInternal::submitWorkloadAndGetResult<Message>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::editFollowUpMessageAsync()");
        }
    }

    void Interactions::deleteFollowUpMessageToBeWrapped(DeleteFollowUpMessageData dataPackage) {
        try {
            DiscordCoreInternal::HttpWorkloadData workload{};
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Followup_Message;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
            workload.relativePath = "/webhooks/" + dataPackage.interactionPackage.applicationId + "/" + dataPackage.interactionPackage.interactionToken + "/messages/" + dataPackage.messagePackage.messageId;
            workload.callStack = "Interactions::deleteFollowUpMessageToBeWrappe";
            return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Interactions::httpClient, workload);
        }
        catch (...) {
            reportException("Interactions::deleteFollowUpMessageToBeWrapped()");
        }
    }

    CoRoutine<void> Interactions::deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
        try {
            co_await NewThreadAwaitable<void>();
            if (dataPackage.timeDelay > 0) {
                TimeElapsedHandler onSend = [=]() {
                    Interactions::deleteFollowUpMessageToBeWrapped(dataPackage);
                };
                ThreadPoolTimer::executeFunctionAfterTimePeriod(onSend, dataPackage.timeDelay);
            }
            else {
                Interactions::deleteFollowUpMessageToBeWrapped(dataPackage);
            }
            co_return;
        }
        catch (...) {
            reportException("Interactions::deleteFollowUpMessageAsync()");
        }
    }

    SelectMenuCollector::SelectMenuCollector(InputEventData dataPackage) {
        this->channelId = dataPackage.getChannelId();
        this->messageId = dataPackage.getMessageId();
        this->userId = dataPackage.getRequesterId();
        this->selectMenuIncomingInteractionBuffer = std::make_unique<UnboundedMessageBlock<InteractionData>>();
        SelectMenuCollector::selectMenuInteractionBufferMap.insert(std::make_pair(this->channelId + this->messageId, this->selectMenuIncomingInteractionBuffer.get()));
    }

    CoRoutine<std::vector<SelectMenuResponseData>> SelectMenuCollector::collectSelectMenuData(bool getSelectMenuDataForAllNew,  int32_t maxWaitTimeInMsNew, int32_t maxCollectedSelectMenuCountNew, std::string targetUser) {
        co_await NewThreadAwaitable<std::vector<SelectMenuResponseData>>();
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
        if (SelectMenuCollector::selectMenuInteractionBufferMap.contains(this->channelId + this->messageId)) {
            SelectMenuCollector::selectMenuInteractionBufferMap.erase(this->channelId + this->messageId);
        }
    }
    
    void SelectMenuCollector::run() {
            while (!this->doWeQuit) {
                try {
                if (this->getSelectMenuDataForAll == false) {
                    auto buttonInteractionData = std::make_unique<InteractionData>();
                    if (waitForTimeToPass(*this->selectMenuIncomingInteractionBuffer.get(), *buttonInteractionData.get(), this->maxTimeInMs)) {
                        this->selectMenuId = "empty";
                        auto response = std::make_unique<SelectMenuResponseData>();
                        response->selectionId = this->selectMenuId;
                        response->channelId = this->channelId;
                        response->messageId = this->messageId;
                        response->userId = buttonInteractionData->user.id;
                        response->interactionData = *buttonInteractionData;
                        response->values = this->interactionData.data.componentData.values;
                        this->responseVector.push_back(*response);
                        break;
                    }
                    if (buttonInteractionData->user.id != this->userId) {
                        auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
                        auto embedData = std::make_unique<EmbedData>();
                        embedData->setColor("FEFEFE");
                        embedData->setTitle("__**Permission Issue:**__");
                        embedData->setTimeStamp(getTimeAndDate());
                        embedData->setDescription("Sorry, but that menu can only be selected by <@!" + this->userId + ">!");
                        createResponseData->addMessageEmbed(*embedData);
                        createResponseData->data.data.flags = 64;
                        createResponseData->data.type = InteractionCallbackType::ChannelMessageWithSource;
                        Interactions::createInteractionResponseAsync(*createResponseData).get();
                    }
                    else {
                        this->interactionData = *buttonInteractionData;
                        this->selectMenuId = buttonInteractionData->data.componentData.customId;
                        auto response = std::make_unique<SelectMenuResponseData>();
                        response->selectionId = this->selectMenuId;
                        response->channelId = this->channelId;
                        response->messageId = this->messageId;
                        response->userId = buttonInteractionData->user.id;
                        response->values = this->interactionData.data.componentData.values;
                        response->interactionData = *buttonInteractionData;
                        this->responseVector.push_back(*response);
                        this->currentCollectedSelectMenuCount += 1;
                        if (this->currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
                            doWeQuit = true;
                        }
                    }
                }
                else {
                    auto buttonInteractionData = std::make_unique<InteractionData>();
                    if (waitForTimeToPass(*this->selectMenuIncomingInteractionBuffer.get(), *buttonInteractionData.get(), this->maxTimeInMs)) {
                        this->selectMenuId = "empty";
                        auto response = std::make_unique<SelectMenuResponseData>();
                        response->selectionId = this->selectMenuId;
                        response->channelId = this->channelId;
                        response->messageId = this->messageId;
                        response->userId = buttonInteractionData->user.id;
                        response->interactionData = *buttonInteractionData;
                        response->values = this->interactionData.data.componentData.values;
                        this->responseVector.push_back(*response);
                        break;
                    }
                    this->interactionData = *buttonInteractionData;
                    this->selectMenuId = buttonInteractionData->data.componentData.customId;
                    auto response = std::make_unique<SelectMenuResponseData>();
                    response->selectionId = this->selectMenuId;
                    response->channelId = this->channelId;
                    response->messageId = this->messageId;
                    response->userId = buttonInteractionData->user.id;
                    response->interactionData = *buttonInteractionData;
                    response->values = this->interactionData.data.componentData.values;
                    this->responseVector.push_back(*response);
                    this->currentCollectedSelectMenuCount += 1;
                    if (this->currentCollectedSelectMenuCount >= this->maxCollectedSelectMenuCount) {
                        this->doWeQuit = true;
                    }
                }
            }
            catch (...) {
                reportException("SelectMenuCollector::run()");
                SelectMenuCollector::selectMenuInteractionBufferMap.erase(this->channelId + this->messageId);
            }
        }        
            SelectMenuCollector::selectMenuInteractionBufferMap.erase(this->channelId + this->messageId);
    }

    ButtonCollector::ButtonCollector(InputEventData dataPackage) {
        this->channelId = dataPackage.getChannelId();
        this->messageId = dataPackage.getMessageId();
        this->userId = dataPackage.getRequesterId();
        this->interactionData = dataPackage.getInteractionData();
        this->buttonIncomingInteractionBuffer = std::make_unique<UnboundedMessageBlock<InteractionData>>();
        ButtonCollector::buttonInteractionBufferMap.insert_or_assign(this->channelId + this->messageId, this->buttonIncomingInteractionBuffer.get());
    }

    CoRoutine<std::vector<ButtonResponseData>> ButtonCollector::collectButtonData(bool getButtonDataForAllNew,  int32_t maxWaitTimeInMsNew, int32_t maxNumberOfPressesNew, std::string targetUser) {
        co_await NewThreadAwaitable<std::vector<ButtonResponseData>>();
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
            try {
                if (this->getButtonDataForAll == false) {
                    auto buttonInteractionData = std::make_unique<InteractionData>();
                    if (waitForTimeToPass(*this->buttonIncomingInteractionBuffer.get(), *buttonInteractionData.get(), this->maxTimeInMs)) {
                        this->buttonId = "empty";
                        auto response = std::make_unique<ButtonResponseData>();
                        response->buttonId = this->buttonId;
                        response->channelId = this->channelId;
                        response->messageId = this->messageId;
                        response->userId = buttonInteractionData->user.id;
                        response->interactionData = this->interactionData;
                        this->responseVector.push_back(*response);
                        break;
                    }
                    if (buttonInteractionData->user.id != this->userId) {
                        auto createResponseData = std::make_unique<CreateInteractionResponseData>(*buttonInteractionData);
                        auto embedData = std::make_unique<EmbedData>();
                        embedData->setColor("FEFEFE");
                        embedData->setTitle("__**Permission Issue:**__");
                        embedData->setTimeStamp(getTimeAndDate());
                        embedData->setDescription("Sorry, but that button can only be pressed by <@!" + this->userId + ">!");
                        createResponseData->addMessageEmbed(*embedData);
                        createResponseData->data.data.flags = 64;
                        createResponseData->data.type = InteractionCallbackType::ChannelMessageWithSource;
                        Interactions::createInteractionResponseAsync(*createResponseData).get();
                    }
                    else {
                        this->interactionData = *buttonInteractionData;
                        this->buttonId = buttonInteractionData->data.componentData.customId;
                        auto response = std::make_unique<ButtonResponseData>();
                        response->buttonId = this->buttonId;
                        response->channelId = this->channelId;
                        response->messageId = this->messageId;
                        response->userId = buttonInteractionData->user.id;
                        response->interactionData = *buttonInteractionData;
                        this->responseVector.push_back(*response);
                        this->currentCollectedButtonCount += 1;
                        if (this->currentCollectedButtonCount >= this->maxCollectedButtonCount) {
                            doWeQuit = true;
                        }
                    }
                }
                else {
                    auto buttonInteractionData = std::make_unique<InteractionData>();
                    if (waitForTimeToPass(*this->buttonIncomingInteractionBuffer.get(), *buttonInteractionData.get(), this->maxTimeInMs)) {
                        this->buttonId = "empty";
                        auto response = std::make_unique<ButtonResponseData>();
                        response->buttonId = this->buttonId;
                        response->channelId = this->channelId;
                        response->messageId = this->messageId;
                        response->userId = buttonInteractionData->user.id;
                        response->interactionData = *buttonInteractionData;
                        this->responseVector.push_back(*response);
                        break;
                    }
                    this->interactionData = *buttonInteractionData;
                    this->buttonId = buttonInteractionData->data.componentData.customId;
                    auto response = std::make_unique<ButtonResponseData>();
                    response->buttonId = this->buttonId;
                    response->channelId = this->channelId;
                    response->messageId = this->messageId;
                    response->userId = buttonInteractionData->user.id;
                    response->interactionData = *buttonInteractionData;
                    this->responseVector.push_back(*response);
                    this->currentCollectedButtonCount += 1;
                    if (this->currentCollectedButtonCount >= this->maxCollectedButtonCount) {
                        this->doWeQuit = true;
                    }
                }
                
            }
        catch (...) {
            reportException("ButtonCollector::run()");
            ButtonCollector::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
        }
    }
        
        ButtonCollector::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
    }
    
    std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> SelectMenuCollector::selectMenuInteractionBufferMap{};
    std::unordered_map<std::string, std::unique_ptr<UnboundedMessageBlock<MessageData>>> Interactions::collectMessageDataBuffers{};
    std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> ButtonCollector::buttonInteractionBufferMap{};
    DiscordCoreInternal::HttpClient* Interactions::httpClient{ nullptr };
};