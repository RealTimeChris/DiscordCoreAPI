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
/// MessageEntities.hpp - Header for the Message related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file MessageEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// \brief MessageCollectorReturnData responseData.
	struct DiscordCoreAPI_Dll MessageCollectorReturnData {
		std::vector<MessageData> messages{};///< A vector of collected Objects.
	};

	/// \brief Message collector, for collecting Messages from a Channel.
	class DiscordCoreAPI_Dll MessageCollector {
	  public:
		static std::unordered_map<std::string, UnboundedMessageBlock<MessageData>*> objectsBuffersMap;

		MessageCollector() noexcept = default;

		/// \brief Begin waiting for Messages.
		/// \param quantityToCollect Maximum quantity of Messages to collect before returning the results.
		/// \param msToCollectForNew Maximum number of Milliseconds to wait for Messages before returning the results.
		/// \param filteringFunctionNew A filter function to apply to new Messages, where returning "true" from the function results in a Message being stored.
		/// \returns A ObjectCollectorReturnData structure.
		CoRoutine<MessageCollectorReturnData> collectMessages(int32_t quantityToCollect, int32_t msToCollectForNew,
			ObjectFilter<MessageData> filteringFunctionNew);

		void run();

		~MessageCollector();

	  protected:
		ObjectFilter<MessageData> filteringFunction{ nullptr };
		UnboundedMessageBlock<MessageData> objectsBuffer{};
		MessageCollectorReturnData objectReturnData{};
		int32_t quantityOfMessageToCollect{};
		std::string collectorId{};
		int32_t msToCollectFor{};
	};

	/// \brief For getting a collection of Messages.
	struct DiscordCoreAPI_Dll GetMessagesData {
		Snowflake beforeThisId{};///< Before this id.
		uint64_t aroundThisId{};///< Around this id.
		Snowflake afterThisId{};///< After this id.
		Snowflake channelId{};///< Channel from which to collect the Messages.
		int32_t limit{};///< Limit of Messages to collect.
	};

	/// \brief For getting a Message.
	struct DiscordCoreAPI_Dll GetMessageData {
		Snowflake channelId{};///< The Channel from which to collect the Message.
		Snowflake id{};///< The id of the Message to collect.
	};

	/// \brief For creating a Message.
	class DiscordCoreAPI_Dll CreateMessageData : public MessageResponseBase {
	  public:
		friend class InputEvents;
		friend class Messages;

		CreateMessageData(const Snowflake channelIdNew);

		CreateMessageData(RespondToInputEventData dataPackage);

		CreateMessageData(InputEventData dataPackage);

		CreateMessageData(MessageData dataPackage);

		Snowflake channelId{};

		CreateMessageData() noexcept = default;

		operator Jsonifier();

	  protected:
		std::vector<AttachmentData> attachments{};
		MessageReferenceData messageReference{};
		std::vector<std::string> stickerIds{};
	};

	/// \brief For sending a direct-message.
	class DiscordCoreAPI_Dll SendDMData : public CreateMessageData {
	  public:
		friend class InputEvents;

		SendDMData(RespondToInputEventData dataPackage);

	  protected:
		Snowflake targetUserId{};
	};

	/// \brief For crossposting a Message.
	struct DiscordCoreAPI_Dll CrosspostMessageData {
		Snowflake messageId{};///< Id of the message to be crossposted.
		Snowflake channelId{};///< Channel within which to crosspost the Message from.
	};

	/// \brief For editing a Message.
	class DiscordCoreAPI_Dll EditMessageData : public MessageResponseBase {
	  public:
		friend class InputEvents;
		friend class Messages;

		EditMessageData(InputEventData dataPackage);

		EditMessageData(RespondToInputEventData dataPackage);

		operator Jsonifier();

	  protected:
		std::vector<AttachmentData> attachments{};
		Snowflake channelId{};
		Snowflake messageId{};
		int32_t flags{};

		EditMessageData() noexcept = default;
	};

	/// \brief For deleting a Message.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	  public:
		TimeStamp timeStamp{};///< The created-at timeStamp of the original message.
		Snowflake channelId{};///< The channel Id of the Message to delete.
		Snowflake messageId{};///< The message Id of the Message to delete.
		std::string reason{};///< The reason for deleting the Message.
		int32_t timeDelay{};///< Number of Milliseconds to wait before deleting the Message.
	};

	/// \brief For deleting a bulk of Messages.
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
		std::vector<Snowflake> messageIds{};///< Array of Message ids to delete.
		Snowflake channelId{};///< Channel within which to delete the Messages.
		std::string reason{};///< The reason for deleting the Messages.

		operator Jsonifier();
	};

	/// \brief For getting a collection of pinned Messages.
	struct DiscordCoreAPI_Dll GetPinnedMessagesData {
		Snowflake channelId{};///< The Channel from which to collect pinned Messages.
	};

	/// \brief For pinning a single Message.
	struct DiscordCoreAPI_Dll PinMessageData {
		Snowflake channelId{};///< The Channel within which to pin the Message.
		Snowflake messageId{};///< The Message which you would like to pin.
		std::string reason{};///< Reason for pinning this Message.
	};

	/// \brief For unpinning a single Message.
	struct DiscordCoreAPI_Dll UnpinMessageData {
		Snowflake channelId{};///< The Channel within which to unpin the Message.
		Snowflake messageId{};///< The Message which you would like to unpin.
		std::string reason{};///< Reason for pinning this Message.
	};

	/// \brief A single Message.
	class DiscordCoreAPI_Dll Message : public MessageData {
	  public:
		Message() noexcept = default;

		Message(simdjson::ondemand::value jsonObjectData);

		virtual ~Message() noexcept = default;
	};

	class DiscordCoreAPI_Dll MessageVector {
	  public:
		MessageVector() noexcept = default;

		operator std::vector<Message>();

		MessageVector(simdjson::ondemand::value jsonObjectData);

		virtual ~MessageVector() noexcept = default;

	  protected:
		std::vector<Message> messages{};
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the Message related Discord endpoints;
	class DiscordCoreAPI_Dll Messages {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Collects a collection of Message from the Discord servers
		/// \param dataPackage A GetMessagesData structure.
		/// \returns A CoRoutine containing a MessageVector.
		static CoRoutine<std::vector<Message>> getMessagesAsync(GetMessagesData dataPackage);

		/// \brief Collects a Message from the Discord servers.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> getMessageAsync(GetMessageData dataPackage);

		/// \brief Creates a new Message.
		/// \param dataPackage A CreateMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> createMessageAsync(CreateMessageData dataPackage);

		/// \brief Crossposts a message from a News Channel to the following Channels.
		/// \param dataPackage A CrosspostMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<Message> crosspostMessageAsync(CrosspostMessageData dataPackage);

		/// \brief Edit a Message.
		/// \param dataPackage An EditMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<Message> editMessageAsync(EditMessageData dataPackage);

		/// \brief Deletes a Message.
		/// \param dataPackage A DeleteMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteMessageAsync(DeleteMessageData dataPackage);

		/// \brief Deletes a collection of Messages.
		/// \param dataPackage A DeleteMessagesBulkData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage);

		/// \brief Collects a collection of pinned Messages from the Discord servers.
		/// \param dataPackage A GetPinnedMessagesData structure.
		/// \returns A CoRoutine containing a MessageVector.
		static CoRoutine<std::vector<Message>> getPinnedMessagesAsync(GetPinnedMessagesData dataPackage);

		/// \brief Pins a Message to a given Channel.
		/// \param dataPackage A PinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> pinMessageAsync(PinMessageData dataPackage);

		/// \brief Unpins a Message from a given Channel.
		/// \param dataPackage An UnpinMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> unpinMessageAsync(UnpinMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/

}// namespace DiscordCoreAPI