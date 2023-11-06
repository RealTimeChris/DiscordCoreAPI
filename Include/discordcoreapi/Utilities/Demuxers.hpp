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
/// Demuxers.hpp - Header file for the Demuxer classes.
/// Jun 8, 2023
/// https://discordcoreapi.com
/// \file Demuxers.hpp
#pragma once

#include <discordcoreapi/Utilities.hpp>
#include <opus/opus.h>
#include <fstream>

namespace discord_core_api {

	namespace discord_core_internal {

		/**
		* \addtogroup discord_core_internal
		* @{
		*/

		static constexpr uint32_t segmentId{ 0x18538067 };
		static constexpr uint8_t simpleBlockId{ 0xA3 };
		static constexpr uint8_t opusTrackId{ 0x81 };

		static constexpr uint8_t ffLog2Tab[]{ 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
			5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 };

		/// @brief A class for demuxing Matroska-contained audio data.
		class matroska_demuxer {
		  public:
			/// @brief Constructor for matroska_demuxer.
			inline matroska_demuxer() = default;

			/// @brief Writes data to the Matroska demuxer.
			/// @param dataNew The data to be written.
			inline void writeData(jsonifier::string_view_base<uint8_t> dataNew) {
				data = dataNew;
			}

			/// @brief Collects the next frame from the demuxer.
			/// @param frameNew The reference to store the collected frame.
			/// @return True if a frame was collected, false otherwise.
			inline bool collectFrame(audio_frame_data& frameNew) {
				if (frames.size() > 0) {
					frameNew = std::move(frames.at(0));
					frames.erase(frames.begin());
					return true;
				} else {
					return false;
				}
			}

			/// @brief Proceed with the demuxing process.
			inline void proceedDemuxing() {
				if (!doWeHaveTotalSize) {
					if (reverseBytes<uint32_t>() != segmentId) {
						message_printer::printError<print_message_type::general>(
							jsonifier::string{ "Missing a Segment, which was expected at index: " + jsonifier::toString(currentPosition) + jsonifier::string{ "..." } });
						if (!findNextId(segmentId)) {
							if ((totalSize > 0 && static_cast<int64_t>(currentPosition) >= totalSize)) {
								areWeDoneVal = true;
							}
							return;
						}
						message_printer::printSuccess<print_message_type::general>(
							jsonifier::string{ "Missing Segment, found at index: " + jsonifier::toString(currentPosition) + "." });
					} else {
						currentPosition += sizeof(uint32_t);
					}
					totalSize		  = collectElementSize();
					doWeHaveTotalSize = true;
				}
				while (currentPosition + 3 < data.size() && data.find(static_cast<uint8_t>(0xa3)) != jsonifier::string::npos) {
					if (currentPosition >= data.size() - 8) {
						if ((totalSize > 0 && static_cast<int64_t>(currentPosition) >= totalSize)) {
							areWeDoneVal = true;
						}
						return;
					}
					if (data.at(currentPosition) == simpleBlockId && ((data.at(currentPosition + 2) == opusTrackId || data.at(currentPosition + 3) == opusTrackId))) {
						++currentPosition;
						if (currentSize == 0) {
							currentSize = static_cast<uint64_t>(collectElementSize());
							if (static_cast<int64_t>(currentSize) >= totalSize || static_cast<int64_t>(currentSize) >= 1276 || static_cast<int64_t>(currentSize) < 4) {
								++currentPosition;
								currentSize = 0;
								continue;
							} else if (currentSize == static_cast<uint64_t>(-1) || currentPosition + currentSize >= data.size()) {
								if ((totalSize > 0 && static_cast<int64_t>(currentPosition) >= totalSize)) {
									areWeDoneVal = true;
								}
								return;
							} else {
								parseOpusFrame();
							}
						} else {
							currentSize = 0;
						}
					} else {
						++currentPosition;
					}
				}
				if ((totalSize > 0 && static_cast<int64_t>(currentPosition) >= totalSize)) {
					areWeDoneVal = true;
				}
				return;
			}

			/// @brief Checks if the demuxing process is complete.
			/// @return True if demuxing is complete, false otherwise.
			inline bool areWeDone() {
				return areWeDoneVal;
			}

		  protected:
			jsonifier::string_view_base<uint8_t> data{};///< Input data for demuxing.
			std::deque<audio_frame_data> frames{};///< Queue to store collected frames.
			bool doWeHaveTotalSize{ false };///< Flag indicating if total size has been determined.
			bool areWeDoneVal{ false };///< Flag indicating if demuxing is complete.
			uint64_t currentPosition{};///< Current position in the data.
			uint64_t currentSize{};///< Current size of the element being processed.
			int64_t totalSize{};///< Total size of the segment.

