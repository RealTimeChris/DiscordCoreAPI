// ReactionEntities.hpp - Header for the Reaction related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include <FoundationEntities.hpp>
#include <UserEntities.hpp>
#include <JSONIfier.hpp>

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// For creating a single Reaction. \brief For creating a single Reaction.
	struct DiscordCoreAPI_Dll CreateReactionData {
		std::string channelId{ "" };///< The Channel for which to add the Reaction.
		std::string messageId{ "" };///< The Message on which to add the Reaction.
		std::string emojiName{ "" };///< The emoji name of the Reaction to add.
		std::string emojiId{ "" };///< The emoji id of the Reaction to add.
	};

	/// For deleting one's own Reaction. \brief For deleting one's own Reaction.
	struct DiscordCoreAPI_Dll DeleteOwnReactionData {
		std::string channelId{ "" };///< The Channel from which to remove the Reaction.
		std::string messageId{ "" };///< The Message from which to remove the Reaction.
		std::string emojiName{ "" };///< The emoji name for which to remove the emoji.
		std::string emojiId{ "" };///< The emoji id for which to remove the emoji.
	};

	/// For deleting a reaction, by User. \brief For deleting a reaction, by User.
	struct DiscordCoreAPI_Dll DeleteUserReactionData {
		std::string channelId{ "" };///< The Channel from which to remove the Reaction.
		std::string messageId{ "" };///< The Message from which to remove the Reaction.
		std::string emojiName{ "" };///< The name of which emoji to remove.
		std::string emojiId{ "" };///< The id of which emoji to remove.
		std::string userId{ "" };///< The User id for whom to remove their emoji.
	};

	/// For getting all of the Reactions. \brief For getting all of the Reactions.
	struct DiscordCoreAPI_Dll GetReactionsData {
		std::string channelId{ "" };///< The Channel from which to acquire the reactors.
		std::string messageId{ "" };///< The Message from which to acquire the reactors.
		std::string afterId{ "" };///< Get users after this user ID.
		std::string emoji{ "" };///< The emoji name for which to acquire the reactors.
		int32_t limit{ 0 };///< The maximum number of reactors to collect.		
	};
	
	/// For deleting all of the Reactions on a particular Message. \brief For deleting all of the Reactions on a particular Message.
	struct DiscordCoreAPI_Dll DeleteAllReactionsData {
		std::string channelId{ "" };///< The Channel from which you would like to remove the emoji.
		std::string messageId{ "" };///< The Message from which you would like to remove the emoji.
	};

	/// For deleting all of the Reactions of a particular Emoji. \brief For deleting all of the Reactions of a particular Emoji.
	struct DiscordCoreAPI_Dll DeleteReactionsByEmojiData {
		std::string channelId{ "" };///< The Channel from which you would like to remove the emoji.
		std::string messageId{ "" };///< The Message from which you would like to remove the emoji.
		std::string emojiName{ "" };///< The name of which emoji to remove.
		std::string emojiId{ "" };///< The id of which emoji to remove.
	};

	/// For collecting a list of Emoji from a Guild.
	struct DiscordCoreAPI_Dll GetEmojiListData {
		std::string guildId{ "" };///< The id of the chosen Guild.
	};

	/// For collecting a single Guild Emoji.
	struct DiscordCoreAPI_Dll GetGuildEmojiData {
		std::string guildId{ "" };///< The id of the chosen Guild.
		std::string emojiId{ "" };///< The id of the chosen Emoji
	};

	enum class ImageType {
		Jpg = 0,
		Png = 1,
		Gif = 2,
	};

	/// For creating a new Guild Emoji.
	struct DiscordCoreAPI_Dll CreateGuildEmojiData {
		friend std::string DiscordCoreInternal::JSONIFY(CreateGuildEmojiData dataPackage);
		friend Reactions;

		std::vector<uint8_t> imageData{};///< The image responseData.
		std::vector<std::string> roles{};///< Roles that can use this Emoji.
		std::string guildId{ "" };///< The Guild within which to create the Emoji.
		std::string reason{ "" };///< Reason for creating the new Emoji.
		std::string name{ "" };///< Name of the emoji.
		ImageType type{};///< The type of image being uploaded.		
	protected:
		std::string imageDataFinal{};
	};

	/// For modifying a Guild Emoji.
	struct DiscordCoreAPI_Dll ModifyGuildEmojiData {
		std::vector<std::string> roles{};///< Roles that can use this Emoji.
		std::string guildId{ "" };///< The Guild within which to modify the Emoji.
		std::string emojiId{ "" };///< The id of the Emoji to modify.
		std::string reason{ "" };///< Reason for modifying the Emoji.
		std::string name{ "" };///< Name of the Emoji.		
	};

	/// For deleting a Guild Emoji.
	struct DiscordCoreAPI_Dll DeleteGuildEmojiData {
		std::string guildId{ "" };///< The Guild within which to delete the Emoji.
		std::string emojiId{ "" };///< The id of the Emoji to delete.
		std::string reason{ "" };///< Reason for deleting the Emoji.
	};

	/// A single Reaction. \brief A single Reaction.
	class DiscordCoreAPI_Dll Reaction : public ReactionData {
	public:

		Reaction();

		Reaction(ReactionData dataNew);
	};

	/**@}*/

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class DiscordCoreAPI_Dll for the Reaction related Discord endpoints. \brief An interface class DiscordCoreAPI_Dll for the Reaction related Discord endpoints.
	class DiscordCoreAPI_Dll Reactions {
	public:

		static void initialize(DiscordCoreInternal::HttpClient*);

		/// Creates a Reaction on a Message. \brief Creates a Reaction on a Message.
		/// \param dataPackage A CreateReactionData structure.
		/// \returns A CoRoutine containing a Reaction.
		static CoRoutine<Reaction> createReactionAsync(CreateReactionData dataPackage);

		/// Deletes your own Reactions from a given Message. \brief Deletes your own Reactions from a given Message.
		/// \param dataPackage A DeleteOwnReactionData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage);

		/// Deletes all of the Reactions by a specific User from a given Message. \brief Deletes all of the Reactions by a specific User from a given Message. 
		/// \param dataPackage A DeleteUserReactionData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage);

		/// Get a list of users that reacted with this emoji. Returns an array of user objects on success. \brief Get a list of users that reacted with this emoji. Returns an array of user objects on success.
		/// \param dataPackage A GetReactionsData structure.
		/// \returns A CoRoutine containing a std::vector<User>.
		static CoRoutine<std::vector<User>> getReactionsAsync(GetReactionsData dataPackage);

		/// Deletes all of the Reactions from a given Message. \brief Deletes all of the Reactions from a given Message.
		/// \param dataPackage A DeleteAllReactionsData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage);

		/// Deletes all of the Reactions by a specific emoji from a given Message. \brief Deletes all of the Reactions by a specific emoji from a given Message. 
		/// \param dataPackage A DeleteReactionsByEmojiData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage);		

		/// Collects a list of Guild Emoji from a chosen Guild. \brief Collects a list of Guild Emoji from a chosen Guild.
		/// \param dataPackage A GetEmojiListData structure.
		/// \returns A CoRoutine containing a std::vector<EmojiData>.
		static CoRoutine<std::vector<EmojiData>> getEmojiListAsync(GetEmojiListData dataPackage);

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

	protected:

		static DiscordCoreInternal::HttpClient* httpClient;
	};
	/**@}*/
}
