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

#include <string>
#include <jsonifier/Vector.hpp>

namespace DiscordCoreAPI {

	template<typename ValueType> class LightString : public JsonifierInternal::AllocWrapper<ValueType> {
	  public:
		using value_type = ValueType;
		using traits_type = std::char_traits<value_type>;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = JsonifierInternal::Iterator<value_type>;
		using const_iterator = const iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = const reverse_iterator;
		using size_type = uint64_t;
		using difference_type = ptrdiff_t;
		using allocator = JsonifierInternal::AllocWrapper<value_type>;

		inline constexpr LightString() noexcept = default;

		static constexpr size_type npos{ std::numeric_limits<size_type>::max() };

		inline constexpr LightString& operator=(LightString&& other) noexcept {
			if (this != &other) {
				swap(other);
			}
			return *this;
		}

		inline explicit LightString(LightString&& other) noexcept {
			*this = std::move(other);
		}

		inline LightString& operator=(const LightString& other) {
			if (this != &other) {
				size_type sizeNew = other.size();
				if (sizeNew > 0) {
					LightString temp{};
					temp.values = allocator::allocate(sizeNew + 1);
					std::memcpy(temp.values, other.data(), sizeNew);
					temp.capacityVal = sizeNew;
					temp.sizeVal = sizeNew;
					swap(temp);
				}
			}
			return *this;
		}

		inline LightString(const LightString& other) noexcept : JsonifierInternal::AllocWrapper<value_type>{} {
			*this = other;
		}

		inline LightString(value_type other) noexcept {
			*this = other;
		}

		inline LightString& operator=(const std::string& other) noexcept {
			size_type sizeNew = other.size();
			if (sizeNew > 0) {
				LightString temp{};
				temp.values = allocator::allocate(sizeNew + 1);
				std::memcpy(temp.values, other.data(), sizeNew);
				temp.capacityVal = sizeNew;
				temp.sizeVal = sizeNew;
				swap(temp);
			}
			return *this;
		}

		inline LightString(const std::string& other) noexcept {
			*this = other;
		}

		inline constexpr LightString& operator=(const std::string_view& other) noexcept {
			size_type sizeNew = other.size();
			if (sizeNew > 0) {
				LightString temp{};
				temp.values = allocator::allocate(sizeNew + 1);
				std::memcpy(temp.values, other.data(), sizeNew);
				temp.capacityVal = sizeNew;
				temp.sizeVal = sizeNew;
				swap(temp);
			}
			return *this;
		}

		inline LightString(const std::string_view& other) noexcept {
			*this = other;
		}

		template<uint64_t strLength> inline constexpr LightString(const char (&other)[strLength]) {
			if (strLength > 0) {
				LightString temp{};
				temp.values = allocator::allocate(strLength + 1);
				std::memcpy(temp.values, other, strLength);
				temp.capacityVal = strLength;
				temp.sizeVal = strLength;
				swap(temp);
			}
		}

		inline constexpr LightString& operator=(const_pointer other) noexcept {
			auto sizeNew = traits_type::length(other);
			if (sizeNew) [[likely]] {
				LightString temp{};
				temp.values = allocator::allocate(sizeNew + 1);
				std::memcpy(temp.values, other, sizeNew);
				temp.capacityVal = sizeNew;
				temp.sizeVal = sizeNew;
				swap(temp);
			}
			return *this;
		}

		inline LightString(const_pointer other) noexcept {
			*this = other;
		}

		inline LightString(const_pointer other, uint64_t sizeNew) noexcept {
			if (sizeNew) [[likely]] {
				LightString temp{};
				temp.values = allocator::allocate(sizeNew + 1);
				std::memcpy(temp.values, other, sizeNew);
				temp.capacityVal = sizeNew;
				temp.sizeVal = sizeNew;
				swap(temp);
			}
		}

		inline LightString substr(size_type pos, size_type count) {
			if (pos + count >= sizeVal) {
				return LightString{};
			}
			LightString result{};
			result.reserve(count);
			std::memcpy(result.values, values + pos, count * sizeof(value_type));
			result.sizeVal = count;
			allocator::construct(&result.values[count], '\0');
			return result;
		}

		inline constexpr const_iterator begin() const noexcept {
			return const_iterator(values, sizeVal, 0);
		}

		inline constexpr const_iterator end() const noexcept {
			return const_iterator(values, sizeVal, sizeVal);
		}

		inline constexpr iterator begin() noexcept {
			return iterator(values, sizeVal, 0);
		}

		inline constexpr iterator end() noexcept {
			return iterator(values, sizeVal, sizeVal);
		}

		inline constexpr void writeData(const_pointer valuesNew, uint64_t sizeNew) {
			if (sizeVal + sizeNew > capacityVal) {
				reserve(sizeVal + sizeNew + 1);
			}
			std::memcpy(values + sizeVal, valuesNew, sizeNew);
			sizeVal += sizeNew;
		}

		inline constexpr void erase(size_type count) {
			if (count > sizeVal || count == 0) {
				return;
			}
			traits_type::move(values, values + count, sizeVal - count);
			sizeVal -= count;
			values[sizeVal] = '\0';
		}

		inline void pushBack(value_type value) {
			if (sizeVal + 1 >= capacityVal) {
				reserve((sizeVal + 2) * 4);
			}
			allocator::construct(&values[sizeVal++], value);
			allocator::construct(&values[sizeVal], '\0');
		}

		inline constexpr reference at(size_type index) const {
			if (index >= sizeVal) {
				throw std::runtime_error{ "Sorry, but that index is beyond the end of this string." };
			}
			return values[index];
		}

		inline constexpr value_type& operator[](size_type index) noexcept {
			return values[index];
		}

		inline std::string_view stringView(size_type offSet, size_type count) const noexcept {
			return std::string_view{ data() + offSet, count };
		}

		inline operator std::string_view() const noexcept {
			return std::string_view{ data(), size() };
		}

		inline operator std::string() const noexcept {
			return std::string{ data(), size() };
		}

		inline constexpr void clear() noexcept {
			sizeVal = 0;
		}

		inline constexpr size_type maxSize() const {
			return std::numeric_limits<size_type>::max();
		}

		inline void resize(size_type sizeNew) {
			if (sizeNew > 0) [[likely]] {
				if (sizeNew > capacityVal) [[likely]] {
					pointer newPtr = allocator::allocate(sizeNew + 1);
					try {
						if (values) [[likely]] {
							std::uninitialized_move(values, values + sizeVal, newPtr);
							allocator::deallocate(values, capacityVal + 1);
						}
					} catch (...) {
						allocator::deallocate(newPtr, sizeNew + 1);
						throw;
					}
					capacityVal = sizeNew;
					values = newPtr;
					std::uninitialized_default_construct(values + sizeVal, values + sizeVal + (sizeNew - sizeVal));
				} else if (sizeNew > sizeVal) [[unlikely]] {
					std::uninitialized_default_construct(values + sizeVal, values + sizeVal + (sizeNew - sizeVal));
				}
				sizeVal = sizeNew;
				values[sizeVal] = '\0';
			} else {
				sizeVal = 0;
			}
		}

		inline void reserve(size_type capacityNew) {
			if (capacityNew > capacityVal) [[likely]] {
				pointer newPtr = allocator::allocate(capacityNew + 1);
				try {
					if (values) [[likely]] {
						std::uninitialized_move(values, values + sizeVal, newPtr);
						allocator::deallocate(values, capacityVal + 1);
					}
				} catch (...) {
					allocator::deallocate(newPtr, capacityNew + 1);
					throw;
				}
				capacityVal = capacityNew;
				values = newPtr;
			}
		}

		inline constexpr size_type capacity() const noexcept {
			return capacityVal;
		}

		inline constexpr size_type size() const noexcept {
			return sizeVal;
		}

		inline constexpr bool empty() const noexcept {
			return sizeVal == 0;
		}

		inline constexpr pointer data() const noexcept {
			return values;
		}

		inline constexpr bool operator==(const pointer rhs) const {
			if (traits_type::length(rhs) != size()) {
				return false;
			}
			return JsonifierInternal::JsonifierCoreInternal::compare(rhs, data(), size());
		}

		template<uint64_t strLength> inline constexpr bool operator==(const char (&other)[strLength]) {
			if (strLength != size()) {
				return false;
			}
			return JsonifierInternal::JsonifierCoreInternal::compare(other, data(), size());
		}

		template<typename ValueType02> inline constexpr std::enable_if_t<
			std::convertible_to<ValueType02, LightString> && !std::is_pointer_v<ValueType02> && !std::is_array_v<ValueType02>, bool>
		operator==(const ValueType02& rhs) const {
			if (rhs.size() != size()) {
				return false;
			}
			return JsonifierInternal::JsonifierCoreInternal::compare(rhs.data(), data(), rhs.size());
		}

		inline void swap(LightString& other) {
			std::swap(capacityVal, other.capacityVal);
			std::swap(sizeVal, other.sizeVal);
			std::swap(values, other.values);
		}

		inline LightString& operator+=(const LightString& rhs) noexcept {
			auto oldSize = size();
			resize(oldSize + rhs.size());
			std::copy(rhs.data(), rhs.data() + rhs.size(), data() + oldSize);
			return *this;
		}

		inline LightString& operator+=(const_pointer rhs) noexcept {
			auto oldSize = size();
			auto rhsSize = traits_type::length(rhs);
			resize(oldSize + rhsSize);
			std::copy(rhs, rhs + rhsSize, data() + oldSize);
			return *this;
		}

		inline LightString operator+(const LightString& rhs) noexcept {
			LightString newString(*this);
			newString += rhs;
			return newString;
		}

		inline LightString operator+(const_pointer rhs) noexcept {
			LightString newString(*this);
			newString += rhs;
			return newString;
		}

		inline friend LightString operator+(const std::string& lhs, const LightString& rhs) {
			LightString newString{ lhs };
			newString += rhs;
			return newString;
		}

		inline ~LightString() {
			if (values && capacityVal > 0) {
				allocator::deallocate(values, capacityVal + 1);
				values = nullptr;
				capacityVal = 0;
			}
		};

	  protected:
		size_type capacityVal{};
		size_type sizeVal{};
		pointer values{};
	};

