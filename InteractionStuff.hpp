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

    struct ButtonInteractionData {

        friend class EventHandler;
        friend class Button;        

        string applicationId{ "" };
        GuildMemberData member{};
        string channelId{ "" };
        InteractionType type{};
        string customId{ "" };
        MessageData message{};
        string guildId{ "" };
        string token{ "" };
        string id{ "" };
        UserData user{};

        ButtonInteractionData(const ButtonInteractionData& dataPackage) {
            this->applicationId = dataPackage.applicationId;
            this->channelId = dataPackage.channelId;
            this->customId = dataPackage.customId;
            this->guildId = dataPackage.guildId;
            this->message = dataPackage.message;
            this->member = dataPackage.member;
            this->token = dataPackage.token;
            this->type = dataPackage.type;
            this->user = dataPackage.user;
            this->id = dataPackage.id;
        }
        
    protected:
        ButtonInteractionData() {}
    };

    struct SelectMenuInteractionData {

        friend class EventHandler;
        friend class SelectMenu;

        string applicationId{ "" };
        GuildMemberData member{};
        vector<string> values{};
        string channelId{ "" };
        InteractionType type{};
        MessageData message{};
        string customId{ "" };
        string guildId{ "" };
        string token{ "" };
        string id{ "" };
        UserData user{};

        SelectMenuInteractionData(const SelectMenuInteractionData& dataPackage) {
            this->applicationId = dataPackage.applicationId;
            this->channelId = dataPackage.channelId;
            this->customId = dataPackage.customId;
            this->guildId = dataPackage.guildId;
            this->message = dataPackage.message;
            this->member = dataPackage.member;
            this->values = dataPackage.values;
            this->token = dataPackage.token;
            this->type = dataPackage.type;
            this->user = dataPackage.user;
            this->id = dataPackage.id;
        }

    protected:
        SelectMenuInteractionData() {};
    };

    struct DeferComponentResponseData {

        friend class InputEvents;
        
        DeferComponentResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->responseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
            this->type = InteractionCallbackType::DeferredUpdateMessage;
        }
    protected:
        InteractionPackageData interactionPackage{};
        InputEventResponseType responseType{};
        InteractionCallbackType type{};
    };

    struct CreateDeferredInteractionResponseData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        CreateDeferredInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->data.type = InteractionCallbackType::DeferredChannelMessageWithSource;
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->requesterId = dataPackage.getAuthorId();
            this->data.data.flags = 64;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
    };

    struct CreateInteractionResponseData {
    public:

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;
        friend class SelectMenu;
        friend class Button;

        CreateInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }

        CreateInteractionResponseData(SelectMenuInteractionData dataPackage) {
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->interactionPackage.interactionId = dataPackage.id;            
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
        }

        CreateInteractionResponseData(ButtonInteractionData dataPackage) {
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->interactionPackage.interactionId = dataPackage.id;            
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
        }

        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.type = ComponentType::Button;
                    component.emoji.name = emojiName;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.id = emojiId;
                    component.url = url;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }
            }
        }

        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(componentData);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }

            }
        }

        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }
        
    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };

        CreateInteractionResponseData() {};
    };

    struct CreateEphemeralInteractionResponseData {
    public:

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        CreateEphemeralInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
            this->data.data.flags = 64;
        }

        CreateEphemeralInteractionResponseData(SelectMenuInteractionData dataPackage) {
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->interactionPackage.interactionId = dataPackage.id;
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
            this->data.data.flags = 64;
        }

        CreateEphemeralInteractionResponseData(ButtonInteractionData dataPackage) {
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->interactionPackage.interactionId = dataPackage.id;
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
            this->data.data.flags = 64;
        }

        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.type = ComponentType::Button;
                    component.emoji.name = emojiName;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.id = emojiId;
                    component.url = url;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }
            }
        }

        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(componentData);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }

            }
        }

        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
        
    };

    struct EditInteractionResponseData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        EditInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->data.type = InteractionCallbackType::UpdateMessage;
            this->requesterId = dataPackage.getRequesterId();
        }

        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.type = ComponentType::Button;
                    component.emoji.name = emojiName;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.id = emojiId;
                    component.url = url;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }
            }
        }

        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(componentData);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }

            }
        }

        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
    };

    struct DeleteInteractionResponseData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        DeleteInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
        }
        
    protected:
        InteractionPackageData interactionPackage{};
        unsigned int timeDelay{ 0 };
    };

    struct CreateFollowUpMessageData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        CreateFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->requesterId = dataPackage.getRequesterId();
        }

        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.type = ComponentType::Button;
                    component.emoji.name = emojiName;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.id = emojiId;
                    component.url = url;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }
            }
        }

        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(componentData);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }

            }
        }

        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };

        CreateFollowUpMessageData() {};
    };

    struct CreateEphemeralFollowUpMessageData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        CreateEphemeralFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->requesterId = dataPackage.getRequesterId();
            this->data.data.flags = 64;
        }

        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.type = ComponentType::Button;
                    component.emoji.name = emojiName;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.id = emojiId;
                    component.url = url;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }
            }
        }

        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(componentData);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }

            }
        }

        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
    };

    struct EditFollowUpMessageData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        EditFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
            this->requesterId = dataPackage.getRequesterId();
        }

        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.type = ComponentType::Button;
                    component.emoji.name = emojiName;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.id = emojiId;
                    component.url = url;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }
            }
        }

        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.data.components.push_back(actionRowData);
            }
            if (this->data.data.components.size() < 5) {
                if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    this->data.data.components.at(this->data.data.components.size() - 1).components.push_back(componentData);
                }
                else if (this->data.data.components.at(this->data.data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.data.components.push_back(actionRowData);
                }

            }
        }

        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        MessagePackageData messagePackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
    };

    struct DeleteFollowUpMessageData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEvents;

        DeleteFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
        }
        
    protected:
        InteractionPackageData interactionPackage{};
        MessagePackageData messagePackage{};
        unsigned int timeDelay{ 0 };        
    };

    struct GetInteractionResponseData {
        string interactionToken{ "" };
        string applicationId{ "" };
    };

};

