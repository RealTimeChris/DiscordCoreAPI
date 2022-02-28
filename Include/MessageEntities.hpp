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
/// MessageEntities.hpp - Header for the Message related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file MessageEntities.hpp

#pragma once

#include <JSONIfier.hpp>

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For getting a collection of Messages. \brief For getting a collection of Messages.
	struct DiscordCoreAPI_Dll GetMessagesData {
		std::string aroundThisId{ "" };///<Around this id.
		std::string beforeThisId{ "" };///< Before this id.
		std::string afterThisId{ "" };///< After this id.
		std::string channelId{ "" };///< Channel from which to collect the Messages.
		 int32_t limit{ 0 };///< Limit of Messages to collect.		
	};

	/// For getting a Message. \brief For getting a Message.
	struct DiscordCoreAPI_Dll GetMessageData {
		std::string requesterId{ "" };///< The requester id.
		std::string channelId{ "" };///< The Channel from which to collect the Message.
		std::string id{ "" };///< The id of the Message to collect.
	};

	class DiscordCoreAPI_Dll MessageResponseBase {
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
					component.style = static_cast<int32_t>(buttonStyle);
					component.customId = customIdNew;
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
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A std::vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		void addSelectMenu(bool disabled, std::string customIdNew, std::vector<SelectOptionData> options, std::string placeholder, int32_t maxValues, int32_t minValues) {
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
					componentData.customId = customIdNew;
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
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A std::string, containing the content.
		void addContent(std::string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:

		std::vector<AttachmentData> attachments{};
		MessageReferenceData messageReference{};
		std::vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		std::vector<std::string> stickerIds{};
		std::vector<EmbedData> embeds{};
		std::string content{ "" };
		bool tts{ false };
	};

	/// For creating a Message. \brief For creating a Message.
	class DiscordCoreAPI_Dll CreateMessageData : public MessageResponseBase {
	public:

		friend std::string DiscordCoreInternal::JSONIFY(CreateMessageData dataPackage);
		friend InputEvents;
		friend Messages;

		CreateMessageData(std::string channelIdNew) {
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
		
		std::string requesterId{ "" };
		std::string channelId{ "" };	

		CreateMessageData() = default;
	};

	/// For sending a direct-message. \brief For sending a direct-message.
	class DiscordCoreAPI_Dll SendDMData : public CreateMessageData {
	public:
		friend InputEvents;

		SendDMData(RespondToInputEventData dataPackage) {
			this->targetUserId = dataPackage.targetUserId;
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

	protected:

		std::string targetUserId{ "" };
		std::string requesterId{ "" };
		std::string channelId{ "" };

	};

	/// For crossposting a Message. \brief For crossposting a Message.
	struct DiscordCoreAPI_Dll CrosspostMessageData : public MessageResponseBase {
		std::string messageId{ "" };///< Id of the message to be crossposted.
		std::string channelId{ "" };///< Channel within which to crosspost the Message from.
	};

	/// For editing a Message. \brief For editing a Message.
	class DiscordCoreAPI_Dll EditMessageData : public MessageResponseBase {
	public:

		friend std::string DiscordCoreInternal::JSONIFY(EditMessageData dataPackage);
		friend InputEvents;
		friend Messages;

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

	protected:

		std::string requesterId{ "" };
		std::string channelId{ "" };
		std::string messageId{ "" };
		int32_t flags{ 0 };

		EditMessageData() = default;

	};

	/// For deleting a Message. \brief For deleting a Message.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	public:
		 int32_t timeDelay{ 0 };///< Number of std::chrono::milliseconds to wait before deleting the Message.
		MessageData messageData;///< The Message to delete.
		std::string reason{ "" };///< The reason for deleting the Message.
	};

	/// For deleting a bulk of Messages. \brief For deleting a bulk of Messages.
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
		std::vector<std::string> messageIds{};///< Array of Message ids to delete.
		std::string channelId{ "" };///< Channel within which to delete the Messages.
		std::string reason{ "" };///< The reason for deleting the Messages.
	};

	/// For getting a collection of pinned Messages. \brief For getting a collection of pinned Messages.
	struct DiscordCoreAPI_Dll GetPinnedMessagesData {
		std::string channelId{ "" };///< The Channel from which to collect pinned Messages.
	};

	/// For pinning a single Message. \brief For pinning a single Message.
	struct DiscordCoreAPI_Dll PinMessageData {
		std::string channelId{ "" };///< The Channel within which to pin the Message.
		std::string messageId{ "" };///< The Message which you would like to pin.
		std::string reason{ "" };///< Reason for pinning this Message.
	};

	/// For unpinning a single Message. \brief For unpinning a single Message.
	struct DiscordCoreAPI_Dll UnpinMessageData {
		std::string channelId{ "" };///< The Channel within which to unpin the Message.
		std::string messageId{ "" };///< The Message which you would like to unpin.
		std::string reason{ "" };///< Reason for pinning this Message.
	};

	/// A single Message. \brief A single Message.
	class DiscordCoreAPI_Dll Message : public MessageData {
	public:

		Message() = default;

		Message(MessageData dataNew);
	};

	/**@}*/

	/**
	* \addtogroup utilities
	* @{
	*/

	/// MessageCollectorReturn responseData. \brief MessageCollectorReturn responseData.
	struct DiscordCoreAPI_Dll MessageCollectorReturnData {
		std::vector<Message> messages{};///< A std::vector of collected Messages.
	};

	/// MessageCollector, for collecting Messages from a Channel. \brief Message collector, for collecting Messages from a Channel.
	class DiscordCoreAPI_Dll MessageCollector {
	public:

		friend DiscordCoreClient;

		MessageCollector() = default;

		/// Begin waiting for Messages. \brief Begin waiting for Messages.
		/// \param quantityToCollect Maximum quantity of Messages to collect before returning the results.
		/// \param msToCollectForNew Maximum number of std::chrono::milliseconds to wait for Messages before returning the results.
		/// \param userIdNew User id to set for possible comparison.
		/// \param filteringFunctionNew A filter function to apply to new Messages, where returning "true" from the function results in a Message being stored.
		/// \returns A MessageCollectorReturnData structure.
		CoRoutine<MessageCollectorReturnData> collectMessages(int32_t quantityToCollect, int32_t msToCollectForNew, std::string userIdNew, std::function<bool(Message)> filteringFunctionNew);

		~MessageCollector();

	protected:

		static std::unordered_map<std::string, UnboundedMessageBlock<Message>*> messagesBufferMap;
		std::unique_ptr<UnboundedMessageBlock<Message>> messagesBuffer{ nullptr };
		std::function<bool(Message)> filteringFunction{ nullptr };
		MessageCollectorReturnData messageReturnData{};
		int32_t quantityOfMessageToCollect{ 0 };
		int32_t msToCollectFor{ 0 };
		std::string userId{ "" };

		void run();

	};
	/**@}*/

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class for the Message related Discord endpoints. \brief An interface class for the Message related Discord endpoints;
	class DiscordCoreAPI_Dll Messages {
	public:

		static void initialize(DiscordCoreInternal::HttpClient*);

		/// Collects a collection of Message from the Discord servers. \brief Collects a collection of Message from the Discord servers.
		/// \param dataPackage A GetMessagesData structure.
		/// \returns A CoRoutine containing a vector<Message>.
		static CoRoutine<std::vector<Message>> getMessagesAsync(GetMessagesData dataPackage);

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
		/// \param dataPackage An EditMessageData structure.
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
		/// \returns A CoRoutine containing a vector<Message>.
		static CoRoutine<std::vector<Message>> getPinnedMessagesAsync(GetPinnedMessagesData dataPackage);

		/// Pins a Message to a given Channel. \brief Pins a Message to a given Channel.
		/// \param dataPackage A PinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> pinMessageAsync(PinMessageData dataPackage);

		/// Unpins a Message from a given Channel. \brief Unpins a Message from a given Channel.
		/// \param dataPackage An UnpinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> unpinMessageAsync(UnpinMessageData dataPackage);

	protected:

		static DiscordCoreInternal::HttpClient* httpClient;

		static void deleteMessageToBeWrapped(DeleteMessageData dataPackage);
	};
	/**@}*/

}
