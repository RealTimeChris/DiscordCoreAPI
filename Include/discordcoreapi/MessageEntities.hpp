/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// MessageEntities.hpp - Header for the Message related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file MessageEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For getting a collection of Messages. \brief For getting a collection of Messages.
	struct DiscordCoreAPI_Dll GetMessagesData {
		std::string aroundThisId{};///< Around this id.
		std::string beforeThisId{};///< Before this id.
		std::string afterThisId{};///< After this id.
		std::string channelId{};///< Channel from which to collect the Messages.
		int32_t limit{ 0 };///< Limit of Messages to collect.
	};

	/// For getting a Message. \brief For getting a Message.
	struct DiscordCoreAPI_Dll GetMessageData {
		std::string requesterId{};///< The requester id.
		std::string channelId{};///< The Channel from which to collect the Message.
		std::string id{};///< The id of the Message to collect.
	};

	/// For creating a Message. \brief For creating a Message.
	class DiscordCoreAPI_Dll CreateMessageData : public MessageResponseBase {
	  public:
		friend std::string DiscordCoreInternal::JSONIFY(CreateMessageData dataPackage);
		friend InputEvents;
		friend Messages;

		CreateMessageData(const std::string& channelIdNew) {
			this->channelId = channelIdNew;
		}

		CreateMessageData(RespondToInputEventData dataPackage) {
			this->channelId = dataPackage.channelId;
			this->addAllowedMentions(dataPackage.allowedMentions);
			this->requesterId = dataPackage.requesterId;
			for (auto& value: dataPackage.components) {
				this->components.push_back(value);
			}
			this->addContent(dataPackage.content);
			for (auto& value: dataPackage.embeds) {
				this->embeds.push_back(value);
			}
			this->tts = dataPackage.tts;
		}

		CreateMessageData(InputEventData dataPackage) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
		}

		std::string requesterId{};
		std::string channelId{};

		CreateMessageData() = default;

	  protected:
		std::vector<AttachmentData> attachments{};
		MessageReferenceData messageReference{};
		std::vector<std::string> stickerIds{};
	};

	/// For sending a direct-message. \brief For sending a direct-message.
	class DiscordCoreAPI_Dll SendDMData : public CreateMessageData {
	  public:
		friend InputEvents;

		SendDMData(RespondToInputEventData dataPackage) {
			this->targetUserId = dataPackage.targetUserId;
			this->addAllowedMentions(dataPackage.allowedMentions);
			this->requesterId = dataPackage.requesterId;
			for (auto& value: dataPackage.components) {
				this->components.push_back(value);
			}
			this->addContent(dataPackage.content);
			for (auto& value: dataPackage.embeds) {
				this->embeds.push_back(value);
			}
			this->channelId = dataPackage.targetUserId;
			this->tts = dataPackage.tts;
		}

	  protected:
		std::string targetUserId{};
	};

	/// For crossposting a Message. \brief For crossposting a Message.
	struct DiscordCoreAPI_Dll CrosspostMessageData {
		std::string messageId{};///< Id of the message to be crossposted.
		std::string channelId{};///< Channel within which to crosspost the Message from.
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
			this->allowedMentions = dataPackage.allowedMentions;
			this->requesterId = dataPackage.requesterId;
			this->channelId = dataPackage.channelId;
			this->messageId = dataPackage.messageId;
			for (auto& value: dataPackage.components) {
				this->components.push_back(value);
			}
			this->content = dataPackage.content;
			for (auto& value: dataPackage.embeds) {
				this->embeds.push_back(value);
			}
		}

	  protected:
		std::vector<AttachmentData> attachments{};
		std::string requesterId{};
		std::string channelId{};
		std::string messageId{};
		int32_t flags{ 0 };

		EditMessageData() = default;
	};

	/// For deleting a Message. \brief For deleting a Message.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	  public:
		TimeStamp timeStamp{ "" };///< The created-at timestamp of the original message.
		std::string channelId{};///< The channel Id of the Message to delete.
		std::string messageId{};///< The message Id of the Message to delete.
		int32_t timeDelay{ 0 };///< Number of std::chrono::milliseconds to wait before deleting the Message.
		std::string reason{};///< The reason for deleting the Message.
	};

	/// For deleting a bulk of Messages. \brief For deleting a bulk of Messages.
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
		std::vector<std::string> messageIds{};///< Array of Message ids to delete.
		std::string channelId{};///< Channel within which to delete the Messages.
		std::string reason{};///< The reason for deleting the Messages.
	};

	/// For getting a collection of pinned Messages. \brief For getting a collection of pinned Messages.
	struct DiscordCoreAPI_Dll GetPinnedMessagesData {
		std::string channelId{};///< The Channel from which to collect pinned Messages.
	};

	/// For pinning a single Message. \brief For pinning a single Message.
	struct DiscordCoreAPI_Dll PinMessageData {
		std::string channelId{};///< The Channel within which to pin the Message.
		std::string messageId{};///< The Message which you would like to pin.
		std::string reason{};///< Reason for pinning this Message.
	};

	/// For unpinning a single Message. \brief For unpinning a single Message.
	struct DiscordCoreAPI_Dll UnpinMessageData {
		std::string channelId{};///< The Channel within which to unpin the Message.
		std::string messageId{};///< The Message which you would like to unpin.
		std::string reason{};///< Reason for pinning this Message.
	};

	/// A single Message. \brief A single Message.
	class DiscordCoreAPI_Dll Message : public MessageData {
	  public:
		Message() = default;

		virtual ~Message() = default;
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// Typedef for the message filter. \brief Typedef for the message filter.
	using MessageFilter = std::function<bool(Message)>;

	/// MessageCollectorReturnData responseData. \brief MessageCollectorReturnData responseData.
	struct DiscordCoreAPI_Dll MessageCollectorReturnData {
		std::vector<Message> messages{};///< A std::vector of collected Messages.
	};

	/// MessageCollector, for collecting Messages from a Channel. \brief Message collector, for collecting Messages from a Channel.
	class DiscordCoreAPI_Dll MessageCollector {
	  public:
		static std::unordered_map<std::string, UnboundedMessageBlock<Message>*> messagesBufferMap;

		MessageCollector() = default;

		/// Begin waiting for Messages. \brief Begin waiting for Messages.
		/// \param quantityToCollect Maximum quantity of Messages to collect before returning the results.
		/// \param msToCollectForNew Maximum number of std::chrono::milliseconds to wait for Messages before returning the results.
		/// \param messageFilter A filter function to apply to new Messages, where returning "true" from the function results in a Message being stored.
		/// \returns A MessageCollectorReturnData structure.
		CoRoutine<MessageCollectorReturnData> collectMessages(int32_t quantityToCollect, int32_t msToCollectForNew, MessageFilter messageFilter);

		~MessageCollector();

	  protected:
		UnboundedMessageBlock<Message> messagesBuffer{};
		MessageCollectorReturnData messageReturnData{};
		MessageFilter filteringFunction{ nullptr };
		int32_t quantityOfMessageToCollect{ 0 };
		std::string collectorId{};
		int32_t msToCollectFor{ 0 };

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

		/// Collects a collection of Message from the Discord servers. \brief Collects a collection of Message from the Discord servers
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
	};
	/**@}*/

}// namespace DiscordCoreAPI
