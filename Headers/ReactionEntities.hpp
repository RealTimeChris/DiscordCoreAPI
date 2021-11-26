// ReactionEntities.hpp - Header for the Reaction related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "JSONIfier.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// Create Reaction data. \brief Create Reaction data.
	struct DiscordCoreAPI_Dll CreateReactionData {
		string channelId{ "" };///< The Channel for which to add the Reaction.
		string messageId{ "" };///< The Message on which to add the Reaction.
		string emojiName{ "" };///< The emoji name of the Reaction to add.
		string emojiId{ "" };///< The emoji id of the Reaction to add.
	};

	/// Get Reactions Data. \brief Get Reactions Data.
	struct DiscordCoreAPI_Dll GetReactionsData {
		string channelId{ "" };///< The Channel from which to acquire the reactors.
		string messageId{ "" };///< The Message from which to acquire the reactors.
		string afterId{ "" };///< Get users after this user ID.
		string emoji{ "" };///< The emoji name for which to acquire the reactors.
		__int32 limit{ 0 };///< The maximum number of reactors to collect.		
	};

	/// Delete own Reaction data. \brief Delete own Reaction data.
	struct DiscordCoreAPI_Dll DeleteOwnReactionData {
		string channelId{ "" }; ///< The Channel from which to remove the Reaction.
		string messageId{ "" };///< The Message from which to remove the Reaction.
		string emojiName{ "" };///< The emoji name for which to remove the emoji.
		string emojiId{ "" };///< The emoji id for which to remove the emoji.
	};

	/// Delete User Reaction data. \brief Delete User Reaction data.
	struct DiscordCoreAPI_Dll DeleteUserReactionData {
		string channelId{ "" };///< The Channel from which to remove the Reaction.
		string messageId{ "" };///< The Message from which to remove the Reaction.
		string emojiName{ "" };///< The name of which emoji to remove.
		string emojiId{ "" };///< The id of which emoji to remove.
		string userId{ "" };///< The User id for whom to remove their emoji.
	};

	/// Delete all Reaction data. \brief Delete all Reaction data.
	struct DiscordCoreAPI_Dll DeleteAllReactionsData {
		string channelId{ "" };///< The Channel from which you would like to remove the emoji.
		string messageId{ "" };///< The Message from which you would like to remove the emoji.
	};

	/// Delete Reactions by emoji data. \brief Delete Reactions by emoji data.
	struct DiscordCoreAPI_Dll DeleteReactionsByEmojiData {
		string channelId{ "" };///< The Channel from which you would like to remove the emoji.
		string messageId{ "" };///< The Message from which you would like to remove the emoji.
		string emojiName{ "" };///< The name of which emoji to remove.
		string emojiId{ "" };///< The id of which emoji to remove.
	};

	/// For collecting a list of Emoji from a Guild.
	struct DiscordCoreAPI_Dll GetEmojiListData {
		string guildId{ "" };///< The id of the chosen Guild.
	};

	/// For collecting a single Guild Emoji.
	struct DiscordCoreAPI_Dll GetGuildEmojiData {
		string guildId{ "" };///< The id of the chosen Guild.
		string emojiId{ "" };///< The id of the chosen Emoji
	};

	enum class ImageType {
		JPG = 0,
		PNG = 1,
		GIF = 2,
	};

	/// For creating a new Guild Emoji.
	struct DiscordCoreAPI_Dll CreateGuildEmojiData {
		friend string DiscordCoreInternal::JSONIFY(CreateGuildEmojiData dataPackage);
		friend class Reactions;

		vector<unsigned __int8> imageData{};///< The image data.
		vector<string> roles{};///< Roles that can use this Emoji.
		string guildId{ "" };///< The Guild within which to create the Emoji.
		string reason{ "" };///< Reason for creating the new Emoji.
		string name{ "" };///< Name of the emoji.
		ImageType type{};///< The type of image being uploaded.		
	protected:
		string imageDataFinal{};
	};

	/// For modifying a Guild Emoji.
	struct DiscordCoreAPI_Dll ModifyGuildEmojiData {
		vector<string> roles{};///< Roles that can use this Emoji.
		string guildId{ "" };///< The Guild within which to modify the Emoji.
		string emojiId{ "" };///< The id of the Emoji to modify.
		string reason{ "" };///< Reason for modifying the Emoji.
		string name{ "" };///< Name of the Emoji.		
	};

	/// For deleting a Guild Emoji.
	struct DiscordCoreAPI_Dll DeleteGuildEmojiData {
		string guildId{ "" };///< The Guild within which to delete the Emoji.
		string emojiId{ "" };///< The id of the Emoji to delete.
		string reason{ "" };///< Reason for deleting the Emoji.
	};

	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Reaction related Discord endpoints. \brief An interface class for the Reaction related Discord endpoints.
	class DiscordCoreAPI_Dll Reactions {
	public:

		/// Creates a Reaction on a Message. \brief Creates a Reaction on a Message.
		/// \param dataPackage A CreateReactionData structure.
		/// \returns A CoRoutine containing a Reaction.
		static CoRoutine<Reaction> createReactionAsync(CreateReactionData dataPackage);

		/// Get a list of users that reacted with this emoji. Returns an array of user objects on success. \brief Get a list of users that reacted with this emoji. Returns an array of user objects on success.
		/// \param dataPackage A GetReactionsData structure.
		/// \returns A CoRoutine containing a vector of Users.
		static CoRoutine<vector<User>> getReactionsAsync(GetReactionsData dataPackage);

		/// Deletes all of the Reactions from a given Message. \brief Deletes all of the Reactions from a given Message.
		/// \param dataPackage A DeleteAllReactionsData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage);

		/// Deletes your own Reactions from a given Message. \brief Deletes your own Reactions from a given Message.
		/// \param dataPackage A DeleteOwnReactionData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage);

		/// Deletes all of the Reactions by a specific emoji from a given Message. \brief Deletes all of the Reactions by a specific emoji from a given Message. 
		/// \param dataPackage A DeleteReactionsByEmojiData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage);

		/// Deletes all of the Reactions by a specific User from a given Message. \brief Deletes all of the Reactions by a specific User from a given Message. 
		/// \param dataPackage A DeleteUserReactionData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage);

		/// Collects a list of Guild Emoji from a chosen Guild. \brief Collects a list of Guild Emoji from a chosen Guild.
		/// \param dataPackage A GetEmojiListData structure.
		/// \returns A CoRoutine containing a vector of EmojiData.
		static CoRoutine<vector<EmojiData>> getEmojiListAsync(GetEmojiListData dataPackage);

		/// Collects a single Guild Emoji from a chosen Guild. \brief Collects a single Guild Emoji from a chosen Guild.
		/// \param dataPackage A GetGuildEmojiData structure.
		/// \returns A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> getGuildEmojiAsync(GetGuildEmojiData dataPackage);

		/// Creates a single Guild Emoji within a chosen Guild. \brief Creates a single Guild Emoji within a chosen Guild.
		/// \param dataPackage A CreateGuildEmojiData structure.
		/// \returns A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> createGuildEmojiAsync(CreateGuildEmojiData dataPackage);

		/// Modifies a single Guild Emoji within a chosen Guild. \brief Modifies a single Guild Emoji within a chosen Guild.
		/// \param dataPackage A ModifyGuildEmojiData structure.
		/// \returns A CoRoutine containing an EmojiData.
		static CoRoutine<EmojiData> modifyGuildEmojiAsync(ModifyGuildEmojiData dataPackage);

		/// Deletes a single Guild Emoji within a chosen Guild. \brief Deletes a single Guild Emoji within a chosen Guild.
		/// \param dataPackage A DeleteGuildEmojiData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildEmojiAsync(DeleteGuildEmojiData dataPackage);
	};
	/**@}*/
}