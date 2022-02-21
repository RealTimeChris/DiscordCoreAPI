/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// YouTubeAPI.hpp - Header for the YouTube API related stuff.
/// Jun 30, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <CoRoutine.hpp>
#include <FoundationEntities.hpp>
#include <AudioEncoder.hpp>
#include <AudioDecoder.hpp>
#include <regex>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll YouTubeRequestBuilder {
	public:

		YouTubeRequestBuilder() = default;

		YouTubeRequestBuilder(DiscordCoreInternal::HttpClient*);

		YouTubeSong collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong);

		std::vector<YouTubeSong> collectSearchResults(std::string theString);

	protected:

		DiscordCoreInternal::HttpClient* httpClient{ nullptr };
		std::string baseUrl{ "https://www.youtube.com" };

		YouTubeSong constructDownloadInfo(GuildMemberData guildMember, YouTubeSong newSong);

		YouTubeSong constructFinalDownloadUrl(Song newSong);

		std::vector<char> sliceVector(std::vector<char> vectorToSlice, int32_t firstElement, int32_t lastElement = 0);
		
		std::vector<char> swapHeadAndPosition(std::vector<char> inputVector, int32_t position);
		
		YouTubeFormat decipherFormat(YouTubeFormat format, std::string html5playerFile);

		std::string decipher(std::vector<std::string> tokens, std::string cipherSignature);

		std::vector<std::string> extractActions(std::string html5PlayerPageBody);

		std::vector<char> reverseString(std::vector<char> stringToReverse);

		std::string between(std::string body, std::string left, std::string right);

		std::vector<std::string> getTokens(std::string html5PlayerFile);

		std::vector<char> splitString(std::string stringToSplit);

		std::string joinString(std::vector<char> stringToJoin);

		std::string setDownloadUrl(YouTubeFormat format);

	};

	class DiscordCoreAPI_Dll YouTubeAPI {
	public:

		YouTubeAPI(std::string guildId, DiscordCoreInternal::HttpClient* httpClient);

		YouTubeSong collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong);

		CoRoutine<void> downloadAndStreamAudio(Song newSong, YouTubeAPI* youtubeAPI);

		std::vector<YouTubeSong> searchForSong(std::string searchQuery);

		void stop();

	protected:

		YouTubeRequestBuilder requestBuilder{};
		const int32_t maxBufferSize{ 8192 };
		std::string guildId{ "" };
		YouTubeSong theSong{};

		void cancelCurrentSong();		
		
	};

};
