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
#include "GuildStuff.hpp"
#include "InteractionStuff.hpp"
#include "EventStuff01.hpp"
#include "ApplicationCommandStuff.hpp"
#include "InputEventStuff.hpp"
#include "DatabaseStuff.hpp"
#include "YouTubeStuff.hpp"
#include "SoundCloudStuff.hpp"
#include "WebSocketStuff.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
}

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

namespace DiscordCoreAPI {

	class DiscordCoreClient : public DiscordCoreClientBase, protected agent, enable_shared_from_this<DiscordCoreClient> {
	public:
		friend class PermissionsConverter;
		friend class ApplicationCommands;
		friend class GuildMembers;
		friend class Interactions;
		friend class Reactions;
		friend class Channels;
		friend class Messages;
		friend class Guilds;
		friend class Roles;
		friend class Users;

		static shared_ptr<DiscordCoreClient> thisPointer;
		shared_ptr<EventManager> eventManager{ nullptr };
		shared_ptr<DiscordUser> discordUser{ nullptr };

		DiscordCoreClient(hstring botTokenNew) : DiscordCoreClientBase(), agent(*DiscordCoreInternal::ThreadManager::getThreadContext().get()->scheduler->ptrScheduler) {
			this->botToken = botTokenNew;
		}

		DiscordCoreClient() {}

		static void finalSetup(string botToken, string commandPrefix, vector<RepeatedFunctionData>* functionVector);

		static void runBot() {
			wait((agent*)DiscordCoreClient::thisPointer.get());
			DiscordCoreClient::thisPointer->getError();
		}

		static void terminate() {
			DiscordCoreClient::thisPointer->doWeQuit = true;
			DatabaseManagerAgent::cleanup();
			DiscordCoreClient::thisPointer->webSocketConnectionAgent->terminate();
			DiscordCoreClient::thisPointer->webSocketReceiverAgent->terminate();
			wait(DiscordCoreClient::thisPointer->webSocketConnectionAgent.get());
			wait(DiscordCoreClient::thisPointer->webSocketReceiverAgent.get());
			DiscordCoreClientBase::currentUser.~BotUser();
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
			DiscordCoreInternal::InteractionManagerAgent::cleanup();
			DiscordCoreInternal::MessageManagerAgent::cleanup();
			DiscordCoreInternal::GuildManagerAgent::cleanup();
			DiscordCoreInternal::RoleManagerAgent::cleanup();
			DiscordCoreInternal::UserManagerAgent::cleanup();
			DiscordCoreInternal::ReactionManagerAgent::cleanup();
			DiscordCoreInternal::ChannelManagerAgent::cleanup();
			DiscordCoreInternal::GuildMemberManagerAgent::cleanup();
			DiscordCoreClient::thisPointer->mainThreadContext->releaseGroup();
		}

		~DiscordCoreClient() {
			DiscordCoreClient::terminate();
		}

	protected:
		shared_ptr<DiscordCoreInternal::WebSocketReceiverAgent> webSocketReceiverAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::ApplicationCommandManager> applicationCommands{ nullptr };
		shared_ptr<DiscordCoreInternal::ThreadContext> mainThreadContext{ nullptr };
		shared_ptr<DiscordCoreInternal::InteractionManager> interactions{ nullptr };
		shared_ptr<DiscordCoreInternal::ReactionManager> reactions{ nullptr };
		shared_ptr<DiscordCoreInternal::MessageManager> messages{ nullptr };
		shared_ptr<DiscordCoreInternal::GuildManager> guilds{ nullptr };
		hstring gatewayBaseURL{ L"wss://gateway.discord.gg/?v=9" };
		DiscordCoreInternal::HttpAgentResources agentResources{};
		unbounded_buffer<exception> errorBuffer{ nullptr };
		hstring baseURL{ L"https://discord.com/api/v9" };
		bool doWeQuit{ false };

