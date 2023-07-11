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
/// LightString.hpp - Header for the LightString class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file LightString.hpp

#pragma once

#include <discordcoreapi/Base.hpp>

namespace DiscordCoreAPI {

	template<typename ValueType> class LightString {
	  public:
		using value_type = ValueType;
		using size_type = size_t;
		using traits_type = std::char_traits<value_type>;
		using pointer = ValueType*;
		using const_pointer = const value_type*;
		using allocator = AllocWrapper<value_type>;

		inline constexpr LightString() noexcept = default;

		inline static size_type npos{ std::numeric_limits<size_type>::max() };

		inline constexpr LightString& operator=(LightString&& other) noexcept {
			if (this != &other) {
				std::swap(capacityVal, other.capacityVal);
				std::swap(sizeVal, other.sizeVal);
				std::swap(values, other.values);
			}
			return *this;
		}

		inline constexpr LightString& operator=(const LightString& other) noexcept {
			if (this != &other) {
				auto sizeNew = other.size();
				if (sizeNew) [[likely]] {
					reserve(sizeNew);
					std::memcpy(values, other.data(), sizeNew);
					sizeVal = sizeNew;
				}
			}
			return *this;
		}

		inline LightString(const LightString& other) noexcept {
			*this = other;
		}

		inline LightString(value_type other) noexcept {
			*this = other;
		}

		inline constexpr LightString& operator=(const std::string& other) noexcept {
			auto sizeNew = other.size();
			if (sizeNew) [[likely]] {
				reserve(sizeNew);
				std::memcpy(values, other.data(), sizeNew);
				sizeVal = sizeNew;
			}
			return *this;
		}

		inline LightString(const std::string& other) noexcept {
			*this = other;
		}

		inline constexpr LightString& operator=(const std::string_view& other) noexcept {
			auto sizeNew = other.size();
			if (sizeNew) [[likely]] {
				reserve(sizeNew);
				std::memcpy(values, other.data(), sizeNew);
				sizeVal = sizeNew;
			}
			return *this;
		}

		inline LightString(const std::string_view& other) noexcept {
			*this = other;
		}

		template<size_t strLength> inline constexpr LightString(const char (&other)[strLength]) {
			if (strLength) [[likely]] {
				reserve(strLength);
				std::memcpy(values, other, strLength);
				sizeVal = strLength;
			}
		}

		inline constexpr LightString& operator=(const_pointer other) noexcept {
			auto sizeNew = traits_type::length(other);
			if (sizeNew) [[likely]] {
				reserve(sizeNew);
				std::memcpy(values, other, sizeNew);
				sizeVal = sizeNew;
			}
			return *this;
		}

		inline LightString(const_pointer other) noexcept {
			*this = other;
		}

		inline LightString(const_pointer other, size_t sizeNew) noexcept {
			if (sizeNew) [[likely]] {
				reserve(sizeNew);
				std::memcpy(values, other, sizeNew);
				sizeVal = sizeNew;
			}
		}

		inline constexpr void insert(const value_type* newValues, size_type position) {
			auto amount = traits_type::length(newValues);
			if (position > 0 && amount > 0) {
				size_type sizeNew = sizeVal + amount;
				if (sizeNew + 1 >= capacityVal) {
					reserve(sizeNew + 1);
				}
				std::memcpy(values + amount + position, values + position, sizeVal - position);
				std::memcpy(values + position, newValues, amount);
				sizeVal = sizeNew;
				allocator alloc{};
				alloc.construct(&values[sizeVal], '\0');
			}
		}

		inline constexpr value_type& operator[](size_type index) const noexcept {
			return values[index];
		}

		inline constexpr const value_type* c_str() const noexcept {
			return values;
		}

		inline constexpr size_type capacity() const noexcept {
			return capacityVal;
		}

		inline constexpr void writeData(const value_type* valuesNew, uint64_t sizeNew) noexcept {
			if (sizeVal + sizeNew > capacityVal) {
				reserve(sizeVal + sizeNew + 1);
			}
			std::memcpy(values + sizeVal, valuesNew, sizeNew);
			sizeVal += sizeNew;
		}

		inline constexpr void erase(size_type count, size_type pos = 0) {
			if (pos >= sizeVal) {
				return;
			}
			count = std::min(count, sizeVal - pos);
			if (count > 0) {
				traits_type::move(values + pos, values + pos + count, sizeVal - pos - count);
				sizeVal -= count;
				values[sizeVal] = '\0';
			}
		}

		inline std::string_view stringView(size_t offset, size_t size) {
			return { values + offset, size };
		}

		inline constexpr size_type size() const noexcept {
			return sizeVal;
		}

		inline constexpr size_type maxSize() const noexcept {
			return std::numeric_limits<uint64_t>::max();
		}

		inline operator std::string() const noexcept {
			return { values, sizeVal };
		}

		inline constexpr value_type* data() const noexcept {
			return values;
		}

		inline constexpr void push_back(value_type c) {
			if (sizeVal + 1 >= capacityVal) {
				reserve(capacityVal * 2 + 2);
			}
			allocator alloc{};
			alloc.construct(&values[sizeVal++], c);
			alloc.construct(&values[sizeVal], '\0');
		}

		inline constexpr void clear() {
			sizeVal = 0;
		}

		inline constexpr operator std::string_view() noexcept {
			return { values, sizeVal };
		}

		inline constexpr void resize(size_type sizeNew) {
			if (sizeNew > 0) {
				if (sizeNew > capacityVal) {
					allocator alloc{};
					pointer newPtr = alloc.allocate(sizeNew + 1);
					try {
						if (values) {
							std::uninitialized_move(values, values + sizeVal, newPtr);
							alloc.deallocate(values, capacityVal + 1);
						}
					} catch (...) {
						alloc.deallocate(newPtr, sizeNew + 1);
						throw;
					}
					capacityVal = sizeNew;
					values = newPtr;
					std::uninitialized_default_construct(values + sizeVal, values + sizeVal + (sizeNew - sizeVal));
				} else if (sizeNew > sizeVal) {
					std::uninitialized_default_construct(values + sizeVal, values + sizeVal + (sizeNew - sizeVal));
				}
				sizeVal = sizeNew;
				values[sizeVal] = '\0';
			} else {
				sizeVal = 0;
			}
		}

		inline constexpr void reserve(size_type capacityNew) {
			if (capacityNew > capacityVal) {
				allocator alloc{};
				pointer newPtr = alloc.allocate(capacityNew + 1);
				try {
					if (values) {
						std::uninitialized_move(values, values + sizeVal, newPtr);
						alloc.deallocate(values, capacityVal + 1);
					}
				} catch (...) {
					alloc.deallocate(newPtr, capacityNew + 1);
					throw;
				}
				capacityVal = capacityNew;
				values = newPtr;
			}
		}

		inline constexpr bool operator==(const pointer rhs) const noexcept {
			if (traits_type::length(rhs) != size()) {
				return false;
			}
			return JsonifierInternal::JsonifierCoreInternal::compare(rhs, data(), size());
		}

		template<size_t strLength> inline constexpr bool operator==(const char (&other)[strLength]) {
			if (strLength != size()) {
				return false;
			}
			return JsonifierInternal::JsonifierCoreInternal::compare(other, data(), size());
		}

		template<typename ValueType02> inline constexpr std::enable_if_t<
			std::convertible_to<ValueType02, LightString> && !std::is_pointer_v<ValueType02> && !std::is_array_v<ValueType02>, bool>
		operator==(const ValueType02& rhs) const noexcept {
			if (rhs.size() != size()) {
				return false;
			}
			return JsonifierInternal::JsonifierCoreInternal::compare(rhs.data(), data(), rhs.size());
		}

		template<typename ValueType02> inline constexpr bool operator!=(const ValueType02& rhs) const noexcept {
			return !(*this == rhs);
		}

		inline constexpr LightString& operator+=(const LightString& rhs) noexcept {
			auto oldSize = size();
			resize(oldSize + rhs.size());
			std::copy(rhs.data(), rhs.data() + rhs.size(), data() + oldSize);
			return *this;
		}

		inline constexpr LightString& operator+=(const value_type* rhs) noexcept {
			auto rhsSize = traits_type::length(rhs);
			auto oldSize = size();
			resize(oldSize + rhsSize);
			std::copy(rhs, rhs + rhsSize, data() + oldSize);
			return *this;
		}

		inline constexpr ~LightString() {
			if (values && capacityVal) {
				allocator alloc{};
				alloc.deallocate(values, capacityVal + 1);
				values = nullptr;
				capacityVal = 0;
			}
		};

	  protected:
		size_type capacityVal{};
		size_type sizeVal{};
		pointer values{};
	};

	template<typename value_type> inline std::ostream& operator<<(std::ostream& os, const LightString<value_type>& string) {
		os << string.operator typename std::string();
		return os;
	}

	inline LightString<char> operator+(const char* lhs, const LightString<char>& rhs) noexcept {
		LightString<char> newString(lhs);
		newString += rhs;
		return newString;
	}

	template<typename ValueType> inline LightString<char> operator+(const ValueType& lhs, const LightString<char>& rhs) noexcept {
		LightString<char> newString(lhs);
		newString += rhs;
		return newString;
	}

	using String = LightString<char>;

}
