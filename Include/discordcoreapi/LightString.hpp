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
/// LightString.hpp - Header for the LightString class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file LightString.hpp

#pragma once

#include <discordcoreapi/Base.hpp>

namespace DiscordCoreAPI {

	inline static constexpr size_t BufferSize = 16 / sizeof(char) < 1 ? 1 : 16 / sizeof(char);

	inline static constexpr size_t AllocationMask = sizeof(char) <= 1 ? 15
		: sizeof(char) <= 2											  ? 7
		: sizeof(char) <= 4											  ? 3
		: sizeof(char) <= 8											  ? 1
																	  : 0;

	template<typename OTy> class LightString {
	  public:
		using value_type = OTy;
		using size_type = size_t;
		using traits_type = std::char_traits<OTy>;
		using pointer = OTy*;
		using const_pointer = const OTy*;
		using allocator = std::allocator<OTy>;

		inline constexpr LightString() noexcept = default;

		inline static size_t npos{ std::numeric_limits<size_t>::max() };

		inline constexpr LightString& operator=(LightString&& other) noexcept {
			if (this != &other) {
				std::swap(capacityVal, other.capacityVal);
				std::swap(sizeVal, other.sizeVal);
				std::swap(values, other.values);
			}
			return *this;
		}

		inline constexpr LightString(LightString&& other) noexcept {
			*this = std::move(other);
		}

		inline constexpr LightString& operator=(const LightString& other) noexcept {
			if (this != &other) {
				if (other.size() > 0) {
					reserve(other.size());
					std::memcpy(values, other.values, capacityVal);
					sizeVal = other.size();
				}
			}
			return *this;
		}

		inline constexpr LightString(const LightString& other) noexcept {
			*this = other;
		}

		inline constexpr LightString& operator=(OTy other) noexcept {
			if (other == '\0') {
				push_back(' ');
			} else {
				push_back(other);
			}
			return *this;
		}

		inline constexpr void insert(const OTy* newValues, size_t position) {
			auto amount = traits_type::length(newValues);
			if (position > 0 && amount > 0) {
				size_t newSize = sizeVal + amount;
				if (newSize + 1 >= capacityVal) {
					reserve(newSize + 1);
				}
				std::memcpy(values + amount + position, values + position, sizeVal - position);
				std::memcpy(values + position, newValues, amount);
				sizeVal = newSize;
				values[sizeVal] = '\0';
			}
		}

		inline constexpr LightString(OTy other) noexcept {
			*this = other;
		}

		inline LightString& operator=(const std::string& other) noexcept {
			if (other.size() > 0) {
				reserve(other.size());
				std::memcpy(values, other.data(), capacityVal);
				sizeVal = other.size();
			}
			return *this;
		}

		inline LightString(const std::string& other) noexcept {
			*this = other;
		}

		inline constexpr LightString& operator=(const std::string_view& other) noexcept {
			if (other.size() > 0) {
				reserve(other.size());
				std::memcpy(values, other.data(), capacityVal);
				sizeVal = other.size();
			}
			return *this;
		}

		inline constexpr LightString(const std::string_view& other) noexcept {
			*this = other;
		}

		inline constexpr LightString& operator=(const OTy* other) noexcept {
			auto newSize = std::char_traits<OTy>::length(other);
			if (newSize > 0) {
				reserve(newSize);
				std::memcpy(values, other, capacityVal);
				sizeVal = newSize;
			}
			return *this;
		}

		inline constexpr explicit LightString(const OTy* other) noexcept {
			*this = other;
		}

		inline constexpr OTy& operator[](size_t index) const noexcept {
			return values[index];
		}

		inline constexpr const OTy* c_str() const noexcept {
			return values;
		}

		inline constexpr size_t capacity() const noexcept {
			return capacityVal;
		}

		inline constexpr size_t size() const noexcept {
			return sizeVal;
		}

		inline constexpr size_t maxSize() const noexcept {
			return std::numeric_limits<uint64_t>::max();
		}

		inline operator std::string() const noexcept {
			return { values, sizeVal };
		}

		inline constexpr OTy* data() const noexcept {
			return values;
		}

		inline constexpr void push_back(value_type c) {
			if (sizeVal + 1 >= capacityVal) {
				reserve(capacityVal * 2 + 2);
			}
			values[sizeVal++] = c;
			values[sizeVal] = '\0';
		}

		inline constexpr void clear() {
			sizeVal = 0;
		}

		inline constexpr operator std::string_view() noexcept {
			return { values, sizeVal };
		}

		inline constexpr LightString& append(const_pointer chars, size_type count) {
			if (count > 0) {
				if (size() + count >= capacity()) {
					reserve(capacity() + count);
				}
				std::memcpy(values + size(), chars, count);
				sizeVal += count;
				values[sizeVal++] = '\0';
			}
			return *this;
		}

		inline constexpr LightString& append(const size_type count, const value_type newChar) {
			const size_type oldSize = size();
			if (count <= capacity() - oldSize) {
				sizeVal = oldSize + count;
				pointer const old = values;
				traits_type::assign(old + oldSize, count, newChar);
				traits_type::assign(old[oldSize + count], value_type{});
				return *this;
			}
			return reallocateGrowBy(
				count,
				[](pointer const newVal, const pointer old, const size_type oldSize, const size_type count, const value_type newChar) {
					traits_type::copy(newVal, old, oldSize);
					traits_type::assign(newVal + oldSize, count, newChar);
					traits_type::assign(newVal[oldSize + count], value_type{});
				},
				count, newChar);
		}

		inline constexpr auto begin() noexcept {
			return values;
		}

		inline constexpr auto end() noexcept {
			return values + sizeVal;
		}

		inline constexpr void resize(const size_type newSize) {
			const size_type oldSize = size();
			if (newSize <= oldSize) {
				traits_type::assign(values[sizeVal = newSize], value_type{});
			} else {
				append(newSize - oldSize, value_type{});
			}
		}

		inline constexpr void writeData(const OTy* values, uint64_t sizeNew) noexcept {
			if (sizeVal + sizeNew > capacityVal) {
				reserve(capacityVal + sizeNew);
			}
			std::memcpy(data() + sizeVal, values, sizeNew);
			sizeVal += sizeNew;
		}

		inline constexpr std::basic_string_view<OTy> stringView(size_t offSet, size_t length) {
			return std::basic_string_view<OTy>{ values + offSet, length };
		}

		inline constexpr void reserve(size_t newCapacity) {
			if (newCapacity > capacityVal) {
				pointer newBuffer = allocator{}.allocate(newCapacity + 1);
				std::memcpy(newBuffer, values, sizeVal);
				newBuffer[sizeVal] = '\0';
				allocator{}.deallocate(values, capacityVal);
				values = newBuffer;
				capacityVal = newCapacity;
			}
		}

		inline constexpr void erase(size_type count, size_type pos = 0) {
			if (pos >= sizeVal) {
				return;
			}

			count = (std::min)(count, sizeVal - pos);

			if (count > 0) {
				traits_type::move(values + pos, values + pos + count, sizeVal - pos - count);
				sizeVal -= count;
				values[sizeVal] = '\0';
			}
		}

		inline constexpr ~LightString(){};

	  protected:
		size_type capacityVal{};
		size_type sizeVal{};
		pointer values{};

		template<class ValueType, class... Types>
		inline constexpr void constructInPlace(ValueType& object, Types&&... args) noexcept(std::is_nothrow_constructible_v<ValueType, Types...>) {
			if (std::is_constant_evaluated()) {
				std::construct_at(std::addressof(object), std::forward<Types>(args)...);
			} else {
				::new (static_cast<void*>(std::addressof(object))) ValueType(std::forward<Types>(args)...);
			}
		}

		inline constexpr size_type calculateGrowth(const size_type requested) noexcept {
			const size_type masked = requested | AllocationMask;
			if (masked > maxSize()) {
				return maxSize();
			}

			if (capacity() > maxSize() - capacity() / 2) {
				return maxSize();
			}

			return (std::max)(masked, capacity() + capacity() / 2);
		}

		template<class FTy, class... ArgTypes>
		inline constexpr LightString& reallocateGrowBy(const size_type sizeIncrease, FTy function, ArgTypes... args) {
			auto& myData = values;
			const size_type oldSize = size();
			if (maxSize() - oldSize < sizeIncrease) {
				throw "String too long.";
			}

			const size_type newSize = oldSize + sizeIncrease;
			const size_type oldCapacity = capacityVal;
			const size_type newCapacity = calculateGrowth(newSize);
			auto alloc = allocator{};
			const pointer newVal = allocator{}.allocate(newCapacity + 1);

			if (std::is_constant_evaluated()) {
				traits_type::assign(newVal, newCapacity + 1, value_type{});
			}

			sizeVal = newSize;
			capacityVal = newCapacity;
			pointer const rawNew = newVal;
			if (BufferSize <= oldCapacity) {
				const pointer old = values;
				function(rawNew, old, oldSize, args...);
				allocator{}.deallocate(old, oldCapacity + 1);
				values = newVal;
			} else {
				function(rawNew, values, oldSize, args...);
				constructInPlace(values, newVal);
			}
			return *this;
		}
	};

	template<typename OTy> inline std::ostream& operator<<(std::ostream& os, const LightString<OTy>& string) {
		os << string.operator typename std::string();
		return os;
	}

	using String = LightString<char>;

}
