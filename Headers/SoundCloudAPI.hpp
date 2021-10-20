// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SOUNDCLOUD_API_
#define _SOUNDCLOUD_API_

#include "IndexInitial.hpp"
#include "Http.hpp"
#include "DataParsingFunctions.hpp"
#include "VoiceConnection.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SoundCloudAPI  {
	public:

		friend void downloadAndStreamAudio(std::stop_token stopToken, Song newSong, SoundCloudAPI* soundCloudAPI);
		template <class _Ty>
		friend 	_CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
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
		const __int32 maxBufferSize{ 16384 };
		SongDecoder* songDecoder{ nullptr };
		SongEncoder* songEncoder{ nullptr };
		std::jthread* threadOfExecution{};
		SoundCloudSong theSong{ };
		string guildId{ "" };

		bool stop();

		void sendNextSong(Song newSong);

		void sendEmptyingFrames(unbounded_buffer<vector<unsigned __int8>>* sendAudioDataBufferNew);

		static bool stop(string guildId);

		static void sendNextSong(Song newSong, string guildId);

		static vector<SoundCloudSong> searchForSong(string searchQuery, string guildId);

	};

};
#endif