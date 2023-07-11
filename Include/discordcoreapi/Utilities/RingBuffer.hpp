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
/// RingBuffer.hpp - Header file for the RingBuffer class.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file RingBuffer.hpp

#pragma once

#include <string_view>
#include <cstring>
#include <cstdint>
#include <vector>
#include <array>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		enum class RingBufferAccessType { Read = 0, Write = 1 };

		template<typename ValueType, uint64_t Size> class RingBufferInterface {
		  public:
			using value_type = ValueType;
			using pointer = value_type*;
			using size_type = size_t;

			inline RingBufferInterface() noexcept {
				arrayValue.resize(Size);
			}

			template<typename ValueType2, size_type SliceCount> friend class RingBuffer;

			inline void modifyReadOrWritePosition(RingBufferAccessType type, size_type size) noexcept {
				if (type == RingBufferAccessType::Read) {
					tail += size;
				} else {
					head += size;
				}
			}

			inline size_type getUsedSpace() noexcept {
				return head - tail;
			}

			inline pointer getCurrentTail() noexcept {
				return arrayValue.data() + (tail % Size);
			}

			inline pointer getCurrentHead() noexcept {
				return arrayValue.data() + (head % Size);
			}

			inline bool isItEmpty() noexcept {
				return tail == head;
			}

			inline bool isItFull() noexcept {
				return getUsedSpace() == Size;
			}

			inline void clear() noexcept {
				tail = 0;
				head = 0;
			}

		  protected:
			std::vector<std::decay_t<value_type>> arrayValue{};
			size_type tail{};
			size_type head{};
		};

		template<typename ValueType, uint64_t SliceCount> class RingBuffer
			: public RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount> {
		  public:
			using value_type = RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>::value_type;
			using const_pointer = const value_type*;
			using pointer = value_type*;
			using size_type = size_t;

			inline void writeData(const_pointer data, size_type size) noexcept {
				if (RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::isItFull() ||
					size > 16384 -
							RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentHead()
								->getUsedSpace()) {
					RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentTail()->clear();
					RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::modifyReadOrWritePosition(
						RingBufferAccessType::Read, 1);
				}
				size_type writeSize{ size };
				std::memcpy(
					RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentHead()->getCurrentHead(),
					data, size);
				RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentHead()->modifyReadOrWritePosition(
					RingBufferAccessType::Write, writeSize);
				RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::modifyReadOrWritePosition(
					RingBufferAccessType::Write, 1);
			}

			inline std::basic_string_view<std::decay_t<value_type>> readData() noexcept {
				std::basic_string_view<std::decay_t<value_type>> returnData{};
				if (RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentTail()->getUsedSpace() > 0) {
					returnData = std::basic_string_view<std::decay_t<value_type>>{
						RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentTail()->getCurrentTail(),
						RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentTail()->getUsedSpace()
					};
					RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::getCurrentTail()->clear();
					RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>::modifyReadOrWritePosition(
						RingBufferAccessType::Read, 1);
				}
				return returnData;
			}
		};
	}
}
