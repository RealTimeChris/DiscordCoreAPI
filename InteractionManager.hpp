// InteractionManager.hpp - Header for the interaction manager class.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INTERACTION_MANAGER_
#define _INTERACTION_MANAGER_

#include "../pch.h"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "ThreadManager.hpp"

namespace DiscordCoreAPI {

    class DiscordCoreClient;

    class InteractionManager;

    class Button;

    class InteractionManagerAgent : public agent {
    public:
        static map<string, unbounded_buffer<MessageData>*> collectMessageDataBuffers;
        unbounded_buffer<DiscordCoreInternal::PostDeferredInteractionResponseData> requestPostDeferredInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData> requestDeleteInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData> requestPatchInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::PostInteractionResponseData> requestPostInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::GetInteractionResponseData> requestGetInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::DeleteFollowUpMessageData> requestDeleteFollowUpMessageBuffer;
        unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData> requestPatchFollowUpMessageBuffer;
        unbounded_buffer<DiscordCoreInternal::PostFollowUpMessageData> requestPostFollowUpMessageBuffer;
        unbounded_buffer<InteractionResponseData> outInteractionresponseDataBuffer;
        unbounded_buffer<MessageData> outInteractionResponseBuffer;
        unbounded_buffer<exception> errorBuffer;

        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        DiscordCoreInternal::HttpAgentResources agentResources;
        ScheduleGroup* pScheduleGroup{ nullptr };

        InteractionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew)
            :agent(*InteractionManagerAgent::threadContext->scheduler) {
            this->agentResources = agentResourcesNew;
        }

