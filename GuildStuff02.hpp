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
		if (DiscordCoreClientBase::voiceConnectionMap->contains(this->data.id)) {
			voiceConnectionPtr = DiscordCoreClientBase::voiceConnectionMap->at(this->data.id);
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
					voiceConnectionPtr = make_shared<VoiceConnection>(DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get(), voiceConnectData, DiscordCoreClientBase::audioBuffersMap.at(this->data.id));
				}
				else {
					this->discordCoreClientBase->audioBuffersMap.insert(make_pair(this->data.id, make_shared<unbounded_buffer<AudioFrameData*>>()));
					voiceConnectionPtr = make_shared<VoiceConnection>(DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get(), voiceConnectData, DiscordCoreClientBase::audioBuffersMap.at(this->data.id));
				}
				DiscordCoreClientBase::pWebSocketConnectionAgent->setVoiceConnectionWebSocket(voiceConnectionPtr->voicechannelWebSocketAgent);
				DiscordCoreClientBase::voiceConnectionMap->insert(make_pair(this->data.id, voiceConnectionPtr));
				return voiceConnectionPtr;
			}
		}
		return voiceConnectionPtr;
	}

	void Guild::disconnectFromVoice() {
		if (DiscordCoreClientBase::voiceConnectionMap->contains(this->data.id)) {
			DiscordCoreClientBase::voiceConnectionMap->at(this->data.id)->terminate();
			this->discordCoreClientBase->currentUser->updateVoiceStatus({ .guildId = this->data.id,.channelId = "", .selfMute = false,.selfDeaf = false });
			return;
		}
	}
	
}

#endif