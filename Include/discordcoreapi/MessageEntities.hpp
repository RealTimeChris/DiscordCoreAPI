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

	/// \brief ObjectCollectorReturnData responseData.
	struct DiscordCoreAPI_Dll MessageCollectorReturnData {
		std::vector<MessageData> objects{};///< A vector of collected Objects.
	};

	/// \brief ObjectCollectorReturnData responseData.
	struct DiscordCoreAPI_Dll ReactionCollectorReturnData {
		std::vector<ReactionData> objects{};///< A vector of collected Objects.
	};

	/// \brief Object collector, for collecting Objects from a ChannelData.
	class DiscordCoreAPI_Dll MessageCollector {
	  public:
		inline static std::unordered_map<std::string, UnboundedMessageBlock<MessageData>*> objectsBuffersMap{};

		inline MessageCollector() {
			collectorId = std::to_string(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
			MessageCollector::objectsBuffersMap[collectorId] = &objectsBuffer;
		};

		/// \brief Begin waiting for Objects.
		/// \param quantityToCollect Maximum quantity of Objects to collect before returning the results.
		/// \param msToCollectForNew Maximum number of Milliseconds to wait for Objects before returning the results.
		/// \param filteringFunctionNew A filter function to apply to new Objects, where returning "true" from the function results in a Object being stored.
		/// \returns A ObjectCollectorReturnData structure.
		inline CoRoutine<MessageCollectorReturnData> collectObjects(int32_t quantityToCollect, int32_t msToCollectForNew,
			ObjectFilter<MessageData> filteringFunctionNew) {
			co_await NewThreadAwaitable<MessageCollectorReturnData>();
			quantityOfObjectsToCollect = quantityToCollect;
			filteringFunction = filteringFunctionNew;
			msToCollectFor = msToCollectForNew;

			run();
			co_return std::move(objectReturnData);
		}

		inline void run() {
			int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
			int64_t elapsedTime{};
			while (elapsedTime < msToCollectFor) {
				MessageData message{};
				waitForTimeToPass<MessageData>(objectsBuffer, message, static_cast<uint64_t>(msToCollectFor - static_cast<uint64_t>(elapsedTime)));
				if (filteringFunction(message)) {
					objectReturnData.objects.emplace_back(message);
				}
				if (static_cast<int32_t>(objectReturnData.objects.size()) >= quantityOfObjectsToCollect) {
					break;
				}

				elapsedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count() - startingTime;
			}
		}

		inline ~MessageCollector() {
			if (MessageCollector::objectsBuffersMap.contains(collectorId)) {
				MessageCollector::objectsBuffersMap.erase(collectorId);
			}
		};

	  protected:
		UnboundedMessageBlock<MessageData> objectsBuffer{};
		MessageCollectorReturnData objectReturnData{};
		ObjectFilter<MessageData> filteringFunction{};
		int32_t quantityOfObjectsToCollect{};
		std::string collectorId{};
		int32_t msToCollectFor{};
	};

	/// \brief Object collector, for collecting Objects from a ChannelData.
	class DiscordCoreAPI_Dll ReactionCollector {
	  public:
		inline static std::unordered_map<std::string, UnboundedMessageBlock<ReactionData>*> objectsBuffersMap{};

		inline ReactionCollector() {
			collectorId = std::to_string(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
			ReactionCollector::objectsBuffersMap[collectorId] = &objectsBuffer;
		};

		/// \brief Begin waiting for Objects.
		/// \param quantityToCollect Maximum quantity of Objects to collect before returning the results.
		/// \param msToCollectForNew Maximum number of Milliseconds to wait for Objects before returning the results.
		/// \param filteringFunctionNew A filter function to apply to new Objects, where returning "true" from the function results in a Object being stored.
		/// \returns A ObjectCollectorReturnData structure.
		inline CoRoutine<ReactionCollectorReturnData> collectObjects(int32_t quantityToCollect, int32_t msToCollectForNew,
			ObjectFilter<ReactionData> filteringFunctionNew) {
			co_await NewThreadAwaitable<ReactionCollectorReturnData>();
			quantityOfObjectsToCollect = quantityToCollect;
			filteringFunction = filteringFunctionNew;
			msToCollectFor = msToCollectForNew;

			run();
			co_return std::move(objectReturnData);
		}

		inline void run() {
			int64_t startingTime = static_cast<int64_t>(std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count());
			int64_t elapsedTime{};
			while (elapsedTime < msToCollectFor) {
				ReactionData reactionData{};
				waitForTimeToPass<ReactionData>(objectsBuffer, reactionData,
					static_cast<uint64_t>(msToCollectFor - static_cast<uint64_t>(elapsedTime)));
				if (filteringFunction(reactionData)) {
					objectReturnData.objects.emplace_back(reactionData);
				}
				if (static_cast<int32_t>(objectReturnData.objects.size()) >= quantityOfObjectsToCollect) {
					break;
				}

				elapsedTime = std::chrono::duration_cast<Milliseconds>(HRClock::now().time_since_epoch()).count() - startingTime;
			}
		}

		inline ~ReactionCollector() {
			if (ReactionCollector::objectsBuffersMap.contains(collectorId)) {
				ReactionCollector::objectsBuffersMap.erase(collectorId);
			}
		};

	  protected:
		UnboundedMessageBlock<ReactionData> objectsBuffer{};
		ReactionCollectorReturnData objectReturnData{};
		ObjectFilter<ReactionData> filteringFunction{};
		int32_t quantityOfObjectsToCollect{};
		std::string collectorId{};
		int32_t msToCollectFor{};
	};

	/// \brief For getting a collection of Messages.
	struct DiscordCoreAPI_Dll GetMessagesData {
		Snowflake beforeThisId{};///< Before this id.
		uint64_t aroundThisId{};///< Around this id.
		Snowflake afterThisId{};///< After this id.
		Snowflake channelId{};///< ChannelData from which to collect the Messages.
		int32_t limit{};///< Limit of Messages to collect.
	};

	/// \brief For getting a Message.
	struct DiscordCoreAPI_Dll GetMessageData {
		Snowflake channelId{};///< The ChannelData from which to collect the MessageData.
		Snowflake id{};///< The id of the Message to collect.
	};

	/// \brief For creating a Message.
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
		Snowflake messageId{};///< Snowflake of the message to be crossposted.
		Snowflake channelId{};///< ChannelData within which to crosspost the Message from.
	};

	/// \brief For editing a Message.
	class DiscordCoreAPI_Dll EditMessageData : public MessageResponseBase {
	  public:
		friend struct Jsonifier::Core<DiscordCoreAPI::EditMessageData>;
		friend class InputEvents;
		friend class Messages;

		EditMessageData(InputEventData dataPackage);

		EditMessageData(RespondToInputEventData dataPackage);

	  protected:
		std::vector<AttachmentData> attachments{};
		Snowflake channelId{};
		Snowflake messageId{};
		int32_t flags{};

		EditMessageData() = default;
	};

	/// \brief For deleting a Message.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	  public:
		TimeStampParse timeStamp{};///< The created-at timeStamp of the original message.
		Snowflake channelId{};///< The channel Snowflake of the Message to delete.
		Snowflake messageId{};///< The message Snowflake of the Message to delete.
		std::string reason{};///< The reason for deleting the MessageData.
		int32_t timeDelay{};///< Number of Milliseconds to wait before deleting the MessageData.
	};

	/// \brief For deleting a bulk of Messages.
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
		std::vector<Snowflake> messageIds{};///< Array of Message ids to delete.
		Snowflake channelId{};///< ChannelData within which to delete the Messages.
		std::string reason{};///< The reason for deleting the Messages.
	};

	/// \brief For getting a collection of pinned Messages.
	struct DiscordCoreAPI_Dll GetPinnedMessagesData {
		Snowflake channelId{};///< The ChannelData from which to collect pinned Messages.
	};

	/// \brief For pinning a single MessageData.
	struct DiscordCoreAPI_Dll PinMessageData {
		Snowflake channelId{};///< The ChannelData within which to pin the MessageData.
		Snowflake messageId{};///< The Message which you would like to pin.
		std::string reason{};///< Reason for pinning this MessageData.
	};

	/// \brief For unpinning a single MessageData.
	struct DiscordCoreAPI_Dll UnpinMessageData {
		Snowflake channelId{};///< The ChannelData within which to unpin the MessageData.
		Snowflake messageId{};///< The Message which you would like to unpin.
		std::string reason{};///< Reason for pinning this MessageData.
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
	/// \brief An interface class for the Message related Discord endpoints.
	class DiscordCoreAPI_Dll Messages {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Collects a collection of Message from the Discord servers
		/// \param dataPackage A GetMessagesData structure.
		/// \returns A CoRoutine containing a std::vector<MessageData>.
		static CoRoutine<std::vector<MessageData>> getMessagesAsync(GetMessagesData dataPackage);

		/// \brief Collects a Message from the Discord servers.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> getMessageAsync(GetMessageData dataPackage);

		/// \brief Creates a new MessageData.
		/// \param dataPackage A CreateMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> createMessageAsync(CreateMessageData dataPackage);

		/// \brief Crossposts a message from a News ChannelData to the following Channels.
		/// \param dataPackage A CrosspostMessageData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<MessageData> crosspostMessageAsync(CrosspostMessageData dataPackage);

		/// \brief Edit a Message.
		/// \param dataPackage An EditMessageData structure.
		/// \returns A CoRoutine containing a Message.
		static CoRoutine<MessageData> editMessageAsync(EditMessageData dataPackage);

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
		/// \returns A CoRoutine containing a std::vector<MessageData>.
		static CoRoutine<std::vector<MessageData>> getPinnedMessagesAsync(GetPinnedMessagesData dataPackage);

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

}
