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
/// InteractionEntities.hpp - Header for the interaction_data related classes and
/// structs. may 28, 2021 Chris M.
/// https://discordcoreapi.com
/// \file InteractionEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>
#include <discordcoreapi/WebHookEntities.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	using auto_complete_entry_function = std::function<jsonifier::string(jsonifier::string)>;

	class DiscordCoreAPI_Dll interaction_response_base {
	  public:
		template<typename value_type> friend struct jsonifier::core;

		unordered_set<jsonifier::string> jsonifierExcludedKeys{};

		/// @brief Adds a button to the response message_data.
		/// @param disabled whether the button is active or not.
		/// @param customIdNew a custom id to give for identifying the button.
		/// @param buttonLabel a visible label for the button.
		/// @param buttonStyle the style of the button.
		/// @param emojiName an emoji name, if desired.
		/// @param emojiId an emoji id, if desired.
		/// @param url a url, if applicable.
		interaction_response_base& addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, button_style buttonStyle,
			jsonifier::string_view emojiName = "", snowflake emojiId = snowflake{}, jsonifier::string_view url = "");

		/// @brief Adds a select-menu to the response message_data.
		/// @param disabled whether the select-menu is active or not.
		/// @param customIdNew a custom id to give for identifying the select-menu.
		/// @param options a vector of select-menu-options to offer.
		/// @param placeholder custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues maximum number of selections that are possible.
		/// @param minValues minimum required number of selections that are required.
		interaction_response_base& addSelectMenu(bool disabled, jsonifier::string_view customIdNew, jsonifier::vector<select_option_data> options,
			jsonifier::string_view placeholder, uint64_t maxValues, uint64_t minValues, select_menu_type type,
			jsonifier::vector<channel_type> channelTypes = jsonifier::vector<channel_type>{});

		/// @brief Adds a modal to the response message_data.
		/// @param topTitleNew a title for the modal.
		/// @param topCustomIdNew a custom id to give for the modal.
		/// @param titleNew a title for the modal's individual input.
		/// @param customIdNew a custom id to give for the modal's individual input.
		/// @param required is it a required response?
		/// @param minLength minimum length.
		/// @param maxLength maximum length.
		/// @param inputStyle the input style.
		/// @param label a label for the modal.
		/// @param placeholder a placeholder for the modal.
		/// @return respond_to_input_event_data& a reference to this data structure.
		interaction_response_base& addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
			jsonifier::string_view customIdNew, bool required, uint64_t minLength, uint64_t maxLength, text_input_style inputStyle, jsonifier::string_view label = "",
			jsonifier::string_view placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile the file to be added.
		/// @return message_response_base& a reference to this data structure.
		interaction_response_base& addFile(const file& theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage an allowed_mentions_data structure.
		interaction_response_base& addAllowedMentions(const allowed_mentions_data dataPackage);

		/// @brief For setting the components in a response.
		/// @param dataPackage an action_row_data structure.
		interaction_response_base& addComponentRow(const action_row_data dataPackage);

		/// @brief Sets the response type of the current message_data.
		/// @param type interaction callback type.
		interaction_response_base& setResponseType(interaction_callback_type type);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage an embed_data structure.send_dmdata
		interaction_response_base& addMessageEmbed(const embed_data dataPackage);

		/// @brief For setting the content in a response.
		/// @param dataPackage a string, containing the content.
		interaction_response_base& addContent(jsonifier::string_view dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs a bool.
		interaction_response_base& setTTSStatus(bool enabledTTs);

		interaction_response_base& setFlags(uint64_t flag);

		interaction_response_data getInteractionResponseData();

		void generateExcludedKeys();

		virtual ~interaction_response_base() = default;

	  protected:
		interaction_package_data interactionPackage{};
		message_package_data messagePackage{};
		interaction_callback_data data{};
		interaction_callback_type type{};
	};

	/// @brief For creating an ephemeral interaction response.
	class DiscordCoreAPI_Dll create_ephemeral_interaction_response_data : public interaction_response_base {
	  public:
		friend class create_interaction_response_data;
		friend class interactions;
		friend class input_events;

		create_ephemeral_interaction_response_data(const respond_to_input_event_data dataPackage);

		virtual ~create_ephemeral_interaction_response_data() = default;
	};

	/// @brief For creating a deferred interaction response.
	class DiscordCoreAPI_Dll create_deferred_interaction_response_data : public interaction_response_base {
	  public:
		friend class create_interaction_response_data;
		friend class interactions;
		friend class input_events;

		create_deferred_interaction_response_data(const respond_to_input_event_data dataPackage);

		virtual ~create_deferred_interaction_response_data() = default;
	};

	/// @brief For creating an interaction response.
	class DiscordCoreAPI_Dll create_interaction_response_data : public interaction_response_base {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class select_menu_collector;
		friend class button_collector;
		friend class modal_collector;
		friend class interactions;
		friend class input_events;

		create_interaction_response_data(const create_ephemeral_interaction_response_data dataPackage);

		create_interaction_response_data(const create_deferred_interaction_response_data dataPackage);

		create_interaction_response_data(const respond_to_input_event_data dataPackage);

		create_interaction_response_data(const interaction_data dataPackage);

		create_interaction_response_data() = default;

		virtual ~create_interaction_response_data() = default;
	};

	/// @brief For getting an interaction response.
	struct get_interaction_response_data {
		jsonifier::string interactionToken{};///< Interaction token.
		snowflake applicationId{};///< Application id.
	};

	/// @brief For editing an interaction response.
	class DiscordCoreAPI_Dll edit_interaction_response_data : public edit_web_hook_data {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class interactions;
		friend class input_events;

		edit_interaction_response_data(const respond_to_input_event_data dataPackage);

		virtual ~edit_interaction_response_data() = default;

	  protected:
		interaction_package_data interactionPackage{};
	};

	/// @brief For deleting an interaction response.
	struct DiscordCoreAPI_Dll delete_interaction_response_data {
		friend class interactions;
		friend class input_events;

		delete_interaction_response_data(const respond_to_input_event_data dataPackage);

	  protected:
		interaction_package_data interactionPackage{};
		uint32_t timeDelay{};
	};

	/// @brief For creating an ephemeral follow up message_data.
	class DiscordCoreAPI_Dll create_ephemeral_follow_up_message_data : public execute_web_hook_data {
	  public:
		friend class create_follow_up_message_data;
		friend class interactions;
		friend class input_events;

		create_ephemeral_follow_up_message_data(const respond_to_input_event_data dataPackage);

		virtual ~create_ephemeral_follow_up_message_data() = default;

	  protected:
		interaction_package_data interactionPackage{};
	};

	/// @brief For creating a follow up message_data.
	class DiscordCoreAPI_Dll create_follow_up_message_data : public execute_web_hook_data {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class select_menu_collector;
		friend class button_collector;
		friend class interactions;
		friend class input_events;

		create_follow_up_message_data(const create_ephemeral_follow_up_message_data dataPackage);

		create_follow_up_message_data(const respond_to_input_event_data dataPackage);

		virtual ~create_follow_up_message_data() = default;

	  protected:
		interaction_package_data interactionPackage{};
	};

	/// @brief For getting a follow-up message_data.
	struct get_follow_up_message_data {
		jsonifier::string interactionToken{};///< Interaction token.
		snowflake applicationId{};///< Application id.
		snowflake messageId{};///< Message id.
	};

	/// @brief For editing a follow up message_data.
	class DiscordCoreAPI_Dll edit_follow_up_message_data : public edit_web_hook_data {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class interactions;
		friend class input_events;

		edit_follow_up_message_data(const respond_to_input_event_data dataPackage);

		virtual ~edit_follow_up_message_data() = default;

	  protected:
		interaction_package_data interactionPackage{};
		message_package_data messagePackage{};
	};

	/// @brief For deleting a follow up message_data.
	struct DiscordCoreAPI_Dll delete_follow_up_message_data {
		friend class interactions;
		friend class input_events;

		delete_follow_up_message_data(const respond_to_input_event_data dataPackage);

	  protected:
		interaction_package_data interactionPackage{};
		message_package_data messagePackage{};
		uint32_t timeDelay{};
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	/// @brief An interface class for the interaction_data related discord endpoints.
	class DiscordCoreAPI_Dll interactions {
	  public:
		friend class discord_core_internal::base_socket_agent;
		friend class discord_core_client;
		friend class event_manager;
		friend class input_events;

		static void initialize(discord_core_internal::https_client*);

		/// @brief Creates a response to an input interaction.
		/// @param dataPackage a create_interaction_response_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> createInteractionResponseAsync(create_interaction_response_data dataPackage);

		/// @brief Collects an interaction response.
		/// @param dataPackage a get_interaction_response_data structure.
		/// @return A co_routine containing an interaction_response_data.
		static co_routine<message_data> getInteractionResponseAsync(get_interaction_response_data dataPackage);

		/// @brief Edits an interaction response.
		/// @param dataPackage a edit_interaction_response_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> editInteractionResponseAsync(edit_interaction_response_data dataPackage);

		/// @brief Deletes an interaction respnose.
		/// @param dataPackage a delete_interaction_response_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> deleteInteractionResponseAsync(delete_interaction_response_data dataPackage);

		/// @brief Creates a follow up message to an input interaction.
		/// @param dataPackage a create_follow_up_message_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> createFollowUpMessageAsync(create_follow_up_message_data dataPackage);

		/// @brief Creates a follow up message to an input interaction.
		/// @param dataPackage a create_follow_up_message_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> getFollowUpMessageAsync(get_follow_up_message_data dataPackage);

		/// @brief Edits a follow up message_data.
		/// @param dataPackage a edit_follow_up_message_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> editFollowUpMessageAsync(edit_follow_up_message_data dataPackage);

		/// @brief Deletes a follow up message_data.
		/// @param dataPackage a delete_follow_up_message_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> deleteFollowUpMessageAsync(delete_follow_up_message_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;

		static message_data createInteractionResponse(create_interaction_response_data dataPackage);

		static message_data editInteractionResponse(edit_interaction_response_data dataPackage);

		static message_data getInteractionResponse(get_interaction_response_data dataPackage);

		static message_data createFollowUpMessage(create_follow_up_message_data dataPackage);

		static message_data editFollowUpMessage(edit_follow_up_message_data dataPackage);
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// @brief Select menu response data.
	struct select_menu_response_data {
		DCA_INLINE operator interaction_data() {
			return *interactionData;
		}

		DCA_INLINE select_menu_response_data& operator=(const select_menu_response_data& other) {
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

		DCA_INLINE select_menu_response_data(const select_menu_response_data& other) {
			*this = other;
		}

		DCA_INLINE select_menu_response_data& operator=(select_menu_response_data& other) {
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

		DCA_INLINE select_menu_response_data(select_menu_response_data& other) {
			*this = other;
		}

		DCA_INLINE select_menu_response_data() = default;

		unique_ptr<interaction_data> interactionData{ makeUnique<interaction_data>() };///< Interaction data.
		jsonifier::vector<jsonifier::string> values{};///< A vector of the chosen values.
		jsonifier::string selectionId{};///< Selection id.
		snowflake channelId{};///< The channel_data id where it took place.
		snowflake messageId{};///< The message id where it took place.
		snowflake userId{};///< The user_data id who selected the menu options.
	};

	/// select_menu_collector, for collecting select-menu input from one or more
	/// @brief Select_menu_collector, for collecting select-menu input from one
	/// or more users.
	class DiscordCoreAPI_Dll select_menu_collector {
	  public:
		friend class discord_core_client;

		static unordered_map<jsonifier::string, unbounded_message_block<interaction_data>*> selectMenuInteractionBuffersMap;
		static discord_core_internal::trigger_event<void, interaction_data> selectMenuInteractionEventsMap;

		/// @brief Constructor.
		/// @param dataPackage an input_event_data structure, from the response that came from the submitted select-menu.
		select_menu_collector(input_event_data dataPackage);

		/// @brief Used to collect the select-menu inputs from one or more users.
		/// @param getSelectMenuDataForAllNew whether or not to collect select-menu input from a single target user_data or all potential users.
		/// @param maxWaitTimeInMsNew the maximum amount of time to wait for new inputs, in milliseconds.
		/// @param maxCollectedSelectMenuCountNew the maximum number of inputs to collect before stopping.
		/// @param errorMessageDataNew the message-data for when an individual other than the selected individual attemps to use this interaction.
		/// @param targetUserId the id of the single user_data to collect inputs from, if getSelectMenuDataForAllNew is set to false.
		/// @return A vector of select_menu_response_data.
		co_routine<jsonifier::vector<select_menu_response_data>, false> collectSelectMenuData(bool getSelectMenuDataForAllNew, uint32_t maxWaitTimeInMsNew,
			uint32_t maxCollectedSelectMenuCountNew, create_interaction_response_data errorMessageDataNew, snowflake targetUserId = snowflake{});

		/// @brief Used to collect the select-menu inputs from one or more users.
		/// @param triggerFunctionNew a std::function<bool(interaction_data)> to decide whether or not to trigger the event's main function.
		/// @param functionNew takes a discord_core_internal::trigger_event_delegate<void, interaction_data> as a function to be executed upon returning true from the "Trigger-function".
		void collectSelectMenuData(std::function<bool(interaction_data)> triggerFunctionNew, discord_core_internal::trigger_event_delegate<void, interaction_data> functionNew);

		~select_menu_collector();

	  protected:
		unbounded_message_block<interaction_data> selectMenuIncomingInteractionBuffer{};
		unique_ptr<interaction_data> interactionData{ makeUnique<interaction_data>() };
		jsonifier::vector<select_menu_response_data> responseVector{};
		create_interaction_response_data errorMessageData{};
		jsonifier::vector<jsonifier::string> values{};
		uint32_t currentCollectedSelectMenuCount{};
		uint32_t maxCollectedSelectMenuCount{};
		jsonifier::string buffersMapKey{};
		jsonifier::string selectMenuId{};
		bool getSelectMenuDataForAll{};
		uint32_t maxTimeInMs{};
		snowflake channelId{};
		snowflake messageId{};
		snowflake userId{};
		bool doWeQuit{};

		void run();
	};

	/// @brief Button response data.
	struct button_response_data {
		DCA_INLINE operator interaction_data() {
			return *interactionData;
		}

		DCA_INLINE button_response_data& operator=(const button_response_data& other) {
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

		DCA_INLINE button_response_data(const button_response_data& other) {
			*this = other;
		}

		DCA_INLINE button_response_data& operator=(button_response_data& other) {
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

		DCA_INLINE button_response_data(button_response_data& other) {
			*this = other;
		}

		DCA_INLINE button_response_data() = default;

		unique_ptr<interaction_data> interactionData{ makeUnique<interaction_data>() };///< Interaction data.
		jsonifier::string emojiName{};///< The emoji name, if applicable.
		jsonifier::string buttonId{};///< The id of the button, for identification.
		snowflake channelId{};///< The channel_data id where it took place.
		snowflake messageId{};///< The message id where it took place.
		snowflake userId{};///< The user_data id who selected the menu options.
	};

	/// @brief Button_collector, for collecting button input from one or more users.
	class DiscordCoreAPI_Dll button_collector {
	  public:
		friend class discord_core_client;

		static unordered_map<jsonifier::string, unbounded_message_block<interaction_data>*> buttonInteractionBuffersMap;
		static discord_core_internal::trigger_event<void, interaction_data> buttonInteractionEventsMap;

		/// @brief Constructor.
		/// @param dataPackage an input_event_data structure, from the response that came from the submitted button.
		button_collector(input_event_data dataPackage);

		/// @brief Used to collect the button inputs from one or more users.
		/// @param getButtonDataForAllNew whether or not to collect input from a single target user_data or all potential users.
		/// @param maxWaitTimeInMsNew the maximum amount of time to wait for new inputs, in milliseconds.
		/// @param maxNumberOfPressesNew the maximum number of inputs to collect before stopping.
		/// @param errorMessageDataNew the message-data for when an individual other than the selected individual attemps to use this interaction.
		/// @param targetUserId the id of the single user_data to collect inputs from, if getButtonDataForAllNew is set to false.
		/// @return A vector of button_response_data.
		co_routine<jsonifier::vector<button_response_data>, false> collectButtonData(bool getButtonDataForAllNew, uint32_t maxWaitTimeInMsNew, uint32_t maxNumberOfPressesNew,
			create_interaction_response_data errorMessageDataNew, snowflake targetUserId = snowflake{});

		/// @brief Used to collect the button inputs from one or more users.
		/// @param triggerFunctionNew a std::function<bool(interaction_data)> to decide whether or not to trigger the event's main function.
		/// @param functionNew takes a discord_core_internal::trigger_event_delegate<void, interaction_data> as a function to be executed upon returning true from the "Trigger-function".
		void collectButtonData(std::function<bool(interaction_data)> triggerFunctionNew, discord_core_internal::trigger_event_delegate<void, interaction_data> functionNew);

		~button_collector();

	  protected:
		unique_ptr<interaction_data> interactionData{ makeUnique<interaction_data>() };
		unbounded_message_block<interaction_data> buttonIncomingInteractionBuffer{};
		jsonifier::vector<button_response_data> responseVector{};
		create_interaction_response_data errorMessageData{};
		jsonifier::vector<jsonifier::string> values{};
		uint32_t currentCollectedButtonCount{};
		uint32_t maxCollectedButtonCount{};
		jsonifier::string buffersMapKey{};
		bool getButtonDataForAll{};
		uint32_t maxTimeInMs{};
		jsonifier::string buttonId{};
		snowflake channelId{};
		snowflake messageId{};
		snowflake userId{};
		bool doWeQuit{};

		void run();
	};

	/// @brief Button response data.
	struct modal_response_data {
		DCA_INLINE operator interaction_data() {
			return *interactionData;
		}

		DCA_INLINE modal_response_data& operator=(const modal_response_data& other) {
			if (this != &other) {
				*interactionData = *other.interactionData;
				customIdSmall	 = other.customIdSmall;
				channelId		 = other.channelId;
				customId		 = other.customId;
				userId			 = other.userId;
				values			 = other.values;
			}
			return *this;
		}

		DCA_INLINE modal_response_data(const modal_response_data& other) {
			*this = other;
		}

		DCA_INLINE modal_response_data() = default;

		unique_ptr<interaction_data> interactionData{ makeUnique<interaction_data>() };///< Interaction data.
		jsonifier::vector<jsonifier::string> values{};/// the input values of the modal component.
		jsonifier::string customIdSmall{};///< The customId of the particular input.
		jsonifier::string customId{};///< The customId of the modal component.
		snowflake channelId{};///< The channel_data id where it took place.
		snowflake userId{};///< The user_data id who selected the menu options.
	};

	/// @brief Modal_collector, for collecting modal text input from one or more users.
	class DiscordCoreAPI_Dll modal_collector {
	  public:
		friend class discord_core_client;

		static unordered_map<jsonifier::string, unbounded_message_block<interaction_data>*> modalInteractionBuffersMap;
		static discord_core_internal::trigger_event<void, interaction_data> modalInteractionEventsMap;

		/// @brief Constructor.
		/// @param dataPackage an input_event_data structure, from the response that came from the submitted button.
		modal_collector(input_event_data dataPackage);

		/// @brief Used to collect the button inputs from one or more users.
		/// @param maxWaitTimeInMsNew the maximum amount of time to wait for new inputs, in milliseconds.
		/// @return A vector of button_response_data.
		co_routine<modal_response_data, false> collectModalData(uint32_t maxWaitTimeInMsNew);

		/// @brief Used to collect the modal from one or more users.
		/// @param triggerFunctionNew a std::function<bool(interaction_data)> to decide whether or not to trigger the event's main function.
		/// @param functionNew takes a discord_core_internal::trigger_event_delegate<void, interaction_data> as a function to be executed upon returning true from the "Trigger-function".
		void collectModalData(std::function<bool(interaction_data)> triggerFunctionNew, discord_core_internal::trigger_event_delegate<void, interaction_data> functionNew);

		~modal_collector();

	  protected:
		unbounded_message_block<interaction_data> modalIncomingInteractionBuffer{};
		create_interaction_response_data errorMessageData{};
		uint32_t currentCollectedButtonCount{};
		modal_response_data responseData{};
		uint32_t maxTimeInMs{};
		snowflake channelId{};
		bool doWeQuit{};

		void run();
	};

	/**@}*/
};
