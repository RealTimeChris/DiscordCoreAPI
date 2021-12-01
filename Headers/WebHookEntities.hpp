// WebHookEntities.hpp - The header file for WebHook relate stuff.
// Dec 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For creating a new WebHook. \brief For creating a new WebHook.
	struct DiscordCoreAPI_Dll CreateWebHookData{
		vector<uint8_t>avatar{};///< Image for the default webhook avatar.
		string channelId{ "" };///< The Channel within which to create the WebHook.
		string name{ "" };///< Name of the webhook(1 - 80 characters).
	};

	/// For collecting a list of WebHooks from a chosen Channel. \brief For collecting a list of WebHooks from a chosen Channel.
	struct DiscordCoreAPI_Dll GetChannelWebHooksData {
		string channelId{ "" };///< The Channel from which to collect the WebHooks.
	};

	/// For collecting a list of WebHooks from a chosen Guild. \brief For collecting a list of WebHooks from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildWebHooksData {
		string guildId{ "" };///< The Guild from which to collect the WebHooks.
	};

	/// A single WebHook. \brief A single WebHook.
	class DiscordCoreAPI_Dll WebHook :public WebHookData {
	public:
		WebHook();

		WebHook(WebHookData dataPackage);

		virtual ~WebHook() {};
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the WebHook related Discord endpoints. \brief An interface class for the WebHook related Discord endpoints;
	class DiscordCoreAPI_Dll WebHooks {
	public:

		/// Creates a new WebHook. \brief Creates a new WebHook.
		/// \param dataPackage A GetMessageData structure.
		/// \returns A CoRoutine containing a WebHook.
		static CoRoutine<WebHook> createWebHookAsync(CreateWebHookData dataPackage);

		/// Collects a list of WebHooks from a chosen Channel. \brief Collects a list of WebHooks from a chosen Channel.
		/// \param dataPackage A GetChannelWebHooksData structure.
		/// \returns A CoRoutine containing a vector<WebHook>.
		static CoRoutine<vector<WebHook>> getChannelWebHooksAsync(GetChannelWebHooksData dataPackage);

		/// Collects a list of WebHooks from a chosen Guild. \brief Collects a list of WebHooks from a chosen Guild.
		/// \param dataPackage A GetGuildWebHooksData structure.
		/// \returns A CoRoutine containing a vector<WebHook>.
		static CoRoutine<vector<WebHook>> getGuildWebHooksAsync(GetGuildWebHooksData dataPackage);

	};
	/**@}*/
	
}
