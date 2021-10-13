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

		template <class _Ty>
		friend 	_CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreClient;
		friend class SoundCloudAPI;
		friend class SongAPI;
		friend class SongAPI;
		friend class Guild;

		SoundCloudAPI(string guildIdNew);

		static void initialize(map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMapNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMapNew, map<string, shared_ptr<VoiceConnection>>* voiceConnectionMapNew);

		static void cleanup();

		~SoundCloudAPI();

	protected:

		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap;
		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;
		static map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMap;

		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioDataBuffer{ nullptr };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		concurrency::event readyToQuitEventOut {};
		concurrency::event readyToQuitEventIn {};
		std::jthread* threadOfExecution{};
		SoundCloudSong theSong{ };
		string guildId{ "" };

		bool stop();

		void sendNextSong(Song newSong);

		void sendEmptyingFrames(unbounded_buffer<vector<unsigned __int8>>* sendAudioDataBufferNew);

		static void downloadAndStreamAudio(stop_token stopToken, Song newSong, SoundCloudAPI* soundCloudAPI);

		static bool stop(string guildId);

		static void sendNextSong(Song newSong, string guildId);

		static vector<SoundCloudSong> searchForSong(string searchQuery, string guildId);

	};

};
#endif