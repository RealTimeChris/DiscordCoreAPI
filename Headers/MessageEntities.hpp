// MessageEntities.hpp - Header for the Message related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "JSONIFier.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// Get Messages data. \brief Get Messages data.
	struct DiscordCoreAPI_Dll GetMessagesData {
		string aroundThisId{ "" };///<Around this id.
		string beforeThisId{ "" };///< Before this id.
		string afterThisId{ "" };///< After this id.
		string channelId{ "" };///< Channel from which to collect the Messages.
		 int32_t limit{ 0 };///< Limit of Messages to collect.		
	};

	/// Get Message data. \brief Get Message data.
	struct DiscordCoreAPI_Dll GetMessageData {
		string requesterId{ "" };///< The requester id.
		string channelId{ "" };///< The Channel from which to collect the Message.
		string id{ "" };///< The id of the Message to collect.
	};

	/// Create Message data. \brief Create Message data.
	struct DiscordCoreAPI_Dll CreateMessageData {

		friend string DiscordCoreInternal::JSONIFY(CreateMessageData dataPackage);
		friend class InputEvents;
		friend class Messages;

		CreateMessageData(string channelIdNew) {
			this->channelId = channelIdNew;
		}

		CreateMessageData(RespondToInputEventData dataPackage) {
			this->channelId = dataPackage.channelId;
			this->addAllowedMentions(dataPackage.allowedMentions);
			this->requesterId = dataPackage.requesterId;
			for (auto& value : dataPackage.components) {
				this->components.push_back(value);
			}
			this->addContent(dataPackage.content);
			for (auto& value : dataPackage.embeds) {
				this->embeds.push_back(value);
			}
			this->tts = dataPackage.tts;
		}

		CreateMessageData(InputEventData dataPackage) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
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
		void addButton(bool disabled, string customId, string buttonLabel, ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
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
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int32_t maxValues, int32_t minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns void
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		/// \returns void
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns void
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns void
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns void
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:

		MessageReferenceData messageReference{};
		AllowedMentionsData allowedMentions{};
		vector<ActionRowData> components{};
		vector<string> stickerIds{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string content{ "" };
		bool tts{ false };

		CreateMessageData() {};
	};

	/// Crosspost Message Data. \brief Crosspost Message Data.
	struct DiscordCoreAPI_Dll CrosspostMessageData {
		string messageId{ "" };///< Id of the message to be crossposted.
		string channelId{ "" };///< Channel within which to crosspost the Message from.
	};

	/// Edit Message data. \brief Edit Message data.
	struct DiscordCoreAPI_Dll EditMessageData {

		friend string DiscordCoreInternal::JSONIFY(EditMessageData dataPackage);
		friend class InputEvents;
		friend class Messages;

		EditMessageData(InputEventData dataPackage) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
			this->messageId = dataPackage.getMessageId();
		}

		EditMessageData(RespondToInputEventData dataPackage) {
			this->channelId = dataPackage.channelId;
			this->messageId = dataPackage.messageId;
			this->addAllowedMentions(dataPackage.allowedMentions);
			this->requesterId = dataPackage.requesterId;
			for (auto& value : dataPackage.components) {
				this->components.push_back(value);
			}
			this->addContent(dataPackage.content);
			for (auto& value : dataPackage.embeds) {
				this->embeds.push_back(value);
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
		void addButton(bool disabled, string customId, string buttonLabel, ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
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
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int32_t maxValues, int32_t minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns void
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		/// \returns void
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns void
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns void
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns void
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:

		AllowedMentionsData allowedMentions{};
		vector<AttachmentData> attachments{};
		vector<ActionRowData> components{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string messageId{ "" };
		string content{ "" };
		int32_t flags{ 0 };
		bool tts{ false };

		EditMessageData() {};

	};

	/// Delete Message data. \brief Delete Message data.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	public:
		 int32_t timeDelay{ 0 }; ///< Number of milliseconds to wait before deleting the Message.
		MessageData messageData;///< The Message to delete.
		string reason{ "" };///< The reason for deleting the Message.
	};

	/// Delete Messages bulk data. \brief Delete Messages bulk data.
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
		vector<string> messageIds{}; ///< Array of Message ids to delete.
		string channelId{ "" };///< Channel within which to delete the Messages.
		string reason{ "" };///< The reason for deleting the Messages.
	};

	/// Get pinned Messages data. \brief Get pinned Messages data.
	struct DiscordCoreAPI_Dll GetPinnedMessagesData {
		string channelId{ "" };///< The Channel from which to collect pinned Messages.
	};

	/// Pin Message data. \brief Pin Message data.
	struct DiscordCoreAPI_Dll PinMessageData {
		string channelId{ "" };///< The Channel within which to pin the Message.
		string messageId{ "" };///< The Message which you would like to pin.
		string reason{ "" };///< Reason for pinning this Message.
	};

	/// Pin Message data. \brief Pin Message data.
	struct DiscordCoreAPI_Dll UnpinMessageData {
		string channelId{ "" };///< The Channel within which to unpin the Message.
		string messageId{ "" };///< The Message which you would like to unpin.
		string reason{ "" };///< Reason for pinning this Message.
	};

	/// A single Message. \brief A single Message.
	class DiscordCoreAPI_Dll Message : public MessageData {
	public:

		Message();

		Message(MessageData dataNew);

		virtual ~Message() {};
	};

	/**@}*/

	/**
	* \addtogroup utilities
	* @{
	*/

	/// MessageCollectorReturn data. \brief MessageCollectorReturn data.
	struct DiscordCoreAPI_Dll MessageCollectorReturnData {
		vector<Message> messages{}; ///< A vector of collected Messages.
	};

	/// MessageCollector, for collecting Messages from a Channel. \brief Message collector, for collecting Messages from a Channel.
	class DiscordCoreAPI_Dll MessageCollector {
	public:

		friend class DiscordCoreClient;

		MessageCollector();

		/// Begin waiting for Messages. \brief Begin waiting for Messages.
		/// \param quantityToCollect Maximum quantity of Messages to collect before returning the results.
		/// \param msToCollectForNew Maximum number of milliseconds to wait for Messages before returning the results.
		/// \param userIdNew User id to set for possible comparison.
		/// \param filteringFunctionNew A filter function to apply to new Messages, where returning "true" from the function results in a Message being stored.
		/// \returns A CoRoutine containing MessageCollectorReturnData.
		MessageCollectorReturnData  collectMessages(int32_t quantityToCollect, int32_t msToCollectForNew, string userIdNew, function<bool(Message)> filteringFunctionNew);

		~MessageCollector();

	protected:
		static map<string, unbounded_buffer<Message>*> messagesBufferMap;
		unique_ptr<unbounded_buffer<Message>> messagesBuffer{ nullptr };
		function<bool(Message)> filteringFunction{ nullptr };
		MessageCollectorReturnData messageReturnData{};
		int32_t quantityOfMessageToCollect{ 0 };
		int32_t msToCollectFor{ 0 };
		int32_t startingTime{ 0 };
		int32_t elapsedTime{ 0 };
		string userId{ "" };

		CoRoutine<void> run();

	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Message related Discord endpoints. \brief An interface class for the Message related Discord endpoints;
	class DiscordCoreAPI_Dll Messages {
	public:

		/// Collects a collection of Message from the Discord servers. \brief Collects a collection of Message from the Discord servers.
		/// \param dataPackage A GetMessagesData structure.
		/// \returns A CoRoutine containing an optional vector of Message.
		static CoRoutine<vector<Message>> getMessagesAsync(GetMessagesData dataPackage);

		/// Collects a Message from the Discord servers. \brief Collects a Message from the Discord servers.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> getMessageAsync(GetMessageData dataPackage);

		/// Creates a new Message. \brief Creates a new Message.
		/// \param dataPackage A CreateMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> createMessageAsync(CreateMessageData dataPackage);

		/// Crossposts a message from a News Channel to the following Channels. \brief Crossposts a message from a News Channel to the following Channels.
		/// \param dataPackage A CrosspostMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<Message> crosspostMessageAsync(CrosspostMessageData dataPackage);

		/// Edit a Message. \brief Edit a Message.
		/// \param dataPackage A EditMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> editMessageAsync(EditMessageData dataPackage);

		/// Deletes a Message. \brief Deletes a Message.
		/// \param dataPackage A DeleteMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteMessageAsync(DeleteMessageData dataPackage);

		/// Deletes a collection of Messages. \brief Deletes a collection of Messages.
		/// \param dataPackage A DeleteMessagesBulkData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage);		

		/// Collects a collection of pinned Messages from the Discord servers. \brief Collects a collection of pinned Messages from the Discord servers.
		/// \param dataPackage A GetPinnedMessagesData structure.
		/// \returns A CoRoutine containing a vector of Message.
		static CoRoutine<vector<Message>> getPinnedMessagesAsync(GetPinnedMessagesData dataPackage);

		/// Pins a Message to a given Channel. \brief Pins a Message to a given Channel.
		/// \param dataPackage A PinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> pinMessageAsync(PinMessageData dataPackage);

		/// Unpins a Message from a given Channel. \brief Unpins a Message from a given Channel.
		/// \param dataPackage A UnpinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> unpinMessageAsync(UnpinMessageData dataPackage);
	};
	/**@}*/

}