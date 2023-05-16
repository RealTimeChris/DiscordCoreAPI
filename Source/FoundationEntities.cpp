/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// FoundationEntities.cpp - Source file for the foundation entities.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file FoundationEntities.cpp

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/InteractionEntities.hpp>
#include <discordcoreapi/GuildScheduledEventEntities.hpp>
#include <discordcoreapi/RoleEntities.hpp>
#include <discordcoreapi/UserEntities.hpp>
#include <discordcoreapi/ChannelEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/InputEvents.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreInternal {

	WebSocketResumeData::operator DiscordCoreAPI::EtfSerializer() {
		DiscordCoreAPI::EtfSerializer data{};
		data["op"] = 6;
		data["d"]["seq"] = this->lastNumberReceived;
		data["d"]["session_id"] = this->sessionId;
		data["d"]["token"] = this->botToken;
		return data;
	}

	WebSocketIdentifyData::operator DiscordCoreAPI::EtfSerializer() {
		DiscordCoreAPI::EtfSerializer serializer{};
		serializer["d"]["intents"] = this->intents;
		serializer["d"]["large_threshold"] = 250;
		for (auto& value: this->presence.activities) {
			DiscordCoreAPI::EtfSerializer serializer01{};
			if (value.url != "") {
				serializer01["url"] = value.url;
			}
			serializer01["name"] = value.name;
			serializer01["type"] = value.type;
			serializer["d"]["presence"]["activities"].emplaceBack(serializer01);
		}
		serializer["d"]["presence"]["afk"] = this->presence.afk;
		if (this->presence.since != 0) {
			serializer["d"]["presence"]["since"] = this->presence.since;
		}
		serializer["d"]["presence"]["status"] = this->presence.status;
		serializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		serializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		serializer["d"]["properties"]["os"] = "Windows";
#elif __linux__
		serializer["d"]["properties"]["os"] = "Linux";
#endif
		serializer["d"]["shard"].emplaceBack(this->shard[0]);
		serializer["d"]["shard"].emplaceBack(this->shard[1]);
		serializer["d"]["token"] = this->botToken;
		serializer["op"] = 2;
		return serializer;
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadData&& other) noexcept {
		if (this != &other) {
			this->headersToInsert = std::move(other.headersToInsert);
			this->thisWorkerId.store(this->thisWorkerId.load());
			this->relativePath = std::move(other.relativePath);
			this->callStack = std::move(other.callStack);
			this->workloadClass = other.workloadClass;
			this->baseUrl = std::move(other.baseUrl);
			this->content = std::move(other.content);
			this->workloadType = other.workloadType;
			this->payloadType = other.payloadType;
		}
		return *this;
	}

	HttpsWorkloadData& HttpsWorkloadData::operator=(HttpsWorkloadType type) noexcept {
		if (!HttpsWorkloadData::workloadIdsExternal.contains(type)) {
			HttpsWorkloadData::workloadIdsExternal[type] = std::make_unique<std::atomic_int64_t>();
			HttpsWorkloadData::workloadIdsInternal[type] = std::make_unique<std::atomic_int64_t>();
		}
		this->thisWorkerId.store(HttpsWorkloadData::incrementAndGetWorkloadId(type));
		this->workloadType = type;
		return *this;
	}

	HttpsWorkloadData::HttpsWorkloadData(HttpsWorkloadType type) noexcept {
		*this = type;
	}

	int64_t HttpsWorkloadData::incrementAndGetWorkloadId(HttpsWorkloadType workloadType) noexcept {
		int64_t value{ HttpsWorkloadData::workloadIdsExternal[workloadType]->load() };
		HttpsWorkloadData::workloadIdsExternal[workloadType]->store(value + 1);
		return value;
	}

	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsExternal{};
	std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> HttpsWorkloadData::workloadIdsInternal{};

}

namespace DiscordCoreAPI {

	std::string DiscordEntity::getCreatedAtTimestamp(TimeFormat timeFormat) {
		TimeStamp timeStamp{ (this->id.operator uint64_t() >> 22) + 1420070400000, timeFormat };
		return timeStamp.operator std::string();
	}

	GuildMemberData& GuildMemberData::operator=(GuildMemberData&& other) noexcept {
		if (this != &other) {
			this->permissions = std::move(other.permissions);
			this->joinedAt = std::move(other.joinedAt);
			this->avatar = std::move(other.avatar);
			this->roles = std::move(other.roles);
			this->flags = std::move(other.flags);
			this->nick = std::move(other.nick);
			this->guildId = other.guildId;
			this->user.id = other.user.id;
		}
		return *this;
	}

	UpdatePresenceData::operator EtfSerializer() {
		EtfSerializer data{};
		data["op"] = 3;
		for (auto& value: this->activities) {
			EtfSerializer dataNew{};
			if (value.url != "") {
				dataNew["url"] = std::string{ value.url };
			}
			dataNew["name"] = std::string{ value.name };
			dataNew["type"] = value.type;
			data["d"]["activities"].emplaceBack(dataNew);
		}
		data["status"] = this->status;
		if (this->since != 0) {
			data["since"] = this->since;
		}
		data["afk"] = this->afk;
		return data;
	}

