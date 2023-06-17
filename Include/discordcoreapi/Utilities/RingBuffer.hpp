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
/// RingBuffer.hpp - Header file for the RingBuffer class.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file RingBuffer.hpp

#pragma once

#include <string_view>
#include <cstring>
#include <cstdint>
#include <array>

namespace DiscordCoreInternal {

	enum class RingBufferAccessType { Read = 0, Write = 1 };

	template<typename ValueType, uint64_t Size> class RingBufferInterface {
	  public:
		template<typename ValueType2, uint64_t SliceCount> friend class RingBuffer;

		inline constexpr void modifyReadOrWritePosition(RingBufferAccessType type, uint64_t size) noexcept {
			if (type == RingBufferAccessType::Read) {
				tail += size;
			} else {
				head += size;
			}
		}

		inline constexpr bool isItEmpty() {
			return tail == head;
		}

		inline constexpr bool isItFull() {
			return getUsedSpace() == Size;
		}

		inline constexpr size_t getUsedSpace() {
			return head - tail;
		}

		inline constexpr ValueType* getCurrentTail() noexcept {
			return arrayValue.data() + (tail % Size);
		}

		inline constexpr ValueType* getCurrentHead() noexcept {
			return arrayValue.data() + (head % Size);
		}

		inline constexpr virtual void clear() noexcept {
			tail = 0;
			head = 0;
		}

	  protected:
		std::array<std::decay_t<ValueType>, Size> arrayValue{};
		uint64_t tail{};
		uint64_t head{};
	};

	template<typename ValueType, uint64_t SliceCount> class RingBuffer
		: public RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount> {
	  public:
		inline constexpr void clear() noexcept {
			for (uint64_t x = 0; x < SliceCount; ++x) {
				RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::arrayValue[x].tail = 0;
				RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::arrayValue[x].head = 0;
			}
			RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::tail = 0;
			RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::head = 0;
		}

		inline constexpr void modifyReadOrWritePosition(RingBufferAccessType type, uint64_t size) noexcept {
			RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::modifyReadOrWritePosition(type, size);
		}

		inline constexpr uint64_t getUsedSpace() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getUsedSpace();
		}

		inline constexpr auto getCurrentTail() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentTail();
		}

		inline constexpr auto isItFull() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::isItFull();
		}

		inline constexpr auto getCurrentHead() {
			return RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentHead();
		}

		inline constexpr void writeData(std::basic_string_view<std::decay_t<ValueType>> data) {
			if (isItFull() || data.size() > 16384 - getCurrentHead()->getUsedSpace()) {
				getCurrentTail()->clear();
				modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
			}
			uint64_t writeSize{ data.size() };
			std::memcpy(getCurrentHead()->getCurrentHead(), data.data(), data.size());
			getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, writeSize);
			modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
		}

		inline constexpr std::basic_string_view<std::decay_t<ValueType>> readData() {
			std::basic_string_view<std::decay_t<ValueType>> returnData{};
			if (getCurrentTail()->getUsedSpace() > 0) {
				returnData = std::basic_string_view<std::decay_t<ValueType>>{ getCurrentTail()->getCurrentTail(), getCurrentTail()->getUsedSpace() };
				getCurrentTail()->clear();
				modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
			}
			return returnData;
		}
	};
}
