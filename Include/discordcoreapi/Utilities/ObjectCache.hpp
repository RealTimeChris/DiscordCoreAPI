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
/// ObjectCache.hpp - Header file for the "object_cache" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file ObjectCache.hpp
#pragma once

#include <discordcoreapi/Utilities/Base.hpp>
#include <discordcoreapi/Utilities/Etf.hpp>
#include <discordcoreapi/Utilities/UniquePtr.hpp>

namespace discord_core_api {

	/// @brief A template class representing an object cache.
	/// @tparam value_type the type of values stored in the cache.
	template<typename value_type> class object_cache {
	  public:
		using mapped_type	  = value_type;
		using reference		  = mapped_type&;
		using const_reference = const mapped_type&;
		using pointer		  = mapped_type*;

		/// @brief Default constructor for the object_cache class.
		DCA_INLINE object_cache() : cacheMap{} {};

		/// @brief Move assignment operator for the object_cache class.
		/// @param other another object_cache instance to be moved.
		/// @return reference to the current object_cache instance.
		DCA_INLINE object_cache& operator=(object_cache&& other) noexcept {
			if (this != &other) {
				std::unique_lock lock01{ other.cacheMutex };
				std::unique_lock lock02{ cacheMutex };
				std::swap(cacheMap, other.cacheMap);
			}
			return *this;
		}

		/// @brief Move constructor for the object_cache class.
		/// @param other another object_cache instance to be moved.
		DCA_INLINE object_cache(object_cache&& other) noexcept {
			*this = std::move(other);
		}

		/// @brief Add an object to the cache.
		/// @tparam mapped_type_new the type of the object to be added.
		/// @param object the object to be added to the cache.
		/// @return an iterator pointing to the newly added object in the cache.
		template<typename mapped_type_new> DCA_INLINE auto emplace(mapped_type_new&& object) {
			std::unique_lock lock(cacheMutex);
			return cacheMap.emplace(makeUnique<std::remove_cvref_t<mapped_type_new>>(std::forward<mapped_type_new>(object)));
		}

		/// @brief Access an object in the cache using a key.
		/// @tparam mapped_type_new the type of the key used for access.
		/// @param key the key used for accessing the object in the cache.
		/// @return reference to the object associated with the provided key.
		template<typename mapped_type_new> DCA_INLINE reference operator[](mapped_type_new&& key) {
			std::shared_lock lock(cacheMutex);
			if (!cacheMap.contains(key)) {
				cacheMap.emplace(makeUnique<mapped_type>());
			}
			return *cacheMap.operator[](key);
		}

		/// @brief Check if the cache contains an object with a given key.
		/// @tparam mapped_type_new the type of the key to be checked.
		/// @param key the key to check for existence in the cache.
		/// @return `true` if the cache contains the key, `false` otherwise.
		template<typename mapped_type_new> DCA_INLINE bool contains(mapped_type_new&& key) {
			std::shared_lock lock(cacheMutex);
			return cacheMap.contains(std::forward<mapped_type_new>(key));
		}

		/// @brief Remove an object from the cache using a key.
		/// @tparam mapped_type_new the type of the key used for removal.
		/// @param key the key used to remove the object from the cache.
		template<typename mapped_type_new> DCA_INLINE void erase(mapped_type_new&& key) {
			std::unique_lock lock(cacheMutex);
			cacheMap.erase(std::forward<mapped_type_new>(key));
		}

		/// @brief Get the number of objects currently in the cache.
		/// @return the number of objects in the cache.
		DCA_INLINE uint64_t count() {
			std::shared_lock lock(cacheMutex);
			return cacheMap.size();
		}

		/// @brief Get an iterator to the beginning of the cache.
		/// @return an iterator to the beginning of the cache.
		DCA_INLINE auto begin() {
			return cacheMap.begin();
		}

		/// @brief Get an iterator to the end of the cache.
		/// @return an iterator to the end of the cache.
		DCA_INLINE auto end() {
			return cacheMap.end();
		}

		/// @brief Destructor for the object_cache class.
		DCA_INLINE ~object_cache(){};

	  protected:
		unordered_set<unique_ptr<mapped_type>> cacheMap{};///< The underlying container for storing objects.
		std::shared_mutex cacheMutex{};///< Mutex for ensuring thread-safe access to the cache.
	};


}
