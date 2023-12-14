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
/// unordered_map.hpp - Header file for the unordered_map class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file unordered_map.hpp
#pragma once

#include <discordcoreapi/Utilities/Hash.hpp>
#include <discordcoreapi/Utilities/UniquePtr.hpp>

#include <memory_resource>
#include <shared_mutex>
#include <exception>
#include <optional>
#include <utility>
#include <vector>
#include <mutex>

namespace discord_core_api {

	template<typename key_type, typename value_type> class unordered_map;

	template<typename map_iterator, typename key_type, typename value_type>
	concept map_container_iterator_t = std::same_as<typename unordered_map<key_type, value_type>::iterator, std::decay_t<map_iterator>>;

	template<typename key_type_new, typename value_type_new> class unordered_map : protected hash_policy<unordered_map<key_type_new, value_type_new>>,
																				   protected jsonifier_internal::alloc_wrapper<std::pair<key_type_new, value_type_new>>,
																				   protected object_compare {
	  public:
		using key_type			= key_type_new;
		using value_type		= std::pair<key_type_new, value_type_new>;
		using allocator_type	= jsonifier_internal::alloc_wrapper<value_type>;
		using allocator_traits	= std::allocator_traits<allocator_type>;
		using size_type			= uint64_t;
		using difference_type	= int64_t;
		using pointer			= typename allocator_traits::pointer;
		using const_pointer		= typename allocator_traits::const_pointer;
		using mapped_type		= value_type_new;
		using reference			= value_type&;
		using const_reference	= const value_type&;
		using iterator			= hash_iterator<unordered_map<key_type, mapped_type>>;
		using const_iterator	= hash_iterator<const unordered_map<key_type, mapped_type>>;
		using object_compare	= object_compare;
		using hash_policy_new	= hash_policy<unordered_map<key_type, mapped_type>>;

		friend hash_policy_new;
		friend iterator;
		friend const_iterator;

		DCA_INLINE unordered_map(){};

		DCA_INLINE unordered_map& operator=(unordered_map&& other) noexcept {
			if (this != &other) {
				reset();
				swap(other);
			}
			return *this;
		}

		DCA_INLINE unordered_map(unordered_map&& other) noexcept {
			*this = std::move(other);
		}

		DCA_INLINE unordered_map& operator=(const unordered_map& other) {
			if (this != &other) {
				reset();

				reserve(other.capacity());
				for (const auto& [key, value]: other) {
					emplace(key, value);
				}
			}
			return *this;
		}

		DCA_INLINE unordered_map(const unordered_map& other) {
			*this = other;
		}

		DCA_INLINE unordered_map(std::initializer_list<value_type> list) {
			reserve(list.size());
			for (auto& value: list) {
				emplace(std::move(value.first), std::move(value.second));
			}
		};

		template<typename... Args> iterator emplace(Args&&... value) {
			return emplaceInternal(std::forward<Args>(value)...);
		}

		template<typename key_type_newer> DCA_INLINE const_iterator find(key_type_newer&& key) const {
			if (sizeVal > 0) {
				auto currentIndex = hash_policy_new::indexForHash(key);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						return { this, currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_newer> DCA_INLINE iterator find(key_type_newer&& key) {
			if (sizeVal > 0) {
				auto currentIndex = hash_policy_new::indexForHash(key);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						return { this, currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_newer> DCA_INLINE const mapped_type& operator[](key_type_newer&& key) const {
			return emplaceInternal(key)->second;
		}

		template<typename key_type_newer> DCA_INLINE mapped_type& operator[](key_type_newer&& key) {
			return emplaceInternal(key)->second;
		}

		template<typename key_type_newer> DCA_INLINE const mapped_type& at(key_type_newer&& key) const {
			auto iter = find(std::forward<key_type_newer>(key));
			if (iter == end()) {
				throw std::runtime_error{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return iter->second;
		}

		template<typename key_type_newer> DCA_INLINE mapped_type& at(key_type_newer&& key) {
			auto iter = find(std::forward<key_type_newer>(key));
			if (iter == end()) {
				throw std::runtime_error{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return iter->second;
		}

		template<typename key_type_newer> DCA_INLINE bool contains(key_type_newer&& key) const {
			if (sizeVal > 0) {
				auto currentIndex = hash_policy_new::indexForHash(key);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						return true;
					}
				}
			}
			return false;
		}

		template<map_container_iterator_t<key_type, mapped_type> map_iterator> DCA_INLINE iterator erase(map_iterator&& iter) {
			if (sizeVal > 0) {
				auto currentIndex = static_cast<size_type>(iter.getRawPtr() - data);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex], iter.operator*())) {
						allocator_traits::destroy(*this, data + currentIndex);
						sentinelVector[currentIndex] = 0;
						--sizeVal;
						return { this, ++currentIndex };
					}
				}
			}
			return end();
		}

		template<typename key_type_newer> DCA_INLINE iterator erase(key_type_newer&& key) {
			if (sizeVal > 0) {
				auto currentIndex = hash_policy_new::indexForHash(key);
				for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
					if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
						allocator_traits::destroy(*this, data + currentIndex);
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

		DCA_INLINE void swap(unordered_map& other) {
			std::swap(maxLookAheadDistance, other.maxLookAheadDistance);
			std::swap(sentinelVector, other.sentinelVector);
			std::swap(capacityVal, other.capacityVal);
			std::swap(sizeVal, other.sizeVal);
			std::swap(data, other.data);
		}

		DCA_INLINE size_type capacity() const {
			return capacityVal;
		}

		DCA_INLINE bool operator==(const unordered_map& other) const {
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

		DCA_INLINE void clear() {
			for (size_type x = 0; x < sentinelVector.size(); ++x) {
				if (sentinelVector.at(x) > 0) {
					allocator_traits::destroy(*this, data + x);
					sentinelVector.at(x) = 0;
				}
			}
			sizeVal = 0;
		}

		DCA_INLINE ~unordered_map() {
			reset();
		};

	  protected:
		jsonifier::vector<int8_t> sentinelVector{};
		int8_t maxLookAheadDistance{ 0 };
		size_type capacityVal{};
		size_type sizeVal{};
		value_type* data{};

		template<typename key_type_newer, typename... mapped_type_new> DCA_INLINE iterator emplaceInternal(key_type_newer&& key, mapped_type_new&&... value) {
			if (full() || capacityVal == 0) {
				resize(capacityVal + 1);
			}
			auto currentIndex = hash_policy_new::indexForHash(key);
			for (size_type x{}; x < static_cast<size_type>(maxLookAheadDistance); ++x, ++currentIndex) {
				if (sentinelVector[currentIndex] == 0) {
					if constexpr ((( !std::is_void_v<mapped_type_new> ) || ...)) {
						new (std::addressof(data[currentIndex])) value_type{ std::make_pair(std::forward<key_type_newer>(key), std::forward<mapped_type_new>(value)...) };
					} else {
						new (std::addressof(data[currentIndex])) value_type{ std::make_pair(std::forward<key_type_newer>(key), mapped_type{}) };
					}
					sentinelVector[currentIndex] = 1;
					sizeVal++;
					return { this, currentIndex };
				} else if (sentinelVector[currentIndex] > 0 && object_compare()(data[currentIndex].first, key)) {
					if constexpr ((( !std::is_void_v<mapped_type_new> ) || ...)) {
						data[currentIndex].second = mapped_type{ std::forward<mapped_type_new>(value)... };
					}
					return { this, currentIndex };
				}
			}
			resize(capacityVal + 1);
			return emplaceInternal(std::forward<key_type_newer>(key), std::forward<mapped_type_new>(value)...);
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
						emplaceInternal(std::move(oldPtr[x].first), std::move(oldPtr[x].second));
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