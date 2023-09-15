/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// InteractionEntities.hpp - Header for the interaction related classes and
/// structs. May 28, 2021 Chris M.
/// https://discordcoreapi.com
/// \file InteractionEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/WebHookEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	using AutoCompleteEntryFunction = std::function<std::string(std::string)>;

	class DiscordCoreAPI_Dll InteractionResponseBase {
	  public:
		template<typename ValueType> friend struct jsonifier::core;

		UnorderedSet<std::string_view> excludedKeys{};

		/// @brief Adds a button to the response MessageData.
		/// @param disabled Whether the button is active or not.
		/// @param customIdNew A custom id to give for identifying the button.
		/// @param buttonLabel A visible label for the button.
		/// @param buttonStyle The style of the button.
		/// @param emojiName An emoji name, if desired.
		/// @param emojiId An emoji id, if desired.
		/// @param url A url, if applicable.
		InteractionResponseBase& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
			const std::string& emojiName = "", Snowflake emojiId = Snowflake{}, const std::string& url = "");

		/// @brief Adds a select-menu to the response MessageData.
		/// @param disabled Whether the select-menu is active or not.
		/// @param customIdNew A custom id to give for identifying the select-menu.
		/// @param options A vector of select-menu-options to offer.
		/// @param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues Maximum number of selections that are possible.
		/// @param minValues Minimum required number of selections that are required.
		InteractionResponseBase& addSelectMenu(bool disabled, const std::string& customIdNew, jsonifier::vector<SelectOptionData> options, const std::string& placeholder,
			uint64_t maxValues, uint64_t minValues, SelectMenuType type, jsonifier::vector<ChannelType> channelTypes = jsonifier::vector<ChannelType>{});

		/// @brief Adds a modal to the response MessageData.
		/// @param topTitleNew A title for the modal.
		/// @param topCustomIdNew A custom id to give for the modal.
		/// @param titleNew A title for the modal's individual input.
		/// @param customIdNew A custom id to give for the modal's individual input.
		/// @param required Is it a required response?
		/// @param minLength Minimum length.
		/// @param maxLength Maximum length.
		/// @param inputStyle The input style.
		/// @param label A label for the modal.
		/// @param placeholder A placeholder for the modal.
		/// @returns RespondToInputEventData& A reference to this data structure.
		InteractionResponseBase& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew,
			bool required, uint64_t minLength, uint64_t maxLength, TextInputStyle inputStyle, const std::string& label = "", const std::string& placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile The file to be added.
		/// @returns MessageResponseBase& A reference to this data structure.
		InteractionResponseBase& addFile(const File& theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage An AllowedMentionsData structure.
		InteractionResponseBase& addAllowedMentions(const AllowedMentionsData& dataPackage);

		/// @brief For setting the components in a response.
		/// @param dataPackage An ActionRowData structure.
		InteractionResponseBase& addComponentRow(const ActionRowData& dataPackage);

		/// @brief Sets the response type of the current MessageData.
		/// @param type Interaction callback type.
		InteractionResponseBase& setResponseType(InteractionCallbackType type);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage An EmbedData structure.SendDMData
		InteractionResponseBase& addMessageEmbed(const EmbedData& dataPackage);

		/// @brief For setting the content in a response.
		/// @param dataPackage A string, containing the content.
		InteractionResponseBase& addContent(const std::string& dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs A bool.
		InteractionResponseBase& setTTSStatus(bool enabledTTs);

		InteractionResponseBase& setFlags(uint64_t flag);

		InteractionResponseData getInteractionResponseData();

		void generateExcludedKeys();

		virtual ~InteractionResponseBase() = default;

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		InteractionCallbackData data{};
		InteractionCallbackType type{};
	};

	/// @brief For creating an ephemeral Interaction response.
	class DiscordCoreAPI_Dll CreateEphemeralInteractionResponseData : public InteractionResponseBase {
	  public:
		friend class CreateInteractionResponseData;
		friend class Interactions;
		friend class InputEvents;

		CreateEphemeralInteractionResponseData(const RespondToInputEventData& dataPackage);

		virtual ~CreateEphemeralInteractionResponseData() = default;
	};

	/// @brief For creating a deferred Interaction response.
	class DiscordCoreAPI_Dll CreateDeferredInteractionResponseData : public InteractionResponseBase {
	  public:
		friend class CreateInteractionResponseData;
		friend class Interactions;
		friend class InputEvents;

		CreateDeferredInteractionResponseData(const RespondToInputEventData& dataPackage);

		virtual ~CreateDeferredInteractionResponseData() = default;
	};

	/// @brief For creating an Interaction response.
	class DiscordCoreAPI_Dll CreateInteractionResponseData : public InteractionResponseBase {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class SelectMenuCollector;
		friend class ButtonCollector;
		friend class ModalCollector;
		friend class Interactions;
		friend class InputEvents;

		CreateInteractionResponseData(const CreateEphemeralInteractionResponseData& dataPackage);

		CreateInteractionResponseData(const CreateDeferredInteractionResponseData& dataPackage);

		CreateInteractionResponseData(const RespondToInputEventData& dataPackage);

		CreateInteractionResponseData(const InteractionData& dataPackage);

		CreateInteractionResponseData() = default;

		virtual ~CreateInteractionResponseData() = default;
	};

	/// @brief For getting an Interaction response.
	struct GetInteractionResponseData {
		std::string interactionToken{};///< Interaction token.
		Snowflake applicationId{};///< application id.
	};

	/// @brief For editing an Interaction response.
	class DiscordCoreAPI_Dll EditInteractionResponseData : public EditWebHookData {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class Interactions;
		friend class InputEvents;

		UnorderedSet<std::string_view> excludedKeys{};

		EditInteractionResponseData(const RespondToInputEventData& dataPackage);

		void generateExcludedKeys();

		virtual ~EditInteractionResponseData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
	};

	/// @brief For deleting an Interaction response.
	struct DiscordCoreAPI_Dll DeleteInteractionResponseData {
		friend class Interactions;
		friend class InputEvents;

		DeleteInteractionResponseData(const RespondToInputEventData& dataPackage);

	  protected:
		InteractionPackageData interactionPackage{};
		uint32_t timeDelay{};
	};

	/// @brief For creating an ephemeral follow up MessageData.
	class DiscordCoreAPI_Dll CreateEphemeralFollowUpMessageData : public ExecuteWebHookData {
	  public:
		friend class CreateFollowUpMessageData;
		friend class Interactions;
		friend class InputEvents;

		CreateEphemeralFollowUpMessageData(const RespondToInputEventData& dataPackage);

		virtual ~CreateEphemeralFollowUpMessageData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
	};

	/// @brief For creating a follow up MessageData.
	class DiscordCoreAPI_Dll CreateFollowUpMessageData : public ExecuteWebHookData {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class SelectMenuCollector;
		friend class ButtonCollector;
		friend class Interactions;
		friend class InputEvents;

		UnorderedSet<std::string_view> excludedKeys{};

		CreateFollowUpMessageData(const CreateEphemeralFollowUpMessageData& dataPackage);

		CreateFollowUpMessageData(const RespondToInputEventData& dataPackage);

		void generateExcludedKeys();

		virtual ~CreateFollowUpMessageData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
	};

	/// @brief For getting a follow-up MessageData.
	struct GetFollowUpMessageData {
		std::string interactionToken{};///< Interaction token.
		Snowflake applicationId{};///< application id.
		Snowflake messageId{};///< Message id.
	};

	/// @brief For editing a follow up MessageData.
	class DiscordCoreAPI_Dll EditFollowUpMessageData : public EditWebHookData {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class Interactions;
		friend class InputEvents;

		UnorderedSet<std::string_view> excludedKeys{};

		EditFollowUpMessageData(const RespondToInputEventData& dataPackage);

		virtual ~EditFollowUpMessageData() = default;

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
	};

	/// @brief For deleting a follow up MessageData.
	struct DiscordCoreAPI_Dll DeleteFollowUpMessageData {
		friend class Interactions;
		friend class InputEvents;

		DeleteFollowUpMessageData(const RespondToInputEventData& dataPackage);

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		uint32_t timeDelay{};
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// @brief An interface class for the Interaction related Discord endpoints.
	class DiscordCoreAPI_Dll Interactions {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreClient;
		friend class EventManager;
		friend class InputEvents;

		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Creates a response to an input Interaction.
		/// @param dataPackage A CreateInteractionResponseData structure.
		/// @returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> createInteractionResponseAsync(CreateInteractionResponseData dataPackage);

		/// @brief Collects an Interaction response.
		/// @param dataPackage A GetInteractionResponseData structure.
		/// @returns A CoRoutine containing an InteractionResponseData.
		static CoRoutine<MessageData> getInteractionResponseAsync(GetInteractionResponseData dataPackage);

		/// @brief Edits an Interaction response.
		/// @param dataPackage A EditInteractionResponseData structure.
		/// @returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> editInteractionResponseAsync(EditInteractionResponseData dataPackage);

		/// @brief Deletes an Interaction respnose.
		/// @param dataPackage A DeleteInteractionResponseData structure.
		/// @returns A CoRoutine containing void.
		static CoRoutine<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage);

		/// @brief Creates a follow up Message to an input Interaction.
		/// @param dataPackage A CreateFollowUpMessageData structure.
		/// @returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage);

		/// @brief Creates a follow up Message to an input Interaction.
		/// @param dataPackage A CreateFollowUpMessageData structure.
		/// @returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> getFollowUpMessageAsync(GetFollowUpMessageData dataPackage);

		/// @brief Edits a follow up MessageData.
		/// @param dataPackage A EditFollowUpMessageData structure.
		/// @returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage);

		/// @brief Deletes a follow up MessageData.
		/// @param dataPackage A DeleteFollowUpMessageData structure.
		/// @returns A CoRoutine containing void.
		static CoRoutine<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;

		static MessageData createInteractionResponse(CreateInteractionResponseData dataPackage);

		static MessageData editInteractionResponse(EditInteractionResponseData dataPackage);

		static MessageData getInteractionResponse(GetInteractionResponseData dataPackage);

		static MessageData createFollowUpMessage(CreateFollowUpMessageData dataPackage);

		static MessageData editFollowUpMessage(EditFollowUpMessageData dataPackage);
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// @brief Select menu response data.
	struct SelectMenuResponseData {
		inline operator InteractionData() {
			return *interactionData;
		}

		inline SelectMenuResponseData& operator=(const SelectMenuResponseData& other) {
			if (this != &other) {
				*interactionData = *other.interactionData;
				selectionId		 = other.selectionId;
				messageId		 = other.messageId;
				channelId		 = other.channelId;
				values			 = other.values;
				userId			 = other.userId;
			}
			return *this;
		}

		inline SelectMenuResponseData(const SelectMenuResponseData& other) {
			*this = other;
		}

		inline SelectMenuResponseData& operator=(SelectMenuResponseData& other) {
			if (this != &other) {
				*interactionData = *other.interactionData;
				selectionId		 = other.selectionId;
				messageId		 = other.messageId;
				channelId		 = other.channelId;
				values			 = other.values;
				userId			 = other.userId;
			}
			return *this;
		}

		inline SelectMenuResponseData(SelectMenuResponseData& other) {
			*this = other;
		}

		inline SelectMenuResponseData() = default;

		UniquePtr<InteractionData> interactionData{ makeUnique<InteractionData>() };///< Interaction data.
		jsonifier::vector<std::string> values{};///< A vector of the chosen values.
		std::string selectionId{};///< Selection id.
		Snowflake channelId{};///< The ChannelData id where it took place.
		Snowflake messageId{};///< The Message id where it took place.
		Snowflake userId{};///< The UserData id who selected the menu options.
	};

	/// SelectMenuCollector, for collecting select-menu input from one or more
	/// @brief SelectMenuCollector, for collecting select-menu input from one
	/// or more Users.
	class DiscordCoreAPI_Dll SelectMenuCollector {
	  public:
		friend class DiscordCoreClient;

		static UnorderedMap<std::string, UnboundedMessageBlock<InteractionData>*> selectMenuInteractionBuffersMap;
		static DiscordCoreInternal::TriggerEvent<void, InteractionData> selectMenuInteractionEventsMap;

		/// @brief Constructor.
		/// @param dataPackage An InputEventData structure, from the response that came from the submitted select-menu.
		SelectMenuCollector(InputEventData& dataPackage);

		/// @brief Used to collect the select-menu inputs from one or more users.
		/// @param getSelectMenuDataForAllNew Whether or not to collect select-menu input from a single target UserData or all potential users.
		/// @param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// @param maxCollectedSelectMenuCountNew The maximum number of inputs to collect before stopping.
		/// @param errorMessageDataNew The message-data for when an individual other than the selected individual attemps to use this interaction.
		/// @param targetUserId The id of the single UserData to collect inputs from, if getSelectMenuDataForAllNew is set to false.
		/// @returns A vector of SelectMenuResponseData.
		CoRoutine<jsonifier::vector<SelectMenuResponseData>, false> collectSelectMenuData(bool getSelectMenuDataForAllNew, uint32_t maxWaitTimeInMsNew,
			uint32_t maxCollectedSelectMenuCountNew, CreateInteractionResponseData errorMessageDataNew, Snowflake targetUserId = Snowflake{});

		/// @brief Used to collect the select-menu inputs from one or more users.
		/// @param triggerFunctionNew A std::function<bool(InteractionData)> to decide whether or not to trigger the event's main function.
		/// @param functionNew Takes a DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> as a function to be executed upon returning true from the "trigger-function".
		void collectSelectMenuData(std::function<bool(InteractionData)> triggerFunctionNew, DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew);

		~SelectMenuCollector();

	  protected:
		UnboundedMessageBlock<InteractionData> selectMenuIncomingInteractionBuffer{};
		UniquePtr<InteractionData> interactionData{ makeUnique<InteractionData>() };
		jsonifier::vector<SelectMenuResponseData> responseVector{};
		CreateInteractionResponseData errorMessageData{};
		uint32_t currentCollectedSelectMenuCount{};
		jsonifier::vector<std::string> values{};
		uint32_t maxCollectedSelectMenuCount{};
		bool getSelectMenuDataForAll{};
		std::string buffersMapKey{};
		std::string selectMenuId{};
		uint32_t maxTimeInMs{};
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake userId{};
		bool doWeQuit{};

		void run();
	};

	/// @brief Button response data.
	struct ButtonResponseData {
		inline operator InteractionData() {
			return *interactionData;
		}

		inline ButtonResponseData& operator=(const ButtonResponseData& other) {
			if (this != &other) {
				*interactionData = *other.interactionData;
				messageId		 = other.messageId;
				channelId		 = other.channelId;
				emojiName		 = other.emojiName;
				buttonId		 = other.buttonId;
				userId			 = other.userId;
			}
			return *this;
		}

		inline ButtonResponseData(const ButtonResponseData& other) {
			*this = other;
		}

		inline ButtonResponseData& operator=(ButtonResponseData& other) {
			if (this != &other) {
				*interactionData = *other.interactionData;
				messageId		 = other.messageId;
				channelId		 = other.channelId;
				emojiName		 = other.emojiName;
				buttonId		 = other.buttonId;
				userId			 = other.userId;
			}
			return *this;
		}

		inline ButtonResponseData(ButtonResponseData& other) {
			*this = other;
		}

		inline ButtonResponseData() = default;

		UniquePtr<InteractionData> interactionData{ makeUnique<InteractionData>() };///< Interaction data.
		std::string emojiName{};///< The emoji name, if applicable.
		std::string buttonId{};///< The id of the button, for identification.
		Snowflake channelId{};///< The ChannelData id where it took place.
		Snowflake messageId{};///< The Message id where it took place.
		Snowflake userId{};///< The UserData id who selected the menu options.
	};

	/// @brief ButtonCollector, for collecting button input from one or more Users.
	class DiscordCoreAPI_Dll ButtonCollector {
	  public:
		friend class DiscordCoreClient;

		static UnorderedMap<std::string, UnboundedMessageBlock<InteractionData>*> buttonInteractionBuffersMap;
		static DiscordCoreInternal::TriggerEvent<void, InteractionData> buttonInteractionEventsMap;

		/// @brief Constructor.
		/// @param dataPackage An InputEventData structure, from the response that came from the submitted button.
		ButtonCollector(InputEventData& dataPackage);

		/// @brief Used to collect the button inputs from one or more users.
		/// @param getButtonDataForAllNew Whether or not to collect input from a single target UserData or all potential users.
		/// @param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// @param maxNumberOfPressesNew The maximum number of inputs to collect before stopping.
		/// @param errorMessageDataNew The message-data for when an individual other than the selected individual attemps to use this interaction.
		/// @param targetUserId The id of the single UserData to collect inputs from, if getButtonDataForAllNew is set to false.
		/// @returns A vector of ButtonResponseData.
		CoRoutine<jsonifier::vector<ButtonResponseData>, false> collectButtonData(bool getButtonDataForAllNew, uint32_t maxWaitTimeInMsNew, uint32_t maxNumberOfPressesNew,
			CreateInteractionResponseData errorMessageDataNew, Snowflake targetUserId = Snowflake{});

		/// @brief Used to collect the button inputs from one or more users.
		/// @param triggerFunctionNew A std::function<bool(InteractionData)> to decide whether or not to trigger the event's main function.
		/// @param functionNew Takes a DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> as a function to be executed upon returning true from the "trigger-function".
		void collectButtonData(std::function<bool(InteractionData)> triggerFunctionNew, DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew);

		~ButtonCollector();

	  protected:
		UniquePtr<InteractionData> interactionData{ makeUnique<InteractionData>() };
		UnboundedMessageBlock<InteractionData> buttonIncomingInteractionBuffer{};
		jsonifier::vector<ButtonResponseData> responseVector{};
		CreateInteractionResponseData errorMessageData{};
		jsonifier::vector<std::string> values{};
		uint32_t currentCollectedButtonCount{};
		uint32_t maxCollectedButtonCount{};
		std::string buffersMapKey{};
		bool getButtonDataForAll{};
		uint32_t maxTimeInMs{};
		std::string buttonId{};
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake userId{};
		bool doWeQuit{};

		void run();
	};

	/// @brief Button response data.
	struct ModalResponseData {
		inline operator InteractionData() {
			return *interactionData;
		}

		inline ModalResponseData& operator=(const ModalResponseData& other) {
			if (this != &other) {
				*interactionData = *other.interactionData;
				customIdSmall	 = other.customIdSmall;
				channelId		 = other.channelId;
				customId		 = other.customId;
				userId			 = other.userId;
				value			 = other.value;
			}
			return *this;
		}

		inline ModalResponseData(const ModalResponseData& other) {
			*this = other;
		}

		inline ModalResponseData() = default;

		UniquePtr<InteractionData> interactionData{ makeUnique<InteractionData>() };///< Interaction data.
		std::string customIdSmall{};///< The customId of the particular input.
		std::string customId{};///< The customId of the modal component.
		Snowflake channelId{};///< The ChannelData id where it took place.
		std::string value{};/// The input value of the modal component.
		Snowflake userId{};///< The UserData id who selected the menu options.
	};

	/// @brief ModalCollector, for collecting modal text input from one or more Users.
	class DiscordCoreAPI_Dll ModalCollector {
	  public:
		friend class DiscordCoreClient;

		static UnorderedMap<std::string, UnboundedMessageBlock<InteractionData>*> modalInteractionBuffersMap;
		static DiscordCoreInternal::TriggerEvent<void, InteractionData> modalInteractionEventsMap;

		/// @brief Constructor.
		/// @param dataPackage An InputEventData structure, from the response that came from the submitted button.
		ModalCollector(InputEventData& dataPackage);

		/// @brief Used to collect the button inputs from one or more users.
		/// @param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// @returns A vector of ButtonResponseData.
		CoRoutine<ModalResponseData, false> collectModalData(uint32_t maxWaitTimeInMsNew);

		/// @brief Used to collect the modal from one or more users.
		/// @param triggerFunctionNew A std::function<bool(InteractionData)> to decide whether or not to trigger the event's main function.
		/// @param functionNew Takes a DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> as a function to be executed upon returning true from the "trigger-function".
		void collectModalData(std::function<bool(InteractionData)> triggerFunctionNew, DiscordCoreInternal::TriggerEventDelegate<void, InteractionData> functionNew);

		~ModalCollector();

	  protected:
		UnboundedMessageBlock<InteractionData> modalIncomingInteractionBuffer{};
		CreateInteractionResponseData errorMessageData{};
		uint32_t currentCollectedButtonCount{};
		ModalResponseData responseData{};
		uint32_t maxTimeInMs{};
		Snowflake channelId{};
		bool doWeQuit{};

		void run();
	};

	/**@}*/
};
