// YouTubeAPI.hpp - Header for the YouTube API related stuff.
// Jun 30, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "CoRoutine.hpp"
#include "FoundationEntities.hpp"
#include "AudioEncoder.hpp"
#include "AudioDecoder.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll YouTubeRequestBuilder {
	public:

		YouTubeRequestBuilder() = default;

		static YouTubeSong constructFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong);

		static vector<YouTubeSong> constructFirstDownloadUrl(string theString);

		static void initialize(shared_ptr<DiscordCoreInternal::HttpClient>);

	protected:

		static shared_ptr<DiscordCoreInternal::HttpClient> httpClient;
		static string baseUrl;

		static YouTubeSong constructDownloadInfo(GuildMemberData guildMember, YouTubeSong newSong);

		static YouTubeSong constructFinalDownloadUrl(Song newSong);

		static vector<char> sliceVector(vector<char> vectorToSlice, int32_t firstElement, int32_t lastElement = 0);
		
		static vector<char> swapHeadAndPosition(vector<char> inputVector, int32_t position);
		
		static YouTubeFormat decipherFormat(YouTubeFormat format, string html5playerFile);

		static string decipher(vector<string> tokens, string cipherSignature);

		static vector<string> extractActions(string html5PlayerPageBody);

		static vector<char> reverseString(vector<char> stringToReverse);

		static string between(string body, string left, string right);

		static vector<string> getTokens(string html5PlayerFile);

		static vector<char> splitString(string stringToSplit);

		static string joinString(vector<char> stringToJoin);

		static string setDownloadUrl(YouTubeFormat format);

	};

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

		EventWaiter readyToQuitEventOut{};
		EventWaiter readyToQuitEventIn{};
		const int32_t maxBufferSize{ 8192 };
		CoRoutineWrapper theTask{ nullptr };
		YouTubeSong theSong{};
		string guildId{ "" };
		mutex accessMutex{};

		void sendEmptyingFrames(TSUnboundedMessageBlock<vector<uint8_t>>& sendAudioDataBufferNew);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, YouTubeAPI* youtubeAPI);

		static vector<YouTubeSong> searchForSong(string searchQuery, string guildId);

		void sendNextSong(Song newSong);

		void cancelCurrentSong();
		
		bool stop();
	};

};