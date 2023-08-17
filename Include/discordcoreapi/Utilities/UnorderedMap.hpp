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
/// UnorderedMap.hpp - Header file for the UnorderedMap class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file UnorderedMap.hpp

#pragma once

#include <discordcoreapi/Utilities/Hash.hpp>

#include <memory_resource>
#include <shared_mutex>
#include <exception>
#include <optional>
#include <vector>
#include <mutex>

namespace DiscordCoreAPI {

	template<typename KeyType, typename ValueType> class UnorderedMap;

	template<typename MapIterator, typename KeyType, typename ValueType>
	concept MapContainerIteratorT = std::is_same_v<typename UnorderedMap<KeyType, ValueType>::iterator, std::decay_t<MapIterator>>;

	template<typename KeyType, typename ValueType>
	class UnorderedMap : protected HashPolicy<UnorderedMap<KeyType, ValueType>>, protected JsonifierInternal::AllocWrapper<Pair<KeyType, ValueType>>, protected ObjectCompare {
	  public:
		using mapped_type	  = ValueType;
		using key_type		  = KeyType;
		using reference		  = mapped_type&;
		using value_type	  = Pair<key_type, mapped_type>;
		using const_reference = const mapped_type&;
		using size_type		  = uint64_t;
		using object_compare  = ObjectCompare;
		using hash_policy	  = HashPolicy<UnorderedMap<key_type, mapped_type>>;
		using key_hasher	  = hash_policy;
		friend hash_policy;

		using iterator = HashIterator<UnorderedMap<key_type, mapped_type>>;
		friend iterator;
		using const_iterator = HashIterator<const UnorderedMap<key_type, mapped_type>>;
		friend const_iterator;

		using allocator = JsonifierInternal::AllocWrapper<value_type>;

		inline UnorderedMap(){};

		inline UnorderedMap& operator=(UnorderedMap&& other) noexcept {
			if (this != &other) {
				clear();
				swap(other);
			}
			return *this;
		}

		inline UnorderedMap(UnorderedMap&& other) noexcept {
			*this = std::move(other);
		}

		inline UnorderedMap& operator=(const UnorderedMap& other) {
			if (this != &other) {
				clear();

				reserve(other.capacity());
				for (const auto& [key, value]: other) {
					emplace(key, value);
				}
			}
			return *this;
		}

		inline UnorderedMap(const UnorderedMap& other) {
			*this = other;
		}

		inline UnorderedMap(std::initializer_list<value_type> list) {
			reserve(list.size());
			for (auto& value: list) {
				emplace(std::move(value.first), std::move(value.second));
			}
		};

		template<typename... Args> iterator emplace(Args&&... value) {
			return emplaceInternal(std::forward<Args>(value)...);
		}

