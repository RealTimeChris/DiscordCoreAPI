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

		YouTubeRequestBuilder(shared_ptr<DiscordCoreInternal::HttpClient>);

		YouTubeSong collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong);

		vector<YouTubeSong> collectSearchResults(string theString);

	protected:

		shared_ptr<DiscordCoreInternal::HttpClient> httpClient{ nullptr };
		string baseUrl{ "https://www.youtube.com" };

		YouTubeSong constructDownloadInfo(GuildMemberData guildMember, YouTubeSong newSong);

		YouTubeSong constructFinalDownloadUrl(Song newSong);

		vector<char> sliceVector(vector<char> vectorToSlice, int32_t firstElement, int32_t lastElement = 0);
		
		vector<char> swapHeadAndPosition(vector<char> inputVector, int32_t position);
		
		YouTubeFormat decipherFormat(YouTubeFormat format, string html5playerFile);

		string decipher(vector<string> tokens, string cipherSignature);

		vector<string> extractActions(string html5PlayerPageBody);

		vector<char> reverseString(vector<char> stringToReverse);

		string between(string body, string left, string right);

		vector<string> getTokens(string html5PlayerFile);

		vector<char> splitString(string stringToSplit);

		string joinString(vector<char> stringToJoin);

		string setDownloadUrl(YouTubeFormat format);

	};

	class DiscordCoreAPI_Dll YouTubeAPI {
	public:

		YouTubeAPI(string guildId, shared_ptr<DiscordCoreInternal::HttpClient> httpClient);

		YouTubeSong collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong);

		vector<YouTubeSong> searchForSong(string searchQuery, string guildId);

		void sendNextSong(Song newSong);

		bool stop();

		~YouTubeAPI();

	protected:

		YouTubeRequestBuilder requestBuilder;
		const int32_t maxBufferSize{ 8192 };
		CoRoutineWrapper theTask{ nullptr };
		EventWaiter readyToQuitEventOut{};
		EventWaiter readyToQuitEventIn{};
		YouTubeSong theSong{};
		string guildId{ "" };

		void sendEmptyingFrames(TSUnboundedMessageBlock<vector<uint8_t>>& sendAudioDataBufferNew);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, YouTubeAPI* youtubeAPI);

		void cancelCurrentSong();		
		
	};

};