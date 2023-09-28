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
#include <vector>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {
		struct EventDelegateToken;
	}

	class GuildMemberCacheData;
	struct VoiceStateDataLight;
	struct VoiceStateData;
	class GuildMemberData;

	template<typename ValueType>
	concept VoiceStateT = std::same_as<ValueType, VoiceStateDataLight>;

	template<typename ValueType>
	concept GuildMemberT = std::same_as<ValueType, GuildMemberCacheData> || std::same_as<ValueType, GuildMemberData>;

	template<typename ValueType>
	concept HasId = requires(ValueType value) { value.id; };

	template<typename ValueType>
	concept HasTwoId = VoiceStateT<ValueType> || GuildMemberT<ValueType>;

	template<typename ValueType>
	concept EventDelegateTokenT = std::same_as<ValueType, DiscordCoreInternal::EventDelegateToken>;

	struct ObjectCompare {
		template<typename ValueType01, typename ValueType02> inline bool operator()(const ValueType01& lhs, const ValueType02& rhs) {
			return lhs == rhs;
		}

		template<typename ValueType01, typename ValueType02> inline bool operator()(ValueType01& lhs, ValueType02& rhs) {
			return lhs == rhs;
		}
	};

	inline uint64_t internalHashFunction(const void* value, uint64_t count) {
		static constexpr uint64_t fnvOffsetBasis{ 0xcbf29ce484222325 };
		static constexpr uint64_t fnvPrime{ 0x00000100000001B3 };
		uint64_t hash{ fnvOffsetBasis };
		for (uint64_t x = 0; x < count; ++x) {
			hash ^= static_cast<const uint8_t*>(value)[x];
			hash *= fnvPrime;
		}
		return hash;
	}

	class TwoIdKey {
	  public:
		template<GuildMemberT ValueType> TwoIdKey(const ValueType& other);
		template<VoiceStateT ValueType> TwoIdKey(const ValueType& other);

		Snowflake idOne{};
		Snowflake idTwo{};
	};

	template<typename ValueType> struct KeyHasher;

	template<HasId ValueType> struct KeyHasher<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return internalHashFunction(&other.id.operator const uint64_t&(), sizeof(uint64_t));
		}
	};

	template<uint64_t size> struct KeyHasher<char[size]> {
		inline static uint64_t getHashKey(const char (&other)[size]) {
			return internalHashFunction(other, std::char_traits<char>::length(other));
		}
	};

	template<jsonifier_internal::integer_t ValueType> struct KeyHasher<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return internalHashFunction(&other, sizeof(other));
		}
	};

	template<> struct KeyHasher<TwoIdKey> {
		inline static uint64_t getHashKey(const TwoIdKey& other) {
			uint64_t values[2]{};
			values[0] = other.idOne.operator const uint64_t&();
			values[1] = other.idTwo.operator const uint64_t&();
			return internalHashFunction(values, sizeof(uint64_t) * std::size(values));
		}
	};

	template<jsonifier_internal::enum_t ValueType> struct KeyHasher<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return internalHashFunction(&other, sizeof(other));
		}
	};

	template<jsonifier_internal::string_t ValueType> struct KeyHasher<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return internalHashFunction(other.data(), other.size());
		}
	};

	template<> struct KeyHasher<Snowflake> {
		inline static uint64_t getHashKey(const Snowflake& data) {
			return internalHashFunction(&data.operator const uint64_t&(), sizeof(uint64_t));
		}
	};

	template<> struct KeyHasher<jsonifier::vector<jsonifier::string>> {
		inline static uint64_t getHashKey(const jsonifier::vector<jsonifier::string>& data) {
			jsonifier::string newString{};
			for (auto& value: data) {
				newString.append(value);
			}
			return internalHashFunction(newString.data(), newString.size());
		}
	};

	template<HasTwoId ValueType> struct KeyHasher<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return KeyHasher<TwoIdKey>::getHashKey(TwoIdKey{ other });
		}
	};

	template<jsonifier_internal::unique_ptr_t ValueType> struct KeyHasher<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return KeyHasher<typename ValueType::element_type>::getHashKey(*other);
		}
	};

	template<typename value_Type> struct KeyAccessor;

	template<GuildMemberT ValueType> struct KeyAccessor<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return KeyHasher<ValueType>::getHashKey(other);
		}
	};

	template<VoiceStateT ValueType> struct KeyAccessor<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return KeyHasher<ValueType>::getHashKey(other);
		}
	};

	template<> struct KeyAccessor<Snowflake> {
		inline static uint64_t getHashKey(const Snowflake& other) {
			return KeyHasher<Snowflake>::getHashKey(other);
		}
	};

	template<> struct KeyAccessor<TwoIdKey> {
		inline static uint64_t getHashKey(const TwoIdKey& other) {
			return KeyHasher<TwoIdKey>::getHashKey(other);
		}
	};

	template<uint64_t size> struct KeyAccessor<char[size]> {
		inline static uint64_t getHashKey(const char (&other)[size]) {
			return KeyHasher<char[size]>::getHashKey(other);
		}
	};

	template<jsonifier_internal::string_t ValueType> struct KeyAccessor<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return KeyHasher<ValueType>::getHashKey(other);
		}
	};

	template<HasId ValueType> struct KeyAccessor<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return KeyHasher<ValueType>::getHashKey(other);
		}
	};

	template<jsonifier_internal::unique_ptr_t ValueType> struct KeyAccessor<ValueType> {
		inline static uint64_t getHashKey(const ValueType& other) {
			return KeyHasher<ValueType>::getHashKey(other);
		}
	};

	template<> struct KeyAccessor<jsonifier::vector<jsonifier::string>> {
		inline static uint64_t getHashKey(const jsonifier::vector<jsonifier::string>& other) {
			return KeyHasher<jsonifier::vector<jsonifier::string>>::getHashKey(other);
		}
	};

	inline constexpr int8_t minLookups{ 0 };

	template<typename ValueType> struct HashPolicy {
		template<typename KeyType> inline uint64_t indexForHash(KeyType&& key) const {
			return KeyHasher<std::remove_cvref_t<KeyType>>::getHashKey(key) & (static_cast<const ValueType*>(this)->capacityVal - 1);
		}

		inline static int8_t log2(uint64_t value) {
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

		inline static uint64_t nextPowerOfTwo(uint64_t size) {
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
			int8_t desired = log2(num_buckets);
			return std::max(minLookups, desired);
		}
	};

	template<typename FirstType, typename SecondType> class Pair {
	  public:
		using first_type  = FirstType;
		using second_type = SecondType;

		first_type first;
		second_type second;

		template<typename FirstTypeNew, typename SecondTypeNew> inline Pair(FirstTypeNew&& firstNew, SecondTypeNew&& secondNew)
			: first{ std::forward<FirstTypeNew>(firstNew) }, second{ std::forward<SecondTypeNew>(secondNew) } {
		}

		template<typename FirstTypeNew> inline Pair(FirstTypeNew&& firstNew) : first{ std::forward<FirstTypeNew>(firstNew) } {
		}

		template<typename... Args> inline Pair(Args&&... args) : Pair{ std::forward<Args>(args)... } {
		}

		inline bool operator==(const Pair& other) {
			return first == other.first && second == other.second;
		}
	};

	template<typename ValueTypeInternal> class HashIterator {
	  public:
		using iterator_category	  = std::forward_iterator_tag;
		using value_type_internal = ValueTypeInternal;
		using value_type		  = ValueTypeInternal::value_type;
		using reference			  = value_type&;
		using pointer			  = value_type*;
		using pointer_internal	  = value_type_internal*;
		using size_type			  = uint64_t;

		inline HashIterator() = default;

		inline HashIterator(pointer_internal valueNew, size_type currentIndexNew) : value{ valueNew }, currentIndex{ currentIndexNew } {};

		inline HashIterator& operator++() {
			skipEmptySlots();
			return *this;
		}

		inline HashIterator& operator--() {
			skipEmptySlotsRev();
			return *this;
		}

		inline HashIterator& operator-(size_type amountToReverse) {
			for (size_type x = 0; x < amountToReverse; ++x) {
				skipEmptySlotsRev();
			}
			return *this;
		}

		inline HashIterator& operator+(size_type amountToAdd) {
			for (size_type x = 0; x < amountToAdd; ++x) {
				skipEmptySlots();
			}
			return *this;
		}

		inline pointer getRawPtr() {
			return &value->data[currentIndex];
		}

		inline bool operator==(const HashIterator&) const {
			return !value || value->sentinelVector[currentIndex] == -1;
		}

		inline pointer operator->() {
			return &value->data[currentIndex];
		}

		inline reference operator*() {
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