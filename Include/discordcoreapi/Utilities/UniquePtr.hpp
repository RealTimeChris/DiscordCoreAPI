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
/// UniquePtr.hpp - Header for the "UniquePtr" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file UniquePtr.hpp

#pragma once

#include <discordcoreapi/Base.hpp>

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
		using const_reference = const element_type&;

		inline constexpr UniquePtr() noexcept = default;

		inline UniquePtr& operator=(const UniquePtr&) noexcept = delete;
		inline UniquePtr(const UniquePtr&) noexcept = delete;

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02>>
		inline UniquePtr& operator=(UniquePtr<ValueType02, OtherDeleter>&& other) {
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

		inline UniquePtr(pointer newPtr) noexcept {
			*this = newPtr;
		}

		inline pointer get() const noexcept {
			return ptr;
		}

		inline operator bool() const noexcept {
			return ptr != nullptr;
		}

		inline reference operator*() const noexcept {
			return *ptr;
		}

		inline pointer operator->() const noexcept {
			return ptr;
		}

		inline pointer release() noexcept {
			pointer releasedPtr = ptr;
			ptr = nullptr;
			return releasedPtr;
		}

		inline void reset(pointer newPtr) noexcept {
			pointer oldPtr = std::exchange(ptr, newPtr);
			Deleter{}(oldPtr);
		}

		inline void swap(UniquePtr& other) noexcept {
			std::swap(ptr, other.ptr);
		}

		inline ~UniquePtr() noexcept {
			reset(nullptr);
		}

	  private:
		inline void commit(pointer other) noexcept {
			pointer tempPtr = other;
			reset(tempPtr);
		}

		pointer ptr{ nullptr };
	};

	template<typename ValueType, typename Deleter> class UniquePtr<ValueType[], Deleter> {
	  public:
		using element_type = std::remove_extent_t<ValueType>;
		using pointer = element_type*;
		using deleter_type = Deleter;
		using reference = element_type&;
		using const_reference = const element_type&;

		inline constexpr UniquePtr() noexcept = default;

		inline UniquePtr& operator=(const UniquePtr&) noexcept = delete;
		inline UniquePtr(const UniquePtr&) noexcept = delete;

		template<IsRelatedPtr<element_type> ValueType02, typename OtherDeleter = std::default_delete<ValueType02[]>>
		inline UniquePtr& operator=(UniquePtr<ValueType02[], OtherDeleter>&& other) {
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

		inline UniquePtr(pointer newPtr) noexcept {
			*this = newPtr;
		}

		inline pointer get() const noexcept {
			return ptr;
		}

		template<typename ValueType02 = ValueType>
		inline std::enable_if_t<std::is_array_v<ValueType02>, reference> operator[](std::ptrdiff_t index) const noexcept {
			return ptr[index];
		}

		inline operator bool() const noexcept {
			return ptr != nullptr;
		}

		inline reference operator*() const noexcept {
			return *ptr;
		}

		inline pointer operator->() const noexcept {
			return ptr;
		}

		inline pointer release() noexcept {
			pointer releasedPtr = ptr;
			ptr = nullptr;
			return releasedPtr;
		}

		inline void reset(pointer newPtr) noexcept {
			pointer oldPtr = std::exchange(ptr, newPtr);
			Deleter{}(oldPtr);
		}

		inline void swap(UniquePtr& other) noexcept {
			std::swap(ptr, other.ptr);
		}

		inline ~UniquePtr() noexcept {
			reset(nullptr);
		}

	  private:
		inline void commit(pointer other) noexcept {
			pointer tempPtr = other;
			reset(tempPtr);
		}

		pointer ptr{ nullptr };
	};

	template<class ValueType, typename Deleter = std::default_delete<ValueType>, typename... Args,
		std::enable_if_t<!std::is_array_v<ValueType>, int> = 0>
	inline constexpr UniquePtr<ValueType, Deleter> makeUnique(Args&&... args) {
		return UniquePtr<ValueType, Deleter>(new ValueType(std::forward<Args>(args)...));
	}

	template<class ValueType, typename Deleter = std::default_delete<ValueType>,
		std::enable_if_t<std::is_array_v<ValueType> && std::extent_v<ValueType> == 0, int> = 0>
	inline constexpr UniquePtr<ValueType, Deleter> makeUnique(const size_t count) {
		using element_type = std::remove_extent_t<ValueType>;
		return UniquePtr<ValueType, Deleter>(new element_type[count]());
	}
}
