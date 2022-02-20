/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// DiscordCoreClient01.cpp - Source file for the main/exposed DiscordCoreClient class DiscordCoreAPI_Dll.
/// May 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <DiscordCoreClient.hpp>
#include <CoRoutine.hpp>
#include <Http.hpp>
#include <UserEntities.hpp>
#include <DataParsingFunctions.hpp>
#include <RoleEntities.hpp>
#include <InteractionEntities.hpp>
#include <ReactionEntities.hpp>
#include <MessageEntities.hpp>
#include <InputEvents.hpp>
#include <StickerEntities.hpp>
#include <ReactionEntities.hpp>
#include <ApplicationCommandEntities.hpp>
#include <CommandController.hpp>

namespace DiscordCoreAPI {

	bool doWeQuit{ false };

	std::unordered_map<std::string, TSUnboundedMessageBlock<AudioFrameData>*>* getAudioBufferMap() {
		return &Statics::audioBufferMap;
	}
	
	std::unordered_map<std::string, std::unique_ptr<VoiceConnection>>* getVoiceConnectionMap() {
		return &Statics::voiceConnectionMap;
	}

	std::unordered_map<std::string, std::unique_ptr<SoundCloudAPI>>* getSoundCloudAPIMap() {
		return &Statics::soundCloudAPIMap;
	}

	std::unordered_map<std::string, std::unique_ptr<YouTubeAPI>>* getYouTubeAPIMap() {
		return &Statics::youtubeAPIMap;
	}

	std::unordered_map<std::string, std::unique_ptr<SongAPI>>* getSongAPIMap() {
		return &Statics::songAPIMap;
	}

	DiscordCoreClient::DiscordCoreClient(std::string botTokenNew, std::string commandPrefixNew, std::vector<RepeatedFunctionData> functionsToExecuteNew,  CacheOptions cacheOptionsNew) {
#ifdef _WIN32
		SetConsoleCtrlHandler(CtrlHandler, true);
#endif
		this->functionsToExecute = functionsToExecuteNew;
		this->commandPrefix = commandPrefixNew;
		this->botToken = botTokenNew;
		this->cacheOptions = cacheOptionsNew;
		this->eventManager = std::make_unique<EventManager>();
		this->eventManager->onChannelCreation(&EventHandler::onChannelCreation);
		this->eventManager->onChannelUpdate(&EventHandler::onChannelUpdate);
		this->eventManager->onChannelDeletion(&EventHandler::onChannelDeletion);
		this->eventManager->onGuildCreation(&EventHandler::onGuildCreation);
		this->eventManager->onGuildUpdate(&EventHandler::onGuildUpdate);
		this->eventManager->onGuildDeletion(&EventHandler::onGuildDeletion);
		this->eventManager->onGuildMemberAdd(&EventHandler::onGuildMemberAdd);
		this->eventManager->onGuildMemberRemove(&EventHandler::onGuildMemberRemove);
		this->eventManager->onGuildMemberUpdate(&EventHandler::onGuildMemberUpdate);
		this->eventManager->onRoleCreation(&EventHandler::onRoleCreation);
		this->eventManager->onRoleUpdate(&EventHandler::onRoleUpdate);
		this->eventManager->onRoleDeletion(&EventHandler::onRoleDeletion);
		this->eventManager->onVoiceStateUpdate(&EventHandler::onVoiceStateUpdate);
		this->httpClient = std::make_unique<DiscordCoreInternal::HttpClient>(this->botToken);
		ApplicationCommands::initialize(this->httpClient.get());
		Channels::initialize(this->httpClient.get());
		Guilds::initialize(this->httpClient.get(), this);
		GuildMembers::initialize(this->httpClient.get());
		GuildScheduledEvents::initialize(this->httpClient.get());
		Interactions::initialize(this->httpClient.get());
		Messages::initialize(this->httpClient.get());
		Reactions::initialize(this->httpClient.get());
		Roles::initialize(this->httpClient.get());
		StageInstances::initialize(this->httpClient.get());
		Stickers::initialize(this->httpClient.get());
		Threads::initialize(this->httpClient.get());
		Users::initialize(this->httpClient.get());
		WebHooks::initialize(this->httpClient.get());
		this->commandController = CommandController(this->commandPrefix, this);
		this->baseSocketAgent = std::make_unique<DiscordCoreInternal::BaseSocketAgent>(this->botToken, this->getGateWayBot());
		this->currentUser = std::make_unique<BotUser>(Users::getCurrentUserAsync().get(), this->baseSocketAgent.get());
		for (auto& value : this->functionsToExecute) {
			auto thePtr = this;
			if (value.repeated) {
				TimeElapsedHandler onSend = [&](void)->void {
					value.function(this);
				};
				this->threadPoolTimers.push_back(ThreadPoolTimer::createPeriodicTimer(onSend, value.intervalInMs));
			}
			else {
				ThreadPoolTimer::executeFunctionAfterTimePeriod(value.function, static_cast<int32_t>(value.intervalInMs), thePtr);
			}
		}
	}

