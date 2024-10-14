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
/// UnorderedSet.hpp - Header file for the unordered_set class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file UnorderedSet.hpp
#pragma once

#include <discordcoreapi/Utilities/Hash.hpp>

namespace discord_core_api {

	template<typename value_type> class unordered_set;

	template<typename set_iterator, typename value_type>
	concept set_container_iterator_t = std::same_as<typename unordered_set<value_type>::iterator, std::remove_cvref_t<set_iterator>>;

	template<typename value_type_new>
	class unordered_set : protected hash_policy<unordered_set<value_type_new>>, protected jsonifier_internal::alloc_wrapper<value_type_new>, protected object_compare {
	  public:
		template<typename value_type_newer> using key_accessor = key_accessor<value_type_newer>;
		using key_type										   = value_type_new;
		using value_type									   = value_type_new;
		using mapped_type									   = value_type;
		using allocator_type								   = jsonifier_internal::alloc_wrapper<value_type>;
		using allocator_traits								   = std::allocator_traits<allocator_type>;
		using size_type										   = uint64_t;
		using difference_type								   = int64_t;
		using pointer										   = typename allocator_traits::pointer;
		using const_pointer									   = typename allocator_traits::const_pointer;
		using reference										   = value_type&;
		using const_reference								   = const value_type&;
		using iterator										   = hash_iterator<unordered_set<value_type>>;
		using const_iterator								   = hash_iterator<const unordered_set<value_type>>;
		using object_compare								   = object_compare;
		using hash_policy_new								   = hash_policy<unordered_set<value_type>>;

		friend hash_policy_new;
		friend iterator;
		friend const_iterator;

		DCA_INLINE unordered_set(){};

		DCA_INLINE unordered_set& operator=(unordered_set&& other) noexcept {
			if (this != &other) {
				reset();
				swap(other);
			}
			return *this;
		}

		DCA_INLINE unordered_set(unordered_set&& other) noexcept {
			*this = std::move(other);
		}

		DCA_INLINE unordered_set& operator=(const unordered_set& other) {
			if (this != &other) {
				reset();

				reserve(other.capacity());
				for (const auto& value: other) {
					emplace(value);
				}
			}
			return *this;
		}

		DCA_INLINE unordered_set(const unordered_set& other) {
			*this = other;
		}

		DCA_INLINE unordered_set(std::initializer_list<value_type> list) {
			reserve(list.size());
			for (auto& value: list) {
				emplace(std::move(value));
			}
		};

		template<typename args> iterator emplace(args&& value) {
			return emplaceInternal(std::forward<args>(value));
		}