	UpdateVoiceStateData::operator EtfSerializer() {
		EtfSerializer data{};
		data["op"] = 4;
		if (this->channelId == 0) {
			data["d"]["channel_id"] = JsonType::Null;
		} else {
			data["d"]["channel_id"] = this->channelId.operator std::string();
		}
		data["d"]["self_deaf"] = this->selfDeaf;
		data["d"]["self_mute"] = this->selfMute;
		data["d"]["guild_id"] = this->guildId.operator std::string();
		return data;
	}

	UpdateVoiceStateDataDC::operator EtfSerializer() {
		EtfSerializer data{};
		data["op"] = 4;
		data["d"]["channel_id"] = this->channelId;
		data["d"]["self_deaf"] = this->selfDeaf;
		data["d"]["self_mute"] = this->selfMute;
		data["d"]["guild_id"] = this->guildId.operator std::string();
		return data;
	}

	std::string GuildMemberData::getAvatarUrl() noexcept {
		if (this->avatar.operator std::string() != "") {
			std::string stringNew{ "https://cdn.discordapp.com/" };
			stringNew += "guilds/" + this->guildId + "/users/" + this->user.id + "/avatars/" + this->avatar.operator std::string();
			return stringNew;
		} else {
			return this->getUserData().getAvatarUrl();
		}
	}

	UserData GuildMemberData::getUserData() {
		if (this->user.id.operator uint64_t() != 0) {
			return Users::getCachedUser(GetUserData{ .userId = this->user.id });
		} else {
			return {};
		}
	}

	VoiceConnection* GuildData::connectToVoice(const Snowflake guildMemberId, const Snowflake channelId, bool selfDeaf, bool selfMute,
		StreamInfo streamInfoNew) {
		if (DiscordCoreClient::getVoiceConnection(this->id) && DiscordCoreClient::getVoiceConnection(this->id)->areWeConnected()) {
			this->voiceConnectionPtr = DiscordCoreClient::getVoiceConnection(this->id);
			return this->voiceConnectionPtr;
		} else if (static_cast<Snowflake>(guildMemberId) != 0 || static_cast<Snowflake>(channelId) != 0) {
			Snowflake channelId{};
			if (static_cast<Snowflake>(guildMemberId) != 0) {
				VoiceStateDataLight dataLight{};
				dataLight.guildId = this->id;
				dataLight.id = guildMemberId;
				GuildMemberData getData{};
				getData.guildId = this->id;
				getData.user.id = guildMemberId;
				auto voiceStateData = GuildMembers::getVoiceStateData(getData);
				if (voiceStateData.channelId != 0) {
					channelId = voiceStateData.channelId;
				}
			} else {
				channelId = channelId;
			}
			uint64_t theShardId{ (this->id.operator uint64_t() >> 22) % this->discordCoreClient->configManager.getTotalShardCount() };
			auto theBaseSocketAgentIndex{ static_cast<int32_t>(
				floor(static_cast<double>(theShardId) / static_cast<double>(this->discordCoreClient->configManager.getTotalShardCount()) *
					this->discordCoreClient->baseSocketAgentsMap.size())) };
			VoiceConnectInitData voiceConnectInitData{};
			voiceConnectInitData.currentShard = theShardId;
			voiceConnectInitData.streamInfo = streamInfoNew;
			voiceConnectInitData.channelId = channelId;
			voiceConnectInitData.guildId = this->id;
			voiceConnectInitData.userId = this->discordCoreClient->getBotUser().id;
			voiceConnectInitData.selfDeaf = selfDeaf;
			voiceConnectInitData.selfMute = selfMute;
			StopWatch stopWatch{ 10000ms };
			auto voiceConnection = DiscordCoreClient::getVoiceConnection(this->id);
			voiceConnection->connect(voiceConnectInitData);
			while (voiceConnection->areWeConnected()) {
				std::this_thread::sleep_for(1ms);
			}
			this->voiceConnectionPtr = voiceConnection;
			return this->voiceConnectionPtr;
		} else {
			return nullptr;
		}
	}

	std::string GuildData::getIconUrl() noexcept {
		std::string stringNew{ "https://cdn.discordapp.com/" };
		stringNew += "icons/" + this->id + "/" + this->icon.operator std::string() + ".png";
		return stringNew;
	}

	bool GuildData::areWeConnected() {
		return discordCoreClient->getVoiceConnection(id)->areWeConnected();
	}

	void GuildData::disconnect() {
		if (DiscordCoreClient::getVoiceConnection(this->id)) {
			DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::UpdateVoiceStateData> data{};
			data.excludedKeys.emplace("t");
			data.excludedKeys.emplace("s");
			data.d.channelId = 0;
			data.d.selfDeaf = false;
			data.d.selfMute = false;
			data.d.guildId = this->id;
			this->discordCoreClient->getBotUser().updateVoiceStatus(data.d);
			DiscordCoreClient::getVoiceConnection(this->id)->disconnect();
			this->voiceConnectionPtr = nullptr;
		}
	}

