// YouTubeAPI.hpp - Header for the YouTube API related stuff.
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _YOUTUBE_API_
#define _YOUTUBE_API_

#include "IndexInitial.hpp"
#include "Http.hpp"
#include "DataParsingFunctions.hpp"
#include "VoiceConnection.hpp"

namespace DiscordCoreAPI {

	DiscordCoreAPI_Dll string between(string body, string left, string right);

	DiscordCoreAPI_Dll vector<char> splitString(string stringToSplit);

	DiscordCoreAPI_Dll vector<char> sliceVector(vector<char> vectorToSlice,  __int32 firstElement,  __int32 lastElement = 0);

	DiscordCoreAPI_Dll vector<char> reverseString(vector<char> stringToReverse);

	DiscordCoreAPI_Dll vector<char> swapHeadAndPosition(vector<char> inputVector, __int32 position);

	DiscordCoreAPI_Dll string joinString(vector<char> stringToJoin);

	DiscordCoreAPI_Dll string setDownloadURL(YouTubeFormat format);

	DiscordCoreAPI_Dll vector<string> extractActions(string html5PlayerPageBody);

	DiscordCoreAPI_Dll vector<string> getTokens(string html5PlayerFile);

	DiscordCoreAPI_Dll string decipher(vector<string> tokens, string cipherSignature);

	DiscordCoreAPI_Dll YouTubeFormat decipherFormat(YouTubeFormat format, string html5playerFile);

	class DiscordCoreAPI_Dll YouTubeAPI  {
	public:

		template <class _Ty>
		friend 	_CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreClient;
		friend class YouTubeSong;
		friend class SongAPI;
		friend class Guild;

		YouTubeAPI(string guildIdNew);

		static void initialize(map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMapNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMapNew, map<string, shared_ptr<VoiceConnection>>* voiceConnectionMapNew);

		static void cleanup();

		~YouTubeAPI();

	protected:

		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioDataBufferMap;
		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;
		static map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMap;

		shared_ptr<unbounded_buffer<AudioFrameData>> sendAudioDataBuffer{ nullptr };
		shared_ptr<VoiceConnection> voiceConnection{ nullptr };
		concurrency::event readyToQuitEventOut {};
		concurrency::event readyToQuitEventIn {};
		SongDecoder* songDecoder{ nullptr };
		SongEncoder* songEncoder{ nullptr };
		std::jthread* threadOfExecution{};
		const __int32 maxBufSize{ 8192 };
		const string newLine{ "\n\r" };
		YouTubeSong theSong{};
		string guildId{ "" };

		bool stop();

		void sendNextSong(Song newSong);

		void sendEmptyingFrames(unbounded_buffer<vector<unsigned __int8>>* sendAudioDataBufferNew);

		static void downloadAndStreamAudio(stop_token stopToken, Song newSong, YouTubeAPI* youtubeAPI);

		static bool stop(string guildId);

		static void sendNextSong(Song newSong, string guildId);

		static vector<YouTubeSong> searchForSong(string searchQuery, string guildId);

	};
};
#endif