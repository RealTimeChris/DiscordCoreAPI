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

		SoundCloudRequestBuilder() = default;
		
		static SoundCloudSong constructFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

		static vector<SoundCloudSong> constructFirstDownloadUrl(string theString);

		static SoundCloudSong constructSecondDownloadUrl(SoundCloudSong newSong);

		static SoundCloudSong constructFinalDownloadUrl(SoundCloudSong newSong);
		
		static string collectClientId();

		static void initialize(shared_ptr<DiscordCoreInternal::HttpClient>);		

	protected:
		
		static shared_ptr<DiscordCoreInternal::HttpClient> httpClient;

		static string appVersion;
		static string baseUrl02;
		static string clientId;
		static string baseUrl;

	};

	class DiscordCoreAPI_Dll SoundCloudAPI {
	public:

		friend class DiscordCoreClient;
		friend class VoiceConnection;
		friend class SoundCloudAPI;
		friend class SongAPI;
		friend class Guild;

		SoundCloudAPI(string guildId) noexcept;

		~SoundCloudAPI();

	protected:

		EventWaiter readyToQuitEventOut{};
		EventWaiter readyToQuitEventIn{};
		const int32_t maxBufferSize{ 8192 };
		CoRoutineWrapper theTask{ nullptr };
		SoundCloudSong theSong{ };
		string guildId{ "" };
		mutex accessMutex{};

		void sendEmptyingFrames(TSUnboundedMessageBlock<vector<uint8_t>>& sendAudioDataBufferNew);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI);

		static vector<SoundCloudSong> searchForSong(string searchQuery, string guildId);

		void sendNextSong(Song newSong);

		void cancelCurrentSong();

		bool stop();
	};

};