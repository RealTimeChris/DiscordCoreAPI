// ReactionEntities.hpp - Header for the Reaction related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REACTION_ENTITIES_
#define _REACTION_ENTITIES_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "UserEntities.hpp"

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

	/// A single Reaction. \brief A single Reaction.
	class DiscordCoreAPI_Dll Reaction : public ReactionData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Reaction>;
		friend class DiscordCoreInternal::ReactionManagerAgent;
		friend class DiscordCoreInternal::ReactionManager;
		template<typename returnValueType>
		friend class DiscordCoreAPI::CoRoutine;
		friend struct OnReactionAddData;
		friend class DiscordCoreClient;

		Reaction();

		Reaction(ReactionData dataNew);
	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll ReactionManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Reactions;

		ReactionManager();

	protected:

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Reaction> createReactionAsync(DiscordCoreAPI::CreateReactionData dataPackage);

		DiscordCoreAPI::CoRoutine<vector<DiscordCoreAPI::User>> getReactionsAsync(DiscordCoreAPI::GetReactionsData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteOwnReactionAsync(DiscordCoreAPI::DeleteOwnReactionData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteUserReactionAsync(DiscordCoreAPI::DeleteUserReactionData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteAllReactionsAsync(DiscordCoreAPI::DeleteAllReactionsData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteReactionsByEmojiAsync(DiscordCoreAPI::DeleteReactionsByEmojiData dataPackage);
	};
}
#endif
