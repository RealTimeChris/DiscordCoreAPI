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

#include <discordcoreapi/Utilities/UniquePtr.hpp>
#include <discordcoreapi/Utilities/Base.hpp>

#include <memory_resource>
#include <shared_mutex>
#include <vector>
#include <mutex>

namespace DiscordCoreAPI {

	template<typename ValueType> struct Fnv1aHash;

	struct DiscordCoreAPI_Dll Fnv1aHashFunction {
		inline static uint64_t internalHashFunction(const void* value, uint64_t count) {
			static constexpr uint64_t fnvOffsetBasis{ 14695981039346656037ull };
			static constexpr uint64_t fnvPrime{ 1099511628211ull };
			uint64_t hash = fnvOffsetBasis;
			for (uint64_t x = 0; x < count; ++x) {
				hash ^= static_cast<const uint8_t*>(value)[x];
				hash *= fnvPrime;
			}
			return hash;
		}
	};

	template<> struct Fnv1aHash<std::string> : public Fnv1aHashFunction {
		inline static uint64_t getHash(const std::string& data) {
			return internalHashFunction(data.data(), data.size());
		}
	};

	template<> struct Fnv1aHash<uint64_t> : public Fnv1aHashFunction {
		inline static uint64_t getHash(const uint64_t& data) {
			return internalHashFunction(&data, sizeof(data));
		}
	};

	template<> struct Fnv1aHash<Snowflake> : public Fnv1aHashFunction {
		inline static uint64_t getHash(const Snowflake& data) {
			auto newValue = data.operator uint64_t();
			return internalHashFunction(&newValue, sizeof(newValue));
		}
	};

	template<typename KeyType, typename ValueType> struct KeyAccessor {
		inline static const KeyType& getKey(const ValueType& value) {
			return value.id;
		}
	};

	template<typename KeyType, typename ValueType> class UnorderedSet {
	  public:
		using key_type = KeyType;
		using value_type = ValueType;
		using key_accessor = KeyAccessor<key_type, value_type>;
		using reference = value_type&;
		using size_type = uint64_t;
		using key_hasher = Fnv1aHash<key_type>;

		class Iterator {
		  public:
			using iterator_category = std::forward_iterator_tag;

			inline Iterator(const UnorderedSet* core, size_type index) : core(core), index(index) {
			}

			inline Iterator& operator++() {
				do {
					index++;
				} while (index < core->capacityVal && !core->data[index].operator bool());
				return *this;
			}

			inline bool operator==(const Iterator& other) const {
				return core == other.core && index == other.index;
			}

			inline reference operator*() const {
				return core->data[index];
			}

			inline reference operator*() {
				return core->data[index];
			}

		  protected:
			const UnorderedSet* core;
			size_type index;
		};

		using iterator = Iterator;
		using const_iterator = const iterator;

		class SentinelHolder {
		  public:
			inline SentinelHolder() = default;

			inline SentinelHolder& operator=(SentinelHolder&& other) = delete;
			inline SentinelHolder(SentinelHolder&& other) = delete;
			inline SentinelHolder& operator=(const SentinelHolder& other) = delete;
			inline SentinelHolder(const SentinelHolder& other) = delete;

			inline operator bool() const {
				return areWeActive;
			}

			inline operator reference() {
				return object;
			}

			inline void activate(value_type&& data) {
				object = std::move(data);
				areWeActive = true;
			}

			inline value_type disable() {
				areWeActive = false;
				return std::move(object);
			}

		  protected:
			value_type object{};
			bool areWeActive{};
		};

		using allocator = AllocWrapper<SentinelHolder>;

		inline UnorderedSet() : capacityVal(5), data(allocator{}.allocate(5)), sizeVal(0) {
			std::uninitialized_default_construct_n(data, 5);
		};

		inline UnorderedSet(size_type capacityNew) : capacityVal(capacityNew), data(allocator{}.allocate(capacityNew)), sizeVal(0) {
			std::uninitialized_default_construct_n(data, capacityNew);
		};

		inline UnorderedSet& operator=(UnorderedSet&& other) {
			if (this != &other) {
				std::swap(capacityVal, other.capacityVal);
				std::swap(data, other.data);
				std::swap(sizeVal, other.sizeVal);
			}
			return *this;
		}

		inline UnorderedSet& operator=(const UnorderedSet& other) = delete;
		inline UnorderedSet(const UnorderedSet& other) = delete;

		inline void emplace(value_type&& value) {
			emplaceInternal(std::move(value));
		}

		inline const_iterator find(const key_type& key) const {
			size_type index = key_hasher::getHash(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor::getKey(data[index].operator reference()) == key) {
					return const_iterator{ this, index };
				}
				index = (index + 1) % capacityVal;
			}

			return end();
		}

