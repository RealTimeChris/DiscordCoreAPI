// DiscordCoreClient.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_
#define _DISCORD_CORE_CLIENT_

#include "../pch.h"
#include "ThreadManager.hpp"
#include "HttpStuff.hpp"
#include "WebSocketStuff.hpp"
#include "GuildStuff.hpp"
#include "UserStuff.hpp"
#include "InteractionManager.hpp"
#include "EventManager.hpp"
#include "SlashCommandStuff.hpp"
#include "InputEventManager.hpp"
#include "DatabaseStuff.hpp"
#include "FFMPEGStuff.hpp"
#include "YouTubeStuff.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
	return;
}

namespace DiscordCoreAPI {

	static string commandPrefix;

	struct CommandData {
		CommandData(InputEventData inputEventData) {
			this->eventData = inputEventData;
		}
		InputEventData eventData;
	};

	shared_ptr<VoiceConnection> Guild::connectToVoice(string channelId, shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> websocketAgent, shared_ptr<unbounded_buffer<AudioDataChunk>> bufferMessageBlockNew) {
		if (channelId != "") {
			if ((this->voiceConnection == nullptr || this->voiceConnection->voiceConnectionData.channelId != channelId)) {
				auto voiceConnectData = websocketAgent->getVoiceConnectionData(channelId, this->data.id);
				voiceConnectData.channelId = channelId;
				voiceConnectData.guildId = this->data.id;
				voiceConnectData.endpoint = "wss://" + voiceConnectData.endpoint + "/?v=4";
				voiceConnectData.userId = this->discordCoreClientBase->currentUser->data.id;
				this->voiceConnection = make_shared<VoiceConnection>(voiceConnectData, bufferMessageBlockNew);
				map<string, Guild> guildMap;
				try_receive(GuildManagerAgent::cache, guildMap);
				if (guildMap.contains(this->data.id)) {
					if (guildMap.at(this->data.id).voiceConnection != nullptr) {
						return guildMap.at(this->data.id).voiceConnection;
					}
					guildMap.erase(this->data.id);
				}
				guildMap.insert(make_pair(this->data.id, *this));
				send(GuildManagerAgent::cache, guildMap);
				return this->voiceConnection;
			}
		}
		return this->voiceConnection;
	}

	void Guild::disconnectFromVoice() {
		if (this->voiceConnection != nullptr) {
			this->voiceConnection->terminate();
			this->voiceConnection = nullptr;
			map<string, Guild> guildMap = receive(GuildManagerAgent::cache);
			if (guildMap.contains(this->data.id)) {
				guildMap.erase(this->data.id);
			}
			guildMap.insert(make_pair(this->data.id, *this));
			send(GuildManagerAgent::cache, guildMap);
			this->discordCoreClientBase->currentUser->updateVoiceStatus({ .guildId = this->data.id,.channelId = "", .selfMute = false,.selfDeaf = false });
			return;
		}
	}

