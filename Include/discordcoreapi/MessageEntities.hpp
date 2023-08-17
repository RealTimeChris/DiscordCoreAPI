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
/// MessageEntities.hpp - Header for the Message related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file MessageEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief Typedef for the message filter.
	template<typename ValueType> using ObjectFilter = std::function<bool(ValueType)>;

	/// @brief Object collector, for collecting Objects from a Channel.
	template<typename ValueType> class DiscordCoreAPI_Dll ObjectCollector {
	  public:
		/// @brief ObjectCollectorReturnData responseData.c
		struct ObjectCollectorReturnData {
			Jsonifier::Vector<ValueType> objects{};///< A vector of collected Objects.
		};

		static UnorderedMap<std::string, UnboundedMessageBlock<ValueType>*> objectsBuffersMap;

		ObjectCollector();

		/// @brief Begin waiting for Objects.
		/// @param quantityToCollect Maximum quantity of Objects to collect before returning the results.
		/// @param msToCollectForNew Maximum number of Milliseconds to wait for Objects before returning the results.
		/// @param filteringFunctionNew A filter function to apply to new Objects, where returning "true" from the function results in a Object being stored.
		/// @return A ObjectCollectorReturnData structure.
		CoRoutine<ObjectCollectorReturnData> collectObjects(int32_t quantityToCollect, int32_t msToCollectForNew,
			ObjectFilter<ValueType> filteringFunctionNew);

		void run(std::coroutine_handle<typename DiscordCoreAPI::CoRoutine<
				typename DiscordCoreAPI::ObjectCollector<ValueType>::ObjectCollectorReturnData>::promise_type>& coroHandle);

		~ObjectCollector();

	  protected:
		UnboundedMessageBlock<ValueType> objectsBuffer{};
		ObjectCollectorReturnData objectReturnData{};
		ObjectFilter<ValueType> filteringFunction{};
		int32_t quantityOfObjectsToCollect{};
		std::string collectorId{};
		int32_t msToCollectFor{};
	};

	using MessageCollector = ObjectCollector<MessageData>;

	/// @brief For getting a collection of Messages.
	struct GetMessagesData {
		Snowflake beforeThisId{};///< Before this id.
		uint64_t aroundThisId{};///< Around this id.
		Snowflake afterThisId{};///< After this id.
		Snowflake channelId{};///< ChannelData from which to collect the Messages.
		int32_t limit{};///< Limit of Messages to collect.
	};

	/// @brief For getting a Message.
	struct GetMessageData {
		Snowflake channelId{};///< The ChannelData from which to collect the MessageData.
		Snowflake id{};///< The id of the Message to collect.
	};

	/// @brief For creating a Message.
	class DiscordCoreAPI_Dll CreateMessageData : public MessageResponseBase {
	  public:
		friend struct Jsonifier::Core<DiscordCoreAPI::CreateMessageData>;
		friend class InputEvents;
		friend class Messages;

		CreateMessageData(const Snowflake channelIdNew);

		CreateMessageData(RespondToInputEventData dataPackage);

		CreateMessageData(InputEventData dataPackage);

		CreateMessageData(MessageData dataPackage);

		Snowflake channelId{};

		CreateMessageData() = default;

	  protected:
		Jsonifier::Vector<AttachmentData> attachments{};
		MessageReferenceData messageReference{};
		Jsonifier::Vector<std::string> stickerIds{};
	};

	/// @brief For sending a direct-message.
	class DiscordCoreAPI_Dll SendDMData : public CreateMessageData {
	  public:
		friend class InputEvents;

		SendDMData(RespondToInputEventData dataPackage);

	  protected:
		Snowflake targetUserId{};
	};

	/// @brief For crossposting a Message.
	struct CrosspostMessageData {
		Snowflake messageId{};///< Snowflake of the message to be crossposted.
		Snowflake channelId{};///< ChannelData within which to crosspost the Message from.
	};

	/// @brief For editing a Message.
	class DiscordCoreAPI_Dll EditMessageData : public MessageResponseBase {
	  public:
		friend struct Jsonifier::Core<DiscordCoreAPI::EditMessageData>;
		friend class InputEvents;
		friend class Messages;

		EditMessageData(InputEventData dataPackage);

		EditMessageData(RespondToInputEventData dataPackage);

	  protected:
		Jsonifier::Vector<AttachmentData> attachments{};
		Snowflake channelId{};
		Snowflake messageId{};
		int32_t flags{};

		EditMessageData() = default;
	};

	/// @brief For deleting a Message.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	  public:
		DeleteMessageData(const MessageData& messageToDelete);
		TimeStampParse timeStamp{};///< The created-at timeStamp of the original message.
		Snowflake channelId{};///< The channel Snowflake of the Message to delete.
		Snowflake messageId{};///< The message Snowflake of the Message to delete.
		std::string reason{};///< The reason for deleting the MessageData.
		int32_t timeDelay{};///< Number of Milliseconds to wait before deleting the MessageData.
	};

	/// @brief For deleting a bulk of Messages.
	struct DeleteMessagesBulkData {
		Jsonifier::Vector<Snowflake> messageIds{};///< Array of Message ids to delete.
		Snowflake channelId{};///< ChannelData within which to delete the Messages.
		std::string reason{};///< The reason for deleting the Messages.
	};

	/// @brief For getting a collection of pinned Messages.
	struct GetPinnedMessagesData {
		Snowflake channelId{};///< The ChannelData from which to collect pinned Messages.
	};

	/// @brief For pinning a single MessageData.
	struct PinMessageData {
		Snowflake channelId{};///< The ChannelData within which to pin the MessageData.
		Snowflake messageId{};///< The Message which you would like to pin.
		std::string reason{};///< Reason for pinning this MessageData.
	};

	/// @brief For unpinning a single MessageData.
	struct UnpinMessageData {
		Snowflake channelId{};///< The ChannelData within which to unpin the MessageData.
		Snowflake messageId{};///< The Message which you would like to unpin.
		std::string reason{};///< Reason for pinning this MessageData.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the Message related Discord endpoints.
	class DiscordCoreAPI_Dll Messages {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Collects a collection of Message from the Discord servers
		/// @param dataPackage A GetMessagesData structure.
		/// @return A CoRoutine containing a Jsonifier::Vector<MessageData>.
		static CoRoutine<Jsonifier::Vector<MessageData>> getMessagesAsync(GetMessagesData dataPackage);

		/// @brief Collects a Message from the Discord servers.
		/// @param dataPackage A GetMessageData structure.
		/// @return A CoRoutine containing a Message.
		static CoRoutine<MessageData> getMessageAsync(GetMessageData dataPackage);

		/// @brief Creates a new MessageData.
		/// @param dataPackage A CreateMessageData structure.
		/// @return A CoRoutine containing a Message.
		static CoRoutine<MessageData> createMessageAsync(CreateMessageData dataPackage);

		/// @brief Crossposts a message from a News ChannelData to the following Channels.
		/// @param dataPackage A CrosspostMessageData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<MessageData> crosspostMessageAsync(CrosspostMessageData dataPackage);

		/// @brief Edit a Message.
		/// @param dataPackage An EditMessageData structure.
		/// @return A CoRoutine containing a Message.
		static CoRoutine<MessageData> editMessageAsync(EditMessageData dataPackage);

		/// @brief Deletes a Message.
		/// @param dataPackage A DeleteMessageData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteMessageAsync(DeleteMessageData dataPackage);

		/// @brief Deletes a collection of Messages.
		/// @param dataPackage A DeleteMessagesBulkData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage);

		/// @brief Collects a collection of pinned Messages from the Discord servers.
		/// @param dataPackage A GetPinnedMessagesData structure.
		/// @return A CoRoutine containing a Jsonifier::Vector<MessageData>.
		static CoRoutine<Jsonifier::Vector<MessageData>> getPinnedMessagesAsync(GetPinnedMessagesData dataPackage);

		/// @brief Pins a Message to a given Channel.
		/// @param dataPackage A PinMessageData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> pinMessageAsync(PinMessageData dataPackage);

		/// @brief Unpins a Message from a given Channel.
		/// @param dataPackage An UnpinMessageData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> unpinMessageAsync(UnpinMessageData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/

}
