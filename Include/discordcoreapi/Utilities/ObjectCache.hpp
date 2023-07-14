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
/// ObjectCache.hpp - Header file for the "ObjectCache" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file ObjectCache.hpp

#pragma once

#include <discordcoreapi/Utilities/Base.hpp>
#include <discordcoreapi/Utilities/Etf.hpp>

namespace DiscordCoreAPI {

	template<typename KeyType, typename ValueType> class ObjectCache {
	  public:
		using key_type = KeyType;
		using mapped_type = ValueType;
		using reference = mapped_type&;
		using const_reference = const mapped_type&;
		using pointer = mapped_type*;

		inline ObjectCache() = default;

		inline ObjectCache& operator=(ObjectCache&& other) {
			if (this != &other) {
				std::unique_lock lock01{ other.cacheMutex };
				std::unique_lock lock02{ cacheMutex };
				std::swap(cacheMap, other.cacheMap);
			}
			return *this;
		}

		inline ObjectCache(ObjectCache&& other) {
			*this = std::move(other);
		}

		inline ObjectCache& operator=(const ObjectCache& other) {
			if (this != &other) {
				cacheMap = other.cacheMap;
			}
			return *this;
		}

		inline ObjectCache(const ObjectCache& other) {
			*this = other;
		}

		inline void emplace(mapped_type&& object) {
			std::unique_lock lock(cacheMutex);
			cacheMap.emplace(std::move(object));
		}

		inline reference operator[](key_type&& key) {
			std::shared_lock lock(cacheMutex);
			auto r = cacheMap.find(std::forward<key_type>(key));
			if (r != cacheMap.end()) {
				return *r;
			} else {
				throw DCAException{ "Sorry, but that object was not found in the cache." };
			}
		}

		inline reference operator[](const key_type& key) {
			std::shared_lock lock(cacheMutex);
			auto r = cacheMap.find(key);
			if (r != cacheMap.end()) {
				return *r;
			} else {
				throw DCAException{ "Sorry, but that object was not found in the cache." };
			}
		}

		inline bool contains(key_type&& key) {
			std::shared_lock lock(cacheMutex);
			return cacheMap.contains(std::forward<key_type>(key));
		}

		inline bool contains(const key_type& key) {
			std::shared_lock lock(cacheMutex);
			return cacheMap.contains(key);
		}

		inline void erase(key_type&& key) {
			std::unique_lock lock(cacheMutex);
			key_type newKey{ key };
			auto existing = cacheMap.find(std::forward<key_type>(key));
			if (existing != cacheMap.end()) {
				cacheMap.erase(std::move(newKey));
			}
		}

		inline void erase(const key_type& key) {
			std::unique_lock lock(cacheMutex);
			auto existing = cacheMap.find(key);
			if (existing != cacheMap.end()) {
				cacheMap.erase(key);
			}
		}

		inline uint64_t count() {
			std::shared_lock lock(cacheMutex);
			return cacheMap.size();
		}

		inline auto begin() {
			return cacheMap.begin();
		}

		inline auto end() {
			return cacheMap.end();
		}

		inline ~ObjectCache(){};

	  protected:
		UnorderedSet<key_type, mapped_type> cacheMap{};
		std::shared_mutex cacheMutex{};
	};

}
