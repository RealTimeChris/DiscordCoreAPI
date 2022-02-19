/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// InteractionEntities.hpp - Header for the interaction related classes and structs.
/// May 28, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <FoundationEntities.hpp>
#include <JSONIfier.hpp>
#include <MessageEntities.hpp>
#include <iostream>

namespace DiscordCoreAPI {

    /**
    * \addtogroup foundation_entities
    * @{
    */

    class DiscordCoreAPI_Dll InteractionResponse {
    public:

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customIdNew A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        void addButton(bool disabled, std::string customIdNew, std::string buttonLabel, ButtonStyle buttonStyle, std::string emojiName = "", std::string emojiId = "", std::string url = "") {
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
                    component.style = static_cast<int32_t>(buttonStyle);
                    component.customId = customIdNew;
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
        /// \param customIdNew A custom id to give for identifying the select-menu.
        /// \param options A std::vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        void addSelectMenu(bool disabled, std::string customIdNew, std::vector<SelectOptionData> options, std::string placeholder, int32_t maxValues, int32_t minValues) {
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
                    componentData.customId = customIdNew;
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
        void addAllowedMentions(AllowedMentionsData dataPackage) {
            this->data.data.allowedMentions = dataPackage;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        void addComponentRow(ActionRowData dataPackage) {
            this->data.data.components.push_back(dataPackage);
        }

        /// Sets the response type of the current Message. \brief Sets the response type of the current Message.
        /// \param type Interaction callback type.
        void setResponseType(InteractionCallbackType type) {
            this->data.type = type;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        void addMessageEmbed(EmbedData dataPackage) {
            this->data.data.embeds.push_back(dataPackage);
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A std::string, containing the content.
        void addContent(std::string dataPackage) {
            this->data.data.content = dataPackage;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        void setTTSStatus(bool enabledTTs) {
            this->data.data.tts = enabledTTs;
        }

        virtual ~InteractionResponse() = default;

    protected:

        InteractionPackageData interactionPackage{};
        MessagePackageData messagePackage{};
        InteractionResponseData data{};
        std::string requesterId{ "" };
    };

    /// For deferring a component response. \brief For deferring a component response.
    class DiscordCoreAPI_Dll DeferComponentResponseData : public InteractionResponse {
    public:
        friend CreateInteractionResponseData;
        friend InputEvents;
        DeferComponentResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->responseType = InputEventResponseType::Deferred_Response;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->type = InteractionCallbackType::DeferredUpdateMessage;
        }
        virtual ~DeferComponentResponseData() = default;
    protected:
        InputEventResponseType responseType{};
        InteractionCallbackType type{};
    };

    /// For creating an ephemeral Interaction response. \brief For creating an ephemeral Interaction response.
    class DiscordCoreAPI_Dll CreateEphemeralInteractionResponseData : public InteractionResponse {
    public:
        friend CreateInteractionResponseData;
        friend Interactions;
        friend InputEvents;
        CreateEphemeralInteractionResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            if (dataPackage.eventType == InteractionType::Message_Component) {
                this->data.type = InteractionCallbackType::UpdateMessage;
            }
            else {
                this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            }
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
        virtual ~CreateEphemeralInteractionResponseData() = default;
    };

    /// For creating an Interaction response. \brief For creating an Interaction response.
    class DiscordCoreAPI_Dll CreateInteractionResponseData : public InteractionResponse {
    public:
        friend std::string DiscordCoreInternal::JSONIFY(CreateInteractionResponseData dataPackage);
        friend SelectMenuCollector;
        friend ButtonCollector;
        friend ModalCollector;
        friend Interactions;
        friend InputEvents;

        CreateInteractionResponseData(DeferComponentResponseData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
            this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
            this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
            this->data.type = dataPackage.type;
        }
        CreateInteractionResponseData(CreateEphemeralInteractionResponseData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionPackage.interactionToken;
            this->interactionPackage.applicationId = dataPackage.interactionPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionPackage.interactionId;
            this->data.data.components = dataPackage.data.data.components;
            this->requesterId = dataPackage.requesterId;
            this->data.type = dataPackage.data.type;
            this->data = dataPackage.data;
            this->data.data.flags = 64;
        }
        CreateInteractionResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            if (dataPackage.eventType == InteractionType::Message_Component) {
                this->data.type = InteractionCallbackType::UpdateMessage;
            }
            else {
                this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            }
            if (dataPackage.title != "") {
                this->data.type = InteractionCallbackType::Modal;
            }
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->messagePackage.channelId = dataPackage.channelId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->data.data.title = dataPackage.title;
            this->data.data.customId = dataPackage.customId;
            this->requesterId = dataPackage.requesterId;
            this->data.data.flags = dataPackage.flags;
            this->data.data.tts = dataPackage.tts;
        }
        CreateInteractionResponseData(InteractionData dataPackage) {
            if (dataPackage.type == InteractionType::Message_Component) {
                this->data.type = InteractionCallbackType::UpdateMessage;
            }
            else {
                this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            }
            this->interactionPackage.interactionToken = dataPackage.token;
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
        }
        virtual ~CreateInteractionResponseData() = default;
    };

    /// For creating a deferred Interaction response. \brief For creating a deferred Interaction response.
    class DiscordCoreAPI_Dll CreateDeferredInteractionResponseData : public InteractionResponse {
    public:
        friend std::string DiscordCoreInternal::JSONIFY(CreateDeferredInteractionResponseData dataPackage);
        friend Interactions;
        friend InputEvents;
        CreateDeferredInteractionResponseData(RespondToInputEventData dataPackage) {
            if (dataPackage.eventType == InteractionType::Message_Component) {
                this->data.type = InteractionCallbackType::DeferredUpdateMessage;
            }
            else {
                this->data.type = InteractionCallbackType::DeferredChannelMessageWithSource;
            }
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
        virtual ~CreateDeferredInteractionResponseData() = default;
    protected:
        std::string channelId{ "" };
    };

    /// For getting an Interaction response. \brief For getting an Interaction response.
    struct DiscordCoreAPI_Dll GetInteractionResponseData {
        std::string interactionToken{ "" };///< Interaction token.
        std::string applicationId{ "" };///< application id.
    };
    /// For editing an Interaction response. \brief For editing an Interaction response.
    class DiscordCoreAPI_Dll EditInteractionResponseData : public InteractionResponse {
    public:
        friend std::string DiscordCoreInternal::JSONIFY(EditInteractionResponseData dataPackage);
        friend Interactions;
        friend InputEvents;
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
        virtual ~EditInteractionResponseData() = default;
    };

    /// For deleting an Interaction response. \brief For deleting an Interaction response.
    struct DiscordCoreAPI_Dll DeleteInteractionResponseData {
        friend void deleteInteractionResponseToBeWrapped(DeleteInteractionResponseData dataPackage);
        friend InputEvents;
        friend Interactions;
        DeleteInteractionResponseData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
        }
    protected:
        InteractionPackageData interactionPackage{};
        uint32_t timeDelay{ 0 };
    };

    /// For creating an ephemeral follow up Message. \brief For creating an ephemeral follow up Message.
    class DiscordCoreAPI_Dll CreateEphemeralFollowUpMessageData : public InteractionResponse {
    public:
        friend CreateFollowUpMessageData;
        friend Interactions;
        friend InputEvents;
        CreateEphemeralFollowUpMessageData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            if (dataPackage.eventType == InteractionType::Message_Component) {
                this->data.type = InteractionCallbackType::UpdateMessage;
            }
            else {
                this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            }
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
        virtual ~CreateEphemeralFollowUpMessageData() = default;
    };

    /// For creating a follow up Message. \brief For creating a follow up Message.
    class DiscordCoreAPI_Dll CreateFollowUpMessageData : public InteractionResponse {
    public:
        friend std::string DiscordCoreInternal::JSONIFY(CreateFollowUpMessageData dataPackage);
        friend SelectMenuCollector;
        friend ButtonCollector;
        friend Interactions;
        friend InputEvents;
        CreateFollowUpMessageData(CreateEphemeralFollowUpMessageData dataPackage) {
            this->interactionPackage = dataPackage.interactionPackage;
            this->requesterId = dataPackage.requesterId;
            this->data = dataPackage.data;
        }
        CreateFollowUpMessageData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            if (dataPackage.eventType == InteractionType::Message_Component) {
                this->data.type = InteractionCallbackType::UpdateMessage;
            }
            else {
                this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            }
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
        virtual ~CreateFollowUpMessageData() = default;
    };

    /// For getting a follow-up Message. \brief For getting a follow-up Message.
    struct DiscordCoreAPI_Dll GetFollowUpMessageData {
        std::string messageId{ "" };///< Message id.
        std::string interactionToken{ "" };///< Interaction token.
        std::string applicationId{ "" };///< application id.
    };

    /// For editing a follow up Message. \brief For editing a follow up Message.
    class DiscordCoreAPI_Dll EditFollowUpMessageData : public InteractionResponse {
    public:
        friend std::string DiscordCoreInternal::JSONIFY(EditFollowUpMessageData dataPackage);
        friend Interactions;
        friend InputEvents;
        EditFollowUpMessageData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->data.data.allowedMentions = dataPackage.allowedMentions;
            if (dataPackage.eventType == InteractionType::Message_Component) {
                this->data.type = InteractionCallbackType::UpdateMessage;
            }
            else {
                this->data.type = InteractionCallbackType::ChannelMessageWithSource;
            }
            this->messagePackage.channelId = dataPackage.channelId;
            this->messagePackage.messageId = dataPackage.messageId;
            this->data.data.components = dataPackage.components;
            this->data.data.content = dataPackage.content;
            this->data.data.embeds = dataPackage.embeds;
            this->requesterId = dataPackage.requesterId;
            this->data.data.flags = dataPackage.flags;
            this->data.data.tts = dataPackage.tts;
        }
        virtual ~EditFollowUpMessageData() = default;
    };

    /// For deleting a follow up Message. \brief For deleting a follow up Message.
    struct DiscordCoreAPI_Dll DeleteFollowUpMessageData {
        friend void deleteFollowUpMessageToBeWrapped(DeleteFollowUpMessageData dataPackage);
        friend InputEvents;
        friend Interactions;

        DeleteFollowUpMessageData(RespondToInputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.interactionToken;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionId = dataPackage.interactionId;
            this->messagePackage.messageId = dataPackage.messageId;
        }
    protected:
        InteractionPackageData interactionPackage{};
        MessagePackageData messagePackage{};
        uint32_t timeDelay{ 0 };
    };

    /// A single Interaction.
    struct DiscordCoreAPI_Dll Interaction : public InteractionData {
    public:

        Interaction(InteractionData dataPackage);

        virtual ~Interaction() = default;
    };

    /**@}*/

    /**
    * \addtogroup main_endpoints
    * @{
    */

    /// An interface class for the Interaction related Discord endpoints. \brief An interface class for the Interaction related Discord endpoints.
    class DiscordCoreAPI_Dll Interactions {
    public:

        friend DiscordCoreClient;
        friend EventHandler;
        friend EventManager;

        static void initialize(DiscordCoreInternal::HttpClient*);

        /// Creates a deferred response to an input Interaction. \brief Creates a deferred response to an input Interaction.
        /// \param dataPackage A CreateDeferredInteractionResponseData structure.
        /// \returns A CoRoutine containing void.
        static CoRoutine<void> createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData dataPackage);

        /// Creates a response to an input Interaction. \brief Creates a response to an input Interaction.
        /// \param dataPackage A CreateInteractionResponseData structure.
        /// \returns A CoRoutine containing a MessageData.
        static CoRoutine<Message> createInteractionResponseAsync(CreateInteractionResponseData dataPackage);

        /// Collects an Interaction response. \brief Collects an Interaction response.
        /// \param dataPackage A GetInteractionResponseData structure.
        /// \returns A CoRoutine containing an InteractionResponseData.
        static CoRoutine<InteractionResponseData> getInteractionResponseAsync(GetInteractionResponseData dataPackage);

        /// Edits an Interaction response. \brief Edits an Interaction response.
        /// \param dataPackage A EditInteractionResponseData structure.
        /// \returns A CoRoutine containing a MessageData.
        static CoRoutine<Message> editInteractionResponseAsync(EditInteractionResponseData dataPackage);

        /// Deletes an Interaction respnose. \brief Deletes an Interaction respnose.
        /// \param dataPackage A DeleteInteractionResponseData structure.
        /// \returns A CoRoutine containing void.
        static CoRoutine<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage);

