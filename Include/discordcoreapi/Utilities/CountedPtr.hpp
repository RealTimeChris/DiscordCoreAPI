/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright ,  Chris M. (RealTimeChris)

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
/// CountedPtr.hpp - Header for the "counted_ptr" related stuff.
/// Dec ,
/// https://discordcoreapi.com
/// \file CountedPtr.hpp

#pragma once

#include <discordcoreapi/Utilities/UniquePtr.hpp>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <iosfwd>
#include <type_traits>
#include <utility>

namespace discord_core_api {

	template<typename value_type, typename deleter_type_new = std::default_delete<value_type>> class reference_counter : public deleter_type_new {
	  public:
		using element_type = value_type;
		using pointer	   = element_type*;
		using deleter_type = deleter_type_new;

		inline reference_counter(value_type* p) : ptr{ p } {
		}

		inline void incRef() noexcept {
			refCount.fetch_add(1, std::memory_order_release);
		}

		inline void decRef() noexcept {
			if (refCount.fetch_sub(1, std::memory_order_release) == 0) {
				if (ptr) {
					getDeleter()(ptr);
				}
				ptr = nullptr;
			}
		}

		inline pointer get() {
			return ptr;
		}

		inline deleter_type& getDeleter() noexcept {
			return *this;
		}

		inline ~reference_counter() {
		}

	  protected:
		std::atomic<long> refCount{ 1 };
		value_type* ptr;
	};

	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename value_type, typename deleter_type_new = std::default_delete<value_type>> class counted_ptr {
	  public:
		using element_type = value_type;
		using reference	   = element_type&;
		using pointer	   = element_type*;

		inline counted_ptr() noexcept : referenceCount{} {
		}

		inline counted_ptr(std::nullptr_t) noexcept : referenceCount{} {
		}

		inline counted_ptr& operator=(counted_ptr&& sp) noexcept {
			counted_ptr{ std::move(sp) }.swap(*this);
			return *this;
		}

		inline counted_ptr(counted_ptr&& sp) noexcept : referenceCount{ std::move(sp.referenceCount) } {
			sp.referenceCount = nullptr;
		}

		inline counted_ptr& operator=(const counted_ptr& sp) noexcept {
			counted_ptr{ sp }.swap(*this);
			return *this;
		}

		inline counted_ptr(const counted_ptr& sp) noexcept : referenceCount{ sp.referenceCount } {
			if (referenceCount) {
				referenceCount->incRef();
			}
		}

		inline explicit counted_ptr(pointer p) : referenceCount{ new reference_counter<element_type>{ p } } {
		}

		inline void swap(counted_ptr& sp) noexcept {
			using std::swap;
			swap(referenceCount, sp.referenceCount);
		}

		inline pointer operator->() {
			return this->referenceCount->get();
		}

		inline reference operator*() {
			return *this->ptr->get();
		}

		inline void reset() noexcept {
			counted_ptr{}.swap(*this);
		}

		inline void reset(pointer p) {
			counted_ptr{ p }.swap(*this);
		}

		inline pointer get() const noexcept {
			return referenceCount->get();
		}

		inline explicit operator bool() const noexcept {
			return (referenceCount->get()) ? true : false;
		}

		inline ~counted_ptr() {
			if (referenceCount) {
				referenceCount->decRef();
			}
		}

	  protected:
		reference_counter<element_type, deleter_type_new>* referenceCount;
	};

	template<typename value_type, typename... Args> counted_ptr<value_type> makeCounted(Args&&... args) {
		return counted_ptr<value_type>(new value_type(std::forward<Args>(args)...));
	}

	/// @brief Creates a counted_ptr instance for arrays with zero extent.
	/// @tparam value_type The type of array element.
	/// @tparam deleter The deleter type to be used (default: std::default_delete<value_type>).
	/// @param size The size of the array to be created.
	/// @return A counted_ptr instance managing the created array.
	template<typename value_type, typename deleter = std::default_delete<value_type>, std::enable_if_t<std::is_array_v<value_type> && std::extent_v<value_type> == 0, uint64_t> = 0>
	inline counted_ptr<value_type> makeCounted(const uint64_t size) {
		using element_type = std::remove_extent_t<value_type>;
		return counted_ptr<value_type>(new element_type[size]());
	}

	/// @brief Explicitly deletes the function for arrays with non-zero extent.
	/// @tparam value_type The type of array with non-zero extent.
	/// @tparam ArgTypes The argument types (not applicable for deletion).
	template<typename value_type, typename... ArgTypes, std::enable_if_t<std::extent_v<value_type> != 0, uint64_t> = 0> inline void makeCounted(ArgTypes&&...) = delete;
	/**@}*/

}
