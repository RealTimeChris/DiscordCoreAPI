/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// Https.hpp - Header file for the "Https Stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file Https.hpp

#pragma once

#include <memory_resource>
#include <shared_mutex>
#include <vector>
#include <stack>

#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(no_unique_address)
		#ifdef _MSC_VER
			#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
		#else
			#define NO_UNIQUE_ADDRESS [[no_unique_address]]
		#endif
	#else
		#define NO_UNIQUE_ADDRESS
	#endif
#else
	#define NO_UNIQUE_ADDRESS
#endif

namespace DiscordCoreAPI {

	template<typename OTy = void> struct Fnv1aHash {
		inline uint64_t operator()(const OTy& data) const;

		inline uint64_t operator()(OTy&& data) const;

	  protected:
		static constexpr uint64_t fnvOffsetBasis{ 14695981039346656037ull };
		static constexpr uint64_t fnvPrime{ 1099511628211ull };

		inline uint64_t internalHashFunction(const uint8_t* value, size_t count) const;
	};

	template<typename KTy, typename OTy> struct KeyAccessor {
		KTy& operator()(OTy& other) {
			return other.id;
		}

		KTy& operator()(OTy&& other) {
			return other.id;
		}
	};

	template<typename KTy, typename OTy, typename KATy = KeyAccessor<KTy, OTy>> class MemoryCore {
	  public:
		using key_type = KTy;
		using value_type = OTy;
		using key_accessor = KATy;
		using reference = value_type&;
		using const_reference = const reference;
		using pointer = value_type*;
		using size_type = size_t;
		using key_hasher = Fnv1aHash<key_type>;

		class MemoryCoreIterator {
		  public:
			using value_type = typename MemoryCore::value_type;
			using reference = typename MemoryCore::reference;
			using pointer = typename MemoryCore::pointer;
			using iterator_category = std::forward_iterator_tag;

			inline MemoryCoreIterator(MemoryCore* core, size_t index) : core(core), index(index) {
			}

			inline MemoryCoreIterator& operator++() {
				index++;
				while (index < core->capacity && !core->data[index].operator bool()) {
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

			inline reference operator*() const {
				return core->data[index];
			}

		  protected:
			MemoryCore* core;
			size_t index;
		};

		using iterator = MemoryCoreIterator;

		class SentinelHolder {
		  public:
			inline SentinelHolder() noexcept = default;

			inline SentinelHolder& operator=(SentinelHolder&& other) = delete;
			inline SentinelHolder(SentinelHolder&& other) = delete;
			inline SentinelHolder& operator=(const SentinelHolder& other) = delete;
			inline SentinelHolder(const SentinelHolder& other) = delete;

			inline operator bool() const noexcept {
				return isItActive;
			}

			inline operator reference() noexcept {
				return object;
			}

			inline void activate(value_type&& data) noexcept {
				object = std::forward<value_type>(data);
				isItActive = true;
			}

			inline value_type&& disable() noexcept {
				isItActive = false;
				return std::move(object);
			}

		  protected:
			bool isItActive{ false };
			value_type object{};
		};

		using sentinel_allocator = std::pmr::polymorphic_allocator<SentinelHolder>;

		inline MemoryCore(size_type newCapacity) : capacity(newCapacity), size(0), data(allocator.allocate(newCapacity)) {
			for (size_t x = 0; x < newCapacity; ++x) {
				allocator.construct(&data[x]);
			}
		};

		inline MemoryCore& operator=(MemoryCore&& other) noexcept {
			if (this != &other) {
				pointer* tmpData = data;
				size_t tmpCapacity = capacity;
				size_t tmpSize = size;
				data = other.data;
				capacity = other.capacity;
				size = other.size;
				other.data = tmpData;
				other.capacity = tmpCapacity;
				other.size = tmpSize;
			}
			return *this;
		}

		inline MemoryCore(MemoryCore&& other) noexcept {
			*this = std::move(other);
		}

		inline MemoryCore& operator=(const MemoryCore& other) noexcept {
			if (this != &other) {
				MemoryCore<key_type, value_type> newData{ other.capacity };
				for (size_t x = 0; x < other.capacity; ++x) {
					if (other.data[x].operator bool()) {
						newData.emplace(other.data[x].disable());
					}
				}
				std::swap(this->capacity, newData.capacity);
				std::swap(this->data, newData.data);
				std::swap(this->size, newData.size);
			}
			return *this;
		}

		inline MemoryCore(const MemoryCore& other) noexcept {
			*this = other;
		}

		inline void emplace(value_type&& value) noexcept {
			emplaceInternal(std::forward<value_type>(value));
		}

		inline void emplace(const value_type& value) noexcept {
			emplaceInternal(value);
		}

		inline iterator find(key_type&& key) noexcept {
			size_type index = key_hasher{}(key) % capacity;
			size_type currentIndex{};
			while (currentIndex < capacity) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					return iterator{ this, index };
				}
				index = (index + 1) % capacity;
				++currentIndex;
			}

			return end();
		}

		inline iterator find(const key_type& key) noexcept {
			size_type index = key_hasher{}(key) % capacity;
			size_type currentIndex{};
			while (currentIndex < capacity) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					return iterator{ this, index };
				}
				index = (index + 1) % capacity;
				++currentIndex;
			}