		inline iterator find(key_type&& key) {
			size_type index = key_hasher::getHash(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor::getKey(data[index].operator reference()) == key) {
					return iterator{ this, index };
				}
				index = (index + 1) % capacityVal;
			}

			return end();
		}

		inline bool contains(const key_type& key) const {
			size_type index = key_hasher::getHash(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor::getKey(data[index].operator reference()) == key) {
					return true;
				}
				index = (index + 1) % capacityVal;
			}

			return false;
		}

		inline bool contains(key_type&& key) {
			size_type index = key_hasher::getHash(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor::getKey(data[index].operator reference()) == key) {
					return true;
				}
				index = (index + 1) % capacityVal;
			}

			return false;
		}

		inline void erase(const key_type& key) {
			size_type index = key_hasher::getHash(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor::getKey(data[index].operator reference()) == key) {
					data[index].disable();
					sizeVal--;
					if (sizeVal < capacityVal / 4 && capacityVal > 10) {
						resize(capacityVal / 2);
					}
					return;
				}
				index = (index + 1) % capacityVal;
			}
		}

		inline void erase(key_type&& key) {
			size_type index = key_hasher::getHash(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor::getKey(data[index].operator reference()) == key) {
					data[index].disable();
					sizeVal--;
					if (sizeVal < capacityVal / 4 && capacityVal > 10) {
						resize(capacityVal / 2);
					}
					return;
				}
				index = (index + 1) % capacityVal;
			}
		}

		inline const_iterator begin() const {
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[currentIndex].operator bool()) {
					return iterator{ this, currentIndex };
				}
			}
			return end();
		}

		inline const_iterator end() const {
			return iterator{ this, capacityVal };
		}

		inline iterator begin() {
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[currentIndex].operator bool()) {
					return iterator{ this, currentIndex };
				}
			}
			return end();
		}

		inline iterator end() {
			return iterator{ this, capacityVal };
		}

		inline bool full() const {
			return static_cast<float>(sizeVal) >= static_cast<float>(capacityVal) * 0.70f;
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

		inline size_type capacity() const {
			return capacityVal;
		}

		inline ~UnorderedSet() {
			if (data && capacityVal > 0) {
				std::destroy_n(data, capacityVal);
				allocator{}.deallocate(data, capacityVal);
			}
		};

	  protected:
		size_type capacityVal{};
		SentinelHolder* data{};
		size_type sizeVal{};

		inline void emplaceInternal(value_type&& value, size_type recursionLimit = 1000) {
			if (full()) {
				resize(roundUpToCacheLine(static_cast<uint64_t>(static_cast<float>(capacityVal) * 2.0)), recursionLimit);
			}
			auto keyVal = key_accessor::getKey(value);
			size_type index = key_hasher::getHash(keyVal) % capacityVal;
			size_type currentIndex = index;
			bool inserted = false;
			while (!inserted) {
				if (!data[currentIndex].operator bool()) {
					data[currentIndex].activate(std::move(value));
					sizeVal++;
					inserted = true;
				} else if (key_accessor::getKey(data[currentIndex].operator reference()) == keyVal) {
					data[currentIndex].activate(std::move(value));
					inserted = true;
				} else {
					currentIndex = (currentIndex + 1) % capacityVal;
					if (currentIndex == index) {
						resize(roundUpToCacheLine(capacityVal * 4), recursionLimit);
						emplaceInternal(std::move(value), recursionLimit);
						return;
					}
				}
			}
		}

		inline void resize(size_type capacityNew, size_type recursionLimit = 1000) {
			--recursionLimit;
			if (recursionLimit == 0) {
				throw std::runtime_error{ "Sorry, but the max number of recursive resizes have been exceeded." };
			}
			UnorderedSet<key_type, value_type> newData{ capacityNew };
			for (size_type x = 0; x < capacityVal; x++) {
				if (data[x].operator bool()) {
					newData.emplaceInternal(data[x].disable(), recursionLimit);
				}
			}
			std::swap(data, newData.data);
			std::swap(sizeVal, newData.sizeVal);
			std::swap(capacityVal, newData.capacityVal);
		}

		inline size_type roundUpToCacheLine(size_type sizeValNew) {
			const size_type multiple = 64 / sizeof(void*);
			return (sizeValNew + multiple - 1) / multiple * multiple;
		}
	};
}
