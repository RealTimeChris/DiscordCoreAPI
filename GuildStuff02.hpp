// GuildStuff02.hpp - Header #2 for the "guild stuff".
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_STUFF_02_
#define _GUILD_STUFF_02_

#include "../pch.h"
#include "../DiscordCoreAPI-main/DiscordCoreClientBase.hpp"

namespace DiscordCoreAPI {

	shared_ptr<VoiceConnection> Guild::connectToVoice(string channelId) {
		shared_ptr<VoiceConnection> voiceConnectionPtr;
		if (Guild::voiceConnectionMap->contains(this->data.id)) {
			voiceConnectionPtr = Guild::voiceConnectionMap->at(this->data.id);
			cout << "WERE HERE WERE HERE 020202312312313" << endl;
			return voiceConnectionPtr;
		}
		else if (channelId != "") {
			if ((voiceConnectionPtr== nullptr || voiceConnectionPtr->voiceConnectionData.channelId != channelId)) {
				auto voiceConnectData = DiscordCoreClientBase::pWebSocketConnectionAgent->getVoiceConnectionData(channelId, this->data.id);
				voiceConnectData.channelId = channelId;
				voiceConnectData.guildId = this->data.id;
				voiceConnectData.endpoint = "wss://" + voiceConnectData.endpoint + "/?v=4";
				voiceConnectData.userId = this->discordCoreClientBase->currentUser->data.id;
				if (this->discordCoreClientBase->audioBuffersMap.contains(this->data.id)) {
					voiceConnectionPtr = make_shared<VoiceConnection>(voiceConnectData, this->discordCoreClientBase->audioBuffersMap.at(this->data.id), Guild::voiceConnectionMap);
				}
				else {
					this->discordCoreClientBase->audioBuffersMap.insert(make_pair(this->data.id, make_shared<unbounded_buffer<AudioFrameData*>>()));
					voiceConnectionPtr = make_shared<VoiceConnection>(voiceConnectData, this->discordCoreClientBase->audioBuffersMap.at(this->data.id), Guild::voiceConnectionMap);
				}
				this->discordCoreClientBase->pWebSocketConnectionAgent->setVoiceConnectionWebSocket(voiceConnectionPtr->voicechannelWebSocketAgent);
				Guild::voiceConnectionMap->insert(make_pair(this->data.id, voiceConnectionPtr));
				return voiceConnectionPtr;
			}
		}
		return voiceConnectionPtr;
	}

	void Guild::disconnectFromVoice() {
		if (Guild::voiceConnectionMap->contains(this->data.id)) {
			Guild::voiceConnectionMap->at(this->data.id)->terminate();
			this->discordCoreClientBase->currentUser->updateVoiceStatus({ .guildId = this->data.id,.channelId = "", .selfMute = false,.selfDeaf = false });
			return;
		}
	}
	
}

#endif