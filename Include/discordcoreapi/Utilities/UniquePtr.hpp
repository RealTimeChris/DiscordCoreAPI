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
/// UniquePtr.hpp - Header for the "UniquePtr" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file UniquePtr.hpp

#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

namespace DiscordCoreAPI {

	template<typename ValueType01, typename ValueType02>
	concept DerivedFrom = std::derived_from<ValueType01, ValueType02>;

	template<typename ValueType01, typename ValueType02>
	concept BaseOf = std::is_base_of_v<ValueType01, ValueType02>;

	template<typename ValueType01, typename ValueType02>
	concept SameAs = std::same_as<ValueType01, ValueType02>;

	template<typename ValueType01, typename ValueType02>
	concept IsRelatedPtr = DerivedFrom<ValueType01, ValueType02> || BaseOf<ValueType01, ValueType02> || SameAs<ValueType01, ValueType02>;

	template<typename ValueType, typename Deleter = std::default_delete<ValueType>> class UniquePtr {
	  public:
		using element_type = std::remove_extent_t<ValueType>;
		using pointer = element_type*;
		using deleter_type = Deleter;
		using reference = element_type&;

		inline constexpr UniquePtr() = default;

		inline UniquePtr& operator=(const UniquePtr&) = delete;
		inline UniquePtr(const UniquePtr&) = delete;

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02>>
		inline UniquePtr& operator=(UniquePtr<ValueType02, OtherDeleter>&& other) {
			if (this != static_cast<void*>(&other)) {
				reset(nullptr);
				try {
					commit(other.release());
				} catch (...) {
					reset(nullptr);
					throw;
				}
			}
			return *this;
		}

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02>>
		inline UniquePtr(UniquePtr<ValueType02, OtherDeleter>&& other) {
			*this = std::move(other);
		}

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02>>
		inline UniquePtr& operator=(std::unique_ptr<ValueType02, OtherDeleter>&& other) {
			reset(nullptr);
			try {
				commit(other.release());
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02>>
		inline UniquePtr(std::unique_ptr<ValueType02, OtherDeleter>&& other) {
			*this = std::move(other);
		}

		inline UniquePtr& operator=(pointer newPtr) {
			reset(nullptr);
			try {
				commit(newPtr);
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		inline UniquePtr(pointer newPtr) {
			*this = newPtr;
		}

		inline pointer get() const {
			return ptr;
		}

		inline operator bool() const {
			return ptr != nullptr;
		}

		inline reference operator*() const {
			return *ptr;
		}

		inline pointer operator->() const {
			return ptr;
		}

		inline pointer release() {
			pointer releasedPtr = ptr;
			ptr = nullptr;
			return releasedPtr;
		}

		inline void reset(pointer newPtr) {
			pointer oldPtr = std::exchange(ptr, newPtr);
			deleter_type{}(oldPtr);
		}

		inline void swap(UniquePtr& other) {
			std::swap(ptr, other.ptr);
		}

		inline ~UniquePtr() {
			reset(nullptr);
		}

	  protected:
		pointer ptr{ nullptr };

		inline void commit(pointer other) {
			pointer tempPtr = other;
			reset(tempPtr);
		}
	};

	template<typename ValueType, typename Deleter> class UniquePtr<ValueType[], Deleter> {
	  public:
		using element_type = std::remove_extent_t<ValueType>;
		using pointer = element_type*;
		using deleter_type = Deleter;
		using reference = element_type&;

		inline constexpr UniquePtr() = default;

		inline UniquePtr& operator=(const UniquePtr&) = delete;
		inline UniquePtr(const UniquePtr&) = delete;

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02[]>>
		inline UniquePtr& operator=(UniquePtr<ValueType02[], OtherDeleter>&& other) {
			if (this != static_cast<void*>(&other)) {
				reset(nullptr);
				try {
					commit(other.release());
				} catch (...) {
					reset(nullptr);
					throw;
				}
			}
			return *this;
		}

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02[]>>
		inline UniquePtr(UniquePtr<ValueType02[], OtherDeleter>&& other) {
			*this = std::move(other);
		}

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02[]>>
		inline UniquePtr& operator=(std::unique_ptr<ValueType02[], OtherDeleter>&& other) {
			reset(nullptr);
			try {
				commit(other.release());
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02[]>>
		inline UniquePtr(std::unique_ptr<ValueType02[], OtherDeleter>&& other) {
			*this = std::move(other);
		}

		inline UniquePtr& operator=(pointer newPtr) {
			reset(nullptr);
			try {
				commit(newPtr);
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		inline UniquePtr(pointer newPtr) {
			*this = newPtr;
		}

		inline pointer get() const {
			return ptr;
		}

		template<typename ValueType02 = ValueType>
		inline std::enable_if_t<std::is_array_v<ValueType02>, reference> operator[](std::ptrdiff_t index) const {
			return ptr[index];
		}

		inline operator bool() const {
			return ptr != nullptr;
		}

		inline reference operator*() const {
			return *ptr;
		}

		inline pointer operator->() const {
			return ptr;
		}

		inline pointer release() {
			pointer releasedPtr = ptr;
			ptr = nullptr;
			return releasedPtr;
		}

		inline void reset(pointer newPtr) {
			pointer oldPtr = std::exchange(ptr, newPtr);
			deleter_type{}(oldPtr);
		}

		inline void swap(UniquePtr& other) {
			std::swap(ptr, other.ptr);
		}

		inline ~UniquePtr() {
			reset(nullptr);
		}

	  protected:
		pointer ptr{ nullptr };

		inline void commit(pointer other) {
			pointer tempPtr = other;
			reset(tempPtr);
		}
	};

	template<typename ValueType, typename Deleter = std::default_delete<ValueType>, typename... ArgTypes,
		std::enable_if_t<!std::is_array_v<ValueType>, int32_t> = 0>
	inline constexpr UniquePtr<ValueType, Deleter> makeUnique(ArgTypes&&... args) {
		return UniquePtr<ValueType, Deleter>(new ValueType(std::forward<ArgTypes>(args)...));
	}

	template<typename ValueType, typename Deleter = std::default_delete<ValueType>,
		std::enable_if_t<std::is_array_v<ValueType> && std::extent_v<ValueType> == 0, int32_t> = 0>
	inline constexpr UniquePtr<ValueType, Deleter> makeUnique(const uint64_t size) {
		using element_type = std::remove_extent_t<ValueType>;
		return UniquePtr<ValueType, Deleter>(new element_type[size]());
	}

	template<typename ValueType, typename... ArgTypes, std::enable_if_t<std::extent_v<ValueType> != 0, int32_t> = 0>
	inline void makeUnique(ArgTypes&&...) = delete;

}
