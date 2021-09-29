// DiscordCoreClient01.hpp - Header file for the main/exposed DiscordCoreClient class.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_01_
#define _DISCORD_CORE_CLIENT_01_

#include "../pch.h"
#include "ThreadManager.hpp"
#include "Http.hpp"
#include "GuildEntities.hpp"
#include "InteractionEntities.hpp"
#include "EventTypes.hpp"
#include "InputEvents.hpp"
#include "SongAPI.hpp"
#include "StickerEntities.hpp"
#include "WebSocketEntities.hpp"

void myPurecallHandler(void) {
	cout << "CURRENT THREAD: " << this_thread::get_id() << endl;
}

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);

namespace DiscordCoreAPI {

	class DiscordCoreClient : public DiscordCoreClientBase, DiscordCoreInternal::ThreadContext, agent {
	public:
		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend BOOL WINAPI::HandlerRoutine(_In_ DWORD dwCtrlType);
		friend DiscordUser getBotDiscordUserData();
		friend class PermissionsConverter;
		friend class ApplicationCommands;
		friend BotUser getBotUser();		
		friend class EventHandler;
		friend class GuildMembers;
		friend class Interactions;
		friend class Reactions;
		friend class Messages;
		friend class Stickers;
		friend class Channels;
		friend class Guilds;
		friend class Roles;
		friend class Users;
		friend class Guild;

		static DiscordCoreClient* thisPointer;

		shared_ptr<EventManager> eventManager{ nullptr };

		void finalSetup(string botTokenNew, string commandPrefix, vector<RepeatedFunctionData>* functionVector);

		void runBot() {
			this->start();
			if (this->functionsToExecute != nullptr) {
				for (auto value : *this->functionsToExecute) {
					executeFunctionAfterTimePeriod(value.function, (unsigned int)value.intervalInMs, value.repeated, this);
				}
			}
			wait(this);
		}

	protected:

		shared_ptr<DiscordCoreInternal::ApplicationCommandManager> applicationCommands{ nullptr };
		shared_ptr<DiscordCoreInternal::WebSocketReceiverAgent> webSocketReceiverAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::InteractionManager> interactions{ nullptr };
		shared_ptr<DiscordCoreInternal::ReactionManager> reactions{ nullptr };
		shared_ptr<DiscordCoreInternal::MessageManager> messages{ nullptr };
		shared_ptr<DiscordCoreInternal::StickerManager> stickers{ nullptr };
		shared_ptr<DiscordCoreInternal::GuildManager> guilds{ nullptr };
		vector<RepeatedFunctionData>* functionsToExecute{};
		shared_ptr<DiscordUser> discordUser{ nullptr };
		string baseURL{ "https://discord.com/api/v9" };
		bool doWeQuit{ false };
		string botToken;

		DiscordCoreClient(string botTokenNew) : DiscordCoreClientBase(), ThreadContext(*DiscordCoreInternal::ThreadManager::getThreadContext().get()), agent(*this->scheduler->scheduler) {
			this->botToken = botTokenNew;
		}

