// InteractionEntities.hpp - Header for the interaction related classes and structs.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _INTERACTION_ENTITIES_
#define _INTERACTION_ENTITIES_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DataParsingFunctions.hpp"
#include "EventManager.hpp"
#include "JSONIFIer.hpp"

namespace DiscordCoreAPI {

    /**
    * \addtogroup foundation_entities
    * @{
    */
    /// Defer component response data. \brief Defer component response data.
    struct DiscordCoreAPI_Dll DeferComponentResponseData {

        friend class InputEventHandler;

        DeferComponentResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->responseType = InputEventResponseType::DeferredResponse;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->type = InteractionCallbackType::DeferredUpdateMessage;
        }

        DeferComponentResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->responseType = InputEventResponseType::DeferredResponse;
            this->type = InteractionCallbackType::DeferredUpdateMessage;
        }
    protected:
        InteractionPackageData interactionPackage{};
        InputEventResponseType responseType{};
        InteractionCallbackType type{};
    };

    /// Create Interaction response data. \brief Create Interaction response data.
    struct DiscordCoreAPI_Dll CreateInteractionResponseData {
    public:

        friend string DiscordCoreInternal::getCreateInteractionResponsePayload(DiscordCoreAPI::CreateInteractionResponseData dataPackage);
        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;
        friend class SelectMenuCollector;
        friend class ButtonCollector;

        CreateInteractionResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.flags = dataPackage.flags;
            this->data.data.tts = dataPackage.tts;
        }

        CreateInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }

        CreateInteractionResponseData(InteractionData dataPackage) {
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

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customId A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        /// \returns void
        void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
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

        /// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
        /// \param disabled Whether the select-menu is active or not.
        /// \param customId A custom id to give for identifying the select-menu.
        /// \param options A vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        /// \returns void
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
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

        /// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
        /// \param dataPackage An AllowedMentionsData structure.
        /// \returns void
        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        /// \returns void
        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        /// Sets the response type of the current Message. \brief Sets the response type of the current Message.
        /// \param type Interaction callback type.
        /// \returns void
        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        /// \returns void
        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A string, containing the content.
        /// \returns void
        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        /// \returns void
        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };

        CreateInteractionResponseData() {};
    };

    /// Create deferred Interaction response data. \brief Create deferred Interaction response data.
    struct DiscordCoreAPI_Dll CreateDeferredInteractionResponseData {

        friend string DiscordCoreInternal::getCreateDeferredInteractionResponsePayload(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage);
        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        CreateDeferredInteractionResponseData(RespondToInputEventData dataPackage) {
            this->data.type = InteractionCallbackType::DeferredChannelMessageWithSource;
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.flags = dataPackage.flags;
            this->data.data.tts = dataPackage.tts;
        }

        CreateDeferredInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->data.type = InteractionCallbackType::DeferredChannelMessageWithSource;
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->requesterId = dataPackage.getAuthorId();
            this->channelId = dataPackage.getChannelId();
            this->data.data.flags = 64;
        }

    protected:

        CreateDeferredInteractionResponseData() {};

        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
        string channelId{ "" };
    };

    /// Create ephemeral Interaction response data. \brief Create ephemeral Interaction response data.
    struct DiscordCoreAPI_Dll CreateEphemeralInteractionResponseData {
    public:

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        CreateEphemeralInteractionResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.tts = dataPackage.tts;
            this->data.data.flags = 64;
        }

        CreateEphemeralInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
            this->data.data.flags = 64;
        }

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customId A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        /// \returns void
        void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
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

        /// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
        /// \param disabled Whether the select-menu is active or not.
        /// \param customId A custom id to give for identifying the select-menu.
        /// \param options A vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        /// \returns void
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
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

        /// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
        /// \param dataPackage An AllowedMentionsData structure.
        /// \returns void
        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        /// \returns void
        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        /// Sets the response type of the current Message. \brief Sets the response type of the current Message.
        /// \param type Interaction callback type.
        /// \returns void
        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        /// \returns void
        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A string, containing the content.
        /// \returns void
        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        /// \returns void
        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };

    };

    /// Edit Interaction response data. \brief Edit Interaction response data.
    struct DiscordCoreAPI_Dll EditInteractionResponseData {

        friend string DiscordCoreInternal::getEditInteractionResponsePayload(DiscordCoreAPI::EditInteractionResponseData dataPackage);
        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        EditInteractionResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.type = InteractionCallbackType::UpdateMessage;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.flags = dataPackage.flags;
            this->data.data.tts = dataPackage.tts;
        }

        EditInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->data.type = InteractionCallbackType::UpdateMessage;
            this->requesterId = dataPackage.getRequesterId();
        }

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customId A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        /// \returns void
        void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
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

        /// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
        /// \param disabled Whether the select-menu is active or not.
        /// \param customId A custom id to give for identifying the select-menu.
        /// \param options A vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        /// \returns void
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
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

        /// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
        /// \param dataPackage An AllowedMentionsData structure.
        /// \returns void
        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        /// \returns void
        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        /// Sets the response type of the current Message. \brief Sets the response type of the current Message.
        /// \param type Interaction callback type.
        /// \returns void
        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        /// \returns void
        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A string, containing the content.
        /// \returns void
        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        /// \returns void
        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }
    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
    };

    /// Delete Interaction response data. \brief Delete Interaction response data.
    struct DiscordCoreAPI_Dll DeleteInteractionResponseData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        DeleteInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
        }

    protected:
        InteractionPackageData interactionPackage{};
        unsigned __int32 timeDelay{ 0 };
    };

    /// Create follow up Message data. \brief Create follow up Message data.
    struct DiscordCoreAPI_Dll CreateFollowUpMessageData {

        friend string DiscordCoreInternal::getPostFollowUpMessagePayload(DiscordCoreAPI::CreateFollowUpMessageData dataPackage);
        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        CreateFollowUpMessageData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.flags = dataPackage.flags;
            this->data.data.tts = dataPackage.tts;
        }

        CreateFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customId A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        /// \returns void
        void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
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

        /// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
        /// \param disabled Whether the select-menu is active or not.
        /// \param customId A custom id to give for identifying the select-menu.
        /// \param options A vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        /// \returns void
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
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

        /// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
        /// \param dataPackage An AllowedMentionsData structure.
        /// \returns void
        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        /// \returns void
        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        /// Sets the response type of the current Message. \brief Sets the response type of the current Message.
        /// \param type Interaction callback type.
        /// \returns void
        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        /// \returns void
        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A string, containing the content.
        /// \returns void
        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        /// \returns void
        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };

        CreateFollowUpMessageData() {};
    };

    /// Create ephemeral follow up Message data. \brief Create ephemeral follow up Message data.
    struct DiscordCoreAPI_Dll CreateEphemeralFollowUpMessageData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        CreateEphemeralFollowUpMessageData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.tts = dataPackage.tts;
            this->data.data.flags = 64;
        }

        CreateEphemeralFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
            this->data.data.flags = 64;
        }

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customId A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        /// \returns void
        void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
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

        /// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
        /// \param disabled Whether the select-menu is active or not.
        /// \param customId A custom id to give for identifying the select-menu.
        /// \param options A vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        /// \returns void
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
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

        /// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
        /// \param dataPackage An AllowedMentionsData structure.
        /// \returns void
        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        /// \returns void
        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        /// Sets the response type of the current Message. \brief Sets the response type of the current Message.
        /// \param type Interaction callback type.
        /// \returns void
        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        /// \returns void
        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A string, containing the content.
        /// \returns void
        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        /// \returns void
        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
    };

    /// Edit follow up Message data. \brief Edit follow up Message data.
    struct DiscordCoreAPI_Dll EditFollowUpMessageData {

        friend string DiscordCoreInternal::getEditFollowUpMessagePayload(DiscordCoreAPI::EditFollowUpMessageData dataPackage);
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        EditFollowUpMessageData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.type = InteractionCallbackType::UpdateMessage;
            this->messagePackage.channelId = dataPackage.channelId;
            this->messagePackage.messageId = dataPackage.messageId;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.flags = dataPackage.flags;
            this->data.data.tts = dataPackage.tts;
        }

        EditFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
            this->data.type = InteractionCallbackType::UpdateMessage;
            this->requesterId = dataPackage.getRequesterId();
        }

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customId A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        /// \returns void
        void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
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

        /// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
        /// \param disabled Whether the select-menu is active or not.
        /// \param customId A custom id to give for identifying the select-menu.
        /// \param options A vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        /// \returns void
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
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

        /// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
        /// \param dataPackage An AllowedMentionsData structure.
        /// \returns void
        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        /// \returns void
        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        /// Sets the response type of the current Message. \brief Sets the response type of the current Message.
        /// \param type Interaction callback type.
        /// \returns void
        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        /// \returns void
        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A string, containing the content.
        /// \returns void
        void addContent(string dataPackage) {
            this->data.data.content = dataPackage;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        /// \returns void
        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

    protected:
        InteractionPackageData interactionPackage{};
        MessagePackageData messagePackage{};
        InteractionResponseData data{};
        string requesterId{ "" };
    };

    /// Delete follow up Message data. \brief Delete follow up Message data;
    struct DiscordCoreAPI_Dll DeleteFollowUpMessageData {

        friend class DiscordCoreInternal::InteractionManagerAgent;
        friend class DiscordCoreInternal::InteractionManager;
        friend class InputEventHandler;

        DeleteFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
        }

    protected:
        InteractionPackageData interactionPackage{};
        MessagePackageData messagePackage{};
        unsigned __int32 timeDelay{ 0 };
    };

    /// Get Interaction response data. \brief Get Interaction response data.
    struct DiscordCoreAPI_Dll GetInteractionResponseData {
        string interactionToken{ "" }; ///< Interaction token.
        string applicationId{ "" }; ///< application id.
    };

    /// Get FollowUp Message data. \brief Get FollowUp Message data.
    struct DiscordCoreAPI_Dll GetFollowUpMessageData {
        string messageId{ "" };///< Message id.
        string interactionToken{ "" }; ///< Interaction token.
        string applicationId{ "" }; ///< application id.
    };

    /// A single Interaction.
    class DiscordCoreAPI_Dll Interaction : public InteractionData {
    public:

        template<typename returnValueType>
        friend class DiscordCoreAPI::CoRoutine;

        Interaction(InteractionData dataPackage) {
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
    };
    /**@}*/
};