			/// @brief Finds the next occurrence of the specified value in the data.
			/// @tparam object_type The type of value to search for.
			/// @param value The value to search for.
			/// @return True if the value was found, false otherwise.
			template<typename object_type> inline bool findNextId(object_type value) {
				if (currentPosition + sizeof(object_type) >= data.size()) {
					return false;
				}
				while (currentPosition + sizeof(object_type) < data.size()) {
					if (reverseBytes<object_type>() == value) {
						currentPosition += sizeof(object_type);
						return true;
					}
					++currentPosition;
				}
				return false;
			}

			/// @brief Reverses the byte order of the current element being processed.
			/// @tparam object_type The type of the current element.
			/// @return The current element with reversed byte order.
			template<typename object_type> inline object_type reverseBytes() {
				if (data.size() <= currentPosition + sizeof(object_type)) {
					return static_cast<object_type>(-1);
				}
				object_type newValue{};
				std::memcpy(&newValue, &data.at(currentPosition), sizeof(object_type));
				newValue = reverseByteOrder(newValue);
				return newValue;
			}

			/// @brief Collects the size of the current element being processed.
			/// @return The size of the current element.
			inline int64_t collectElementSize() {
				if (currentPosition >= data.size() - 8) {
					return -1;
				}
				return collectNumber();
			}

			/// @brief Collects a number from the data.
			/// @return The collected number.
			inline int64_t collectNumber() {
				int32_t read{}, n{ 1 };
				uint64_t total{};
				total = static_cast<uint8_t>(data.at(currentPosition++));

				read = 8 - ffLog2Tab[total];

				total ^= 1ULL << ffLog2Tab[total];
				while (n++ < read) {
					total = (total << 8) | static_cast<uint8_t>(data.at(currentPosition++));
				}
				return static_cast<int64_t>(total);
			}

			/// @brief Parses an Opus frame.
			inline void parseOpusFrame() {
				audio_frame_data frameNew{};
				frameNew.currentSize = static_cast<int64_t>(currentSize - 4);
				frameNew += jsonifier::string_view_base<uint8_t>{ data.data() + currentPosition + 4, static_cast<uint64_t>(frameNew.currentSize) };
				frameNew.type = audio_frame_type::encoded;
				currentPosition += currentSize;
				frames.emplace_back(std::move(frameNew));
				currentSize = 0;
			}
		};

		/// @brief A class representing an Opus packet.
		class opus_packet {
		  public:
			inline opus_packet() = default;

			/// @brief Constructor for opus_packet.
			/// @param newData The data for the Opus packet.
			inline opus_packet(jsonifier::vector<uint8_t> newData) {
				dataVal = newData;
			};

			/// @brief Returns the size of the Opus packet data.
			/// @return The size of the Opus packet data.
			inline uint64_t size() {
				return dataVal.size();
			}

			/// @brief Returns a pointer to the Opus packet data.
			/// @return A pointer to the Opus packet data.
			inline auto data() {
				return dataVal.data();
			}

		  protected:
			jsonifier::vector<uint8_t> dataVal{};///< The data for the Opus packet.
		};

		/// @brief A class representing an Ogg page for demuxing.
		class ogg_page {
		  public:
			/// @brief Constructor for ogg_page.
			/// @param newData The data for the Ogg page.
			inline ogg_page(jsonifier::vector<uint8_t>& newData) {
				data = std::move(newData);
				verifyAsOggPage();
				getSegmentData();
			}

			/// @brief Retrieves the next Opus packet from the Ogg page.
			/// @param newPacket Reference to store the retrieved Opus packet.
			/// @return True if an Opus packet was retrieved, false otherwise.
			inline bool getOpusPacket(opus_packet& newPacket) {
				if (segmentTable.size() > 0) {
					auto newSpace = static_cast<uint64_t>(segmentTable.front());
					segmentTable.pop_front();
					jsonifier::vector<uint8_t> returnValue{};
					returnValue.resize(newSpace);
					std::memcpy(returnValue.data(), data.data() + currentPosition, newSpace);
					currentPosition += newSpace;
					newPacket = returnValue;
					return true;
				} else {
					return false;
				}
			}

			/// @brief Parses the segment data of the Ogg page.
			inline void getSegmentData() {
				segmentCount = data.at(26);
				currentPosition += 27;
				for (int32_t x{}; x < segmentCount; ++x) {
					int32_t packetLength{ data.at(27ULL + x) };
					while (data.at(27ULL + x) == 255) {
						++x;
						packetLength += data.at(27ULL + x);
					}
					segmentTable.emplace_back(packetLength);
				}
				currentPosition += segmentCount;
				return;
			}

