// ChannelEntities.hpp - Header for the classes related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CHANNEL_ENTITIES_
#define _CHANNEL_ENTITIES_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "Http.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// For editing the Permission overwrites of a given Channel, for a given role or User. \brief For editing the Permission overwrites of a given Channel, for a given role or User.
	struct DiscordCoreAPI_Dll EditChannelPermissionOverwritesData {
		EditChannelPermissionOverwritesType type{}; ///< The type of overwrite.
		string roleOrUserId{ "" };	///< The role or User Id to modify the permissions for.
		string channelId{ "" };	///< The Channel id of the desired Channel to modify the permissions for.
		string allow{ "" };	///< The permissions to list as "allowed".
		string deny{ "" };	///< The permissions to list as "deny".
	};

	/// For collecting a Channel from Discord's servers. \brief For collecting a Channel from Discord's servers.
	struct DiscordCoreAPI_Dll GetChannelData {
		string channelId{ "" }; ///< The id of the Channel to acquire.
	};

	/// For modifying a channel's properties. \brief For modifying a channel's properties.
	struct DiscordCoreAPI_Dll ModifyChannelData {
		UpdateChannelData channelData{  };///< The data of the channel to be updated.
		string channelId{ "" }; ///< The id of the Channel to modify.
	};

	/// For a channel's properties. \brief For modifying a channel's properties
	struct DiscordCoreAPI_Dll DeleteChannelData {
		string channelId{ "" };///< The id of the Channel to delete.
	};

	/// For deleting the Permission overwrites of a given Channel for a given role or User. \brief For deleting the Permission overwrites of a given Channel for a given role or User.
	struct DiscordCoreAPI_Dll DeleteChannelPermissionOverwritesData {
		string roleOrUserId{ "" }; ///< The role or User id for whom to delete the Permission overwrites.
		string channelId{ "" }; ///< The id of the Channel to modify permissions for..
	};

	/// For collecting a direct-messaging Channel. \brief For collecting a direct-messaging Channel.
	struct DiscordCoreAPI_Dll GetDMChannelData {
		string userId{ "" }; ///< The User for whom to collect the direct-messaging Channel to.
	};

	/// A Channel object. \brief A Channel object.
	class DiscordCoreAPI_Dll Channel : public ChannelData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Channel>;
		friend class DiscordCoreInternal::ChannelManagerAgent;
		friend class DiscordCoreInternal::ChannelManager;
		template<typename returnValueType>
		friend class DiscordCoreAPI::CoRoutine;
		friend struct OnChannelDeletionData;
		friend struct OnChannelCreationData;
		friend struct OnChannelUpdateData;
		friend struct OnThreadCreationData;
		friend struct OnThreadDeletionData;
		friend struct OnThreadUpdateData;
		friend class DiscordCoreClient;
		friend class Guild;

		Channel();

		Channel(ChannelData dataNew);
	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll ChannelManagerAgent : public ThreadContext, public agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class ChannelManager;

		unbounded_buffer<DeleteChannelPermissionOverwritesData> requestDeleteChannelPermOWsBuffer{ nullptr };
		unbounded_buffer<PutPermissionOverwritesData> requestPutChannelPermOWsBuffer{ nullptr };
		unbounded_buffer<DeleteChannelData> requestDeleteChannelBuffer{ nullptr };
		unbounded_buffer<PatchChannelData> requestPatchChannelBuffer{ nullptr };
		unbounded_buffer<GetDMChannelData>requestGetDMChannelBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Channel> outChannelBuffer{ nullptr };
		unbounded_buffer<GetChannelData> requestGetChannelBuffer{ nullptr };

		ChannelManagerAgent();

		static void initialize();

		DiscordCoreAPI::Channel getObjectData(GetChannelData dataPackage);

		DiscordCoreAPI::Channel postObjectData(GetDMChannelData dataPackage);

		DiscordCoreAPI::Channel patchObjectData(PatchChannelData dataPackage);

		void putObjectData(PutPermissionOverwritesData dataPackage);

		void deleteObjectData(DeleteChannelPermissionOverwritesData dataPackage);

		void deleteObjectData(DeleteChannelData dataPackage);

		void run();

	};

	class DiscordCoreAPI_Dll ChannelManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::InputEvents;
		friend class DiscordCoreAPI::Channels;
		friend class DiscordCoreAPI::Guild;

		ChannelManager(ChannelManager* pointer);

	protected:

		shared_ptr<overwrite_buffer<map<string, DiscordCoreAPI::Channel>>> cache{};

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> getCachedChannelAsync(DiscordCoreAPI::GetChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> getChannelAsync(DiscordCoreAPI::GetChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> modifyChannelAsync(DiscordCoreAPI::ModifyChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteChannelAsync(DiscordCoreAPI::DeleteChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel> getDMChannelAsync(DiscordCoreAPI::GetDMChannelData dataPackage);
		
		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Channel>  getCachedChannelAsyncToBeWrapped(DiscordCoreAPI::GetChannelData dataPackage);

		DiscordCoreAPI::CoRoutine<void> editChannelPermissionOverwritesAsync(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

		DiscordCoreAPI::CoRoutine<void> deleteChannelPermissionOverwritesAsync(DiscordCoreAPI::DeleteChannelPermissionOverwritesData dataPackage);

		void insertChannel(DiscordCoreAPI::Channel channel);

		void removeChannel(string channelId);
	};
}
#endif