namespace DiscordCoreInternal {

    class DiscordCoreAPI_Dll InteractionManager {
    public:

        friend class DiscordCoreAPI::SelectMenuCollector;
        friend class DiscordCoreAPI::DiscordCoreClient;
        friend class DiscordCoreAPI::ButtonCollector;
        friend class DiscordCoreAPI::Interactions;
        friend class DiscordCoreAPI::EventHandler;
        friend class DiscordCoreAPI::InputEventHandler;

        InteractionManager();

    protected:

        static map<string, shared_ptr<unbounded_buffer<DiscordCoreAPI::MessageData>>> collectMessageDataBuffers;

        DiscordCoreAPI::CoRoutine<DiscordCoreAPI::MessageData> createInteractionResponseAsync(DiscordCoreAPI::CreateInteractionResponseData dataPackage);

        DiscordCoreAPI::CoRoutine<void> createDeferredInteractionResponseAsync(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage);

        DiscordCoreAPI::CoRoutine<DiscordCoreAPI::InteractionResponseData> getInteractionResponseAsync(DiscordCoreAPI::GetInteractionResponseData dataPackage);

        DiscordCoreAPI::CoRoutine<DiscordCoreAPI::MessageData> editInteractionResponseAsync(DiscordCoreAPI::EditInteractionResponseData dataPackage);

