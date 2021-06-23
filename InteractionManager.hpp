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

    struct ButtonInteractionData {
        ButtonInteractionData() { };
        ButtonInteractionData(const ButtonInteractionData& dataPackage) {
            this->applicationId = dataPackage.applicationId;
            this->channelId = dataPackage.channelId;
            this->customId = dataPackage.customId;
            this->guildId = dataPackage.guildId;
            this->id = dataPackage.id;
            this->isItForHere = dataPackage.isItForHere;
            this->member = dataPackage.member;
            this->message = dataPackage.message;
            this->token = dataPackage.token;
            this->type = dataPackage.type;
            this->user = dataPackage.user;
        }
        bool isItForHere;
        string token;
        string id;
        string applicationId;
        InteractionType type;
        string customId;
        GuildMemberData member;
        UserData user;
        MessageData message;
        string channelId;
        string guildId;
    };

    struct DeferButtonResponseData {
        DeferButtonResponseData(ButtonInteractionData dataPackage) {
            this->type = InteractionCallbackType::DeferredUpdateMessage;
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->responseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
        }
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionPackageData interactionPackage;
        InteractionCallbackType type;
        InputEventResponseType responseType;
    };

    struct CreateDeferredInteractionResponseData {
        CreateDeferredInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->type = InteractionCallbackType::DeferredChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }
        string requesterId;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionPackageData interactionPackage;
        InteractionCallbackType type;
    };

    struct CreateInteractionResponseData {
    public:
        CreateInteractionResponseData(InteractionPackageData dataPackage) {
            this->interactionPackage = dataPackage;
        }
        CreateInteractionResponseData(ButtonInteractionData dataPackage) {
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->type = InteractionCallbackType::ChannelMessageWithSource;
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
        }
        CreateInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.components.push_back(actionRowData);
            }
            if (this->data.components.size() < 5) {
                if (this->data.components.at(this->data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->data.components.at(this->data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.components.at(this->data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.components.push_back(actionRowData);
                }
            }
        }
        InteractionApplicationCommandCallbackData data;
        string requesterId;
    protected:
        CreateInteractionResponseData(InteractionData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->type = InteractionCallbackType::ChannelMessageWithSource;
        }
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionCallbackType type;
        InteractionPackageData interactionPackage;
    };

    struct CreateEphemeralInteractionResponseData {
    public:
        CreateEphemeralInteractionResponseData(ButtonInteractionData dataPackage) {
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->type = InteractionCallbackType::ChannelMessageWithSource;
        }
        CreateEphemeralInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }
        InteractionApplicationCommandCallbackData data;
        string requesterId;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionCallbackType type;
        InteractionPackageData interactionPackage;
    };

    struct EditInteractionResponseData {
        EditInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->type = InteractionCallbackType::UpdateMessage;
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        vector<AttachmentData> attachments;
        vector<ActionRowData>components;
        int flags;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionCallbackType type;
        InteractionPackageData interactionPackage;
        string requesterId;
    };

    struct DeleteInteractionResponseData {
        DeleteInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
        }
        unsigned int timeDelay = 0;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionPackageData interactionPackage;
    };

    struct CreateFollowUpMessageData {
        CreateFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->requesterId = dataPackage.getRequesterId();
        }
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        int flags;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionPackageData interactionPackage;
        string requesterId;
    };

    struct EditFollowUpMessageData {
        EditFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
            this->components = dataPackage.getComponents();
            this->content = dataPackage.getMessageContent();
            this->embeds = dataPackage.getEmbeds();
            this->requesterId = dataPackage.getRequesterId();
        }
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        int flags;
        string requesterId;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionPackageData interactionPackage;
        MessagePackageData messagePackage;
    };

    struct DeleteFollowUpMessageData {
        DeleteFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
        }
        unsigned int timeDelay = 0;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionPackageData interactionPackage;
        MessagePackageData messagePackage;
    };

    class InteractionManagerAgent : public agent {
    public:
        static unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>* requestPostDeferredInteractionResponseBuffer;
        static unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>* requestPostFollowUpMessageBuffer;
        static unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>* requestPatchFollowUpMessageBuffer;
        static unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData>* requestDeleteInteractionResponseBuffer;
        static unbounded_buffer<DiscordCoreInternal::DeleteFollowUpMessageData>* requestDeleteFollowUpMessageBuffer;
        static unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>* requestPostInteractionResponseBuffer;
        static unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData>* requestPatchInteractionResponseBuffer;
        static DiscordCoreInternal::ThreadManager* threadManager;
        static DiscordCoreInternal::ThreadContext threadContext;
        static unbounded_buffer<MessageData>* outInteractionResponseBuffer;
        static unbounded_buffer<exception>* errorBuffer;

        InteractionManagerAgent(Scheduler* pSchedulerNew)
            :agent(*pSchedulerNew) {}

        bool getError(exception& error) {
            if (try_receive(errorBuffer, error)) {
                return true;
            }
            return false;
        }

        static void initialize(DiscordCoreInternal::ThreadContext threadContextNew, DiscordCoreInternal::ThreadManager* threadManagerNew) {
            InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer = new unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>;
            InteractionManagerAgent::requestPostFollowUpMessageBuffer = new unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>;
            InteractionManagerAgent::requestPatchFollowUpMessageBuffer = new unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>;
            InteractionManagerAgent::requestDeleteInteractionResponseBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData>;
            InteractionManagerAgent::requestPostInteractionResponseBuffer = new unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>;
            InteractionManagerAgent::requestDeleteFollowUpMessageBuffer = new unbounded_buffer<DiscordCoreInternal::DeleteFollowUpMessageData>;
            InteractionManagerAgent::requestPatchInteractionResponseBuffer = new unbounded_buffer <DiscordCoreInternal::EditInteractionResponseData>;
            InteractionManagerAgent::outInteractionResponseBuffer = new unbounded_buffer<MessageData>;
            InteractionManagerAgent::errorBuffer = new unbounded_buffer<exception>;
            InteractionManagerAgent::threadManager = threadManagerNew;
            InteractionManagerAgent::threadContext = threadContextNew;
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
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::patchObjectData() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::patchObjectData() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::patchObjectData() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::patchObjectData() Error: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::patchObjectData() Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::patchObjectData() Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            MessageData messageData;
            DiscordCoreInternal::parseObject(returnData.data, &messageData);
            return messageData;
        }

        void postObjectData(DiscordCoreInternal::CreateInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_INTERACTION_RESPONSE;
            workload.content = dataPackage.content;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::postObjectData() Error 00: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData() Error 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData() Success 00: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return ;
        }

        void postObjectData(DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_DEFERRED_INTERACTION_RESPONSE;
            workload.content = dataPackage.content;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::postObjectData() Error 01: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData() Error 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData() Success 01: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        MessageData postObjectData(DiscordCoreInternal::CreateFollowUpMessageData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_FOLLOW_UP_MESSAGE;
            workload.content = DiscordCoreInternal::getPostFollowUpMessagePayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            send(requestAgent.workSubmissionBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::postObjectData() Error 02: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
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
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::deleteObjectData() Error 02: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::deleteObjectData() Error 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::deleteObjectData() Success 02: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        void deleteObjectDataTimer(DiscordCoreInternal::DeleteInteractionResponseData dataPackage) {
            if (dataPackage.timeDelayInMs> 0){
                DispatcherQueueTimer timer = InteractionManagerAgent::threadContext.dispatcherQueue.get()->CreateTimer();
                timer.Interval(chrono::milliseconds(dataPackage.timeDelayInMs));
                timer.Tick([this, dataPackage, timer](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
                    deleteObjectData(dataPackage);
                    timer.Stop();
                    });
                timer.Start();
            }
            else {
                deleteObjectData(dataPackage);
            }
            return;
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
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManagerAgent::deleteObjectData() Error 03: " << error.what() << endl << endl;
            }
            DiscordCoreInternal::HttpData returnData;
            try_receive(requestAgent.workReturnBuffer, returnData);
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::deleteObjectData() Error 03: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::deleteObjectData() Success 03: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        void deleteObjectDataTimer(DiscordCoreInternal::DeleteFollowUpMessageData dataPackage) {
            if (dataPackage.timeDelayInMs > 0) {
                DispatcherQueueTimer timer = InteractionManagerAgent::threadContext.dispatcherQueue.get()->CreateTimer();
                timer.Interval(chrono::milliseconds(dataPackage.timeDelayInMs));
                timer.Tick([this, dataPackage, timer](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args) {
                    deleteObjectData(dataPackage);
                    timer.Stop();
                    });
                timer.Start();
            }
            else {
                deleteObjectData(dataPackage);
            }
            return;
        }

    protected:

        void run() {
            try {
                DiscordCoreInternal::EditInteractionResponseData dataPackage01;
                if (try_receive(InteractionManagerAgent::requestPatchInteractionResponseBuffer, dataPackage01)) {
                    MessageData messageData = patchObjectData(dataPackage01);
                    asend(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::CreateInteractionResponseData dataPackage02;
                if (try_receive(InteractionManagerAgent::requestPostInteractionResponseBuffer, dataPackage02)) {
                    postObjectData(dataPackage02);
                }
                DiscordCoreInternal::CreateFollowUpMessageData dataPackage03;
                if (try_receive(InteractionManagerAgent::requestPostFollowUpMessageBuffer, dataPackage03)) {
                    MessageData messageData = postObjectData(dataPackage03);
                    asend(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::DeleteInteractionResponseData dataPackage04;
                if (try_receive(InteractionManagerAgent::requestDeleteInteractionResponseBuffer, dataPackage04)) {
                    deleteObjectDataTimer(dataPackage04);
                }
                DiscordCoreInternal::EditFollowUpMessageData dataPackage05;
                if (try_receive(InteractionManagerAgent::requestPatchFollowUpMessageBuffer, dataPackage05)) {
                    MessageData messageData = patchObjectData(dataPackage05);
                    asend(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackage06;
                if (try_receive(InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer, dataPackage06)) {
                    postObjectData(dataPackage06);
                }
                DiscordCoreInternal::DeleteFollowUpMessageData dataPackage07;
                if(try_receive(InteractionManagerAgent::requestDeleteFollowUpMessageBuffer, dataPackage07)){
                    deleteObjectDataTimer(dataPackage07);
                }
            }
            catch (const exception& e) {
                send(errorBuffer, e);
            }
            done();
        }

    };

	class InteractionManager {
    public:
        friend class InteractionManagerAgent;

		InteractionManager() {}

        static void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, DiscordCoreInternal::ThreadContext threadContextNew) {
            InteractionManager::agentResources = agentResourcesNew;
            InteractionManager::threadContext = threadContextNew;
        }

        static task<void> createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData dataPackage) {
            unsigned int groupId = InteractionManager::threadContext.createGroup();
            co_await resume_foreground(*InteractionManager::threadContext.dispatcherQueue.get());
            DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackageNew;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.content = DiscordCoreInternal::getCreateDeferredInteractionResponsePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(DiscordCoreInternal::ThreadManager::getThreadContext().get().scheduler);
            send(InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createDeferredInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            InteractionManager::threadContext.releaseGroup(groupId);
            co_return;
        }

        static task<void> createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
            unsigned int groupId = InteractionManager::threadContext.createGroup();
            co_await resume_foreground(*InteractionManager::threadContext.dispatcherQueue.get());
            DiscordCoreInternal::CreateInteractionResponseData dataPackageNew;
            dataPackageNew.data = dataPackage.data;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.content = DiscordCoreInternal::getCreateInteractionResponsePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(DiscordCoreInternal::ThreadManager::getThreadContext().get().scheduler);
            send(InteractionManagerAgent::requestPostInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            InteractionManager::threadContext.releaseGroup(groupId);
            co_return;
        }

        static task<MessageData> editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
            unsigned int groupId = InteractionManager::threadContext.createGroup();
            co_await resume_foreground(*InteractionManager::threadContext.dispatcherQueue.get());
            DiscordCoreInternal::EditInteractionResponseData dataPackageNew;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.agentResources = InteractionManager::agentResources;
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
            InteractionManagerAgent requestAgent(DiscordCoreInternal::ThreadManager::getThreadContext().get().scheduler);
            send(InteractionManagerAgent::requestPatchInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::editInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            InteractionManager::threadContext.releaseGroup(groupId);
            co_return messageData;
        }

        static task<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
            unsigned int groupId = InteractionManager::threadContext.createGroup();
            co_await resume_foreground(*InteractionManager::threadContext.dispatcherQueue.get());
            DiscordCoreInternal::DeleteInteractionResponseData dataPackageNew;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.timeDelayInMs = dataPackage.timeDelay;
            InteractionManagerAgent requestAgent(DiscordCoreInternal::ThreadManager::getThreadContext().get().scheduler);;
            send(InteractionManagerAgent::requestDeleteInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::deleteInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            InteractionManager::threadContext.releaseGroup(groupId);
            co_return;
        }

        static task<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
            unsigned int groupId = InteractionManager::threadContext.createGroup();
            co_await resume_foreground(*InteractionManager::threadContext.dispatcherQueue.get());
            DiscordCoreInternal::CreateFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.allowedMentions = dataPackage.allowedMentions;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.avatarURL = dataPackage.avatarUrl;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            for (auto value : dataPackage.components) {
                dataPackageNew.components.push_back(value);
            }
            dataPackageNew.content = dataPackage.content;
            for (auto value : dataPackage.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.flags = dataPackage.flags;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.tts = dataPackage.tts;
            dataPackageNew.username = dataPackage.username;
            InteractionManagerAgent requestAgent(DiscordCoreInternal::ThreadManager::getThreadContext().get().scheduler);
            send(InteractionManagerAgent::requestPostFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createFollowUpMessage() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            InteractionManager::threadContext.releaseGroup(groupId);
            co_return messageData;
        }

        static task<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
            unsigned int groupId = InteractionManager::threadContext.createGroup();
            co_await resume_foreground(*InteractionManager::threadContext.dispatcherQueue.get());
            DiscordCoreInternal::EditFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = InteractionManager::agentResources;
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
            InteractionManagerAgent requestAgent(DiscordCoreInternal::ThreadManager::getThreadContext().get().scheduler);
            send(InteractionManagerAgent::requestPatchFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::editFollowUpMessageAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(InteractionManagerAgent::outInteractionResponseBuffer, messageData);
            InteractionManager::threadContext.releaseGroup(groupId);
            co_return messageData;
        }

        static task<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
            unsigned int groupId = InteractionManager::threadContext.createGroup();
            co_await resume_foreground(*InteractionManager::threadContext.dispatcherQueue.get());
            DiscordCoreInternal::DeleteFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = InteractionManager::agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.timeDelayInMs = dataPackage.timeDelay;
            dataPackageNew.messageId = dataPackage.messagePackage.messageId;
            InteractionManagerAgent requestAgent(DiscordCoreInternal::ThreadManager::getThreadContext().get().scheduler);
            send(InteractionManagerAgent::requestDeleteFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::deleteFollowUpMessageData() Error: " << error.what() << endl << endl;
            }
            InteractionManager::threadContext.releaseGroup(groupId);
            co_return;
        }

        protected:
            friend class Button;
            static DiscordCoreInternal::HttpAgentResources agentResources;
            static DiscordCoreInternal::ThreadContext threadContext;
    };
    unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData>* InteractionManagerAgent::requestPostDeferredInteractionResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData>* InteractionManagerAgent::requestDeleteInteractionResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData>* InteractionManagerAgent::requestPostInteractionResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData>* InteractionManagerAgent::requestPatchInteractionResponseBuffer;
    unbounded_buffer<DiscordCoreInternal::DeleteFollowUpMessageData>* InteractionManagerAgent::requestDeleteFollowUpMessageBuffer;
    unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData>* InteractionManagerAgent::requestPostFollowUpMessageBuffer;
    unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData>* InteractionManagerAgent::requestPatchFollowUpMessageBuffer;
    DiscordCoreInternal::ThreadContext InteractionManagerAgent::threadContext;
    unbounded_buffer<MessageData>* InteractionManagerAgent::outInteractionResponseBuffer;
    unbounded_buffer<exception>* InteractionManagerAgent::errorBuffer;
    DiscordCoreInternal::ThreadContext InteractionManager::threadContext;
    DiscordCoreInternal::ThreadManager* InteractionManagerAgent::threadManager;
    DiscordCoreInternal::HttpAgentResources InteractionManager::agentResources;

    class Button : public agent {
    public:
        static map<string, unbounded_buffer<ButtonInteractionData>*> buttonInteractioinBufferMap;
        static DiscordCoreInternal::ThreadContext threadContext;

        Button() {};

        Button(const Button& B1) {
            this->buttonId = B1.buttonId;
            this->buttonIncomingInteractionBuffer = B1.buttonIncomingInteractionBuffer;
            this->channelId = B1.channelId;
            this->discordCoreClient = B1.discordCoreClient;
            this->doWeQuit = B1.doWeQuit;
            this->getButtonDataForAll = B1.getButtonDataForAll;
            this->interactionData = B1.interactionData;
            this->maxTimeInMs = B1.maxTimeInMs;
            this->messageId = B1.messageId;
            this->userId = B1.userId;
        };

        Button(InputEventData dataPackage) : agent(*Button::threadContext.schedulerGroups.at(0)) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->buttonIncomingInteractionBuffer = new unbounded_buffer<ButtonInteractionData>();
            Button::buttonInteractioinBufferMap.insert(make_pair(this->channelId + this->messageId, this->buttonIncomingInteractionBuffer));
        }

        static void initialize(DiscordCoreInternal::ThreadManager* threadManager, DiscordCoreClient* discordCoreClientNew) {
            Button::discordCoreClient = discordCoreClientNew;
            Button::threadContext = threadManager->getThreadContext().get();
            Button::threadContext.createGroup(Button::threadContext);
        }

        string getButtonId() {
            done();
            if (this->buttonId == "") {
                return "";
            }
            else {
                return this->buttonId;
            }
        }

        ButtonInteractionData getOurButtonData(bool getButtonDataForAllNew, unsigned int maxWaitTimeInMsNew, string targetUser = "") {
            if (targetUser != "") {
                this->userId = targetUser;
                cout << "USER ID: " << this->userId << endl;
            }
            this->maxTimeInMs = maxWaitTimeInMsNew;
            this->getButtonDataForAll = getButtonDataForAllNew;
            start();
            agent::wait(this);
            return this->interactionData;
        }

    protected:
        static DiscordCoreClient* discordCoreClient;
        unsigned int maxTimeInMs;
        bool getButtonDataForAll;
        ButtonInteractionData interactionData;
        unbounded_buffer<ButtonInteractionData>* buttonIncomingInteractionBuffer;
        string channelId;
        string messageId;
        string userId;
        string buttonId = "";
        bool doWeQuit = false;

        void run() {
            ButtonInteractionData buttonInteractionData;
            try {
                while (doWeQuit == false) {
                    if (this->getButtonDataForAll == false) {
                        buttonInteractionData = receive(Button::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        if (buttonInteractionData.user.id != this->userId) {
                            CreateInteractionResponseData createResponseData(buttonInteractionData);
                            createResponseData.data.flags = 64;
                            EmbedData embedData;
                            embedData.setColor("FEFEFE");
                            embedData.setTitle("__**Button Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that button can only be pressed by <@!" + this->userId + ">!");
                            createResponseData.data.embeds.push_back(embedData);
                            InteractionManager::createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = buttonInteractionData;
                            this->buttonId = buttonInteractionData.customId;
                            doWeQuit = true;
                        }

                    }
                    else {
                        buttonInteractionData = receive(Button::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = buttonInteractionData;
                        this->buttonId = buttonInteractionData.customId;
                        doWeQuit = true;
                    }
                }
                done();
                Button::buttonInteractioinBufferMap.erase(this->channelId + this->messageId);
            }
            catch (exception& e) {
                this->buttonId = "exit";
                done();
                Button::buttonInteractioinBufferMap.erase(this->channelId + this->messageId);
                return;
            }
        }
    };
    map<string, unbounded_buffer<ButtonInteractionData>*>Button::buttonInteractioinBufferMap;
    DiscordCoreClient* Button::discordCoreClient;
    DiscordCoreInternal::ThreadContext Button::threadContext;
};
#endif