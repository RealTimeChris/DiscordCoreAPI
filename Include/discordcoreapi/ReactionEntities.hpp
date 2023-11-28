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
/// ReactionEntities.hpp - Header for the reaction_data related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ReactionEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>

namespace discord_core_api {

	using reaction_collector = object_collector<reaction_data>;

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For creating a single reaction_data.
	struct create_reaction_data {
		jsonifier::string emojiName{};///< The emoji name of the reaction_data to add.
		snowflake channelId{};///< The channel_data for which to add the reaction_data.
		snowflake messageId{};///< The message on which to add the reaction_data.
		snowflake emojiId{};///< The emoji id of the reaction_data to add.
	};

	/// @brief For deleting one's own reaction_data.
	struct delete_own_reaction_data {
		jsonifier::string emojiName{};///< The emoji name for which to remove the emoji.
		snowflake channelId{};///< The channel_data from which to remove the reaction_data.
		snowflake messageId{};///< The message from which to remove the reaction_data.
		snowflake emojiId{};///< The emoji id for which to remove the emoji.
	};

	/// @brief For deleting a reaction, by user_data.
	struct delete_user_reaction_data {
		jsonifier::string emojiName{};///< The name of which emoji to remove.
		snowflake channelId{};///< The channel_data from which to remove the reaction_data.
		snowflake messageId{};///< The message from which to remove the reaction_data.
		snowflake emojiId{};///< The id of which emoji to remove.
		snowflake userId{};///< The user_data id for whom to remove their emoji.
	};

	/// @brief For getting all of the reactions.
	struct get_reactions_data {
		snowflake channelId{};///< The channel_data from which to acquire the reactors.
		snowflake messageId{};///< The message from which to acquire the reactors.
		snowflake afterId{};///< Get users after this user id.
		jsonifier::string emoji{};///< The emoji name for which to acquire the reactors.
		int32_t limit{};///< The maximum number of reactors to collect.
	};

	/// @brief For deleting all of the reactions on a particular message_data.
	struct delete_all_reactions_data {
		snowflake channelId{};///< The channel_data from which you would like to remove the emoji.
		snowflake messageId{};///< The message from which you would like to remove the emoji.
	};

	/// @brief For deleting all of the reactions of a particular emoji.
	struct delete_reactions_by_emoji_data {
		jsonifier::string emojiName{};///< The name of which emoji to remove.
		snowflake channelId{};///< The channel_data from which you would like to remove the emoji.
		snowflake messageId{};///< The message from which you would like to remove the emoji.
		snowflake emojiId{};///< The id of which emoji to remove.
	};

	/// for collecting a list of emoji from a guild.
	struct get_emoji_list_data {
		snowflake guildId{};///< The id of the chosen guild.
	};

	/// for collecting a single guild emoji.
	struct get_guild_emoji_data {
		snowflake guildId{};///< The id of the chosen guild.
		snowflake emojiId{};///< The id of the chosen emoji
	};

	enum class image_type {
		jpg = 0,
		png = 1,
		gif = 2,
	};

	/// for creating a new guild emoji.
	struct create_guild_emoji_data {
		template<typename value_type> friend struct jsonifier::core;
		friend class reactions;

		jsonifier::vector<snowflake> roles{};///< Roles that can use this emoji.
		jsonifier::string imageFilePath{};///< The image responseData.
		jsonifier::string reason{};///< Reason for creating the new emoji.
		snowflake guildId{};///< The guild within which to create the emoji.
		jsonifier::string name{};///< Name of the emoji.
		image_type type{};///< The type of image being uploaded.

	  protected:
		jsonifier::string imageDataFinal{};
	};

	/// for modifying a guild emoji.
	struct modify_guild_emoji_data {
		jsonifier::vector<snowflake> roles{};///< Roles that can use this emoji.
		jsonifier::string reason{};///< Reason for modifying the emoji.
		snowflake guildId{};///< The guild within which to modify the emoji.
		snowflake emojiId{};///< The id of the emoji to modify.
		jsonifier::string name{};///< Name of the emoji.
	};

	/// for deleting a guild emoji.
	struct delete_guild_emoji_data {
		jsonifier::string reason{};///< Reason for deleting the emoji.
		snowflake guildId{};///< The guild within which to delete the emoji.
		snowflake emojiId{};///< The id of the emoji to delete.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the reaction_data related discord endpoints.
	class DiscordCoreAPI_Dll reactions {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Creates a reaction_data on a message.
		/// @param dataPackage a create_reaction_data structure.
		/// @return a co_routine containing a reaction_data.
		static co_routine<reaction_data> createReactionAsync(create_reaction_data dataPackage);

		/// @brief Deletes your own reactions from a given message_data.
		/// @param dataPackage a delete_own_reaction_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteOwnReactionAsync(delete_own_reaction_data dataPackage);

		/// @brief Deletes all of the reactions by a specific user_data from a given message_data.
		/// @param dataPackage a delete_user_reaction_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteUserReactionAsync(delete_user_reaction_data dataPackage);

		/// @brief Get a list of users that reacted with this emoji. returns An array of user objects on success.
		/// @param dataPackage a get_reactions_data structure.
		/// @return a co_routine containing a jsonifier::vector<user_data>.
		static co_routine<jsonifier::vector<user_data>> getReactionsAsync(get_reactions_data dataPackage);

		/// @brief Deletes all of the reactions from a given message_data.
		/// @param dataPackage a delete_all_reactions_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteAllReactionsAsync(delete_all_reactions_data dataPackage);

		/// @brief Deletes all of the reactions by a specific emoji from a given message_data.
		/// @param dataPackage a delete_reactions_by_emoji_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteReactionsByEmojiAsync(delete_reactions_by_emoji_data dataPackage);

		/// @brief Collects a list of guild emoji from a chosen guild.
		/// @param dataPackage a get_emoji_list_data structure.
		/// @return a co_routine containing a jsonifier::vector<emoji_data>.
		static co_routine<jsonifier::vector<emoji_data>> getEmojiListAsync(get_emoji_list_data dataPackage);

		/// @brief Collects a single guild emoji from a chosen guild.
		/// @param dataPackage a get_guild_emoji_data structure.
		/// @return a co_routine containing an emoji_data.
		static co_routine<emoji_data> getGuildEmojiAsync(get_guild_emoji_data dataPackage);

		/// @brief Creates a single guild emoji within a chosen guild.
		/// @param dataPackage a create_guild_emoji_data structure.
		/// @return a co_routine containing an emoji_data.
		static co_routine<emoji_data> createGuildEmojiAsync(create_guild_emoji_data dataPackage);

		/// @brief Modifies a single guild emoji within a chosen guild.
		/// @param dataPackage a modify_guild_emoji_data structure.
		/// @return a co_routine containing an emoji_data.
		static co_routine<emoji_data> modifyGuildEmojiAsync(modify_guild_emoji_data dataPackage);

		/// @brief Deletes a single guild emoji within a chosen guild.
		/// @param dataPackage a delete_guild_emoji_data structure.
		/// @return a co_routine containing void.
		static co_routine<void> deleteGuildEmojiAsync(delete_guild_emoji_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};
	/**@}*/
}
