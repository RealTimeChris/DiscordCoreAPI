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
	concept BoolT = std::same_as<std::decay_t<ValueType>, bool> && !std::integral<ValueType>;

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
	};

	class TwoIdKey {
	  public:
		template<GuildMemberT ValueType> TwoIdKey(const ValueType& other);
		template<VoiceStateT ValueType> TwoIdKey(const ValueType& other);

		Snowflake idOne{};
		Snowflake idTwo{};
	};

	struct KeyHasher {
	  public:
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
			uint64_t data[2]{};
			data[0] = other.idOne.operator const uint64_t&();
			data[1] = other.idTwo.operator const uint64_t&();
			return internalHashFunction(data, sizeof(uint64_t) * std::size(data));
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

		inline uint64_t operator()(const Jsonifier::Vector<std::string>& data) const {
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

	template<typename ValueType> struct HashPolicy : public KeyHasher {
	  public:
		template<typename KeyType> inline uint64_t indexForHash(const KeyType& hash) const {
			return static_cast<const KeyHasher*>(this)->operator()(hash) & (static_cast<const ValueType*>(this)->capacityVal - 1);
		}

		inline uint64_t nextSizeOver(uint64_t size) const {
			size = getNextPowerOfTwo(size);
			size = nextSizeOverPrime(size);
			return getNextPowerOfTwo(size);
		}

		inline int8_t computeMaxProbingDistance(size_t numBuckets) const {
			return std::max(int8_t{ 4 }, log2(numBuckets));
		}

	  protected:
		inline uint64_t getNextPowerOfTwo(uint64_t size) const {
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

		inline int8_t log2(size_t value) const {
			constexpr int8_t table[64]{ 63, 0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11,
				43, 14, 22, 4, 62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56, 45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6,
				5 };
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			value |= value >> 32;
			return table[((value - (value >> 1)) * std::numeric_limits<uint64_t>::max() * std::numeric_limits<uint64_t>::max()) >> 58];
		}

		inline uint64_t nextSizeOverPrime(uint64_t size) const {
			// prime numbers generated by the following method:
			// 1. start with a prime p = 2
			// 2. go to wolfram alpha and get p = NextPrime(2 * p)
			// 3. repeat 2. until you overflow 64 bits
			// you now have large gaps which you would hit if somebody called reserve() with an unlucky number.
			// 4. to fill the gaps for every prime p go to wolfram alpha and get ClosestPrime(p * 2^(1/3)) and ClosestPrime(p * 2^(2/3)) and put those in the gaps
			// 5. get PrevPrime(2^64) and put it at the end
			constexpr const uint64_t primeList[]{ 2llu, 3llu, 5llu, 7llu, 11llu, 13llu, 17llu, 23llu, 29llu, 37llu, 47llu, 59llu, 73llu, 97llu,
				127llu, 151llu, 197llu, 251llu, 313llu, 397llu, 499llu, 631llu, 797llu, 1009llu, 1259llu, 1597llu, 2011llu, 2539llu, 3203llu, 4027llu,
				5087llu, 6421llu, 8089llu, 10193llu, 12853llu, 16193llu, 20399llu, 25717llu, 32401llu, 40823llu, 51437llu, 64811llu, 81649llu,
				102877llu, 129607llu, 163307llu, 205759llu, 259229llu, 326617llu, 411527llu, 518509llu, 653267llu, 823117llu, 1037059llu, 1306601llu,
				1646237llu, 2074129llu, 2613229llu, 3292489llu, 4148279llu, 5226491llu, 6584983llu, 8296553llu, 10453007llu, 13169977llu, 16593127llu,
				20906033llu, 26339969llu, 33186281llu, 41812097llu, 52679969llu, 66372617llu, 83624237llu, 105359939llu, 132745199llu, 167248483llu,
				210719881llu, 265490441llu, 334496971llu, 421439783llu, 530980861llu, 668993977llu, 842879579llu, 1061961721llu, 1337987929llu,
				1685759167llu, 2123923447llu, 2675975881llu, 3371518343llu, 4247846927llu, 5351951779llu, 6743036717llu, 8495693897llu,
				10703903591llu, 13486073473llu, 16991387857llu, 21407807219llu, 26972146961llu, 33982775741llu, 42815614441llu, 53944293929llu,
				67965551447llu, 85631228929llu, 107888587883llu, 135931102921llu, 171262457903llu, 215777175787llu, 271862205833llu, 342524915839llu,
				431554351609llu, 543724411781llu, 685049831731llu, 863108703229llu, 1087448823553llu, 1370099663459llu, 1726217406467llu,
				2174897647073llu, 2740199326961llu, 3452434812973llu, 4349795294267llu, 5480398654009llu, 6904869625999llu, 8699590588571llu,
				10960797308051llu, 13809739252051llu, 17399181177241llu, 21921594616111llu, 27619478504183llu, 34798362354533llu, 43843189232363llu,
				55238957008387llu, 69596724709081llu, 87686378464759llu, 110477914016779llu, 139193449418173llu, 175372756929481llu,
				220955828033581llu, 278386898836457llu, 350745513859007llu, 441911656067171llu, 556773797672909llu, 701491027718027llu,
				883823312134381llu, 1113547595345903llu, 1402982055436147llu, 1767646624268779llu, 2227095190691797llu, 2805964110872297llu,
				3535293248537579llu, 4454190381383713llu, 5611928221744609llu, 7070586497075177llu, 8908380762767489llu, 11223856443489329llu,
				14141172994150357llu, 17816761525534927llu, 22447712886978529llu, 28282345988300791llu, 35633523051069991llu, 44895425773957261llu,
				56564691976601587llu, 71267046102139967llu, 89790851547914507llu, 113129383953203213llu, 142534092204280003llu, 179581703095829107llu,
				226258767906406483llu, 285068184408560057llu, 359163406191658253llu, 452517535812813007llu, 570136368817120201llu,
				718326812383316683llu, 905035071625626043llu, 1140272737634240411llu, 1436653624766633509llu, 1810070143251252131llu,
				2280545475268481167llu, 2873307249533267101llu, 3620140286502504283llu, 4561090950536962147llu, 5746614499066534157llu,
				7240280573005008577llu, 9122181901073924329llu, 11493228998133068689llu, 14480561146010017169llu, 18446744073709551557llu };
			return *std::lower_bound(std::begin(primeList), std::end(primeList) - 1, size);
		}
	};

	template<typename FirstType, typename SecondType> class Pair {
	  public:
		using first_type = FirstType;
		using second_type = SecondType;

		first_type first;
		second_type second;

		constexpr Pair() noexcept = default;

		template<typename FirstTypeNew, typename SecondTypeNew> constexpr Pair(FirstTypeNew&& firstNew, SecondTypeNew&& secondNew)
			: first{ std::forward<FirstTypeNew>(firstNew) }, second{ std::forward<SecondTypeNew>(secondNew) } {};

		template<typename FirstTypeNew, typename SecondTypeNew> constexpr Pair& operator=(Pair<FirstTypeNew, SecondTypeNew>&& valueNew) {
			first = { std::forward<FirstTypeNew>(valueNew.first) };
			second = { std::forward<SecondTypeNew>(valueNew.second) };
			return *this;
		};

		template<typename FirstTypeNew, typename SecondTypeNew> constexpr Pair(Pair<FirstTypeNew, SecondTypeNew>&& valueNew) {
			*this = valueNew;
		};

		template<typename FirstTypeNew> constexpr Pair& operator=(FirstTypeNew&& valueNew) {
			first = std::forward<FirstTypeNew>(valueNew);
			return *this;
		};

		template<typename FirstTypeNew> constexpr Pair(FirstTypeNew&& valueNew) {
			*this = std::forward<FirstTypeNew>(valueNew);
		};

		constexpr bool operator==(const Pair& other) const {
			return this->first == other.first && second == other.second;
		}
	};

	template<typename ValueTypeInternal> class HashIterator {
	  public:
		using iterator_category = std::forward_iterator_tag;
		using value_type_internal = ValueTypeInternal;
		using value_type = ValueTypeInternal::value_type;
		using reference = value_type&;
		using pointer = value_type*;
		using pointer_internal = value_type_internal*;
		using size_type = uint64_t;

		constexpr HashIterator() noexcept = default;

		constexpr HashIterator(pointer_internal valueNew, uint64_t currentIndexNew) : currentIndex{ currentIndexNew }, value{ valueNew } {
		}

		constexpr HashIterator& operator++() {
			++currentIndex;
			skipEmptySlots();
			return *this;
		}

		constexpr pointer_internal getRawPtr() {
			return value;
		}

		constexpr bool operator==(const HashIterator&) const {
			return !value || currentIndex >= value->sentinelVector.size();
		}

		constexpr pointer operator->() const {
			return &value->data[currentIndex];
		}

		constexpr reference operator*() const {
			return value->data[currentIndex];
		}

		constexpr ~HashIterator(){};

	  protected:
		pointer_internal value{};
		uint64_t currentIndex{};

		constexpr void skipEmptySlots() {
			for (; currentIndex < value->sentinelVector.size(); ++currentIndex) {
				if (value->sentinelVector[currentIndex] > 0) {
					break;
				}
			}
		}
	};	
}