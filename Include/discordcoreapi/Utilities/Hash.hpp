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
	concept EventDelegateTokenT = std::same_as<ValueType, DiscordCoreInternal::EventDelegateToken>;

	struct ObjectCompare {
		template<typename ValueType01, typename ValueType02> inline bool operator()(const ValueType01& lhs, const ValueType02& rhs) const {
			return lhs == rhs;
		}

		template<typename ValueType01, typename ValueType02> inline bool operator()(ValueType01& lhs, ValueType02& rhs) {
			return lhs == rhs;
		}
	};

	class TwoIdKey {
	  public:
		template<GuildMemberT ValueType> TwoIdKey(const ValueType& other);
		template<VoiceStateT ValueType> TwoIdKey(const ValueType& other);

		Snowflake idOne{};
		Snowflake idTwo{};
	};

	struct KeyHasher {
		template<VoiceStateT ValueType> uint64_t operator()(const ValueType& other) const {
			return operator()(TwoIdKey{ other });
		}

		template<GuildMemberT ValueType> uint64_t operator()(const ValueType& other) const {
			return operator()(TwoIdKey{ other });
		}

		template<HasId ValueType> uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other.id.operator const uint64_t&(), sizeof(uint64_t));
		}

		template<EventDelegateTokenT ValueType> uint64_t operator()(const ValueType& other) const;

		template<uint64_t size> inline uint64_t operator()(const char (&other)[size]) const {
			return internalHashFunction(other, std::char_traits<char>::length(other));
		}

		inline uint64_t operator()(const TwoIdKey& other) const {
			uint64_t values[2]{};
			values[0] = other.idOne.operator const uint64_t&();
			values[1] = other.idTwo.operator const uint64_t&();
			return internalHashFunction(values, sizeof(uint64_t) * std::size(values));
		}

		template<typename ValueType> inline uint64_t operator()(const Jsonifier::StringBase<ValueType>& other) {
			return internalHashFunction(other.data(), other.size());
		}

		template<IntegerT ValueType> inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other, sizeof(other));
		}

		template<EnumT ValueType> inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other, sizeof(other));
		}

		inline uint64_t operator()(const std::string& other) const {
			return internalHashFunction(other.data(), other.size());
		}

		inline uint64_t operator()(const std::string_view& other) const {
			return internalHashFunction(other.data(), other.size());
		}

		inline uint64_t operator()(const Snowflake& data) const {
			return internalHashFunction(&data.operator const uint64_t&(), sizeof(uint64_t));
		}

		inline uint64_t operator()(const std::vector<std::string>& data) const {
			std::string newString{};
			for (auto& value: data) {
				newString.append(value);
			}
			return internalHashFunction(newString.data(), newString.size());
		}

	  protected:
		inline uint64_t internalHashFunction(const void* value, uint64_t count) const {
			static constexpr uint64_t fnvOffsetBasis{ 0xcbf29ce484222325 };
			static constexpr uint64_t fnvPrime{ 0x00000100000001B3 };
			uint64_t hash{ fnvOffsetBasis };
			for (uint64_t x = 0; x < count; ++x) {
				hash ^= static_cast<const uint8_t*>(value)[x];
				hash *= fnvPrime;
			}
			return hash;
		}
	};

	template<typename ValueType> struct PowerOfTwoHashPolicy {
		inline uint64_t indexForHash(uint64_t hash) const {
			return hash & (static_cast<const ValueType*>(this)->capacityVal - 1);
		}
		inline uint64_t nextSizeOver(uint64_t size) const {
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
	};

	static constexpr int8_t maxLookupDistance{ 5 };
	static constexpr int8_t endValue{ -1 };

	template<typename FirstType, typename SecondType> class Pair {
	  public:
		using first_type = FirstType;
		using second_type = SecondType;

		first_type first;
		second_type second;

		template<typename FirstTypeNew, typename SecondTypeNew> inline Pair(FirstTypeNew&& firstNew, SecondTypeNew&& secondNew)
			: first{ std::forward<FirstTypeNew>(firstNew) }, second{ std::forward<SecondTypeNew>(secondNew) } {};

		template<typename... Args> inline Pair(Args&&... args) : Pair{ std::forward<Args>(args)... } {};

		inline bool operator==(const Pair& other) const {
			return this->first == other.first && second == other.second;
		}
	};

	template<typename ValueType> struct ObjectCore {
		using value_type = ValueType;

		union {
			value_type value;
		};
		int8_t currentIndex{ 0 };

		inline ObjectCore(int8_t newIndex) : currentIndex{ newIndex } {};

		inline ObjectCore() : currentIndex{ 0 } {};

		template<typename... Args> inline void enable(Args&&... valueNew) {
			if (currentIndex > 0) {
				value.~value_type();
			}
			new (std::addressof(value)) value_type{ std::forward<Args>(valueNew)... };
			currentIndex = 1;
		}

		inline bool areWeActive() const {
			return currentIndex > 0;
		}

		inline bool areWeEmpty() const {
			return currentIndex == 0;
		}

		inline bool areWeDone() const {
			return currentIndex == -1;
		}

		inline void disable() {
			if (currentIndex > 0) {
				value.~value_type();
			}
			currentIndex = 0;
		}

		inline bool operator==(const ObjectCore& other) const {
			return value == other.value;
		}

		inline ~ObjectCore() {
			if (currentIndex > 0) {
				value.~value_type();
			}
		}
	};

	template<typename ValueTypeInternal> class HashIterator {
	  public:
		using iterator_category = std::forward_iterator_tag;
		using value_type_internal = ValueTypeInternal;
		using value_type = ValueTypeInternal::value_type;
		using reference = value_type&;
		using pointer = value_type*;
		using const_reference = const value_type&;
		using const_pointer = const value_type*;
		using pointer_internal = value_type_internal*;
		using size_type = uint64_t;

		inline HashIterator(const pointer_internal valueNew) : value(valueNew) {
			skipEmptySlots();
		};

		inline HashIterator& operator=(HashIterator&& other) noexcept {
			value = other.value;
			return *this;
		}

		inline HashIterator(HashIterator&& other) noexcept {
			*this = std::move(other);
		}

		inline HashIterator& operator++() {
			++value;
			skipEmptySlots();
			return *this;
		}

		inline pointer_internal getRawPtr() {
			return value;
		}

		inline bool operator==(const HashIterator&) const {
			return !value || value->areWeDone();
		}

		inline const_pointer operator->() const {
			return &value->value;
		}

		inline const_reference operator*() const {
			return value->value;
		}

		inline pointer operator->() {
			return &value->value;
		}

		inline reference operator*() {
			return value->value;
		}

		inline ~HashIterator(){};

	  protected:
		pointer_internal value;

		void skipEmptySlots() {
			while (value && value->areWeEmpty() && !value->areWeDone()) {
				value++;
			};
		}
	};
}