		template<typename key_type_new> DCA_INLINE const_iterator find(key_type_new&& key) const {
			if (sizeVal > 0) {
				auto currentIndex = getKey(key) & (capacityVal - 1);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(getKey(data[currentIndex]), getKey(key))) {
						return { this, currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> DCA_INLINE iterator find(key_type_new&& key) {
			if (sizeVal > 0) {
				auto currentIndex = getKey(key) & (capacityVal - 1);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(getKey(data[currentIndex]), getKey(key))) {
						return { this, currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> DCA_INLINE const_reference operator[](key_type_new&& key) const {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				iter = emplace(mapped_type{});
			}
			return *iter;
		}

		template<typename key_type_new> DCA_INLINE reference operator[](key_type_new&& key) {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				iter = emplace(mapped_type{});
			}
			return *iter;
		}

		template<typename key_type_new> DCA_INLINE const_reference at(key_type_new&& key) const {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw std::runtime_error{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return *iter;
		}

		template<typename key_type_new> DCA_INLINE reference at(key_type_new&& key) {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw std::runtime_error{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return *iter;
		}

		template<typename key_type_new> DCA_INLINE bool contains(key_type_new&& key) const {
			if (sizeVal > 0) {
				auto currentIndex = getKey(key) & (capacityVal - 1);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(getKey(data[currentIndex]), getKey(key))) {
						return true;
					}
				}
			}
			return false;
		}

		template<set_container_iterator_t<mapped_type> set_iterator> DCA_INLINE iterator erase(set_iterator&& iter) {
			if (sizeVal > 0) {
				auto currentIndex = static_cast<size_type>(iter.getRawPtr() - data);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(getKey(data[currentIndex]), getKey(iter.operator*()))) {
						sentinelVector[currentIndex] = 0;
						--sizeVal;
						return { this, ++currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_new> DCA_INLINE iterator erase(key_type_new&& key) {
			if (sizeVal > 0) {
				auto currentIndex = getKey(key) & (capacityVal - 1);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(getKey(data[currentIndex]), getKey(key))) {
						sentinelVector[currentIndex] = 0;
						--sizeVal;
						return { this, ++currentIndex };
					}
				}
			}
			return end();
		}

		DCA_INLINE const_iterator begin() const {
			for (size_type x{ 0 }; x < capacityVal; ++x) {
				if (sentinelVector.at(x) > 0) {
					return { this, x };
				}
			}
			return end();
		}

		DCA_INLINE const_iterator end() const {
			return {};
		}

		DCA_INLINE iterator begin() {
			for (size_type x{ 0 }; x < capacityVal; ++x) {
				if (sentinelVector.at(x) > 0) {
					return { this, x };
				}
			}
			return end();
		}

		DCA_INLINE iterator end() {
			return {};
		}

		DCA_INLINE bool full() const {
			return static_cast<float>(sizeVal) >= static_cast<float>(capacityVal) * 0.90f;
		}

		DCA_INLINE size_type size() const {
			return sizeVal;
		}

		DCA_INLINE bool empty() const {
			return sizeVal == 0;
		}

		DCA_INLINE void reserve(size_type sizeNew) {
			resize(sizeNew);
		}

		DCA_INLINE void swap(unordered_set& other) {
			std::swap(maxLookAheadDistance, other.maxLookAheadDistance);
			std::swap(sentinelVector, other.sentinelVector);
			std::swap(capacityVal, other.capacityVal);
			std::swap(sizeVal, other.sizeVal);
			std::swap(data, other.data);
		}

		DCA_INLINE size_type capacity() const {
			return capacityVal;
		}

		DCA_INLINE bool operator==(const unordered_set& other) const {
			if (capacityVal != other.capacityVal || sizeVal != other.sizeVal || data != other.data) {
				return false;
			}
			for (auto iter01{ begin() }, iter02{ other.begin() }; iter01 != end(); ++iter01, ++iter02) {
				if (!object_compare()(iter01.operator*(), iter02.operator*())) {
					return false;
				}
			}
			return true;
		}

		DCA_INLINE void clear() {
			for (size_type x = 0; x < sentinelVector.size(); ++x) {
				if (sentinelVector.at(x) > 0) {
					allocator_traits::destroy(*this, data + x);
					sentinelVector.at(x) = 0;
				}
			}
			sizeVal = 0;
		}

		DCA_INLINE ~unordered_set() {
			reset();
		};

	  protected:
		jsonifier::vector<int8_t> sentinelVector{};
		int8_t maxLookAheadDistance{ 0 };
		size_type capacityVal{};
		size_type sizeVal{};
		value_type* data{};

		template<typename mapped_type_new> DCA_INLINE iterator emplaceInternal(mapped_type_new&& value) {
			if (full() || capacityVal == 0) {
				resize(capacityVal + 1);
			}
			auto currentIndex = getKey(value) & (capacityVal - 1);
			for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
				if (sentinelVector[currentIndex] == 0) {
					new (std::addressof(data[currentIndex])) value_type{ std::forward<mapped_type_new>(value) };
					sentinelVector[currentIndex] = 1;
					sizeVal++;
					return { this, currentIndex };
				} else if (sentinelVector[currentIndex] > 0 && object_compare()(getKey(data[currentIndex]), getKey(value))) {
					if constexpr (!std::is_void_v<mapped_type_new>) {
						data[currentIndex] = std::forward<mapped_type_new>(value);
					}
					return { this, currentIndex };
				}
			}
			resize(capacityVal + 1);
			return emplaceInternal(std::forward<mapped_type_new>(value));
		}

		template<typename value_type_newer> DCA_INLINE uint64_t getKey(value_type_newer&& keyValue) const {
			return key_accessor<std::remove_cvref_t<value_type_newer>>::getHashKey(std::forward<value_type_newer>(keyValue));
		}

		DCA_INLINE void resize(size_type capacityNew) {
			auto newSize = hash_policy_new::nextPowerOfTwo(capacityNew);
			if (newSize > capacityVal) {
				jsonifier::vector<int8_t> oldSentinelVector = std::move(sentinelVector);
				auto oldMaxLookAheadDistance				= maxLookAheadDistance;
				auto oldCapacity							= capacityVal;
				auto oldSize								= sizeVal;
				auto oldPtr									= data;
				maxLookAheadDistance						= hash_policy_new::computeMaxLookAheadDistance(newSize);
				sizeVal										= 0;
				data										= allocator_traits::allocate(*this, newSize + 1 + maxLookAheadDistance);
				sentinelVector.resize(newSize + 1 + maxLookAheadDistance);
				sentinelVector[newSize + maxLookAheadDistance] = -1;
				capacityVal									   = newSize;
				for (size_type x = 0, y = 0; y < oldSize; ++x) {
					if (oldSentinelVector.at(x) > 0) {
						++y;
						emplaceInternal(std::move(oldPtr[x]));
					}
				}
				if (oldPtr && oldCapacity) {
					allocator_traits::deallocate(*this, oldPtr, oldCapacity + 1 + oldMaxLookAheadDistance);
				}
			}
		}

		DCA_INLINE void reset() {
			if (data && sizeVal > 0) {
				for (uint64_t x = 0; x < sentinelVector.size(); ++x) {
					if (sentinelVector.at(x) > 0) {
						allocator_traits::destroy(*this, data + x);
						sentinelVector.at(x) = 0;
					}
				}
				allocator_traits::deallocate(*this, data, capacityVal + 1 + maxLookAheadDistance);
				sentinelVector.clear();
				sizeVal		= 0;
				capacityVal = 0;
				data		= nullptr;
			}
		}
	};
}
