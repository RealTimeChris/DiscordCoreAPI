// GuildStuff02.hpp - Header #2 for the "guild stuff".
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_02_
#define _GUILD_STUFF_02_

#include "../pch.h"

namespace DiscordCoreAPI {

	shared_ptr<VoiceConnection> Guild::connectToVoice(string channelId, shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> websocketAgent) {
		if (channelId != "") {
			if ((this->voiceConnection == nullptr || this->voiceConnection->voiceConnectionData.channelId != channelId)) {
				auto voiceConnectData = websocketAgent->getVoiceConnectionData(channelId, this->data.id);
				voiceConnectData.channelId = channelId;
				voiceConnectData.guildId = this->data.id;
				voiceConnectData.endpoint = "wss://" + voiceConnectData.endpoint + "/?v=4";
				voiceConnectData.userId = this->discordCoreClientBase->currentUser->data.id;
				this->voiceConnection = make_shared<VoiceConnection>(voiceConnectData, this->discordCoreClientBase->audioBuffersMap.at(this->data.id));
				this->discordCoreClientBase->pWebSocketConnectionAgent->setVoiceConnectionWebSocket(this->voiceConnection->voicechannelWebSocketAgent);
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
	
}

#endif