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

	template<> struct core<discord_core_api::discord_core_internal::data_class> {
		using value_type = discord_core_api::discord_core_internal::data_class;
		static constexpr auto parseValue =
			createValue("artwork_url", &value_type::artworkUrl, "description", &value_type::description, "duration", &value_type::duration, "media", &value_type::mediaVal, "title",
				&value_type::title, "track_authorization", &value_type::trackAuthorization, "avatar_url", &value_type::avatarUrl, "permalink_url", &value_type::viewUrl);
	};

	template<> struct core<discord_core_api::discord_core_internal::welcome_element> {
		using value_type				 = discord_core_api::discord_core_internal::welcome_element;
		static constexpr auto parseValue = createValue("data", &value_type::data, "hydratable", &value_type::hydratable, "tracks", &value_type::data);
	};

	template<> struct core<discord_core_api::discord_core_internal::welcome> {
		using value_type				 = discord_core_api::discord_core_internal::welcome;
		static constexpr auto parseValue = createValue(&value_type::data);
	};

	template<> struct core<discord_core_api::discord_core_internal::media> {
		using value_type				 = discord_core_api::discord_core_internal::media;
		static constexpr auto parseValue = createValue("transcodings", &value_type::transcodings);
	};

	template<> struct core<discord_core_api::discord_core_internal::second_download_url> {
		using value_type				 = discord_core_api::discord_core_internal::second_download_url;
		static constexpr auto parseValue = createValue("url", &value_type::url);
	};

	template<> struct core<discord_core_api::discord_core_internal::transcoding> {
		using value_type				 = discord_core_api::discord_core_internal::transcoding;
		static constexpr auto parseValue = createValue("preset", &value_type::preset, "url", &value_type::url);
	};

	template<> struct core<discord_core_api::discord_core_internal::raw_sound_cloud_song> {
		using value_type				 = discord_core_api::discord_core_internal::raw_sound_cloud_song;
		static constexpr auto parseValue = createValue("artwork_url", &value_type::artworkUrl, "description", &value_type::description, "duration", &value_type::duration, "media",
			&value_type::mediaVal, "title", &value_type::title, "track_authorization", &value_type::trackAuthorization, "permalink_url", &value_type::viewUrl);
	};

	template<> struct core<discord_core_api::discord_core_internal::sound_cloud_search_results> {
		using value_type				 = discord_core_api::discord_core_internal::sound_cloud_search_results;
		static constexpr auto parseValue = createValue("collection", &value_type::collection);
	};
}

namespace discord_core_api {

	namespace discord_core_internal {

		sound_cloud_request_builder::sound_cloud_request_builder(config_manager* configManagerNew) : https_client_core{ jsonifier::string{ configManagerNew->getBotToken() } } {};

		inline search_type collectSearchType(jsonifier::string_view string, jsonifier::string& stringNew) {
			if (string.find("soundcloud.com") != jsonifier::string::npos && string.find("/sets/") == jsonifier::string::npos) {
				stringNew = string.substr(string.find("soundcloud.com/") + jsonifier::string{ "soundcloud.com/" }.size());
				return search_type::single_song_with_id;
			} else if (string.find("/sets/") != jsonifier::string::npos) {
				stringNew = string.substr(string.find("soundcloud.com/") + jsonifier::string{ "soundcloud.com/" }.size());
				return search_type::playlist;
			} else {
				stringNew = string;
				return search_type::single_song_without_id;
			}
		}

