/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
/// https://discordcoreapi.com
/// \file MessageEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/JSONIfier.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For getting a collection of Messages. \brief For getting a collection of Messages.
	struct DiscordCoreAPI_Dll GetMessagesData {
		uint64_t aroundThisId{};///< Around this id.
		uint64_t beforeThisId{};///< Before this id.
		uint64_t afterThisId{};///< After this id.
		uint64_t channelId{};///< Channel from which to collect the Messages.
		int32_t limit{ 0 };///< Limit of Messages to collect.
	};

	/// For getting a Message. \brief For getting a Message.
	struct DiscordCoreAPI_Dll GetMessageData {
		uint64_t channelId{};///< The Channel from which to collect the Message.
		uint64_t id{};///< The id of the Message to collect.
	};

	/// For creating a Message. \brief For creating a Message.
	class DiscordCoreAPI_Dll CreateMessageData : public MessageResponseBase {
	  public:
		friend class DiscordCoreInternal::JSONIfier;
		friend InputEvents;
		friend Messages;

		CreateMessageData(const uint64_t& channelIdNew) {
			this->channelId = channelIdNew;
		}

		CreateMessageData(RespondToInputEventData dataPackage) {
			this->channelId = dataPackage.channelId;
			this->addAllowedMentions(dataPackage.allowedMentions);
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
			this->channelId = dataPackage.getChannelId();
		}

		uint64_t channelId{};

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
		uint64_t targetUserId{};
	};

	/// For crossposting a Message. \brief For crossposting a Message.
	struct DiscordCoreAPI_Dll CrosspostMessageData {
		uint64_t messageId{};///< Id of the message to be crossposted.
		uint64_t channelId{};///< Channel within which to crosspost the Message from.
	};

	/// For editing a Message. \brief For editing a Message.
	class DiscordCoreAPI_Dll EditMessageData : public MessageResponseBase {
	  public:
		friend class DiscordCoreInternal::JSONIfier;
		friend InputEvents;
		friend Messages;

		EditMessageData(InputEventData dataPackage) {
			this->channelId = dataPackage.getChannelId();
			this->messageId = dataPackage.getMessageId();
		}

		EditMessageData(RespondToInputEventData dataPackage) {
			this->allowedMentions = dataPackage.allowedMentions;
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
		uint64_t channelId{};
		uint64_t messageId{};
		int32_t flags{ 0 };

		EditMessageData() = default;
	};

	/// For deleting a Message. \brief For deleting a Message.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	  public:
		TimeStamp timeStamp{ "" };///< The created-at timestamp of the original message.
		uint64_t channelId{};///< The channel Id of the Message to delete.
		uint64_t messageId{};///< The message Id of the Message to delete.
		int32_t timeDelay{ 0 };///< Number of std::chrono::milliseconds to wait before deleting the Message.
		std::string reason{};///< The reason for deleting the Message.
	};

	/// For deleting a bulk of Messages. \brief For deleting a bulk of Messages.
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
		std::vector<uint64_t> messageIds{};///< Array of Message ids to delete.
		uint64_t channelId{};///< Channel within which to delete the Messages.
		std::string reason{};///< The reason for deleting the Messages.
	};

	/// For getting a collection of pinned Messages. \brief For getting a collection of pinned Messages.
	struct DiscordCoreAPI_Dll GetPinnedMessagesData {
		uint64_t channelId{};///< The Channel from which to collect pinned Messages.
	};

	/// For pinning a single Message. \brief For pinning a single Message.
	struct DiscordCoreAPI_Dll PinMessageData {
		uint64_t channelId{};///< The Channel within which to pin the Message.
		uint64_t messageId{};///< The Message which you would like to pin.
		std::string reason{};///< Reason for pinning this Message.
	};

	/// For unpinning a single Message. \brief For unpinning a single Message.
	struct DiscordCoreAPI_Dll UnpinMessageData {
		uint64_t channelId{};///< The Channel within which to unpin the Message.
		uint64_t messageId{};///< The Message which you would like to unpin.
		std::string reason{};///< Reason for pinning this Message.
	};

	/// A single Message. \brief A single Message.
	class DiscordCoreAPI_Dll Message : public MessageData {
	  public:
		Message() = default;

		Message& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObjectReal(jsonObjectData, this);
			return *this;
		}

		Message(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~Message() = default;

	  	inline void parseObjectReal(const nlohmann::json& jsonObjectData, Message* pDataStructure) {
			if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
				pDataStructure->content = jsonObjectData["content"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
				pDataStructure->author = jsonObjectData["author"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
				pDataStructure->editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
				pDataStructure->tts = jsonObjectData["tts"].get<bool>();
			}

			if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
				pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
			}

			if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
				pDataStructure->mentions.clear();
				pDataStructure->mentions.reserve(jsonObjectData["mentions"].size());
				for (auto& value: jsonObjectData["mentions"]) {
					UserData newData{ value };
					pDataStructure->mentions.push_back(newData);
				}
				pDataStructure->mentions.shrink_to_fit();
			}

			if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
				pDataStructure->mentionRoles.clear();
				pDataStructure->mentionRoles.reserve(jsonObjectData["mention_roles"].size());
				for (auto& value: jsonObjectData["mention_roles"]) {
					pDataStructure->mentionRoles.push_back(value.get<std::string>());
				}
				pDataStructure->mentionRoles.shrink_to_fit();
			}

			if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
				pDataStructure->mentionChannels.clear();
				pDataStructure->mentionChannels.reserve(jsonObjectData["mention_channels"].size());
				for (auto& value: jsonObjectData["mention_channels"]) {
					ChannelMentionData newData{ value };
					pDataStructure->mentionChannels.push_back(newData);
				}
				pDataStructure->mentionChannels.shrink_to_fit();
			}

			if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
				pDataStructure->attachments.clear();
				pDataStructure->attachments.reserve(jsonObjectData["attachments"].size());
				for (auto& value: jsonObjectData["attachments"]) {
					AttachmentData newData{ value };
					pDataStructure->attachments.push_back(newData);
				}
				pDataStructure->attachments.shrink_to_fit();
			}

			if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
				pDataStructure->embeds.clear();
				pDataStructure->embeds.reserve(jsonObjectData["embeds"].size());
				for (auto& value: jsonObjectData["embeds"]) {
					EmbedData newData{ value };
					pDataStructure->embeds.push_back(newData);
				}
				pDataStructure->embeds.shrink_to_fit();
			}

			if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
				pDataStructure->reactions.clear();
				pDataStructure->reactions.reserve(jsonObjectData["reactions"].size());
				for (auto& value: jsonObjectData["reactions"]) {
					ReactionData newData{ value };
					pDataStructure->reactions.push_back(newData);
				}
				pDataStructure->reactions.shrink_to_fit();
			}

			if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
				pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
			}

			if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
				pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
			}

			if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
				pDataStructure->webhookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<MessageType>();
			}

			if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
				pDataStructure->activity = jsonObjectData["activity"];
			}

			if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
				pDataStructure->application = jsonObjectData["application"];
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
				pDataStructure->messageReference = jsonObjectData["message_reference"];
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
			}

			if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
				pDataStructure->stickerItems.clear();
				pDataStructure->stickerItems.reserve(jsonObjectData["sticker_items"].size());
				for (auto& value: jsonObjectData["sticker_items"]) {
					StickerItemData newData{ value };
					pDataStructure->stickerItems.push_back(newData);
				}
				pDataStructure->stickerItems.shrink_to_fit();
			}

			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
				pDataStructure->stickers.shrink_to_fit();
			}

			if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
				pDataStructure->interaction = jsonObjectData["interaction"];
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->components.clear();
				pDataStructure->components.reserve(jsonObjectData["components"].size());
				for (auto& value: jsonObjectData["components"]) {
					ActionRowData newData{ value };
					pDataStructure->components.push_back(newData);
				}
				pDataStructure->components.shrink_to_fit();
			}

			if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
				pDataStructure->thread = jsonObjectData["thread"];
			}
		}
	};

	class MessageVector {
	  public:
		std::vector<Message> theMessages{};

		MessageVector() = default;

		MessageVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObjectReal(jsonObjectData, this);
			return *this;
		}

		MessageVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageVector() = default;

		inline void parseObjectReal(const nlohmann::json& jsonObjectData, MessageVector* pDataStructure) {
			pDataStructure->theMessages.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::Message newData{ value };
				pDataStructure->theMessages.push_back(newData);
			}
			pDataStructure->theMessages.shrink_to_fit();
		}
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
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// Collects a collection of Message from the Discord servers. \brief Collects a collection of Message from the Discord servers
		/// \param dataPackage A GetMessagesData structure.
		/// \returns A CoRoutine containing a MessageVector.
		static CoRoutine<MessageVector> getMessagesAsync(GetMessagesData dataPackage);

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
		/// \returns A CoRoutine containing a MessageVector.
		static CoRoutine<MessageVector> getPinnedMessagesAsync(GetPinnedMessagesData dataPackage);

		/// Pins a Message to a given Channel. \brief Pins a Message to a given Channel.
		/// \param dataPackage A PinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> pinMessageAsync(PinMessageData dataPackage);

		/// Unpins a Message from a given Channel. \brief Unpins a Message from a given Channel.
		/// \param dataPackage An UnpinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> unpinMessageAsync(UnpinMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/

}// namespace DiscordCoreAPI