		task<void> initialize() {
			PHANDLER_ROUTINE handlerRoutine(&HandlerRoutine);
			SetConsoleCtrlHandler(handlerRoutine, true);
			_set_purecall_handler(myPurecallHandler);
			DiscordCoreClientBase::thisPointerBase.reset(this);
			apartment_context mainThread;
			this->mainThreadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
			co_await resume_foreground(*this->mainThreadContext->dispatcherQueue.get());
			this->eventManager = make_shared<DiscordCoreAPI::EventManager>();
			this->webSocketReceiverAgent = make_unique<DiscordCoreInternal::WebSocketReceiverAgent>(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreClientBase::webSocketConnectionAgent = make_shared<DiscordCoreInternal::WebSocketConnectionAgent>(&this->webSocketReceiverAgent->workloadSource, this->botToken, DiscordCoreInternal::ThreadManager::getThreadContext().get());
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
			DiscordCoreInternal::GuildMemberManagerAgent::intialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::ChannelManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::ReactionManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::UserManagerAgent::intialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::RoleManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::GuildManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::MessageManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			DiscordCoreInternal::InteractionManagerAgent::initialize(DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->thisPointerBase->initialize(this->agentResources, this->thisPointer);
			this->users = this->thisPointerBase->users;
			this->roles = this->thisPointerBase->roles;
			this->guildMembers = this->thisPointerBase->guildMembers;
			this->channels = this->thisPointerBase->channels;
			this->webSocketConnectionAgent->setSocketPath(returnData.data.dump());
			this->interactions = make_shared<DiscordCoreInternal::InteractionManager>(nullptr);
			this->interactions->initialize(this->agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->reactions = make_shared<DiscordCoreInternal::ReactionManager>(nullptr);
			this->reactions->initialize(this->agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->messages = make_shared<DiscordCoreInternal::MessageManager>(nullptr);
			this->messages->initialize(this->agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->guilds = make_shared<DiscordCoreInternal::GuildManager>(nullptr);
			this->guilds->initialize(this->agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer, this->thisPointerBase);
			DatabaseManagerAgent::initialize(this->thisPointerBase->currentUser.id, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->discordUser = make_shared<DiscordUser>(this->thisPointerBase->currentUser.username, this->thisPointerBase->currentUser.id);
			this->applicationCommands = make_shared<DiscordCoreInternal::ApplicationCommandManager>(nullptr);
			this->applicationCommands->initialize(this->agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->discordUser->data.userId);
			ButtonManager::initialize(this->interactions);
			SelectMenuManager::initialize(this->interactions);
			InputEvents::initialize(DiscordCoreClient::thisPointerBase, DiscordCoreClient::thisPointer, this->messages, this->interactions);
			DiscordCoreAPI::commandPrefix = this->discordUser->data.prefix;
			this->discordUser->writeDataToDB();
			this->webSocketReceiverAgent->start();
			this->webSocketConnectionAgent->start();
			this->start();
			co_await mainThread;
			co_return;
		}

		Guild createGuild(GuildData guildData) {
			auto valueNew = this->guilds->getAllGuildsAsync().get();
			bool isItFound = false;
			Guild guild(guildData);
			for (auto value : valueNew) {
				if (guildData.id == value.id) {
					isItFound = true;
					DiscordGuild discordGuild(value);
					discordGuild.getDataFromDB();
					discordGuild.writeDataToDB();
					guild = value;
				}
			}
			if (!isItFound) {
				this->discordUser->data.guildCount += 1;
				this->discordUser->writeDataToDB();
			}
			return guild;
		}

		void removeGuild(GuildData guildData) {
			this->discordUser->writeDataToDB();
			Guild guild = this->guilds->fetchAsync({ .guildId = guildData.id }).get();
			if (guild.id == "") {
				this->discordUser->data.guildCount -= 1;
				this->discordUser->writeDataToDB();
			}
			this->audioBuffersMap.erase(guildData.id);
		}

		void getError() {
			exception error;
			while (try_receive(this->errorBuffer, error)) {
				cout << "DiscordCoreClient Error: " << error.what() << endl;
			}
		}

		void run() {
			try {
				while (doWeQuit == false) {
					DiscordCoreInternal::WebSocketWorkload workload = receive(this->webSocketReceiverAgent->workloadTarget, INFINITE);
					switch (workload.eventType) {
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_CREATE:
					{
						ChannelData channelData;
						channelData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channel);
						channelUpdateData.channelNew = channel;
						this->eventManager->onChannelUpdateEvent(channelUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::CHANNEL_DELETE:
					{
						ChannelData channelData;
						channelData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData);
						OnChannelDeletionData channelDeleteData;
						channelDeleteData.channel = channel;
						this->eventManager->onChannelDeletionEvent(channelDeleteData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_CREATE:
					{
						GuildData guildData;
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						guildData.discordCoreClientBase = DiscordCoreClient::thisPointerBase;
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
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						guildData.discordCoreClientBase = DiscordCoreClient::thisPointerBase;
						DiscordCoreAPI::OnGuildUpdateData guildUpdateData;
						Guild guild = this->guilds->getGuildAsync({ .guildId = workload.payLoad.at("id") }).get();
						guildUpdateData.guildOld = guild;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guild);
						guildUpdateData.guildNew = guild;
						this->eventManager->onGuildUpdateEvent(guildUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_DELETE:
					{
						GuildData guildData;
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						guildData.discordCoreClientBase = DiscordCoreClient::thisPointerBase;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildData);
						removeGuild(guildData);
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						guildData.discordCoreClientBase = DiscordCoreClient::thisPointerBase;
						Guild guild(guildData);
						DiscordCoreAPI::OnGuildDeletionData guildDeletionData;
						guildDeletionData.guild = guild;
						this->eventManager->onGuildDeletionEvent(guildDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_BAN_ADD:
					{
						UserData userData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData);
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData;
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
						User user(userData);
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData;
						DiscordCoreAPI::OnGuildBanRemoveData guildBanRemoveData;
						guildBanRemoveData.user = user;
						guildBanRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildBanRemoveEvent(guildBanRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_ADD:
					{
						GuildMemberData guildMemberData;
						guildMemberData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMember(guildMemberData, workload.payLoad.at("guild_id").get<string>());
						DiscordCoreAPI::OnGuildMemberAddData guildMemberAddData;
						guildMemberAddData.guildMember = guildMember;
						this->eventManager->onGuildMemberAddEvent(guildMemberAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_REMOVE:
					{
						UserData userData;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
						userData.discordCoreClient = DiscordCoreClient::thisPointer;
						User user(userData);
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
						guildMemberData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMemberNew(guildMemberData, workload.payLoad.at("guild_id").get<string>());
						guildMemberUpdateData.guildMemberNew = guildMemberNew;
						this->eventManager->onGuildMemberUpdateEvent(guildMemberUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_CREATE:
					{
						RoleData roleData;
						roleData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &roleData);
						Role role(roleData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &role);
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
						if (DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.contains(messageData.interaction.id)) {
							send(*DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.at(messageData.interaction.id), messageData);
						}
						messageData.discordCoreClient = DiscordCoreClient::thisPointer;
						Message message(messageData);
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
						messageData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &messageData);
						Message message(messageData);
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
						reactionData.discordCoreClient = DiscordCoreClient::thisPointer;
						Reaction reaction(reactionData);
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
						newData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
						User userOld = this->users->getUserAsync({ .userId = newData.id }).get();
						User userNew(newData);
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
				send(this->errorBuffer, e);
			}
			done();
			return;
		}

	};
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
			return DiscordCoreClient::thisPointer->guilds->removeGuildAsync(guild.id);
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
			return DiscordCoreClient::thisPointer->guildMembers->insertGuildMemberAsync(dataPackage, dataPackage.guildId);
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

	class ApplicationCommands {
	public:
		static task<vector<ApplicationCommand>> getGlobalApplicationCommandsAsync() {
			return DiscordCoreClient::thisPointer->applicationCommands->getGlobalApplicationCommandsAsync();
		}

		static task<ApplicationCommand> createGlobalApplicationCommandAsync(CreateApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->createGlobalApplicationCommandAsync(dataPackage);
		}

		static task<ApplicationCommand> getGlobalApplicationCommandAsync(GetGlobalApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->getGlobalApplicationCommandAsync(dataPackage);
		}

		static task<ApplicationCommand> editGlobalApplicationCommandAsync(EditApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->editGlobalApplicationCommandAsync(dataPackage);
		}

		static task<void> deleteGlobalApplicationCommandAsync(DeleteApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->deleteGlobalApplicationCommandAsync(dataPackage);
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