        static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew){
            InteractionManagerAgent::threadContext = threadContextNew;
        }

        static void cleanup(){
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
            DiscordCoreInternal::parseObject(returnData.data, &interactionResponseData);
            return interactionResponseData;
        }

        MessageData patchObjectData(DiscordCoreInternal::EditFollowUpMessageData dataPackage){
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_FOLLOW_UP_MESSAGE;
            workload.content = dataPackage.finalContent;
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
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
            return messageData;
        }

        MessageData patchObjectData(DiscordCoreInternal::EditInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
            workload.content = dataPackage.finalContent;
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
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
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
            return ;
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
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
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
                DiscordCoreInternal::EditInteractionResponseData dataPackage01;
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
                DiscordCoreInternal::EditFollowUpMessageData dataPackage05;
                if (try_receive(this->requestPatchFollowUpMessageBuffer, dataPackage05)) {
                    MessageData messageData = patchObjectData(dataPackage05);
                    send(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::PostDeferredInteractionResponseData dataPackage06;
                if (try_receive(this->requestPostDeferredInteractionResponseBuffer, dataPackage06)) {
                    postObjectData(dataPackage06);
                }
                DiscordCoreInternal::DeleteFollowUpMessageData dataPackage07;
                if(try_receive(this->requestDeleteFollowUpMessageBuffer, dataPackage07)){
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
            InteractionManagerAgent::collectMessageDataBuffers.insert(make_pair(dataPackage.interactionPackage.interactionId, &requestAgent.outInteractionResponseBuffer));
            send(requestAgent.requestPostInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::createInteractionResponseAsync");
            MessageData messageData;
            if (dataPackage.type == InteractionCallbackType::ChannelMessage || dataPackage.type == InteractionCallbackType::ChannelMessageWithSource) {
                try {
                    messageData = receive(requestAgent.outInteractionResponseBuffer, 1000);
                }
                catch (exception&) {};
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
            DiscordCoreInternal::EditInteractionResponseData dataPackageNew;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.allowedMentions = dataPackage.allowedMentions;
            for (auto value : dataPackage.attachments) {
                dataPackageNew.attachments.push_back(value);
            }
            for (auto value : dataPackage.components) {
                dataPackageNew.components.push_back(value);
            }
            dataPackageNew.content = dataPackage.content;
            for (auto value : dataPackage.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.flags = dataPackage.flags;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.finalContent = DiscordCoreInternal::getEditInteractionResponsePayload(dataPackageNew);
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
            DiscordCoreInternal::EditFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.allowedMentions = dataPackage.allowedMentions;
            dataPackageNew.avatarUrl = dataPackage.avatarUrl;
            for (auto value : dataPackage.components) {
                dataPackageNew.components.push_back(value);
            }
            for (auto value : dataPackage.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.content = dataPackage.content;
            dataPackageNew.flags = dataPackage.flags;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.messageId = dataPackage.messagePackage.messageId;
            dataPackageNew.tts = dataPackage.tts;
            dataPackageNew.username = dataPackage.username;
            dataPackageNew.finalContent = DiscordCoreInternal::getEditFollowUpMessagePayload(dataPackageNew);
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

        InteractionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
        }

        ~InteractionManager() {
            this->threadContext->releaseGroup();
        }

        protected:
            friend class InteractionManagerAgent;
            friend class DiscordCoreClient;

            DiscordCoreInternal::HttpAgentResources agentResources;
            shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
    };

    class SelectMenu : public agent {
    public:
        static map<string, unbounded_buffer<SelectMenuInteractionData>*>selectMenuInteractionMap;

        SelectMenu(InputEventData dataPackage) : agent(*SelectMenu::threadContext->schedulerGroup) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->selectMenuIncomingInteractionBuffer = new unbounded_buffer<SelectMenuInteractionData>;
            SelectMenu::selectMenuInteractionMap.insert(make_pair(this->channelId + this->messageId, this->selectMenuIncomingInteractionBuffer));
        }

        static void initialize(shared_ptr<InteractionManager> interactionsNew) {
            SelectMenu::interactions = interactionsNew;
            SelectMenu::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
        }

        static void cleanup() {
            SelectMenu::threadContext->releaseGroup();
        }

        ~SelectMenu() {
            done();
        }

        string getSelectMenuId() {
            return this->selectMenuId;
        }

        SelectMenuInteractionData getOurSelectMenuData(bool getButtonDataForAllNew, unsigned int maxWaitTimeInMsNew, string targetUser = "") {
            if (targetUser != "") {
                this->userId = targetUser;
            }
            this->maxTimeInMs = maxWaitTimeInMsNew;
            this->getButtonDataForAll = getButtonDataForAllNew;
            start();
            agent::wait(this);
            exception error;
            while (getError(error)) {
                cout << "SelectMenu::run() Error: " << error.what() << endl;
            }
            return this->interactionData;
        }

    protected:
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static shared_ptr<InteractionManager> interactions;
        unsigned int maxTimeInMs;
        bool getButtonDataForAll;
        SelectMenuInteractionData interactionData;
        unbounded_buffer<SelectMenuInteractionData>* selectMenuIncomingInteractionBuffer{ nullptr };
        unbounded_buffer<exception> errorBuffer;
        string channelId;
        string messageId;
        string userId;
        string selectMenuId = "";
        bool doWeQuit = false;

        bool getError(exception& error) {
            return try_receive(this->errorBuffer, error);
        }

        void run() {
            try {
                while (doWeQuit == false) {
                    if (this->getButtonDataForAll == false) {
                        SelectMenuInteractionData selectMenuInteractionData = receive(SelectMenu::selectMenuIncomingInteractionBuffer, this->maxTimeInMs);
                        if (selectMenuInteractionData.user.id != this->userId) {
                            CreateInteractionResponseData createResponseData(selectMenuInteractionData);
                            createResponseData.data.flags = 64;
                            EmbedData embedData;
                            embedData.setColor("FEFEFE");
                            embedData.setTitle("__**Permission Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that menu can only be selected by <@!" + this->userId + ">!");
                            createResponseData.data.embeds.push_back(embedData);
                            SelectMenu::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = selectMenuInteractionData;
                            this->selectMenuId = selectMenuInteractionData.customId;
                            CreateInteractionResponseData dataPackage(selectMenuInteractionData);
                            dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                            SelectMenu::interactions->createInteractionResponseAsync(dataPackage);
                            doWeQuit = true;
                        }
                    }
                    else {
                        SelectMenuInteractionData selectMenuInteractionData = receive(SelectMenu::selectMenuIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = selectMenuInteractionData;
                        this->selectMenuId = selectMenuInteractionData.customId;
                        CreateInteractionResponseData dataPackage(selectMenuInteractionData);
                        dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                        SelectMenu::interactions->createInteractionResponseAsync(dataPackage);
                        doWeQuit = true;
                    }
                }
                done();
                SelectMenu::selectMenuInteractionMap.erase(this->channelId + this->messageId);
            }
            catch (exception&e) {
                this->selectMenuId = "exit";
                send(this->errorBuffer, e);
                done();
                SelectMenu::selectMenuInteractionMap.erase(this->channelId + this->messageId);
                return;
            }
        }
    };

    class Button : public agent {
    public:
        static map<string, unbounded_buffer<ButtonInteractionData>*> buttonInteractionMap;

        Button(InputEventData dataPackage) : agent(*Button::threadContext->scheduler) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->buttonIncomingInteractionBuffer = new unbounded_buffer<ButtonInteractionData>;
            Button::buttonInteractionMap.insert(make_pair(this->channelId + this->messageId, this->buttonIncomingInteractionBuffer));
        }

        static void initialize(shared_ptr<InteractionManager> interactionsNew) {
            Button::interactions = interactionsNew;
            Button::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
        }

        static void cleanup() {
            Button::threadContext->releaseGroup();
        }

        ~Button() {
            done();
        }

        string getButtonId() {
            return this->buttonId;
        }

        ButtonInteractionData getOurButtonData(bool getButtonDataForAllNew, unsigned int maxWaitTimeInMsNew, string targetUser = "") {
            if (targetUser != "") {
                this->userId = targetUser;
            }
            this->maxTimeInMs = maxWaitTimeInMsNew;
            this->getButtonDataForAll = getButtonDataForAllNew;
            start();
            agent::wait(this);
            exception error;
            while (getError(error)) {
                cout << "Button::run() Error: " << error.what() << endl;
            }
            return this->interactionData;
        }

    protected:
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static shared_ptr<InteractionManager> interactions;
        unsigned int maxTimeInMs;
        bool getButtonDataForAll;
        ButtonInteractionData interactionData;
        unbounded_buffer<ButtonInteractionData>* buttonIncomingInteractionBuffer{ nullptr };
        unbounded_buffer<exception> errorBuffer;
        string channelId;
        string messageId;
        string userId;
        string buttonId = "";
        bool doWeQuit = false;

        bool getError(exception& error) {
            return try_receive(this->errorBuffer, error);
        }

        void run() {
            try {
                while (doWeQuit == false) {
                    if (this->getButtonDataForAll == false) {
                        ButtonInteractionData buttonInteractionData = receive(Button::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        if (buttonInteractionData.user.id != this->userId) {
                            CreateInteractionResponseData createResponseData(buttonInteractionData);
                            createResponseData.data.flags = 64;
                            EmbedData embedData;
                            embedData.setColor("FEFEFE");
                            embedData.setTitle("__**Permission Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that button can only be pressed by <@!" + this->userId + ">!");
                            createResponseData.data.embeds.push_back(embedData);
                            Button::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = buttonInteractionData;
                            this->buttonId = buttonInteractionData.customId;
                            CreateInteractionResponseData dataPackage(buttonInteractionData);
                            dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                            Button::interactions->createInteractionResponseAsync(dataPackage);
                            doWeQuit = true;
                        }
                    }
                    else {
                        ButtonInteractionData buttonInteractionData = receive(Button::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = buttonInteractionData;
                        this->buttonId = buttonInteractionData.customId;
                        CreateInteractionResponseData dataPackage(buttonInteractionData);
                        dataPackage.type = InteractionCallbackType::DeferredUpdateMessage;
                        Button::interactions->createInteractionResponseAsync(dataPackage);
                        doWeQuit = true;
                    }
                }
                done();
                Button::buttonInteractionMap.erase(this->channelId + this->messageId);
            }
            catch (exception&e) {
                this->buttonId = "exit";
                send(this->errorBuffer, e);
                done();

                Button::buttonInteractionMap.erase(this->channelId + this->messageId);
                return;
            }
        }
    };
    map<string, unbounded_buffer<ButtonInteractionData>*> Button::buttonInteractionMap;
    shared_ptr<InteractionManager> Button::interactions{ nullptr };
    shared_ptr<DiscordCoreInternal::ThreadContext> Button::threadContext{ nullptr };
    map<string, unbounded_buffer<SelectMenuInteractionData>*> SelectMenu::selectMenuInteractionMap;
    shared_ptr<InteractionManager> SelectMenu::interactions{ nullptr };
    shared_ptr<DiscordCoreInternal::ThreadContext> SelectMenu::threadContext{ nullptr };
    map<string, unbounded_buffer<MessageData>*> InteractionManagerAgent::collectMessageDataBuffers;
    shared_ptr<DiscordCoreInternal::ThreadContext> InteractionManagerAgent::threadContext;
};
#endif