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
#include "InputEventStuff.hpp"
#include "DatabaseStuff.hpp"
#include "SongAPI.hpp"
#include "StickerStuff.hpp"
#include "WebSocketStuff.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
}

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

namespace DiscordCoreAPI {

	class DiscordCoreClient : public DiscordCoreClientBase, DiscordCoreInternal::ThreadContext, agent {
	public:

		friend class PermissionsConverter;
		friend class ApplicationCommands;
		friend class EventHandler;
		friend class GuildMembers;
		friend class Interactions;
		friend class YouTubeAPI;
		friend class Reactions;
		friend class Channels;
		friend class Messages;
		friend class Guilds;
		friend class Roles;
		friend class Users;

		static shared_ptr<DiscordCoreClient> thisPointer;
		shared_ptr<EventManager> eventManager{ nullptr };
		shared_ptr<DiscordUser> discordUser{ nullptr };

		DiscordCoreClient(string botTokenNew) : DiscordCoreClientBase(), ThreadContext(*DiscordCoreInternal::ThreadManager::getThreadContext().get()), agent(*this->scheduler->scheduler) {
			this->botToken = botTokenNew;
		}

		static void finalSetup(string botToken, string commandPrefix, vector<RepeatedFunctionData>* functionVector);

		static void runBot() {
			wait((agent*)DiscordCoreClient::thisPointer.get());
			if (DiscordCoreClient::thisPointer->doWeQuitWebSocket) {
				cout << "It appears as though there was a websocket issue!" << endl << endl;
			}
			DiscordCoreClient::thisPointer->getError();
		}

		static void terminate() {
			DiscordCoreClient::thisPointer->doWeQuit = true;
			SelectMenu::cleanup();
			Button::cleanup();
			DatabaseManagerAgent::cleanup();
			DiscordCoreInternal::UserManagerAgent::cleanup();
			DiscordCoreInternal::RoleManagerAgent::cleanup();
			DiscordCoreInternal::GuildManagerAgent::cleanup();
			DiscordCoreInternal::StickerManagerAgent::cleanup();
			DiscordCoreInternal::MessageManagerAgent::cleanup();
			DiscordCoreInternal::ChannelManagerAgent::cleanup();
			DiscordCoreInternal::ReactionManagerAgent::cleanup();
			DiscordCoreInternal::GuildMemberManagerAgent::cleanup();
			DiscordCoreInternal::InteractionManagerAgent::cleanup();
			DiscordCoreClient::thisPointer->webSocketConnectionAgent->terminate();
			DiscordCoreClient::thisPointer->webSocketReceiverAgent->terminate();
			wait(DiscordCoreClient::thisPointer->webSocketConnectionAgent.get());
			wait(DiscordCoreClient::thisPointer->webSocketReceiverAgent.get());
			exit(0);
		}

		~DiscordCoreClient() {
			DiscordCoreClient::terminate();
		}

	protected:

		shared_ptr<DiscordCoreInternal::WebSocketReceiverAgent> webSocketReceiverAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::ApplicationCommandManager> applicationCommands{ nullptr };
		shared_ptr<DiscordCoreInternal::InteractionManager> interactions{ nullptr };
		shared_ptr<DiscordCoreInternal::ReactionManager> reactions{ nullptr };
		shared_ptr<DiscordCoreInternal::MessageManager> messages{ nullptr };
		shared_ptr<DiscordCoreInternal::StickerManager> stickers{ nullptr };
		shared_ptr<DiscordCoreInternal::GuildManager> guilds{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources{};
		unbounded_buffer<exception> errorBuffer{ nullptr };
		string baseURL{ "https://discord.com/api/v9" };
		map<string, DiscordGuild*> discordGuildMap{};
		map<string, vector<Song>> youtubeQueueMap{};
		bool doWeQuitWebSocket{ false };
		bool doWeQuit{ false };
		string botToken{ "" };

		task<void> initialize() {
			PHANDLER_ROUTINE handlerRoutine(&HandlerRoutine);
			SetConsoleCtrlHandler(handlerRoutine, true);
			_set_purecall_handler(myPurecallHandler);
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			this->eventManager = make_shared<DiscordCoreAPI::EventManager>();
			DiscordCoreInternal::HttpRequestAgent::initialize(this->botToken, this->baseURL);
			SoundCloudSong::initialize(SoundCloudAPI::baseSearchURL, SoundCloudAPI::baseSearchURL02);
			this->webSocketReceiverAgent = make_unique<DiscordCoreInternal::WebSocketReceiverAgent>();
			DiscordCoreClientBase::webSocketConnectionAgent = make_shared<DiscordCoreInternal::WebSocketConnectionAgent>(&this->webSocketReceiverAgent->webSocketWorkloadSource, this->botToken, &this->doWeQuitWebSocket);
			this->webSocketConnectionAgent->setSocketPath(this->getGateWayUrl());
			DiscordCoreInternal::InteractionManagerAgent::initialize(this->agentResources);
			DiscordCoreInternal::GuildMemberManagerAgent::intialize(this->agentResources, this->thisPointer);
			DiscordCoreInternal::ReactionManagerAgent::initialize(this->agentResources, this->thisPointer);
			DiscordCoreInternal::ChannelManagerAgent::initialize(this->agentResources, this->thisPointer);
			DiscordCoreInternal::MessageManagerAgent::initialize(this->agentResources, this->thisPointer);
			DiscordCoreInternal::StickerManagerAgent::intialize(this->agentResources, this->thisPointer);
			DiscordCoreInternal::GuildManagerAgent::initialize(this->agentResources, this->thisPointer, this->thisPointer);
			DiscordCoreInternal::RoleManagerAgent::initialize(this->agentResources, this->thisPointer);
			DiscordCoreInternal::UserManagerAgent::intialize(this->agentResources, this->thisPointer);
			this->interactions = make_shared<DiscordCoreInternal::InteractionManager>(nullptr);
			this->interactions->initialize(this->agentResources);
			this->guildMembers = make_shared<DiscordCoreInternal::GuildMemberManager>(nullptr);
			this->guildMembers->initialize(agentResources, this->thisPointer);
			this->reactions = make_shared<DiscordCoreInternal::ReactionManager>(nullptr);
			this->reactions->initialize(this->agentResources, this->thisPointer);
			this->channels = make_shared<DiscordCoreInternal::ChannelManager>(nullptr);
			this->channels->initialize(agentResources, this->thisPointer);
			this->messages = make_shared<DiscordCoreInternal::MessageManager>(nullptr);
			this->messages->initialize(this->agentResources, this->thisPointer);
			this->stickers = make_shared<DiscordCoreInternal::StickerManager>(nullptr);
			this->stickers->initialize(this->agentResources, this->thisPointer);
			this->guilds = make_shared<DiscordCoreInternal::GuildManager>(nullptr);
			this->guilds->initialize(this->agentResources, this->thisPointer, make_shared<DiscordCoreClientBase>((DiscordCoreClientBase)*this));
			this->roles = make_shared<DiscordCoreInternal::RoleManager>(nullptr);
			this->roles->initialize(this->agentResources, this->thisPointer);
			this->users = make_shared<DiscordCoreInternal::UserManager>(nullptr);
			this->users->initialize(agentResources, this->thisPointer);
			DiscordCoreClientBase::initialize();
			DatabaseManagerAgent::initialize(this->currentUser.id, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			SongAPI::initialize(DiscordCoreClientBase::youtubeAPIMap, DiscordCoreClientBase::soundCloudAPIMap, DiscordCoreClientBase::songAPIMap, DiscordCoreClientBase::audioBuffersMap, &this->discordGuildMap, DiscordCoreClientBase::voiceConnectionMap);
			this->discordUser = make_shared<DiscordUser>(this->currentUser.userName, this->currentUser.id);
			this->applicationCommands = make_shared<DiscordCoreInternal::ApplicationCommandManager>(nullptr);
			this->applicationCommands->initialize(this->agentResources, this->discordUser->data.userId);
			Button::initialize(this->interactions);
			SelectMenu::initialize(this->interactions);
			InputEvents::initialize(make_shared<DiscordCoreClientBase>((DiscordCoreClientBase)*this), DiscordCoreClient::thisPointer, this->messages, this->interactions);
			DiscordCoreAPI::commandPrefix = this->discordUser->data.prefix;
			this->discordUser->writeDataToDB();
			this->webSocketReceiverAgent->start();
			this->webSocketConnectionAgent->start();
			this->start();
			co_await mainThread;
			co_return;
		}

		string getGateWayUrl() {
			this->agentResources.headers.push_back(DiscordCoreInternal::Headers{ .headerType = DiscordCoreInternal::HeaderTypes::Bot_Auth,.headerValue = this->botToken });
			this->agentResources.baseURL = this->baseURL;
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
			DiscordCoreInternal::HttpWorkloadData workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SOCKET_PATH;
			workload.relativePath = "/gateway/bot";
			DiscordCoreInternal::HttpData returnData = requestAgent.submitWorkloadAndGetResult(workload, "DiscordCoreClient::getGateWayUrl()");
			return returnData.data.dump();
		}

		Guild createGuild(GuildData guildData) {
			auto valueNew = this->guilds->getAllGuildsAsync().get();
			bool isItFound = false;
			Guild guild(guildData);
			for (auto value : valueNew) {
				DiscordGuild discordGuild(value);
				if (guildData.id == value.id) {
					isItFound = true;
					discordGuild.getDataFromDB();
					discordGuild.writeDataToDB();
					guild = value;
					SongAPI::discordGuildMap->insert(make_pair(discordGuild.data.guildId, &discordGuild));
				}
				this->youtubeQueueMap.insert_or_assign(discordGuild.data.guildId, discordGuild.data.playlist.songList);
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
			this->audioBuffersMap->erase(guildData.id);
		}

		void getError() {
			exception error;
			while (try_receive(this->errorBuffer, error)) {
				cout << "DiscordCoreClient::run() Error: " << error.what() << endl << endl;
			}
		}

		void run() {
			try {
			startingPoint:
				while (!this->doWeQuit && !this->doWeQuitWebSocket) {
					DiscordCoreInternal::WebSocketWorkload workload{};
					while (!try_receive(this->webSocketReceiverAgent->webSocketWorkloadTarget, workload)) {
						concurrency::wait(50);
						if (this->doWeQuit || this->doWeQuitWebSocket) {
							goto startingPoint;
						}
					}
					switch (workload.eventType) {
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Create:
					{
						ApplicationCommandData appCommandData{};
						ApplicationCommand appCommand(appCommandData);
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &appCommandData);
						OnApplicationCommandCreationData dataPackage{};
						dataPackage.applicationCommand = appCommand;
						this->eventManager->onApplicationCommandCreationEvent(dataPackage);
					}
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Update:
					{
						ApplicationCommandData appCommandData{};
						ApplicationCommand appCommand(appCommandData);
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &appCommandData);
						OnApplicationCommandUpdateData dataPackage{};
						dataPackage.applicationCommand = appCommand;
						this->eventManager->onApplicationCommandUpdateEvent(dataPackage);
					}
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Delete:
					{
						ApplicationCommandData appCommandData{};
						ApplicationCommand appCommand(appCommandData);
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &appCommandData);
						OnApplicationCommandDeletionData dataPackage{};
						dataPackage.applicationCommand = appCommand;
						this->eventManager->onApplicationCommandDeletionEvent(dataPackage);
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Create:
					{
						ChannelData channelData{};
						channelData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData);
						OnChannelCreationData channelCreationData{};
						channelCreationData.channel = channel;
						this->eventManager->onChannelCreationEvent(channelCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Update:
					{
						OnChannelUpdateData channelUpdateData{};
						Channel channel = this->channels->getChannelAsync({ .channelId = workload.payLoad.at("id") }).get();
						channelUpdateData.channelOld = channel;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channel);
						channelUpdateData.channelNew = channel;
						this->eventManager->onChannelUpdateEvent(channelUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Delete:
					{
						ChannelData channelData{};
						channelData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelData);
						Channel channel(channelData);
						OnChannelDeletionData channelDeleteData{};
						channelDeleteData.channel = channel;
						this->eventManager->onChannelDeletionEvent(channelDeleteData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Pins_Update:
					{
						OnChannelPinsUpdateEventData dataPackage{};
						ChannelPinsUpdateEventData dataPackage02{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage02);
						dataPackage.dataPackage = dataPackage02;
						this->eventManager->onChannelPinsUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Create:
					{
						GuildData guildData{};
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						guildData.discordCoreClientBase = make_shared<DiscordCoreClientBase>((DiscordCoreClientBase)*this);
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildData);
						Guild guild = createGuild(guildData);
						DiscordCoreAPI::OnGuildCreationData guildCreationData{};
						guildCreationData.guild = guild;
						this->eventManager->onGuildCreationEvent(guildCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Update:
					{
						GuildData guildData{};
						guildData.discordCoreClientBase = make_shared<DiscordCoreClientBase>((DiscordCoreClientBase)*this);
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreAPI::OnGuildUpdateData guildUpdateData{};
						Guild guild = this->guilds->getGuildAsync({ .guildId = workload.payLoad.at("id") }).get();
						guildUpdateData.guildOld = guild;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guild);
						guildUpdateData.guildNew = guild;
						this->eventManager->onGuildUpdateEvent(guildUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Delete:
					{
						GuildData guildData{};
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						guildData.discordCoreClientBase = make_shared<DiscordCoreClientBase>((DiscordCoreClientBase)*this);
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildData);
						removeGuild(guildData);
						guildData.discordCoreClientBase = make_shared<DiscordCoreClientBase>((DiscordCoreClientBase)*this);
						guildData.discordCoreClient = DiscordCoreClient::thisPointer;
						Guild guild(guildData);
						DiscordCoreAPI::OnGuildDeletionData guildDeletionData{};
						guildDeletionData.guild = guild;
						this->eventManager->onGuildDeletionEvent(guildDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Ban_Add:
					{
						UserData userData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData);
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData{};
						DiscordCoreAPI::OnGuildBanAddData guildBanAddData{};
						guildBanAddData.guildId = workload.payLoad.at("guild_id");
						guildBanAddData.user = user;
						this->eventManager->onGuildBanAddEvent(guildBanAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Ban_Remove:
					{
						UserData userData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData);
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData{};
						DiscordCoreAPI::OnGuildBanRemoveData guildBanRemoveData{};
						guildBanRemoveData.guildId = workload.payLoad.at("guild_id");
						guildBanRemoveData.user = user;
						this->eventManager->onGuildBanRemoveEvent(guildBanRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Add:
					{
						GuildMemberData guildMemberData{};
						guildMemberData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMember(guildMemberData, workload.payLoad.at("guild_id").get<string>());
						DiscordCoreAPI::OnGuildMemberAddData guildMemberAddData{};
						guildMemberAddData.guildMember = guildMember;
						this->eventManager->onGuildMemberAddEvent(guildMemberAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Remove:
					{
						UserData userData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &userData);
						userData.discordCoreClient = DiscordCoreClient::thisPointer;
						User user(userData);
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData{};
						guildMemberRemoveData.user = user;
						guildMemberRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildMemberRemoveEvent(guildMemberRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Update:
					{
						DiscordCoreAPI::OnGuildMemberUpdateData guildMemberUpdateData{};
						GuildMember guildMemberOld = this->guildMembers->getGuildMemberAsync({ .guildMemberId = workload.payLoad.at("user").at("id"),.guildId = workload.payLoad.at("guild_id") }).get();
						guildMemberUpdateData.guildMemberOld = guildMemberOld;
						GuildMemberData guildMemberData{};
						guildMemberData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberData);
						GuildMember guildMemberNew(guildMemberData, workload.payLoad.at("guild_id").get<string>());
						guildMemberUpdateData.guildMemberNew = guildMemberNew;
						this->eventManager->onGuildMemberUpdateEvent(guildMemberUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Create:
					{
						RoleData roleData{};
						roleData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &roleData);
						Role role(roleData);
						DiscordCoreAPI::OnRoleCreationData roleCreationData{};
						roleCreationData.guildId = workload.payLoad.at("guild_id");
						roleCreationData.role = role;
						this->eventManager->onRoleCreationEvent(roleCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Update:
					{
						DiscordCoreAPI::OnRoleUpdateData roleUpdateData{};
						Role role = this->roles->getRoleAsync({ .guildId = workload.payLoad.at("guild_id"), .roleId = workload.payLoad.at("role").at("id") }).get();
						roleUpdateData.roleOld = role;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &role);
						roleUpdateData.guildId = workload.payLoad.at("guild_id");
						roleUpdateData.roleNew = role;
						this->eventManager->onRoleUpdateEvent(roleUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Delete:
					{
						DiscordCoreAPI::OnRoleDeletionData roleDeletionData{};
						roleDeletionData.guildId = workload.payLoad.at("guild_id");
						roleDeletionData.roleOld = this->roles->getRoleAsync({ .guildId = roleDeletionData.guildId, .roleId = workload.payLoad.at("role_id") }).get();
						this->eventManager->onRoleDeletionEvent(roleDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Invite_Create:
					{
						InviteData inviteData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &inviteData);
						OnInviteCreationData inviteCreationData{};
						inviteCreationData.discordCoreClient = DiscordCoreClient::thisPointer;
						inviteCreationData.invite = inviteData;
						this->eventManager->onInviteCreationEvent(inviteCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Invite_Delete:
					{
						string channelId = workload.payLoad.at("channel_id");
						string guildId = workload.payLoad.at("guild_id");
						string code = workload.payLoad.at("code");
						OnInviteDeletionData inviteDeletionData{};
						inviteDeletionData.discordCoreClient = DiscordCoreClient::thisPointer;
						inviteDeletionData.channelId = channelId;
						inviteDeletionData.guildId = guildId;
						inviteDeletionData.code = code;
						this->eventManager->onInviteDeletionEvent(inviteDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Interaction_Create:
					{
						InteractionData interactionData{};
						if (workload.payLoad.contains("user")) {
							InteractionData interactionDataNew(workload.payLoad.at("user").at("id").get<string>());
							interactionData = interactionDataNew;
						}
						else if (workload.payLoad.contains("member")) {
							InteractionData interactionDataNew(workload.payLoad.at("member").at("user").at("id").get<string>());
							interactionData = interactionDataNew;
						}
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &interactionData);
						InputEventData eventData{};
						if (interactionData.type == InteractionType::ApplicationCommand) {
							if (workload.payLoad.at("data").at("type") == ApplicationCommandType::CHAT_INPUT) {
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.requesterId = interactionData.requesterId;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (workload.payLoad.at("data").at("type") == ApplicationCommandType::MESSAGE) {
								MessageCommandInteractionData dataPackage;
								DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage);
								interactionData.applicationId = dataPackage.applicationId;
								interactionData.user = dataPackage.messages.member.user;
								interactionData.member = dataPackage.messages.member;
								interactionData.channelId = dataPackage.channelId;
								interactionData.message = dataPackage.messages;
								interactionData.id = dataPackage.interactionId;
								interactionData.guildId = dataPackage.guildId;
								interactionData.version = dataPackage.version;
								interactionData.token = dataPackage.token;
								interactionData.name = dataPackage.name;
								eventData.eventType = InputEventType::MESSAGE_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.messageCommandInteractionData = dataPackage;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (workload.payLoad.at("data").at("type") == ApplicationCommandType::USER) {
								UserCommandInteractionData dataPackage;
								DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage);
								interactionData.applicationId = dataPackage.applicationId;
								interactionData.channelId = dataPackage.channelId;
								interactionData.id = dataPackage.interactionId;
								interactionData.version = dataPackage.version;
								interactionData.guildId = dataPackage.guildId;
								interactionData.member = dataPackage.member;
								interactionData.token = dataPackage.token;
								interactionData.user = dataPackage.users;
								interactionData.name = dataPackage.name;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.eventType = InputEventType::USER_COMMAND_INTERACTION;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.userCommandInteractionData = dataPackage;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
						}
						else if (interactionData.type == InteractionType::MessageComponent) {
							if (interactionData.componentType == ComponentType::Button) {
								eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.eventType = InputEventType::BUTTON_INTERACTION;
								eventData.requesterId = interactionData.requesterId;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.componentType == ComponentType::SelectMenu) {
								eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
								eventData.discordCoreClient = DiscordCoreClient::thisPointer;
								eventData.eventType = InputEventType::SELECT_MENU_INPUT;
								eventData.requesterId = interactionData.requesterId;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
						}
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Create:
					{
						MessageData messageData{};
						messageData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &messageData);
						if (messageData.interaction.id != "") {
							if (DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.contains(messageData.interaction.id)) {
								send(*DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.at(messageData.interaction.id), messageData);
							}
						}
						Message message(messageData);
						if (MessageCollector::messagesBufferMap.size() > 0) {
							for (auto [key, value] : MessageCollector::messagesBufferMap) {
								send(value, message);
							}
						}
						OnMessageCreationData messageCreationData{};
						messageCreationData.message = message;
						this->eventManager->onMessageCreationEvent(messageCreationData);
						InputEventData eventData{};
						eventData.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
						eventData.discordCoreClient = DiscordCoreClient::thisPointer;
						eventData.eventType = InputEventType::REGULAR_MESSAGE;
						eventData.requesterId = messageData.author.id;
						eventData.messageData = messageData;
						OnInteractionCreationData eventCreationData{};
						eventCreationData.eventData = eventData;
						this->eventManager->onInteractionCreationEvent(eventCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Update:
					{
						MessageData messageData{};
						messageData.discordCoreClient = DiscordCoreClient::thisPointer;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &messageData);
						if (messageData.interaction.id != "") {
							if (DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.contains(messageData.interaction.id)) {
								send(*DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.at(messageData.interaction.id), messageData);
							}
						}
						DiscordCoreAPI::OnMessageUpdateData messageUpdateData{};
						messageData.discordCoreClient = DiscordCoreClient::thisPointer;
						Message message(messageData);
						messageUpdateData.messageNew = message;
						this->eventManager->onMessageUpdateEvent(messageUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Delete:
					{
						OnMessageDeletionData messageDeletionData{};
						if (workload.payLoad.contains("guild_id")) {
							messageDeletionData.guildId = workload.payLoad.at("guild_id");
						}
						messageDeletionData.channelId = workload.payLoad.at("channel_id");
						messageDeletionData.messageId = workload.payLoad.at("id");
						this->eventManager->onMessageDeletionEvent(messageDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Delete_Bulk:
					{
						OnMessageDeleteBulkData messageDeleteBulkData{};
						messageDeleteBulkData.channelId = workload.payLoad.at("channel_id");
						messageDeleteBulkData.guildId = workload.payLoad.at("guild_id");
						for (auto value : workload.payLoad.at("ids")) {
							messageDeleteBulkData.ids.push_back(value);
						}
						this->eventManager->onMessageDeleteBulkEvent(messageDeleteBulkData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Add:
					{
						ReactionAddData reactionAddDataNew;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &reactionAddDataNew);
						ReactionData reactionData{};
						reactionData.discordCoreClient = DiscordCoreClient::thisPointer;
						reactionData.channelId = reactionAddDataNew.channelId;
						reactionData.messageId = reactionAddDataNew.messageId;
						reactionData.guildId = reactionAddDataNew.guildId;
						reactionData.member = reactionAddDataNew.member;
						reactionData.userId = reactionAddDataNew.userId;
						reactionData.emoji = reactionAddDataNew.emoji;
						Reaction reaction(reactionData);
						OnReactionAddData reactionAddData{};
						reactionAddData.reaction = reaction;
						this->eventManager->onReactionAddEvent(reactionAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Remove:
					{
						ReactionRemoveData reactionRemoveDataNew;
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &reactionRemoveDataNew);
						reactionRemoveDataNew.discordCoreClient = this;
						OnReactionRemoveData reactionRemoveData{};
						reactionRemoveData.reactionRemoveData = reactionRemoveDataNew;
						this->eventManager->onReactionRemoveEvent(reactionRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Remove_All:
					{
						OnReactionRemoveAllData reactionRemoveAllData{};
						reactionRemoveAllData.channelId = workload.payLoad.at("channel_id");
						reactionRemoveAllData.messageId = workload.payLoad.at("message_id");
						reactionRemoveAllData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onReactionRemoveAllEvent(reactionRemoveAllData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Remove_Emoji:
					{
						OnReactionRemoveEmojiData reactionRemoveEmojiData{};
						reactionRemoveEmojiData.channelId = workload.payLoad.at("channel_id");
						reactionRemoveEmojiData.messageId = workload.payLoad.at("message_id");
						reactionRemoveEmojiData.guildId = workload.payLoad.at("guild_id");
						EmojiData emojiData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("emoji"), &emojiData);
						reactionRemoveEmojiData.emoji = emojiData;
						this->eventManager->onReactionRemoveEmojiEvent(reactionRemoveEmojiData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Presence_Update:
					{
						OnPresenceUpdateData presenceUpdateData{};
						PresenceUpdateData newData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
						presenceUpdateData.presenceData = newData;
						this->eventManager->onPresenceUpdateEvent(presenceUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Typing_Start:
					{
						OnTypingStartData typingStartData{};
						TypingStartData newData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
						typingStartData.typingStartData = newData;
						this->eventManager->onTypingStartEvent(typingStartData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::User_Update:
					{
						OnUserUpdateData userUpdateData{};
						UserData newData{};
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
					case DiscordCoreInternal::WebSocketEventType::Voice_State_Update:
					{
						OnVoiceStateUpdateData voiceStateUpdateData{};
						VoiceStateData newData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData);
						voiceStateUpdateData.voiceStateData = newData;
						this->eventManager->onVoiceStateUpdateEvent(voiceStateUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Voice_Server_Update:
					{
						OnVoiceServerUpdateData voiceServerUpdateData{};
						voiceServerUpdateData.endpoint = workload.payLoad.at("endpoint");
						voiceServerUpdateData.guildId = workload.payLoad.at("guild_id");
						voiceServerUpdateData.token = workload.payLoad.at("token");
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
			catch (...) {
				rethrowException("DiscordCoreClient::run() Error: ", &this->errorBuffer);
			}
			done();
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

		static void deleteGlobalApplicationCommand(DeleteApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->deleteGlobalApplicationCommand(dataPackage);
		}

		static task<vector<ApplicationCommand>>  bulkOverwriteApplicationCommandsAsync(BulkOverwriteApplicationCommandsData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->bulkOverwriteApplicationCommandsAsync(dataPackage);
		}

		static task<vector<ApplicationCommand>> getGuildApplicationCommandsAsync(GetGuildApplicationCommandsData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->getGuildApplicationCommandsAsync(dataPackage);
		}

		static task<ApplicationCommand> createGuildApplicationCommandAsync(CreateApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->createGuildApplicationCommandAsync(dataPackage);
		}

		static task<ApplicationCommand> getGuildApplicationCommandAsync(GetGuildApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->getGuildApplicationCommandAsync(dataPackage);
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
		DiscordCoreAPI::DiscordCoreClient::terminate();
	}
	return true;
};

#endif
