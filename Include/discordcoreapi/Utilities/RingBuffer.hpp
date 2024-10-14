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
/// RingBuffer.hpp - Header file for the ring_buffer class.
/// Jun 28, 2022
/// https://discordcoreapi.com
/// \file RingBuffer.hpp
#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

#include <string_view>
#include <cstring>
#include <cstdint>
#include <vector>
#include <array>

namespace discord_core_api {

	namespace discord_core_internal {

		/**
		* \addtogroup discord_core_internal
		* @{
		*/

		/// @brief Enum representing different access types for a ring buffer.
		enum class ring_buffer_access_type { read = 0, write = 1 };

		/// @brief A template interface for a ring buffer.
		/// @tparam value_type the type of data stored in the buffer.
		/// @tparam size the size of the buffer.
		template<typename value_type_new, uint64_t size> class ring_buffer_interface {
		  public:
			using value_type = value_type_new;
			using pointer	 = value_type*;
			using size_type	 = uint64_t;

			/// @brief Constructor. initializes the buffer size.
			DCA_INLINE ring_buffer_interface() {
				arrayValue.resize(size);
			}

			DCA_INLINE ring_buffer_interface& operator=(ring_buffer_interface&&) noexcept	   = default;
			DCA_INLINE ring_buffer_interface(ring_buffer_interface&&) noexcept				   = default;
			DCA_INLINE ring_buffer_interface& operator=(const ring_buffer_interface&) noexcept = default;
			DCA_INLINE ring_buffer_interface(const ring_buffer_interface&) noexcept			   = default;

			// forward declaration to grant friendship to the ring_buffer class.
			template<typename value_type2, size_type slice_count> friend class ring_buffer;

			/// @brief Modify the read or write position of the buffer.
			/// @param type the access type (read or write).
			/// @param sizeNew the size by which to modify the position.
			DCA_INLINE void modifyReadOrWritePosition(ring_buffer_access_type type, size_type sizeNew) {
				if (type == ring_buffer_access_type::read) {
					tail += sizeNew;
				} else {
					head += sizeNew;
				}
			}

			/// @brief Get the used space in the buffer.
			/// @return the used space in the buffer.
			DCA_INLINE size_type getUsedSpace() {
				return head - tail;
			}

			/// @brief Get the used space in the buffer.
			/// @return the used space in the buffer.
			DCA_INLINE size_type getFreeSpace() {
				return size - getUsedSpace();
			}

			/// @brief Get a pointer to the current tail position.
			/// @return a pointer to the current tail position.
			DCA_INLINE pointer getCurrentTail() {
				return arrayValue.data() + (tail % size);
			}

			/// @brief Get a pointer to the current head position.
			/// @return a pointer to the current head position.
			DCA_INLINE pointer getCurrentHead() {
				return arrayValue.data() + (head % size);
			}

			/// @brief Check if the buffer is empty.
			/// @return true if the buffer is empty, otherwise false.
			DCA_INLINE bool isItEmpty() {
				return tail == head;
			}

			/// @brief Check if the buffer is full.
			/// @return true if the buffer is full, otherwise false.
			bool isItFull() {
				return getUsedSpace() == size;
			}

			/// @brief Clear the buffer by resetting positions.
			DCA_INLINE void clear() {
				tail = 0;
				head = 0;
			}

		  protected:
			jsonifier::vector<std::remove_cvref_t<value_type>> arrayValue{};///< The underlying data array.
			size_type tail{};///< The tail position in the buffer.
			size_type head{};///< The head position in the buffer.
		};

		/// @brief A template implementation of a ring buffer using ring_buffer_interface.
		/// @tparam value_type the type of data stored in the buffer.
		/// @tparam slice_count the number of slices.
		template<typename value_type_new, uint64_t slice_count> class ring_buffer
			: public ring_buffer_interface<ring_buffer_interface<std::remove_cvref_t<value_type_new>, 1024 * 16>, slice_count> {
		  public:
			using base_type		= ring_buffer_interface<ring_buffer_interface<std::remove_cvref_t<value_type_new>, 1024 * 16>, slice_count>;
			using value_type	= typename ring_buffer_interface<std::remove_cvref_t<value_type_new>, 1024 * 16>::value_type;
			using const_pointer = const value_type*;
			using pointer		= value_type*;
			using size_type		= uint64_t;

			/// @brief Default constructor. initializes the buffer size.
			DCA_INLINE ring_buffer() noexcept							   = default;
			DCA_INLINE ring_buffer& operator=(ring_buffer&&) noexcept	   = default;
			DCA_INLINE ring_buffer(ring_buffer&&) noexcept				   = default;
			DCA_INLINE ring_buffer& operator=(const ring_buffer&) noexcept = default;
			DCA_INLINE ring_buffer(const ring_buffer&) noexcept			   = default;

			/// @brief Write data into the buffer.
			/// @tparam value_type_new the type of data to be written.
			/// @param data pointer to the data.
			/// @param sizeNew size of the data.
			template<typename value_type_newer> DCA_INLINE void writeData(value_type_newer* data, size_type sizeNew) {
				if (base_type::isItFull()) {
					base_type::getCurrentTail()->clear();
					base_type::modifyReadOrWritePosition(ring_buffer_access_type::read, 1);
				}
				size_type writeSize{ sizeNew };
				std::memcpy(base_type::getCurrentHead()->getCurrentHead(), data, sizeNew);
				base_type::getCurrentHead()->modifyReadOrWritePosition(ring_buffer_access_type::write, writeSize);
				base_type::modifyReadOrWritePosition(ring_buffer_access_type::write, 1);
			}

			/// @brief Read data from the buffer.
			/// @return a string view containing the read data.
			DCA_INLINE jsonifier::string_view_base<std::remove_cvref_t<value_type>> readData() {
				jsonifier::string_view_base<std::remove_cvref_t<value_type>> returnData{};
				if (base_type::getCurrentTail()->getUsedSpace() > 0) {
					returnData =
						jsonifier::string_view_base<std::remove_cvref_t<value_type>>{ base_type::getCurrentTail()->getCurrentTail(), base_type::getCurrentTail()->getUsedSpace() };
					base_type::getCurrentTail()->clear();
					base_type::modifyReadOrWritePosition(ring_buffer_access_type::read, 1);
				}
				return returnData;
			}
		};

		/**@}*/
	}
}
