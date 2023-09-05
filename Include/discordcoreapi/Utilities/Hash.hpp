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

	class EtfSerializer;

	template<typename ValueType>
	concept EtfSerializerT = std::same_as<ValueType, EtfSerializer>;

	template<typename ValueType>
	concept BoolT = std::same_as<std::decay_t<ValueType>, bool> && !EtfSerializerT<ValueType> && !std::integral<ValueType>;

	template<typename ValueType>
	concept EnumT = std::is_enum<std::decay_t<ValueType>>::value;

	template<typename ValueType>
	concept IntegerT = std::integral<std::decay_t<ValueType>> && !BoolT<std::decay_t<ValueType>>;

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
		template<typename ValueType01, typename ValueType02> inline bool operator()(const ValueType01& lhs, const ValueType02& rhs) const {
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
		inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other.id.operator const uint64_t&(), sizeof(uint64_t));
		}
	};

	template<uint64_t size> struct KeyHasher<char[size]> {
		inline uint64_t operator()(const char (&other)[size]) const {
			return internalHashFunction(other, std::char_traits<char>::length(other));
		}
	};

	template<IntegerT ValueType> struct KeyHasher<ValueType> {
		inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other, sizeof(other));
		}
	};

	template<> struct KeyHasher<TwoIdKey> {
		inline uint64_t operator()(const TwoIdKey& other) const {
			uint64_t values[2]{};
			values[0] = other.idOne.operator const uint64_t&();
			values[1] = other.idTwo.operator const uint64_t&();
			return internalHashFunction(values, sizeof(uint64_t) * std::size(values));
		}
	};

	template<EnumT ValueType> struct KeyHasher<ValueType> {
		inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other, sizeof(other));
		}
	};

	template<JsonifierInternal::StringT ValueType> struct KeyHasher<ValueType> {
		inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(other.data(), other.size());
		}
	};

	template<> struct KeyHasher<Snowflake> {
		inline uint64_t operator()(const Snowflake& data) const {
			return internalHashFunction(&data.operator const uint64_t&(), sizeof(uint64_t));
		}
	};

	template<> struct KeyHasher<Jsonifier::Vector<std::string>> {
		inline uint64_t operator()(const Jsonifier::Vector<std::string>& data) const {
			std::string newString{};
			for (auto& value: data) {
				newString.append(value);
			}
			return internalHashFunction(newString.data(), newString.size());
		}
	};

	template<HasTwoId ValueType> struct KeyHasher<ValueType> {
		inline uint64_t operator()(const ValueType& other) const {
			return KeyHasher<TwoIdKey>{}.operator()(TwoIdKey{ other });
		}
	};

	template<JsonifierInternal::UniquePtrT ValueType> struct KeyHasher<ValueType> {
		inline uint64_t operator()(const ValueType& other) const {
			return KeyHasher<typename ValueType::element_type>{}.operator()(*other);
		}
	};

	struct KeyAccessor {
		template<GuildMemberT ValueType> uint64_t operator()(const ValueType& other) const {
			return KeyHasher<ValueType>{}.operator()(other);
		}

		template<VoiceStateT ValueType> uint64_t operator()(const ValueType& other) const {
			return KeyHasher<ValueType>{}.operator()(other);
		}

		uint64_t operator()(const Snowflake& other) const {
			return KeyHasher<Snowflake>{}.operator()(other);
		}

		uint64_t operator()(const TwoIdKey& other) const {
			return KeyHasher<TwoIdKey>{}.operator()(other);
		}

		template<uint64_t size> inline uint64_t operator()(const char (&other)[size]) const {
			return KeyHasher<char[size]>{}.operator()(other);
		}

		template<JsonifierInternal::StringT ValueType> uint64_t operator()(const ValueType& other) const {
			return KeyHasher<ValueType>{}.operator()(other);
		}

		template<HasId ValueType> uint64_t operator()(const ValueType& other) const {
			return KeyHasher<ValueType>{}.operator()(other);
		}

		template<JsonifierInternal::UniquePtrT ValueType> uint64_t operator()(const ValueType& other) const {
			return KeyHasher<ValueType>{}.operator()(other);
		}

		uint64_t operator()(const Jsonifier::Vector<std::string>& other) const {
			return KeyHasher<Jsonifier::Vector<std::string>>{}.operator()(other);
		}
	};

	inline constexpr int8_t minLookups{ 0 };

	template<typename ValueType> struct HashPolicy {
		template<typename KeyType> inline uint64_t indexForHash(KeyType&& key) const {
			return KeyHasher<std::remove_cvref_t<KeyType>>{}.operator()(key) & (static_cast<const ValueType*>(this)->capacityVal - 1);
		}

		static inline int8_t log2(size_t value) {
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

		static inline size_t nextPowerOfTwo(size_t size) {
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

		static int8_t computeMaxLookAheadDistance(size_t num_buckets) {
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

		inline bool operator==(const Pair& other) const {
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

		inline HashIterator() noexcept = default;

		inline HashIterator(pointer_internal valueNew, size_type currentIndexNew) : value{ valueNew }, currentIndex{ currentIndexNew } {};

		inline HashIterator& operator++() {
			skipEmptySlots();
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

		inline ~HashIterator(){};

	  protected:
		pointer_internal value{};
		size_type currentIndex{};

		void skipEmptySlots() {
			++currentIndex;
			while (value && value->sentinelVector[currentIndex] == 0) {
				++currentIndex;
			};
		}
	};
}