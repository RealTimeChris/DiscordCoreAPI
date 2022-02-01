// UserEntities.hpp - Header for User related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#ifndef USER_ENTITIES
#define USER_ENTITIES

#include "FoundationEntities.hpp"
#include "WebSocketEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	/// For updating a User's presence. \brief For updating a User's presence.
	struct DiscordCoreAPI_Dll UpdatePresenceData {
		std::vector<ActivityData> activities{};///< A std::vector of activities.
		std::string status{ "" };///< Current status.
		int64_t since{ 0 };///< When was the activity started?
		bool afk{ false };///< Are we afk.
	};

	/// For adding a user to a group Dm. \brief For adding a user to a group Dm.
	struct DiscordCoreAPI_Dll AddRecipientToGroupDMData {
		std::string token{ "" };///< The user's access token.
		std::string nick{ "" };///< The user's nickname.
		std::string channelId{ "" };///< The channel Id of the Dm.
		std::string userId{ "" };///< The user's Id.
	};

	/// For removing a User from a group Dm. \brief For removing a User from a group Dm.
	struct DiscordCoreAPI_Dll RemoveRecipientFromGroupDMData {
		std::string channelId{ "" };///< The channel Id of the Dm.
		std::string userId{ "" };///< The user's Id.
	};

	/// For updating the bot's current voice state. \brief For updating the bot's current voice state.
	struct DiscordCoreAPI_Dll ModifyCurrentUserVoiceStateData {
		std::string requestToSpeakTimestamp{ "" };///< ISO8601 timestamp.
		std::string channelId{ "" };///< The id of the channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		std::string guildId{ "" };///< The Guild within which to update the bot's voice state.
	};

	/// For modifying a User's voice state. \brief For modifying a User's voice state.
	struct DiscordCoreAPI_Dll ModifyUserVoiceStateData {
		std::string channelId{ "" };///< The id of the channel the user is currently in.
		bool suppress{ false };///< Toggles the user's suppress state.
		std::string guildId{ "" };///< The Guild within which you would like to modify their voice state.
		std::string userId{ "" };///< The user for which you would like to modify the voice state of.
	};

	/// For getting User responseData from the library's cache or the Discord server. \brief For getting User responseData from the library's cache or the Discord server.
	struct DiscordCoreAPI_Dll GetUserData {
		std::string userId{ "" };///< The id of the desired User.
	};

	/// For modifying the Bot's User responseData. \brief For modifying the Bot's User responseData.
	struct DiscordCoreAPI_Dll ModifyCurrentUserData {
		std::vector<uint8_t> avatar{};///< If passed, modifies the user's avatar.
		std::string username{ "" };///< User's username, if changed may cause the user's discriminator to be randomized.
	};
	
	enum class UserFlags : int8_t {
		Bot = 0b00000000'00000001,
		MFAEnabled = 0b00000000'00000010,
		System = 0b00000000'00000100,
		Verified = 0b00000000'00001000
	};

	/// A single User. \brief A single User.
	class DiscordCoreAPI_Dll User : public DiscordEntity {
	public:
		std::string discriminator{ "" }; ///< The # next to their User name.
		int32_t premiumType{ 0 };///< Their premium nitro status.
		int32_t publicFlags{ 0 };///< Public flags.
		std::string createdAt{ "" };///< When the User was created.
		std::string userName{ "" };///< Their username.
		std::string avatar{ "" };///< Their avatar url.
		std::string locale{ "" };///< The region they are from/in.
		std::string email{ "" };///< Their email address.
		int32_t flags{ 0 };///< Flags.
		int8_t flagsEnum{};///< Flags for enumerated boolean values.

		User();

		User(UserData dataNew);

		bool getBot() {
			return (this->flagsEnum & static_cast<int8_t>(UserFlags::Bot));
		}

		bool getSystem() {
			return (this->flagsEnum & static_cast<int8_t>(UserFlags::System));
		}

		bool getVerified() {
			return (this->flagsEnum & static_cast<int8_t>(UserFlags::Verified));
		}

		bool getMFAEnabled() {
			return (this->flagsEnum & static_cast<int8_t>(UserFlags::MFAEnabled));
		}
	};

	/// A type of User, to represent the Bot and some of its associated endpoints. \brief A type of User, to represent the Bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll BotUser : public User {
	public:

		friend DiscordCoreClient;

		BotUser(User dataPackage, DiscordCoreInternal::BaseSocketAgent* pBaseBaseSocketAgentNew);

		/// Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes. \brief Updates the bot's current voice-status. Joins/leaves a channel, and/or self deafens/mutes.
		void updateVoiceStatus(UpdateVoiceStateData datdataPackageaPackage);

		/// Updates the bot's current activity status, to be viewed by others in the same server as the bot. \brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		void updatePresence(UpdatePresenceData dataPackage);

	protected:

		BotUser();

		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{ nullptr };
	};
	/**@}*/

	/**
	* \addtogroup main_endpoints
	* @{
	*/
	/// An interface class for the User related Discord endpoints. \brief An interface class for the User related Discord endpoints.
	class DiscordCoreAPI_Dll Users {
	public:

		friend DiscordCoreClient;
		friend EventHandler;
		friend Users;
		friend Guild;

		static void initialize(DiscordCoreInternal::HttpClient*);

		/// Adds a chosen recipient to a group Dm. \brief Adds a chosen recipient to a group Dm.
		/// \param dataPackage An AddRecipientToGroupDMData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> addRecipientToGroupDMAsync(AddRecipientToGroupDMData dataPackage);

		/// Removes a chosen recipient from a group Dm. \brief Removes a chosen recipient from a group Dm.
		/// \param dataPackage A RemoveRecipientFromGroupDMData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> removeRecipientFromGroupDMAsync(RemoveRecipientFromGroupDMData dataPackage);

		/// Sets the bot's current voice state. \brief Sets the bot's current voice state.
		/// \param dataPackage A ModifyCurrentUserVoiceStateData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyCurrentUserVoiceStateAsync(ModifyCurrentUserVoiceStateData dataPackage);

		/// Sets another user's current voice state. \brief Sets another user's current voice state.
		/// \param dataPackage A ModifyUserVoiceStateData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyUserVoiceStateAsync(ModifyUserVoiceStateData dataPackage);

		/// Collects the Bot's current User responseData. \brief Collects the Bot's current User responseData.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCurrentUserAsync();

		/// Collects a given User from the library's cache. \brief Collects a given User from the library's cache.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getCachedUserAsync(GetUserData dataPackage);

		/// Collects a given User from the Discord servers. \brief Collects a given User from the Discord servers.
		/// \param dataPackage A GetUserData structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> getUserAsync(GetUserData dataPackage);

		/// Modifies the Bot's User responseData. \brief Modifies the Bot's User responseData.
		/// \param dataPackage A ModifyCurrentUserData  structure.
		/// \returns A CoRoutine containing a User.
		static CoRoutine<User> modifyCurrentUserAsync(ModifyCurrentUserData dataPackage);

		/// Collects the User's Connections. \brief Collects the User's Connections.
		/// \returns A CoRoutine containing a std::vector<ConnectionData>.
		static CoRoutine<std::vector<ConnectionData>> getUserConnections();

		/// Collects the Application responseData associated with the current Bot. \brief Collects the Application responseData associated with the current Bot.
		/// \returns A CoRoutine containing an ApplicationData.
		static CoRoutine<ApplicationData> getCurrentUserApplicationInfoAsync();

		/// Collects the Authorization info associated with the current Bot. \brief Collects the Authorization info associated with the current Bot.
		/// \returns A CoRoutine containing an AuthorizationInfoData.
		static CoRoutine<AuthorizationInfoData> getCurrentUserAuthorizationInfoAsync();


	protected:

		static DiscordCoreInternal::HttpClient* httpClient;
		static std::unordered_map<std::string, User> cache;

		static void insertUser(User user);
	};
	/**@}*/

}
#endif