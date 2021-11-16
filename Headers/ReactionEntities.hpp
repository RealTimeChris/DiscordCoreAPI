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
		friend class DiscordCoreInternal::ReactionManager;

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
	struct ModifyGuildEmojiData {
		vector<string> roles{};///< Roles that can use this Emoji.
		string guildId{ "" };///< The Guild within which to modify the Emoji.
		string emojiId{ "" };///< The id of the Emoji to modify.
		string reason{ "" };///< Reason for modifying the Emoji.
		string name{ "" };///< Name of the Emoji.		
	};


	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll ReactionManager {
	public:

		friend class DiscordCoreAPI::Reactions;

	protected:

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Reaction> createReactionAsync(DiscordCoreAPI::CreateReactionData dataPackage);

		DiscordCoreAPI::CoRoutine<vector<DiscordCoreAPI::User>> getReactionsAsync(DiscordCoreAPI::GetReactionsData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteOwnReactionAsync(DiscordCoreAPI::DeleteOwnReactionData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteUserReactionAsync(DiscordCoreAPI::DeleteUserReactionData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteAllReactionsAsync(DiscordCoreAPI::DeleteAllReactionsData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteReactionsByEmojiAsync(DiscordCoreAPI::DeleteReactionsByEmojiData dataPackage);

		DiscordCoreAPI::CoRoutine<vector<DiscordCoreAPI::EmojiData>> getEmojiListAsync(DiscordCoreAPI::GetEmojiListData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::EmojiData> getGuildEmojiAsync(DiscordCoreAPI::GetGuildEmojiData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::EmojiData> createGuildEmojiAsync(DiscordCoreAPI::CreateGuildEmojiData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::EmojiData> modifyGuildEmojiAsync(DiscordCoreAPI::ModifyGuildEmojiData dataPackage);
	};
}