namespace DiscordCoreInternal {

    class InteractionManagerAgent : public agent {
    protected:

        friend class DiscordCoreAPI::DiscordCoreClient;
        friend class DiscordCoreAPI::EventHandler;
        friend class InteractionManager;

        static map<string, shared_ptr<unbounded_buffer<DiscordCoreAPI::MessageData>>> collectMessageDataBuffers;
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;

        unbounded_buffer<DiscordCoreInternal::PostDeferredInteractionResponseData> requestPostDeferredInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::DeleteInteractionResponseData> requestDeleteInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PatchInteractionResponseData> requestPatchInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PostInteractionResponseData> requestPostInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::GetInteractionResponseData> requestGetInteractionResponseBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::DeleteFollowUpMessageData> requestDeleteFollowUpMessageBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PatchFollowUpMessageData> requestPatchFollowUpMessageBuffer{ nullptr };
        unbounded_buffer<DiscordCoreInternal::PostFollowUpMessageData> requestPostFollowUpMessageBuffer{ nullptr };
        unbounded_buffer<DiscordCoreAPI::InteractionResponseData> outInteractionresponseDataBuffer{ nullptr };
        unbounded_buffer<DiscordCoreAPI::MessageData> outInteractionResponseBuffer{ nullptr };
        DiscordCoreInternal::HttpAgentResources agentResources{};
        unbounded_buffer<exception> errorBuffer{ nullptr };

        InteractionManagerAgent(DiscordCoreInternal::HttpAgentResources agentResourcesNew)
            :agent(*InteractionManagerAgent::threadContext->scheduler->scheduler) {
            this->agentResources = agentResourcesNew;
        }

        static void initialize(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            InteractionManagerAgent::threadContext = threadContextNew;
        }

        void getError(string stackTrace) {
            exception error;
            while (try_receive(errorBuffer, error)) {
                cout << stackTrace + "::InteractionManagerAgent Error: " << error.what() << endl << endl;
            }
            return;
        }