		template<typename key_type_new> inline const_iterator find(key_type_new&& key) const {
			if (sizeVal > 0) {
				auto currentIndex = getHashPolicy().indexForHash(key);
				for (size_type x{}; x < maxLookAheadDistance; ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						return { this, currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator find(key_type_new&& key) {
			if (sizeVal > 0) {
				auto currentIndex = getHashPolicy().indexForHash(key);
				for (size_type x{}; x < maxLookAheadDistance; ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						return { this, currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline const_reference operator[](key_type_new&& key) const {
			return emplace(key)->second;
		}

		template<typename key_type_new> inline reference operator[](key_type_new&& key) {
			return emplace(key)->second;
		}

		template<typename key_type_new> inline const_reference at(key_type_new&& key) const {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw DCAException{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return iter->second;
		}

		template<typename key_type_new> inline reference at(key_type_new&& key) {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw DCAException{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return iter->second;
		}

		template<typename key_type_new> inline bool contains(key_type_new&& key) const {
			if (sizeVal > 0) {
				auto currentIndex = getHashPolicy().indexForHash(key);
				for (size_type x{}; x < maxLookAheadDistance; ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						return true;
					}
				}
			}
			return false;
		}

		template<MapContainerIteratorT<key_type, mapped_type> MapIterator> inline iterator erase(MapIterator&& iter) {
			if (sizeVal > 0) {
				auto currentIndex = static_cast<size_type>(iter.getRawPtr() - data);
				for (size_type x{}; x < maxLookAheadDistance; ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex], iter.operator*())) {
						getAlloc().destroy(data + currentIndex);
						sentinelVector[currentIndex] = 0;
						sizeVal--;
						return { this, ++currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator erase(key_type_new&& key) {
			if (sizeVal > 0) {
				auto currentIndex = getHashPolicy().indexForHash(key);
				for (size_type x{}; x < maxLookAheadDistance; ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						getAlloc().destroy(data + currentIndex);
						sentinelVector[currentIndex] = 0;
						sizeVal--;
						return { this, ++currentIndex };
					}
				}
			}
			return end();
		}

		inline const_iterator begin() const {
			for (size_type x{ 0 }; x < capacityVal; ++x) {
				if (sentinelVector[x] > 0) {
					return { this, x };
				}
			}
			return end();
		}

		inline const_iterator end() const {
			return {};
		}

		inline iterator begin() {
			for (size_type x{ 0 }; x < capacityVal; ++x) {
				if (sentinelVector[x] > 0) {
					return { this, x };
				}
			}
			return end();
		}

		inline iterator end() {
			return {};
		}

		inline bool full() const {
			return static_cast<float>(sizeVal) >= static_cast<float>(capacityVal) * 0.90f;
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

		void swap(UnorderedMap& other) noexcept {
			std::swap(maxLookAheadDistance, other.maxLookAheadDistance);
			std::swap(sentinelVector, other.sentinelVector);
			std::swap(capacityVal, other.capacityVal);
			std::swap(sizeVal, other.sizeVal);
			std::swap(data, other.data);
		}

		inline size_type capacity() const {
			return capacityVal;
		}

		inline bool operator==(const UnorderedMap& other) const {
			if (capacityVal != other.capacityVal || sizeVal != other.sizeVal || data != other.data) {
				return false;
			}
			for (auto iter01{ begin() }, iter02{ other.begin() }; iter01 != end(); ++iter01, ++iter02) {
				if (!object_compare()(iter01.operator*().second, iter02.operator*().second)) {
					return false;
				}
			}
			return true;
		}

		void clear() {
			if (data && sizeVal > 0) {
				for (size_t x = 0; x < sentinelVector.size(); ++x) {
					if (sentinelVector[x] > 0) {
						getAlloc().destroy(data + x);
					}
				}
				getAlloc().deallocate(data, capacityVal);
				sentinelVector.clear();
				sizeVal		= 0;
				capacityVal = 0;
				data		= nullptr;
			}
		}

		inline ~UnorderedMap() {
			clear();
		};

	  protected:
		Jsonifier::Vector<int8_t> sentinelVector{};
		int8_t maxLookAheadDistance{ minLookups };
		size_type capacityVal{};
		size_type sizeVal{};
		value_type* data{};

		template<typename key_type_new, typename... mapped_type_new> inline iterator emplaceInternal(key_type_new&& key, mapped_type_new&&... value) {
			if (full() || capacityVal == 0) {
				resize(capacityVal + 1);
			}
			auto currentIndex = getHashPolicy().indexForHash(key);
			for (size_type x{}; x < maxLookAheadDistance; ++x, ++currentIndex) {
				if (sentinelVector[currentIndex] == 0) {
					new (std::addressof(data[currentIndex])) value_type{ std::forward<key_type_new>(key), std::forward<mapped_type_new>(value)... };
					sentinelVector[currentIndex] = 1;
					sizeVal++;
					return { this, currentIndex };
				} else if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
					if constexpr ((( !std::is_void_v<mapped_type_new> ) || ...)) {
						new (std::addressof(data[currentIndex])) value_type{ std::forward<key_type_new>(key), std::forward<mapped_type_new>(value)... };
					}
					return { this, currentIndex };
				}
			}
			resize(capacityVal + 1);
			return emplaceInternal(std::forward<key_type_new>(key), std::forward<mapped_type_new>(value)...);
		}

		inline const allocator& getAlloc() const {
			return *this;
		}

		inline const hash_policy& getHashPolicy() const {
			return *this;
		}

		inline void resize(size_type capacityNew) {
			auto newSize = getHashPolicy().nextPowerOfTwo(capacityNew);
			if (newSize > capacityVal) {
				Jsonifier::Vector<int8_t> oldSentinelVector = std::move(sentinelVector);
				auto oldMaxLookAheadDistance				= maxLookAheadDistance;
				auto oldCapacity							= capacityVal;
				auto oldSize								= sizeVal;
				auto oldPtr									= data;
				maxLookAheadDistance						= getHashPolicy().computeMaxLookAheadDistance(newSize);
				sizeVal										= 0;
				data										= getAlloc().allocate(newSize + 1 + maxLookAheadDistance);
				sentinelVector.resize(newSize + 1 + maxLookAheadDistance);
				sentinelVector[newSize + maxLookAheadDistance] = -1;
				capacityVal									   = newSize;
				for (size_type x = 0, y = 0; y < oldSize; ++x) {
					if (oldSentinelVector[x] > 0) {
						++y;
						emplaceInternal(std::move(oldPtr[x].first), std::move(oldPtr[x].second));
					}
				}
				if (oldPtr && oldCapacity) {
					getAlloc().deallocate(oldPtr, oldCapacity + 1 + oldMaxLookAheadDistance);
				}
			}
		}
	};


}