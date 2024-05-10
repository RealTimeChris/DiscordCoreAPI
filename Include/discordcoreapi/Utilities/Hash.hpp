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
/// Hash.hpp - Header file for the hash related stuff.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Hash.hpp
#pragma once

#include <memory_resource>
#include <exception>
#include <utility>
#include <vector>

namespace discord_core_api {

	namespace discord_core_internal {
		struct event_delegate_token;
	}

	class guild_member_cache_data;
	struct voice_state_data_light;
	struct voice_state_data;
	class guild_member_data;

	template<typename value_type>
	concept voice_state_t = std::same_as<value_type, voice_state_data_light>;

	template<typename value_type>
	concept guild_member_t = std::same_as<value_type, guild_member_cache_data> || std::same_as<value_type, guild_member_data>;

	template<typename value_type>
	concept has_id = requires(value_type value) { value.id; };

	template<typename value_type>
	concept has_two_id = voice_state_t<value_type> || guild_member_t<value_type>;

	template<typename value_type>
	concept event_delegate_token_t = std::same_as<value_type, discord_core_internal::event_delegate_token>;

	struct object_compare {
		template<typename value_type01, typename value_type02> DCA_INLINE bool operator()(const value_type01& lhs, const value_type02& rhs) {
			return lhs == static_cast<value_type01>(rhs);
		}

		template<typename value_type01> DCA_INLINE bool operator()(const value_type01& lhs, const value_type01& rhs) {
			return lhs == rhs;
		}
	};

	DCA_INLINE uint64_t internalHashFunction(const void* value, uint64_t count) {
		static constexpr uint64_t fnvOffsetBasis{ 0xcbf29ce484222325 };
		static constexpr uint64_t fnvPrime{ 0x00000100000001B3 };
		uint64_t hash{ fnvOffsetBasis };
		for (uint64_t x = 0; x < count; ++x) {
			hash ^= static_cast<const uint8_t*>(value)[x];
			hash *= fnvPrime;
		}
		return hash;
	}

	class two_id_key {
	  public:
		template<guild_member_t value_type> two_id_key(const value_type& other);
		template<voice_state_t value_type> two_id_key(const value_type& other);

		snowflake idOne{};
		snowflake idTwo{};
	};

	template<typename value_type> struct key_hasher;

