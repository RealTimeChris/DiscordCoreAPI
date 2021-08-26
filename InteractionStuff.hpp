// InteractionStuff.hpp - Header for the interaction manager class.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INTERACTION_STUFF_
#define _INTERACTION_STUFF_

#include "../pch.h"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "ThreadManager.hpp"

namespace DiscordCoreAPI {

    class DiscordCoreClient;

    class InteractionManager;

    class InteractionManagerAgent : public agent {
    public:

        static map<string, shared_ptr<unbounded_buffer<MessageData>>> collectMessageDataBuffers;
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;

        unbounded_buffer<DiscordCoreInternal::PostDeferredInteractionResponseData> requestPostDeferredInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData> requestDeleteInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PatchInteractionResponseData> requestPatchInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PostInteractionResponseData> requestPostInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::GetInteractionResponseData> requestGetInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::DeleteFollowUpMessageData> requestDeleteFollowUpMessageBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PatchFollowUpMessageData> requestPatchFollowUpMessageBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PostFollowUpMessageData> requestPostFollowUpMessageBuffer{ nullptr };
        unbounded_buffer<InteractionResponseData> outInteractionresponseDataBuffer{ nullptr };
        unbounded_buffer<MessageData> outInteractionResponseBuffer{ nullptr };
        DiscordCoreInternal::HttpAgentResources agentResources{};
        unbounded_buffer<exception> errorBuffer{ nullptr };

        InteractionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew)
            :agent(*InteractionManagerAgent::threadContext->scheduler) {
            this->agentResources = agentResourcesNew;
        }

