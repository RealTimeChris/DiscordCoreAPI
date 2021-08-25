// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_01_
#define _DISCORD_CORE_CLIENT_01_

#include "../pch.h"
#include "ThreadManager.hpp"
#include "HttpStuff.hpp"
#include "GuildStuff01.hpp"
#include "InteractionStuff.hpp"
#include "EventStuff01.hpp"
#include "SlashCommandStuff.hpp"
#include "InputEventStuff.hpp"
#include "DatabaseStuff.hpp"
#include "YouTubeStuff.hpp"
#include "GuildStuff02.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
	return;
}

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

namespace DiscordCoreAPI {

	class DiscordCoreClient :public DiscordCoreClientBase, protected agent, enable_shared_from_this<DiscordCoreClient> {
	public:

		static shared_ptr<DiscordCoreClient> thisPointer;
		shared_ptr<DiscordUser> discordUser{ nullptr };
		shared_ptr<EventManager> eventManager{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources;

		DiscordCoreClient(hstring botTokenNew) :DiscordCoreClientBase(this),agent(*DiscordCoreInternal::ThreadManager::getThreadContext().get()->scheduler) {
			this->botToken = botTokenNew;
		}

		static void finalSetup(string botToken, string commandPrefix, vector<RepeatedFunctionData>* lambda);

		static void runBot() {
			wait((agent*)DiscordCoreClient::thisPointer.get());
			DiscordCoreClient::getError();
		}

		static void terminate() {
			DiscordCoreClient::thisPointer->doWeQuit = true;
			DatabaseManagerAgent::cleanup();
			wait(DiscordCoreClient::thisPointer->pWebSocketConnectionAgent.get());
			wait(DiscordCoreClient::thisPointer->pWebSocketReceiverAgent.get());
			DiscordCoreClientBase::currentUser->~BotUser();
			DiscordCoreClient::thisPointer->guilds->~GuildManager();
			DiscordCoreClient::thisPointer->messages->~MessageManager();
			DiscordCoreClient::thisPointer->reactions->~ReactionManager();
			DiscordCoreClient::thisPointer->users->~UserManager();
			DiscordCoreClient::thisPointer->channels->~ChannelManager();
			DiscordCoreClient::thisPointer->guildMembers->~GuildMemberManager();
			DiscordCoreClient::thisPointer->roles->~RoleManager();
			DiscordCoreClient::thisPointer->interactions->~InteractionManager();
			SelectMenuManager::cleanup();
			ButtonManager::cleanup();
			InteractionManagerAgent::cleanup();
			MessageManagerAgent::cleanup();
			GuildManagerAgent::cleanup();
			RoleManagerAgent::cleanup();
			UserManagerAgent::cleanup();
			ReactionManagerAgent::cleanup();
			ChannelManagerAgent::cleanup();
			GuildMemberManagerAgent::cleanup();
			DiscordCoreClient::thisPointer->mainThreadContext->releaseGroup();
		}

		~DiscordCoreClient() {
			DiscordCoreClient::terminate();
		}

	protected:
		friend class Guild;
		friend class BotUser;
		friend class Guilds;
		friend class Channels;
		friend class Roles;
		friend class Users;
		friend class GuildMembers;
		friend class PermissionsConverter;
		friend class SlashCommands;
		friend class Messages;
		friend class Interactions;
		friend class Reactions;
		static unbounded_buffer<exception> errorBuffer;
		shared_ptr<SlashCommandManager> slashCommands{ nullptr };
		shared_ptr<InteractionManager> interactions{ nullptr };
		shared_ptr<ReactionManager> reactions{ nullptr };
		shared_ptr<MessageManager> messages{ nullptr };
		shared_ptr<GuildManager> guilds{ nullptr };
		bool doWeQuit{ false };
		hstring baseURL{ L"https://discord.com/api/v9" };
		hstring gatewayBaseURL{ L"wss://gateway.discord.gg/?v=9" };
		shared_ptr<DiscordCoreInternal::WebSocketReceiverAgent> pWebSocketReceiverAgent{ nullptr };
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer{ nullptr };
		unbounded_buffer<json> webSocketIncWorkloadBuffer{ nullptr };
		shared_ptr<DiscordCoreInternal::ThreadContext> mainThreadContext{ nullptr };

		task<void> initialize() {
			PHANDLER_ROUTINE handlerRoutine(&HandlerRoutine);
			SetConsoleCtrlHandler(handlerRoutine, true);
			_set_purecall_handler(myPurecallHandler);
			this->thisPointerBase = make_shared<DiscordCoreClientBase>((DiscordCoreClientBase*)this);
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
			requestAgent.getError("DiscordCoreClient::initialize()");
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
			ButtonManager::initialize(this->interactions);
			SelectMenuManager::initialize(this->interactions);
			this->thisPointerBase->initialize(this->agentResources, this->thisPointer, this->pWebSocketConnectionAgent);
			this->pWebSocketConnectionAgent->setSocketPath(returnData.data.dump());
			this->pWebSocketReceiverAgent = make_shared<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkCollectionBuffer, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->interactions = make_shared<InteractionManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->reactions = make_shared<ReactionManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->users = this->thisPointerBase->users;
			this->messages = make_shared<MessageManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), DiscordCoreClient::thisPointer);
			this->roles = this->thisPointerBase->roles;
			this->guildMembers = this->thisPointerBase->guildMembers;
			this->channels = this->thisPointerBase->channels;
			this->guilds = make_shared<GuildManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), (shared_ptr<DiscordCoreClient>)DiscordCoreClient::thisPointer, (shared_ptr<DiscordCoreClientBase>)DiscordCoreClient::thisPointerBase);
			this->slashCommands = make_shared<SlashCommandManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointerBase->currentUser->data.id);
			DatabaseManagerAgent::initialize(this->thisPointerBase->currentUser->data.id, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			InputEvents::initialize(DiscordCoreClient::thisPointerBase, DiscordCoreClient::thisPointer, this->messages, this->interactions);
			this->discordUser = make_shared<DiscordUser>(this->thisPointerBase->currentUser->data.username, this->thisPointerBase->currentUser->data.id);
			DiscordCoreClientBase::currentUser = this->thisPointerBase->currentUser;
			DiscordCoreAPI::commandPrefix = this->discordUser->data.prefix;
			this->discordUser->writeDataToDB();
			this->pWebSocketReceiverAgent->start();
			this->pWebSocketConnectionAgent->start();
			this->start();
			co_await mainThread;
			co_return;
		}

		Guild createGuild(GuildData guildData) {
			Guild guild(this->agentResources, guildData, (shared_ptr<DiscordCoreClient>)DiscordCoreClient::thisPointer, DiscordCoreClient::thisPointerBase);
			auto valueNew = this->guilds->getAllGuildsAsync().get();
			bool isItFound = false;
			for (auto value : valueNew) {
				if (guildData.id == value.data.id) {
					isItFound = true;
				}
			}
			DiscordGuild discordGuild(guild.data);
			discordGuild.getDataFromDB();
			discordGuild.writeDataToDB();
			if (!isItFound) {
				this->discordUser->data.guildCount += 1;
				this->discordUser->writeDataToDB();
			}
			return guild;
		}

		void removeGuild(GuildData guildData) {
			this->discordUser->writeDataToDB();
			shared_ptr<unbounded_buffer<vector<RawFrameData>>>thePtr = make_shared<unbounded_buffer<vector<RawFrameData>>>();
			this->audioBuffersMap.erase(guildData.id);
		}

		static void getError() {
			exception error;
			while (try_receive(DiscordCoreClient::errorBuffer, error)) {
				cout << "DiscordCoreClient Error: " << error.what() << endl;
			}
		}

		void run() {
			try {
				while (doWeQuit == false) {
					DiscordCoreInternal::WebSocketWorkload workload = receive(this->webSocketWorkCollectionBuffer, INFINITE);
					switch (workload.eventType) {
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_CREATE:
					{
						ChannelData channelData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channel.data);
						channelUpdateData.channelNew = channel;
						this->eventManager->onChannelUpdateEvent(channelUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_DELETE:
					{
						ChannelData channelData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData, DiscordCoreClient::thisPointer);
						OnChannelDeletionData channelDeleteData;
						channelDeleteData.channel = channel;
						this->eventManager->onChannelDeletionEvent(channelDeleteData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_CREATE:
					{
						GuildData guildData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guild.data);
						guildUpdateData.guildNew = guild;
						this->eventManager->onGuildUpdateEvent(guildUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_DELETE:
					{
						GuildData guildData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildData);
						removeGuild(guildData);
						Guild guild(this->agentResources, guildData, DiscordCoreClient::thisPointer, DiscordCoreClient::thisPointerBase);
						DiscordCoreAPI::OnGuildDeletionData guildDeletionData;
						guildDeletionData.guild = guild;
						this->eventManager->onGuildDeletionEvent(guildDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_BAN_ADD:
					{
						UserData userData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMember(guildMemberData, workload.payLoad.at("guild_id").get<string>(), DiscordCoreClient::thisPointer);
						DiscordCoreAPI::OnGuildMemberAddData guildMemberAddData;
						guildMemberAddData.guildMember = guildMember;
						this->eventManager->onGuildMemberAddEvent(guildMemberAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_REMOVE:
					{
						UserData userData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMemberNew(guildMemberData, workload.payLoad.at("guild_id"), DiscordCoreClient::thisPointer);
						guildMemberUpdateData.guildMemberNew = guildMemberNew;
						this->eventManager->onGuildMemberUpdateEvent(guildMemberUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_CREATE:
					{
						RoleData roleData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &roleData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &role.data);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &inviteData);
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
						InteractionData interactionData;
						if (workload.payLoad.contains("user")) {
							InteractionData interactionDataNew(workload.payLoad.at("user").at("id").get<string>());
							interactionData = interactionDataNew;
						}
						else if (workload.payLoad.contains("member")) {
							InteractionData interactionDataNew(workload.payLoad.at("member").at("user").at("id").get<string>());
							interactionData = interactionDataNew;
						}
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &interactionData);
						InputEventData eventData;
						if (interactionData.type == InteractionType::ApplicationCommand) {
							if (workload.payLoad.at("data").at("type") == ApplicationCommandType::CHAT_INPUT) {
								eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.interactionData = interactionData;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.requesterId = interactionData.requesterId;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (workload.payLoad.at("data").at("type") == ApplicationCommandType::MESSAGE) {
								MessageCommandInteractionData dataPackage;
								DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage);
								interactionData.applicationId = dataPackage.applicationId;
								interactionData.channelId = dataPackage.channelId;
								interactionData.guildId = dataPackage.guildId;
								interactionData.id = dataPackage.interactionId;
								interactionData.member = dataPackage.messages.member;
								interactionData.message = dataPackage.messages;
								interactionData.name = dataPackage.name;
								interactionData.token = dataPackage.token;
								interactionData.user = dataPackage.messages.member.user;
								interactionData.version = dataPackage.version;
								eventData.eventType = InputEventType::MESSAGE_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.interactionData = interactionData;
								eventData.messageCommandInteractionData = dataPackage;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (workload.payLoad.at("data").at("type") == ApplicationCommandType::USER) {
								UserCommandInteractionData dataPackage;
								DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage);
								interactionData.applicationId = dataPackage.applicationId;
								interactionData.channelId = dataPackage.channelId;
								interactionData.guildId = dataPackage.guildId;
								interactionData.id = dataPackage.interactionId;
								interactionData.member = dataPackage.member;
								interactionData.name = dataPackage.name;
								interactionData.token = dataPackage.token;
								interactionData.user = dataPackage.users;
								interactionData.version = dataPackage.version;
								eventData.eventType = InputEventType::USER_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.interactionData = interactionData;
								eventData.userCommandInteractionData = dataPackage;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
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
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_CREATE:
					{
						MessageData messageData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &messageData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &messageData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &reactionAddDataNew);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &reactionRemoveDataNew);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("emoji"), &emojiData);
						reactionRemoveEmojiData.emoji = emojiData;
						this->eventManager->onReactionRemoveEmojiEvent(reactionRemoveEmojiData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::PRESENCE_UPDATE:
					{
						OnPresenceUpdateData presenceUpdateData;
						PresenceUpdateData newData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
						presenceUpdateData.presenceData = newData;
						this->eventManager->onPresenceUpdateEvent(presenceUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::TYPING_START:
					{
						OnTypingStartData typingStartData;
						TypingStartData newData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
						typingStartData.typingStartData = newData;
						this->eventManager->onTypingStartEvent(typingStartData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::USER_UPDATE:
					{
						OnUserUpdateData userUpdateData;
						UserData newData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
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
	unbounded_buffer<exception> DiscordCoreClient::errorBuffer{ nullptr };
	shared_ptr<DiscordCoreClient> DiscordCoreClient::thisPointer{ nullptr };

	class Guilds {
	public:
		static task<Guild> getGuildAsync(GetGuildData dataPackage) {
			return DiscordCoreClient::thisPointer->guilds->getGuildAsync(dataPackage);
		}

		static task<Guild> fetchGuildAsync(FetchGuildData dataPackage) {
			return DiscordCoreClient::thisPointer->guilds->fetchAsync(dataPackage);
		}

		static task<AuditLogData> fetchAuditLogDataAsync(FetchAuditLogData dataPackage) {
			return DiscordCoreClient::thisPointer->guilds->fetchAuditLogDataAsync(dataPackage);
		}

		static task<InviteData> fetchInviteAsync(FetchInviteData dataPackage) {
			return DiscordCoreClient::thisPointer->guilds->fetchInviteAsync(dataPackage);
		}

		static task<vector<InviteData>> fetchInvitesAsync(FetchInvitesData dataPackage) {
			return DiscordCoreClient::thisPointer->guilds->fetchInvitesAsync(dataPackage);
		}

		static task<InviteData> fetchVanityInviteAsync(FetchVanityInviteData dataPackage) {
			return DiscordCoreClient::thisPointer->guilds->fetchVanityInviteAsync(dataPackage);
		}

		static task<BanData> createGuildBanAsync(CreateGuildBanData dataPackage) {
			return DiscordCoreClient::thisPointer->guilds->createGuildBanAsync(dataPackage);
		}

		static task<vector<Guild>> getAllGuildsAsync() {
			return DiscordCoreClient::thisPointer->guilds->getAllGuildsAsync();
		}

		static task<void> insertGuildAsync(Guild guild) {
			return DiscordCoreClient::thisPointer->guilds->insertGuildAsync(guild);
		}

		static task<void> removeGuildAsync(Guild guild) {
			return DiscordCoreClient::thisPointer->guilds->removeGuildAsync(guild.data.id);
		}
	};

	class Channels {
	public:
		static task<Channel> getChannelAsync(GetChannelData dataPackage) {
			return DiscordCoreClient::thisPointer->channels->getChannelAsync(dataPackage);
		}

		static task<Channel> fetchChannelAsync(FetchChannelData dataPackage) {
			return DiscordCoreClient::thisPointer->channels->fetchAsync(dataPackage);
		}

		static task<void>editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage) {
			return DiscordCoreClient::thisPointer->channels->editChannelPermissionOverwritesAsync(dataPackage);
		}

		static task<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage) {
			return DiscordCoreClient::thisPointer->channels->deleteChannelPermissionOverwritesAsync(dataPackage);
		}

		static task<Channel> fetchDMChannelAsync(FetchDMChannelData dataPackage) {
			return DiscordCoreClient::thisPointer->channels->fetchDMChannelAsync(dataPackage);
		}

		static task<void> insertChannelAsync(Channel dataPackage) {
			return DiscordCoreClient::thisPointer->channels->insertChannelAsync(dataPackage);
		}

		static task<void> removeChannelAsync(string channelId) {
			return DiscordCoreClient::thisPointer->channels->removeChannelAsync(channelId);
		}
	};

	class Messages {
	public:
		static task<Message> fetchMessageAsync(FetchMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->fetchAsync(dataPackage);
		}

		static task<Message> createMessageAsync(CreateMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->createMessageAsync(dataPackage);
		}

		static task<void> deleteMessageAsync(DeleteMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->deleteMessageAsync(dataPackage);
		}

		static task<void> deleteMessagesBulkAsync(DeleteMessagesBulkData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->deleteMessagesBulkAsync(dataPackage);
		}

		static task<Message> editMessageAsync(EditMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->editMessageAsync(dataPackage);
		}

		static task<std::optional<vector<Message>>> fetchMessagesAsync(FetchMessagesData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->fetchMessagesAsync(dataPackage);
		}

		static task<vector<Message>> fetchPinnedMessagesAsync(FetchPinnedMessagesData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->fetchPinnedMessagesAsync(dataPackage);
		}

		static task<void> pinMessageAsync(PinMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->pinMessageAsync(dataPackage);
		}

		static task<Message> pinMessageAsync(ReplyMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->replyAsync(dataPackage);
		}

		static task<Message> pinMessageAsync(SendDMData dataPackage) {
			return DiscordCoreClient::thisPointer->messages->sendDMAsync(dataPackage);
		}
	};

	class GuildMembers {
	public:
		static task<GuildMember> getGuildMemberAsync(GetGuildMemberData dataPackage) {
			return DiscordCoreClient::thisPointer->guildMembers->getGuildMemberAsync(dataPackage);
		}

		static task<GuildMember> fetchGuildMemberAsync(FetchGuildMemberData dataPackage) {
			return DiscordCoreClient::thisPointer->guildMembers->fetchAsync(dataPackage);
		}

		static task<GuildMember> modifyGuildMemberAsync(ModifyGuildMemberData dataPackage) {
			return DiscordCoreClient::thisPointer->guildMembers->modifyGuildMemberAsync(dataPackage);
		}

		static task<void> insertGuildMemberAsync(GuildMember dataPackage) {
			return DiscordCoreClient::thisPointer->guildMembers->insertGuildMemberAsync(dataPackage, dataPackage.data.guildId);
		}

		static task<void> removeGuildMemberAsync(string guildId, string guildMemberId) {
			return DiscordCoreClient::thisPointer->guildMembers->removeGuildMemberAsync(guildId, guildMemberId);
		}
	};

	class Users {
	public:
		static task<User> getUserAsync(GetUserData dataPackage) {
			return DiscordCoreClient::thisPointer->users->getUserAsync(dataPackage);
		}

		static task<User> fetchUserAsync(FetchUserData dataPackage) {
			return DiscordCoreClient::thisPointer->users->fetchAsync(dataPackage);
		}
	};

	class Roles {
	public:
		static task<Role> getRoleAsync(GetRoleData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->getRoleAsync(dataPackage);
		}

		static task<void> addRoleToGuildMemberAsync(AddRoleToGuildMemberData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->addRoleToGuildMemberAsync(dataPackage);
		}

		static task<Role> createRoleAsync(CreateRoleData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->createRoleAsync(dataPackage);
		}

		static task<Role> fetchRoleAsync(FetchRoleData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->fetchAsync(dataPackage);
		}

		static vector<Role> getGuildMemberRoles(GetGuildMemberRolesData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->getGuildMemberRoles(dataPackage);
		}

		static task<vector<Role>> getGuildRolesAsync(GetGuildRolesData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->getGuildRolesAsync(dataPackage);
		}

		static task<void> removeRoleFromGuildAsync(RemoveRoleFromGuildData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->removeRoleFromGuildAsync(dataPackage);
		}

		static task<void> removeRoleFromGuildMemberAsync(RemoveRoleFromGuildMemberData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->removeRoleFromGuildMemberAsync(dataPackage);
		}

		static task<Role> updateRoleAsync(UpdateRoleData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->updateRoleAsync(dataPackage);
		}

		static vector<Role> updateRoleAsync(UpdateRolePositionData dataPackage) {
			return DiscordCoreClient::thisPointer->roles->updateRolePositions(dataPackage);
		}

		static task<void> insertRoleAsync(Role dataPackage) {
			return DiscordCoreClient::thisPointer->roles->insertRoleAsync(dataPackage);
		}

		static task<void> removeRoleAsync(string roleId) {
			return DiscordCoreClient::thisPointer->roles->removeRoleAsync(roleId);
		}
	};

	class SlashCommands {
	public:
		static task<ApplicationCommand> createGlobalApplicationCommandAsync(CreateApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->slashCommands->createGlobalApplicationCommandAsync(dataPackage);
		}

		static task<void> deleteGlobalApplicationCommandAsync(DeleteApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->slashCommands->deleteGlobalApplicationCommandAsync(dataPackage);
		}

		static task<ApplicationCommand> editGlobalApplicationCommandAsync(EditApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->slashCommands->editGlobalApplicationCommandAsync(dataPackage);
		}

		static vector<ApplicationCommand> getGlobalApplicationCommands() {
			return DiscordCoreClient::thisPointer->slashCommands->getGlobalApplicationCommands();
		}

		static task<void> editGlobalApplicationCommandAsync() {
			return DiscordCoreClient::thisPointer->slashCommands->displayGlobalApplicationCommandsAsync();
		}
	};

	class Reactions {
	public:
		static task<Reaction> createReactionAsync(CreateReactionData dataPackage) {
			return DiscordCoreClient::thisPointer->reactions->createReactionAsync(dataPackage);
		}

		static task<void> deleteAllReactionsAsync(DeleteAllReactionsData dataPackage) {
			return DiscordCoreClient::thisPointer->reactions->deleteAllReactionsAsync(dataPackage);
		}

		static task<void> deleteOwnReactionAsync(DeleteOwnReactionData dataPackage) {
			return DiscordCoreClient::thisPointer->reactions->deleteOwnReactionAsync(dataPackage);
		}

		static task<void> deleteReactionsByEmojiAsync(DeleteReactionsByEmojiData dataPackage) {
			return DiscordCoreClient::thisPointer->reactions->deleteReactionsByEmojiAsync(dataPackage);
		}

		static task<void> deleteUserReactionAsync(DeleteUserReactionData dataPackage) {
			return DiscordCoreClient::thisPointer->reactions->deleteUserReactionAsync(dataPackage);
		}
	};

	class Interactions {
	public:

		static task<MessageData> createInteractionResponseAsync(CreateInteractionResponseData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->createInteractionResponseAsync(dataPackage);
		}

		static task<MessageData> createFollowUpMessageAsync(CreateFollowUpMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->createFollowUpMessageAsync(dataPackage);
		}

		static task<void> createDeferredInteractionResponseAsync(CreateDeferredInteractionResponseData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->createDeferredInteractionResponseAsync(dataPackage);
		}

		static task<void> deleteFollowUpMessageAsync(DeleteFollowUpMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->deleteFollowUpMessageAsync(dataPackage);
		}

		static task<void> deleteInteractionResponseAsync(DeleteInteractionResponseData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->deleteInteractionResponseAsync(dataPackage);
		}

		static task<MessageData> editFollowUpMessageAsync(EditFollowUpMessageData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->editFollowUpMessageAsync(dataPackage);
		}

		static task<MessageData> editInteractionResponseAsync(EditInteractionResponseData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->editInteractionResponseAsync(dataPackage);
		}

		static task<InteractionResponseData> getInteractionResponseAsync(GetInteractionResponseData dataPackage) {
			return DiscordCoreClient::thisPointer->interactions->getInteractionResponseAsync(dataPackage);
		}
	};
}

#include "Commands/CommandsList.hpp"

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_CLOSE_EVENT) {
		exit(0);
	}
	return true;
};

#endif
