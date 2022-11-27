/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
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
		/// \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		InteractionResponseBase& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
			const std::string& emojiName = "", Snowflake emojiId = Snowflake{ 0 }, const std::string& url = "");

		/// \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		InteractionResponseBase& addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
			const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type,
			std::vector<ChannelType> channelTypes = std::vector<ChannelType>{});

		/// \brief Adds a modal to the response Message.
		/// \param topTitleNew A title for the modal.
		/// \param topCustomIdNew A custom id to give for the modal.
		/// \param titleNew A title for the modal's individual input.
		/// \param customIdNew A custom id to give for the modal's individual input.
		/// \param required Is it a required response?
		/// \param minLength Minimum length.
		/// \param maxLength Maximum length.
		/// \param inputStyle The input style.
		/// \param label A label for the modal.
		/// \param placeholder A placeholder for the modal.
		/// \returns RespondToInputEventData& A reference to this data structure.
		InteractionResponseBase& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
			const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle,
			const std::string& label = "", const std::string& placeholder = "");

		/// \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns MessageResponseBase& A reference to this data structure.
		InteractionResponseBase& addFile(const File& theFile);

		/// \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		InteractionResponseBase& addAllowedMentions(const AllowedMentionsData& dataPackage);

		/// \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		InteractionResponseBase& addComponentRow(const ActionRowData& dataPackage);

		/// \brief Sets the response type of the current Message.
		/// \param type Interaction callback type.
		InteractionResponseBase& setResponseType(InteractionCallbackType type);

		/// \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.SendDMData
		InteractionResponseBase& addMessageEmbed(const EmbedData& dataPackage);

		/// \brief For setting the content in a response.
		/// \param dataPackage A string, containing the content.
		InteractionResponseBase& addContent(const std::string& dataPackage);

		/// \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		InteractionResponseBase& setTTSStatus(bool enabledTTs);

		InteractionResponseBase& setFlags(int64_t flag);

		InteractionResponseData getInteractionResponseData();

		virtual ~InteractionResponseBase() noexcept = default;

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		InteractionResponseData data{};
	};

	/// \brief For creating an ephemeral Interaction response.
	class DiscordCoreAPI_Dll CreateEphemeralInteractionResponseData : public InteractionResponseBase {
	  public:
		friend class CreateInteractionResponseData;
		friend class Interactions;
		friend class InputEvents;

		CreateEphemeralInteractionResponseData(const RespondToInputEventData& dataPackage);

		virtual ~CreateEphemeralInteractionResponseData() noexcept = default;
	};

	/// \brief For creating a deferred Interaction response.
	class DiscordCoreAPI_Dll CreateDeferredInteractionResponseData {
	  public:
		friend class CreateInteractionResponseData;
		friend class Interactions;
		friend class InputEvents;

		CreateDeferredInteractionResponseData(const RespondToInputEventData& dataPackage);

		virtual ~CreateDeferredInteractionResponseData() noexcept = default;

	  protected:
		InteractionPackageData interactionPackage{};
		InteractionResponseData data{};
	};

	/// \brief For creating an Interaction response.
	class DiscordCoreAPI_Dll CreateInteractionResponseData : public InteractionResponseBase {
	  public:
		friend class SelectMenuCollector;
		friend class ButtonCollector;
		friend class ModalCollector;
		friend class Interactions;
		friend class InputEvents;

		CreateInteractionResponseData(const CreateEphemeralInteractionResponseData& dataPackage);

		CreateInteractionResponseData(const CreateDeferredInteractionResponseData& dataPackage);

		CreateInteractionResponseData(const RespondToInputEventData& dataPackage);

		CreateInteractionResponseData(const InteractionData& dataPackage);

		CreateInteractionResponseData() noexcept = default;

		virtual ~CreateInteractionResponseData() noexcept = default;
	};

	/// \brief For getting an Interaction response.
	struct DiscordCoreAPI_Dll GetInteractionResponseData {
		std::string interactionToken{};///< Interaction token.
		Snowflake applicationId{};///< application id.
	};

	/// \brief For editing an Interaction response.
	class DiscordCoreAPI_Dll EditInteractionResponseData {
	  public:
		friend class Interactions;
		friend class InputEvents;

		EditInteractionResponseData(const RespondToInputEventData& dataPackage);

		virtual ~EditInteractionResponseData() noexcept = default;

	  protected:
		InteractionPackageData interactionPackage{};
		EditWebHookData data{};
	};

	/// \brief For deleting an Interaction response.
	struct DiscordCoreAPI_Dll DeleteInteractionResponseData {
		friend class Interactions;
		friend class InputEvents;

		DeleteInteractionResponseData(const RespondToInputEventData& dataPackage);

	  protected:
		InteractionPackageData interactionPackage{};
		uint32_t timeDelay{ 0 };
	};

	/// \brief For creating an ephemeral follow up Message.
	class DiscordCoreAPI_Dll CreateEphemeralFollowUpMessageData : public ExecuteWebHookData {
	  public:
		friend class CreateFollowUpMessageData;
		friend class Interactions;
		friend class InputEvents;

		CreateEphemeralFollowUpMessageData(const RespondToInputEventData& dataPackage);

		virtual ~CreateEphemeralFollowUpMessageData() noexcept = default;

	  protected:
		InteractionPackageData interactionPackage{};
	};

	/// \brief For creating a follow up Message.
	class DiscordCoreAPI_Dll CreateFollowUpMessageData : public ExecuteWebHookData {
	  public:
		friend class SelectMenuCollector;
		friend class ButtonCollector;
		friend class Interactions;
		friend class InputEvents;

		CreateFollowUpMessageData(const CreateEphemeralFollowUpMessageData& dataPackage);

		CreateFollowUpMessageData(const RespondToInputEventData& dataPackage);

		virtual ~CreateFollowUpMessageData() noexcept = default;

	  protected:
		InteractionPackageData interactionPackage{};
	};

	/// \brief For getting a follow-up Message.
	struct DiscordCoreAPI_Dll GetFollowUpMessageData {
		std::string interactionToken{};///< Interaction token.
		Snowflake applicationId{};///< application id.
		Snowflake messageId{};///< Message id.
	};

	/// \brief For editing a follow up Message.
	class DiscordCoreAPI_Dll EditFollowUpMessageData {
	  public:
		friend class Interactions;
		friend class InputEvents;

		EditFollowUpMessageData(const RespondToInputEventData& dataPackage);

		virtual ~EditFollowUpMessageData() noexcept = default;

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		EditWebHookData data{};
	};

	/// \brief For deleting a follow up Message.
	struct DiscordCoreAPI_Dll DeleteFollowUpMessageData {
		friend class Interactions;
		friend class InputEvents;

		DeleteFollowUpMessageData(const RespondToInputEventData& dataPackage);

	  protected:
		InteractionPackageData interactionPackage{};
		MessagePackageData messagePackage{};
		uint32_t timeDelay{ 0 };
	};

	/// \brief A single Interaction.
	struct DiscordCoreAPI_Dll Interaction : public InteractionData {
	  public:
		Interaction() noexcept = default;

		virtual ~Interaction() noexcept = default;
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// \brief An interface class for the Interaction related Discord endpoints.
	class DiscordCoreAPI_Dll Interactions {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class DiscordCoreClient;
		friend class EventManager;

		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Creates a response to an input Interaction.
		/// \param dataPackage A CreateInteractionResponseData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> createInteractionResponseAsync(CreateInteractionResponseData dataPackage);

		/// \brief Collects an Interaction response.
		/// \param dataPackage A GetInteractionResponseData structure.
		/// \returns A CoRoutine containing an InteractionResponseData.
		static CoRoutine<Message> getInteractionResponseAsync(GetInteractionResponseData dataPackage);

		/// \brief Edits an Interaction response.
		/// \param dataPackage A EditInteractionResponseData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> editInteractionResponseAsync(EditInteractionResponseData dataPackage);

		/// \brief Deletes an Interaction respnose.
		/// \param dataPackage A DeleteInteractionResponseData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage);

		/// \brief Creates a follow up Message to an input Interaction.
		/// \param dataPackage A CreateFollowUpMessageData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage);

		/// \brief Creates a follow up Message to an input Interaction.
		/// \param dataPackage A CreateFollowUpMessageData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> getFollowUpMessageAsync(GetFollowUpMessageData dataPackage);

		/// \brief Edits a follow up Message.
		/// \param dataPackage A EditFollowUpMessageData structure.
		/// \returns A CoRoutine containing a MessageData.
		static CoRoutine<Message> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage);

		/// \brief Deletes a follow up Message.
		/// \param dataPackage A DeleteFollowUpMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// \brief Select menu response data.
	struct DiscordCoreAPI_Dll SelectMenuResponseData {
		operator InteractionData() {
			return *this->interactionData;
		}

		SelectMenuResponseData& operator=(const SelectMenuResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->selectionId = other.selectionId;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->values = other.values;
				this->userId = other.userId;
			}
			return *this;
		}

		SelectMenuResponseData(const SelectMenuResponseData& other) {
			*this = other;
		}

		SelectMenuResponseData& operator=(SelectMenuResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->selectionId = other.selectionId;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->values = other.values;
				this->userId = other.userId;
			}
			return *this;
		}

		SelectMenuResponseData(SelectMenuResponseData& other) {
			*this = other;
		}

		SelectMenuResponseData() noexcept = default;

		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };///< Interaction data.
		std::vector<std::string> values{};///< A vector of the chosen values.
		std::string selectionId{};///< Selection id.
		Snowflake channelId{};///< The Channel id where it took place.
		Snowflake messageId{};///< The Message id where it took place.
		Snowflake userId{};///< The User id who selected the menu options.
	};

	/// SelectMenuCollector, for collecting select-menu input from one or more
	/// \brief SelectMenuCollector, for collecting select-menu input from one
	/// or more Users.
	class DiscordCoreAPI_Dll SelectMenuCollector {
	  public:
		friend class DiscordCoreClient;

		static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> selectMenuInteractionBuffersMap;

		/// \brief Constructor.
		/// \param dataPackage An InputEventData structure, from the response that came from the submitted select-menu.
		SelectMenuCollector(InputEventData& dataPackage);

		/// \brief Used to collect the select-menu inputs from one or more users.
		/// \param getSelectMenuDataForAllNew Whether or not to collect select-menu input from a single target User or all potential users.
		/// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// \param maxCollectedSelectMenuCountNew The maximum number of inputs to collect before stopping.
		/// \param errorMessageDataNew The message-data for when an individual other than the selected individual attemps to use this interaction.
		/// \param targetUserId The id of the single User to collect inputs from, if getSelectMenuDataForAllNew is set to false.
		/// \returns A vector of SelectMenuResponseData.
		CoRoutine<std::vector<SelectMenuResponseData>> collectSelectMenuData(bool getSelectMenuDataForAllNew, int32_t maxWaitTimeInMsNew,
			int32_t maxCollectedSelectMenuCountNew, CreateInteractionResponseData errorMessageDataNew, Snowflake targetUserId = Snowflake{ 0 });

		~SelectMenuCollector();

	  protected:
		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
		UnboundedMessageBlock<InteractionData> selectMenuIncomingInteractionBuffer{};
		std::vector<SelectMenuResponseData> responseVector{};
		CreateInteractionResponseData errorMessageData{};
		int32_t currentCollectedSelectMenuCount{ 0 };
		int32_t maxCollectedSelectMenuCount{ 0 };
		bool getSelectMenuDataForAll{ false };
		std::vector<std::string> values{};
		std::string buffersMapKey{};
		std::string selectMenuId{};
		uint32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake userId{};

		void run();
	};

	/// \brief Button response data.
	struct DiscordCoreAPI_Dll ButtonResponseData {
		operator InteractionData() {
			return *this->interactionData;
		}

		ButtonResponseData& operator=(const ButtonResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->emojiName = other.emojiName;
				this->buttonId = other.buttonId;
				this->userId = other.userId;
			}
			return *this;
		}

		ButtonResponseData(const ButtonResponseData& other) {
			*this = other;
		}

		ButtonResponseData& operator=(ButtonResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->messageId = other.messageId;
				this->channelId = other.channelId;
				this->emojiName = other.emojiName;
				this->buttonId = other.buttonId;
				this->userId = other.userId;
			}
			return *this;
		}

		ButtonResponseData(ButtonResponseData& other) {
			*this = other;
		}

		ButtonResponseData() noexcept = default;

		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };///< Interaction data.
		std::string emojiName{};///< The emoji name, if applicable.
		std::string buttonId{};///< The id of the button, for identification.
		Snowflake channelId{};///< The Channel id where it took place.
		Snowflake messageId{};///< The Message id where it took place.
		Snowflake userId{};///< The User id who selected the menu options.
	};

	/// \brief ButtonCollector, for collecting button input from one or more Users.
	class DiscordCoreAPI_Dll ButtonCollector {
	  public:
		friend class DiscordCoreClient;

		static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> buttonInteractionBuffersMap;

		/// \brief Constructor.
		/// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
		ButtonCollector(InputEventData& dataPackage);

		/// \brief Used to collect the button inputs from one or more users.
		/// \param getButtonDataForAllNew Whether or not to collect input from a single target User or all potential users.
		/// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// \param maxNumberOfPressesNew The maximum number of inputs to collect before stopping.
		/// \param errorMessageDataNew The message-data for when an individual other than the selected individual attemps to use this interaction.
		/// \param targetUserId The id of the single User to collect inputs from, if getButtonDataForAllNew is set to false.
		/// \returns A vector of ButtonResponseData.
		CoRoutine<std::vector<ButtonResponseData>> collectButtonData(bool getButtonDataForAllNew, int32_t maxWaitTimeInMsNew,
			int32_t maxNumberOfPressesNew, CreateInteractionResponseData errorMessageDataNew, Snowflake targetUserId = Snowflake{ 0 });

		~ButtonCollector();

	  protected:
		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
		UnboundedMessageBlock<InteractionData> buttonIncomingInteractionBuffer{};
		CreateInteractionResponseData errorMessageData{};
		std::vector<ButtonResponseData> responseVector{};
		int32_t currentCollectedButtonCount{ 0 };
		int32_t maxCollectedButtonCount{ 0 };
		bool getButtonDataForAll{ false };
		std::vector<std::string> values{};
		std::string buffersMapKey{};
		uint32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		std::string buttonId{};
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake userId{};

		void run();
	};

	/// \brief Button response data.
	struct DiscordCoreAPI_Dll ModalResponseData {
		operator InteractionData() {
			return *this->interactionData;
		}

		ModalResponseData& operator=(const ModalResponseData& other) {
			if (this != &other) {
				*this->interactionData = *other.interactionData;
				this->customIdSmall = other.customIdSmall;
				this->channelId = other.channelId;
				this->customId = other.customId;
				this->userId = other.userId;
				this->value = other.value;
			}
			return *this;
		}

		ModalResponseData(const ModalResponseData& other) {
			*this = other;
		}

		ModalResponseData() noexcept = default;

		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };///< Interaction data.
		std::string customIdSmall{};///< The customId of the particular input.
		std::string customId{};///< The customId of the modal component.
		Snowflake channelId{};///< The Channel id where it took place.
		std::string value{};/// The input value of the modal component.
		Snowflake userId{};///< The User id who selected the menu options.
	};

	/// \brief ModalCollector, for collecting modal text input from one or more Users.
	class DiscordCoreAPI_Dll ModalCollector {
	  public:
		friend class DiscordCoreClient;

		static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> modalInteractionBuffersMap;

		/// \brief Constructor.
		/// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
		ModalCollector(InputEventData& dataPackage);

		/// \brief Used to collect the button inputs from one or more users.
		/// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// \returns A vector of ButtonResponseData.
		CoRoutine<ModalResponseData> collectModalData(int32_t maxWaitTimeInMsNew);

		~ModalCollector();

	  protected:
		UnboundedMessageBlock<InteractionData> modalIncomingInteractionBuffer{};
		CreateInteractionResponseData errorMessageData{};
		int32_t currentCollectedButtonCount{ 0 };
		ModalResponseData responseData{};
		uint32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		Snowflake channelId{};

		void run();
	};

	/**@}*/
};// namespace DiscordCoreAPI
		std::string value{};/// The input value of the modal component.
		Snowflake userId{};///< The User id who selected the menu options.
	};

	/// \brief ModalCollector, for collecting modal text input from one or more Users.
	class DiscordCoreAPI_Dll ModalCollector {
	  public:
		friend class DiscordCoreClient;

		static std::unordered_map<std::string, UnboundedMessageBlock<InteractionData>*> modalInteractionBuffersMap;

		/// \brief Constructor.
		/// \param dataPackage An InputEventData structure, from the response that came from the submitted button.
		ModalCollector(InputEventData& dataPackage);

		/// \brief Used to collect the button inputs from one or more users.
		/// \param maxWaitTimeInMsNew The maximum amount of time to wait for new inputs, in milliseconds.
		/// \returns A vector of ButtonResponseData.
		CoRoutine<ModalResponseData> collectModalData(int32_t maxWaitTimeInMsNew);

		~ModalCollector();

	  protected:
		UnboundedMessageBlock<InteractionData> modalIncomingInteractionBuffer{};
		CreateInteractionResponseData errorMessageData{};
		int32_t currentCollectedButtonCount{ 0 };
		ModalResponseData responseData{};
		uint32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		Snowflake channelId{};

		void run();
	};

	/**@}*/
};// namespace DiscordCoreAPI
t32_t maxTimeInMs{ 0 };
		bool doWeQuit{ false };
		Snowflake channelId{};

		void run();
	};

	/**@}*/
};// namespace DiscordCoreAPI
