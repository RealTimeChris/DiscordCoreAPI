/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// ReactionEntities.hpp - Header for the Reaction related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ReactionEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	/// \brief For creating a single Reaction.
	struct DiscordCoreAPI_Dll CreateReactionData {
		std::string emojiName{};///< The emoji name of the Reaction to add.
		Snowflake channelId{};///< The Channel for which to add the Reaction.
		Snowflake messageId{};///< The Message on which to add the Reaction.
		Snowflake emojiId{};///< The emoji id of the Reaction to add.
	};

	/// \brief For deleting one's own Reaction.
	struct DiscordCoreAPI_Dll DeleteOwnReactionData {
		std::string emojiName{};///< The emoji name for which to remove the emoji.
		Snowflake channelId{};///< The Channel from which to remove the Reaction.
		Snowflake messageId{};///< The Message from which to remove the Reaction.
		Snowflake emojiId{};///< The emoji id for which to remove the emoji.
	};

	/// \brief For deleting a reaction, by User.
	struct DiscordCoreAPI_Dll DeleteUserReactionData {
		std::string emojiName{};///< The name of which emoji to remove.
		Snowflake channelId{};///< The Channel from which to remove the Reaction.
		Snowflake messageId{};///< The Message from which to remove the Reaction.
		Snowflake emojiId{};///< The id of which emoji to remove.
		Snowflake userId{};///< The User id for whom to remove their emoji.
	};

	/// \brief For getting all of the Reactions.
	struct DiscordCoreAPI_Dll GetReactionsData {
		Snowflake channelId{};///< The Channel from which to acquire the reactors.
		Snowflake messageId{};///< The Message from which to acquire the reactors.
		Snowflake afterId{};///< Get users after this user ID.
		std::string emoji{};///< The emoji name for which to acquire the reactors.
		int32_t limit{};///< The maximum number of reactors to collect.
	};

	/// \brief For deleting all of the Reactions on a particular Message.
	struct DiscordCoreAPI_Dll DeleteAllReactionsData {
		Snowflake channelId{};///< The Channel from which you would like to remove the emoji.
		Snowflake messageId{};///< The Message from which you would like to remove the emoji.
	};

	/// \brief For deleting all of the Reactions of a particular Emoji.
	struct DiscordCoreAPI_Dll DeleteReactionsByEmojiData {
		std::string emojiName{};///< The name of which emoji to remove.
		Snowflake channelId{};///< The Channel from which you would like to remove the emoji.
		Snowflake messageId{};///< The Message from which you would like to remove the emoji.
		Snowflake emojiId{};///< The id of which emoji to remove.
	};

	/// For collecting a list of Emoji from a Guild.
	struct DiscordCoreAPI_Dll GetEmojiListData {
		Snowflake guildId{};///< The id of the chosen Guild.
	};

	/// For collecting a single Guild Emoji.
	struct DiscordCoreAPI_Dll GetGuildEmojiData {
		Snowflake guildId{};///< The id of the chosen Guild.
		Snowflake emojiId{};///< The id of the chosen Emoji
	};

	enum class ImageType {
		Jpg = 0,
		Png = 1,
		Gif = 2,
	};

	/// For creating a new Guild Emoji.
	struct DiscordCoreAPI_Dll CreateGuildEmojiData {
		friend class Reactions;

		std::vector<Id> roles{};///< Roles that can use this Emoji.
		std::string imageFilePath{};///< The image responseData.
		std::string reason{};///< Reason for creating the new Emoji.
		Snowflake guildId{};///< The Guild within which to create the Emoji.
		std::string name{};///< Name of the emoji.
		ImageType type{};///< The type of image being uploaded.

	  protected:
		std::string imageDataFinal{};
	};

	/// For modifying a Guild Emoji.
	struct DiscordCoreAPI_Dll ModifyGuildEmojiData {
		std::vector<Id> roles{};///< Roles that can use this Emoji.
		std::string reason{};///< Reason for modifying the Emoji.
		Snowflake guildId{};///< The Guild within which to modify the Emoji.
		Snowflake emojiId{};///< The id of the Emoji to modify.
		std::string name{};///< Name of the Emoji.
	};

	/// For deleting a Guild Emoji.
	struct DiscordCoreAPI_Dll DeleteGuildEmojiData {
		std::string reason{};///< Reason for deleting the Emoji.
		Snowflake guildId{};///< The Guild within which to delete the Emoji.
		Snowflake emojiId{};///< The id of the Emoji to delete.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// \brief An interface class for the Reaction related Discord endpoints.
	class DiscordCoreAPI_Dll Reactions {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// \brief Creates a Reaction on a Message.
		/// \param dataPackage A CreateReactionData structure.
		/// \returns A CoRoutine containing a Reaction.
		static CoRoutine<Reaction> createReactionAsync(CreateReactionData dataPackage);

		/// \brief Deletes your own Reactions from a given Message.
		/// \param dataPackage A DeleteOwnReactionData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage);

		/// \brief Deletes all of the Reactions by a specific User from a given Message.
		/// \param dataPackage A DeleteUserReactionData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage);

		/// \brief Get a list of users that reacted with this emoji. Returns an array of user objects on success.
		/// \param dataPackage A GetReactionsData structure.
		/// \returns A CoRoutine containing a UserVector.
		static CoRoutine<UserVector> getReactionsAsync(GetReactionsData dataPackage);

		/// \brief Deletes all of the Reactions from a given Message.
		/// \param dataPackage A DeleteAllReactionsData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage);

		/// \brief Deletes all of the Reactions by a specific emoji from a given Message.
		/// \param dataPackage A DeleteReactionsByEmojiData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage);

		/// \brief Collects a list of Guild Emoji from a chosen Guild.
		/// \param dataPackage A GetEmojiListData structure.
		/// \returns A CoRoutine containing a EmojiDataVector.
		static CoRoutine<EmojiDataVector> getEmojiListAsync(GetEmojiListData dataPackage);

		/// \brief Collects a single Guild Emoji from a chosen Guild.
		/// \param dataPackage A GetGuildEmojiData structure.
		/// \returns A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> getGuildEmojiAsync(GetGuildEmojiData dataPackage);

		/// \brief Creates a single Guild Emoji within a chosen Guild.
		/// \param dataPackage A CreateGuildEmojiData structure.
		/// \returns A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> createGuildEmojiAsync(CreateGuildEmojiData dataPackage);

		/// \brief Modifies a single Guild Emoji within a chosen Guild.
		/// \param dataPackage A ModifyGuildEmojiData structure.
		/// \returns A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> modifyGuildEmojiAsync(ModifyGuildEmojiData dataPackage);

		/// \brief Deletes a single Guild Emoji within a chosen Guild.
		/// \param dataPackage A DeleteGuildEmojiData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildEmojiAsync(DeleteGuildEmojiData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
}