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
			static jsonifier::string clientId;
			inline static constexpr jsonifier::string_view baseUrl02{ "https://api-v2.soundcloud.com" };
			inline static constexpr jsonifier::string_view baseUrl{ "https://soundcloud.com" };
			inline static constexpr jsonifier::string_view appVersion{ "1681464840" };

			Song constructDownloadInfo(const Song& songNew);

			jsonifier::vector<Song> collectSearchResults(jsonifier::string_view string, int32_t limit = 20);

			virtual Song collectFinalSong(const Song& songNew);

			Song collectSingleResult(jsonifier::string_view string);

			jsonifier::string collectClientId();

			virtual ~SoundCloudRequestBuilder() = default;
		};

		class DiscordCoreAPI_Dll SoundCloudAPI : public SoundCloudRequestBuilder {
		  public:
			SoundCloudAPI(ConfigManager* configManagerNew, const Snowflake guildId);

			CoRoutine<void, false> downloadAndStreamAudio(const Song songNew, NewThreadAwaiter<void, false> threadHandle = NewThreadAwaiter<void, false>{},
				uint64_t currentReconnectTries = 0);
						
			jsonifier::vector<Song> searchForSong(jsonifier::string_view searchQuery, int32_t limit);

			void weFailedToDownloadOrDecode(const Song& songNew);

			Song collectFinalSong(const Song& songNew) override;

			Song collectSingleResult(jsonifier::string_view string);

			bool areWeWorking();

		  protected:
			std::atomic_bool areWeWorkingBool{ false };
			Snowflake guildId{};
		};

		struct Transcoding {
			jsonifier::string preset{};
			jsonifier::string url{};
		};

		struct Media {
			jsonifier::vector<Transcoding> transcodings{};
		};

		struct SecondDownloadUrl {
			jsonifier::string url{};
		};

		struct RawSoundCloudSong {
			jsonifier::string trackAuthorization{};
			jsonifier::string description{};
			jsonifier::string artworkUrl{};
			jsonifier::string viewUrl{};
			jsonifier::string title{};
			uint64_t duration{};
			Media media{};
		};

		struct SoundCloudSearchResults {
			jsonifier::vector<RawSoundCloudSong> collection{};
		};

		struct Visual {
			jsonifier::string visualUrl{};
			int64_t entryTime{};
			jsonifier::string urn{};
		};

		struct DataClass {
			DiscordCoreAPI::DiscordCoreInternal::Media media{};
			jsonifier::string trackAuthorization{};
			jsonifier::string description{};
			jsonifier::string artworkUrl{};
			jsonifier::string avatarUrl{};
			jsonifier::string viewUrl{};
			jsonifier::string title{};
			int64_t duration{};
			jsonifier::string url{};
			int64_t id{};
		};

		struct WelcomeElement {
			jsonifier::string hydratable{};
			DataClass data{};
		};

		struct FinalData {
			jsonifier::vector<WelcomeElement> data{};
		};
	};
}
