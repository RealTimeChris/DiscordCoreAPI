// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris
#pragma once

#include "IndexInitial.hpp"
#include "Http.hpp"
#include "DataParsingFunctions.hpp"
#include "VoiceConnection.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SoundCloudAPI {
	public:

		friend class DiscordCoreClient;
		friend class VoiceConnection;
		friend class SoundCloudAPI;
		friend class SongAPI;
		friend class SongAPI;
		friend class Guild;

		SoundCloudAPI(string guildId);

		~SoundCloudAPI();

	protected:

		concurrency::event readyToQuitEventOut {};
		concurrency::event readyToQuitEventIn {};
		CoRoutine<void> currentlyRunningSong{};
		const __int32 maxBufferSize{ 8192 };
		SongDecoder* songDecoder{ nullptr };
		SongEncoder* songEncoder{ nullptr };
		SoundCloudSong theSong{ };
		string guildId{ "" };

		bool stop();

		void sendNextSong(Song newSong);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, SoundCloudAPI* soundCloudAPI);

		void sendEmptyingFrames(unbounded_buffer<vector<unsigned __int8>>* sendAudioDataBufferNew);

		static vector<SoundCloudSong> searchForSong(string searchQuery, string guildId);

	};
};