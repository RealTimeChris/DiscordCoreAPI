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

	template<typename KeyType, typename ValueType> class UnorderedMap
		: protected PowerOfTwoHashPolicy<UnorderedMap<KeyType, ValueType>>,
		  protected JsonifierInternal::AllocWrapper<ObjectCore<Pair<KeyType, ValueType>>>,
		  protected ObjectCompare,
		  protected KeyHasher {
	  public:
		using mapped_type = ValueType;
		using key_type = KeyType;
		using reference = mapped_type&;
		using value_type = Pair<key_type, mapped_type>;
		using value_type_internal = ObjectCore<Pair<key_type, mapped_type>>;
		using const_reference = const mapped_type&;
		using size_type = uint64_t;
		using key_hasher = KeyHasher;
		using object_compare = ObjectCompare;
		using hash_policy = PowerOfTwoHashPolicy<UnorderedMap<key_type, mapped_type>>;
		friend hash_policy;

		using iterator = HashIterator<value_type_internal>;
		friend iterator;
		using const_iterator = const HashIterator<value_type_internal>;
		friend const_iterator;

		using allocator = JsonifierInternal::AllocWrapper<value_type_internal>;

		inline UnorderedMap(size_type capacityNew = 5) {
			reserve(capacityNew);
		};

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
			if (capacityVal > 0) {
				auto currentEntry = data + hash_policy::indexForHash(key_hasher()(key));
				for (size_type x{}; x < maxLookupDistance; ++x, ++currentEntry) {
					if (currentEntry->areWeActive() && object_compare()(currentEntry->value.first, key)) {
						return { currentEntry };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator find(key_type_new&& key) {
			if (capacityVal > 0) {
				auto currentEntry = data + hash_policy::indexForHash(key_hasher()(key));
				for (size_type x{}; x < maxLookupDistance; ++x, ++currentEntry) {
					if (currentEntry->areWeActive() && object_compare()(currentEntry->value.first, key)) {
						return { currentEntry };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline const_reference operator[](key_type_new&& key) const {
			auto iter = find(key);
			if (iter == end()) {
				iter = emplace(key, mapped_type{});
			}
			return iter->second;
		}

		template<typename key_type_new> inline reference operator[](key_type_new&& key) {
			auto iter = find(key);
			if (iter == end()) {
				iter = emplace(key, mapped_type{});
			}
			return iter->second;
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
			if (capacityVal > 0) {
				auto currentEntry = data + hash_policy::indexForHash(key_hasher()(key));
				for (size_type x{}; x < maxLookupDistance; ++x, ++currentEntry) {
					if (currentEntry->areWeActive() && object_compare()(currentEntry->value.first, key)) {
						return true;
					}
				}
			}
			return false;
		}

		template<MapContainerIteratorT<key_type, mapped_type> MapIterator> inline iterator erase(MapIterator&& iter) {
			if (capacityVal > 0) {
				auto currentEntry = data + static_cast<size_type>(iter.getRawPtr() - data);
				for (size_type x{}; x < maxLookupDistance; ++x, ++currentEntry) {
					if (currentEntry->areWeActive() && object_compare()(currentEntry->value.first, iter.operator*().first)) {
						currentEntry->disable();
						sizeVal--;
						return { ++currentEntry };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator erase(key_type_new&& key) {
			if (capacityVal > 0) {
				auto currentEntry = data + hash_policy::indexForHash(key_hasher()(key));
				for (size_type x{}; x < maxLookupDistance; ++x, ++currentEntry) {
					if (currentEntry->areWeActive() && object_compare()(currentEntry->value.first, key)) {
						currentEntry->disable();
						sizeVal--;
						return { ++currentEntry };
					}
				}
			}
			return end();
		}

		inline const_iterator begin() const {
			for (size_type x{ 0 }; x < capacityVal; ++x) {
				if (data[x].areWeActive()) {
					return { data + x };
				}
			}
			return end();
		}

		inline const_iterator end() const {
			return { data + capacityVal };
		}

		inline iterator begin() {
			for (size_type x{ 0 }; x < capacityVal; ++x) {
				if (data[x].areWeActive()) {
					return { data + x };
				}
			}
			return end();
		}

		inline iterator end() {
			return { data + capacityVal };
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
			if (data && capacityVal > 0) {
				std::destroy(data, data + capacityVal);
				allocator::deallocate(data, capacityVal);
				sizeVal = 0;
				capacityVal = 0;
				data = nullptr;
			}
		}

		inline ~UnorderedMap() {
			clear();
		};

	  protected:
		value_type_internal* data{};
		size_type capacityVal{};
		size_type sizeVal{};

		template<typename key_type_new, typename mapped_type_new> inline iterator emplaceInternal(key_type_new&& key, mapped_type_new&& value) {
			if (full() || capacityVal == 0) {
				resize(static_cast<uint64_t>(static_cast<double>(capacityVal) * (static_cast<float>(maxLookupDistance) / 2.0f) + 2.0f));
			}
			auto currentEntry = data + hash_policy::indexForHash(key_hasher()(key));
			for (size_type x{}; x < maxLookupDistance; ++x, ++currentEntry) {
				if (currentEntry->areWeEmpty()) {
					currentEntry->enable(std::forward<key_type_new>(key), std::forward<mapped_type_new>(value));
					sizeVal++;
					return currentEntry;
				} else if (currentEntry->areWeActive() && object_compare()(currentEntry->value.first, key)) {
					currentEntry->value.second = std::forward<mapped_type_new>(value);
					return currentEntry;
				}
			}
			resize(static_cast<uint64_t>(static_cast<double>(capacityVal) * (static_cast<float>(maxLookupDistance) / 2.0f)));
			return emplaceInternal(std::forward<key_type_new>(key), std::forward<mapped_type_new>(value));
		}

		inline void resize(size_type capacityNew) {
			auto newSize = hash_policy::nextSizeOver(capacityNew);
			if (newSize > capacityVal) {
				auto oldPtr = data;
				auto oldCapacity = capacityVal;
				auto oldSize = sizeVal;
				sizeVal = 0;
				data = allocator::allocate(newSize + 1 + maxLookupDistance);
				std::uninitialized_default_construct(data, data + newSize + 1 + maxLookupDistance);
				capacityVal = newSize;
				new (data + capacityVal) value_type_internal{ ObjectCore<value_type>{ -1 } };
				for (size_type x = 0, y = 0; x < oldCapacity && y < oldSize; ++x) {
					if (oldPtr[x].areWeActive()) {
						++y;
						emplaceInternal(std::move(oldPtr[x].value.first), std::move(oldPtr[x].value.second));
					}
				}
				if (oldPtr && oldCapacity) {
					allocator::deallocate(oldPtr, oldCapacity + 1 + maxLookupDistance);
				}
			}
		}
	};


}