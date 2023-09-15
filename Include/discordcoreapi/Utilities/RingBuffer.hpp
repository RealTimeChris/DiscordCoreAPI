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

#include <jsonifier/Index.hpp>

#include <string_view>
#include <cstring>
#include <cstdint>
#include <vector>
#include <array>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		/**
		* \addtogroup discord_core_internal
		* @{
		*/

		/// @brief Enum representing different access types for a ring buffer.
		enum class RingBufferAccessType { Read = 0, Write = 1 };

		/// @brief A template interface for a ring buffer.
		/// @tparam ValueType The type of data stored in the buffer.
		/// @tparam Size The size of the buffer.
		template<typename ValueType, uint64_t Size> class RingBufferInterface {
		  public:
			using value_type = ValueType;
			using pointer	 = value_type*;
			using size_type	 = uint64_t;

			/// @brief Constructor. Initializes the buffer size.
			RingBufferInterface() {
				arrayValue.resize(Size);
			}

			// Forward declaration to grant friendship to the RingBuffer class.
			template<typename ValueType2, size_type SliceCount> friend class RingBuffer;

			/// @brief Modify the read or write position of the buffer.
			/// @param type The access type (Read or Write).
			/// @param size The size by which to modify the position.
			void modifyReadOrWritePosition(RingBufferAccessType type, size_type size) {
				if (type == RingBufferAccessType::Read) {
					tail += size;
				} else {
					head += size;
				}
			}

			/// @brief Get the used space in the buffer.
			/// @return The used space in the buffer.
			size_type getUsedSpace() {
				return head - tail;
			}

			/// @brief Get a pointer to the current tail position.
			/// @return A pointer to the current tail position.
			pointer getCurrentTail() {
				return arrayValue.data() + (tail % Size);
			}

			/// @brief Get a pointer to the current head position.
			/// @return A pointer to the current head position.
			pointer getCurrentHead() {
				return arrayValue.data() + (head % Size);
			}

			/// @brief Check if the buffer is empty.
			/// @return True if the buffer is empty, otherwise false.
			bool isItEmpty() {
				return tail == head;
			}

			/// @brief Check if the buffer is full.
			/// @return True if the buffer is full, otherwise false.
			bool isItFull() {
				return getUsedSpace() == Size;
			}

			/// @brief Clear the buffer by resetting positions.
			void clear() {
				tail = 0;
				head = 0;
			}

		  protected:
			jsonifier::vector<std::decay_t<value_type>> arrayValue{};///< The underlying data array.
			size_type tail{};///< The tail position in the buffer.
			size_type head{};///< The head position in the buffer.
		};

		/// @brief A template implementation of a ring buffer using RingBufferInterface.
		/// @tparam ValueType The type of data stored in the buffer.
		/// @tparam SliceCount The number of slices.
		template<typename ValueType, uint64_t SliceCount> class RingBuffer : public RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount> {
		  public:
			using base_type		= RingBufferInterface<RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>, SliceCount>;
			using value_type	= RingBufferInterface<std::decay_t<ValueType>, 1024 * 16>::value_type;
			using const_pointer = const value_type*;
			using pointer		= value_type*;
			using size_type		= uint64_t;

			/// @brief Default constructor. Initializes the buffer size.
			RingBuffer() : base_type{} {};

			/// @brief Write data into the buffer.
			/// @tparam ValueTypeNew The type of data to be written.
			/// @param data Pointer to the data.
			/// @param size Size of the data.
			template<typename ValueTypeNew> void writeData(ValueTypeNew* data, size_type size) {
				if (base_type::isItFull() || base_type::getCurrentHead()->getUsedSpace() + size >= 16384) {
					base_type::getCurrentTail()->clear();
					base_type::modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
				}
				size_type writeSize{ size };
				std::memcpy(base_type::getCurrentHead()->getCurrentHead(), data, size);
				base_type::getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, writeSize);
				base_type::modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
			}

			/// @brief Read data from the buffer.
			/// @return A string view containing the read data.
			std::basic_string_view<std::decay_t<value_type>> readData() {
				std::basic_string_view<std::decay_t<value_type>> returnData{};
				if (base_type::getCurrentTail()->getUsedSpace() > 0) {
					returnData = std::basic_string_view<std::decay_t<value_type>>{ base_type::getCurrentTail()->getCurrentTail(), base_type::getCurrentTail()->getUsedSpace() };
					base_type::getCurrentTail()->clear();
					base_type::modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
				}
				return returnData;
			}
		};

		/**@}*/
	}
}
