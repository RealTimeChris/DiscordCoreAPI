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
#include <discordcoreapi/Utilities/UniquePtr.hpp>

namespace DiscordCoreAPI {

	/// @brief A template class representing an object cache.
	/// @tparam ValueType The type of values stored in the cache.
	template<typename ValueType> class ObjectCache {
	  public:
		using mapped_type	  = ValueType;
		using reference		  = mapped_type&;
		using const_reference = const mapped_type&;
		using pointer		  = mapped_type*;

		/// @brief Default constructor for the ObjectCache class.
		inline ObjectCache() : cacheMap{} {};

		/// @brief Move assignment operator for the ObjectCache class.
		/// @param other Another ObjectCache instance to be moved.
		/// @return Reference to the current ObjectCache instance.
		inline ObjectCache& operator=(ObjectCache&& other) {
			if (this != &other) {
				std::unique_lock lock01{ other.cacheMutex };
				std::unique_lock lock02{ cacheMutex };
				std::swap(cacheMap, other.cacheMap);
			}
			return *this;
		}

		/// @brief Move constructor for the ObjectCache class.
		/// @param other Another ObjectCache instance to be moved.
		inline ObjectCache(ObjectCache&& other) {
			*this = std::move(other);
		}

		/// @brief Add an object to the cache.
		/// @tparam mapped_type_new The type of the object to be added.
		/// @param object The object to be added to the cache.
		/// @return An iterator pointing to the newly added object in the cache.
		template<typename mapped_type_new> inline auto emplace(mapped_type_new&& object) {
			std::unique_lock lock(cacheMutex);
			return cacheMap.emplace(makeUnique<mapped_type_new>(std::forward<mapped_type_new>(object)));
		}

		/// @brief Access an object in the cache using a key.
		/// @tparam mapped_type_new The type of the key used for access.
		/// @param key The key used for accessing the object in the cache.
		/// @return Reference to the object associated with the provided key.
		template<typename mapped_type_new> inline reference operator[](mapped_type_new&& key) {
			std::shared_lock lock(cacheMutex);
			if (!cacheMap.contains(key)) {
				cacheMap.emplace(makeUnique<mapped_type>());
			}
			return *cacheMap.operator[](key);
		}

		/// @brief Check if the cache contains an object with a given key.
		/// @tparam mapped_type_new The type of the key to be checked.
		/// @param key The key to check for existence in the cache.
		/// @return `true` if the cache contains the key, `false` otherwise.
		template<typename mapped_type_new> inline bool contains(mapped_type_new&& key) {
			std::shared_lock lock(cacheMutex);
			return cacheMap.contains(std::forward<mapped_type_new>(key));
		}

		/// @brief Remove an object from the cache using a key.
		/// @tparam mapped_type_new The type of the key used for removal.
		/// @param key The key used to remove the object from the cache.
		template<typename mapped_type_new> inline void erase(mapped_type_new&& key) {
			std::unique_lock lock(cacheMutex);
			cacheMap.erase(std::forward<mapped_type_new>(key));
		}

		/// @brief Get the number of objects currently in the cache.
		/// @return The number of objects in the cache.
		inline uint64_t count() {
			std::shared_lock lock(cacheMutex);
			return cacheMap.size();
		}

		/// @brief Get an iterator to the beginning of the cache.
		/// @return An iterator to the beginning of the cache.
		inline auto begin() {
			return cacheMap.begin();
		}

		/// @brief Get an iterator to the end of the cache.
		/// @return An iterator to the end of the cache.
		inline auto end() {
			return cacheMap.end();
		}

		/// @brief Destructor for the ObjectCache class.
		inline ~ObjectCache(){};

	  protected:
		UnorderedSet<UniquePtr<mapped_type>> cacheMap{};///< The underlying container for storing objects.
		std::shared_mutex cacheMutex{};///< Mutex for ensuring thread-safe access to the cache.
	};


}