        DiscordCoreAPI::InteractionResponseData getObjectData(DiscordCoreInternal::GetInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_INTERACTION_RESPONSE;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::getObjectData_00");
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

        DiscordCoreAPI::MessageData patchObjectData(DiscordCoreInternal::PatchFollowUpMessageData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/" + dataPackage.messageId;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_FOLLOW_UP_MESSAGE;
            workload.content = DiscordCoreInternal::getEditFollowUpMessagePayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::patchObjectData_00");
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

        DiscordCoreAPI::MessageData patchObjectData(DiscordCoreInternal::PatchInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::PATCH;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::PATCH_INTERACTION_RESPONSE;
            workload.content = DiscordCoreInternal::getEditInteractionResponsePayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::patchObjectData_01");
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::patchObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::patchObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreAPI::MessageData messageData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
            return messageData;
        }

        void postObjectData(DiscordCoreInternal::PostInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/interactions/" + dataPackage.interactionId + "/" + dataPackage.interactionToken + "/callback";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_INTERACTION_RESPONSE;
            workload.content = DiscordCoreInternal::getCreateInteractionResponsePayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::postObjectData_00");
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
            workload.content = DiscordCoreInternal::getCreateDeferredInteractionResponsePayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::postObjectData_01");
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData_01 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData_01 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            return;
        }

        DiscordCoreAPI::MessageData postObjectData(DiscordCoreInternal::PostFollowUpMessageData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken;
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::POST;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::POST_FOLLOW_UP_MESSAGE;
            workload.content = DiscordCoreInternal::getPostFollowUpMessagePayload(dataPackage);
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::postObjectData_02");
            if (returnData.returnCode != 204 && returnData.returnCode != 201 && returnData.returnCode != 200) {
                cout << "InteractionManagerAgent::postObjectData_02 Error: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            else {
                cout << "InteractionManagerAgent::postObjectData_02 Success: " << returnData.returnCode << ", " << returnData.returnMessage << endl << endl;
            }
            DiscordCoreAPI::MessageData messageData;
            DiscordCoreInternal::DataParser::parseObject(returnData.data, &messageData);
            return messageData;
        }

        void deleteObjectData(DiscordCoreInternal::DeleteInteractionResponseData dataPackage) {
            DiscordCoreInternal::HttpWorkload workload;
            workload.relativePath = "/webhooks/" + dataPackage.applicationId + "/" + dataPackage.interactionToken + "/messages/@original";
            workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::DELETED;
            workload.workloadType = DiscordCoreInternal::HttpWorkloadType::DELETE_INTERACTION;
            DiscordCoreInternal::HttpRequestAgent requestAgent(dataPackage.agentResources);
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::deleteObjectData_00");
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
                auto onSend = [=]() {
                    deleteObjectData(dataPackage);
                };
                DiscordCoreAPI::executeFunctionAfterTimePeriod(onSend, dataPackage.timeDelayInMs, false);
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
            HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "InteractionManagerAgent::deleteObjectData_01");
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
                auto onSend = [=]() {
                    deleteObjectData(dataPackage);
                };
                DiscordCoreAPI::executeFunctionAfterTimePeriod(onSend, dataPackage.timeDelayInMs, false);
            }
            else {
                deleteObjectData(dataPackage);
            }
        }

        static void cleanup() {
            InteractionManagerAgent::threadContext->releaseGroup();
        }

    protected:

        void run() {
            try {
                DiscordCoreInternal::PatchInteractionResponseData dataPackage01;
                if (try_receive(this->requestPatchInteractionResponseBuffer, dataPackage01)) {
                    DiscordCoreAPI::MessageData messageData = patchObjectData(dataPackage01);
                    send(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::PostInteractionResponseData dataPackage02;
                if (try_receive(this->requestPostInteractionResponseBuffer, dataPackage02)) {
                    postObjectData(dataPackage02);
                }
                DiscordCoreInternal::PostFollowUpMessageData dataPackage03;
                if (try_receive(this->requestPostFollowUpMessageBuffer, dataPackage03)) {
                    DiscordCoreAPI::MessageData messageData = postObjectData(dataPackage03);
                    send(this->outInteractionResponseBuffer, messageData);
                }
                DiscordCoreInternal::DeleteInteractionResponseData dataPackage04;
                if (try_receive(this->requestDeleteInteractionResponseBuffer, dataPackage04)) {
                    deleteObjectDataTimer(dataPackage04);
                }
                DiscordCoreInternal::PatchFollowUpMessageData dataPackage05;
                if (try_receive(this->requestPatchFollowUpMessageBuffer, dataPackage05)) {
                    DiscordCoreAPI::MessageData messageData = patchObjectData(dataPackage05);
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

        template <class _Ty>
        friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
        friend class DiscordCoreAPI::DiscordCoreClient;
        friend class DiscordCoreAPI::Interactions;
        friend class DiscordCoreAPI::InputEvents;
        friend class DiscordCoreAPI::SelectMenu;
        friend class DiscordCoreAPI::Button;

        InteractionManager(InteractionManager* pointer) {
            if (pointer != nullptr) {
                *this = *pointer;
            }
        }

    protected:
        
        shared_ptr<ThreadContext> threadContext{ nullptr };
        HttpAgentResources agentResources{};

        InteractionManager initialize(HttpAgentResources agentResourcesNew, shared_ptr<ThreadContext> threadContextNew) {
            this->agentResources = agentResourcesNew;
            this->threadContext = threadContextNew;
            return *this;
        }

        task<void> createDeferredInteractionResponseAsync(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PostDeferredInteractionResponseData dataPackageNew;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.data.type;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.flags = dataPackage.data.data.flags;
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPostDeferredInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::createDeferredInteractionResponseAsync");
            co_await mainThread;
            co_return;
        }

        task<DiscordCoreAPI::MessageData> createInteractionResponseAsync(DiscordCoreAPI::CreateInteractionResponseData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PostInteractionResponseData dataPackageNew;
            dataPackageNew.interactionId = dataPackage.interactionPackage.interactionId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            InteractionManagerAgent::collectMessageDataBuffers.insert(make_pair(dataPackage.interactionPackage.interactionId, make_shared<unbounded_buffer<DiscordCoreAPI::MessageData>>()));
            dataPackageNew.data = dataPackage.data.data;
            dataPackageNew.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.data.type;
            DiscordCoreInternal::HttpAgentResources httpAgentResources;
            dataPackageNew.agentResources = httpAgentResources;
            InteractionManagerAgent requestAgent(httpAgentResources);
            send(requestAgent.requestPostInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::createInteractionResponseAsync");
            if (dataPackage.data.type == DiscordCoreAPI::InteractionCallbackType::ChannelMessage || dataPackage.data.type == DiscordCoreAPI::InteractionCallbackType::ChannelMessageWithSource) {
                if (InteractionManagerAgent::collectMessageDataBuffers.contains(dataPackage.interactionPackage.interactionId)) {
                    shared_ptr<unbounded_buffer<DiscordCoreAPI::MessageData>> messageBlock = InteractionManagerAgent::collectMessageDataBuffers.at(dataPackage.interactionPackage.interactionId);
                    try {
                        DiscordCoreAPI::MessageData messageData = receive(*messageBlock, 1000);
                        co_await mainThread;
                        InteractionManagerAgent::collectMessageDataBuffers.erase(dataPackage.interactionPackage.interactionId);
                        co_return messageData;
                    }
                    catch (exception&) {
                        co_return DiscordCoreAPI::MessageData();
                    };
                }
            }
        }

        task<DiscordCoreAPI::InteractionResponseData> getInteractionResponseAsync(DiscordCoreAPI::GetInteractionResponseData dataPackage) {
            apartment_context mainThread;
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
            DiscordCoreAPI::InteractionResponseData outData;
            try_receive(requestAgent.outInteractionresponseDataBuffer, outData);
            co_await mainThread;
            co_return outData;
        }

        task<DiscordCoreAPI::MessageData> editInteractionResponseAsync(DiscordCoreAPI::EditInteractionResponseData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PatchInteractionResponseData dataPackageNew;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.data.data.allowedMentions = dataPackage.data.data.allowedMentions;
            for (auto value : dataPackage.data.data.components) {
                dataPackageNew.data.data.components.push_back(value);
            }
            dataPackageNew.data.data.content = dataPackage.data.data.content;
            for (auto value : dataPackage.data.data.embeds) {
                dataPackageNew.data.data.embeds.push_back(value);
            }
            dataPackageNew.data.data.flags = dataPackage.data.data.flags;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.data.type = (DiscordCoreInternal::InteractionCallbackType)dataPackage.data.type;
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPatchInteractionResponseBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::editInteractionResponseAsync");
            DiscordCoreAPI::MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            co_await mainThread;
            co_return messageData;
        }

        task<void> deleteInteractionResponseAsync(DiscordCoreAPI::DeleteInteractionResponseData dataPackage) {
            apartment_context mainThread;
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
            co_await mainThread;
            co_return;
        }

        task<DiscordCoreAPI::MessageData> createFollowUpMessageAsync(DiscordCoreAPI::CreateFollowUpMessageData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PostFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.content = dataPackage.data.data.content;
            dataPackageNew.allowedMentions = dataPackage.data.data.allowedMentions;
            for (auto value : dataPackage.data.data.components) {
                dataPackageNew.components.push_back(value);
            }
            dataPackageNew.content = dataPackage.data.data.content;
            for (auto value : dataPackage.data.data.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.flags = dataPackage.data.data.flags;
            dataPackageNew.tts = dataPackage.data.data.tts;
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPostFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::createFollowUpMessageAsync");
            DiscordCoreAPI::MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            co_await mainThread;
            co_return messageData;
        }

        task<DiscordCoreAPI::MessageData> editFollowUpMessageAsync(DiscordCoreAPI::EditFollowUpMessageData dataPackage) {
            apartment_context mainThread;
            co_await resume_foreground(*this->threadContext->dispatcherQueue.get());
            DiscordCoreInternal::PatchFollowUpMessageData dataPackageNew;
            dataPackageNew.agentResources = this->agentResources;
            dataPackageNew.applicationId = dataPackage.interactionPackage.applicationId;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.content = dataPackage.data.data.content;
            dataPackageNew.interactionToken = dataPackage.interactionPackage.interactionToken;
            dataPackageNew.messageId = dataPackage.messagePackage.messageId;
            dataPackageNew.allowedMentions = dataPackage.data.data.allowedMentions;
            for (auto value : dataPackage.data.data.components) {
                dataPackageNew.components.push_back(value);
            }
            dataPackageNew.content = dataPackage.data.data.content;
            for (auto value : dataPackage.data.data.embeds) {
                dataPackageNew.embeds.push_back(value);
            }
            dataPackageNew.flags = dataPackage.data.data.flags;
            dataPackageNew.tts = dataPackage.data.data.tts;
            InteractionManagerAgent requestAgent(this->agentResources);
            send(requestAgent.requestPatchFollowUpMessageBuffer, dataPackageNew);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("InteractionManager::editFollowUpMessageAsync");
            DiscordCoreAPI::MessageData messageData;
            try_receive(requestAgent.outInteractionResponseBuffer, messageData);
            co_await mainThread;
            co_return messageData;
        }

        task<void> deleteFollowUpMessageAsync(DiscordCoreAPI::DeleteFollowUpMessageData dataPackage) {
            apartment_context mainThread;
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
            co_await mainThread;
            co_return;
        }

        ~InteractionManager() {}
    };
    map<string, shared_ptr<unbounded_buffer<DiscordCoreAPI::MessageData>>> InteractionManagerAgent::collectMessageDataBuffers{};
    shared_ptr<DiscordCoreInternal::ThreadContext> InteractionManagerAgent::threadContext{ nullptr };
};

namespace DiscordCoreAPI {

    struct SelectMenuResponseData {
        string selectionId{ "" };
        vector<string> values{};
        string channelId{ "" };
        string messageId{ "" };
        string userId{ "" };
    };

    class SelectMenu : public agent {
    public:
        static map<string, unbounded_buffer<DiscordCoreAPI::SelectMenuInteractionData>*>selectMenuInteractionBufferMap;

        SelectMenu(InputEventData dataPackage) : agent(*SelectMenu::threadContext->scheduler->scheduler) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->selectMenuIncomingInteractionBuffer = new unbounded_buffer<DiscordCoreAPI::SelectMenuInteractionData>;
            SelectMenu::selectMenuInteractionBufferMap.insert(make_pair(this->channelId + this->messageId, this->selectMenuIncomingInteractionBuffer));
        }

        static void initialize(shared_ptr<DiscordCoreInternal::InteractionManager> interactionsNew) {
            SelectMenu::interactions = interactionsNew;
            SelectMenu::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
        }

        static void cleanup() {
            SelectMenu::threadContext->releaseGroup();
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
                cout << "SelectMenu::run() Error: " << error.what() << endl;
            }
            return this->responseVector;
        }

        ~SelectMenu() {
            done();
        }

    protected:
        static shared_ptr<DiscordCoreInternal::InteractionManager> interactions;
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        unbounded_buffer<DiscordCoreAPI::SelectMenuInteractionData>* selectMenuIncomingInteractionBuffer{ nullptr };
        DiscordCoreAPI::SelectMenuInteractionData interactionData{};
        unbounded_buffer<exception> errorBuffer{ nullptr };
        vector<SelectMenuResponseData> responseVector{};
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
                        DiscordCoreAPI::SelectMenuInteractionData selectMenuInteractionData = receive(SelectMenu::selectMenuIncomingInteractionBuffer, this->maxTimeInMs);
                        if (selectMenuInteractionData.user.id != this->userId) {
                            DiscordCoreAPI::CreateInteractionResponseData createResponseData(selectMenuInteractionData);
                            DiscordCoreAPI::EmbedData embedData;
                            embedData.setColor("FEFEFE");
                            embedData.setTitle("__**Permission Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that menu can only be selected by <@!" + this->userId + ">!");
                            createResponseData.addMessageEmbed(embedData);
                            createResponseData.data.data.flags = 64;
                            SelectMenu::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = selectMenuInteractionData;
                            this->values = selectMenuInteractionData.values;
                            this->selectMenuId = selectMenuInteractionData.customId;
                            DiscordCoreAPI::CreateInteractionResponseData dataPackage(selectMenuInteractionData);
                            dataPackage.setResponseType(DiscordCoreAPI::InteractionCallbackType::DeferredUpdateMessage);
                            SelectMenu::interactions->createInteractionResponseAsync(dataPackage);
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
                        DiscordCoreAPI::SelectMenuInteractionData selectMenuInteractionData = receive(SelectMenu::selectMenuIncomingInteractionBuffer, this->maxTimeInMs);
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
                        DiscordCoreAPI::CreateInteractionResponseData dataPackage(selectMenuInteractionData);
                        dataPackage.setResponseType(DiscordCoreAPI::InteractionCallbackType::DeferredUpdateMessage);
                        SelectMenu::interactions->createInteractionResponseAsync(dataPackage);
                    }
                }
                SelectMenu::selectMenuInteractionBufferMap.erase(this->channelId + this->messageId);
                done();
            }
            catch (exception& e) {
                this->selectMenuId = "exit";
                send(this->errorBuffer, e);
                done();
                SelectMenu::selectMenuInteractionBufferMap.erase(this->channelId + this->messageId);
                return;
            }
        }
    };

    struct ButtonResponseData {
        string emojiName{ "" };
        string channelId{ "" };
        string messageId{ "" };
        string buttonId{ "" };
        string userId{ "" };
    };

    class Button : public agent {
    public:
        static map<string, unbounded_buffer<DiscordCoreAPI::ButtonInteractionData>*> buttonInteractionBufferMap;

        Button(DiscordCoreAPI::InputEventData dataPackage) : agent(*Button::threadContext->scheduler->scheduler) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->userId = dataPackage.getRequesterId();
            this->buttonIncomingInteractionBuffer = new unbounded_buffer<DiscordCoreAPI::ButtonInteractionData>;
            Button::buttonInteractionBufferMap.insert_or_assign(this->channelId + this->messageId, this->buttonIncomingInteractionBuffer);
        }

        static void initialize(shared_ptr<DiscordCoreInternal::InteractionManager> interactionsNew) {
            Button::interactions = interactionsNew;
            Button::threadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
        }

        static void cleanup() {
            Button::threadContext->releaseGroup();
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
                cout << "Button::run() Error: " << error.what() << endl;
            }
            return this->responseVector;
        }

