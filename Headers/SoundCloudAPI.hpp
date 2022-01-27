// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "CoRoutine.hpp"
#include "AudioEncoder.hpp"
#include "Http.hpp"
#include "AudioDecoder.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SoundCloudRequestBuilder {
	public:

		SoundCloudRequestBuilder(DiscordCoreInternal::HttpClient*);
		
		SoundCloudSong collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

		std::vector<DiscordCoreInternal::HttpData> submitWorkloadAndGetResult(std::vector<DiscordCoreInternal::HttpWorkloadData> httpData);

		std::vector<SoundCloudSong> collectSearchResults(std::string theString);

	protected:
		
		DiscordCoreInternal::HttpClient* httpClient{ nullptr };
		std::string baseUrl02{ "https://api-v2.soundcloud.com" };
		std::string baseUrl{ "https://soundcloud.com" };
		std::string appVersion{ "1631696495" };
		std::string clientId{ "" };

		SoundCloudSong constructSecondDownloadUrl(SoundCloudSong newSong);

		SoundCloudSong constructFinalDownloadUrl(SoundCloudSong newSong);

		std::string collectClientId();

	};

	class DiscordCoreAPI_Dll SoundCloudAPI {
	public:

		SoundCloudAPI(std::string guildId, DiscordCoreInternal::HttpClient* httpClient);

		SoundCloudSong collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

		std::vector<SoundCloudSong> searchForSong(std::string searchQuery, std::string guildId);

		void sendNextSong(Song newSong);

		bool stop();

		~SoundCloudAPI();

	protected:

		std::unique_ptr<CoRoutine<void>> theTask{ nullptr };
		SoundCloudRequestBuilder requestBuilder;
		const int32_t maxBufferSize{ 8192 };
		SoundCloudSong theSong{ };
		std::string guildId{ "" };

		void sendEmptyingFrames(TSUnboundedMessageBlock<std::vector<uint8_t>>& sendAudioDataBufferNew);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI);

		void cancelCurrentSong();
		
	};

};