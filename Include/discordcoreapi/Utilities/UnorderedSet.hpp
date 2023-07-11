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

#include <discordcoreapi/Base.hpp>

#include <memory_resource>
#include <shared_mutex>
#include <vector>
#include <mutex>

namespace DiscordCoreAPI {

	template<typename ValueType = void> struct Fnv1aHash {
		inline uint64_t operator()(const ValueType& data) const;

		inline uint64_t operator()(ValueType&& data) const;

	  protected:
		uint64_t internalHashFunction(const uint8_t* value, uint64_t count) const {
			static constexpr size_t fnvOffsetBasis{ 14695981039346656037ull };
			static constexpr size_t fnvPrime{ 1099511628211ull };
			auto hash = fnvOffsetBasis;
			for (uint64_t x = 0; x < count; ++x) {
				hash ^= value[x];
				hash *= fnvPrime;
			}
			return hash;
		}
	};

	template<> struct Fnv1aHash<std::string> {
		inline uint64_t operator()(const std::string& data) const {
			return internalHashFunction(data.data(), data.size());
		}

		inline uint64_t operator()(std::string&& data) {
			return internalHashFunction(data.data(), data.size());
		}

	  protected:
		uint64_t internalHashFunction(const char* value, uint64_t count) const {
			static constexpr size_t fnvOffsetBasis{ 14695981039346656037ull };
			static constexpr size_t fnvPrime{ 1099511628211ull };
			auto hash = fnvOffsetBasis;
			for (uint64_t x = 0; x < count; ++x) {
				hash ^= value[x];
				hash *= fnvPrime;
			}
			return hash;
		}
	};

	template<> struct Fnv1aHash<int64_t> {
		inline uint64_t operator()(const int64_t& data) const {
			return internalHashFunction(&data, sizeof(data));
		}

		inline uint64_t operator()(int64_t&& data) {
			return internalHashFunction(&data, sizeof(data));
		}

	  protected:
		uint64_t internalHashFunction(const void* value, uint64_t count) const {
			static constexpr size_t fnvOffsetBasis{ 14695981039346656037ull };
			static constexpr size_t fnvPrime{ 1099511628211ull };
			auto hash = fnvOffsetBasis;
			for (uint64_t x = 0; x < count; ++x) {
				hash ^= static_cast<const char*>(value)[x];
				hash *= fnvPrime;
			}
			return hash;
		}
	};

	template<typename KeyType, typename ValueType> struct KeyAccessor {
		inline const KeyType& operator()(const ValueType& other);

		inline KeyType& operator()(ValueType&& other);
	};

	template<typename KeyType, typename ValueType, typename KATy = KeyAccessor<KeyType, ValueType>> class MemoryCore {
	  public:
		using key_type = KeyType;
		using value_type = ValueType;
		using key_accessor = KATy;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = size_t;
		using key_hasher = Fnv1aHash<key_type>;

		class MemoryCoreIterator {
		  public:
			using iterator_category = std::forward_iterator_tag;
			using reference = typename MemoryCore::reference;

			inline MemoryCoreIterator(const MemoryCore* core, size_type index) : core(core), index(index) {
			}

			inline MemoryCoreIterator& operator++() {
				index++;
				while (index < core->capacityVal && !core->data[index].operator bool()) {
					index++;
				}
				return *this;
			}

			inline bool operator==(const MemoryCoreIterator& other) const {
				return core == other.core && index == other.index;
			}

			inline bool operator!=(const MemoryCoreIterator& other) const {
				return !(*this == other);
			}

			inline const_reference operator*() const {
				return core->data[index];
			}

			inline reference operator*() {
				return core->data[index];
			}

		  protected:
			const MemoryCore* core;
			size_type index;
		};

		using iterator = MemoryCoreIterator;
		using const_iterator = const iterator;

		class SentinelHolder {
		  public:
			inline SentinelHolder() noexcept = default;

			inline SentinelHolder& operator=(SentinelHolder&& other) = delete;
			inline SentinelHolder(SentinelHolder&& other) = delete;
			inline SentinelHolder& operator=(const SentinelHolder& other) = delete;
			inline SentinelHolder(const SentinelHolder& other) = delete;

			inline operator bool() const noexcept {
				return areWeActive;
			}

			inline operator const_reference() const noexcept {
				return object;
			}

			inline operator reference() noexcept {
				return object;
			}

			inline void activate(value_type&& data) noexcept {
				object = std::move(data);
				areWeActive = true;
			}

			inline value_type&& disable() noexcept {
				areWeActive = false;
				return std::move(object);
			}

		  protected:
			value_type object{};
			bool areWeActive{};
		};

		using allocator = AllocWrapper<SentinelHolder>;

		inline MemoryCore(size_type capacityNew) : capacityVal(capacityNew), sizeVal(0), data(allocator{}.allocate(capacityNew)) {
			std::uninitialized_default_construct_n(data, capacityNew);
		};

		inline MemoryCore& operator=(MemoryCore&& other) noexcept {
			if (this != &other) {
				std::swap(capacityVal, other.capacityVal);
				std::swap(data, other.data);
				std::swap(sizeVal, other.sizeVal);
			}
			return *this;
		}

		inline MemoryCore& operator=(const MemoryCore& other) noexcept = delete;
		inline MemoryCore(const MemoryCore& other) noexcept = delete;

		inline void emplace(value_type&& value) noexcept {
			emplaceInternal(std::move(value));
		}

		inline iterator find(key_type&& key) noexcept {
			size_type index = key_hasher{}(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					return iterator{ this, index };
				}
				index = (index + 1) % capacityVal;
			}

			return end();
		}

