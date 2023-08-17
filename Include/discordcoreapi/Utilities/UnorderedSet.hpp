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
/// UnorderedSet.hpp - Header file for the UnorderedSet class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file UnorderedSet.hpp

#pragma once

#include <discordcoreapi/Utilities/Hash.hpp>

namespace DiscordCoreAPI {

	template<typename ValueType> class UnorderedSet;

	template<typename SetIterator, typename ValueType>
	concept SetContainerIteratorT = std::is_same_v<typename UnorderedSet<ValueType>::iterator, std::decay_t<SetIterator>>;

	template<typename ValueType> class UnorderedSet : protected HashPolicy<UnorderedSet<ValueType>>,
																		protected JsonifierInternal::AllocWrapper<ValueType>, 
																		protected ObjectCompare { 
	  public: 
		using mapped_type = ValueType;
		using key_type = mapped_type;
		using reference = mapped_type&;
		using value_type = mapped_type;
		using const_reference = const mapped_type&;
		using size_type = uint64_t;
		using object_compare = ObjectCompare;
		using hash_policy = HashPolicy<UnorderedSet<mapped_type>>;
		using key_hasher = hash_policy;
		friend hash_policy;

		using iterator = HashIterator<UnorderedSet<mapped_type>>;
		friend iterator;
		using const_iterator = HashIterator<const UnorderedSet<mapped_type>>;
		friend const_iterator;

		using allocator = JsonifierInternal::AllocWrapper<value_type>;

		inline UnorderedSet(){};

		inline UnorderedSet& operator=(UnorderedSet&& other) noexcept {
			if (this != &other) {
				clear();
				swap(other);
			}
			return *this;
		}

		inline UnorderedSet(UnorderedSet&& other) noexcept {
			*this = std::move(other);
		}

		inline UnorderedSet& operator=(const UnorderedSet& other) {
			if (this != &other) {
				clear();

				reserve(other.capacity());
				for (const auto& value: other) {
					emplace(value);
				}
			}
			return *this;
		}

		inline UnorderedSet(const UnorderedSet& other) {
			*this = other;
		}

		template<typename... Args> iterator emplace(Args&&... value) {
			return emplaceInternal(std::forward<Args>(value)...);
		}

		template<typename key_type_new> inline const_iterator find(key_type_new&& key) const {
			if (capacityVal) {
				auto currentEntry = indexForHash(key);
				for (int8_t x{}; x < maxLookupDistance; ++x) {
					if (sentinelVector[currentEntry + x] > 0 && getObjectComparitor()(getKeyHasher()(*(data + currentEntry + x)), getKeyHasher()(key))) {
						return { this, currentEntry + x };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator find(key_type_new&& key) {
			if (capacityVal) {
				auto currentEntry = indexForHash(key);
				for (int8_t x{}; x < maxLookupDistance; ++x) {
					if (sentinelVector[currentEntry + x] > 0 && getObjectComparitor()(getKeyHasher()(*(data + currentEntry + x)), getKeyHasher()(key))) {
						return { this, currentEntry + x };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline const_reference operator[](key_type_new&& key) const {
			auto iter = find(key);
			if (iter == end()) {
				iter = emplaceInternal(std::forward<key_type_new>(key));
			}
			return *iter;
		}

		template<typename key_type_new> inline reference operator[](key_type_new&& key) {
			auto iter = find(key);
			if (iter == end()) {
				iter = emplaceInternal(std::forward<key_type_new>(key));
			}
			return *iter;
		}

		template<typename key_type_new> inline const_reference at(key_type_new&& key) const {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw DCAException{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return *iter;
		}

		template<typename key_type_new> inline reference at(key_type_new&& key) {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw DCAException{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return *iter;
		}

		template<typename key_type_new> inline bool contains(key_type_new&& key) const {
			if (capacityVal) {
				auto currentEntry = indexForHash(key);
				for (int8_t x{}; x < maxLookupDistance; ++x) {
					if (sentinelVector[currentEntry + x] > 0 && getObjectComparitor()(getKeyHasher()(*(data + currentEntry + x)), getKeyHasher()(key))) {
						return true;
					}
				}
			}
			return false;
		}

		template<SetContainerIteratorT<mapped_type> SetIterator> inline iterator erase(SetIterator&& iter) {
			if (capacityVal) {
				auto currentEntry = static_cast<size_type>(iter.operator->() - data);
				for (int8_t x{}; x < maxLookupDistance; ++x) {
					if (sentinelVector[currentEntry + x] > 0 && getObjectComparitor()(getKeyHasher()(*(data + currentEntry + x), getKeyHasher()(iter.operator*().first)))) {
						std::destroy_at(data + currentEntry + x);
						sentinelVector[currentEntry + x] = 0;
						sizeVal--;
						return { this, ++currentEntry + x };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator erase(key_type_new&& key) {
			if (capacityVal) {
				auto currentEntry = indexForHash(key);
				for (int8_t x{}; x < maxLookupDistance; ++x) {
					if (sentinelVector[currentEntry + x] > 0 && getObjectComparitor()(getKeyHasher()(*(data + currentEntry + x)), getKeyHasher()(key))) {
						std::destroy_at(data + currentEntry + x);
						sentinelVector[currentEntry + x] = 0;
						sizeVal--;
						return { this, ++currentEntry + x };
					}
				}
			}
			return end();
		}

		inline const_iterator begin() const {
			if (sizeVal) {
				for (size_type x = 0; x < capacityVal; ++x) {
					if (sentinelVector[x] > 0) {
						return const_iterator{ this, x };
					}
				}
			}
			return end();
		}

		inline const_iterator end() const {
			return {};
		}

		inline iterator begin() {
			if (sizeVal) {
				for (size_type x = 0; x < capacityVal; ++x) {
					if (sentinelVector[x] > 0) {
						return iterator{ this, x };
					}
				}
			}
			return end();
		}

		inline iterator end() {
			return {};
		}

		inline bool full() const {
			return static_cast<float>(sizeVal) >= static_cast<float>(capacityVal) * 0.95f;
		}

		inline size_type size() const {
			return sizeVal;
		}

		inline bool empty() const {
			return sizeVal == 0;
		}

		inline void reserve(size_type sizeNew) {
			resize(sizeNew);
		}

		inline void swap(UnorderedSet& other) noexcept {
			std::swap(maxLookupDistance, other.maxLookupDistance);
			std::swap(sentinelVector, other.sentinelVector);
			std::swap(capacityVal, other.capacityVal);
			std::swap(sizeVal, other.sizeVal);
			std::swap(data, other.data);
		}

		inline size_type capacity() const {
			return capacityVal;
		}

		inline bool operator==(const UnorderedSet& other) const {
			if (sizeVal != other.sizeVal) {
				return false;
			}
			for (auto iter01{ begin() }, iter02{ other.begin() }; iter01 != end() && iter02 != other.end(); ++iter01, ++iter02) {
				if (!getObjectComparitor()(iter01.operator*().second, iter02.operator*().second)) {
					return false;
				}
			}
			return true;
		}

		inline void clear() {
			if (data && capacityVal) {
				for (size_type x = 0; x < capacityVal + maxLookupDistance; ++x) {
					if (sentinelVector[x] > 0) {
						getAllocator().destroy(data + x);
					}
				}
				getAllocator().deallocate(data, capacityVal);
				sentinelVector.clear();
				capacityVal = 0;
				data = nullptr;
				sizeVal = 0;
			}
		}

		inline ~UnorderedSet() {
			clear();
		};

	  protected:
		Jsonifier::Vector<int8_t> sentinelVector{};
		int8_t maxLookupDistance{ 4 };
		size_type capacityVal{};
		size_type sizeVal{};
		value_type* data{};

		template<typename... mapped_type_new> inline iterator emplaceInternal(mapped_type_new&&... value) {
			if (full() || capacityVal == 0) {
				resize(static_cast<uint64_t>(static_cast<double>(capacityVal) * 2.0f) + 2.0f);
			}
			auto currentEntry = indexForHash(value...);
			for (int8_t x{}; x < maxLookupDistance; ++x) {
				if (sentinelVector[currentEntry + x] == 0) {
					sentinelVector[currentEntry + x] = 1;
					new (data + currentEntry + x) value_type(std::forward<mapped_type_new>(value)...);
					sizeVal++;
					return { this, currentEntry + x };
				} else if (sentinelVector[currentEntry + x] == 1 &&
					getObjectComparitor()(getKeyHasher()(*(data + currentEntry + x)), getKeyHasher()(value...))) {
					if constexpr ((( !std::is_void_v<mapped_type_new> ) || ...)) {
						*(data + currentEntry + x) = mapped_type{ std::forward<mapped_type_new>(value)... };
					}
					return { this, currentEntry + x };
				}
			}
			resize(static_cast<uint64_t>(static_cast<double>(capacityVal) * 2.0f));
			return emplaceInternal(std::forward<mapped_type_new>(value)...);
		}

		inline const object_compare& getObjectComparitor() const {
			return *this;
		}

		template<typename KeyTypeNew> inline uint64_t indexForHash(KeyTypeNew&& key) const {
			return getKeyHasher().indexForHash(std::forward<KeyTypeNew>(key));
		}

		inline size_type nextSizeOver(size_type key) const {
			return getKeyHasher().nextSizeOver(key);
		}

		inline const key_hasher& getKeyHasher() const {
			return *this;
		}

		inline const hash_policy& getHashPolicy() const {
			return *this;
		}

		inline allocator& getAllocator() {
			return *this;
		}

		inline void resize(size_type capacityNew) {
			auto newSize = nextSizeOver(capacityNew);
			if (newSize > capacityVal) {
				Jsonifier::Vector<int8_t> oldSentinelVector = std::move(sentinelVector);
				auto oldMaxLookupDistance = maxLookupDistance;
				auto oldCapacity = capacityVal;
				auto oldSize = sizeVal;
				auto oldPtr = data;
				sizeVal = 0;
				maxLookupDistance = getHashPolicy().computeMaxProbingDistance(newSize);
				data = getAllocator().allocate(newSize + maxLookupDistance);
				capacityVal = newSize;
				sentinelVector.resize(newSize + maxLookupDistance);
				for (size_type y{}, x{}; y < oldSize; ++x) {
					if (oldSentinelVector[x] > 0) {
						++y;
						emplaceInternal(std::move(oldPtr[x]));
					}
				}
				if (oldPtr && oldCapacity) {
					getAllocator().deallocate(oldPtr, oldCapacity + oldMaxLookupDistance);
				}
			}
		}
	};
}