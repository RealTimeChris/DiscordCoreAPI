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
/// SoundCloudAPI.cpp - Source file for the SoundCloud api related stuff.
/// Aug 25, 2021
/// https://discordcoreapi.com
/// \file SoundCloudAPI.cpp

#include <discordcoreapi/SoundCloudAPI.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Utilities/AudioEncoder.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/Utilities/Demuxers.hpp>

namespace jsonifier {

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::DataClass> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::DataClass;
		static constexpr auto parseValue =
			createObject("artwork_url", &ValueType::artworkUrl, "description", &ValueType::description, "duration", &ValueType::duration, "media", &ValueType::media, "title",
				&ValueType::title, "track_authorization", &ValueType::trackAuthorization, "avatar_url", &ValueType::avatarUrl, "permalink_url", &ValueType::viewUrl);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::WelcomeElement> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::WelcomeElement;
		static constexpr auto parseValue = createObject("data", &ValueType::data, "hydratable", &ValueType::hydratable);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::FinalData> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::FinalData;
		static constexpr auto parseValue = createObject("data", &ValueType::data);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Media> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Media;
		static constexpr auto parseValue = createObject("transcodings", &ValueType::transcodings);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::SecondDownloadUrl> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::SecondDownloadUrl;
		static constexpr auto parseValue = createObject("url", &ValueType::url);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::Transcoding> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::Transcoding;
		static constexpr auto parseValue = createObject("preset", &ValueType::preset, "url", &ValueType::url);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::RawSoundCloudSong> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::RawSoundCloudSong;
		static constexpr auto parseValue = createObject("artwork_url", &ValueType::artworkUrl, "description", &ValueType::description, "duration", &ValueType::duration, "media",
			&ValueType::media, "title", &ValueType::title, "track_authorization", &ValueType::trackAuthorization, "permalink_url", &ValueType::viewUrl);
	};

	template<> struct core<DiscordCoreAPI::DiscordCoreInternal::SoundCloudSearchResults> {
		using ValueType					 = DiscordCoreAPI::DiscordCoreInternal::SoundCloudSearchResults;
		static constexpr auto parseValue = createObject("collection", &ValueType::collection);
	};
}

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		SoundCloudRequestBuilder::SoundCloudRequestBuilder(ConfigManager* configManagerNew) : HttpsClientCore{ jsonifier::string{ configManagerNew->getBotToken() } } {};

		jsonifier::string collectSongIdFromSearchQuery(jsonifier::string_view string) {
			if (string.find("soundcloud.com") != jsonifier::string::npos) {
				jsonifier::string returnString{};
				returnString = string.substr(string.find("soundcloud.com/") + jsonifier::string{ "soundcloud.com/" }.size());
				return returnString;
			} else {
				return static_cast<jsonifier::string>(string);
			}
		}

		Song SoundCloudRequestBuilder::collectSingleResult(jsonifier::string_view songQuery) {
			if (clientId == "") {
				SoundCloudRequestBuilder::collectClientId();
			}
			try {
				HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetSearchResults };
				dataPackage.baseUrl						  = "https://soundcloud.com/";
				dataPackage.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36";
				dataPackage.relativePath				  = collectSongIdFromSearchQuery(songQuery);
				dataPackage.workloadClass				  = HttpsWorkloadClass::Get;
				HttpsResponseData returnData			  = submitWorkloadAndGetResult(std::move(dataPackage));
				Song results{};
				auto findValue = returnData.responseData.find("<script>window.__sc_hydration = ");
				jsonifier::string addedString{ "{\"data\":" };
				if (findValue != jsonifier::string::npos) {
					returnData.responseData = returnData.responseData.substr(findValue + jsonifier::string{ "<script>window.__sc_hydration = " }.size(),
						returnData.responseData.find(";</script>") -
							(returnData.responseData.find("<script>window.__sc_hydration = ") + jsonifier::string{ "<script>window.__sc_hydration = " }.size()));
					addedString += returnData.responseData + "}";
				}
				FinalData resultsNew{};
				parser.parseJson<false, true>(resultsNew, addedString);
				jsonifier::string avatarUrl{};
				for (auto& value: resultsNew.data) {
					if (value.hydratable == "user") {
						avatarUrl = value.data.avatarUrl;
					}
				}
				for (auto& value: resultsNew.data) {
					if (value.hydratable == "sound") {
						if (value.data.title == "") {
							continue;
						}
						bool isItFound{};
						for (auto& valueNew: value.data.media.transcodings) {
							if (valueNew.preset == "opus_0_0") {
								isItFound				 = true;
								results.firstDownloadUrl = valueNew.url;
								results.songId			 = valueNew.url;
							}
						}
						if (isItFound) {
							jsonifier::string newString = value.data.title;
							if (newString.size() > 0) {
								if (newString.size() > 256) {
									newString = newString.substr(0, 256);
								}
								results.songTitle = utf8MakeValid(newString);
							}
							newString = value.data.description;
							if (newString.size() > 0) {
								if (newString.size() > 256) {
									newString = newString.substr(0, 256);
								}
								results.description = utf8MakeValid(newString);
								results.description += "...";
							}
							newString = value.data.artworkUrl;
							if (newString.size() > 0) {
								results.thumbnailUrl = newString;
							}
							results.viewUrl	 = value.data.viewUrl;
							results.duration = TimeStamp::convertMsToDurationString(value.data.duration);
							results.firstDownloadUrl += "?client_id=" + SoundCloudRequestBuilder::clientId + "&track_authorization=" + value.data.trackAuthorization;
							if (value.data.artworkUrl.find("-") != jsonifier::string::npos) {
								jsonifier::string newerString = value.data.artworkUrl.substr(0, value.data.artworkUrl.findLastOf("-") + 1);
								newerString += "t500x500.jpg";
								results.thumbnailUrl = newerString;
							} else if (avatarUrl.find("-") != jsonifier::string::npos) {
								jsonifier::string newerString = avatarUrl.substr(0, avatarUrl.findLastOf("-") + 1);
								newerString += "t500x500.jpg";
								results.thumbnailUrl = newerString;
							}
						}
					}
				}
				return results;
			} catch (const HttpsError& error) {
				MessagePrinter::printError<PrintMessageType::Https>("SoundCloudRequestBuilder::collectSearchResults() Error: " + jsonifier::string{ error.what() });
			}
			return {};
		}

		jsonifier::vector<Song> SoundCloudRequestBuilder::collectSearchResults(jsonifier::string_view songQuery) {
			if (clientId == "") {
				SoundCloudRequestBuilder::collectClientId();
			}
			try {
				HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetSearchResults };
				dataPackage.baseUrl						  = baseUrl02;
				dataPackage.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36";
				dataPackage.relativePath =
					"/search?q=" + urlEncode(collectSongIdFromSearchQuery(songQuery).data()) + "&facet=model&client_id=" + SoundCloudRequestBuilder::clientId;
				dataPackage.workloadClass	 = HttpsWorkloadClass::Get;
				HttpsResponseData returnData = submitWorkloadAndGetResult(std::move(dataPackage));
				jsonifier::vector<Song> results{};
				SoundCloudSearchResults resultsNew{};
				parser.parseJson<true, true>(resultsNew, returnData.responseData);
				for (auto& value: resultsNew.collection) {
					Song songNew{};
					if (value.title == "") {
						continue;
					}
					bool isItFound{};
					for (auto& valueNew: value.media.transcodings) {
						if (valueNew.preset == "opus_0_0") {
							isItFound				 = true;
							songNew.firstDownloadUrl = valueNew.url;
							songNew.songId			 = valueNew.url;
						}
					}
					if (isItFound) {
						jsonifier::string newString = value.title;
						if (newString.size() > 0) {
							if (newString.size() > 256) {
								newString = newString.substr(0, 256);
							}
							songNew.songTitle = utf8MakeValid(newString);
						}
						newString = value.description;
						if (newString.size() > 0) {
							if (newString.size() > 256) {
								newString = newString.substr(0, 256);
							}
							songNew.description = utf8MakeValid(newString);
							songNew.description += "...";
						}
						newString = value.artworkUrl;
						if (newString.size() > 0) {
							songNew.thumbnailUrl = newString;
						}
						songNew.viewUrl	 = value.viewUrl;
						songNew.duration = TimeStamp::convertMsToDurationString(value.duration);
						songNew.firstDownloadUrl += "?client_id=" + SoundCloudRequestBuilder::clientId + "&track_authorization=" + value.trackAuthorization;
						if (songNew.thumbnailUrl.find("-") != jsonifier::string::npos) {
							jsonifier::string newerString = songNew.thumbnailUrl.substr(0, songNew.thumbnailUrl.findLastOf("-") + 1);
							newerString += "t500x500.jpg";
							songNew.thumbnailUrl = newerString;
							results.emplace_back(songNew);
						}
					}
				}
				return results;
			} catch (const HttpsError& error) {
				MessagePrinter::printError<PrintMessageType::Https>("SoundCloudRequestBuilder::collectSearchResults() Error: " + jsonifier::string{ error.what() });
			}
			return {};
		}

		Song SoundCloudRequestBuilder::constructDownloadInfo(const Song& songNew) {
			try {
				HttpsWorkloadData dataPackage01{ HttpsWorkloadType::SoundCloudGetDownloadLinks };
				if (songNew.firstDownloadUrl.find(".com") != jsonifier::string::npos) {
					dataPackage01.baseUrl	   = songNew.firstDownloadUrl.substr(0, songNew.firstDownloadUrl.find(".com") + 4);
					dataPackage01.relativePath = songNew.firstDownloadUrl.substr(songNew.firstDownloadUrl.find(".com") + 4);
				} else {
					return {};
				}
				dataPackage01.workloadClass					= HttpsWorkloadClass::Get;
				dataPackage01.headersToInsert["Connection"] = "Keep-Alive";
				dataPackage01.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36";
				HttpsResponseData results					= submitWorkloadAndGetResult(std::move(dataPackage01));
				SecondDownloadUrl downloadUrl{};
				Song newerSong{ songNew };
				parser.parseJson<true, true>(downloadUrl, results.responseData);
				newerSong.secondDownloadUrl = downloadUrl.url;
				if (newerSong.secondDownloadUrl.find("/playlist") != jsonifier::string::npos) {
					HttpsWorkloadData dataPackage{ HttpsWorkloadType::SoundCloudGetDownloadLinks };
					dataPackage.baseUrl			 = newerSong.secondDownloadUrl;
					dataPackage.workloadClass	 = HttpsWorkloadClass::Get;
					HttpsResponseData resultsNew = submitWorkloadAndGetResult(std::move(dataPackage));
					jsonifier::string newString{ resultsNew.responseData };
					newerSong.finalDownloadUrls.clear();
					while (newString.find("#EXTINF:") != jsonifier::string::npos) {
						jsonifier::string newString01 = "#EXTINF:";
						jsonifier::string newString02 = newString.substr(newString.find("#EXTINF:") + newString01.size());
						auto commandFind		= newString02.find(",");
						jsonifier::string newString00 = newString02.substr(0, commandFind);
						jsonifier::string newString03 = newString02.substr(commandFind + 2, newString02.find("#EXTINF:") - (newString00.size() + 3));
						newString				= newString02.substr(commandFind);
						if (newString03.find("#EXT-X-ENDLIST") != jsonifier::string::npos) {
							newString03 = newString03.substr(0, newString03.find("#EXT-X-ENDLIST"));
						}
						jsonifier::string newString04 = newString03.substr(newString03.findFirstOf("1234567890"));
						uint64_t firstNumber01	= std::stoull(newString04.substr(0, newString04.find("/")).data());
						jsonifier::string newString05 = newString04.substr(newString04.find("/") + 1);
						uint64_t secondNumber		  = std::stoull(newString05.substr(0, newString05.find("/")).data());
						DownloadUrl downloadUrlNew{};
						downloadUrlNew.urlPath	   = newString03;
						downloadUrlNew.contentSize = secondNumber - firstNumber01;
						newerSong.finalDownloadUrls.emplace_back(downloadUrlNew);
					}
					for (uint64_t x = 0; x < newerSong.finalDownloadUrls.size(); ++x) {
						if (x == newerSong.finalDownloadUrls.size() - 1) {
							newerSong.finalDownloadUrls.at(x).urlPath = newerSong.finalDownloadUrls.at(x).urlPath.substr(0, newerSong.finalDownloadUrls.at(x).urlPath.size() - 1);
						}
					}
					for (auto& value: newerSong.finalDownloadUrls) {
						newerSong.contentLength += value.contentSize;
					}
				} else {
					HttpsWorkloadData dataPackage02{ HttpsWorkloadType::SoundCloudGetSearchResults };
					dataPackage02.baseUrl						= newerSong.secondDownloadUrl;
					dataPackage02.workloadClass					= HttpsWorkloadClass::Get;
					dataPackage02.headersToInsert["Connection"] = "Keep-Alive";
					dataPackage02.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36";
					auto headersNew								= submitWorkloadAndGetResult(std::move(dataPackage02));
					uint64_t valueBitRate{};
					uint64_t valueLength{};
					if (headersNew.responseHeaders.find("x-amz-meta-bitrate") != headersNew.responseHeaders.end()) {
						valueBitRate = std::stoull(headersNew.responseHeaders.find("x-amz-meta-bitrate")->second.data());
					}
					if (headersNew.responseHeaders.find("x-amz-meta-duration") != headersNew.responseHeaders.end()) {
						valueLength = std::stoull(headersNew.responseHeaders.find("x-amz-meta-duration")->second.data());
					}
					DownloadUrl downloadUrlNew{};
					downloadUrlNew.contentSize = static_cast<uint64_t>(((valueBitRate * valueLength) / 8) - 193);
					downloadUrlNew.urlPath	   = newerSong.secondDownloadUrl;
					newerSong.finalDownloadUrls.emplace_back(downloadUrlNew);
				}
				return newerSong;
			} catch (const HttpsError& error) {
				MessagePrinter::printError<PrintMessageType::Https>("SoundCloudRequestBuilder::constructDownloadInfo() Error: " + jsonifier::string{ error.what() });
			}
			return {};
		}

		Song SoundCloudRequestBuilder::collectFinalSong(const Song& songNew) {
			return constructDownloadInfo(songNew);
		}

		jsonifier::string SoundCloudRequestBuilder::collectClientId() {
			jsonifier::string clientIdNew{};
			try {
				HttpsWorkloadData dataPackage02{ HttpsWorkloadType::SoundCloudGetClientId };
				dataPackage02.baseUrl						= baseUrl;
				dataPackage02.relativePath					= "/search?q=testValue";
				dataPackage02.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36";
				dataPackage02.workloadClass					= HttpsWorkloadClass::Get;
				HttpsResponseData returnData				= submitWorkloadAndGetResult(std::move(dataPackage02));
				jsonifier::vector<jsonifier::string> assetPaths{};
				jsonifier::string newString01 = "crossorigin src=";
				jsonifier::string newerString{};
				newerString = returnData.responseData;
				if (newerString.find(newString01) != jsonifier::string::npos) {
					jsonifier::string newString	= newerString.substr(newerString.find(newString01) + newString01.size());
					jsonifier::string newString02 = newString.substr(1, newString.find(".js") + 2);
					assetPaths.emplace_back(newString02);
					while (newString.find("crossorigin src=") != jsonifier::string::npos) {
						jsonifier::string newString03 = newString.substr(1, newString.find(".js") + 2);
						newString				= newString.substr(newString.find("crossorigin src=") + newString01.size());
						assetPaths.emplace_back(newString03);
					}
					HttpsWorkloadData dataPackage03{ HttpsWorkloadType::SoundCloudGetClientId };
					dataPackage03.baseUrl		   = assetPaths[5];
					dataPackage03.workloadClass	   = HttpsWorkloadClass::Get;
					HttpsResponseData returnData02 = submitWorkloadAndGetResult(std::move(dataPackage03));
					jsonifier::string newerString02{};
					newerString02.insert(newerString02.begin(), returnData02.responseData.begin(), returnData02.responseData.end());

					jsonifier::string newString03 = newerString02.substr(newerString02.find("JSON.stringify({client_id:\"") + jsonifier::string_view{ "JSON.stringify({client_id:\"" }.size());

					if (newString03.find("\",nonce:e.nonce}))))") != jsonifier::string::npos) {
						clientIdNew = newString03.substr(0, newString03.find("\",nonce:e.nonce}))))"));
					}
					if (returnData02.responseCode != 200) {
						MessagePrinter::printError<PrintMessageType::Https>("SoundCloudAPI::collectClientID() Error: " +
							jsonifier::toString(returnData.responseCode.operator uint64_t()) + jsonifier::string{ newerString02.data() });
					}
				}
			} catch (const HttpsError& error) {
				MessagePrinter::printError<PrintMessageType::Https>("SoundCloudRequestBuilder::collectClientId() Error" + jsonifier::string{ error.what() });
			}
			return clientIdNew;
		}

		SoundCloudAPI::SoundCloudAPI(ConfigManager* configManagerNew, const Snowflake guildIdNew) : SoundCloudRequestBuilder{ configManagerNew } {
			guildId = static_cast<Snowflake>(guildIdNew);
			if (SoundCloudRequestBuilder::clientId == "") {
				SoundCloudRequestBuilder::clientId = collectClientId();
			}
		}

		void SoundCloudAPI::weFailedToDownloadOrDecode(const Song& songNew) {
			std::this_thread::sleep_for(1s);
			GuildMemberData guildMember{ GuildMembers::getCachedGuildMember({ .guildMemberId = songNew.addedByUserId, .guildId = guildId }) };
			DiscordCoreClient::getSongAPI(guildId).play(songNew, guildMember);
		}

		bool SoundCloudAPI::areWeWorking() {
			return areWeWorkingBool.load(std::memory_order_acquire);
		}

		CoRoutine<void, false> SoundCloudAPI::downloadAndStreamAudio(const Song songNew, NewThreadAwaiter<void, false> threadHandle, uint64_t currentReconnectTries) {
			try {
				areWeWorkingBool.store(true, std::memory_order_release);
				std::coroutine_handle<CoRoutine<void, false>::promise_type> coroHandle{};
				if (currentReconnectTries == 0) {
					threadHandle = NewThreadAwaitable<void, false>();
				}
				coroHandle = co_await threadHandle;
				uint64_t counter{};
				jsonifier::vector<HttpsWorkloadData> workloadVector{};
				for (uint64_t x = 0; x < songNew.finalDownloadUrls.size(); ++x) {
					HttpsWorkloadData dataPackage03{ HttpsWorkloadType::SoundCloudGetSearchResults };
					if (counter < songNew.finalDownloadUrls.size()) {
						jsonifier::string baseUrlNew	   = songNew.finalDownloadUrls.at(x).urlPath.substr(0, jsonifier::string_view{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
						jsonifier::string relativeUrl	   = songNew.finalDownloadUrls.at(x).urlPath.substr(jsonifier::string_view{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
						dataPackage03.baseUrl	   = baseUrlNew;
						dataPackage03.relativePath = relativeUrl;
					}
					dataPackage03.workloadClass = HttpsWorkloadClass::Get;
					workloadVector.emplace_back(std::move(dataPackage03));
				}
				jsonifier::vector<jsonifier::string> buffer{};
				OggDemuxer demuxer{};
				for (uint64_t x = 0; x < songNew.finalDownloadUrls.size(); ++x) {
					HttpsResponseData result{ submitWorkloadAndGetResult(std::move(workloadVector.at(x))) };
					if (result.responseCode != 200) {
						areWeWorkingBool.store(false, std::memory_order_release);
						weFailedToDownloadOrDecode(songNew);
						co_return;
					}

					if (result.responseData.size() > 0) {
						buffer.emplace_back(std::move(result.responseData));
						demuxer.writeData({ buffer.back().data(), buffer.back().size() });
						demuxer.proceedDemuxing();
					}
					if (coroHandle.promise().stopRequested()) {
						areWeWorkingBool.store(false, std::memory_order_release);
						co_return;
					}
					bool didWeReceive{ true };
					do {
						AudioFrameData frameData{};
						didWeReceive = demuxer.collectFrame(frameData);
						if (coroHandle.promise().stopRequested()) {
							areWeWorkingBool.store(false, std::memory_order_release);
							co_return;
						}
						if (frameData.currentSize != 0) {
							frameData.guildMemberId = songNew.addedByUserId.operator const uint64_t&();
							DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
						}
					} while (didWeReceive);
					if (coroHandle.promise().stopRequested()) {
						areWeWorkingBool.store(false, std::memory_order_release);
						co_return;
					}
					std::this_thread::sleep_for(1ms);
				}
				areWeWorkingBool.store(false, std::memory_order_release);
				DiscordCoreClient::getVoiceConnection(guildId).skip(false);
				AudioFrameData frameData{};
				frameData.guildMemberId = songNew.addedByUserId.operator const uint64_t&();
				DiscordCoreClient::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
				co_return;
			} catch (const HttpsError& error) {
				MessagePrinter::printError<PrintMessageType::Https>("SoundCloudRequestBuilder::downloadAndStreamAudio() Error: " + jsonifier::string{ error.what() });
				areWeWorkingBool.store(false, std::memory_order_release);
				weFailedToDownloadOrDecode(songNew);
			}
			co_return;
		}

		Song SoundCloudAPI::collectSingleResult(jsonifier::string_view searchQuery) {
			return SoundCloudRequestBuilder::collectSingleResult(searchQuery);
		}

		jsonifier::vector<Song> SoundCloudAPI::searchForSong(jsonifier::string_view searchQuery) {
			return collectSearchResults(searchQuery);
		}

		Song SoundCloudAPI::collectFinalSong(const Song& songNew) {
			return SoundCloudRequestBuilder::collectFinalSong(songNew);
		}

		jsonifier::string SoundCloudRequestBuilder::clientId{};
	};
}