			return end();
		}

		inline bool contains(key_type&& key) noexcept {
			size_type index = key_hasher{}(key) % capacity;
			size_type currentIndex{};
			while (currentIndex < capacity) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					return true;
				}
				index = (index + 1) % capacity;
				++currentIndex;
			}

			return false;
		}

		inline bool contains(const key_type& key) noexcept {
			size_type index = key_hasher{}(key) % capacity;
			size_type currentIndex{};
			while (currentIndex < capacity) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					return true;
				}
				index = (index + 1) % capacity;
				++currentIndex;
			}

			return false;
		}

		inline void erase(key_type&& key) noexcept {
			size_type index = key_hasher{}(key) % capacity;
			size_type currentIndex{};
			while (currentIndex < capacity) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					data[index].disable();
					size--;
					if (size < capacity / 4 && capacity > 10) {
						resize(capacity / 2);
					}
					return;
				}
				index = (index + 1) % capacity;
				++currentIndex;
			}
		}

		inline void erase(const key_type& key) noexcept {
			size_type index = key_hasher{}(key) % capacity;
			size_type currentIndex{};
			while (currentIndex < capacity) {
				if (data[index].operator bool() && key_accessor{}(data[index].operator reference()) == key) {
					data[index].disable();
					size--;
					if (size < capacity / 4 && capacity > 10) {
						resize(capacity / 2);
					}
					return;
				}
				index = (index + 1) % capacity;
				++currentIndex;
			}
		}

		inline iterator begin() noexcept {
			size_type index{};
			while (index < capacity) {
				if (data[index].operator bool()) {
					return iterator{ this, index };
				}
				++index;
			}
			return end();
		}

		inline iterator end() noexcept {
			return iterator{ this, capacity };
		}

		inline iterator begin() const noexcept {
			size_type index{};
			while (index < capacity) {
				if (data[index].operator bool()) {
					return iterator{ this, index };
				}
				++index;
			}
			return end();
		}

		inline iterator end() const noexcept {
			return iterator{ this, capacity };
		}

		inline size_type getSize() const noexcept {
			return size;
		}

		inline bool isEmpty() const noexcept {
			return size == 0;
		}

		inline bool isFull() const noexcept {
			return static_cast<float>(size) >= static_cast<float>(capacity) * 0.75f;
		}

		inline void reserve(size_t newSize) noexcept {
			this->resize(newSize);
		}

		inline size_t getCapacity() noexcept {
			return this->capacity;
		}

		inline ~MemoryCore() noexcept {
			if (data && capacity > 0) {
				allocator.deallocate(data, capacity);
			}
		};

	  protected:
		NO_UNIQUE_ADDRESS sentinel_allocator allocator{};
		SentinelHolder* data{};
		size_type capacity{};
		size_type size{};

		inline void emplaceInternal(value_type&& value, uint64_t recursionLimit = 1000) noexcept {
			if (isFull()) {
				resize(roundUpToCacheLine(capacity * 4), recursionLimit);
			}
			size_type index = key_hasher{}(key_accessor{}(value)) % capacity;
			size_type currentIndex = index;
			bool inserted = false;
			while (!inserted) {
				if (!data[currentIndex].operator bool()) {
					data[currentIndex].activate(std::forward<value_type>(value));
					size++;
					inserted = true;
				} else if (key_accessor{}(data[currentIndex].operator reference()) == key_accessor{}(value)) {
					data[currentIndex].activate(std::forward<value_type>(value));
					inserted = true;
				} else {
					currentIndex = (currentIndex + 1) % capacity;
					if (currentIndex == index) {
						resize(roundUpToCacheLine(capacity * 4), recursionLimit);
						emplaceInternal(std::forward<value_type>(value), recursionLimit);
						return;
					}
				}
			}
		}

		inline void emplaceInternal(const value_type& value, uint64_t recursionLimit = 1000) noexcept {
			if (isFull()) {
				resize(roundUpToCacheLine(capacity * 4), recursionLimit);
			}
			size_type index = key_hasher{}(key_accessor{}(value)) % capacity;
			size_type currentIndex = index;
			bool inserted = false;
			value_type newElement{ value };
			while (!inserted) {
				if (!data[currentIndex].operator bool()) {
					data[currentIndex].activate(std::move(newElement));
					size++;
					inserted = true;
				} else if (key_accessor{}(data[currentIndex].operator reference()) == key_accessor{}(value)) {
					data[currentIndex].activate(std::move(newElement));
					inserted = true;
				} else {
					currentIndex = (currentIndex + 1) % capacity;
					if (currentIndex == index) {
						resize(roundUpToCacheLine(capacity * 4), recursionLimit);
						emplaceInternal(value, recursionLimit);
						return;
					}
				}
			}
		}

		inline void resize(size_type newCapacity, uint64_t recursionLimit = 1000) {
			--recursionLimit;
			if (recursionLimit == 0) {
				throw std::runtime_error{ "Sorry, but the max number of recursive resizes have been exceeded." };
			}
			MemoryCore<key_type, value_type> newData{ newCapacity };
			for (size_type x = 0; x < capacity; x++) {
				if (data[x].operator bool()) {
					newData.emplaceInternal(data[x].disable(), recursionLimit);
				}
			}
			std::swap(data, newData.data);
			std::swap(capacity, newData.capacity);
		}

		size_type roundUpToCacheLine(size_type size) {
			const size_type multiple = 64 / sizeof(void*);
			return (size + multiple - 1) / multiple * multiple;
		}
	};

	template<typename KTy, typename OTy, typename KATy = KeyAccessor<KTy, OTy>> class UnorderedSet {
	  public:
		using key_type = KTy;
		using value_type = OTy;
		using reference = value_type&;
		using const_reference = const reference;
		using pointer = value_type*;
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
			data.emplace(std::forward<value_type>(value));
		}

		inline void emplace(const value_type& value) noexcept {
			data.emplace(value);
		}

		inline bool contains(key_type&& key) noexcept {
			return data.contains(std::forward<key_type>(key));
		}

		inline bool contains(const key_type& key) noexcept {
			return data.contains(key);
		}

		inline void erase(key_type&& key) noexcept {
			data.erase(std::forward<key_type>(key));
		}

		inline void erase(const key_type& key) noexcept {
			data.erase(key);
		}

		inline iterator find(key_type&& key) noexcept {
			return data.find(std::forward<key_type>(key));
		}

		inline iterator find(const key_type& key) noexcept {
			return data.find(key);
		}

		inline reference operator[](key_type&& key) noexcept {
			return *data.find(std::forward<key_type>(key));
		}

		inline reference operator[](const key_type& key) noexcept {
			return *data.find(key);
		}

		inline size_type size() const noexcept {
			return data.getSize();
		}

		inline void reserve(size_t newSize) noexcept {
			data.reserve(newSize);
		}

		inline bool empty() const noexcept {
			return data.isEmpty();
		}

		inline size_type capacity() const noexcept {
			return data.getCapacity();
		}

	  protected:
		MemoryCore<key_type, value_type> data{};
	};

}