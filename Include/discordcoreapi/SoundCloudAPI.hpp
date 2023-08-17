/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// SoundCloudAPI.hpp - Header for the SoundCloud api related stuff.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file SoundCloudAPI.hpp

#pragma once

#include <discordcoreapi/Utilities/AudioDecoder.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		class DiscordCoreAPI_Dll SoundCloudRequestBuilder : public HttpsClientCore {
		  public:
			SoundCloudRequestBuilder(ConfigManager* configManagerNew);

		  protected:
			static std::string clientId;
			static constexpr std::string_view baseUrl02{ "https://api-v2.soundcloud.com" };
			static constexpr std::string_view baseUrl{ "https://soundcloud.com" };
			static constexpr std::string_view appVersion{ "1681464840" };

			Song constructDownloadInfo(const Song& songNew);

			Jsonifier::Vector<Song> collectSearchResults(const std::string& string);

			virtual Song collectFinalSong(const Song& songNew);

			std::string collectClientId();
		};

		class DiscordCoreAPI_Dll SoundCloudAPI : public SoundCloudRequestBuilder {
		  public:
			SoundCloudAPI(ConfigManager* configManagerNew, const Snowflake guildId);

			CoRoutine<void, false> downloadAndStreamAudio(const Song songNew,
				NewThreadAwaiter<void, false> threadHandle = NewThreadAwaiter<void, false>{}, uint64_t currentReconnectTries = 0);

			void weFailedToDownloadOrDecode(const Song& songNew, NewThreadAwaiter<void, false> threadHandle, uint64_t currentRetries);

			Song collectFinalSong(const Song& songNew) override;

			Jsonifier::Vector<Song> searchForSong(const std::string& searchQuery);

			bool areWeWorking();

		  protected:
			std::atomic_bool areWeWorkingBool{ false };
			Snowflake guildId{};
		};

		struct Transcoding {
			std::string preset{};
			std::string url{};
		};

		struct Media {
			Jsonifier::Vector<Transcoding> transcodings{};
		};

		struct SecondDownloadUrl {
			std::string url{};
		};

		struct RawSoundCloudSong {
			std::string trackAuthorization{};
			std::string description{};
			std::string artworkUrl{};
			std::string viewUrl{};
			std::string title{};
			uint64_t duration{};
			Media media{};
		};

		struct SoundCloudSearchResults {
			Jsonifier::Vector<RawSoundCloudSong> collection{};
		};

	};
}