        DiscordCoreAPI::CoRoutine<void> deleteInteractionResponseAsync(DiscordCoreAPI::DeleteInteractionResponseData dataPackage);

        void deleteInteractionResponseToBeWrapped(DiscordCoreAPI::DeleteInteractionResponseData dataPackage);

        DiscordCoreAPI::CoRoutine<DiscordCoreAPI::MessageData> createFollowUpMessageAsync(DiscordCoreAPI::CreateFollowUpMessageData dataPackage);

        DiscordCoreAPI::CoRoutine<DiscordCoreAPI::MessageData> getFollowUpMessageAsync(DiscordCoreAPI::GetFollowUpMessageData dataPackage);

        DiscordCoreAPI::CoRoutine<DiscordCoreAPI::MessageData> editFollowUpMessageAsync(DiscordCoreAPI::EditFollowUpMessageData dataPackage);

        DiscordCoreAPI::CoRoutine<void> deleteFollowUpMessageAsync(DiscordCoreAPI::DeleteFollowUpMessageData dataPackage);

        void deleteFollowUpMessageToBeWrapped(DiscordCoreAPI::DeleteFollowUpMessageData dataPackage);
    };
};

namespace DiscordCoreAPI {

    /**
    * \addtogroup utilities
    * @{
    */
    /// Select menu response data. \brief Select menu response data.
    struct DiscordCoreAPI_Dll SelectMenuResponseData {
        InteractionData interactionData{};///< Interaction data.
        string selectionId{ "" };///< Selection id.
        vector<string> values{};///< A vector of the chosen values.
        string channelId{ "" };///< The Channel id where it took place.
        string messageId{ "" };///< The Message id where it took place.
        string userId{ "" };///< The User id who selected the menu options.
    };