	class DiscordCoreClient :public DiscordCoreClientBase, protected agent {
	public:
		static shared_ptr<DiscordCoreClient> thisPointer;
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgent{ nullptr };
		shared_ptr<SlashCommandManager> slashCommands{ nullptr };
		shared_ptr<InteractionManager> interactions{ nullptr };
		shared_ptr<EventManager> eventManager{ nullptr };
		shared_ptr<ReactionManager> reactions{ nullptr };
		shared_ptr<MessageManager> messages{ nullptr };
		shared_ptr<DiscordUser> discordUser{ nullptr };
		shared_ptr<GuildManager> guilds{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources;
		map<string, shared_ptr<unbounded_buffer<AudioDataChunk>>> audioBuffersMap;

		DiscordCoreClient(hstring botTokenNew) :agent(*DiscordCoreInternal::ThreadManager::getThreadContext().get()->scheduler) {
			this->botToken = botTokenNew;
		}

		static void finalSetup(string botToken);

		static void runBot() {
			wait((agent*)DiscordCoreClient::thisPointer.get());
			DiscordCoreClient::getError();
		}

		static void getError() {
			exception error;
			while (try_receive(DiscordCoreClient::errorBuffer, error)) {
				cout << "DiscordCoreClient Error: " << error.what() << endl;
			}
		}

		void terminate() {
			this->doWeQuit = true;
			DatabaseManagerAgent::cleanup();
			this->slashCommands->~SlashCommandManager();
			this->guilds->~GuildManager();
			this->channels->~ChannelManager();
			this->guildMembers->~GuildMemberManager();
			this->roles->~RoleManager();
			this->messages->~MessageManager();
			this->users->~UserManager();
			this->reactions->~ReactionManager();
			this->interactions->~InteractionManager();
			agent::wait(this->pWebSocketReceiverAgent.get());
			SelectMenu::cleanup();
			Button::cleanup();
			InteractionManagerAgent::cleanup();
			MessageManagerAgent::cleanup();
			GuildManagerAgent::cleanup();
			RoleManagerAgent::cleanup();
			UserManagerAgent::cleanup();
			ReactionManagerAgent::cleanup();
			ChannelManagerAgent::cleanup();
			GuildMemberManagerAgent::cleanup();
			agent::wait(this->pWebSocketConnectionAgent.get());
			this->mainThreadContext->releaseGroup();
		}

	protected:
		friend class Guild;
		friend class BotUser;
		static unbounded_buffer<exception> errorBuffer;
		bool doWeQuit = false;
		hstring baseURL = L"https://discord.com/api/v9";
		hstring gatewayBaseURL = L"wss://gateway.discord.gg/?v=9";
		shared_ptr<DiscordCoreInternal::WebSocketReceiverAgent> pWebSocketReceiverAgent{ nullptr };
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;
		shared_ptr<DiscordCoreInternal::ThreadContext> mainThreadContext{ nullptr };

		task<void> initialize() {
			thisPointer.reset(this);
			_set_purecall_handler(myPurecallHandler);
			apartment_context mainThread;
			this->mainThreadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
			co_await resume_foreground(*this->mainThreadContext->dispatcherQueue.get());
			this->eventManager = make_shared<DiscordCoreAPI::EventManager>();
			this->pWebSocketConnectionAgent = make_shared<DiscordCoreInternal::WebSocketConnectionAgent>(this->webSocketIncWorkloadBuffer, this->botToken, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::HttpRequestAgent::initialize(to_string(this->botToken), to_string(baseURL));
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SOCKET_PATH;
			workload.relativePath = "/gateway/bot";
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			requestAgent.getError("DiscordCoreClient");
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			GuildMemberManagerAgent::intialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			ChannelManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			ReactionManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			UserManagerAgent::intialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			RoleManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			GuildManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			MessageManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			InteractionManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			Button::initialize(this->interactions);
			SelectMenu::initialize(this->interactions);
			this->pWebSocketConnectionAgent->setSocketPath(returnData.data.dump());
			this->pWebSocketReceiverAgent = make_shared<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkCollectionBuffer, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->interactions = make_shared<InteractionManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->reactions = make_shared<ReactionManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->users = make_shared<UserManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->messages = make_shared<MessageManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->roles = make_shared<RoleManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->guildMembers = make_shared<GuildMemberManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->channels = make_shared<ChannelManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->guilds = make_shared<GuildManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), (shared_ptr<DiscordCoreClient>)DiscordCoreClient::thisPointer, (shared_ptr<DiscordCoreClientBase>)DiscordCoreClient::thisPointer);
			this->currentUser = make_shared<BotUser>(this->users->fetchCurrentUserAsync().get().data, DiscordCoreClient::thisPointer, this->pWebSocketConnectionAgent);
			this->slashCommands = make_shared<SlashCommandManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->currentUser->data.id);
			DatabaseManagerAgent::initialize(this->currentUser->data.id, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			InputEventManager::initialize(DiscordCoreClient::thisPointer, DiscordCoreClient::thisPointer, this->messages, this->interactions);
			this->discordUser = make_shared<DiscordUser>(this->currentUser->data.username, this->currentUser->data.id);
			DiscordCoreAPI::commandPrefix = this->discordUser->data.prefix;
			this->discordUser->writeDataToDB();
			this->pWebSocketReceiverAgent->start();
			this->pWebSocketConnectionAgent->start();
			this->start();
			executeFunctionAfterTimePeriod([&]() {
				vector<ActivityData> activities;
				ActivityData activity;
				activity.name = "!help for my commands!";
				activity.type = ActivityType::Game;
				activities.push_back(activity);
				this->currentUser->updatePresence({ .activities = activities, .status = "online",.afk = false });
				}, 5000, false);
			co_await mainThread;
			co_return;
		}