        ~Button() {
            done();
            Button::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
        }

    protected:
        static shared_ptr<DiscordCoreInternal::InteractionManager> interactions;
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        unbounded_buffer<DiscordCoreAPI::ButtonInteractionData>* buttonIncomingInteractionBuffer{ nullptr };
        DiscordCoreAPI::ButtonInteractionData interactionData{};
        unbounded_buffer<exception> errorBuffer{ nullptr };
        vector<ButtonResponseData> responseVector{};
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
                        DiscordCoreAPI::ButtonInteractionData buttonInteractionData = receive(Button::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        if (buttonInteractionData.user.id != this->userId) {
                            DiscordCoreAPI::CreateInteractionResponseData createResponseData(buttonInteractionData);
                            DiscordCoreAPI::EmbedData embedData;
                            embedData.setColor("FEFEFE");
                            embedData.setTitle("__**Permission Issue:**__");
                            embedData.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
                            embedData.setDescription("Sorry, but that button can only be pressed by <@!" + this->userId + ">!");
                            createResponseData.addMessageEmbed(embedData);
                            createResponseData.data.data.flags = 64;
                            Button::interactions->createInteractionResponseAsync(createResponseData).get();
                        }
                        else {
                            this->interactionData = buttonInteractionData;
                            this->buttonId = buttonInteractionData.customId;
                            DiscordCoreAPI::CreateInteractionResponseData dataPackage(buttonInteractionData);
                            dataPackage.setResponseType(DiscordCoreAPI::InteractionCallbackType::DeferredUpdateMessage);
                            Button::interactions->createInteractionResponseAsync(dataPackage);
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
                        DiscordCoreAPI::ButtonInteractionData buttonInteractionData = receive(Button::buttonIncomingInteractionBuffer, this->maxTimeInMs);
                        this->interactionData = buttonInteractionData;
                        this->buttonId = buttonInteractionData.customId;
                        ButtonResponseData response;
                        response.buttonId = this->buttonId;
                        response.channelId = this->channelId;
                        response.messageId = this->messageId;
                        response.userId = buttonInteractionData.user.id;
                        this->responseVector.push_back(response);
                        DiscordCoreAPI::CreateInteractionResponseData dataPackage(buttonInteractionData);
                        dataPackage.setResponseType(DiscordCoreAPI::InteractionCallbackType::DeferredUpdateMessage);
                        Button::interactions->createInteractionResponseAsync(dataPackage);
                    }
                }
                Button::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
                done();
            }
            catch (exception& e) {
                this->buttonId = "exit";
                send(this->errorBuffer, e);
                done();

                Button::buttonInteractionBufferMap.erase(this->channelId + this->messageId);
                return;
            }
        }
    };
    map<string, unbounded_buffer<DiscordCoreAPI::SelectMenuInteractionData>*> SelectMenu::selectMenuInteractionBufferMap{};
    map<string, unbounded_buffer<DiscordCoreAPI::ButtonInteractionData>*> Button::buttonInteractionBufferMap{};
    shared_ptr<DiscordCoreInternal::InteractionManager> SelectMenu::interactions{ nullptr };
    shared_ptr<DiscordCoreInternal::InteractionManager> Button::interactions{ nullptr };
    shared_ptr<DiscordCoreInternal::ThreadContext> SelectMenu::threadContext{ nullptr };
    shared_ptr<DiscordCoreInternal::ThreadContext> Button::threadContext{ nullptr };
};
#endif