	template<typename ValueType, typename Traits, typename SizeType> std::basic_ostream<ValueType, Traits>& insertString(
		std::basic_ostream<ValueType, Traits>& oStream, const ValueType* const data, const SizeType size) {
		using OstreamType = std::basic_ostream<ValueType, Traits>;
		typename OstreamType::iostate state = OstreamType::goodbit;

		SizeType pad;
		if (oStream.width() <= 0 || static_cast<SizeType>(oStream.width()) <= size) {
			pad = 0;
		} else {
			pad = static_cast<SizeType>(oStream.width()) - size;
		}

		const typename OstreamType::sentry ok(oStream);

		if (!ok) {
			state |= OstreamType::badbit;
		} else {
			try {
				if ((oStream.flags() & OstreamType::adjustfield) != OstreamType::left) {
					for (; 0 < pad; --pad) {
						if (Traits::eq_int_type(Traits::eof(), oStream.rdbuf()->sputc(oStream.fill()))) {
							state |= OstreamType::badbit;
							break;
						}
					}
				}

				if (state == OstreamType::goodbit &&
					oStream.rdbuf()->sputn(data, static_cast<std::streamsize>(size)) != static_cast<std::streamsize>(size)) {
					state |= OstreamType::badbit;
				} else {
					for (; 0 < pad; --pad) {
						if (Traits::eq_int_type(Traits::eof(), oStream.rdbuf()->sputc(oStream.fill()))) {
							state |= OstreamType::badbit;
							break;
						}
					}
				}
				oStream.width(0);
			} catch (...) {
				oStream.setstate(OstreamType::badbit);
			}
		}
		oStream.setstate(state);
		return oStream;
	}

	using String = LightString<char>;

	inline std::basic_ostream<typename LightString<char>::value_type, typename LightString<char>::traits_type>& operator<<(
		std::basic_ostream<typename LightString<char>::value_type, typename LightString<char>::traits_type>& oStream,
		const LightString<char>& string) {
		return insertString<typename LightString<char>::value_type, typename LightString<char>::traits_type>(oStream, string.data(), string.size());
	}

	inline LightString<char> operator+(const char* lhs, const LightString<char>& rhs) noexcept {
		LightString<char> newString(lhs);
		newString += rhs;
		return newString;
	}
}// namespace DiscordCoreAPI
