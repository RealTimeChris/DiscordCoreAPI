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

	class DiscordCoreClient :public DiscordCoreClientBase, protected agent {
	public:
		shared_ptr<BotUser> currentUser{ nullptr };
		shared_ptr<GuildManager> guilds{ nullptr };
		shared_ptr<ReactionManager> reactions{ nullptr };
		shared_ptr<MessageManager> messages{ nullptr };
		shared_ptr<SlashCommandManager> slashCommands{ nullptr };
		shared_ptr<EventManager> eventManager{ nullptr };
		shared_ptr<DiscordUser> discordUser{ nullptr };
		shared_ptr<InteractionManager> interactions{ nullptr };
		shared_ptr<DiscordCoreClient> thisPointer{ nullptr };
		DiscordCoreInternal::HttpAgentResources agentResources;

		DiscordCoreClient(hstring botTokenNew) :agent(*DiscordCoreInternal::ThreadManager::getThreadContext().get()->scheduler) {
			this->botToken = botTokenNew;
		}

		DiscordGuild getDiscordGuild(GuildData guildData) {
			auto guildCursor = DiscordCoreClient::guildMap.find(guildData.id);
			if (guildCursor == DiscordCoreClient::guildMap.end()) {
				DiscordGuild discordGuild(guildData);
				DiscordCoreClient::guildMap.insert(make_pair(guildData.id, discordGuild));
				discordGuild.getDataFromDB();
				return discordGuild;
			}
			else {
				guildCursor->second.getDataFromDB();
				return guildCursor->second;
			}
		}

		DiscordGuildMember getDiscordGuildMember(DiscordCoreInternal::GuildMemberData guildMemberData) {
			auto guildMemberCursor = DiscordCoreClient::guildMemberMap.find(guildMemberData.guildId + " + " + guildMemberData.user.id);
			if (guildMemberCursor == DiscordCoreClient::guildMemberMap.end()) {
				DiscordGuildMember discordGuildMember(guildMemberData);
				DiscordCoreClient::guildMemberMap.insert(make_pair(guildMemberData.guildId + " + " + guildMemberData.user.id, discordGuildMember));
				discordGuildMember.getDataFromDB();
				return discordGuildMember;
			}
			else {
				guildMemberCursor->second.getDataFromDB();
				return guildMemberCursor->second;
			}
		}

		static shared_ptr<DiscordCoreAPI::DiscordCoreClient> finalSetup(string botToken);

		bool getError(exception& error) {
			if (try_receive(errorBuffer, error)) {
				return true;
			}
			return false;
		}

		void terminate() {
			DatabaseManagerAgent::cleanup();
			InputEventManager::cleanup();
			this->pWebSocketReceiverAgent->terminate();
			agent::wait(this->pWebSocketReceiverAgent.get());
			exception error;
			while (this->pWebSocketReceiverAgent->getError(error)) {
				cout << "DiscordCoreClient::terminate() Error 01: " << error.what() << endl << endl;
			}
			while (this->pWebSocketConnectionAgent->getError(error)) {
				cout << "DiscordCoreClient::terminate() Error 02: " << error.what() << endl << endl;
			}
			this->doWeQuit = true;
		}

	protected:
		friend class BotUser;
		bool doWeQuit = false;
		hstring botToken;
		hstring baseURL = L"https://discord.com/api/v9";
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgent{ nullptr };
		shared_ptr<DiscordCoreInternal::WebSocketReceiverAgent> pWebSocketReceiverAgent{ nullptr };
		unbounded_buffer<json> webSocketIncWorkloadBuffer;
		unbounded_buffer<DiscordCoreInternal::WebSocketWorkload> webSocketWorkCollectionBuffer;
		unbounded_buffer<exception> errorBuffer;
		shared_ptr<DiscordCoreInternal::ThreadContext> mainThreadContext;

		task<void> initialize() {
			thisPointer.reset(this);
			_set_purecall_handler(myPurecallHandler);
			apartment_context mainThread;
			this->mainThreadContext = DiscordCoreInternal::ThreadManager::getThreadContext().get();
			this->mainThreadContext->createGroup();
			co_await resume_foreground(*this->mainThreadContext->dispatcherQueue.get());
			this->eventManager = make_shared<DiscordCoreAPI::EventManager>();

			this->pWebSocketConnectionAgent = make_shared<DiscordCoreInternal::WebSocketConnectionAgent>(this->webSocketIncWorkloadBuffer, this->botToken, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->agentResources.baseURL = this->baseURL;
			this->agentResources.botToken = this->botToken;
			DiscordCoreInternal::HttpRequestAgent::initialize();
			DiscordCoreInternal::HttpRequestAgent requestAgent(this->agentResources);
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_SOCKET_PATH;
			workload.relativePath = "/gateway/bot";
			send(requestAgent.workSubmissionBuffer, workload);
			requestAgent.start();
			agent::wait(&requestAgent);
			exception error;
			while (requestAgent.getError(error)) {
				cout << "DiscordCoreClient::initialize() Error: " << error.what() << endl << endl;
			}
			DiscordCoreInternal::HttpData returnData;
			try_receive(requestAgent.workReturnBuffer, returnData);
			this->pWebSocketConnectionAgent->setSocketPath(returnData.data.dump());
			this->pWebSocketReceiverAgent = make_shared<DiscordCoreInternal::WebSocketReceiverAgent>(this->webSocketIncWorkloadBuffer, this->webSocketWorkCollectionBuffer, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->interactions = make_shared<InteractionManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			this->reactions = make_shared<ReactionManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->users = make_shared<UserManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->messages = make_shared<MessageManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->roles = make_shared<RoleManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->guildMembers = make_shared<GuildMemberManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->channels = make_shared<ChannelManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->thisPointer);
			this->guilds = make_shared<GuildManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), (shared_ptr<DiscordCoreClient>)this->thisPointer, (shared_ptr<DiscordCoreClientBase>)this->thisPointer);
			this->currentUser = make_shared<BotUser>(this->users->fetchCurrentUserAsync().get().data, this->thisPointer, this->pWebSocketConnectionAgent);
			this->slashCommands = make_shared<SlashCommandManager>(agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->currentUser->data.id);
			DatabaseManagerAgent::initialize(this->currentUser->data.id, DiscordCoreInternal::ThreadManager::getThreadContext().get());
			Button::initialize(this->interactions);
			this->discordUser = make_shared<DiscordUser>(this->currentUser->data.username, this->currentUser->data.id);
			DiscordCoreAPI::commandPrefix = this->discordUser->data.prefix;
			InputEventManager::initialize(this->messages, this->thisPointer, agentResources, DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->interactions);
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
				this->currentUser->updatePresenceAsync({ .activities = activities, .status = "online",.afk = false }).get();
				}, 5000);
			co_await mainThread;
			co_return;
		}

		Guild createGuild(GuildData guildData) {
			Guild guild(this->agentResources, guildData, (shared_ptr<DiscordCoreClient>)this->thisPointer, this->thisPointer);
			DiscordGuild discordGuild(guild.data);
			discordGuild.writeDataToDB();
			if (DiscordCoreClient::guildMap.contains(guild.data.id)) {
				DiscordCoreClient::guildMap.erase(guild.data.id);
				this->discordUser->data.guildCount -= 1;
				this->discordUser->writeDataToDB();
			}
			else {
				this->discordUser->data.guildCount += 1;
				this->discordUser->writeDataToDB();
				DiscordCoreClient::guildMap.insert(make_pair(guild.data.id, discordGuild));
			}
			return guild;
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
						Channel channel(channelData, this->thisPointer);
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
						Channel channel(channelData, this->thisPointer);
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
						Guild guild = createGuild(guildData);
						DiscordCoreAPI::OnGuildDeletionData guildDeletionData;
						guildDeletionData.guild = guild;
						this->eventManager->onGuildDeletionEvent(guildDeletionData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_BAN_ADD:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, this->thisPointer);
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
						User user(userData, this->thisPointer);
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
						GuildMember guildMember(guildMemberData, workload.payLoad.at("guild_id").get<string>(), this->thisPointer);
						DiscordCoreAPI::OnGuildMemberAddData guildMemberAddData;
						guildMemberAddData.guildMember = guildMember;
						this->eventManager->onGuildMemberAddEvent(guildMemberAddData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::GUILD_MEMBER_REMOVE:
					{
						UserData userData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("user"), &userData);
						User user(userData, this->thisPointer);
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
						DiscordCoreInternal::parseObject(workload.payLoad, &guildMemberOld.data);
						guildMemberUpdateData.guildMemberNew = guildMemberOld;
						this->eventManager->onGuildMemberUpdateEvent(guildMemberUpdateData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::ROLE_CREATE:
					{
						RoleData roleData;
						DiscordCoreInternal::parseObject(workload.payLoad.at("role"), &roleData);
						Role role(roleData, this->thisPointer);
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
								eventData.discordCoreClient = this->thisPointer;
								eventData.requesterId = interactionData.requesterId;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.type == InteractionType::MessageComponent) {
								eventData.eventType = InputEventType::BUTTON_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
								eventData.interactionData = interactionData;
								eventData.discordCoreClient = this->thisPointer;
								eventData.requesterId = interactionData.requesterId;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
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
								eventData.discordCoreClient = this->thisPointer;
								eventData.requesterId = interactionData.requesterId;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							else if (interactionData.type == InteractionType::MessageComponent) {
								eventData.eventType = InputEventType::BUTTON_INTERACTION;
								eventData.inputEventResponseType = InputEventResponseType::DEFER_BUTTON_RESPONSE;
								eventData.interactionData = interactionData;
								eventData.discordCoreClient = this->thisPointer;
								eventData.requesterId = interactionData.requesterId;
								OnInteractionCreationData eventCreationData;
								eventCreationData.eventData = eventData;
								this->eventManager->onInteractionCreationEvent(eventCreationData);
							}
							break;
						}
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_CREATE:
					{
						MessageData messageData;
						DiscordCoreInternal::parseObject(workload.payLoad, &messageData);
						Message message(messageData, this->thisPointer);
						OnMessageCreationData messageCreationData;
						messageCreationData.message = message;
						this->eventManager->onMessageCreationEvent(messageCreationData);
						InputEventData eventData;
						eventData.eventType = InputEventType::REGULAR_MESSAGE;
						eventData.messageData = messageData;
						eventData.inputEventResponseType = InputEventResponseType::REGULAR_MESSAGE_RESPONSE;
						eventData.requesterId = messageData.author.id;
						eventData.discordCoreClient = this->thisPointer;
						OnInteractionCreationData eventCreationData;
						eventCreationData.eventData = eventData;
						this->eventManager->onInteractionCreationEvent(eventCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::MESSAGE_UPDATE:
					{
						DiscordCoreAPI::OnMessageUpdateData messageUpdateData;
						Message message = this->messages->getMessageAsync({ .channelId = workload.payLoad.at("channel_id"), .id = workload.payLoad.at("id") }).get();
						messageUpdateData.messageOld = message;
						DiscordCoreInternal::parseObject(workload.payLoad, &message.data);
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
						messageDeletionData.discordCoreClient = this->thisPointer;
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
						messageDeleteBulkData.discordCoreClient = this->thisPointer;
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
						Reaction reaction(reactionData, this->thisPointer);
						OnReactionAddData reactionAddData;
						reactionAddData.discordCoreClient = this->thisPointer;
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
						reactionRemoveAllData.discordCoreClient = this->thisPointer;
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
						reactionRemoveEmojiData.discordCoreClient = this->thisPointer;
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
						User userNew(newData, this->thisPointer);
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
				send(errorBuffer, e);
			}
			done();
			this->terminate();
			return;
		}
	};
	map<string, DiscordGuild> DiscordCoreClientBase::guildMap;
	map<string, DiscordGuildMember> DiscordCoreClientBase::guildMemberMap;
	shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient;
}
#include "Commands/CommandsList.hpp"

#endif
