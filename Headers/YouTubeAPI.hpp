// YouTubeAPI.hpp - Header for the YouTube API related stuff.
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "CoRoutine.hpp"
#include "SongEncoder.hpp"
#include "SongDecoder.hpp"

namespace DiscordCoreAPI {

	DiscordCoreAPI_Dll string between(string body, string left, string right);

	DiscordCoreAPI_Dll vector<char> splitString(string stringToSplit);

	DiscordCoreAPI_Dll vector<char> sliceVector(vector<char> vectorToSlice, int32_t firstElement, int32_t lastElement = 0);

	DiscordCoreAPI_Dll vector<char> reverseString(vector<char> stringToReverse);

	DiscordCoreAPI_Dll vector<char> swapHeadAndPosition(vector<char> inputVector, int32_t position);

	DiscordCoreAPI_Dll string joinString(vector<char> stringToJoin);

	DiscordCoreAPI_Dll string setDownloadURL(YouTubeFormat format);

	DiscordCoreAPI_Dll vector<string> extractActions(string html5PlayerPageBody);

	DiscordCoreAPI_Dll vector<string> getTokens(string html5PlayerFile);

	DiscordCoreAPI_Dll string decipher(vector<string> tokens, string cipherSignature);

	DiscordCoreAPI_Dll YouTubeFormat decipherFormat(YouTubeFormat format, string html5playerFile);

	class DiscordCoreAPI_Dll YouTubeAPI {
	public:

		friend class DiscordCoreClient;
		friend class VoiceConnection;
		friend class YouTubeSong;
		friend class SongAPI;
		friend class Guild;

		YouTubeAPI(string guildId);

		~YouTubeAPI();

	protected:

		unique_ptr<SongDecoder> songDecoder{ nullptr };
		unique_ptr<SongEncoder> songEncoder{ nullptr };
		unique_ptr<CoRoutine<void>> theTask{ nullptr };
		Event<void, void> readyToQuitEventOut{};
		Event<void, void> readyToQuitEventIn{};
		const int32_t maxBufferSize{ 8192 };
		YouTubeSong theSong{};
		string guildId{ "" };

		void sendEmptyingFrames(TSUnboundedMessageBlock<vector<uint8_t>>& sendAudioDataBufferNew);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, YouTubeAPI* youtubeAPI);

		static vector<YouTubeSong> searchForSong(string searchQuery, string guildId);

		void sendNextSong(Song newSong);

		void cancelCurrentSong();
		
		bool stop();
	};

};