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
/// MessageEntities.hpp - Header for the message_data related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file MessageEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief Typedef for the message filter.
	template<typename value_type> using object_filter = std::function<bool(value_type)>;

	/// @brief Object collector, for collecting objects from a channel.
	template<typename value_type> class DiscordCoreAPI_Dll object_collector {
	  public:
		/// @brief Object_collector_return_data responseData.c
		struct object_collector_return_data {
			jsonifier::vector<value_type> objects{};///< A vector of collected objects.
		};

		static unordered_map<jsonifier::string, unbounded_message_block<value_type>*> objectsBuffersMap;

		object_collector();

		/// @brief Begin waiting for objects.
		/// @param quantityToCollect maximum quantity of objects to collect before returning the results.
		/// @param msToCollectForNew maximum number of milliseconds to wait for objects before returning the results.
		/// @param filteringFunctionNew a filter function to apply to new objects, where returning "True" from the function results in a object being stored.
		/// @return A object_collector_return_data structure.
		co_routine<object_collector_return_data, false> collectObjects(int32_t quantityToCollect, int32_t msToCollectForNew, object_filter<value_type> filteringFunctionNew);

		void run(std::coroutine_handle<
			typename discord_core_api::co_routine<typename discord_core_api::object_collector<value_type>::object_collector_return_data, false>::promise_type>& coroHandle);

		~object_collector();

	  protected:
		unbounded_message_block<value_type> objectsBuffer{};
		object_collector_return_data objectReturnData{};
		object_filter<value_type> filteringFunction{};
		int32_t quantityOfObjectsToCollect{};
		jsonifier::string collectorId{};
		int32_t msToCollectFor{};
	};

	using message_collector = object_collector<message_data>;

	/// @brief For getting a collection of messages.
	struct get_messages_data {
		snowflake beforeThisId{};///< Before this id.
		snowflake aroundThisId{};///< Around this id.
		snowflake afterThisId{};///< After this id.
		snowflake channelId{};///< channel_data from which to collect the messages.
		int32_t limit{};///< Limit of messages to collect.
	};

	/// @brief For getting a message.
	struct get_message_data {
		snowflake channelId{};///< The channel_data from which to collect the message_data.
		snowflake id{};///< The id of the message to collect.
	};

	/// @brief For creating a message.
	class DiscordCoreAPI_Dll create_message_data : public message_response_base {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class input_events;
		friend class messages;

		create_message_data(const snowflake channelIdNew);

		create_message_data(respond_to_input_event_data dataPackage);

		create_message_data(input_event_data dataPackage);

		create_message_data(message_data dataPackage);

		snowflake channelId{};

		create_message_data() = default;

	  protected:
		jsonifier::vector<attachment_data> attachments{};
		message_reference_data messageReference{};
		jsonifier::vector<jsonifier::string> stickerIds{};
	};

	/// @brief For sending a direct-message.
	class DiscordCoreAPI_Dll send_dmdata : public create_message_data {
	  public:
		friend class input_events;

		send_dmdata(respond_to_input_event_data dataPackage);

	  protected:
		snowflake targetUserId{};
	};

	/// @brief For crossposting a message.
	struct crosspost_message_data {
		snowflake messageId{};///< snowflake of the message to be crossposted.
		snowflake channelId{};///< channel_data within which to crosspost the message from.
	};

	/// @brief For editing a message.
	class DiscordCoreAPI_Dll edit_message_data : public message_response_base {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class input_events;
		friend class messages;

		edit_message_data(input_event_data dataPackage);

		edit_message_data(respond_to_input_event_data dataPackage);

	  protected:
		jsonifier::vector<attachment_data> attachments{};
		snowflake channelId{};
		snowflake messageId{};
		int32_t flags{};

		edit_message_data() = default;
	};

	/// @brief For deleting a message.
	struct DiscordCoreAPI_Dll delete_message_data {
	  public:
		delete_message_data(const message_data& messageToDelete);
		time_stamp timeStamp{};///< The created-at timeStamp of the original message.
		snowflake channelId{};///< The channel snowflake of the message to delete.
		snowflake messageId{};///< The message snowflake of the message to delete.
		jsonifier::string reason{};///< The reason for deleting the message_data.
		int32_t timeDelay{};///< Number of milliseconds to wait before deleting the message_data.
	};

	/// @brief For deleting a bulk of messages.
	struct delete_messages_bulk_data {
		jsonifier::vector<snowflake> messageIds{};///< Array of message ids to delete.
		snowflake channelId{};///< channel_data within which to delete the messages.
		jsonifier::string reason{};///< The reason for deleting the messages.
	};

	/// @brief For getting a collection of pinned messages.
	struct get_pinned_messages_data {
		snowflake channelId{};///< The channel_data from which to collect pinned messages.
	};

	/// @brief For pinning a single message_data.
	struct pin_message_data {
		snowflake channelId{};///< The channel_data within which to pin the message_data.
		snowflake messageId{};///< The message which you would like to pin.
		jsonifier::string reason{};///< Reason for pinning this message_data.
	};

	/// @brief For unpinning a single message_data.
	struct unpin_message_data {
		snowflake channelId{};///< The channel_data within which to unpin the message_data.
		snowflake messageId{};///< The message which you would like to unpin.
		jsonifier::string reason{};///< Reason for pinning this message_data.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the message_data related discord endpoints.
	class DiscordCoreAPI_Dll messages {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Collects a collection of message from the discord servers
		/// @param dataPackage a get_messages_data structure.
		/// @return A co_routine containing a jsonifier::vector<message_data>.
		static co_routine<jsonifier::vector<message_data>> getMessagesAsync(get_messages_data dataPackage);

		/// @brief Collects a message from the discord servers.
		/// @param dataPackage a get_message_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> getMessageAsync(get_message_data dataPackage);

		/// @brief Creates a new message_data.
		/// @param dataPackage a create_message_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> createMessageAsync(create_message_data dataPackage);

		/// @brief Crossposts a message from a news channel_data to the following channels.
		/// @param dataPackage a crosspost_message_data structure.
		/// @return A co_routine containing void.
		static co_routine<message_data> crosspostMessageAsync(crosspost_message_data dataPackage);

		/// @brief Edit a message.
		/// @param dataPackage an edit_message_data structure.
		/// @return A co_routine containing a message.
		static co_routine<message_data> editMessageAsync(edit_message_data dataPackage);

		/// @brief Deletes a message.
		/// @param dataPackage a delete_message_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> deleteMessageAsync(delete_message_data dataPackage);

		/// @brief Deletes a collection of messages.
		/// @param dataPackage a delete_messages_bulk_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> deleteMessagesBulkAsync(delete_messages_bulk_data dataPackage);

		/// @brief Collects a collection of pinned messages from the discord servers.
		/// @param dataPackage a get_pinned_messages_data structure.
		/// @return A co_routine containing a jsonifier::vector<message_data>.
		static co_routine<jsonifier::vector<message_data>> getPinnedMessagesAsync(get_pinned_messages_data dataPackage);

		/// @brief Pins a message to a given channel.
		/// @param dataPackage a pin_message_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> pinMessageAsync(pin_message_data dataPackage);

		/// @brief Unpins a message from a given channel.
		/// @param dataPackage an unpin_message_data structure.
		/// @return A co_routine containing void.
		static co_routine<void> unpinMessageAsync(unpin_message_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};
	/**@}*/

}