		task<void> initialize() {
			PHANDLER_ROUTINE handlerRoutine(&HandlerRoutine);
			SetConsoleCtrlHandler(handlerRoutine, true);
			_set_purecall_handler(myPurecallHandler);
			apartment_context mainThread;
			co_await resume_foreground(*this->dispatcherQueue.get());
			DiscordCoreClient::thisPointer = this;
			this->eventManager = make_shared<DiscordCoreAPI::EventManager>();
			DiscordCoreInternal::HttpRequestAgent::initialize(this->botToken, this->baseURL);
			SoundCloudSong::initialize();
			this->webSocketReceiverAgent = make_unique<DiscordCoreInternal::WebSocketReceiverAgent>();
			this->webSocketConnectionAgent = make_shared<DiscordCoreInternal::WebSocketConnectionAgent>(&this->webSocketReceiverAgent->webSocketWorkloadSource, this->botToken);
			SongAPI::initialize(getDiscordGuildMap());
			this->webSocketConnectionAgent->setSocketPath(this->getGateWayUrl());
			DiscordCoreInternal::InteractionManagerAgent::initialize();
			DiscordCoreInternal::GuildMemberManagerAgent::intialize();
			DiscordCoreInternal::ReactionManagerAgent::initialize();
			DiscordCoreInternal::ChannelManagerAgent::initialize();
			DiscordCoreInternal::MessageManagerAgent::initialize();
			DiscordCoreInternal::StickerManagerAgent::intialize();
			DiscordCoreInternal::GuildManagerAgent::initialize();
			DiscordCoreInternal::RoleManagerAgent::initialize();
			DiscordCoreInternal::UserManagerAgent::intialize();
			this->interactions = make_shared<DiscordCoreInternal::InteractionManager>(nullptr);
			this->guildMembers = make_shared<DiscordCoreInternal::GuildMemberManager>(nullptr);
			this->reactions = make_shared<DiscordCoreInternal::ReactionManager>(nullptr);
			this->channels = make_shared<DiscordCoreInternal::ChannelManager>(nullptr);
			this->messages = make_shared<DiscordCoreInternal::MessageManager>(nullptr);
			this->stickers = make_shared<DiscordCoreInternal::StickerManager>(nullptr);
			this->guilds = make_shared<DiscordCoreInternal::GuildManager>(nullptr);
			this->roles = make_shared<DiscordCoreInternal::RoleManager>(nullptr);
			this->users = make_shared<DiscordCoreInternal::UserManager>(nullptr);
			DiscordCoreClientBase::initialize();
			DatabaseManagerAgent::initialize(this->currentUser.id, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->discordUser = make_shared<DiscordUser>(this->currentUser.userName, this->currentUser.id);
			this->applicationCommands = make_shared<DiscordCoreInternal::ApplicationCommandManager>(nullptr);
			this->applicationCommands->initialize(this->discordUser->data.userId);
			ButtonCollector::initialize(this->interactions);
			SelectMenuCollector::initialize(this->interactions);
			InputEvents::initialize(this->messages, this->interactions);
			DiscordCoreAPI::commandPrefix = this->discordUser->data.prefix;
			this->discordUser->writeDataToDB();
			this->webSocketReceiverAgent->start();
			this->webSocketConnectionAgent->start();
			co_await mainThread;
			co_return;
		}

		string getGateWayUrl() {
			DiscordCoreInternal::HttpAgentResources agentResources{};
			agentResources.headers.push_back(DiscordCoreInternal::Headers{ .headerType = DiscordCoreInternal::HeaderTypes::Bot_Auth,.headerValue = this->botToken });
			agentResources.baseURL = this->baseURL;
			DiscordCoreInternal::HttpRequestAgent requestAgent(agentResources);
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
			getAudioBuffersMap()->erase(guildData.id);
		}

		void run() {
			startingPoint:
			while (!this->doWeQuit) {
				try {
					DiscordCoreInternal::WebSocketWorkload workload{};
					while (!try_receive(this->webSocketReceiverAgent->webSocketWorkloadTarget, workload)) {
						concurrency::wait(50);
						if (this->doWeQuit) {
							goto startingPoint;
						}
					}
					switch (workload.eventType) {
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Create:
					{
						OnApplicationCommandCreationData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.applicationCommand);
						this->eventManager->onApplicationCommandCreationEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Update:
					{
						OnApplicationCommandUpdateData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.applicationCommand);
						this->eventManager->onApplicationCommandUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Delete:
					{
						OnApplicationCommandDeletionData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.applicationCommand);
						this->eventManager->onApplicationCommandDeletionEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Create:
					{
						OnChannelCreationData channelCreationData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelCreationData.channel);
						this->eventManager->onChannelCreationEvent(channelCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Update:
					{
						OnChannelUpdateData channelUpdateData{};
						channelUpdateData.channelOld = this->channels->getChannelAsync({ .channelId = workload.payLoad.at("id") }).get();
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelUpdateData.channelNew);
						this->eventManager->onChannelUpdateEvent(channelUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Delete:
					{
						OnChannelDeletionData channelDeleteData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &channelDeleteData.channel);
						this->eventManager->onChannelDeletionEvent(channelDeleteData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Pins_Update:
					{
						OnChannelPinsUpdateData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.dataPackage);
						this->eventManager->onChannelPinsUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Create:
					{
						OnThreadCreationData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.channel);
						this->eventManager->onThreadCreationEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Update:
					{
						OnThreadUpdateData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.channel);
						this->eventManager->onThreadUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Delete:
					{
						OnThreadDeletionData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.channel);
						this->eventManager->onThreadDeletionEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_List_Sync:
					{
						OnThreadListSyncData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.threadListSyncData);
						this->eventManager->onThreadListSyncEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Member_Update:
					{
						OnThreadMemberUpdateData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.threadMember);
						this->eventManager->onThreadMemberUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Members_Update:
					{
						OnThreadMembersUpdateData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.threadMembersUpdateData);
						this->eventManager->onThreadMembersUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Create:
					{
						OnGuildCreationData guildCreationData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildCreationData.guild);
						guildCreationData.guild = createGuild(guildCreationData.guild);
						this->eventManager->onGuildCreationEvent(guildCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Update:
					{
						DiscordCoreAPI::OnGuildUpdateData guildUpdateData{};
						guildUpdateData.guildOld = this->guilds->getGuildAsync({ .guildId = workload.payLoad.at("id") }).get();
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildUpdateData.guildNew);
						this->eventManager->onGuildUpdateEvent(guildUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Delete:
					{
						DiscordCoreAPI::OnGuildDeletionData guildDeletionData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildDeletionData.guild);
						removeGuild(guildDeletionData.guild);
						this->eventManager->onGuildDeletionEvent(guildDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Ban_Add:
					{
						DiscordCoreAPI::OnGuildBanAddData guildBanAddData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &guildBanAddData.user);
						guildBanAddData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildBanAddEvent(guildBanAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Ban_Remove:
					{
						DiscordCoreAPI::OnGuildBanRemoveData guildBanRemoveData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &guildBanRemoveData.user);
						guildBanRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildBanRemoveEvent(guildBanRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Emojis_Update:
					{
						OnGuildEmojisUpdateData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.updateData);
						this->eventManager->onGuildEmojisUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Stickers_Update:
					{
						OnGuildStickersUpdateData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.updateData);
						this->eventManager->onGuildStickersUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Integrations_Update:
					{
						OnGuildIntegrationsUpdateData dataPackage{};
						dataPackage.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildIntegrationsUpdateEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Add:
					{
						DiscordCoreAPI::OnGuildMemberAddData guildMemberAddData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberAddData.guildMember);
						guildMemberAddData.guildMember.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildMemberAddEvent(guildMemberAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Remove:
					{
						DiscordCoreAPI::OnGuildMemberRemoveData guildMemberRemoveData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("user"), &guildMemberRemoveData.user);
						guildMemberRemoveData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildMemberRemoveEvent(guildMemberRemoveData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Update:
					{
						DiscordCoreAPI::OnGuildMemberUpdateData guildMemberUpdateData{};
						guildMemberUpdateData.guildMemberOld = this->guildMembers->getGuildMemberAsync({ .guildMemberId = workload.payLoad.at("user").at("id"),.guildId = workload.payLoad.at("guild_id") }).get();
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &guildMemberUpdateData.guildMemberNew);
						guildMemberUpdateData.guildMemberNew.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onGuildMemberUpdateEvent(guildMemberUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Members_Chunk:
					{
						OnGuildMembersChunkData dataPackage{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &dataPackage.chunkEventData);
						this->eventManager->onGuildMembersChunkEvent(dataPackage);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Create:
					{
						DiscordCoreAPI::OnRoleCreationData roleCreationData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &roleCreationData.role);
						roleCreationData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onRoleCreationEvent(roleCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Update:
					{
						DiscordCoreAPI::OnRoleUpdateData roleUpdateData{};
						roleUpdateData.roleOld = this->roles->getRoleAsync({ .guildId = workload.payLoad.at("guild_id"), .roleId = workload.payLoad.at("role").at("id") }).get();
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("role"), &roleUpdateData.roleNew);
						roleUpdateData.guildId = workload.payLoad.at("guild_id");
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
					case DiscordCoreInternal::WebSocketEventType::Integration_Create:
					{
						OnIntegrationCreationData integrationCreationData{};
						integrationCreationData.guildId = workload.payLoad.at("guild_id");
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("integration"), &integrationCreationData.integrationData);
						this->eventManager->onIntegrationCreationEvent(integrationCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Integration_Update:
					{
						OnIntegrationUpdateData integrationCreationData{};
						integrationCreationData.guildId = workload.payLoad.at("guild_id");
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("integration"), &integrationCreationData.integrationData);
						this->eventManager->onIntegrationUpdateEvent(integrationCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Integration_Delete:
					{
						OnIntegrationDeletionData integrationCreationData{};
						integrationCreationData.guildId = workload.payLoad.at("guild_id");
						integrationCreationData.applicationId = workload.payLoad.at("application_id");
						integrationCreationData.id = workload.payLoad.at("id");
						this->eventManager->onIntegrationDeletionEvent(integrationCreationData);
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
						if (interactionData.type == InteractionType::APPLICATION_COMMAND) {
							if (interactionData.data.applicationCommanddata.type == ApplicationCommandType::CHAT_INPUT) {
								eventData.eventType = InputEventType::SLASH_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.requesterId = interactionData.requesterId;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.data.applicationCommanddata.type == ApplicationCommandType::MESSAGE) {
								eventData.eventType = InputEventType::MESSAGE_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.data.applicationCommanddata.type == ApplicationCommandType::USER) {
								eventData.eventType = InputEventType::USER_COMMAND_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::UNSET;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
						}
						else if (interactionData.type == InteractionType::MESSAGE_COMPONENT) {
							if (interactionData.data.componentData.componentType == ComponentType::Button) {
								eventData.eventType = InputEventType::BUTTON_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
								eventData.requesterId = interactionData.requesterId;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.data.componentData.componentType == ComponentType::SelectMenu) {
								eventData.eventType = InputEventType::SELECT_MENU_INPUT;
								eventData.inputEventResponseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
								eventData.requesterId = interactionData.requesterId;
								eventData.interactionData = interactionData;
								OnInteractionCreationData eventCreationData{};
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
						}
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Invite_Create:
					{
						OnInviteCreationData inviteCreationData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &inviteCreationData.invite);
						inviteCreationData.discordCoreClient = this;
						this->eventManager->onInviteCreationEvent(inviteCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Invite_Delete:
					{
						OnInviteDeletionData inviteDeletionData{};
						inviteDeletionData.discordCoreClient = this;
						inviteDeletionData.channelId = workload.payLoad.at("channel_id");
						inviteDeletionData.guildId = workload.payLoad.at("guild_id");
						inviteDeletionData.code = workload.payLoad.at("code");
						this->eventManager->onInviteDeletionEvent(inviteDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Create:
					{
						MessageData messageData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &messageData);
						if (messageData.interaction.id != "") {
							if (DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.contains(messageData.interaction.id)) {
								MessageData messageData01{};
								while (try_receive(*DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.at(messageData.interaction.id), messageData01)) {};
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &messageData);
						if (messageData.interaction.id != "") {
							if (DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.contains(messageData.interaction.id)) {
								send(*DiscordCoreInternal::InteractionManagerAgent::collectMessageDataBuffers.at(messageData.interaction.id), messageData);
							}
						}
						DiscordCoreAPI::OnMessageUpdateData messageUpdateData{};
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
						OnReactionAddData reactionAddData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &reactionAddData.reaction);
						this->eventManager->onReactionAddEvent(reactionAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Remove:
					{
						OnReactionRemoveData reactionRemoveData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &reactionRemoveData.reactionRemoveData);
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
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad.at("emoji"), &reactionRemoveEmojiData.emoji);
						this->eventManager->onReactionRemoveEmojiEvent(reactionRemoveEmojiData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Presence_Update:
					{
						OnPresenceUpdateData presenceUpdateData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &presenceUpdateData.presenceData);
						this->eventManager->onPresenceUpdateEvent(presenceUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Stage_Instance_Create:
					{
						OnStageInstanceCreationData newData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData.stageInstance);
						this->eventManager->onStageInstanceCreationEvent(newData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Stage_Instance_Update:
					{
						OnStageInstanceUpdateData newData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData.stageInstance);
						this->eventManager->onStageInstanceUpdateEvent(newData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Stage_Instance_Delete:
					{
						OnStageInstanceDeletionData newData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &newData.stageInstance);
						this->eventManager->onStageInstanceDeletionEvent(newData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Typing_Start:
					{
						OnTypingStartData typingStartData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &typingStartData.typingStartData);
						this->eventManager->onTypingStartEvent(typingStartData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::User_Update:
					{
						OnUserUpdateData userUpdateData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &userUpdateData.userNew);
						userUpdateData.userOld = this->users->getUserAsync({ .userId = userUpdateData.userNew.id }).get();
						this->users->insertUserAsync(userUpdateData.userNew).get();
						this->eventManager->onUserUpdateEvent(userUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Voice_State_Update:
					{
						OnVoiceStateUpdateData voiceStateUpdateData{};
						DiscordCoreInternal::DataParser::parseObject(workload.payLoad, &voiceStateUpdateData.voiceStateData);
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
					case DiscordCoreInternal::WebSocketEventType::Webhooks_Update:
					{
						OnWebhookUpdateData newData{};
						newData.channelId = workload.payLoad.at("channel_id");
						newData.guildId = workload.payLoad.at("guild_id");
						this->eventManager->onWebhookUpdateEvent(newData);
						break;
					}
					default:
					{
						break;
					}
					}
				}
				catch (...) {
					rethrowException("DiscordCoreClient::run() Error: ");
				}
			}
			done();
		}

		void terminate() {
			this->doWeQuit = true;
			SelectMenuCollector::cleanup();
			ButtonCollector::cleanup();
			DiscordCoreInternal::UserManagerAgent::cleanup();
			DiscordCoreInternal::RoleManagerAgent::cleanup();
			DiscordCoreInternal::GuildManagerAgent::cleanup();
			DiscordCoreInternal::StickerManagerAgent::cleanup();
			DiscordCoreInternal::MessageManagerAgent::cleanup();
			DiscordCoreInternal::ChannelManagerAgent::cleanup();
			DiscordCoreInternal::ReactionManagerAgent::cleanup();
			DiscordCoreInternal::GuildMemberManagerAgent::cleanup();
			DiscordCoreInternal::InteractionManagerAgent::cleanup();
			DatabaseManagerAgent::cleanup();
			YouTubeAPI::cleanup();
			SoundCloudAPI::cleanup();
			SongAPI::cleanup();
			this->webSocketConnectionAgent->terminate();
			this->webSocketReceiverAgent->terminate();
			wait(this->webSocketConnectionAgent.get());
			wait(this->webSocketReceiverAgent.get());
		}

		~DiscordCoreClient() {
			DiscordCoreClient::terminate();
		}

	};
	DiscordCoreClient* DiscordCoreClient::thisPointer{ nullptr };

	BotUser getBotUser() {
		return DiscordCoreClient::thisPointer->currentUser;
	}

	DiscordUser getBotDiscordUserData() {
		DiscordCoreClient::thisPointer->discordUser->getDataFromDB();
		return *DiscordCoreClient::thisPointer->discordUser;
	}

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

		static task<ApplicationCommand> editGuildApplicationCommandAsync(EditGuildApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->editGuildApplicationCommandAsync(dataPackage);
		}

		static void deleteGuildApplicationCommand(DeleteGuildApplicationCommandData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->deleteGuildApplicationCommand(dataPackage);
		}

		static task<vector<ApplicationCommand>>  bulkOverwriteGuildApplicationCommandsAsync(BulkOverwriteGuildApplicationCommandsData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->bulkOverwriteGuildApplicationCommandsAsync(dataPackage);
		};

		static task<vector<GuildApplicationCommandPermissionsData>> getGuildApplicationCommandPermissionsAsync(GetGuildApplicationCommandPermissionsData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->getGuildApplicationCommandPermissionsAsync(dataPackage);
		}

		static GuildApplicationCommandPermissionsData getApplicationCommandPermissions(GetApplicationCommandPermissionsData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->getApplicationCommandPermissions(dataPackage);
		}

		static GuildApplicationCommandPermissionsData editApplicationCommandPermissions(EditApplicationCommandPermissionsData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->editApplicationCommandPermissions(dataPackage);
		}

		static vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData> batchEditApplicationCommandPermissions(BatchEditGuildApplicationCommandPermissionsData dataPackage) {
			return DiscordCoreClient::thisPointer->applicationCommands->batchEditApplicationCommandPermissions(dataPackage);
		}

	};

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

		static task<void> insertUserAsync(User user) {
			return DiscordCoreClient::thisPointer->users->insertUserAsync(user);
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
		DiscordCoreAPI::DiscordCoreClient::thisPointer->terminate();
	}
	return true;
};

#endif
