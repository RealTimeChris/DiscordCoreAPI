// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris
#pragma once

#include "IndexInitial.hpp"
#include "CoRoutine.hpp"
#include "SongEncoder.hpp"
#include "SongDecoder.hpp"

namespace DiscordCoreAPI {
	
	class DiscordCoreAPI_Dll SoundCloudAPI {
	public:

		friend class DiscordCoreClient;
		friend class VoiceConnection;
		friend class SoundCloudAPI;
		friend class SongAPI;
		friend class SongAPI;
		friend class Guild;

		SoundCloudAPI(string guildId) noexcept;

		~SoundCloudAPI();

	protected:

		unique_ptr<SongDecoder> songDecoder{ nullptr };
		unique_ptr<SongEncoder> songEncoder{ nullptr };
		concurrency::event readyToQuitEventOut {};
		concurrency::event readyToQuitEventIn {};
		CoRoutine<void> currentlyRunningSong{};
		const int32_t maxBufferSize{ 8192 };
		SoundCloudSong theSong{ };
		string guildId{ "" };

		bool stop();

		void sendNextSong(Song newSong);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI);

		void sendEmptyingFrames(unbounded_buffer<vector<uint8_t>>& sendAudioDataBufferNew);

		static vector<SoundCloudSong> searchForSong(string searchQuery, string guildId);

	};
	
};