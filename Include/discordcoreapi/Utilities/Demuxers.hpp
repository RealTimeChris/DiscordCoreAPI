/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// Demuxers.hpp - Header file for the Demuxer classes.
/// Jun 8, 2023
/// https://discordcoreapi.com
/// \file Demuxers.hpp

#pragma once

#include <discordcoreapi/Utilities.hpp>
#include <opus/opus.h>
#include <fstream>

namespace DiscordCoreInternal {

	inline static constexpr uint32_t SEEKHEAD_ID{ 0x114d9b74 };
	inline static constexpr uint32_t CLUSTER_ID{ 0x1f43b675 };
	inline static constexpr uint32_t SEGMENT_ID{ 0x18538067 };
	inline static constexpr uint8_t SIMPLEBLOCK_ID{ 0xA3 };
	inline static constexpr uint8_t OPUS_TRACK_ID{ 0x81 };

	inline static constexpr uint8_t ffLog2Tab[256] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 };
		
	class MatroskaDemuxer {
	  public:
		inline MatroskaDemuxer(bool doWePrintErrorsNew) noexcept {
			doWePrintErrors = doWePrintErrorsNew;
		}

		inline void writeData(std::basic_string_view<uint8_t> dataNew) noexcept {
			data = dataNew;
		}

		inline bool collectFrame(DiscordCoreAPI::AudioFrameData& frameNew) noexcept {
			if (frames.size() > 0) {
				frameNew = std::move(frames[0]);
				frames.erase(frames.begin());
				return true;
			} else {
				return false;
			}
		}

		inline void proceedDemuxing() noexcept {
			if (!doWeHaveTotalSize) {
				if (reverseBytes<uint32_t>() != SEGMENT_ID) {
					if (doWePrintErrors) {
						std::cout << "Missing a Segment, which was expected at index: " << currentPosition << "..." << std::endl;
					}
					if (!findNextId(SEGMENT_ID)) {
						if ((totalSize > 0 && currentPosition >= totalSize)) {
							areWeDoneVal = true;
						}
						return;
					}
					if (doWePrintErrors) {
						std::cout << "Missing Segment, found at index: " << currentPosition << "." << std::endl;
					}
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
				if (data[currentPosition] == SIMPLEBLOCK_ID &&
					((data[currentPosition + 2] == OPUS_TRACK_ID || data[currentPosition + 3] == OPUS_TRACK_ID))) {
					++currentPosition;
					if (currentSize == 0) {
						currentSize = collectElementSize();
						if (currentSize >= totalSize || currentSize >= 1276 || currentSize < 4) {
							++currentPosition;
							currentSize = 0;
							continue;
						} else if (currentSize == -1 || currentPosition + currentSize >= data.size()) {
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

		inline bool areWeDone() noexcept {
			return areWeDoneVal;
		}

	  protected:
		std::deque<DiscordCoreAPI::AudioFrameData> frames{};
		std::basic_string_view<uint8_t> data{};
		bool doWeHaveTotalSize{ false };
		bool doWePrintErrors{ false };
		bool areWeDoneVal{ false };
		size_t currentPosition{};
		size_t currentSize{};
		size_t totalSize{};

		template<typename ObjectType> inline bool findNextId(ObjectType value) noexcept {
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

		template<typename ObjectType> inline ObjectType reverseBytes() noexcept {
			if (data.size() <= currentPosition + sizeof(ObjectType)) {
				return -1;
			}
			const ObjectType newValue{ *reinterpret_cast<const ObjectType*>(&data[currentPosition]) };
			ObjectType newerValue{ newValue };
			reverseByteOrder(newerValue);
			return newerValue;
		}

		inline int64_t collectElementSize() noexcept {
			if (currentPosition >= data.size() - 8) {
				return -1;
			}
			return collectNumber();
		}

		inline int64_t collectNumber() noexcept {
			int32_t read{}, n{ 1 };
			uint64_t total{};
			total = static_cast<uint8_t>(data[currentPosition++]);

			read = 8 - ffLog2Tab[total];

			total ^= 1ull << ffLog2Tab[total];
			while (n++ < read) {
				total = (total << 8) | static_cast<uint8_t>(data[currentPosition++]);
			}
			return static_cast<int64_t>(total);
		}

		inline void parseOpusFrame() noexcept {
			DiscordCoreAPI::AudioFrameData frameNew{};
			frameNew.currentSize = currentSize - 4;
			frameNew += std::basic_string_view<uint8_t>{ reinterpret_cast<const uint8_t*>(data.data() + currentPosition + 4),
				static_cast<uint64_t>(frameNew.currentSize) };
			frameNew.type = DiscordCoreAPI::AudioFrameType::Encoded;
			currentPosition += currentSize;
			frames.emplace_back(std::move(frameNew));
			currentSize = 0;
		}
	};

	class OpusPacket {
	  public:
		inline OpusPacket() noexcept = default;

		inline OpusPacket(std::basic_string_view<uint8_t> newData) noexcept {
			data = newData;
		};

		std::basic_string_view<uint8_t> data{};
	};

	class OggPage {
	  public:

		inline OggPage(std::basic_string_view<uint8_t>& newData) noexcept {
			data = std::move(newData);
			verifyAsOggPage();
			getSegmentData();
		}

		inline bool getOpusPacket(OpusPacket& newPacket) noexcept {
			if (segmentTable.size() > 0) {
				auto newSpace = segmentTable.front();
				segmentTable.pop_front();
				std::basic_string_view<uint8_t> returnValue{ data.data() + currentPosition, static_cast<size_t>(newSpace) };
				currentPosition += newSpace;
				newPacket = returnValue;
				return true;
			} else {
				return false;
			}
		}

		inline void getSegmentData() noexcept {
			segmentCount = data[26];
			currentPosition += 27;
			for (int32_t i{}; i < segmentCount; ++i) {
				int32_t packetLength{ data[27ull + i] };
				while (data[27ull + i] == 255) {
					++i;
					packetLength += data[27ull + i];
				}
				segmentTable.emplace_back(packetLength);
			}
			currentPosition += segmentCount;
			return;
		}

		inline size_t getDataSize() noexcept {
			return data.size();
		}

	  protected:
		std::basic_string_view<uint8_t> data{};
		std::deque<int32_t> segmentTable{};
		size_t totalPacketSize{};
		size_t currentPosition{};
		int32_t segmentCount{};

		inline void verifyAsOggPage() {
			while (data[currentPosition] != 'O' || data[currentPosition + 1] != 'g' || data[currentPosition + 2] != 'g' ||
				data[currentPosition + 3] != 'S') {
				++currentPosition;
				if (currentPosition >= data.size()) {
					return;
				}
			}
		}
	};

	class OggDemuxer {
	  public:

		inline OggDemuxer(bool doWePrintErrorsNew) noexcept {
			doWePrintErrors = doWePrintErrorsNew;
		}

		inline bool collectFrame(DiscordCoreAPI::AudioFrameData& frameNew) noexcept {
			if (frames.size() > 0) {
				frameNew = std::move(frames.front());
				frames.pop_front();
				return true;
			} else {
				return false;
			}
		}

		inline void writeData(std::string_view inputData) noexcept {
			size_t pos = 0;
			data.clear();
			data.resize(inputData.size());
			std::memcpy(data.data(), inputData.data(), inputData.size());
			size_t collectedLength{};
			while (pos < inputData.size()) {
				size_t oggPos = inputData.find("OggS", pos);
				if (oggPos != std::string::npos) {
					size_t nextOggPos = inputData.find("OggS", oggPos + 1);
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

		inline bool proceedDemuxing() noexcept {
			while (1) {
				if (!processOggPage()) {
					return false;
				}
			}
			return true;
		}

	  protected:
		std::deque<DiscordCoreAPI::AudioFrameData> frames{};
		DiscordCoreAPI::LightString<uint8_t> data{};
		std::deque<OpusPacket> packets{};
		bool doWePrintErrors{ false };
		std::deque<OggPage> pages{};

		
		inline bool processOggPage() noexcept {
			if (pages.empty()) {
				return false;
			}

			processPages();
			processPackets();

			return true;
		}

		inline void processPackets() noexcept {
			while (!packets.empty()) {
				auto newPacket = packets.front();
				packets.pop_front();
				DiscordCoreAPI::AudioFrameData newFrame{};
				newFrame += std::basic_string_view<uint8_t>{ newPacket.data.data(), newPacket.data.size() };
				newFrame.currentSize = newPacket.data.size();
				newFrame.type = DiscordCoreAPI::AudioFrameType::Encoded;
				frames.emplace_back(std::move(newFrame));
			}
		}

		inline void processPages() noexcept {
			while (!pages.empty()) {
				auto page = pages.front();
				pages.pop_front();
				OpusPacket newPacket{};
				while (page.getOpusPacket(newPacket)) {
					packets.push_back(newPacket);
				}
			}
		}
	};
}