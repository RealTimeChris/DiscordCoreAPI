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
/// ReactionEntities.hpp - Header for the ReactionData related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ReactionEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/MessageEntities.hpp>

namespace DiscordCoreAPI {

	using ReactionCollector = ObjectCollector<ReactionData>;

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For creating a single ReactionData.
	struct CreateReactionData {
		std::string emojiName{};///< The emoji name of the ReactionData to add.
		Snowflake channelId{};///< The ChannelData for which to add the ReactionData.
		Snowflake messageId{};///< The Message on which to add the ReactionData.
		Snowflake emojiId{};///< The emoji id of the ReactionData to add.
	};

	/// @brief For deleting one's own ReactionData.
	struct DeleteOwnReactionData {
		std::string emojiName{};///< The emoji name for which to remove the emoji.
		Snowflake channelId{};///< The ChannelData from which to remove the ReactionData.
		Snowflake messageId{};///< The Message from which to remove the ReactionData.
		Snowflake emojiId{};///< The emoji id for which to remove the emoji.
	};

	/// @brief For deleting a reaction, by UserData.
	struct DeleteUserReactionData {
		std::string emojiName{};///< The name of which emoji to remove.
		Snowflake channelId{};///< The ChannelData from which to remove the ReactionData.
		Snowflake messageId{};///< The Message from which to remove the ReactionData.
		Snowflake emojiId{};///< The id of which emoji to remove.
		Snowflake userId{};///< The UserData id for whom to remove their emoji.
	};

	/// @brief For getting all of the Reactions.
	struct GetReactionsData {
		Snowflake channelId{};///< The ChannelData from which to acquire the reactors.
		Snowflake messageId{};///< The Message from which to acquire the reactors.
		Snowflake afterId{};///< Get users after this user ID.
		std::string emoji{};///< The emoji name for which to acquire the reactors.
		int32_t limit{};///< The maximum number of reactors to collect.
	};

	/// @brief For deleting all of the Reactions on a particular MessageData.
	struct DeleteAllReactionsData {
		Snowflake channelId{};///< The ChannelData from which you would like to remove the emoji.
		Snowflake messageId{};///< The Message from which you would like to remove the emoji.
	};

	/// @brief For deleting all of the Reactions of a particular Emoji.
	struct DeleteReactionsByEmojiData {
		std::string emojiName{};///< The name of which emoji to remove.
		Snowflake channelId{};///< The ChannelData from which you would like to remove the emoji.
		Snowflake messageId{};///< The Message from which you would like to remove the emoji.
		Snowflake emojiId{};///< The id of which emoji to remove.
	};

	/// For collecting a list of Emoji from a Guild.
	struct GetEmojiListData {
		Snowflake guildId{};///< The id of the chosen Guild.
	};

	/// For collecting a single Guild Emoji.
	struct GetGuildEmojiData {
		Snowflake guildId{};///< The id of the chosen Guild.
		Snowflake emojiId{};///< The id of the chosen Emoji
	};

	enum class ImageType {
		Jpg = 0,
		Png = 1,
		Gif = 2,
	};

	/// For creating a new Guild Emoji.
	struct CreateGuildEmojiData {
		friend struct Jsonifier::Core<CreateGuildEmojiData>;
		friend class Reactions;

		std::vector<Snowflake> roles{};///< Roles that can use this Emoji.
		std::string imageFilePath{};///< The image responseData.
		std::string reason{};///< Reason for creating the new Emoji.
		Snowflake guildId{};///< The Guild within which to create the Emoji.
		std::string name{};///< Name of the emoji.
		ImageType type{};///< The type of image being uploaded.

	  protected:
		std::string imageDataFinal{};
	};

	/// For modifying a Guild Emoji.
	struct ModifyGuildEmojiData {
		std::vector<Snowflake> roles{};///< Roles that can use this Emoji.
		std::string reason{};///< Reason for modifying the Emoji.
		Snowflake guildId{};///< The Guild within which to modify the Emoji.
		Snowflake emojiId{};///< The id of the Emoji to modify.
		std::string name{};///< Name of the Emoji.
	};

	/// For deleting a Guild Emoji.
	struct DeleteGuildEmojiData {
		std::string reason{};///< Reason for deleting the Emoji.
		Snowflake guildId{};///< The Guild within which to delete the Emoji.
		Snowflake emojiId{};///< The id of the Emoji to delete.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the ReactionData related Discord endpoints.
	class DiscordCoreAPI_Dll Reactions {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Creates a ReactionData on a Message.
		/// @param dataPackage A CreateReactionData structure.
		/// @return A CoRoutine containing a ReactionData.
		static CoRoutine<ReactionData> createReactionAsync(CreateReactionData dataPackage);

		/// @brief Deletes your own Reactions from a given MessageData.
		/// @param dataPackage A DeleteOwnReactionData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage);

		/// @brief Deletes all of the Reactions by a specific UserData from a given MessageData.
		/// @param dataPackage A DeleteUserReactionData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage);

		/// @brief Get a list of users that reacted with this emoji. Returns an array of user objects on success.
		/// @param dataPackage A GetReactionsData structure.
		/// @return A CoRoutine containing a std::vector<UserData>.
		static CoRoutine<std::vector<UserData>> getReactionsAsync(GetReactionsData dataPackage);

		/// @brief Deletes all of the Reactions from a given MessageData.
		/// @param dataPackage A DeleteAllReactionsData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage);

		/// @brief Deletes all of the Reactions by a specific emoji from a given MessageData.
		/// @param dataPackage A DeleteReactionsByEmojiData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage);

		/// @brief Collects a list of Guild Emoji from a chosen Guild.
		/// @param dataPackage A GetEmojiListData structure.
		/// @return A CoRoutine containing a std::vector<EmojiData>.
		static CoRoutine<std::vector<EmojiData>> getEmojiListAsync(GetEmojiListData dataPackage);

		/// @brief Collects a single Guild Emoji from a chosen Guild.
		/// @param dataPackage A GetGuildEmojiData structure.
		/// @return A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> getGuildEmojiAsync(GetGuildEmojiData dataPackage);

		/// @brief Creates a single Guild Emoji within a chosen Guild.
		/// @param dataPackage A CreateGuildEmojiData structure.
		/// @return A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> createGuildEmojiAsync(CreateGuildEmojiData dataPackage);

		/// @brief Modifies a single Guild Emoji within a chosen Guild.
		/// @param dataPackage A ModifyGuildEmojiData structure.
		/// @return A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> modifyGuildEmojiAsync(ModifyGuildEmojiData dataPackage);

		/// @brief Deletes a single Guild Emoji within a chosen Guild.
		/// @param dataPackage A DeleteGuildEmojiData structure.
		/// @return A CoRoutine containing void.
		static CoRoutine<void> deleteGuildEmojiAsync(DeleteGuildEmojiData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};
	/**@}*/
}