		jsonifier::vector<song> sound_cloud_request_builder::collectPlaylist(jsonifier::string_view songQuery) {
			if (clientId == "") {
				sound_cloud_request_builder::collectClientId();
			}
			try {
				https_workload_data dataPackage{ https_workload_type::SoundCloud_Get_Search_Results };
				dataPackage.baseUrl						  = "https://soundcloud.com/";
				dataPackage.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36";
				dataPackage.relativePath				  = songQuery;
				dataPackage.workloadClass				  = https_workload_class::Get;
				https_response_data returnData			  = submitWorkloadAndGetResult(std::move(dataPackage));
				jsonifier::vector<song> resultsFinal{};
				auto findValue = returnData.responseData.find("window.__sc_hydration = ");
				if (findValue != jsonifier::string::npos) {
					returnData.responseData = returnData.responseData.substr(findValue + jsonifier::string{ "window.__sc_hydration = " }.size(),
						returnData.responseData.find("</script>") -
							(returnData.responseData.find("window.__sc_hydration = ") + jsonifier::string{ "window.__sc_hydration = " }.size()));
				}
				welcome resultsNew{};
				parser.parseJson(resultsNew, returnData.responseData);
				jsonifier::string avatarUrl{};
				jsonifier::string collectionString{ "tracks?ids=" };
				for (auto& value: resultsNew.data) {
					if (value.data.getType() == jsonifier::json_type::Object) {
						auto newObject = value.data.operator jsonifier::raw_json_data::object_type();
						avatarUrl	   = newObject["avatar_url"].operator jsonifier::string();
						if (value.hydratable == "playlist") {
							auto newerObject = value.data.operator jsonifier::raw_json_data::object_type();
							for (auto& [key, valueNew]: newerObject) {
								if (key == "tracks") {
									auto newArray = valueNew.operator jsonifier::raw_json_data::array_type();
									uint32_t currentIndex{};
									auto arraySize = newArray.size();
									for (auto& newValue: newArray) {
										newObject = newValue.operator jsonifier::raw_json_data::object_type();
										collectionString += jsonifier::toString(newValue.operator jsonifier::raw_json_data::object_type()["id"].operator uint64_t());
										if (currentIndex < arraySize - 1) {
											collectionString += "%2C";
										}
										++currentIndex;
									}
									collectionString += "&client_id=" + clientId + "&%5Bobject%20Object%5D=&app_version=" + appVersion + "&app_locale=en";
								}
							}
						}
					}
				}
				https_workload_data dataPackage02{ https_workload_type::SoundCloud_Get_Search_Results };
				dataPackage02.baseUrl						= "https://api-v2.soundcloud.com/";
				dataPackage02.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36";
				dataPackage02.relativePath					= collectionString;
				dataPackage02.workloadClass					= https_workload_class::Get;
				returnData									= submitWorkloadAndGetResult(std::move(dataPackage02));
				jsonifier::vector<jsonifier::raw_json_data> resultsNewer{};
				parser.parseJson(resultsNewer, returnData.responseData);
				for (auto& value: resultsNewer) {
					song results{};
					if (value.getType() == jsonifier::json_type::Object) {
						auto newObject = value.operator jsonifier::raw_json_data::object_type();
						avatarUrl	   = newObject["avatar_url"].operator jsonifier::string();
						if (newObject["title"].operator jsonifier::string() == "") {
							continue;
						}
						bool isItFound{};
						for (auto& valueNew: newObject["media"].operator jsonifier::raw_json_data::object_type()["transcodings"].operator jsonifier::raw_json_data::array_type()) {
							if (valueNew.operator jsonifier::raw_json_data::object_type()["preset"].operator jsonifier::string() == "opus_0_0") {
								isItFound				 = true;
								results.firstDownloadUrl = valueNew.operator jsonifier::raw_json_data::object_type()["url"].operator jsonifier::string();
								results.songId			 = valueNew.operator jsonifier::raw_json_data::object_type()["url"].operator jsonifier::string();
							}
						}
						if (isItFound) {
							jsonifier::string newString = newObject["title"].operator jsonifier::string();
							if (newString.size() > 0) {
								if (newString.size() > 256) {
									newString = newString.substr(0, 256);
								}
								results.songTitle = utf8MakeValid(newString);
							}
							newString = newObject["description"].operator jsonifier::string();
							if (newString.size() > 0) {
								if (newString.size() > 256) {
									newString = newString.substr(0, 256);
								}
								results.description = utf8MakeValid(newString);
								results.description += "...";
							}
							newString = newObject["artwork_url"].operator jsonifier::string();
							if (newString.size() > 0) {
								results.thumbnailUrl = utf8MakeValid(newString);
							}
							results.viewUrl	 = newObject["permalink_url"].operator jsonifier::string();
							results.duration = time_stamp::convertMsToDurationString(static_cast<uint64_t>(newObject["duration"].operator uint64_t()));
							results.firstDownloadUrl +=
								"?client_id=" + sound_cloud_request_builder::clientId + "&track_authorization=" + newObject["track_authorization"].operator jsonifier::string();
							if (newObject["artwork_url"].operator jsonifier::string().find("-") != jsonifier::string::npos) {
								jsonifier::string newerString =
									newObject["artwork_url"].operator jsonifier::string().substr(0, newObject["artwork_url"].operator jsonifier::string().findLastOf("-") + 1);
								newerString += "t500x500.jpg";
								results.thumbnailUrl = newerString;
							} else if (avatarUrl.find("-") != jsonifier::string::npos) {
								jsonifier::string newerString = avatarUrl.substr(0, avatarUrl.findLastOf("-") + 1);
								newerString += "t500x500.jpg";
								results.thumbnailUrl = newerString;
							}
							results.type = song_type::SoundCloud;
							resultsFinal.emplace_back(results);
						}
					}
				}
				return resultsFinal;
			} catch (const https_error& error) {
				message_printer::printError<print_message_type::https>("sound_cloud_request_builder::collectSearchResults() Error: " + jsonifier::string{ error.what() });
			}
			return {};
		}