			/// @brief Returns the size of the Ogg page data.
			/// @return The size of the Ogg page data.
			inline uint64_t getDataSize() {
				return data.size();
			}

		  protected:
			std::deque<int32_t> segmentTable{};///< Segment table storing Opus packet sizes.
			jsonifier::vector<uint8_t> data{};///< The data for the Ogg page.
			uint64_t totalPacketSize{};///< Total size of Opus packets in the page.
			uint64_t currentPosition{};///< Current position in the page data.
			int32_t segmentCount{};///< Number of segments in the Ogg page.

			/// @brief Verifies that the data represents a valid Ogg page.
			inline void verifyAsOggPage() {
				while (data.at(currentPosition) != 'O' || data.at(currentPosition + 1) != 'g' || data.at(currentPosition + 2) != 'g' || data.at(currentPosition + 3) != 'S') {
					++currentPosition;
					if (currentPosition >= data.size()) {
						return;
					}
				}
			}
		};

		/// @brief A class for demuxing Ogg-contained audio data.
		class ogg_demuxer {
		  public:
			inline ogg_demuxer() = default;

			/// @brief Collects the next audio frame from the demuxer.
			/// @param frameNew The reference to store the collected frame.
			/// @return True if a frame was collected, false otherwise.
			inline bool collectFrame(audio_frame_data& frameNew) {
				if (frames.size() > 0) {
					frameNew = std::move(frames.front());
					frames.pop_front();
					return true;
				} else {
					return false;
				}
			}

			/// @brief Writes data to the Ogg demuxer and processes it.
			/// @param inputData The data to be written and processed.
			inline void writeData(jsonifier::string_view inputData) {
				uint64_t pos = 0;
				data.clear();
				data.resize(inputData.size());
				std::memcpy(data.data(), inputData.data(), inputData.size());
				uint64_t collectedLength{};
				while (pos < inputData.size()) {
					uint64_t oggPos = inputData.find("OggS", pos);
					if (oggPos != jsonifier::string::npos) {
						uint64_t nextOggPos = inputData.find("OggS", oggPos + 1);
						if (nextOggPos != jsonifier::string::npos) {
							collectedLength += nextOggPos - oggPos;
							jsonifier::vector<uint8_t> newerString{};
							newerString.resize(nextOggPos - oggPos);
							std::memcpy(newerString.data(), data.data() + oggPos, nextOggPos - oggPos);
							pages.emplace_back(newerString);
							pos = nextOggPos;
						} else {
							jsonifier::vector<uint8_t> newerString{};
							newerString.resize(inputData.size() - collectedLength);
							std::memcpy(newerString.data(), data.data() + oggPos, inputData.size() - collectedLength);
							pages.emplace_back(newerString);
							pos = collectedLength;
							break;
						}
					} else {
						jsonifier::vector<uint8_t> newerString{};
						newerString.resize(inputData.size() - collectedLength);
						std::memcpy(newerString.data(), data.data() + oggPos, inputData.size() - collectedLength);
						pages.emplace_back(newerString);
						break;
					}
				}
				return;
			}

			/// @brief Proceeds with the demuxing process.
			/// @return True if demuxing is successful, false otherwise.
			inline bool proceedDemuxing() {
				while (1) {
					if (!processOggPage()) {
						return false;
					}
				}
				return true;
			}

		  protected:
			jsonifier::string_base<uint8_t> data{};///< Input data for demuxing.
			std::deque<audio_frame_data> frames{};///< Queue to store collected audio frames.
			std::deque<opus_packet> packets{};///< Queue to store Opus packets.
			std::deque<ogg_page> pages{};///< Queue to store Ogg pages.

			/// @brief Processes an Ogg page for demuxing.
			/// @return True if processing is successful, false if there are no more pages.
			inline bool processOggPage() {
				if (pages.empty()) {
					return false;
				}

				processPages();
				processPackets();

				return true;
			}

			/// @brief Processes Opus packets extracted from Ogg pages.
			inline void processPackets() {
				while (!packets.empty()) {
					opus_packet newPacket = packets.front();
					packets.pop_front();
					audio_frame_data newFrame{};
					newFrame += jsonifier::string_view_base<uint8_t>{ newPacket.data(), newPacket.size() };
					newFrame.currentSize = static_cast<int64_t>(newPacket.size());
					newFrame.type		 = audio_frame_type::encoded;
					frames.emplace_back(std::move(newFrame));
				}
			}

			/// @brief Processes Ogg pages to extract Opus packets.
			inline void processPages() {
				while (!pages.empty()) {
					ogg_page page = pages.front();
					pages.pop_front();
					opus_packet newPacket{};
					while (page.getOpusPacket(newPacket)) {
						packets.emplace_back(newPacket);
					}
				}
			}
		};

		/**@}*/
	}
}