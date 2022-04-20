/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// ThreadEntities.cpp - Source file for the thread-related stuff.
/// Nov 29, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file ThreadEntities.cpp

#include <discordcoreapi/ThreadEntities.hpp>
#include <discordcoreapi/Http.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	Thread::Thread(ChannelData dataNew) {
		this->permissionOverwrites = dataNew.permissionOverwrites;
		this->rateLimitPerUser = dataNew.rateLimitPerUser;
		this->lastPinTimestamp = dataNew.lastPinTimestamp;
		this->videoQualityMode = dataNew.videoQualityMode;
		this->threadMetadata = dataNew.threadMetadata;
		this->lastMessageId = dataNew.lastMessageId;
		this->applicationId = dataNew.applicationId;
		this->messageCount = dataNew.messageCount;
		this->memberCount = dataNew.memberCount;
		this->recipients = dataNew.recipients;
		this->userLimit = dataNew.userLimit;
		this->rtcRegion = dataNew.rtcRegion;
		this->position = dataNew.position;
		this->parentId = dataNew.parentId;
		this->ownerId = dataNew.ownerId;
		this->bitrate = dataNew.bitrate;
		this->guildId = dataNew.guildId;
		this->member = dataNew.member;
		this->topic = dataNew.topic;
		this->type = dataNew.type;
		this->name = dataNew.name;
		this->nsfw = dataNew.nsfw;
		this->icon = dataNew.icon;
		this->id = dataNew.id;
	}

	void Threads::initialize(DiscordCoreInternal::HttpClient* theClient) {
		Threads::httpClient = theClient;
	}

	CoRoutine<Thread> Threads::startThreadWithMessageAsync(StartThreadWithMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<Thread>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Thread_With_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/messages/" + dataPackage.messageId + "/threads";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage).dump();
			workload.callStack = "Threads::startThreadWithMessageAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Thread>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::startThreadWithMessageAsync()");
		}
	}

	CoRoutine<Thread> Threads::startThreadWithoutMessageAsync(StartThreadWithoutMessageData dataPackage) {
		try {
			co_await NewThreadAwaitable<Thread>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Thread_Without_Message;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/threads";
			workload.content = DiscordCoreInternal::JSONIFY(dataPackage).dump();
			workload.callStack = "Threads::startThreadWithoutMessageAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Thread>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::startThreadWitouthMessageAsync()");
		}
	}

	CoRoutine<void> Threads::joinThreadAsync(JoinThreadData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Self_In_Thread;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/@me";
			workload.callStack = "Threads::joinThreadAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::joinThreadAsync()");
		}
	}

	CoRoutine<void> Threads::addThreadMemberAsync(AddThreadMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Put_Thread_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Put;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
			workload.callStack = "Threads::addThreadMemberAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::addThreadMemberAsync()");
		}
	}

	CoRoutine<void> Threads::leaveThreadAsync(LeaveThreadData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Self_From_Thread;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/@me";
			workload.callStack = "Threads::leaveThreadAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::leaveThreadAsync()");
		}
	}

	CoRoutine<void> Threads::removeThreadMemberAsync(RemoveThreadMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Thread_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
			workload.callStack = "Threads::removeThreadMemberAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::removeThreadMemberAsync()");
		}
	}

	CoRoutine<ThreadMemberData> Threads::getThreadMemberAsync(GetThreadMemberData dataPackage) {
		try {
			co_await NewThreadAwaitable<ThreadMemberData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Thread_Member;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members/" + dataPackage.userId;
			workload.callStack = "Threads::getThreadMemberAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<ThreadMemberData>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::getThreadMemberAsync()");
		}
	}

	CoRoutine<std::vector<ThreadMemberData>> Threads::getThreadMembersAsync(GetThreadMembersData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<ThreadMemberData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Thread_Members;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/thread-members";
			workload.callStack = "Threads::getThreadMembersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<ThreadMemberData>>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::getThreadMembersAsync()");
		}
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveThreadsAsync(GetActiveThreadsData dataPackage) {
		try {
			co_await NewThreadAwaitable<ActiveThreadsData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Active_Threads;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/threads/active";
			workload.callStack = "Threads::getActiveThreadsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<ActiveThreadsData>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::getActiveThreadsAsync()");
		}
	}

	CoRoutine<ArchivedThreadsData> Threads::getPublicArchivedThreadsAsync(GetPublicArchivedThreadsData dataPackage) {
		try {
			co_await NewThreadAwaitable<ArchivedThreadsData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Public_Archived_Threads;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/threads/archived/public";
			if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			} else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "Threads::getPublicArchivedThreadsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<ArchivedThreadsData>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::getPublicArchivedThreadsAsync()");
		}
	}

	CoRoutine<ArchivedThreadsData> Threads::getPrivateArchivedThreadsAsync(GetPrivateArchivedThreadsData dataPackage) {
		try {
			co_await NewThreadAwaitable<ArchivedThreadsData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Private_Archived_Threads;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/threads/archived/private";
			if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			} else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "Threads::getPrivateArchivedThreadsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<ArchivedThreadsData>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::getPrivateArchivedThreadsAsync()");
		}
	}

	CoRoutine<ArchivedThreadsData> Threads::getJoinedPrivateArchivedThreadsAsync(GetJoinedPrivateArchivedThreadsData dataPackage) {
		try {
			co_await NewThreadAwaitable<ArchivedThreadsData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Joined_Private_Archived_Threads;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/channels/" + dataPackage.channelId + "/users/@me/threads/archived/private";
			if (dataPackage.before != "") {
				workload.relativePath += "?before=" + dataPackage.before;
				if (dataPackage.limit != 0) {
					workload.relativePath += "&limit=" + std::to_string(dataPackage.limit);
				}
			} else if (dataPackage.limit != 0) {
				workload.relativePath += "?limit=" + std::to_string(dataPackage.limit);
			}
			workload.callStack = "Threads::getJoinedPrivateArchivedThreadsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<ArchivedThreadsData>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::getJoinedPrivateArchivedThreadsAsync()");
		}
	}

	CoRoutine<ActiveThreadsData> Threads::getActiveGuildThreadsAsync(GetActiveGuildThreadsData dataPackage) {
		try {
			co_await NewThreadAwaitable<ActiveThreadsData>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Active_Threads;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/threads/active";
			workload.callStack = "Threads::listActiveThreadsAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<ActiveThreadsData>(*Threads::httpClient, workload);
		} catch (...) {
			reportException("Threads::listActiveThreadsAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* Threads::httpClient{ nullptr };
}