		song sound_cloud_request_builder::collectSingleResult(jsonifier::string_view songQuery) {
			if (clientId == "") {
				sound_cloud_request_builder::collectClientId();
			}
			try {
				https_workload_data dataPackage{ https_workload_type::SoundCloud_Get_Search_Results };
				dataPackage.baseUrl						  = "https://soundcloud.com/";
				dataPackage.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36";
				dataPackage.relativePath				  = songQuery;
				dataPackage.workloadClass				  = https_workload_class::Get;
				https_response_data returnData			  = submitWorkloadAndGetResult(std::move(dataPackage));
				song results{};
				auto findValue = returnData.responseData.find("window.__sc_hydration = ");
				if (findValue != jsonifier::string::npos) {
					returnData.responseData = returnData.responseData.substr(findValue + jsonifier::string{ "window.__sc_hydration = " }.size(),
						returnData.responseData.find("</script>") -
							(returnData.responseData.find("window.__sc_hydration = ") + jsonifier::string{ "window.__sc_hydration = " }.size()));
				}
				welcome resultsNew{};
				parser.parseJson(resultsNew, returnData.responseData);
				jsonifier::string avatarUrl{};

				for (auto& value: resultsNew.data) {
					if (value.data.getType() == jsonifier::json_type::Object) {
						auto newObject = value.data.operator jsonifier::raw_json_data::object_type();
						avatarUrl	   = newObject["avatar_url"].operator jsonifier::string();
						if (value.hydratable == "sound") {
							if (newObject["title"].operator jsonifier::string() == "") {
								continue;
							}
							bool isItFound{};
							for (auto& valueNew:
								newObject["media"].operator jsonifier::raw_json_data::object_type()["transcodings"].operator jsonifier::raw_json_data::array_type()) {
								if (valueNew.operator jsonifier::raw_json_data::object_type()["preset"].operator jsonifier::string() == "opus_0_0") {
									isItFound				 = true;
									results.firstDownloadUrl = valueNew.operator jsonifier::raw_json_data::object_type()["url"].operator jsonifier::string();
									results.songId			 = valueNew.operator jsonifier::raw_json_data::object_type()["url"].operator jsonifier::string();
								}
							}
							if (isItFound) {
								jsonifier::string newString = newObject["title"].operator jsonifier::string();
								if (newString.size() > 0) {
									if (newString.size() > 256) {
										newString = newString.substr(0, 256);
									}
									results.songTitle = utf8MakeValid(newString);
								}
								newString = newObject["description"].operator jsonifier::string();
								if (newString.size() > 0) {
									if (newString.size() > 256) {
										newString = newString.substr(0, 256);
									}
									results.description = utf8MakeValid(newString);
									results.description += "...";
								}
								newString = newObject["artwork_url"].operator jsonifier::string();
								if (newString.size() > 0) {
									results.thumbnailUrl = utf8MakeValid(newString);
								}
								results.viewUrl	 = newObject["permalink_url"].operator jsonifier::string();
								results.duration = time_stamp::convertMsToDurationString(static_cast<uint64_t>(newObject["duration"].operator uint64_t()));
								results.firstDownloadUrl +=
									"?client_id=" + sound_cloud_request_builder::clientId + "&track_authorization=" + newObject["track_authorization"].operator jsonifier::string();
								if (newObject["artwork_url"].operator jsonifier::string().find("-") != jsonifier::string::npos) {
									jsonifier::string newerString =
										newObject["artwork_url"].operator jsonifier::string().substr(0, newObject["artwork_url"].operator jsonifier::string().findLastOf("-") + 1);
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
				}
				results.type = song_type::SoundCloud;
				return results;
			} catch (const https_error& error) {
				message_printer::printError<print_message_type::https>("sound_cloud_request_builder::collectSearchResults() Error: " + jsonifier::string{ error.what() });
			}
			return {};
		}

		jsonifier::vector<song> sound_cloud_request_builder::collectSearchResults(jsonifier::string_view songQuery, uint64_t limit) {
			if (clientId == "") {
				sound_cloud_request_builder::collectClientId();
			}
			try {
				jsonifier::string newString{};
				jsonifier::vector<song> results{};
				auto searchType = collectSearchType(songQuery, newString);
				if (searchType == search_type::single_song_with_id) {
					auto result = collectSingleResult(newString);
					if (result.songId != "") {
						results.emplace_back(result);
					}
					return results;
				} else if (searchType == search_type::playlist) {
					return collectPlaylist(newString);
				}
				https_workload_data dataPackage{ https_workload_type::SoundCloud_Get_Search_Results };
				dataPackage.baseUrl						  = baseUrl02;
				dataPackage.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36";
				dataPackage.relativePath				  = "/search?q=" + urlEncode(songQuery) + "&facet=model&client_id=" + sound_cloud_request_builder::clientId;
				dataPackage.workloadClass				  = https_workload_class::Get;
				https_response_data returnData			  = submitWorkloadAndGetResult(std::move(dataPackage));
				sound_cloud_search_results resultsNew{};
				parser.parseJson(resultsNew, returnData.responseData);
				for (auto& value: resultsNew.collection) {
					song songNew{};
					if (value.title == "") {
						continue;
					}
					bool isItFound{};
					for (auto& valueNew: value.mediaVal.transcodings) {
						if (valueNew.preset == "opus_0_0") {
							isItFound				 = true;
							songNew.firstDownloadUrl = valueNew.url;
							songNew.songId			 = valueNew.url;
						}
					}
					if (isItFound) {
						newString = value.title;
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
						songNew.type	 = song_type::SoundCloud;
						songNew.viewUrl	 = value.viewUrl;
						songNew.duration = time_stamp::convertMsToDurationString(value.duration);
						songNew.firstDownloadUrl += "?client_id=" + sound_cloud_request_builder::clientId + "&track_authorization=" + value.trackAuthorization;
						if (songNew.thumbnailUrl.find("-") != jsonifier::string::npos) {
							jsonifier::string newerString = songNew.thumbnailUrl.substr(0, songNew.thumbnailUrl.findLastOf("-") + 1);
							newerString += "t500x500.jpg";
							songNew.thumbnailUrl = newerString;
						}
						results.emplace_back(songNew);
						if (results.size() >= limit) {
							break;
						}
					}
				}
				return results;
			} catch (const https_error& error) {
				message_printer::printError<print_message_type::https>("sound_cloud_request_builder::collectSearchResults() Error: " + jsonifier::string{ error.what() });
			}
			return {};
		}

		song sound_cloud_request_builder::constructDownloadInfo(const song& songNew, uint64_t recursionDepth) {
			try {
				https_workload_data dataPackage01{ https_workload_type::SoundCloud_Get_Download_Links };
				if (songNew.firstDownloadUrl.find(".com") != jsonifier::string::npos) {
					dataPackage01.baseUrl	   = songNew.firstDownloadUrl.substr(0, songNew.firstDownloadUrl.find(".com") + 4);
					dataPackage01.relativePath = songNew.firstDownloadUrl.substr(songNew.firstDownloadUrl.find(".com") + 4);
				} else {
					return {};
				}
				dataPackage01.workloadClass					= https_workload_class::Get;
				dataPackage01.headersToInsert["Connection"] = "Keep-Alive";
				dataPackage01.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36";
				https_response_data results					= submitWorkloadAndGetResult(std::move(dataPackage01));
				second_download_url downloadUrl{};
				song newerSong{ songNew };
				parser.parseJson(downloadUrl, results.responseData);
				newerSong.secondDownloadUrl = downloadUrl.url;
				if (newerSong.secondDownloadUrl.find("/playlist") != jsonifier::string::npos) {
					https_workload_data dataPackage{ https_workload_type::SoundCloud_Get_Download_Links };
					dataPackage.baseUrl			   = newerSong.secondDownloadUrl;
					dataPackage.workloadClass	   = https_workload_class::Get;
					https_response_data resultsNew = submitWorkloadAndGetResult(std::move(dataPackage));
					jsonifier::string newString{ resultsNew.responseData };
					newerSong.finalDownloadUrls.clear();
					while (newString.find("#EXTINF:") != jsonifier::string::npos) {
						jsonifier::string newString01 = "#EXTINF:";
						jsonifier::string newString02 = newString.substr(newString.find("#EXTINF:") + newString01.size());
						auto commandFind			  = newString02.find(",");
						jsonifier::string newString00 = newString02.substr(0, commandFind);
						jsonifier::string newString03 = newString02.substr(commandFind + 2, newString02.find("#EXTINF:") - (newString00.size() + 3));
						newString					  = newString02.substr(commandFind);
						if (newString03.find("#EXT-X-ENDLIST") != jsonifier::string::npos) {
							newString03 = newString03.substr(0, newString03.find("#EXT-X-ENDLIST"));
						}
						jsonifier::string newString04 = newString03.substr(newString03.findFirstOf("1234567890"));
						uint64_t firstNumber01		  = jsonifier::strToUint64(newString04.substr(0, newString04.find("/")).data());
						jsonifier::string newString05 = newString04.substr(newString04.find("/") + 1);
						uint64_t secondNumber		  = jsonifier::strToUint64(newString05.substr(0, newString05.find("/")).data());
						download_url downloadUrlNew{};
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
					https_workload_data dataPackage02{ https_workload_type::SoundCloud_Get_Search_Results };
					dataPackage02.baseUrl						= newerSong.secondDownloadUrl;
					dataPackage02.workloadClass					= https_workload_class::Get;
					dataPackage02.headersToInsert["Connection"] = "Keep-Alive";
					dataPackage02.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36";
					auto headersNew								= submitWorkloadAndGetResult(std::move(dataPackage02));
					uint64_t valueBitRate{};
					uint64_t valueLength{};
					if (headersNew.responseHeaders.find("x-amz-meta-bitrate") != headersNew.responseHeaders.end()) {
						valueBitRate = jsonifier::strToUint64(headersNew.responseHeaders.find("x-amz-meta-bitrate")->second.data());
					}
					if (headersNew.responseHeaders.find("x-amz-meta-duration") != headersNew.responseHeaders.end()) {
						valueLength = jsonifier::strToUint64(headersNew.responseHeaders.find("x-amz-meta-duration")->second.data());
					}
					download_url downloadUrlNew{};
					downloadUrlNew.contentSize = static_cast<uint64_t>(((valueBitRate * valueLength) / 8) - 193);
					downloadUrlNew.urlPath	   = newerSong.secondDownloadUrl;
					newerSong.finalDownloadUrls.emplace_back(downloadUrlNew);
				}
				return newerSong;
			} catch (const https_error& error) {
				if (recursionDepth <= 10) {
					++recursionDepth;
					return constructDownloadInfo(songNew, recursionDepth);
				} else {
					message_printer::printError<print_message_type::https>("YouTubeRequestBuilder::constructDownloadInfo() Error: " + jsonifier::string{ error.what() });
					return {};
				}
			}
		}

		song sound_cloud_request_builder::collectFinalSong(const song& songNew) {
			return constructDownloadInfo(songNew, 0);
		}

		jsonifier::string sound_cloud_request_builder::collectClientId() {
			jsonifier::string clientIdNew{};
			try {
				https_workload_data dataPackage02{ https_workload_type::SoundCloud_Get_Client_Id };
				dataPackage02.baseUrl						= baseUrl;
				dataPackage02.relativePath					= "/search?q=testValue";
				dataPackage02.headersToInsert["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36";
				dataPackage02.workloadClass					= https_workload_class::Get;
				https_response_data returnData				= submitWorkloadAndGetResult(std::move(dataPackage02));
				jsonifier::vector<jsonifier::string> assetPaths{};
				jsonifier::string newString01 = "crossorigin src=";
				jsonifier::string newerString{};
				newerString = returnData.responseData;
				if (newerString.find(newString01) != jsonifier::string::npos) {
					jsonifier::string newString	  = newerString.substr(newerString.find(newString01) + newString01.size());
					jsonifier::string newString02 = newString.substr(1, newString.find(".js") + 2);
					assetPaths.emplace_back(newString02);
					while (newString.find("crossorigin src=") != jsonifier::string::npos) {
						jsonifier::string newString03 = newString.substr(1, newString.find(".js") + 2);
						newString					  = newString.substr(newString.find("crossorigin src=") + newString01.size());
						assetPaths.emplace_back(newString03);
					}
					https_workload_data dataPackage03{ https_workload_type::SoundCloud_Get_Client_Id };
					dataPackage03.baseUrl			 = assetPaths[5];
					dataPackage03.workloadClass		 = https_workload_class::Get;
					https_response_data returnData02 = submitWorkloadAndGetResult(std::move(dataPackage03));
					jsonifier::string newerString02{};
					newerString02.insert(newerString02.begin(), returnData02.responseData.begin(), returnData02.responseData.end());

					jsonifier::string newString03 =
						newerString02.substr(newerString02.find("JSON.stringify({client_id:\"") + jsonifier::string_view{ "JSON.stringify({client_id:\"" }.size());

					if (newString03.find("\",nonce:e.nonce}))))") != jsonifier::string::npos) {
						clientIdNew = newString03.substr(0, newString03.find("\",nonce:e.nonce}))))"));
					}
					if (returnData02.responseCode != 200) {
						message_printer::printError<print_message_type::https>("sound_cloud_api::collectClientID() Error: " +
							jsonifier::toString(returnData.responseCode.operator uint64_t()) + jsonifier::string{ newerString02.data() });
					}
				}
			} catch (const https_error& error) {
				message_printer::printError<print_message_type::https>("sound_cloud_request_builder::collectClientId() Error" + jsonifier::string{ error.what() });
			}
			return clientIdNew;
		}

		sound_cloud_api::sound_cloud_api(config_manager* configManagerNew, const snowflake guildIdNew) : sound_cloud_request_builder{ configManagerNew } {
			guildId = static_cast<snowflake>(guildIdNew);
			if (sound_cloud_request_builder::clientId == "") {
				sound_cloud_request_builder::clientId = collectClientId();
			}
		}

		void sound_cloud_api::weFailedToDownloadOrDecode(const song& songNew, std::coroutine_handle<co_routine<void, false>::promise_type> threadHandle, uint64_t recursionDepth) {
			std::this_thread::sleep_for(1s);
			if (recursionDepth < 10) {
				++recursionDepth;
				song songNewer = constructDownloadInfo(songNew, 0);
				downloadAndStreamAudio(songNewer, threadHandle, recursionDepth);
			} else {
				discord_core_client::getVoiceConnection(guildId).skip(true);
			}
		}

		bool sound_cloud_api::areWeWorking() {
			return areWeWorkingBool.load(std::memory_order_acquire);
		}

		co_routine<void, false> sound_cloud_api::downloadAndStreamAudio(const song songNew, std::coroutine_handle<co_routine<void, false>::promise_type> threadHandle,
			uint64_t currentReconnectTries) {
			try {
				areWeWorkingBool.store(true, std::memory_order_release);
				if (currentReconnectTries == 0) {
					threadHandle = co_await newThreadAwaitable<void, false>();
				}
				uint64_t counter{};
				jsonifier::vector<https_workload_data> workloadVector{};
				for (uint64_t x = 0; x < songNew.finalDownloadUrls.size(); ++x) {
					https_workload_data dataPackage03{ https_workload_type::SoundCloud_Get_Search_Results };
					if (counter < songNew.finalDownloadUrls.size()) {
						jsonifier::string baseUrlNew =
							songNew.finalDownloadUrls.at(x).urlPath.substr(0, jsonifier::string_view{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
						jsonifier::string relativeUrl =
							songNew.finalDownloadUrls.at(x).urlPath.substr(jsonifier::string_view{ "https://cf-hls-opus-media.sndcdn.com/media/" }.size());
						dataPackage03.baseUrl	   = baseUrlNew;
						dataPackage03.relativePath = relativeUrl;
					}
					dataPackage03.workloadClass = https_workload_class::Get;
					workloadVector.emplace_back(std::move(dataPackage03));
				}
				jsonifier::vector<jsonifier::string> buffer{};
				ogg_demuxer demuxer{};
				for (uint64_t x = 0; x < songNew.finalDownloadUrls.size(); ++x) {
					https_response_data result{ submitWorkloadAndGetResult(std::move(workloadVector.at(x))) };
					if (result.responseCode != 200) {
						weFailedToDownloadOrDecode(songNew, threadHandle, currentReconnectTries);
						areWeWorkingBool.store(false, std::memory_order_release);
						co_return;
					}

					if (result.responseData.size() > 0) {
						buffer.emplace_back(std::move(result.responseData));
						demuxer.writeData({ buffer.back().data(), buffer.back().size() });
						demuxer.proceedDemuxing();
					}
					if (threadHandle.promise().stopRequested()) {
						areWeWorkingBool.store(false, std::memory_order_release);
						co_return;
					}
					bool didWeReceive{ true };
					do {
						audio_frame_data frameData{};
						didWeReceive = demuxer.collectFrame(frameData);
						if (threadHandle.promise().stopRequested()) {
							areWeWorkingBool.store(false, std::memory_order_release);
							co_return;
						}
						if (frameData.currentSize != 0) {
							discord_core_client::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
						}
					} while (didWeReceive && !threadHandle.promise().stopRequested());
					if (threadHandle.promise().stopRequested()) {
						areWeWorkingBool.store(false, std::memory_order_release);
						co_return;
					}
					std::this_thread::sleep_for(1ms);
				}
				areWeWorkingBool.store(false, std::memory_order_release);
				discord_core_client::getVoiceConnection(guildId).skip(false);
				audio_frame_data frameData{};
				discord_core_client::getSongAPI(guildId).audioDataBuffer.send(std::move(frameData));
				co_return;
			} catch (const https_error& error) {
				message_printer::printError<print_message_type::https>("sound_cloud_request_builder::downloadAndStreamAudio() Error: " + jsonifier::string{ error.what() });
				weFailedToDownloadOrDecode(songNew, threadHandle, currentReconnectTries);
				areWeWorkingBool.store(false, std::memory_order_release);
			}
			co_return;
		}

		jsonifier::vector<song> sound_cloud_api::searchForSong(jsonifier::string_view searchQuery, uint64_t limit) {
			return collectSearchResults(searchQuery, limit);
		}

		jsonifier::string sound_cloud_request_builder::clientId{};
	};
}