        static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            InteractionManagerAgent::threadContext = threadContextNew;
        }

        static void cleanup() {
            InteractionManagerAgent::threadContext->releaseGroup();
        }

        void getError(string stackTrace) {
            exception error;
            while (try_receive(errorBuffer, error)) {
                cout << stackTrace + "::InteractionManagerAgent Error: " << error.what() << endl << endl;
            }
            return;
        }

        InteractionResponseData getObjectData(DiscordCoreInternal::GetInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_INTERACTION_RESPONSE;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::getObjectData_00");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::getObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::getObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreAPI::InteractionResponseData interactionResponseData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &interactionResponseData);
            return interactionResponseData;
        }

        MessageData patchObjectData(DiscordCoreInternal::PatchFollowUpMessageData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_FOLLOW_UP_MESSAGE;
            workload.content = dataPackage.content;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::patchObjectData_00");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::patchObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::patchObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreAPI::MessageData messageData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
            return messageData;
        }

        MessageData patchObjectData(DiscordCoreInternal::PatchInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
            workload.content = dataPackage.content;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::patchObjectData_01");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::patchObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::patchObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            MessageData messageData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
            return messageData;
        }

        void postObjectData(DiscordCoreInternal::PostInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_INTERACTION_RESPONSE;
            workload.content = dataPackage.content;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::postObjectData_00");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        void postObjectData(DiscordCoreInternal::PostDeferredInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_DEFERRED_INTERACTION_RESPONSE;
            workload.content = dataPackage.content;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::postObjectData_01");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        MessageData postObjectData(DiscordCoreInternal::PostFollowUpMessageData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_FOLLOW_UP_MESSAGE;
            workload.content = dataPackage.content;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::postObjectData_02");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            MessageData messageData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
            return messageData;
        }

        void deleteObjectData(DiscordCoreInternal::DeleteInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_INTERACTION;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::deleteObjectData_00");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::deleteObjectData_00 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::deleteObjectData_00 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        void deleteObjectDataTimer(DiscordCoreInternal::DeleteInteractionResponseData dataPackage) {
            if (dataPackage.timeDelayInMs > 0) {
                ThreadPoolTimer threadPoolTimer = ThreadPoolTimer(nullptr);
                TimerElapsedHandler onSend = [=](ThreadPoolTimer threadPoolTimerNew) {
                    deleteObjectData(dataPackage);
                };
                threadPoolTimer = threadPoolTimer.CreateTimer(onSend, winrt::Windows::Foundation::TimeSpan(dataPackage.timeDelayInMs * 10000));
            }
            else {
                deleteObjectData(dataPackage);
            }
        }

        void deleteObjectData(DiscordCoreInternal::DeleteFollowUpMessageData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_FOLLOW_UP_MESSAGE;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManagerAgent::deleteObjectData_01");
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::deleteObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::deleteObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        void deleteObjectDataTimer(DiscordCoreInternal::DeleteFollowUpMessageData dataPackage) {
            if (dataPackage.timeDelayInMs > 0) {
                ThreadPoolTimer threadPoolTimer = ThreadPoolTimer(nullptr);
                TimerElapsedHandler onSend = [=](ThreadPoolTimer threadPoolTimerNew) {
                    deleteObjectData(dataPackage);
                };
                threadPoolTimer = threadPoolTimer.CreateTimer(onSend, winrt::Windows::Foundation::TimeSpan(dataPackage.timeDelayInMs * 10000));
            }
            else {
                deleteObjectData(dataPackage);
            }
        }

    protected:

        void run() {
            try {
                DiscordCoreInternal::PatchInteractionResponseData dataPackage01;
                if (try_receive(this->requestPatchInteractionResponseBuffer, dataPackage01)) {
                    MessageData messageData = patchObjectData(dataPackage01);
                    send(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::PostInteractionResponseData dataPackage02;
                if (try_receive(this->requestPostInteractionResponseBuffer, dataPackage02)) {
                    postObjectData(dataPackage02);
                }
                DiscordCoreInternal::PostFollowUpMessageData dataPackage03;
                if (try_receive(this->requestPostFollowUpMessageBuffer, dataPackage03)) {
                    MessageData messageData = postObjectData(dataPackage03);
                    send(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::DeleteInteractionResponseData dataPackage04;
                if (try_receive(this->requestDeleteInteractionResponseBuffer, dataPackage04)) {
                    deleteObjectDataTimer(dataPackage04);
                }
                DiscordCoreInternal::PatchFollowUpMessageData dataPackage05;
                if (try_receive(this->requestPatchFollowUpMessageBuffer, dataPackage05)) {
                    MessageData messageData = patchObjectData(dataPackage05);
                    send(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::PostDeferredInteractionResponseData dataPackage06;
                if (try_receive(this->requestPostDeferredInteractionResponseBuffer, dataPackage06)) {
                    postObjectData(dataPackage06);
                }
                DiscordCoreInternal::DeleteFollowUpMessageData dataPackage07;
                if (try_receive(this->requestDeleteFollowUpMessageBuffer, dataPackage07)) {
                    deleteObjectDataTimer(dataPackage07);
                }
                DiscordCoreInternal::GetInteractionResponseData dataPackage08;
                if (try_receive(this->requestGetInteractionResponseBuffer, dataPackage08)) {
                    auto responseData = getObjectData(dataPackage08);
                    send(this->outInteractionresponseDataBuffer, responseData);
                }
            }
            catch (const exception& e) {
                send(this->errorBuffer, e);
            }
            done();
        }

    };

    class InteractionManager {
    public:

        InteractionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
        }

        task<void> createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PostDeferredInteractionResponseData dataPackageNew;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.content = DiscordCoreInternal::getCreateDeferredInteractionResponsePayload(dataPackage);
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPostDeferredInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::createDeferredInteractionResponseAsync");
            co_return;
        }

        task<MessageData> createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
            co_await resume_background();
            DiscordCoreInternal::PostInteractionResponseData dataPackageNew;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            DiscordCoreInternal::HttpAgentResources httpAgentResources;
            dataPackageNew.agentResources = httpAgentResources;
            dataPackageNew.content = DiscordCoreInternal::getCreateInteractionResponsePayload(dataPackage);
            InteractionManagerAgent requestAgent(httpAgentResources);
            send(requestAgent.requestPostInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::createInteractionResponseAsync");
            MessageData messageData;
            if (dataPackage.type == InteractionCallbackType::ChannelMessage || dataPackage.type == InteractionCallbackType::ChannelMessageWithSource) {
                if (InteractionManagerAgent::collectMessageDataBuffers.contains(dataPackage.interactionPackage.interactionId)) {
                    shared_ptr<unbounded_buffer<MessageData>> messageBlock = InteractionManagerAgent::collectMessageDataBuffers.at(dataPackage.interactionPackage.interactionId);
                    try {
                        messageData = receive(*messageBlock, 1000);
                    }
                    catch (exception&) {};
                }
                
            }
            InteractionManagerAgent::collectMessageDataBuffers.erase(dataPackage.interactionPackage.interactionId);
            co_return messageData;
        }

        task<InteractionResponseData> getInteractionResponseAsync(GetInteractionResponseData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::GetInteractionResponseData dataPackageNew;
            dataPackageNew.applicationId = dataPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionToken;
            dataPackageNew.agentResources = this->agentResources;
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestGetInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::getInteractionResponseAsync");
            InteractionResponseData outData;
            try_receive(requestAgent.outInteractionresponseDataBuffer, outData);
            co_return outData;
        }

        task<MessageData> editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PatchInteractionResponseData dataPackageNew;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.content = DiscordCoreInternal::getEditInteractionResponsePayload(dataPackage);
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPatchInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::editInteractionResponseAsync");
            MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            co_return messageData;
        }

        task<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::DeleteInteractionResponseData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.timeDelayInMs = dataPackage.timeDelay;
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestDeleteInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::deleteInteractionResponseAsync");
            co_return;
        }

        task<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PostFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.content = dataPackage.content;
            dataPackageNew.content = DiscordCoreInternal::getPostFollowUpMessagePayload(dataPackage);
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPostFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::createFollowUpMessageAsync");
            MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            co_return messageData;
        }

        task<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PatchFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.content = dataPackage.content;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.messageId = dataPackage.messagePackage.messageId;
            dataPackageNew.content = DiscordCoreInternal::getEditFollowUpMessagePayload(dataPackage);
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPatchFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::editFollowUpMessageAsync");
            MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            co_return messageData;
        }

        task<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::DeleteFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.timeDelayInMs = dataPackage.timeDelay;
            dataPackageNew.messageId = dataPackage.messagePackage.messageId;
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestDeleteFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::deleteFollowUpMessageAsync");
            co_return;
        }

        ~InteractionManager() {
            this->threadContext->releaseGroup();
        }

    protected:
        friend class InteractionManagerAgent;
        friend class DiscordCoreClient;

        shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
        DiscordCoreInternal::HttpAgentResources agentResources{};
    };

    struct SelectMenuResponseData {
        string selectionId{ "" };
        vector<string> values{};
        string channelId{ "" };
        string messageId{ "" };
        string userId{ "" };
    };

    class SelectMenuManager : public agent {
    public:
        static map<string, unbounded_buffer<SelectMenuInteractionData>*>selectMenuInteractionBufferMap;

        SelectMenuManager(InputEventData dataPackage) : agent(*SelectMenuManager::threadContext->schedulerGroup) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->selectMenuIncomingInteractionBuffer = new unbounded_buffer<SelectMenuInteractionData>;
            SelectMenuManager::selectMenuInteractionBufferMap.insert(make_pair(this->channelId + this->messageId, this->selectMenuIncomingInteractionBuffer));
        }

        static void initialize(shared_ptr<InteractionManager> interactionsNew) {
            SelectMenuManager::interactions = interactionsNew;
            SelectMenuManager::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
        }

        static void cleanup() {
            SelectMenuManager::threadContext->releaseGroup();
        }

        string getSelectMenuId() {
            return this->selectMenuId;
        }

        vector<SelectMenuResponseData> collectSelectMenuData(bool getButtonDataForAllNew, unsigned int maxWaitTimeInMsNew, string targetUser = "") {
            if (targetUser != "") {
                this->userId = targetUser;
            }
            this->maxTimeInMs = maxWaitTimeInMsNew;
            this->getButtonDataForAll = getButtonDataForAllNew;
            start();
            agent::wait(this);
            exception error;
            while (getError(error)) {
                cout << "SelectMenuManager::run() Error: " << error.what() << endl;
            }
            return this->responseVector;
        }

        ~SelectMenuManager() {
            done();
        }

    protected:
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static shared_ptr<InteractionManager> interactions;
        unbounded_buffer<SelectMenuInteractionData>* selectMenuIncomingInteractionBuffer{ nullptr };
        unbounded_buffer<exception> errorBuffer{ nullptr };
        vector<SelectMenuResponseData> responseVector{};
        SelectMenuInteractionData interactionData{};
        bool getButtonDataForAll{ false };
        unsigned int maxTimeInMs{ 0 };
        string selectMenuId{ "" };
        vector<string> values{};
        bool doWeQuit{ false };
        string channelId{ "" };
        string messageId{ "" };
        string userId{ "" };

        bool getError(exception& error) {
            return try_receive(this->errorBuffer, error);
        }

        void run() {
            try {
                while (doWeQuit == false) {
                    if (this->getButtonDataForAll == false) {
                        SelectMenuInteractionData selectMenuInteractionData = receive(SelectMenuManager::selectMenuIncomingInteractionBuffer, this->maxTimeInMs);
                        if (selectMenuInteractionData.user.id != this->userId) {
                            CreateInteractionResponseData createResponseData(selectMenuInteractionData);
                            createResponseData.data.flags = 64;
                            EmbedData embedData;
                            embedData.setColor("FEFEFE");
                            embedData.setTitle("__**Permission Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that menu can only be selected by <@!" + this->userId + ">!");
                            createResponseData.data.embeds.push_back(embedData);
                            SelectMenuManager::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = selectMenuInteractionData;
                            this->values = selectMenuInteractionData.values;
                            this->selectMenuId = selectMenuInteractionData.customId;
                            CreateInteractionResponseData dataPackage(selectMenuInteractionData);
                            dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                            SelectMenuManager::interactions->createInteractionResponseAsync(dataPackage);
                            SelectMenuResponseData response;
                            response.selectionId = this->selectMenuId;
                            response.channelId = this->channelId;
                            response.messageId = this->messageId;
                            response.userId = selectMenuInteractionData.user.id;
                            response.values = this->values;
                            this->responseVector.push_back(response);
                            doWeQuit = true;
                        }
                    }
                    else {
                        SelectMenuInteractionData selectMenuInteractionData = receive(SelectMenuManager::selectMenuIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = selectMenuInteractionData;
                        this->selectMenuId = selectMenuInteractionData.customId;
                        this->values = selectMenuInteractionData.values;
                        SelectMenuResponseData response;
                        response.selectionId = this->selectMenuId;
                        response.channelId = this->channelId;
                        response.messageId = this->messageId;
                        response.values = this->values;
                        response.userId = selectMenuInteractionData.user.id;
                        this->responseVector.push_back(response);
                        CreateInteractionResponseData dataPackage(selectMenuInteractionData);
                        dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                        SelectMenuManager::interactions->createInteractionResponseAsync(dataPackage);
                    }
                }
                SelectMenuManager::selectMenuInteractionBufferMap.erase(this->channelId + this->messageId);
                done();
            }
            catch (exception& e) {
                this->selectMenuId = "exit";
                send(this->errorBuffer, e);
                done();
                SelectMenuManager::selectMenuInteractionBufferMap.erase(this->channelId + this->messageId);
                return;
            }
        }
    };

    struct ButtonResponseData {
        string channelId{ "" };
        string messageId{ "" };
        string buttonId{ "" };
        string userId{ "" };
    };

    class ButtonManager : public agent {
    public:
        static map<string, unbounded_buffer<ButtonInteractionData>*> buttonInteractionBufferMap;

        ButtonManager(InputEventData dataPackage) : agent(*ButtonManager::threadContext->scheduler) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->buttonIncomingInteractionBuffer = new unbounded_buffer<ButtonInteractionData>;
            ButtonManager::buttonInteractionBufferMap.insert(make_pair(this->channelId + this->messageId, this->buttonIncomingInteractionBuffer));
        }

        static void initialize(shared_ptr<InteractionManager> interactionsNew) {
            ButtonManager::interactions = interactionsNew;
            ButtonManager::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
        }

        static void cleanup() {
            ButtonManager::threadContext->releaseGroup();
        }

        string getButtonId() {
            return this->buttonId;
        }

        vector<ButtonResponseData> collectButtonData(bool getButtonDataForAllNew, unsigned int maxWaitTimeInMsNew, string targetUser = "") {
            if (targetUser != "") {
                this->userId = targetUser;
            }
            this->maxTimeInMs = maxWaitTimeInMsNew;
            this->getButtonDataForAll = getButtonDataForAllNew;
            start();
            agent::wait(this);
            exception error;
            while (getError(error)) {
                cout << "ButtonManager::run() Error: " << error.what() << endl;
            }
            return this->responseVector;
        }

        ~ButtonManager() {
            done();
        }

    protected:
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static shared_ptr<InteractionManager> interactions;
        unbounded_buffer<ButtonInteractionData>* buttonIncomingInteractionBuffer{ nullptr };
        unbounded_buffer<exception> errorBuffer{ nullptr };
        vector<ButtonResponseData> responseVector{};
        ButtonInteractionData interactionData{};
        bool getButtonDataForAll{ false };
        unsigned int maxTimeInMs{ 0 };
        string channelId{ "" };
        string messageId{ "" };
        bool doWeQuit{ false };
        string buttonId{ "" };
        string userId{ "" };

        bool getError(exception& error) {
            return try_receive(this->errorBuffer, error);
        }

        void run() {
            try {
                while (doWeQuit == false) {
                    if (this->getButtonDataForAll == false) {
                        ButtonInteractionData buttonInteractionData = receive(ButtonManager::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        if (buttonInteractionData.user.id != this->userId) {
                            CreateInteractionResponseData createResponseData(buttonInteractionData);
                            createResponseData.data.flags = 64;
                            EmbedData embedData;
                            embedData.setColor("FEFEFE");
                            embedData.setTitle("__**Permission Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that button can only be pressed by <@!" + this->userId + ">!");
                            createResponseData.data.embeds.push_back(embedData);
                            ButtonManager::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = buttonInteractionData;
                            this->buttonId = buttonInteractionData.customId;
                            CreateInteractionResponseData dataPackage(buttonInteractionData);
                            dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                            ButtonManager::interactions->createInteractionResponseAsync(dataPackage);
                            ButtonResponseData response;
                            response.buttonId = this->buttonId;
                            response.channelId = this->channelId;
                            response.messageId = this->messageId;
                            response.userId = buttonInteractionData.user.id;
                            this->responseVector.push_back(response);
                            doWeQuit = true;
                        }
                    }
                    else {
                        ButtonInteractionData buttonInteractionData = receive(ButtonManager::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = buttonInteractionData;
                        this->buttonId = buttonInteractionData.customId;
                        ButtonResponseData response;
                        response.buttonId = this->buttonId;
                        response.channelId = this->channelId;
                        response.messageId = this->messageId;
                        response.userId = buttonInteractionData.user.id;
                        this->responseVector.push_back(response);
                        CreateInteractionResponseData dataPackage(buttonInteractionData);
                        dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                        ButtonManager::interactions->createInteractionResponseAsync(dataPackage);
                    }
                }
                ButtonManager::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
                done();
            }
            catch (exception& e) {
                this->buttonId = "exit";
                send(this->errorBuffer, e);
                done();

                ButtonManager::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
                return;
            }
        }
    };
    map<string, unbounded_buffer<ButtonInteractionData>*> ButtonManager::buttonInteractionBufferMap{};
    shared_ptr<DiscordCoreInternal::ThreadContext> ButtonManager::threadContext{ nullptr };
    shared_ptr<InteractionManager> ButtonManager::interactions{ nullptr };
    map<string, unbounded_buffer<SelectMenuInteractionData>*> SelectMenuManager::selectMenuInteractionBufferMap{};
    shared_ptr<DiscordCoreInternal::ThreadContext> SelectMenuManager::threadContext{ nullptr };
    shared_ptr<InteractionManager> SelectMenuManager::interactions{ nullptr };
    map<string, shared_ptr<unbounded_buffer<MessageData>>> InteractionManagerAgent::collectMessageDataBuffers{};
    shared_ptr<DiscordCoreInternal::ThreadContext> InteractionManagerAgent::threadContext{ nullptr };
};
#endif