    /// SelectMenuCollector, for collecting select-menu input from one or more Users. \brief SelectMenuCollector, for collecting select-menu input from one or more Users.
    class DiscordCoreAPI_Dll SelectMenuCollector :  public DiscordCoreInternal::ThreadContext, public agent {
    public:
        friend class DiscordCoreClient;

        static map<string, unbounded_buffer<InteractionData>*>selectMenuInteractionBufferMap;
        /// Constructor. \brief Constructor.
        /// \param dataPackage An InputEventData structure, from the response that came from the submitted select-menu.
        /// \returns void
        SelectMenuCollector (InputEventData dataPackage);
        /// Used to collect the select-menu inputs from one or more users. \brief Used to collect the select-menu inputs from one or more users.
        /// \param getSelectMenuDataForAllNew Whether or not to collect select-menu input from a single target User or all potential users.
        /// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
        /// \param maxCollectedSelectMenuCountNew The maximum number of inputs to collect before stopping
        /// \param targetUserId The id of the single User to collect inputs from, if getSelectMenuDataForAllNew is set to false.
        /// \returns A vector of SelectMenuResponseData.
        vector<SelectMenuResponseData>  collectSelectMenuData(bool getSelectMenuDataForAllNew, __int32 maxWaitTimeInMsNew, __int32 maxCollectedSelectMenuCountNew, string targetUserId = "");

        ~SelectMenuCollector();

    protected:

        static shared_ptr<DiscordCoreInternal::InteractionManager> interactions;
        unbounded_buffer<DiscordCoreAPI::InteractionData>* selectMenuIncomingInteractionBuffer{ nullptr };
        DiscordCoreAPI::InteractionData interactionData{};
        vector<SelectMenuResponseData> responseVector{};
        __int32 currentCollectedSelectMenuCount{ 0 };
        __int32 maxCollectedSelectMenuCount{ 0 };
        bool getButtonDataForAll{ false };
        unsigned __int32 maxTimeInMs{ 0 };
        string selectMenuId{ "" };
        vector<string> values{};
        bool doWeQuit{ false };
        string channelId{ "" };
        string messageId{ "" };
        string userId{ "" };

        static void initialize(shared_ptr<DiscordCoreInternal::InteractionManager> interactionsNew);

        void run();
    };

    /// Button response data. \brief Button response data.
    struct DiscordCoreAPI_Dll ButtonResponseData {
        InteractionData interactionData{};///< Interaction data.
        string emojiName{ "" };///< The emoji name, if applicable.
        string channelId{ "" };///< The Channel id where it took place.
        string messageId{ "" };///< The Message id where it took place.
        string buttonId{ "" };///< The id of the button, for identification.
        string userId{ "" };///< The User id who selected the menu options.
    };

    /// ButtonCollector, for collecting button input from one or more Users. \brief ButtonCollector, for collecting button input from one or more Users.
    class DiscordCoreAPI_Dll ButtonCollector : public DiscordCoreInternal::ThreadContext, public agent {
    public:
        friend class DiscordCoreClient;

        static map<string, unbounded_buffer<InteractionData>*> buttonInteractionBufferMap;
        /// Constructor. \brief Constructor.
        /// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
        /// \returns void
        ButtonCollector(InputEventData dataPackage);
        /// Used to collect the button inputs from one or more users. \brief Used to collect the button inputs from one or more users.
        /// \param getButtonDataForAllNew Whether or not to collect input from a single target User or all potential users.
        /// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
        /// \param maxNumberOfPressesNew The maximum number of inputs to collect before stopping.
        /// \param targetUserId The id of the single User to collect inputs from, if getButtonDataForAllNew is set to false.
        /// \returns A vector of ButtonResponseData.
        vector<ButtonResponseData>  collectButtonData(bool getButtonDataForAllNew, __int32 maxWaitTimeInMsNew, __int32 maxNumberOfPressesNew, string targetUserId = "");

        ~ButtonCollector();

    protected:

        static shared_ptr<DiscordCoreInternal::InteractionManager> interactions;

        unbounded_buffer<DiscordCoreAPI::InteractionData>* buttonIncomingInteractionBuffer{ nullptr };
        vector<ButtonResponseData> responseVector{};
        InteractionData interactionData{};
        bool getButtonDataForAll{ false };
        unsigned __int32 maxTimeInMs{ 0 };
        __int32 maxNumberOfPresses{ 0 };
        string channelId{ "" };
        string messageId{ "" };
        bool doWeQuit{ false };
        string buttonId{ "" };
        string userId{ "" };

        static void initialize(shared_ptr<DiscordCoreInternal::InteractionManager> interactionsNew);

        void run();
    };
    /**@}*/
};
#endif