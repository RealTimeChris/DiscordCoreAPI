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

#include <discordcoreapi/Utilities/Utilities.hpp>
#include <opus/opus.h>
#include <fstream>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		/**
		* \addtogroup utilities
		* @{
		*/

		inline static constexpr uint32_t SEGMENT_ID{ 0x18538067 };
		inline static constexpr uint8_t SIMPLEBLOCK_ID{ 0xA3 };
		inline static constexpr uint8_t OPUS_TRACK_ID{ 0x81 };

		inline static constexpr std::array<uint8_t, 256> ffLog2Tab{ 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
			4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 };

		/// \brief For demuxing Matroska-contained audio data.
		class DiscordCoreAPI_Dll MatroskaDemuxer {
		  public:
			/// \brief Constructor for MatroskaDemuxer.
			inline MatroskaDemuxer() = default;

			/// \brief Writes data to the Matroska demuxer.
			/// \param dataNew The data to be written.
			inline void writeData(std::basic_string_view<uint8_t> dataNew) {
				data = dataNew;
			}

			/// \brief Collects the next frame from the demuxer.
			/// \param frameNew The reference to store the collected frame.
			/// \return True if a frame was collected, false otherwise.
			inline bool collectFrame(AudioFrameData& frameNew) {
				if (frames.size() > 0) {
					frameNew = std::move(frames.at(0));
					frames.erase(frames.begin());
					return true;
				} else {
					return false;
				}
			}

			/// \brief Proceed with the demuxing process.
			inline void proceedDemuxing() {
				if (!doWeHaveTotalSize) {
					if (reverseBytes<uint32_t>() != SEGMENT_ID) {
						MessagePrinter::printError<PrintMessageType::General>(
							"Missing a Segment, which was expected at index: " + std::to_string(currentPosition) + std::string{ "..." });
						if (!findNextId(SEGMENT_ID)) {
							if ((totalSize > 0 && currentPosition >= totalSize)) {
								areWeDoneVal = true;
							}
							return;
						}
						MessagePrinter::printSuccess<PrintMessageType::General>(
							"Missing Segment, found at index: " + std::to_string(currentPosition) + ".");
					} else {
						currentPosition += sizeof(uint32_t);
					}
					totalSize = collectElementSize();
					doWeHaveTotalSize = true;
				}
				while (currentPosition + 3 < data.size() && data.find(0xa3) != std::string::npos) {
					if (currentPosition >= data.size() - 8) {
						if ((totalSize > 0 && currentPosition >= totalSize)) {
							areWeDoneVal = true;
						}
						return;
					}
					if (data.at(currentPosition) == SIMPLEBLOCK_ID &&
						((data.at(currentPosition + 2) == OPUS_TRACK_ID || data.at(currentPosition + 3) == OPUS_TRACK_ID))) {
						++currentPosition;
						if (currentSize == 0) {
							currentSize = collectElementSize();
							if (currentSize >= totalSize || currentSize >= 1276 || currentSize < 4) {
								++currentPosition;
								currentSize = 0;
								continue;
							} else if (currentSize == static_cast<uint64_t>(-1) || currentPosition + currentSize >= data.size()) {
								if ((totalSize > 0 && currentPosition >= totalSize)) {
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
				if ((totalSize > 0 && currentPosition >= totalSize)) {
					areWeDoneVal = true;
				}
				return;
			}

			/// \brief Checks if the demuxing process is complete.
			/// \return True if demuxing is complete, false otherwise.
			inline bool areWeDone() {
				return areWeDoneVal;
			}

		  protected:
			std::deque<AudioFrameData> frames{};///< Queue to store collected frames. */
			std::basic_string_view<uint8_t> data{};///< Input data for demuxing. */
			bool doWeHaveTotalSize{ false };///< Flag indicating if total size has been determined. */
			bool areWeDoneVal{ false };///< Flag indicating if demuxing is complete. */
			uint64_t currentPosition{};///< Current position in the data. */
			uint64_t currentSize{};///< Current size of the element being processed. */
			uint64_t totalSize{};///< Total size of the segment. */

			/// \brief Finds the next occurrence of the specified value in the data.
			/// \tparam ObjectType The type of value to search for.
			/// \param value The value to search for.
			/// \return True if the value was found, false otherwise.
			template<typename ObjectType> inline bool findNextId(ObjectType value) {
				if (currentPosition + sizeof(ObjectType) >= data.size()) {
					return false;
				}
				while (currentPosition + sizeof(ObjectType) < data.size()) {
					if (reverseBytes<ObjectType>() == value) {
						currentPosition += sizeof(ObjectType);
						return true;
					}
					++currentPosition;
				}
				return false;
			}

			/// \brief Reverses the byte order of the current element being processed.
			/// \tparam ObjectType The type of the current element.
			/// \return The current element with reversed byte order.
			template<typename ObjectType> inline ObjectType reverseBytes() {
				if (data.size() <= currentPosition + sizeof(ObjectType)) {
					return static_cast<ObjectType>(-1);
				}
				const ObjectType newValue{ *reinterpret_cast<const ObjectType*>(&data.at(currentPosition)) };
				ObjectType newerValue{ newValue };
				newerValue = reverseByteOrder(newerValue);
				return newerValue;
			}

			/// \brief Collects the size of the current element being processed.
			/// \return The size of the current element.
			inline int64_t collectElementSize() {
				if (currentPosition >= data.size() - 8) {
					return -1;
				}
				return collectNumber();
			}

			/// \brief Collects a number from the data.
			/// \return The collected number.
			inline int64_t collectNumber() {
				int32_t read{}, n{ 1 };
				uint64_t total{};
				total = static_cast<uint8_t>(data.at(currentPosition++));

				read = 8 - ffLog2Tab[total];

				total ^= 1ull << ffLog2Tab[total];
				while (n++ < read) {
					total = (total << 8) | static_cast<uint8_t>(data.at(currentPosition++));
				}
				return static_cast<int64_t>(total);
			}

			/// \brief Parses an Opus frame.
			inline void parseOpusFrame() {
				AudioFrameData frameNew{};
				frameNew.currentSize = currentSize - 4;
				frameNew += std::basic_string_view<uint8_t>{ reinterpret_cast<const uint8_t*>(data.data() + currentPosition + 4),
					static_cast<uint64_t>(frameNew.currentSize) };
				frameNew.type = AudioFrameType::Encoded;
				currentPosition += currentSize;
				frames.emplace_back(std::move(frameNew));
				currentSize = 0;
			}
		};

		class DiscordCoreAPI_Dll OpusPacket {
		  public:
			inline OpusPacket() = default;

			inline OpusPacket(std::basic_string_view<uint8_t> newData) {
				dataVal = newData;
			};

			inline uint64_t size() {
				return dataVal.size();
			}

			inline auto data() {
				return dataVal.data();
			}

		  protected:
			std::basic_string_view<uint8_t> dataVal{};
		};

		class DiscordCoreAPI_Dll OggPage {
		  public:
			inline OggPage(std::basic_string_view<uint8_t>& newData) {
				data = std::move(newData);
				verifyAsOggPage();
				getSegmentData();
			}

			inline bool getOpusPacket(OpusPacket& newPacket) {
				if (segmentTable.size() > 0) {
					auto newSpace = segmentTable.front();
					segmentTable.pop_front();
					std::basic_string_view<uint8_t> returnValue{ data.data() + currentPosition, static_cast<uint64_t>(newSpace) };
					currentPosition += newSpace;
					newPacket = returnValue;
					return true;
				} else {
					return false;
				}
			}

			inline void getSegmentData() {
				segmentCount = data.at(26);
				currentPosition += 27;
				for (int32_t i{}; i < segmentCount; ++i) {
					int32_t packetLength{ data.at(27ull + i) };
					while (data.at(27ull + i) == 255) {
						++i;
						packetLength += data.at(27ull + i);
					}
					segmentTable.emplace_back(packetLength);
				}
				currentPosition += segmentCount;
				return;
			}

			inline uint64_t getDataSize() {
				return data.size();
			}

		  protected:
			std::basic_string_view<uint8_t> data{};
			std::deque<int32_t> segmentTable{};
			uint64_t totalPacketSize{};
			uint64_t currentPosition{};
			int32_t segmentCount{};

			inline void verifyAsOggPage() {
				while (data.at(currentPosition) != 'O' || data.at(currentPosition + 1) != 'g' || data.at(currentPosition + 2) != 'g' ||
					data.at(currentPosition + 3) != 'S') {
					++currentPosition;
					if (currentPosition >= data.size()) {
						return;
					}
				}
			}
		};

		class DiscordCoreAPI_Dll OggDemuxer {
		  public:
			inline OggDemuxer() = default;

			inline bool collectFrame(AudioFrameData& frameNew) {
				if (frames.size() > 0) {
					frameNew = std::move(frames.front());
					frames.pop_front();
					return true;
				} else {
					return false;
				}
			}

			inline void writeData(std::string_view inputData) {
				uint64_t pos = 0;
				data.clear();
				data.resize(inputData.size());
				std::memcpy(data.data(), inputData.data(), inputData.size());
				uint64_t collectedLength{};
				while (pos < inputData.size()) {
					uint64_t oggPos = inputData.find("OggS", pos);
					if (oggPos != std::string::npos) {
						uint64_t nextOggPos = inputData.find("OggS", oggPos + 1);
						if (nextOggPos != std::string::npos) {
							collectedLength += nextOggPos - oggPos;
							std::basic_string_view<uint8_t> newerString{ data.data() + oggPos, nextOggPos - oggPos };
							pages.emplace_back(newerString);
							pos = nextOggPos;
						} else {
							std::basic_string_view<uint8_t> newerString{ data.data() + oggPos, inputData.size() - collectedLength };
							pages.emplace_back(newerString);
							pos = collectedLength;
							break;
						}
					} else {
						std::basic_string_view<uint8_t> newerString{ data.data() + collectedLength, inputData.size() - collectedLength };
						pages.emplace_back(newerString);
						break;
					}
				}
				return;
			}

			inline bool proceedDemuxing() {
				while (1) {
					if (!processOggPage()) {
						return false;
					}
				}
				return true;
			}

		  protected:
			std::deque<AudioFrameData> frames{};
			std::deque<OpusPacket> packets{};
			LightString<uint8_t> data{};
			std::deque<OggPage> pages{};

			inline bool processOggPage() {
				if (pages.empty()) {
					return false;
				}

				processPages();
				processPackets();

				return true;
			}

			inline void processPackets() {
				while (!packets.empty()) {
					OpusPacket newPacket = packets.front();
					packets.pop_front();
					AudioFrameData newFrame{};
					newFrame += std::basic_string_view<uint8_t>{ newPacket.data(), newPacket.size() };
					newFrame.currentSize = newPacket.size();
					newFrame.type = AudioFrameType::Encoded;
					frames.emplace_back(std::move(newFrame));
				}
			}

			inline void processPages() {
				while (!pages.empty()) {
					OggPage page = pages.front();
					pages.pop_front();
					OpusPacket newPacket{};
					while (page.getOpusPacket(newPacket)) {
						packets.push_back(newPacket);
					}
				}
			}
		};
		/**@}*/
	}
}