	bool operator==(const ApplicationCommandOptionData& lhs, const ApplicationCommandOptionData& rhs) {
		if (lhs.autocomplete != rhs.autocomplete) {
			return false;
		}
		if (lhs.channelTypes != rhs.channelTypes) {
			return false;
		}
		if (lhs.description != rhs.description) {
			return false;
		}
		if (lhs.descriptionLocalizations != rhs.descriptionLocalizations) {
			return false;
		}
		if (lhs.maxValue != rhs.maxValue) {
			return false;
		}
		if (lhs.minValue != rhs.minValue) {
			return false;
		}
		if (lhs.name != rhs.name) {
			return false;
		}
		if (lhs.nameLocalizations != rhs.nameLocalizations) {
			return false;
		}
		if (lhs.options.size() != rhs.options.size()) {
			return false;
		}
		if (lhs.required != rhs.required) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		if (lhs.choices.size() != rhs.choices.size()) {
			return false;
		}
		for (int32_t x = 0; x < rhs.choices.size(); ++x) {
			if (lhs.choices[x] != rhs.choices[x]) {
			}
		}
		return true;
	}

	EmbedData& EmbedData::setAuthor(const std::string& authorName, const std::string& authorAvatarUrl) {
		this->author.name = authorName;
		this->author.iconUrl = authorAvatarUrl;
		return *this;
	}

	EmbedData& EmbedData::setFooter(const std::string& footerText, const std::string& footerIconUrlText) {
		this->footer.text = footerText;
		this->footer.iconUrl = footerIconUrlText;
		return *this;
	}

	EmbedData& EmbedData::setTimeStamp(const std::string& timeStamp) {
		this->timeStamp = timeStamp;
		return *this;
	}

	EmbedData& EmbedData::addField(const std::string& name, const std::string& value, bool Inline) {
		EmbedFieldData field{};
		field.Inline = Inline;
		field.value = value;
		field.name = name;
		this->fields.emplace_back(field);
		return *this;
	}

	void EmbedData::generateExcludedKeys() noexcept {
		if (this->fields.size() == 0) {
			this->excludedKeys.emplace("fields");
		}
		if (this->url == "") {
			this->excludedKeys.emplace("url");
		}
		if (this->hexColorValue == 0) {
			this->excludedKeys.emplace("color");
		}
		if (this->title == "") {
			this->excludedKeys.emplace("title");
		}
		if (this->type == "") {
			this->excludedKeys.emplace("type");
		}
		if (this->author.iconUrl == "" && this->author.name == "" && this->author.proxyIconUrl == "" && this->author.url == "") {
			this->excludedKeys.emplace("author");
		}
		if (this->video.proxyUrl == "" && this->video.height == 0 && this->video.width == 0 && this->video.url == "") {
			this->excludedKeys.emplace("video");
		}
		if (this->footer.iconUrl == "" && this->footer.proxyIconUrl == "" && this->footer.text == "") {
			this->excludedKeys.emplace("footer");
		}
		if (this->image.height == 0 && this->image.width == 0 && this->image.proxyUrl == "" && this->image.url == "") {
			this->excludedKeys.emplace("image");
		}
		if (this->provider.name == "" && this->provider.url == "") {
			this->excludedKeys.emplace("provider");
		}
		if (this->thumbnail.height == 0 && this->thumbnail.width == 0 && this->thumbnail.proxyUrl == "" && this->thumbnail.url == "") {
			this->excludedKeys.emplace("thumbnail");
		}
		if (this->timeStamp == "") {
			this->excludedKeys.emplace("timestamp");
		}
	}

	EmbedData& EmbedData::setDescription(const std::string& descriptionNew) {
		this->description = descriptionNew;
		return *this;
	}

	EmbedData& EmbedData::setColor(const std::string& hexColorValueNew) {
		this->hexColorValue = std::stoul(hexColorValueNew, nullptr, 16);
		return *this;
	}

	EmbedData& EmbedData::setThumbnail(const std::string& thumbnailUrl) {
		this->thumbnail.url = thumbnailUrl;
		return *this;
	}

	EmbedData& EmbedData::setTitle(const std::string& titleNew) {
		this->title = titleNew;
		return *this;
	}

	EmbedData& EmbedData::setImage(const std::string& imageUrl) {
		this->image.url = imageUrl;
		return *this;
	}

	GuildScheduledEventDataVector::operator std::vector<GuildScheduledEventData>() {
		return this->guildScheduledEvents;
	}

	WebHookDataVector::operator std::vector<WebHookData>() {
		return this->webHooks;
	}

	auto AuditLogData::getAuditLogData(const Snowflake userIdOfChanger, AuditLogEvent auditLogType) {
		for (auto& value: this->auditLogEntries) {
			if (value.id == userIdOfChanger && value.actionType == auditLogType) {
				return value;
			}
		}
		return AuditLogEntryData();
	}

