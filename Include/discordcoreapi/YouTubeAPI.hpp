/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// YouTubeAPI.hpp - Header for the YouTube API related stuff.
/// Jun 30, 2021
/// Chris M.
/// https://discordcoreapi.com
///
/// \file YouTubeAPI.hpp

#pragma once

#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/AudioEncoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <regex>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll YouTubeRequestBuilder {
	  public:
		YouTubeRequestBuilder() = default;

		YouTubeRequestBuilder(DiscordCoreInternal::HttpClient*);

		Song collectFinalSong(GuildMemberData addedByGuildMember, Song newSong);

		std::vector<Song> collectSearchResults(std::string theString);

	  protected:
		DiscordCoreInternal::HttpClient* httpClient{ nullptr };
		std::string baseUrl{ "https://www.youtube.com" };

		Song constructDownloadInfo(GuildMemberData guildMember, Song newSong);

		Song constructFinalDownloadUrl(Song newSong);

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

		void weFailedToDownloadOrDecode(Song newSong, YouTubeAPI* youtubeAPI, std::stop_token theToken, int32_t currentRecursionDepth);

		void downloadAndStreamAudio(Song newSong, YouTubeAPI* youtubeAPI, std::stop_token theToken, int32_t currentRecursionDepth);

		Song collectFinalSong(GuildMemberData addedByGuildMember, Song newSong);

		std::vector<Song> searchForSong(std::string searchQuery);

		void cancelCurrentSong();

	  protected:
		YouTubeRequestBuilder requestBuilder{};
		const int32_t maxBufferSize{ 8192 };
		std::string guildId{ "" };
		bool doWePrint{ false };
		Song theSong{};
	};

};// namespace DiscordCoreAPI
