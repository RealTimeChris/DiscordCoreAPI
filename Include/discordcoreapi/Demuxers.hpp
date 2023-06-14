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

#include <discordcoreapi/AudioDecoder.hpp>
#include <discordcoreapi/AudioEncoder.hpp>
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

	class DiscordCoreAPI_Dll MatroskaDemuxer {
	  public:
		MatroskaDemuxer(bool doWePrintErrorsNew) noexcept;

		bool collectFrame(DiscordCoreAPI::AudioFrameData& frameNew) noexcept;

		void writeData(std::basic_string_view<uint8_t> stringNew) noexcept;

		void proceedDemuxing() noexcept;

		bool areWeDone() noexcept;

	  protected:
		std::deque<DiscordCoreAPI::AudioFrameData> frames{};
		std::basic_string_view<uint8_t> data{};
		bool doWeHaveTotalSize{ false };
		bool doWePrintErrors{ false };
		bool areWeDoneVal{ false };
		size_t currentPosition{};
		size_t currentSize{};
		size_t totalSize{};

		template<typename ObjectType> bool findNextId(ObjectType value) noexcept;

		template<typename ObjectType> inline ObjectType reverseBytes() noexcept;

		int64_t collectElementSize() noexcept;

		int64_t collectNumber() noexcept;

		void parseOpusFrame() noexcept;
	};

	class DiscordCoreAPI_Dll OpusPacket {
	  public:
		OpusPacket() noexcept = default;

		OpusPacket(std::basic_string_view<uint8_t> newData) noexcept;

		std::basic_string_view<uint8_t> data{};
	};

	class DiscordCoreAPI_Dll OggPage {
	  public:
		OggPage(std::basic_string_view<uint8_t>& newData) noexcept;

		bool getOpusPacket(OpusPacket& newPacket) noexcept;

		void getSegmentData() noexcept;

		size_t getDataSize() noexcept;

	  protected:
		std::basic_string_view<uint8_t> data{};
		std::deque<int32_t> segmentTable{};
		size_t totalPacketSize{};
		size_t currentPosition{};
		int32_t segmentCount{};

		void verifyAsOggPage();
	};

	class DiscordCoreAPI_Dll OggDemuxer {
	  public:
		OggDemuxer(bool doWePrintErrorsNew) noexcept;

		bool collectFrame(DiscordCoreAPI::AudioFrameData& frameNew) noexcept;

		void writeData(std::string_view inputData) noexcept;

		bool proceedDemuxing() noexcept;

	  protected:
		std::deque<DiscordCoreAPI::AudioFrameData> frames{};
		DiscordCoreAPI::LightString<uint8_t> data{};
		std::deque<OpusPacket> packets{};
		bool doWePrintErrors{ false };
		std::deque<OggPage> pages{};

		bool processOggPage() noexcept;

		void processPackets() noexcept;

		void processPages() noexcept;
	};
}