	auto AuditLogData::getAuditLogData(AuditLogEvent auditLogType, const Snowflake userIdOfTarget) {
		for (auto& value: this->auditLogEntries) {
			if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
				return value;
			}
		}
		return AuditLogEntryData();
	}

	YouTubeFormatVector::operator std::vector<YouTubeFormat>() {
		return this->formats;
	}

	void PartialEmojiData::generateExcludedKeys() noexcept {
		if (this->name == "") {
			this->excludedKeys.emplace("name");
		}
		if (this->id == 0) {
			this->excludedKeys.emplace("id");
		}
	}

	void EmojiData::generateExcludedKeys() noexcept {
		if (this->id == 0) {
			this->excludedKeys.emplace("id");
		}
		if (this->managed == false) {
			this->excludedKeys.emplace("managed");
		}
		if (this->name == "") {
			this->excludedKeys.emplace("name");
		}
		if (this->roles.size() == 0) {
			this->excludedKeys.emplace("roles");
		}
		if (this->unicodeName.size() == 0) {
			this->excludedKeys.emplace("unicode_name");
		}
	}

	AudioFrameData& AudioFrameData::operator+=(std::basic_string_view<uint8_t> other) noexcept {
		if (this->data.size() < this->currentSize + other.size()) {
			this->data.resize(other.size() + this->currentSize);
		}
		std::copy(other.data(), other.data() + other.size(), this->data.data() + this->currentSize);
		this->currentSize += other.size();
		this->type = AudioFrameType::RawPCM;
		return *this;
	}

	AudioFrameData& AudioFrameData::operator+=(uint8_t character) {
		++this->currentSize;
		this->data.push_back(character);
		return *this;
	}

	bool operator==(const AudioFrameData& lhs, const AudioFrameData& rhs) {
		if (lhs.data != rhs.data) {
			return false;
		}
		if (lhs.currentSize != rhs.currentSize) {
			return false;
		}
		if (lhs.guildMemberId != rhs.guildMemberId) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		return true;
	}

	AudioFrameData& AudioFrameData::operator=(AudioFrameData&& other) noexcept {
		if (this->data.size() < other.data.size() + this->currentSize) {
			this->data.resize(other.data.size() + this->currentSize);
		}
		std::copy(other.data.data(), other.data.data() + other.data.size(), this->data.data() + this->currentSize);
		this->currentSize += other.data.size();
		this->guildMemberId = std::move(other.guildMemberId);
		this->type = other.type;
		other.currentSize = 0;
		other.type = AudioFrameType::Unset;
		return *this;
	}

	AudioFrameData::AudioFrameData(AudioFrameData&& other) noexcept {
		*this = std::move(other);
	}

	void AudioFrameData::clearData() noexcept {
		this->type = AudioFrameType::Unset;
		this->guildMemberId = 0;
		this->currentSize = 0;
		this->data.clear();
	}

	SerializerValue CommandData::getCommandArguments() {
		return this->optionsArgs;
	}

	bool operator==(const ApplicationCommandData& lhs, const ApplicationCommandData& rhs) {
		if (lhs.description != rhs.description) {
			return false;
		}
		if (lhs.name != rhs.name) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		if (lhs.options.size() != rhs.options.size()) {
			return false;
		}
		for (uint64_t x = 0; x < lhs.options.size(); ++x) {
			if (lhs.options[x] != rhs.options[x]) {
				return false;
			}
		}
		return true;
	}

	std::string UserData::getAvatarUrl() {
		std::string stringNew{ "https://cdn.discordapp.com/avatars/" + this->id + "/" + this->avatar.operator std::string() };
		return static_cast<std::string>(stringNew);
	}

	VoiceStateDataLight GuildMemberData::getVoiceStateData() noexcept {
		return GuildMembers::getVoiceStateData(*this);
	}

	bool operator==(const ApplicationCommandOptionChoiceData& lhs, const ApplicationCommandOptionChoiceData& rhs) {
		if (lhs.name != rhs.name) {
			return false;
		}
		if (lhs.nameLocalizations != rhs.nameLocalizations) {
			return false;
		}
		if (lhs.type != rhs.type) {
			return false;
		}
		if (lhs.value != rhs.value) {
			return false;
		}
		return true;
	}

	InputEventData& InputEventData::operator=(InputEventData&& other) noexcept {
		if (this != &other) {
			*this->interactionData = std::move(*other.interactionData);
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(InputEventData&& other) noexcept {
		*this = std::move(other);
	}

	InputEventData& InputEventData::operator=(const InputEventData& other) noexcept {
		if (this != &other) {
			*this->interactionData = *other.interactionData;
			this->responseType = other.responseType;
		}
		return *this;
	}

	InputEventData::InputEventData(const InputEventData& other) noexcept {
		*this = other;
	}

	InputEventData& InputEventData::operator=(const InteractionData& other) noexcept {
		*this->interactionData = other;
		return *this;
	}

	InputEventData::InputEventData(const InteractionData& interactionData) noexcept {
		*this = interactionData;
	}

	UserData InputEventData::getUserData() const {
		UserData returnData{};
		if (this->interactionData->member.user.id != 0) {
			returnData.id = this->interactionData->member.user.id;
		} else {
			returnData.id = this->interactionData->user.id;
		}
		returnData = Users::getCachedUser({ .userId = returnData.id });
		return returnData;
	}

	ChannelData InputEventData::getChannelData() const {
		ChannelData returnData{};
		if (this->interactionData->channelId != 0) {
			returnData.id = this->interactionData->channelId;
		} else {
			returnData.id = this->interactionData->channel.id;
		}
		returnData = Channels::getCachedChannel({ returnData.id });
		return returnData;
	}

	GuildMemberData InputEventData::getGuildMemberData() const {
		GuildMemberData returnData{ this->interactionData->member };
		returnData = GuildMembers::getCachedGuildMember({ .guildMemberId = returnData.user.id, .guildId = this->interactionData->guildId });
		return returnData;
	}

	void InteractionCallbackData::generateExcludedKeys() noexcept {
		if (this->allowedMentions.parse.size() == 0 && this->allowedMentions.roles.size() == 0 && this->allowedMentions.users.size() == 0) {
			this->excludedKeys.emplace("allowed_mentions");
		}
		if (this->components.size() == 0) {
			this->excludedKeys.emplace("components");
		} else {
			for (auto& value: this->components) {
				for (auto& value02: value.components) {
					value02.generateExcludedKeys();
				}
			}
		}
		if (this->embeds.size() == 0) {
			this->excludedKeys.emplace("embeds");
		} else {
			for (auto& value: this->embeds) {
				value.generateExcludedKeys();
			}
		}
		if (this->attachments.size() == 0) {
			this->excludedKeys.emplace("attachments");
		}
		if (this->choices.size() == 0) {
			this->excludedKeys.emplace("choices");
		}
		if (this->content == "") {
			this->excludedKeys.emplace("content");
		}
		if (this->customId == "") {
			this->excludedKeys.emplace("custom_id");
		}
		if (this->files.size() == 0) {
			this->excludedKeys.emplace("files");
		}
		if (this->title == "") {
			this->excludedKeys.emplace("title");
		}
	}

	void ComponentData::generateExcludedKeys() noexcept {
		switch (this->type) {
			case ComponentType::Button: {
				if (this->customId == "") {
					this->excludedKeys.emplace("custom_id");
				}
				if (this->emoji.name == "") {
					this->excludedKeys.emplace("emoji");
				} else {
					this->emoji.generateExcludedKeys();
				}
				if (this->label == "") {
					this->excludedKeys.emplace("label");
				}
				if (this->placeholder == "") {
					this->excludedKeys.emplace("placeholder");
				}
				if (this->url == "") {
					this->excludedKeys.emplace("url");
				}
				this->excludedKeys.emplace("options");
				this->excludedKeys.emplace("channel_types");
				this->excludedKeys.emplace("placeholder");
				this->excludedKeys.emplace("min_values");
				this->excludedKeys.emplace("max_values");
				this->excludedKeys.emplace("min_length");
				this->excludedKeys.emplace("max_length");
				this->excludedKeys.emplace("required");
				this->excludedKeys.emplace("value");
				this->excludedKeys.emplace("title");
				break;
			}
			case ComponentType::Text_Input: {
				if (this->customId == "") {
					this->excludedKeys.emplace("custom_id");
				}
				if (this->label == "") {
					this->excludedKeys.emplace("label");
				}
				if (this->maxLength == 0) {
					this->excludedKeys.emplace("max_length");
				}
				if (this->minLength == 0) {
					this->excludedKeys.emplace("min_length");
				}
				if (this->placeholder == "") {
					this->excludedKeys.emplace("placeholder");
				}
				if (this->title == "") {
					this->excludedKeys.emplace("title");
				}
				if (this->value == "") {
					this->excludedKeys.emplace("value");
				}
				this->excludedKeys.emplace("style");
				this->excludedKeys.emplace("label");
				this->excludedKeys.emplace("emoji");
				this->excludedKeys.emplace("url");
				this->excludedKeys.emplace("min_length");
				this->excludedKeys.emplace("max_length");
				this->excludedKeys.emplace("required");
				this->excludedKeys.emplace("title");
				break;
			}
			default: {
				if (this->customId == "") {
					this->excludedKeys.emplace("custom_id");
				}
				if (this->channelTypes.size() == 0) {
					this->excludedKeys.emplace("channel_types");
				}
				if (this->label == "") {
					this->excludedKeys.emplace("label");
				}
				if (this->maxValues == 0) {
					this->excludedKeys.emplace("max_values");
				}
				if (this->minValues == 0) {
					this->excludedKeys.emplace("min_values");
				}
				if (this->options.size() == 0) {
					this->excludedKeys.emplace("options");
				}
				if (this->placeholder == "") {
					this->excludedKeys.emplace("placeholder");
				}
				this->excludedKeys.emplace("style");
				this->excludedKeys.emplace("label");
				this->excludedKeys.emplace("emoji");
				this->excludedKeys.emplace("url");
				this->excludedKeys.emplace("min_length");
				this->excludedKeys.emplace("max_length");
				this->excludedKeys.emplace("required");
				this->excludedKeys.emplace("value");
				this->excludedKeys.emplace("title");
				this->excludedKeys.emplace("placeholer");
				break;
			}
		}
	}

	GuildData InputEventData::getGuildData() const {
		GuildData returnData{};
		returnData.id = this->interactionData->guildId;
		returnData = Guilds::getCachedGuild({ .guildId = this->interactionData->guildId });
		return returnData;
	}

	InteractionData InputEventData::getInteractionData() const {
		return *this->interactionData;
	}

	MessageData InputEventData::getMessageData() const {
		return this->interactionData->message;
	}

	RespondToInputEventData::operator InteractionCallbackData() const noexcept {
		InteractionCallbackData returnData{};
		returnData.allowedMentions = this->allowedMentions;
		returnData.choices = this->choices;
		returnData.components = this->components;
		returnData.content = this->content;
		returnData.customId = this->customId;
		returnData.embeds = this->embeds;
		returnData.files = this->files;
		returnData.flags = this->flags;
		returnData.title = this->title;
		returnData.tts = this->tts;
		return returnData;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InteractionData& dataPackage) {
		this->applicationId = dataPackage.applicationId;
		this->interactionToken = dataPackage.token;
		this->messageId = dataPackage.message.id;
		this->channelId = dataPackage.channelId;
		this->interactionId = dataPackage.id;
		this->eventType = dataPackage.type;
		return *this;
	};

	RespondToInputEventData::RespondToInputEventData(const InteractionData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::operator=(const InputEventData& dataPackage) {
		this->applicationId = dataPackage.getInteractionData().applicationId;
		this->interactionToken = dataPackage.getInteractionData().token;
		this->interactionId = dataPackage.getInteractionData().id;
		this->channelId = dataPackage.interactionData->channelId;
		this->messageId = dataPackage.getMessageData().id;
		return *this;
	}

	RespondToInputEventData::RespondToInputEventData(const InputEventData& dataPackage) {
		*this = dataPackage;
	}

	RespondToInputEventData& RespondToInputEventData::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addSelectMenu(bool disabled, const std::string& customIdNew,
		std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type,
		std::vector<ChannelType> channelTypes) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				this->components[this->components.size() - 1].components.emplace_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew,
		const std::string& titleNew, const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle,
		const std::string& label, const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type = ComponentType::Text_Input;
				component.customId = customIdNew;
				component.style = static_cast<int32_t>(inputStyle);
				component.title = titleNew;
				component.maxLength = maxLength;
				component.minLength = minLength;
				component.label = label;
				component.required = required;
				component.placeholder = placeholder;
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addFile(File theFile) {
		this->files.emplace_back(theFile);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setResponseType(InputEventResponseType typeNew) {
		this->type = typeNew;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addComponentRow(ActionRowData dataPackage) {
		this->components.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.emplace_back(dataPackage);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setAutoCompleteChoice(EtfSerializer value, const std::string& theName,
		std::unordered_map<std::string, std::string> theNameLocalizations) {
		ApplicationCommandOptionChoiceData choiceData{};
		choiceData.nameLocalizations = theNameLocalizations;
		choiceData.name = theName;
		switch (value.getType()) {
			case JsonType::String: {
				choiceData.type = JsonType::String;
				break;
			}
			case JsonType::Float: {
				choiceData.type = JsonType::Float;
				break;
			}
			case JsonType::Uint64: {
				choiceData.type = JsonType::Uint64;
				break;
			}
			case JsonType::Int64: {
				choiceData.type = JsonType::Int64;
				break;
			}
			case JsonType::Bool: {
				choiceData.type = JsonType::Bool;
				break;
			}
		}
		value.refreshString(SerializerSerializeType::Json);
		choiceData.value = Jsonifier::RawJsonValue{ value.operator std::string() };
		this->choices.emplace_back(choiceData);
		return *this;
	}

	RespondToInputEventData& RespondToInputEventData::setTargetUserID(const Snowflake targetUserIdNew) {
		this->targetUserId = targetUserIdNew;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel,
		ButtonStyle buttonStyle, const std::string& emojiName, Snowflake emojiId, const std::string& url) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component;
				component.type = ComponentType::Button;
				component.emoji.name = emojiName;
				component.label = buttonLabel;
				component.style = static_cast<int32_t>(buttonStyle);
				component.customId = customIdNew;
				component.disabled = disabled;
				component.emoji.id = emojiId;
				component.url = url;
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options,
		const std::string& placeholder, int32_t maxValues, int32_t minValues, SelectMenuType type, std::vector<ChannelType> channelTypes) {
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData componentData;
				componentData.type = static_cast<ComponentType>(type);
				componentData.channelTypes = channelTypes;
				componentData.placeholder = placeholder;
				componentData.customId = customIdNew;
				componentData.maxValues = maxValues;
				componentData.minValues = minValues;
				componentData.disabled = disabled;
				componentData.options = options;
				this->components[this->components.size() - 1].components.emplace_back(componentData);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew,
		const std::string& customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label,
		const std::string& placeholder) {
		this->title = topTitleNew;
		this->customId = topCustomIdNew;
		if (this->components.size() == 0) {
			ActionRowData actionRowData;
			this->components.emplace_back(actionRowData);
		}
		if (this->components.size() < 5) {
			if (this->components[this->components.size() - 1].components.size() < 5) {
				ComponentData component{};
				component.type = ComponentType::Text_Input;
				component.customId = customIdNew;
				component.style = static_cast<int32_t>(inputStyle);
				component.title = titleNew;
				component.maxLength = maxLength;
				component.minLength = minLength;
				component.label = label;
				component.required = required;
				component.placeholder = placeholder;
				this->components[this->components.size() - 1].components.emplace_back(component);
			} else if (this->components[this->components.size() - 1].components.size() == 5) {
				ActionRowData actionRowData;
				this->components.emplace_back(actionRowData);
			}
		}
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addFile(File theFile) {
		this->files.emplace_back(theFile);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addAllowedMentions(AllowedMentionsData dataPackage) {
		this->allowedMentions = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addComponentRow(ActionRowData dataPackage) {
		this->components.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addMessageEmbed(EmbedData dataPackage) {
		this->embeds.emplace_back(dataPackage);
		return *this;
	}

	MessageResponseBase& MessageResponseBase::addContent(const std::string& dataPackage) {
		this->content = dataPackage;
		return *this;
	}

	MessageResponseBase& MessageResponseBase::setTTSStatus(bool enabledTTs) {
		this->tts = enabledTTs;
		return *this;
	}

	InteractionResponseData& InteractionResponseData::operator=(const RespondToInputEventData& other) {
		this->data.allowedMentions = other.allowedMentions;
		this->data.components = other.components;
		this->data.customId = other.customId;
		this->data.choices = other.choices;
		this->data.content = other.content;
		this->data.embeds = other.embeds;
		this->data.title = other.title;
		this->data.files = other.files;
		this->data.flags = other.flags;
		this->data.tts = other.tts;
		return *this;
	}

	InteractionResponseData::InteractionResponseData(const RespondToInputEventData& other) {
		*this = other;
	}

	void parseCommandDataOption(std::unordered_map<std::string, JsonStringValue>& values, ApplicationCommandInteractionDataOption& data) {
		JsonStringValue valueNew{};
		valueNew.value = data.value;
		values.insert_or_assign(data.name, valueNew);
		for (auto& value: data.options) {
			parseCommandDataOption(values, value);
		}
	}

	CommandData& CommandData::operator=(const CommandData& other) noexcept {
		this->subCommandGroupName = other.subCommandGroupName;
		this->subCommandName = other.subCommandName;
		this->commandName = other.commandName;
		this->optionsArgs = other.optionsArgs;
		this->eventData = other.eventData;
		return *this;
	}

	CommandData::CommandData(const CommandData& other) noexcept {
		*this = other;
	}

	CommandData::CommandData(InputEventData inputEventData) {
		if (inputEventData.interactionData->data.name != "") {
			this->commandName = inputEventData.interactionData->data.name;
		}
		if (inputEventData.interactionData->data.targetId != 0) {
			this->optionsArgs.values.emplace("target_id",
				JsonStringValue{ .value = inputEventData.interactionData->data.targetId.operator std::string(), .type = JsonType::String });
		} else if (inputEventData.interactionData->data.targetId != 0) {
			this->optionsArgs.values.emplace("target_id",
				JsonStringValue{ .value = inputEventData.interactionData->data.targetId.operator std::string(), .type = JsonType::String });
		}
		this->eventData = inputEventData;
		for (auto& value: this->eventData.interactionData->data.options) {
			JsonStringValue serializer{};
			this->optionsArgs.values.insert_or_assign(value.name, serializer);
			parseCommandDataOption(this->optionsArgs.values, value);
		}
		for (auto& value: inputEventData.interactionData->data.options) {
			if (value.type == ApplicationCommandOptionType::Sub_Command) {
				this->subCommandName = value.name;
			}
			if (value.type == ApplicationCommandOptionType::Sub_Command_Group) {
				this->subCommandGroupName = value.name;
			}
		}
	}

	InteractionData CommandData::getInteractionData() {
		return *this->eventData.interactionData;
	}

	GuildMemberData CommandData::getGuildMemberData() {
		return this->eventData.getGuildMemberData();
	}

	ChannelData CommandData::getChannelData() {
		return this->eventData.getChannelData();
	}

	MessageData CommandData::getMessageData() {
		return this->eventData.getMessageData();
	}

	GuildData CommandData::getGuildData() {
		return this->eventData.getGuildData();
	}

	UserData CommandData::getUserData() {
		auto returnData = this->eventData.getUserData();
		return returnData;
	}

	std::string CommandData::getCommandName() {
		return this->commandName;
	}

	std::string CommandData::getSubCommandName() {
		return this->subCommandName;
	}

	std::string CommandData::getSubCommandGroupName() {
		return this->subCommandGroupName;
	}

	InputEventData CommandData::getInputEventData() {
		return this->eventData;
	}

	BaseFunctionArguments::BaseFunctionArguments(CommandData commanddataNew, DiscordCoreClient* discordCoreClientNew) : CommandData(commanddataNew) {
		this->discordCoreClient = discordCoreClientNew;
	}

	MoveThroughMessagePagesData moveThroughMessagePages(const std::string& userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const std::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
		MoveThroughMessagePagesData returnData{};
		uint32_t newCurrentPageIndex = currentPageIndex;
		StopWatch stopWatch{ Milliseconds{ waitForMaxMs } };
		auto createResponseData = std::make_unique<CreateInteractionResponseData>(originalEvent);
		auto interactionResponse = std::make_unique<RespondToInputEventData>(originalEvent);
		auto embedData = std::make_unique<EmbedData>();
		embedData->setColor("FEFEFE");
		embedData->setTitle("__**Permissions Issue:**__");
		embedData->setTimeStamp(getTimeAndDate());
		embedData->setDescription("Sorry, but that button can only be pressed by <@" + userID + ">!");
		createResponseData->addMessageEmbed(*embedData);
		createResponseData->setResponseType(InteractionCallbackType::Channel_Message_With_Source);
		if (messageEmbeds.size() > 0) {
			interactionResponse->addMessageEmbed(messageEmbeds[currentPageIndex]);
		}
		if (returnResult) {
			interactionResponse->addButton(false, "select", "Select", ButtonStyle::Success, "✅");
		}
		interactionResponse->addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
		interactionResponse->addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
		interactionResponse->addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
		interactionResponse->setResponseType(InputEventResponseType::Edit_Interaction_Response);
		originalEvent = InputEvents::respondToInputEventAsync(*interactionResponse).get();
		while (!stopWatch.hasTimePassed()) {
			std::this_thread::sleep_for(1ms);
			std::unique_ptr<ButtonCollector> button{ std::make_unique<ButtonCollector>(originalEvent) };
			std::vector<ButtonResponseData> buttonIntData{
				button->collectButtonData(false, waitForMaxMs, 1, *createResponseData, Snowflake{ stoull(userID) }).get()
			};
			std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
			if (buttonIntData.size() == 0 || buttonIntData.at(0).buttonId == "empty" || buttonIntData.at(0).buttonId == "exit") {
				std::unique_ptr<RespondToInputEventData> dataPackage02{ std::make_unique<RespondToInputEventData>(originalEvent) };
				if (buttonIntData.at(0).buttonId == "empty") {
					*dataPackage02 = originalEvent;
				} else {
					interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
					*dataPackage02 = RespondToInputEventData{ *interactionData };
				}

				dataPackage02->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				if (returnResult) {
					dataPackage02->addButton(true, "select", "Select", ButtonStyle::Success, "✅");
				}
				dataPackage02->addButton(true, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
				dataPackage02->addButton(true, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
				dataPackage02->addButton(true, "exit", "Exit", ButtonStyle::Danger, "❌");
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					dataPackage02->setResponseType(InputEventResponseType::Edit_Interaction_Response);
					InputEvents::respondToInputEventAsync(*dataPackage02).get();
				}
				MoveThroughMessagePagesData dataPackage03{};
				dataPackage03.inputEventData = originalEvent;
				dataPackage03.buttonId = "exit";
				return dataPackage03;
			} else if (buttonIntData.at(0).buttonId == "forwards" || buttonIntData.at(0).buttonId == "backwards") {
				if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
					newCurrentPageIndex = 0;
				} else if (buttonIntData.at(0).buttonId == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
					++newCurrentPageIndex;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex > 0)) {
					--newCurrentPageIndex;
				} else if (buttonIntData.at(0).buttonId == "backwards" && (newCurrentPageIndex == 0)) {
					newCurrentPageIndex = static_cast<uint32_t>(messageEmbeds.size()) - 1;
				}
				interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
				auto dataPackage = RespondToInputEventData{ *interactionData };
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				if (returnResult) {
					dataPackage.addButton(false, "select", "Select", ButtonStyle::Success, "✅");
				}
				dataPackage.addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
				dataPackage.addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
				dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
				dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
				InputEvents::respondToInputEventAsync(dataPackage).get();
			} else if (buttonIntData.at(0).buttonId == "select") {
				if (deleteAfter == true) {
					InputEventData dataPackage03{ originalEvent };
					InputEvents::deleteInputEventResponseAsync(dataPackage03);
				} else {
					std::unique_ptr<InteractionData> interactionData = std::make_unique<InteractionData>(buttonIntData.at(0));
					auto dataPackage = RespondToInputEventData{ *interactionData };
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
					if (returnResult) {
						dataPackage.addButton(false, "select", "Select", ButtonStyle::Success, "✅");
					}
					dataPackage.addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
					dataPackage.addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
					dataPackage.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
					originalEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				}
				returnData.currentPageIndex = newCurrentPageIndex;
				returnData.inputEventData = originalEvent;
				returnData.buttonId = buttonIntData.at(0).buttonId;
				return returnData;
			}
		};
		return returnData;
	};

};