		inline const_iterator find(const key_type& key) const noexcept {
			size_type index = key_hasher{}(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator const_reference()) == key) {
					return const_iterator{ this, index };
				}
				index = (index + 1) % capacityVal;
			}

			return end();
		}

		inline bool contains(key_type&& key) noexcept {
			size_type index = key_hasher{}(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					return true;
				}
				index = (index + 1) % capacityVal;
			}

			return false;
		}

		inline bool contains(const key_type& key) const noexcept {
			size_type index = key_hasher{}(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator const_reference()) == key) {
					return true;
				}
				index = (index + 1) % capacityVal;
			}

			return false;
		}

		inline void erase(key_type&& key) noexcept {
			size_type index = key_hasher{}(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
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

		inline void erase(const key_type& key) noexcept {
			size_type index = key_hasher{}(key) % capacityVal;
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
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

		inline iterator begin() noexcept {
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[currentIndex].operator bool()) {
					return iterator{ this, currentIndex };
				}
			}
			return end();
		}

		inline iterator end() noexcept {
			return iterator{ this, capacityVal };
		}

		inline const_iterator begin() const noexcept {
			for (size_type currentIndex{ 0 }; currentIndex < capacityVal; ++currentIndex) {
				if (data[currentIndex].operator bool()) {
					return iterator{ this, currentIndex };
				}
			}
			return end();
		}

		inline const_iterator end() const noexcept {
			return iterator{ this, capacityVal };
		}

		inline size_type size() const noexcept {
			return sizeVal;
		}

		inline bool empty() const noexcept {
			return sizeVal == 0;
		}

		inline bool full() const noexcept {
			return static_cast<float>(sizeVal) >= static_cast<float>(capacityVal) * 0.75f;
		}

		inline void reserve(size_type sizeNew) noexcept {
			resize(sizeNew);
		}

		inline size_type capacity() const noexcept {
			return capacityVal;
		}

		inline ~MemoryCore() noexcept {
			if (data && capacityVal > 0) {
				std::destroy_n(data, capacityVal);
				allocator{}.deallocate(data, capacityVal);
			}
		};

	  protected:
		size_type capacityVal{};
		SentinelHolder* data{};
		size_type sizeVal{};

		inline void emplaceInternal(value_type&& value, size_type recursionLimit = 1000) noexcept {
			if (full()) {
				resize(roundUpToCacheLine(capacityVal * 4), recursionLimit);
			}
			size_type index = key_hasher{}(key_accessor{}(value)) % capacityVal;
			size_type currentIndex = index;
			bool inserted = false;
			while (!inserted) {
				if (!data[currentIndex].operator bool()) {
					data[currentIndex].activate(std::move(value));
					sizeVal++;
					inserted = true;
				} else if (key_accessor{}(data[currentIndex].operator reference()) == key_accessor{}(value)) {
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
			MemoryCore<key_type, value_type> newData{ capacityNew };
			for (size_type x = 0; x < capacityVal; x++) {
				if (data[x].operator bool()) {
					newData.emplaceInternal(data[x].disable(), recursionLimit);
				}
			}
			std::swap(data, newData.data);
			std::swap(sizeVal, newData.sizeVal);
			std::swap(capacityVal, newData.capacityVal);
		}

		inline size_type roundUpToCacheLine(size_type sizeVal) {
			const size_type multiple = 64 / sizeof(void*);
			return (sizeVal + multiple - 1) / multiple * multiple;
		}
	};

	template<typename KeyType, typename ValueType> class UnorderedSet {
	  public:
		using key_type = KeyType;
		using value_type = ValueType;
		using size_type = size_t;
		using iterator = typename MemoryCore<key_type, value_type>::MemoryCoreIterator;

		inline UnorderedSet() : data{ 5 } {};

		inline UnorderedSet& operator=(UnorderedSet&& other) noexcept = default;
		inline UnorderedSet(UnorderedSet&& other) noexcept = default;
		inline UnorderedSet& operator=(const UnorderedSet& other) noexcept = default;
		inline UnorderedSet(const UnorderedSet& other) noexcept = default;

		inline iterator begin() noexcept {
			return iterator(data.begin());
		}

		inline iterator end() noexcept {
			return iterator(data.end());
		}

		inline void emplace(value_type&& value) noexcept {
			data.emplace(std::move(value));
		}

		inline bool contains(key_type&& key) noexcept {
			return data.contains(std::move(key));
		}

		inline bool contains(const key_type& key) const noexcept {
			return data.contains(key);
		}

		inline void erase(key_type&& key) noexcept {
			data.erase(std::move(key));
		}

		inline void erase(const key_type& key) noexcept {
			data.erase(key);
		}

		inline iterator find(key_type&& key) noexcept {
			return data.find(std::move(key));
		}

		inline iterator find(const key_type& key) const noexcept {
			return data.find(key);
		}

		inline size_type size() const noexcept {
			return data.size();
		}

		inline void reserve(size_type sizeNew) noexcept {
			data.reserve(sizeNew);
		}

		inline bool empty() const noexcept {
			return data.empty();
		}

		inline size_type capacity() const noexcept {
			return data.capacity();
		}

	  protected:
		MemoryCore<key_type, value_type> data{};
	};

}
