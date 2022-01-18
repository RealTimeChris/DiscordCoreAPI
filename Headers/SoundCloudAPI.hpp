// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "CoRoutine.hpp"
#include "AudioEncoder.hpp"
#include "AudioDecoder.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SoundCloudRequestBuilder {
	public:

		friend class SoundCloudAPI;

		SoundCloudRequestBuilder(shared_ptr<DiscordCoreInternal::HttpClient>);
		
		SoundCloudSong collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

		vector<SoundCloudSong> collectSearchResults(string theString);

	protected:
		
		shared_ptr<DiscordCoreInternal::HttpClient> httpClient{ nullptr };
		string baseUrl02{ "https://api-v2.soundcloud.com" };
		string baseUrl{ "https://soundcloud.com" };
		string appVersion{ "1631696495" };
		string clientId{ "" };

		SoundCloudSong constructSecondDownloadUrl(SoundCloudSong newSong);

		SoundCloudSong constructFinalDownloadUrl(SoundCloudSong newSong);

		string collectClientId();

	};

	class DiscordCoreAPI_Dll SoundCloudAPI {
	public:

		friend class DiscordCoreClient;
		friend class VoiceConnection;
		friend class SoundCloudAPI;
		friend class SongAPI;
		friend class Guild;

		SoundCloudAPI(string guildId, shared_ptr<DiscordCoreInternal::HttpClient> httpClient);

		~SoundCloudAPI();

	protected:

		SoundCloudRequestBuilder requestBuilder;
		const int32_t maxBufferSize{ 8192 };
		CoRoutineWrapper theTask{ nullptr };
		EventWaiter readyToQuitEventOut{};
		EventWaiter readyToQuitEventIn{};
		SoundCloudSong theSong{ };
		string guildId{ "" };
		mutex accessMutex{};

		void sendEmptyingFrames(TSUnboundedMessageBlock<vector<uint8_t>>& sendAudioDataBufferNew);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI);

		vector<SoundCloudSong> searchForSong(string searchQuery, string guildId);

		void sendNextSong(Song newSong);

		void cancelCurrentSong();

		bool stop();
	};

};