		Guild createGuild(GuildData guildData) {
			Guild guild(this->agentResources, guildData, (shared_ptr<DiscordCoreClient>)DiscordCoreClient::thisPointer, DiscordCoreClient::thisPointer);
			DiscordGuild discordGuild(guild.data);
			discordGuild.getDataFromDB();
			discordGuild.writeDataToDB();
			this->discordUser->data.guildCount += 1;
			this->discordUser->writeDataToDB();
			shared_ptr<unbounded_buffer<AudioDataChunk>>thePtr = make_shared<unbounded_buffer<AudioDataChunk>>();
			this->audioBuffersMap.insert(make_pair(guild.data.id, thePtr));
			return guild;
		}

		void removeGuild(GuildData guildData) {
			this->discordUser->data.guildCount -= 1;
			this->discordUser->writeDataToDB();
			shared_ptr<unbounded_buffer<AudioDataChunk>>thePtr = make_shared<unbounded_buffer<AudioDataChunk>>();
			this->audioBuffersMap.erase(guildData.id);
		}

		void run() {
			try {
				while (doWeQuit == false) {
					DiscordCoreInternal::WebSocketWorkload workload = receive(this->webSocketWorkCollectionBuffer, INFINITE);
					switch (workload.eventType) {
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_CREATE:
					{
						ChannelData channelData;
						DiscordCoreInternal::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData, DiscordCoreClient::thisPointer);
						OnChannelCreationData channelCreationData;
						channelCreationData.channel = channel;
						this->eventManager->onChannelCreationEvent(channelCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_UPDATE:
					{
						OnChannelUpdateData channelUpdateData;
						Channel channel = this->channels->getChannelAsync({ .channelId = workload.payLoad.at("id") }).get();
						channelUpdateData.channelOld = channel;
						DiscordCoreInternal::parseObject(workload.payLoad, &channel.data);
						channelUpdateData.channelNew = channel;
						this->eventManager->onChannelUpdateEvent(channelUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_DELETE:
					{
						ChannelData channelData;
						DiscordCoreInternal::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData, DiscordCoreClient::thisPointer);
						OnChannelDeletionData channelDeleteData;
						channelDeleteData.channel = channel;
						this->eventManager->onChannelDeletionEvent(channelDeleteData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_CREATE:
					{
						GuildData guildData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildData);
						Guild guild = createGuild(guildData);
						DiscordCoreAPI::OnGuildCreationData guildCreationData;
						guildCreationData.guild = guild;
						this->eventManager->onGuildCreationEvent(guildCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_UPDATE:
					{
						GuildData guildData;
						DiscordCoreAPI::OnGuildUpdateData guildUpdateData;
						Guild guild = this->guilds->getGuildAsync({ .guildId = workload.payLoad.at("id") }).get();
						guildUpdateData.guildOld = guild;
						DiscordCoreInternal::parseObject(workload.payLoad, &guild.data);
						guildUpdateData.guildNew = guild;
						this->eventManager->onGuildUpdateEvent(guildUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_DELETE:
					{
						GuildData guildData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildData);
						removeGuild(guildData);
						Guild guild(this->agentResources, guildData, DiscordCoreClient::thisPointer, DiscordCoreClient::thisPointer);
						DiscordCoreAPI::OnGuildDeletionData guildDeletionData;
						guildDeletionData.guild = guild;
						this->eventManager->onGuildDeletionEvent(guildDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_BAN_ADD:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, DiscordCoreClient::thisPointer);
						DiscordCoreAPI::OnGuildBanAddData guildBanAddData;
						guildBanAddData.user = user;
						guildBanAddData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildBanAddEvent(guildBanAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_BAN_REMOVE:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, DiscordCoreClient::thisPointer);
						DiscordCoreAPI::OnGuildBanRemoveData guildBanRemoveData;
						guildBanRemoveData.user = user;
						guildBanRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildBanRemoveEvent(guildBanRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_ADD:
					{
						GuildMemberData guildMemberData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMember(guildMemberData, workload.payLoad.at("guild_id").get<string>(), DiscordCoreClient::thisPointer);
						DiscordCoreAPI::OnGuildMemberAddData guildMemberAddData;
						guildMemberAddData.guildMember = guildMember;
						this->eventManager->onGuildMemberAddEvent(guildMemberAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_REMOVE:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, DiscordCoreClient::thisPointer);
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData;
						guildMemberRemoveData.user = user;
						guildMemberRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildMemberRemoveEvent(guildMemberRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_UPDATE:
					{
						DiscordCoreAPI::OnGuildMemberUpdateData guildMemberUpdateData;
						GuildMember guildMemberOld = this->guildMembers->getGuildMemberAsync({ .guildId = workload.payLoad.at("guild_id"), .guildMemberId = workload.payLoad.at("user").at("id") }).get();
						guildMemberUpdateData.guildMemberOld = guildMemberOld;
						GuildMemberData guildMemberData;
						DiscordCoreInternal::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMemberNew(guildMemberData, workload.payLoad.at("guild_id"), DiscordCoreClient::thisPointer);
						guildMemberUpdateData.guildMemberNew = guildMemberNew;
						this->eventManager->onGuildMemberUpdateEvent(guildMemberUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_CREATE:
					{
						RoleData roleData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("role"), &roleData);
						Role role(roleData, DiscordCoreClient::thisPointer);
						DiscordCoreAPI::OnRoleCreationData roleCreationData;
						roleCreationData.role = role;
						roleCreationData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onRoleCreationEvent(roleCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_UPDATE:
					{
						DiscordCoreAPI::OnRoleUpdateData roleUpdateData;
						Role role = this->roles->getRoleAsync({ .guildId = workload.payLoad.at("guild_id"), .roleId = workload.payLoad.at("role").at("id") }).get();
						roleUpdateData.roleOld = role;
						DiscordCoreInternal::parseObject(workload.payLoad.at("role"), &role.data);
						roleUpdateData.roleNew = role;
						roleUpdateData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onRoleUpdateEvent(roleUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_DELETE:
					{
						DiscordCoreAPI::OnRoleDeletionData roleDeletionData;
						roleDeletionData.guildId = workload.payLoad.at("guild_id");
						roleDeletionData.roleOld = this->roles->getRoleAsync({ .guildId = roleDeletionData.guildId, .roleId = workload.payLoad.at("role_id") }).get();
						this->eventManager->onRoleDeletionEvent(roleDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::INVITE_CREATE:
					{
						InviteData inviteData;
						DiscordCoreInternal::parseObject(workload.payLoad, &inviteData);
						OnInviteCreationData inviteCreationData;
						inviteCreationData.invite = inviteData;
						inviteCreationData.discordCoreClient = DiscordCoreClient::thisPointer;
						this->eventManager->onInviteCreationEvent(inviteCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::INVITE_DELETE:
					{
						string channelId = workload.payLoad.at("channel_id");
						string guildId = workload.payLoad.at("guild_id");
						string code = workload.payLoad.at("code");
						OnInviteDeletionData inviteDeletionData;
						inviteDeletionData.channelId = channelId;
						inviteDeletionData.guildId = guildId;
						inviteDeletionData.code = code;
						inviteDeletionData.discordCoreClient = DiscordCoreClient::thisPointer;
						this->eventManager->onInviteDeletionEvent(inviteDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::INTERACTION_CREATE:
					{
						if (workload.payLoad.contains("user")) {
							InteractionData interactionData(workload.payLoad.at("user").at("id").get<string>());
							DiscordCoreInternal::parseObject(workload.payLoad, &interactionData);
							InputEventData eventData;
							if (interactionData.type == InteractionType::ApplicationCommand) {
								eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.interactionData = interactionData;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.requesterId = interactionData.requesterId;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.type == InteractionType::MessageComponent) {
								if (interactionData.componentType == ComponentType::Button) {
									eventData.eventType = InputEventType::BUTTON_INTERACTION;
									eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
									eventData.interactionData = interactionData;
									eventData.discordCoreClient = DiscordCoreClient::thisPointer;
									eventData.requesterId = interactionData.requesterId;
									OnInteractionCreationData eventCreationData;
									eventCreationData.eventData = eventData;
									this->eventManager->onInteractionCreationEvent(eventCreationData);
								}
								else if (interactionData.componentType == ComponentType::SelectMenu) {
									eventData.eventType = InputEventType::SELECT_MENU_INPUT;
									eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
									eventData.interactionData = interactionData;
									eventData.discordCoreClient = DiscordCoreClient::thisPointer;
									eventData.requesterId = interactionData.requesterId;
									OnInteractionCreationData eventCreationData;
									eventCreationData.eventData = eventData;
									this->eventManager->onInteractionCreationEvent(eventCreationData);
								}
							}
							break;
						}
						else if (workload.payLoad.contains("member")) {
							InteractionData interactionData(workload.payLoad.at("member").at("user").at("id").get<string>());
							DiscordCoreInternal::parseObject(workload.payLoad, &interactionData);
							InputEventData eventData;
							if (interactionData.type == InteractionType::ApplicationCommand) {
								eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.interactionData = interactionData;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.requesterId = interactionData.requesterId;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.type == InteractionType::MessageComponent) {
								if (interactionData.componentType == ComponentType::Button) {
									eventData.eventType = InputEventType::BUTTON_INTERACTION;
									eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
									eventData.interactionData = interactionData;
									eventData.discordCoreClient = DiscordCoreClient::thisPointer;
									eventData.requesterId = interactionData.requesterId;
									OnInteractionCreationData eventCreationData;
									eventCreationData.eventData = eventData;
									this->eventManager->onInteractionCreationEvent(eventCreationData);
								}
								else if (interactionData.componentType == ComponentType::SelectMenu) {
									eventData.eventType = InputEventType::SELECT_MENU_INPUT;
									eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
									eventData.interactionData = interactionData;
									eventData.discordCoreClient = DiscordCoreClient::thisPointer;
									eventData.requesterId = interactionData.requesterId;
									OnInteractionCreationData eventCreationData;
									eventCreationData.eventData = eventData;
									this->eventManager->onInteractionCreationEvent(eventCreationData);
								}
							}
							break;
						}
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_CREATE:
					{
						MessageData messageData;
						DiscordCoreInternal::parseObject(workload.payLoad, &messageData);
						if (InteractionManagerAgent::collectMessageDataBuffers.contains(messageData.interaction.id)) {
							send(*InteractionManagerAgent::collectMessageDataBuffers.at(messageData.interaction.id), messageData);
						}
						Message message(messageData, DiscordCoreClient::thisPointer);
						OnMessageCreationData messageCreationData;
						messageCreationData.message = message;
						this->eventManager->onMessageCreationEvent(messageCreationData);
						InputEventData eventData;
						eventData.eventType = InputEventType::REGULAR_MESSAGE;
						eventData.messageData = messageData;
						eventData.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
						eventData.requesterId = messageData.author.id;
						eventData.discordCoreClient = DiscordCoreClient::thisPointer;
						OnInteractionCreationData eventCreationData;
						eventCreationData.eventData = eventData;
						this->eventManager->onInteractionCreationEvent(eventCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_UPDATE:
					{
						DiscordCoreAPI::OnMessageUpdateData messageUpdateData;
						MessageData messageData;
						DiscordCoreInternal::parseObject(workload.payLoad, &messageData);
						Message message(messageData, DiscordCoreClient::thisPointer);
						messageUpdateData.messageNew = message;
						this->eventManager->onMessageUpdateEvent(messageUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_DELETE:
					{
						OnMessageDeletionData messageDeletionData;
						messageDeletionData.messageId = workload.payLoad.at("id");
						messageDeletionData.channelId = workload.payLoad.at("channel_id");
						if (workload.payLoad.contains("guild_id")) {
							messageDeletionData.guildId = workload.payLoad.at("guild_id");
						}
						this->eventManager->onMessageDeletionEvent(messageDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_DELETE_BULK:
					{
						OnMessageDeleteBulkData messageDeleteBulkData;
						messageDeleteBulkData.channelId = workload.payLoad.at("channel_id");
						messageDeleteBulkData.guildId = workload.payLoad.at("guild_id");
						for (auto value : workload.payLoad.at("ids")) {
							messageDeleteBulkData.ids.push_back(value);
						}
						this->eventManager->onMessageDeleteBulkEvent(messageDeleteBulkData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::REACTION_ADD:
					{
						ReactionAddData reactionAddDataNew;
						DiscordCoreInternal::parseObject(workload.payLoad, &reactionAddDataNew);
						ReactionData reactionData;
						reactionData.channelId = reactionAddDataNew.channelId;
						reactionData.emoji = reactionAddDataNew.emoji;
						reactionData.guildId = reactionAddDataNew.guildId;
						reactionData.member = reactionAddDataNew.member;
						reactionData.messageId = reactionAddDataNew.messageId;
						reactionData.userId = reactionAddDataNew.userId;
						Reaction reaction(reactionData, DiscordCoreClient::thisPointer);
						OnReactionAddData reactionAddData;
						reactionAddData.reaction = reaction;
						this->eventManager->onReactionAddEvent(reactionAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::REACTION_REMOVE:
					{
						ReactionRemoveData reactionRemoveDataNew;
						DiscordCoreInternal::parseObject(workload.payLoad, &reactionRemoveDataNew);
						reactionRemoveDataNew.discordCoreClient = this;
						OnReactionRemoveData reactionRemoveData;
						reactionRemoveData.reactionRemoveData = reactionRemoveDataNew;
						this->eventManager->onReactionRemoveEvent(reactionRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::REACTION_REMOVE_ALL:
					{
						OnReactionRemoveAllData reactionRemoveAllData;
						reactionRemoveAllData.channelId = workload.payLoad.at("channel_id");
						reactionRemoveAllData.guildId = workload.payLoad.at("guild_id");
						reactionRemoveAllData.messageId = workload.payLoad.at("message_id");
						this->eventManager->onReactionRemoveAllEvent(reactionRemoveAllData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::REACTION_REMOVE_EMOJI:
					{
						OnReactionRemoveEmojiData reactionRemoveEmojiData;
						reactionRemoveEmojiData.channelId = workload.payLoad.at("channel_id");
						reactionRemoveEmojiData.guildId = workload.payLoad.at("guild_id");
						reactionRemoveEmojiData.messageId = workload.payLoad.at("message_id");
						EmojiData emojiData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("emoji"), &emojiData);
						reactionRemoveEmojiData.emoji = emojiData;
						this->eventManager->onReactionRemoveEmojiEvent(reactionRemoveEmojiData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::PRESENCE_UPDATE:
					{
						OnPresenceUpdateData presenceUpdateData;
						PresenceUpdateData newData;
						DiscordCoreInternal::parseObject(workload.payLoad, &newData);
						presenceUpdateData.presenceData = newData;
						this->eventManager->onPresenceUpdateEvent(presenceUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::TYPING_START:
					{
						OnTypingStartData typingStartData;
						TypingStartData newData;
						DiscordCoreInternal::parseObject(workload.payLoad, &newData);
						typingStartData.typingStartData = newData;
						this->eventManager->onTypingStartEvent(typingStartData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::USER_UPDATE:
					{
						OnUserUpdateData userUpdateData;
						UserData newData;
						DiscordCoreInternal::parseObject(workload.payLoad, &newData);
						User userOld = this->users->getUserAsync({ .userId = newData.id }).get();
						User userNew(newData, DiscordCoreClient::thisPointer);
						this->users->insertUserAsync(userNew).get();
						userUpdateData.userNew = userNew;
						userUpdateData.userOld = userOld;
						this->eventManager->onUserUpdateEvent(userUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::VOICE_STATE_UPDATE:
					{
						OnVoiceStateUpdateData voiceStateUpdateData;
						VoiceStateData newData;
						DiscordCoreInternal::parseObject(workload.payLoad, &newData);
						voiceStateUpdateData.voiceStateData = newData;
						this->eventManager->onVoiceStateUpdateEvent(voiceStateUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::VOICE_SERVER_UPDATE:
					{
						OnVoiceServerUpdateData voiceServerUpdateData;
						voiceServerUpdateData.token = workload.payLoad.at("token");
						voiceServerUpdateData.guildId = workload.payLoad.at("guild_id");
						voiceServerUpdateData.endpoint = workload.payLoad.at("endpoint");
						this->eventManager->onVoiceServerUpdateEvent(voiceServerUpdateData);
						break;
					}
					default:
					{
						break;
					}
					}
				}
			}
			catch (const operation_timed_out& e) {
				done();
				cout << e.what() << endl;
				start();
			}
			catch (const exception& e) {
				send(DiscordCoreClient::errorBuffer, e);
			}
			done();
			return;
		}
	};
	unbounded_buffer<exception> DiscordCoreClient::errorBuffer;
	shared_ptr<DiscordCoreClient> DiscordCoreClient::thisPointer;
}
#include "Commands/CommandsList.hpp"

#endif