        /// Creates a follow up Message to an input Interaction. \brief Creates a follow up Message to an input Interaction.
        /// \param dataPackage A CreateFollowUpMessageData structure.
        /// \returns A CoRoutine containing a MessageData.
        static CoRoutine<Message> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage);

        /// Creates a follow up Message to an input Interaction. \brief Creates a follow up Message to an input Interaction.
        /// \param dataPackage A CreateFollowUpMessageData structure.
        /// \returns A CoRoutine containing a MessageData.
        static CoRoutine<Message> getFollowUpMessageAsync(GetFollowUpMessageData dataPackage);

        /// Edits a follow up Message. \brief Edits a follow up Message.
        /// \param dataPackage A EditFollowUpMessageData structure.
        /// \returns A CoRoutine containing a MessageData.
        static CoRoutine<Message> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage);

        /// Deletes a follow up Message. \brief Deletes a follow up Message.
        /// \param dataPackage A DeleteFollowUpMessageData structure.
        /// \returns A CoRoutine containing void.
        static CoRoutine<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage);

    protected:

        static std::unordered_map<std::string, std::unique_ptr<UnboundedMessageBlock<MessageData>>> collectMessageDataBuffers;
        static DiscordCoreInternal::HttpClient* httpClient;
        static void deleteInteractionResponseToBeWrapped(DeleteInteractionResponseData dataPackage);
        static void deleteFollowUpMessageToBeWrapped(DeleteFollowUpMessageData dataPackage);
    };

    /**@}*/

    /**
    * \addtogroup utilities
    * @{
    */

    /// Select menu response data. \brief Select menu response data.
    struct DiscordCoreAPI_Dll SelectMenuResponseData {
        InteractionData interactionData{};///< Interaction data.
        std::string selectionId{ "" };///< Selection id.
        std::vector<std::string> values{};///< A std::vector of the chosen values.
        std::string channelId{ "" };///< The Channel id where it took place.
        std::string messageId{ "" };///< The Message id where it took place.
        std::string userId{ "" };///< The User id who selected the menu options.
    };

    /// SelectMenuCollector, for collecting select-menu input from one or more Users. \brief SelectMenuCollector, for collecting select-menu input from one or more Users.
    class DiscordCoreAPI_Dll SelectMenuCollector {
    public:

        friend DiscordCoreClient;

        static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*>selectMenuInteractionBufferMap;

        /// Constructor. \brief Constructor.
        /// \param dataPackage An InputEventData structure, from the response that came from the submitted select-menu.
        SelectMenuCollector(InputEventData dataPackage);

        /// Used to collect the select-menu inputs from one or more users. \brief Used to collect the select-menu inputs from one or more users.
        /// \param getSelectMenuDataForAllNew Whether or not to collect select-menu input from a single target User or all potential users.
        /// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
        /// \param maxCollectedSelectMenuCountNew The maximum number of inputs to collect before stopping
        /// \param targetUserId The id of the single User to collect inputs from, if getSelectMenuDataForAllNew is set to false.
        /// \returns A std::vector of SelectMenuResponseData.
        CoRoutine<std::vector<SelectMenuResponseData>> collectSelectMenuData(bool getSelectMenuDataForAllNew, int32_t maxWaitTimeInMsNew, int32_t maxCollectedSelectMenuCountNew, std::string targetUserId = "");

        ~SelectMenuCollector();

    protected:
        std::unique_ptr<UnboundedMessageBlock<InteractionData>> selectMenuIncomingInteractionBuffer{ nullptr };
        std::vector<SelectMenuResponseData> responseVector{};
        int32_t currentCollectedSelectMenuCount{ 0 };
        int32_t maxCollectedSelectMenuCount{ 0 };
        bool getSelectMenuDataForAll{ false };
        InteractionData interactionData{};
        std::vector<std::string> values{};
        std::string selectMenuId{ "" };
        std::string channelId{ "" };
        std::string messageId{ "" };
        uint32_t maxTimeInMs{ 0 };
        std::string userId{ "" };
        bool doWeQuit{ false };
        void run();
    };

    /// Button response data. \brief Button response data.
    struct DiscordCoreAPI_Dll ButtonResponseData {
        operator InteractionData() {
            return this->interactionData;
        }
        InteractionData interactionData{};///< Interaction data.
        std::string emojiName{ "" };///< The emoji name, if applicable.
        std::string channelId{ "" };///< The Channel id where it took place.
        std::string messageId{ "" };///< The Message id where it took place.
        std::string buttonId{ "" };///< The id of the button, for identification.
        std::string userId{ "" };///< The User id who selected the menu options.
    };

    /// ButtonCollector, for collecting button input from one or more Users. \brief ButtonCollector, for collecting button input from one or more Users.
    class DiscordCoreAPI_Dll ButtonCollector {
    public:

        friend DiscordCoreClient;

        static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> buttonInteractionBufferMap;

        /// Constructor. \brief Constructor.
        /// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
        ButtonCollector(InputEventData dataPackage);

        /// Used to collect the button inputs from one or more users. \brief Used to collect the button inputs from one or more users.
        /// \param getButtonDataForAllNew Whether or not to collect input from a single target User or all potential users.
        /// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
        /// \param maxNumberOfPressesNew The maximum number of inputs to collect before stopping.
        /// \param targetUserId The id of the single User to collect inputs from, if getButtonDataForAllNew is set to false.
        /// \returns A std::vector of ButtonResponseData.
        CoRoutine<std::vector<ButtonResponseData>> collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew, int32_t maxNumberOfPressesNew, std::string targetUserId = "");

        ~ButtonCollector();

    protected:
        std::unique_ptr<UnboundedMessageBlock<InteractionData>> buttonIncomingInteractionBuffer{ nullptr };
        std::vector<ButtonResponseData> responseVector{};
        int32_t currentCollectedButtonCount{ 0 };
        int32_t maxCollectedButtonCount{ 0 };
        InteractionData interactionData{};
        std::vector<std::string> values{};
        bool getButtonDataForAll{ false };
        std::string channelId{ "" };
        std::string messageId{ "" };
        std::string buttonId{ "" };
        uint32_t maxTimeInMs{ 0 };
        std::string userId{ "" };
        bool doWeQuit{ false };

        void run();
    };

    /// Button response data. \brief Button response data.
    struct DiscordCoreAPI_Dll ModalResponseData {
        operator InteractionData() {

            return this->interactionData;
        }

        InteractionData interactionData{};///< Interaction data.
        std::string customIdSmall{ "" };///< The customId of the particular input.
        std::string channelId{ "" };///< The Channel id where it took place.
        std::string customId{ "" };///< The customId of the modal component.
        std::string userId{ "" };///< The User id who selected the menu options.
        std::string value{ "" };/// The input value of the modal component.
    };

    /// ModalCollector, for collecting modal text input from one or more Users. \brief ModalCollector, for collecting modal text input from one or more Users.
    class DiscordCoreAPI_Dll ModalCollector {
    public:
        friend DiscordCoreClient;

        static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> modalInteractionBufferMap;

        /// Constructor. \brief Constructor.
        /// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
        ModalCollector(InputEventData dataPackage);

        /// Used to collect the button inputs from one or more users. \brief Used to collect the button inputs from one or more users.
        /// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
        /// \returns A std::vector of ButtonResponseData.
        CoRoutine<ModalResponseData> collectModalData(int32_t maxWaitTimeInMsNew);

        ~ModalCollector();

    protected:

        std::unique_ptr<UnboundedMessageBlock<InteractionData>> modalIncomingInteractionBuffer{ nullptr };
        int32_t currentCollectedButtonCount{ 0 };
        ModalResponseData responseData{};
        std::string channelId{ "" };
        uint32_t maxTimeInMs{ 0 };
        bool doWeQuit{ false };

        void run();
    };

    /**@}*/
};