	DiscordCoreClient::DiscordCoreClient(nullptr_t other) {}

	void DiscordCoreClient::registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		this->commandController.registerFunction(functionNames, std::move(baseFunction));
	}

	BotUser DiscordCoreClient::getBotUser() {
		return *this->currentUser;
	}

	void DiscordCoreClient::runBot() {
		this->run();
	}

	std::string DiscordCoreClient::getGateWayBot() {
		try {
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Gateway_Bot;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/gateway/bot";
			workload.callStack = "DiscordCoreClient::getGateWayBot";
			auto result = DiscordCoreInternal::submitWorkloadAndGetResult<GatewayBotData>(*this->httpClient.get(), workload);
			std::string newString = result.url.substr(result.url.find("wss://") + std::string("wss://").size());
			return newString;
		}
		catch (...) {
			reportException("DiscordCoreClient::getGateWayBot()");
			return std::string{};
		}
	}

	void DiscordCoreClient::run() {
		while (!doWeQuit) {
			try {
				if (doWeQuit) {
					std::cout << "THIS IS NOT IT!" << std::endl;
					return;
				}
				std::unique_ptr<DiscordCoreInternal::WebSocketWorkload> workload{ std::make_unique<DiscordCoreInternal::WebSocketWorkload>() };
				while (!this->baseSocketAgent->getWorkloadTarget().tryReceive(*workload) && !doWeQuit) {
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
				};
				if (workload->eventType != DiscordCoreInternal::WebSocketEventType::Unset) {
					switch (workload->eventType) {
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Create:
					{
						std::unique_ptr<OnApplicationCommandCreationData> dataPackage{ std::make_unique<OnApplicationCommandCreationData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->applicationCommand);
						this->eventManager->onApplicationCommandCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Update:
					{
						std::unique_ptr<OnApplicationCommandUpdateData> dataPackage{ std::make_unique<OnApplicationCommandUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->applicationCommand);
						this->eventManager->onApplicationCommandUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Application_Command_Delete:
					{
						std::unique_ptr<OnApplicationCommandDeletionData> dataPackage{ std::make_unique<OnApplicationCommandDeletionData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->applicationCommand);
						this->eventManager->onApplicationCommandDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Create:
					{
						std::unique_ptr<OnChannelCreationData> dataPackage{ std::make_unique<OnChannelCreationData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->channel);
						this->eventManager->onChannelCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Update:
					{
						std::unique_ptr<OnChannelUpdateData> dataPackage{ std::make_unique<OnChannelUpdateData>() };
						if (workload->payLoad.contains("id")) {
							dataPackage->channelOld = Channels::getCachedChannelAsync({ .channelId = workload->payLoad.at("id") }).get();
							dataPackage->channelNew = dataPackage->channelOld;
						}
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->channelNew);
						this->eventManager->onChannelUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Delete:
					{
						std::unique_ptr<OnChannelDeletionData> dataPackage{ std::make_unique<OnChannelDeletionData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->channel);
						this->eventManager->onChannelDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Channel_Pins_Update:
					{
						std::unique_ptr<OnChannelPinsUpdateData> dataPackage{ std::make_unique<OnChannelPinsUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->dataPackage);
						this->eventManager->onChannelPinsUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Create:
					{
						std::unique_ptr<OnThreadCreationData> dataPackage{ std::make_unique<OnThreadCreationData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->thread);
						this->eventManager->onThreadCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Update:
					{
						std::unique_ptr<OnThreadUpdateData> dataPackage{ std::make_unique<OnThreadUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->thread);
						this->eventManager->onThreadUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Delete:
					{
						std::unique_ptr<OnThreadDeletionData> dataPackage{ std::make_unique<OnThreadDeletionData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->thread);
						this->eventManager->onThreadDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_List_Sync:
					{
						std::unique_ptr<OnThreadListSyncData> dataPackage{ std::make_unique<OnThreadListSyncData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->threadListSyncData);
						this->eventManager->onThreadListSyncEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Member_Update:
					{
						std::unique_ptr<OnThreadMemberUpdateData> dataPackage{ std::make_unique<OnThreadMemberUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->threadMember);
						this->eventManager->onThreadMemberUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Thread_Members_Update:
					{
						std::unique_ptr<OnThreadMembersUpdateData> dataPackage{ std::make_unique<OnThreadMembersUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->threadMembersUpdateData);
						this->eventManager->onThreadMembersUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Create:
					{
						GuildData guildDataNew{};
						std::unique_ptr<OnGuildCreationData> dataPackage{ std::make_unique<OnGuildCreationData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, guildDataNew);
						Guild guildNew{ guildDataNew, this };
						dataPackage->guild = guildNew;
						this->eventManager->onGuildCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Update:
					{
						std::unique_ptr<OnGuildUpdateData> dataPackage{ std::make_unique<OnGuildUpdateData>() };
						if (workload->payLoad.contains("id")) {
							dataPackage->guildOld = Guilds::getCachedGuildAsync({ .guildId = workload->payLoad.at("id") }).get();
							dataPackage->guildNew = dataPackage->guildOld;
						}
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->guildNew);
						this->eventManager->onGuildUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Delete:
					{
						std::unique_ptr<OnGuildDeletionData> dataPackage{ std::make_unique<OnGuildDeletionData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->guild);
						this->eventManager->onGuildDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Ban_Add:
					{
						std::unique_ptr<OnGuildBanAddData> dataPackage{ std::make_unique<OnGuildBanAddData>() };
						if (workload->payLoad.contains("user")) {
							UserData newData{};
							DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("user"), newData);
							dataPackage->user = newData;
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onGuildBanAddEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Ban_Remove:
					{
						std::unique_ptr<OnGuildBanRemoveData> dataPackage{ std::make_unique<OnGuildBanRemoveData>() };
						if (workload->payLoad.contains("user")) {
							UserData newData{};
							DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("user"), newData);
							dataPackage->user = newData;
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onGuildBanRemoveEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Emojis_Update:
					{
						std::unique_ptr<OnGuildEmojisUpdateData> dataPackage{ std::make_unique<OnGuildEmojisUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->updateData);
						this->eventManager->onGuildEmojisUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Stickers_Update:
					{
						std::unique_ptr<OnGuildStickersUpdateData> dataPackage{ std::make_unique<OnGuildStickersUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->updateData);
						this->eventManager->onGuildStickersUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Integrations_Update:
					{
						std::unique_ptr<OnGuildIntegrationsUpdateData> dataPackage{ std::make_unique<OnGuildIntegrationsUpdateData>() };
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onGuildIntegrationsUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Add:
					{
						std::unique_ptr<OnGuildMemberAddData> dataPackage{ std::make_unique<OnGuildMemberAddData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->guildMember);
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildMember.guildId = workload->payLoad.at("guild_id");
						}
						dataPackage->discordCoreClient = this;
						this->eventManager->onGuildMemberAddEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Remove:
					{
						std::unique_ptr<OnGuildMemberRemoveData> dataPackage{ std::make_unique<OnGuildMemberRemoveData>() };
						if (workload->payLoad.contains("user")) {
							UserData newData{};
							DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("user"), newData);
							dataPackage->user = newData;
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onGuildMemberRemoveEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Member_Update:
					{
						std::unique_ptr<OnGuildMemberUpdateData> dataPackage{ std::make_unique<OnGuildMemberUpdateData>() };
						if (workload->payLoad.contains("user") && workload->payLoad.at("user").contains("id")) {
							dataPackage->guildMemberOld = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = workload->payLoad.at("user").at("id"),.guildId = workload->payLoad.at("guild_id") }).get();
							dataPackage->guildMemberNew = dataPackage->guildMemberOld;
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildMemberNew.guildId = workload->payLoad.at("guild_id");
						}
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->guildMemberNew);
						this->eventManager->onGuildMemberUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Members_Chunk:
					{
						std::unique_ptr<OnGuildMembersChunkData> dataPackage{ std::make_unique<OnGuildMembersChunkData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->chunkEventData);
						this->eventManager->onGuildMembersChunkEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Create:
					{
						std::unique_ptr<OnRoleCreationData> dataPackage{ std::make_unique<OnRoleCreationData>() };
						if (workload->payLoad.contains("role")) {
							DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("role"), dataPackage->role);
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onRoleCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Update:
					{
						std::unique_ptr<OnRoleUpdateData> dataPackage{ std::make_unique<OnRoleUpdateData>() };
						if (workload->payLoad.contains("role")) {
							dataPackage->roleOld = Roles::getCachedRoleAsync({ .roleId = workload->payLoad.at("role").at("id") }).get();
							dataPackage->roleNew = dataPackage->roleOld;
						}
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("role"), dataPackage->roleNew);
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onRoleUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Guild_Role_Delete:
					{
						std::unique_ptr<OnRoleDeletionData> dataPackage{ std::make_unique<OnRoleDeletionData>() };
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("role_id")) {
							dataPackage->roleOld = Roles::getCachedRoleAsync({ .roleId = workload->payLoad.at("role_id") }).get();
						}
						this->eventManager->onRoleDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Integration_Create:
					{
						std::unique_ptr<OnIntegrationCreationData> dataPackage{ std::make_unique<OnIntegrationCreationData>() };
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("integration")) {
							DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("integration"), dataPackage->integrationData);
						}
						this->eventManager->onIntegrationCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Integration_Update:
					{
						std::unique_ptr<OnIntegrationUpdateData> dataPackage{ std::make_unique<OnIntegrationUpdateData>() };
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("integration")) {
							DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("integration"), dataPackage->integrationData);
						}
						this->eventManager->onIntegrationUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Integration_Delete:
					{
						std::unique_ptr<OnIntegrationDeletionData> dataPackage{ std::make_unique<OnIntegrationDeletionData>() };
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("application_id")) {
							dataPackage->applicationId = workload->payLoad.at("application_id");
						}
						if (workload->payLoad.contains("id")) {
							dataPackage->id = workload->payLoad.at("id");
						}
						this->eventManager->onIntegrationDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Interaction_Create:
					{
						std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
						if (workload->payLoad.contains("user")) {
							*interactionData = InteractionData(workload->payLoad.at("user").at("id").get<std::string>());
						}
						else if (workload->payLoad.contains("member")) {
							*interactionData = InteractionData(workload->payLoad.at("member").at("user").at("id").get<std::string>());
						}
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, *interactionData.get());
						std::unique_ptr<InputEventData> eventData{ std::make_unique<InputEventData>(MessageData(), *interactionData, InputEventType::Unset) };
						if (interactionData->type == InteractionType::Application_Command) {
							if (interactionData->data.applicationCommanddata.type == ApplicationCommandType::Chat_Input) {
								eventData->eventType = InputEventType::Application_Command_Interaction;
								eventData->responseType = InputEventResponseType::Unset;
								eventData->requesterId = interactionData->requesterId;
								eventData->interactionData = *interactionData;
								std::unique_ptr<OnInteractionCreationData> dataPackage{ std::make_unique<OnInteractionCreationData>() };
								dataPackage->interactionData = *interactionData;
								std::unique_ptr<CommandData> commandData{ std::make_unique<CommandData>(*eventData) };
								this->commandController.checkForAndRunCommand(std::move(commandData));
								this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
								std::unique_ptr<OnInputEventCreationData> eventCreationData{ std::make_unique<OnInputEventCreationData>() };
								eventCreationData->inputEventData = *eventData;
								this->eventManager->onInputEventCreationEvent(*eventCreationData);
							}
							else if (interactionData->data.applicationCommanddata.type == ApplicationCommandType::Message) {
								eventData->eventType = InputEventType::Message_Command_Interaction;
								eventData->responseType = InputEventResponseType::Unset;
								eventData->requesterId = interactionData->requesterId;
								eventData->interactionData = *interactionData;
								std::unique_ptr<OnInteractionCreationData> dataPackage{ std::make_unique<OnInteractionCreationData>() };
								dataPackage->interactionData = *interactionData;
								std::unique_ptr<CommandData> commandData{ std::make_unique<CommandData>(*eventData) };
								this->commandController.checkForAndRunCommand(std::move(commandData));
								this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
								std::unique_ptr<OnInputEventCreationData> eventCreationData{ std::make_unique<OnInputEventCreationData>() };
								eventCreationData->inputEventData = *eventData;
								this->eventManager->onInputEventCreationEvent(*eventCreationData);
							}
							else if (interactionData->data.applicationCommanddata.type == ApplicationCommandType::User) {
								eventData->eventType = InputEventType::User_Command_Interaction;
								eventData->responseType = InputEventResponseType::Unset;
								eventData->requesterId = interactionData->requesterId;
								eventData->interactionData = *interactionData;
								std::unique_ptr<OnInteractionCreationData> dataPackage{ std::make_unique<OnInteractionCreationData>() };
								dataPackage->interactionData = *interactionData;
								std::unique_ptr<CommandData> commandData{ std::make_unique<CommandData>(*eventData) };
								this->commandController.checkForAndRunCommand(std::move(commandData));
								this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
								std::unique_ptr<OnInputEventCreationData> eventCreationData{ std::make_unique<OnInputEventCreationData>() };
								eventCreationData->inputEventData = *eventData;
								this->eventManager->onInputEventCreationEvent(*eventCreationData);
							}
						}
						else if (interactionData->type == InteractionType::Message_Component) {
							if (interactionData->data.componentData.componentType == ComponentType::Button) {
								eventData->eventType = InputEventType::Button_Interaction;
								eventData->responseType = InputEventResponseType::Unset;
								eventData->requesterId = interactionData->requesterId;
								eventData->interactionData = *interactionData;
								std::unique_ptr<OnInteractionCreationData> dataPackage{ std::make_unique<OnInteractionCreationData>() };
								dataPackage->interactionData = *interactionData;
								std::unique_ptr<OnInputEventCreationData> eventCreationData{ std::make_unique<OnInputEventCreationData>() };
								eventCreationData->inputEventData = *eventData;
								if (ButtonCollector::buttonInteractionBufferMap.contains(eventData->getChannelId() + eventData->getMessageId())) {
									ButtonCollector::buttonInteractionBufferMap.at(eventData->getChannelId() + eventData->getMessageId())->send(eventData->getInteractionData());
								}
								this->eventManager->onInputEventCreationEvent(*eventCreationData);
								this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
							}
							else if (interactionData->data.componentData.componentType == ComponentType::SelectMenu) {
								eventData->eventType = InputEventType::Select_Menu_Interaction;
								eventData->responseType = InputEventResponseType::Unset;
								eventData->requesterId = interactionData->requesterId;
								eventData->interactionData = *interactionData;
								std::unique_ptr<OnInteractionCreationData> dataPackage{ std::make_unique<OnInteractionCreationData>() };
								dataPackage->interactionData = *interactionData;
								std::unique_ptr<OnInputEventCreationData> eventCreationData{ std::make_unique<OnInputEventCreationData>() };
								eventCreationData->inputEventData = *eventData;
								if (SelectMenuCollector::selectMenuInteractionBufferMap.contains(eventData->getChannelId() + eventData->getMessageId())) {
									SelectMenuCollector::selectMenuInteractionBufferMap.at(eventData->getChannelId() + eventData->getMessageId())->send(eventData->getInteractionData());
								}
								this->eventManager->onInputEventCreationEvent(*eventCreationData);
								this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
							}
						}
						else if (interactionData->type == InteractionType::Modal_Submit) {
							eventData->eventType = InputEventType::Modal_Interaction;
							eventData->responseType = InputEventResponseType::Unset;
							eventData->requesterId = interactionData->requesterId;
							eventData->interactionData = *interactionData;
							std::unique_ptr<OnInteractionCreationData> dataPackage{ std::make_unique<OnInteractionCreationData>() };
							dataPackage->interactionData = *interactionData;
							std::unique_ptr<OnInputEventCreationData> eventCreationData{ std::make_unique<OnInputEventCreationData>() };
							eventCreationData->inputEventData = *eventData;
							if (ModalCollector::modalInteractionBufferMap.contains(eventData->getChannelId())) {
								ModalCollector::modalInteractionBufferMap.at(eventData->getChannelId())->send(eventData->getInteractionData());
							}
							this->eventManager->onInputEventCreationEvent(*eventCreationData);
							this->eventManager->onInteractionCreationEvent(std::move(*dataPackage));
						}
						if (Interactions::collectMessageDataBuffers.contains(interactionData->id)) {
							std::unique_ptr<MessageData> messageData{ std::make_unique<MessageData>(interactionData->message) };
							Interactions::collectMessageDataBuffers.at(eventData->getInteractionId())->send(*messageData);
						}
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Invite_Create:
					{
						std::unique_ptr<OnInviteCreationData> dataPackage{ std::make_unique<OnInviteCreationData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->invite);
						this->eventManager->onInviteCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Invite_Delete:
					{
						std::unique_ptr<OnInviteDeletionData> dataPackage{ std::make_unique<OnInviteDeletionData>() };
						if (workload->payLoad.contains("channel_id")) {
							dataPackage->channelId = workload->payLoad.at("channel_id");
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("code")) {
							dataPackage->code = workload->payLoad.at("code");
						}
						this->eventManager->onInviteDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Create:
					{
						std::unique_ptr<Message> message{ std::make_unique<Message>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, *message.get());
						if (message->interaction.id != "") {
							if (Interactions::collectMessageDataBuffers.contains(message->interaction.id)) {
								Interactions::collectMessageDataBuffers.at(message->interaction.id)->send(*message);
							}
						}
						std::unique_ptr<InputEventData> eventData{ std::make_unique<InputEventData>(*message, InteractionData(), InputEventType::Regular_Message) };
						std::unique_ptr<CommandData> commandData{ std::make_unique<CommandData>(*eventData) };
						this->commandController.checkForAndRunCommand(std::move(commandData));
						if (MessageCollector::messagesBufferMap.size() > 0) {
							for (auto& [key, value] : MessageCollector::messagesBufferMap) {
								value->send(*message);
							}
						}
						std::unique_ptr<OnMessageCreationData> dataPackage{ std::make_unique<OnMessageCreationData>() };
						dataPackage->message = *message;
						this->eventManager->onMessageCreationEvent(std::move(*dataPackage));
						std::unique_ptr<OnInputEventCreationData> eventCreationData{ std::make_unique<OnInputEventCreationData>() };
						eventCreationData->inputEventData = *eventData;
						this->eventManager->onInputEventCreationEvent(*eventCreationData);
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Update:
					{
						std::unique_ptr<OnMessageUpdateData> dataPackage{ std::make_unique<OnMessageUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->messageNew);
						if (dataPackage->messageNew.interaction.id != "") {
							if (Interactions::collectMessageDataBuffers.contains(dataPackage->messageNew.interaction.id)) {
								Interactions::collectMessageDataBuffers.at(dataPackage->messageNew.interaction.id)->send(dataPackage->messageNew);
							}
						}
						this->eventManager->onMessageUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Delete:
					{
						std::unique_ptr<OnMessageDeletionData> dataPackage{ std::make_unique<OnMessageDeletionData>() };
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("channel_id")) {
							dataPackage->channelId = workload->payLoad.at("channel_id");
						}
						if (workload->payLoad.contains("id")) {
							dataPackage->messageId = workload->payLoad.at("id");
						}
						this->eventManager->onMessageDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Delete_Bulk:
					{
						std::unique_ptr<OnMessageDeleteBulkData> dataPackage{ std::make_unique<OnMessageDeleteBulkData>() };
						if (workload->payLoad.contains("channel_id")) {
							dataPackage->channelId = workload->payLoad.at("channel_id");
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("ids")) {
							for (auto& value : workload->payLoad.at("ids")) {
								dataPackage->ids.push_back(value);
							}
						}
						this->eventManager->onMessageDeleteBulkEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Add:
					{
						std::unique_ptr<OnReactionAddData> dataPackage{ std::make_unique<OnReactionAddData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->reaction);
						this->eventManager->onReactionAddEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Remove:
					{
						std::unique_ptr<OnReactionRemoveData> dataPackage{ std::make_unique<OnReactionRemoveData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->reactionRemoveData);
						this->eventManager->onReactionRemoveEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Remove_All:
					{
						std::unique_ptr<OnReactionRemoveAllData> dataPackage{ std::make_unique<OnReactionRemoveAllData>() };
						if (workload->payLoad.contains("channel_id")) {
							dataPackage->channelId = workload->payLoad.at("channel_id");
						}
						if (workload->payLoad.contains("message_id")) {
							dataPackage->messageId = workload->payLoad.at("message_id");
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onReactionRemoveAllEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Message_Reaction_Remove_Emoji:
					{
						std::unique_ptr<OnReactionRemoveEmojiData> dataPackage{ std::make_unique<OnReactionRemoveEmojiData>() };
						if (workload->payLoad.contains("channel_id")) {
							dataPackage->channelId = workload->payLoad.at("channel_id");
						}
						if (workload->payLoad.contains("message_id")) {
							dataPackage->messageId = workload->payLoad.at("message_id");
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("emoji"), dataPackage->emoji);
						this->eventManager->onReactionRemoveEmojiEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Presence_Update:
					{
						std::unique_ptr<OnPresenceUpdateData> dataPackage{ std::make_unique<OnPresenceUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->presenceData);
						this->eventManager->onPresenceUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Stage_Instance_Create:
					{
						std::unique_ptr<OnStageInstanceCreationData> dataPackage{ std::make_unique<OnStageInstanceCreationData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->stageInstance);
						this->eventManager->onStageInstanceCreationEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Stage_Instance_Update:
					{
						std::unique_ptr<OnStageInstanceUpdateData> dataPackage{ std::make_unique<OnStageInstanceUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->stageInstance);
						this->eventManager->onStageInstanceUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Stage_Instance_Delete:
					{
						std::unique_ptr<OnStageInstanceDeletionData> dataPackage{ std::make_unique<OnStageInstanceDeletionData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->stageInstance);
						this->eventManager->onStageInstanceDeletionEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Typing_Start:
					{
						std::unique_ptr<OnTypingStartData> dataPackage{ std::make_unique<OnTypingStartData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->typingStartData);
						this->eventManager->onTypingStartEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::User_Update:
					{
						std::unique_ptr<OnUserUpdateData> dataPackage{ std::make_unique<OnUserUpdateData>() };
						UserData newData{};
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad.at("user"), newData);
						dataPackage->userNew = newData;
						dataPackage->userOld = Users::getCachedUserAsync({ .userId = dataPackage->userNew.id }).get();
						this->eventManager->onUserUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Voice_State_Update:
					{
						std::unique_ptr<OnVoiceStateUpdateData> dataPackage{ std::make_unique<OnVoiceStateUpdateData>() };
						DiscordCoreInternal::DataParser::parseObject(workload->payLoad, dataPackage->voiceStateData);
						this->eventManager->onVoiceStateUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Voice_Server_Update:
					{
						std::unique_ptr<OnVoiceServerUpdateData> dataPackage{ std::make_unique<OnVoiceServerUpdateData>() };
						if (workload->payLoad.contains("endpoint")) {
							dataPackage->endpoint = workload->payLoad.at("endpoint");
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						if (workload->payLoad.contains("token")) {
							dataPackage->token = workload->payLoad.at("token");
						}
						this->eventManager->onVoiceServerUpdateEvent(std::move(*dataPackage));
						break;
					}
					case DiscordCoreInternal::WebSocketEventType::Webhooks_Update:
					{
						std::unique_ptr<OnWebhookUpdateData> dataPackage{ std::make_unique<OnWebhookUpdateData>() };
						if (workload->payLoad.contains("channel_id")) {
							dataPackage->channelId = workload->payLoad.at("channel_id");
						}
						if (workload->payLoad.contains("guild_id")) {
							dataPackage->guildId = workload->payLoad.at("guild_id");
						}
						this->eventManager->onWebhookUpdateEvent(std::move(*dataPackage));
						break;
					}
					default:
					{
						break;
					}
					}
					if (doWeQuit) {
						std::cout << "WERE HERE THIS IS IT!0202" << std::endl;
						return;
					}
				}
				
			}
			catch (...) {
				reportException("DiscordCoreClient::run()");
			}
		}
		std::cout << "THIS IS NOT IT!" << std::endl;
		std::cout << "WERE HERE THIS IS IT!0303" << std::endl;
	}

	DiscordCoreClient::~DiscordCoreClient() {
		std::cout << "WERE HERE THIS IS IT!050505" << std::endl;
		if (this != nullptr) {
			std::cout << "WERE HERE THIS IS IT!040404" << std::endl;
			curl_global_cleanup();
		}
	}

}

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT: {
		std::cout << "WERE HERE THIS IS IT!060606" << std::endl;
		DiscordCoreAPI::doWeQuit = true;
		return TRUE;
	}
	case CTRL_CLOSE_EVENT: {
		std::cout << "WERE HERE THIS IS IT!070707" << std::endl;
		DiscordCoreAPI::doWeQuit = true;
		return TRUE;
	}
	case CTRL_BREAK_EVENT: {
		DiscordCoreAPI::doWeQuit = true;
		return FALSE;
	}
	case CTRL_LOGOFF_EVENT: {
		DiscordCoreAPI::doWeQuit = true;
		return FALSE;
	}
	case CTRL_SHUTDOWN_EVENT: {
		DiscordCoreAPI::doWeQuit = true;
		return FALSE;
	}
	default: {
		return FALSE;
	}		
	}
}
#endif