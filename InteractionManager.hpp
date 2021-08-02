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
        ButtonInteractionData() {}
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

    struct SelectMenuInteractionData {
        SelectMenuInteractionData() {}
        SelectMenuInteractionData(const SelectMenuInteractionData& dataPackage) {
            this->applicationId = dataPackage.applicationId;
            this->channelId = dataPackage.channelId;
            this->customId = dataPackage.customId;
            this->guildId = dataPackage.guildId;
            this->id = dataPackage.id;
            this->isItForHere = dataPackage.isItForHere;
            this->values = dataPackage.values;
            this->member = dataPackage.member;
            this->message = dataPackage.message;
            this->token = dataPackage.token;
            this->type = dataPackage.type;
            this->user = dataPackage.user;
        }
        bool isItForHere;
        string token;
        string id;
        vector<string> values;
        string applicationId;
        InteractionType type;
        string customId;
        GuildMemberData member;
        UserData user;
        MessageData message;
        string channelId;
        string guildId;

    };

    struct DeferComponentResponseData {
        DeferComponentResponseData(InteractionData dataPackage) {
            this->type = InteractionCallbackType::DeferredUpdateMessage;
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->responseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
        }
        DeferComponentResponseData(SelectMenuInteractionData dataPackage) {
            this->type = InteractionCallbackType::DeferredUpdateMessage;
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->responseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
        }
        DeferComponentResponseData(ButtonInteractionData dataPackage) {
            this->type = InteractionCallbackType::DeferredUpdateMessage;
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->responseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
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
            this->channelId = dataPackage.getChannelId();
            this->guildId = dataPackage.getGuildId();
        }
        string requesterId;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventManager;
        InteractionPackageData interactionPackage;
        InteractionCallbackType type;
        string guildId;
        string channelId;
    };

    struct CreateInteractionResponseData {
    public:
        CreateInteractionResponseData(SelectMenuInteractionData dataPackage) {
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
        void addSelectMenu(bool disabled,  string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.components.push_back(actionRowData);
            }
            if (this->data.components.size() < 5) {
                if (this->data.components.at(this->data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->data.components.at(this->data.components.size() - 1).components.push_back(componentData);
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
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
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
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        int flags = 0;
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
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
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
        unbounded_buffer<DiscordCoreInternal::CreateDeferredInteractionResponseData> requestPostDeferredInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData> requestDeleteInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::CreateInteractionResponseData> requestPostInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::EditInteractionResponseData> requestPatchInteractionResponseBuffer;
        unbounded_buffer<DiscordCoreInternal::DeleteFollowUpMessageData> requestDeleteFollowUpMessageBuffer;
        unbounded_buffer<DiscordCoreInternal::CreateFollowUpMessageData> requestPostFollowUpMessageBuffer;
        unbounded_buffer<DiscordCoreInternal::EditFollowUpMessageData> requestPatchFollowUpMessageBuffer;
        unbounded_buffer<MessageData> outInteractionResponseBuffer;
        unbounded_buffer<exception> errorBuffer;

        DiscordCoreInternal::HttpAgentResources agentResources;
        shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
        ScheduleGroup* pScheduleGroup{ nullptr };

        InteractionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, ScheduleGroup* scheduleGroup)
            :agent(*scheduleGroup) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
            this->pScheduleGroup = scheduleGroup;
        }

        bool getError(exception& error) {
            return try_receive(this->errorBuffer, error);
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
            if (dataPackage.timeDelayInMs > 0) {
                ThreadPoolTimer threadPoolTimer = ThreadPoolTimer(nullptr);
                TimerElapsedHandler onSend = [=](ThreadPoolTimer threadPoolTimerNew) {
                    deleteObjectData(dataPackage);
                };
                threadPoolTimer = threadPoolTimer.CreateTimer(onSend, TimeSpan(dataPackage.timeDelayInMs * 10000));
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
                ThreadPoolTimer threadPoolTimer = ThreadPoolTimer(nullptr);
                TimerElapsedHandler onSend = [=](ThreadPoolTimer threadPoolTimerNew) {
                    deleteObjectData(dataPackage);
                };
                threadPoolTimer = threadPoolTimer.CreateTimer(onSend, TimeSpan(dataPackage.timeDelayInMs * 10000));
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
                    asend(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::CreateInteractionResponseData dataPackage02;
                if (try_receive(this->requestPostInteractionResponseBuffer, dataPackage02)) {
                    postObjectData(dataPackage02);
                }
                DiscordCoreInternal::CreateFollowUpMessageData dataPackage03;
                if (try_receive(this->requestPostFollowUpMessageBuffer, dataPackage03)) {
                    MessageData messageData = postObjectData(dataPackage03);
                    asend(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::DeleteInteractionResponseData dataPackage04;
                if (try_receive(this->requestDeleteInteractionResponseBuffer, dataPackage04)) {
                    deleteObjectDataTimer(dataPackage04);
                }
                DiscordCoreInternal::EditFollowUpMessageData dataPackage05;
                if (try_receive(this->requestPatchFollowUpMessageBuffer, dataPackage05)) {
                    MessageData messageData = patchObjectData(dataPackage05);
                    asend(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackage06;
                if (try_receive(this->requestPostDeferredInteractionResponseBuffer, dataPackage06)) {
                    postObjectData(dataPackage06);
                }
                DiscordCoreInternal::DeleteFollowUpMessageData dataPackage07;
                if(try_receive(this->requestDeleteFollowUpMessageBuffer, dataPackage07)){
                    deleteObjectDataTimer(dataPackage07);
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
            unsigned int groupIdNew;
            groupIdNew = this->threadContext->createGroup();
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::CreateDeferredInteractionResponseData dataPackageNew;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.content = DiscordCoreInternal::getCreateDeferredInteractionResponsePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(this->agentResources, this->threadContext, this->threadContext->schedulerGroups.at(this->threadContext->schedulerGroups.size() - 1));
            send(requestAgent.requestPostDeferredInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createDeferredInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            this->threadContext->releaseGroup(groupIdNew);
            co_return;
        }

        task<void> createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
            unsigned int groupIdNew;
            groupIdNew = this->threadContext->createGroup();
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::CreateInteractionResponseData dataPackageNew;
            dataPackageNew.data = dataPackage.data;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.type;
            dataPackageNew.content = DiscordCoreInternal::getCreateInteractionResponsePayload(dataPackageNew);
            InteractionManagerAgent requestAgent(this->agentResources, this->threadContext, this->threadContext->schedulerGroups.at(this->threadContext->schedulerGroups.size() - 1));
            send(requestAgent.requestPostInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            this->threadContext->releaseGroup(groupIdNew);
            co_return;
        }

        task<MessageData> editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
            unsigned int groupIdNew;
            groupIdNew = this->threadContext->createGroup();
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
            InteractionManagerAgent requestAgent(this->agentResources, this->threadContext, this->threadContext->schedulerGroups.at(this->threadContext->schedulerGroups.size() - 1));
            send(requestAgent.requestPatchInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::editInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            this->threadContext->releaseGroup(groupIdNew);
            co_return messageData;
        }

        task<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
            unsigned int groupIdNew;
            groupIdNew = this->threadContext->createGroup();
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::DeleteInteractionResponseData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.timeDelayInMs = dataPackage.timeDelay;
            InteractionManagerAgent requestAgent(this->agentResources, this->threadContext, this->threadContext->schedulerGroups.at(this->threadContext->schedulerGroups.size() - 1));
            send(requestAgent.requestDeleteInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::deleteInteractionResponseAsync() Error: " << error.what() << endl << endl;
            }
            this->threadContext->releaseGroup(groupIdNew);
            co_return;
        }

        task<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
            unsigned int groupIdNew;
            groupIdNew = this->threadContext->createGroup();
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::CreateFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
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
            InteractionManagerAgent requestAgent(this->agentResources, this->threadContext, this->threadContext->schedulerGroups.at(this->threadContext->schedulerGroups.size() - 1));
            send(requestAgent.requestPostFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::createFollowUpMessage() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            this->threadContext->releaseGroup(groupIdNew);
            co_return messageData;
        }

        task<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
            unsigned int groupIdNew;
            groupIdNew = this->threadContext->createGroup();
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
            InteractionManagerAgent requestAgent(this->agentResources, this->threadContext, this->threadContext->schedulerGroups.at(this->threadContext->schedulerGroups.size() - 1));
            send(requestAgent.requestPatchFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::editFollowUpMessageAsync() Error: " << error.what() << endl << endl;
            }
            MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            this->threadContext->releaseGroup(groupIdNew);
            co_return messageData;
        }

        task<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
            unsigned int groupIdNew;
            groupIdNew = this->threadContext->createGroup();
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::DeleteFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.timeDelayInMs = dataPackage.timeDelay;
            dataPackageNew.messageId = dataPackage.messagePackage.messageId;
            InteractionManagerAgent requestAgent(this->agentResources, this->threadContext, this->threadContext->schedulerGroups.at(this->threadContext->schedulerGroups.size() - 1));
            send(requestAgent.requestDeleteFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            exception error;
            while (requestAgent.getError(error)) {
                cout << "InteractionManager::deleteFollowUpMessageData() Error: " << error.what() << endl << endl;
            }
            this->threadContext->releaseGroup(groupIdNew);
            co_return;
        }

        InteractionManager(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
            this->groupId = this->threadContext->createGroup();
        }

        ~InteractionManager() {
            this->threadContext->releaseGroup(this->groupId);
        }

        protected:
            friend class InteractionManagerAgent;
            friend class DiscordCoreClient;

            DiscordCoreInternal::HttpAgentResources agentResources;
            shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
            unsigned int groupId;
    };

    class SelectMenu : public agent {
    public:
        static map<string, unbounded_buffer<SelectMenuInteractionData>*>selectMenuInteractionMap;

        SelectMenu(InputEventData dataPackage) : agent(*SelectMenu::threadContext->schedulerGroups.at(0)) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->selectMenuIncomingInteractionBuffer = new unbounded_buffer<SelectMenuInteractionData>;
            SelectMenu::selectMenuInteractionMap.insert(make_pair(this->channelId + this->messageId, this->selectMenuIncomingInteractionBuffer));
        }

        static void initialize(shared_ptr<InteractionManager> interactionsNew) {
            SelectMenu::interactions = interactionsNew;
            SelectMenu::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
            SelectMenu::threadContext->createGroup(*SelectMenu::threadContext);
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
            return this->interactionData;
        }

    protected:
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static shared_ptr<InteractionManager> interactions;
        unsigned int maxTimeInMs;
        bool getButtonDataForAll;
        SelectMenuInteractionData interactionData;
        unbounded_buffer<SelectMenuInteractionData>* selectMenuIncomingInteractionBuffer{ nullptr };
        string channelId;
        string messageId;
        string userId;
        string selectMenuId = "";
        bool doWeQuit = false;

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
                            embedData.setTitle("__**Button Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that menu can only be selected by <@!" + this->userId + ">!");
                            createResponseData.data.embeds.push_back(embedData);
                            SelectMenu::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = selectMenuInteractionData;
                            this->selectMenuId = selectMenuInteractionData.customId;
                            doWeQuit = true;
                        }
                    }
                    else {
                        SelectMenuInteractionData selectMenuInteractionData = receive(SelectMenu::selectMenuIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = selectMenuInteractionData;
                        this->selectMenuId = selectMenuInteractionData.customId;
                        doWeQuit = true;
                    }
                }
                done();
                SelectMenu::selectMenuInteractionMap.erase(this->channelId + this->messageId);
            }
            catch (exception&) {
                this->selectMenuId = "exit";
                done();
                SelectMenu::selectMenuInteractionMap.erase(this->channelId + this->messageId);
                return;
            }
        }

    };

    class Button : public agent {
    public:
        static map<string, unbounded_buffer<ButtonInteractionData>*> buttonInteractionMap;

        Button(InputEventData dataPackage) : agent(*Button::threadContext->schedulerGroups.at(0)) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->buttonIncomingInteractionBuffer = new unbounded_buffer<ButtonInteractionData>;
            Button::buttonInteractionMap.insert(make_pair(this->channelId + this->messageId, this->buttonIncomingInteractionBuffer));
        }

        static void initialize(shared_ptr<InteractionManager> interactionsNew) {
            Button::interactions = interactionsNew;
            Button::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
            Button::threadContext->createGroup(*Button::threadContext);
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
            return this->interactionData;
        }

    protected:
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static shared_ptr<InteractionManager> interactions;
        unsigned int maxTimeInMs;
        bool getButtonDataForAll;
        ButtonInteractionData interactionData;
        unbounded_buffer<ButtonInteractionData>* buttonIncomingInteractionBuffer{ nullptr };
        string channelId;
        string messageId;
        string userId;
        string buttonId = "";
        bool doWeQuit = false;

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
                            embedData.setTitle("__**Button Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that button can only be pressed by <@!" + this->userId + ">!");
                            createResponseData.data.embeds.push_back(embedData);
                            Button::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = buttonInteractionData;
                            this->buttonId = buttonInteractionData.customId;
                            doWeQuit = true;
                        }
                    }
                    else {
                        ButtonInteractionData buttonInteractionData = receive(Button::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = buttonInteractionData;
                        this->buttonId = buttonInteractionData.customId;
                        doWeQuit = true;
                    }
                }
                done();
                Button::buttonInteractionMap.erase(this->channelId + this->messageId);
            }
            catch (exception&) {
                this->buttonId = "exit";
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
};
#endif