	template<has_id value_type> struct key_hasher<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return internalHashFunction(&other.id.operator const uint64_t&(), sizeof(uint64_t));
		}
	};

	template<> struct key_hasher<const char*> {
		DCA_INLINE static uint64_t getHashKey(const char* other) {
			return internalHashFunction(other, std::char_traits<char>::length(other));
		}
	};

	template<uint64_t size> struct key_hasher<char[size]> {
		DCA_INLINE static uint64_t getHashKey(const char (&other)[size]) {
			return internalHashFunction(other, std::char_traits<char>::length(other));
		}
	};

	template<jsonifier::concepts::integer_t value_type> struct key_hasher<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return internalHashFunction(&other, sizeof(other));
		}
	};

	template<> struct key_hasher<two_id_key> {
		DCA_INLINE static uint64_t getHashKey(const two_id_key& other) {
			uint64_t values[2]{};
			values[0] = other.idOne.operator const uint64_t&();
			values[1] = other.idTwo.operator const uint64_t&();
			return internalHashFunction(values, sizeof(uint64_t) * std::size(values));
		}
	};

	template<jsonifier::concepts::enum_t value_type> struct key_hasher<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return internalHashFunction(&other, sizeof(other));
		}
	};

	template<jsonifier::concepts::string_t value_type> struct key_hasher<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return internalHashFunction(other.data(), other.size());
		}
	};

	template<> struct key_hasher<snowflake> {
		DCA_INLINE static uint64_t getHashKey(const snowflake& data) {
			return internalHashFunction(&data.operator const uint64_t&(), sizeof(uint64_t));
		}
	};

	template<> struct key_hasher<jsonifier::vector<jsonifier::string>> {
		DCA_INLINE static uint64_t getHashKey(const jsonifier::vector<jsonifier::string>& data) {
			jsonifier::string newString{};
			for (auto& value: data) {
				newString.append(value);
			}
			return internalHashFunction(newString.data(), newString.size());
		}
	};

	template<has_two_id value_type> struct key_hasher<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return key_hasher<two_id_key>::getHashKey(two_id_key{ other });
		}
	};

	template<jsonifier::concepts::unique_ptr_t value_type> struct key_hasher<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return key_hasher<typename value_type::element_type>::getHashKey(*other);
		}
	};

	template<typename value_Type> struct key_accessor;

	template<voice_state_t value_type> struct key_accessor<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return key_hasher<value_type>::getHashKey(other);
		}
	};

	template<> struct key_accessor<snowflake> {
		DCA_INLINE static uint64_t getHashKey(const snowflake& other) {
			return key_hasher<snowflake>::getHashKey(other);
		}
	};

	template<> struct key_accessor<two_id_key> {
		DCA_INLINE static uint64_t getHashKey(const two_id_key& other) {
			return key_hasher<two_id_key>::getHashKey(other);
		}
	};

	template<> struct key_accessor<const char*> {
		DCA_INLINE static uint64_t getHashKey(const char* other) {
			return key_hasher<const char*>::getHashKey(other);
		}
	};

	template<uint64_t size> struct key_accessor<char[size]> {
		DCA_INLINE static uint64_t getHashKey(const char (&other)[size]) {
			return key_hasher<char[size]>::getHashKey(other);
		}
	};

	template<jsonifier::concepts::string_t value_type> struct key_accessor<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return key_hasher<value_type>::getHashKey(other);
		}
	};

	template<has_id value_type> struct key_accessor<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return key_hasher<value_type>::getHashKey(other);
		}
	};

	template<jsonifier::concepts::unique_ptr_t value_type> struct key_accessor<value_type> {
		DCA_INLINE static uint64_t getHashKey(const value_type& other) {
			return key_hasher<value_type>::getHashKey(other);
		}
	};

	template<> struct key_accessor<jsonifier::vector<jsonifier::string>> {
		DCA_INLINE static uint64_t getHashKey(const jsonifier::vector<jsonifier::string>& other) {
			return key_hasher<jsonifier::vector<jsonifier::string>>::getHashKey(other);
		}
	};

	template<typename value_type> struct hash_policy {
		template<typename key_type> DCA_INLINE uint64_t indexForHash(key_type&& key) const {
			return key_hasher<std::remove_cvref_t<key_type>>::getHashKey(key) & (static_cast<const value_type*>(this)->capacityVal - 1);
		}

		DCA_INLINE static int8_t log2(uint64_t value) {
			static constexpr int8_t table[64] = { 63, 0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4, 62, 57,
				46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56, 45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5 };
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			value |= value >> 32;
			return table[((value - (value >> 1)) * 0x07EDD5E59A4E28C2) >> 58];
		}

		DCA_INLINE static uint64_t nextPowerOfTwo(uint64_t size) {
			--size;
			size |= size >> 1;
			size |= size >> 2;
			size |= size >> 4;
			size |= size >> 8;
			size |= size >> 16;
			size |= size >> 32;
			++size;
			return size;
		}

		static int8_t computeMaxLookAheadDistance(uint64_t num_buckets) {
			return log2(num_buckets);
		}
	};

	template<typename value_type_internal_new> class hash_iterator {
	  public:
		using iterator_category	  = std::forward_iterator_tag;
		using value_type_internal = value_type_internal_new;
		using value_type		  = typename value_type_internal::value_type;
		using reference			  = value_type&;
		using pointer			  = value_type*;
		using pointer_internal	  = value_type_internal*;
		using size_type			  = uint64_t;

		DCA_INLINE hash_iterator() = default;

		DCA_INLINE hash_iterator(pointer_internal valueNew, size_type currentIndexNew) : value{ valueNew }, currentIndex{ currentIndexNew } {};

		DCA_INLINE hash_iterator& operator++() {
			skipEmptySlots();
			return *this;
		}

		DCA_INLINE hash_iterator& operator--() {
			skipEmptySlotsRev();
			return *this;
		}

		DCA_INLINE hash_iterator& operator-(size_type amountToReverse) {
			for (size_type x = 0; x < amountToReverse; ++x) {
				skipEmptySlotsRev();
			}
			return *this;
		}

		DCA_INLINE hash_iterator& operator+(size_type amountToAdd) {
			for (size_type x = 0; x < amountToAdd; ++x) {
				skipEmptySlots();
			}
			return *this;
		}

		DCA_INLINE pointer getRawPtr() {
			return &value->data[currentIndex];
		}

		DCA_INLINE bool operator==(const hash_iterator&) const {
			return !value || value->sentinelVector[currentIndex] == -1;
		}

		DCA_INLINE pointer operator->() {
			return &value->data[currentIndex];
		}

		DCA_INLINE reference operator*() {
			return value->data[currentIndex];
		}

	  protected:
		pointer_internal value{};
		size_type currentIndex{};

		void skipEmptySlots() {
			if (currentIndex < value->sentinelVector.size()) {
				++currentIndex;
				while (value && value->sentinelVector[currentIndex] == 0 && currentIndex < value->sentinelVector.size()) {
					++currentIndex;
				};
			}
		}

		void skipEmptySlotsRev() {
			if (static_cast<int64_t>(currentIndex) > 0) {
				--currentIndex;
				while (value && value->sentinelVector[currentIndex] == 0 && static_cast<int64_t>(currentIndex) > 0) {
					--currentIndex;
				};
			}
		}
	};
}
