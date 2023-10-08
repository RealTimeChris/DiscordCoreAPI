/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright ,  Chris M. (RealTimeChris)

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
/// CountedPtr.hpp - Header for the "counted_ptr" related stuff.
/// Dec ,
/// https://discordcoreapi.com
/// \file CountedPtr.hpp

#pragma once

#include <discordcoreapi/Utilities/UniquePtr.hpp>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <iosfwd>
#include <type_traits>
#include <utility>

namespace discord_core_api {

	/**
	 * \addtogroup utilities
	 * @{
	 */

	template<typename value_type, typename deleter = std::default_delete<value_type>> class counted_ptr : public deleter {
	  public:
		using element_type = value_type;
		using reference	   = element_type&;
		using pointer	   = element_type*;

		template<typename Other, typename OtherDeleter> friend class counted_ptr;

		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr& operator=(value_type_newer&& other) {
			if (ptr == other.ptr)
				return *this;
			decrement();
			ptr		  = other.ptr;
			other.ptr = nullptr;
			return *this;
		}

		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr(value_type_newer&& other) {
			*this	  = std::move(other);
			other.ptr = nullptr;
		}

		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr& operator=(const value_type_newer& other) {
			if (ptr == other.ptr)
				return *this;
			increment(other.ptr);
			decrement();
			ptr = other.ptr;
			return *this;
		}

		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr(const value_type_newer& other) : counted_ptr() {
			*this = other;
		}

		inline explicit counted_ptr(pointer ptr = nullptr) noexcept : ptr(ptr) {
			increment(ptr);
		}

		inline reference operator*() const noexcept {
			assert(ptr);
			return *ptr;
		}

		inline pointer operator->() const noexcept {
			assert(ptr);
			return ptr;
		}

		inline pointer get() const noexcept {
			return ptr;
		}

		inline bool valid() const noexcept {
			return (ptr != nullptr);
		}

		inline operator bool() const noexcept {
			return valid();
		}

		inline void swap(counted_ptr& other) {
			std::swap(other.ptr, ptr);
		}

		inline bool empty() const noexcept {
			return (ptr == nullptr);
		}

		inline bool unique() const noexcept {
			return ptr && ptr->unique();
		}

		inline size_t use_count() const noexcept {
			return ptr->reference_count();
		}

		inline void reset() {
			decrement();
			ptr = nullptr;
		}

		inline void unify() {
			if (ptr && !ptr->unique())
				operator=(counted_ptr(new value_type(*ptr)));
		}

		inline bool operator==(const counted_ptr& other) const noexcept {
			return ptr == other.ptr;
		}

		inline bool operator!=(const counted_ptr& other) const noexcept {
			return ptr != other.ptr;
		}

		inline bool operator==(pointer other) const noexcept {
			return ptr == other;
		}

		inline bool operator!=(pointer other) const noexcept {
			return ptr != other;
		}

		inline bool operator<(const counted_ptr& other) const noexcept {
			return ptr < other.ptr;
		}

		inline bool operator<=(const counted_ptr& other) const noexcept {
			return ptr <= other.ptr;
		}

		inline bool operator>(const counted_ptr& other) const noexcept {
			return ptr > other.ptr;
		}

		inline bool operator>=(const counted_ptr& other) const noexcept {
			return ptr >= other.ptr;
		}

		inline bool operator<(pointer other) const noexcept {
			return ptr < other;
		}

		inline bool operator<=(pointer other) const noexcept {
			return ptr <= other;
		}

		inline bool operator>(pointer other) const noexcept {
			return ptr > other;
		}

		inline bool operator>=(pointer other) const noexcept {
			return ptr >= other;
		}

		inline ~counted_ptr() {
			decrement();
		}

	  protected:
		pointer ptr{};

		void increment(pointer o) noexcept {
			if (o) {
				o->increment();
			}
		};

		void decrement() noexcept {
			if (ptr && ptr->decrement()) {
				getDeleter()(ptr);
			}
		}

		inline deleter& getDeleter() {
			return *this;
		}
	};

	template<typename value_type, typename... Args> counted_ptr<value_type> makeCounted(Args&&... args) {
		return counted_ptr<value_type>(new value_type(std::forward<Args>(args)...));
	}

	template<typename A, typename D> void swap(counted_ptr<A, D>& a1, counted_ptr<A, D>& a2) noexcept {
		a1.swap(a2);
	}

	template<typename A, typename D> std::ostream& operator<<(std::ostream& os, const counted_ptr<A, D>& c) {
		return os << c.get();
	}

	class reference_counter {
	  protected:
		mutable std::atomic<size_t> reference_count_;

	  public:
		reference_counter() noexcept : reference_count_(0) {
		}

		reference_counter(const reference_counter&) noexcept : reference_count_(0) {
		}

		reference_counter& operator=(const reference_counter&) noexcept {
			return *this;
		}

		~reference_counter() {
			assert(reference_count_ == 0);
		}

	  public:
		void increment() const noexcept {
			++reference_count_;
		}

		bool decrement() const noexcept {
			assert(reference_count_ > 0);
			return (--reference_count_ == 0);
		}

		bool unique() const noexcept {
			return (reference_count_ == 1);
		}

		size_t reference_count() const noexcept {
			return reference_count_;
		}
	};

	/// @brief A smart pointer class that maintains a reference count for shared ownership.
	/// @tparam value_type The type of the managed object.
	/// @tparam deleter The type of the deleter used to destroy the managed object (optional).
	template<typename value_type_new, typename deleter> class counted_ptr<value_type_new[], deleter> : protected deleter {
	  public:
		using element_type = std::remove_extent_t<value_type_new>;
		using pointer	   = element_type*;
		using deleter_type = deleter;
		using reference	   = element_type&;

		/// @brief Default constructor.
		inline counted_ptr() : refCount(new std::atomic_uint32_t{}) {
		}

		/// @brief Move assignment operator for related arrays.
		/// @param other The other counted_ptr to move from.
		/// @return counted_ptr The new managed object inside a counted_ptr.
		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr& operator=(value_type_newer&& other) {
			if (this != static_cast<void*>(&other)) {
				reset();
				try {
					commit(other.release());
				} catch (...) {
					reset();
					throw;
				}
			}
			return *this;
		}

		/// @brief Move constructor for related arrays.
		/// @param other The other counted_ptr to move from.
		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr(value_type_newer&& other) {
			*this = std::move(other);
		}

		/// @brief Copy assignment operator.
		/// @param other The other counted_ptr to copy from.
		/// @return Reference to this counted_ptr after assignment.
		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr& operator=(const value_type_newer& other) {
			if (this != &other) {
				reset();
				try {
					refCount = other.refCount;
					ptr		 = other.ptr;
					increment();// Increment the reference count since we are sharing ownership.
				} catch (...) {
					reset();
					throw;
				}
			}
			return *this;
		}

		/// @brief Copy constructor.
		/// @param other The other counted_ptr to copy from.
		template<jsonifier::concepts::shared_ptr_t value_type_newer> inline counted_ptr(const value_type_newer& other) : counted_ptr() {
			*this = other;
		}

		/// @brief Move assignment operator for raw pointer.
		/// @param newPtr The pointer to be managed.
		/// @return counted_ptr The new managed object inside a counted_ptr.
		inline counted_ptr& operator=(pointer newPtr) {
			reset();
			try {
				commit(newPtr);
			} catch (...) {
				reset();
				throw;
			}
			return *this;
		}

		/// @brief Move construction operator for raw pointer.
		/// @param newPtr The pointer to be managed.
		inline counted_ptr(pointer newPtr) {
			*this = newPtr;
		}

		/// @brief Swap the contents of this counted_ptr with another.
		/// @param other The other counted_ptr to swap with.
		inline void swap(counted_ptr& other) noexcept {
			std::swap(refCount, other.refCount);
			std::swap(ptr, other.ptr);
		}

		/// @brief Get the raw pointer managed by this counted_ptr.
		/// @return The raw pointer.
		inline pointer get() const {
			return ptr;
		}

		/// @brief An operator for accessing elements of this array.
		/// @param index The index of the intended element to access.
		/// @return reference A reference to the selected object.
		inline reference operator[](std::ptrdiff_t index) const {
			return ptr[index];
		}

		/// @brief Conversion operator to check if the pointer is valid.
		/// @return `true` if the pointer is valid, `false` otherwise.
		inline operator bool() const {
			return ptr != nullptr;
		}

		/// @brief Dereference operator.
		/// @return A reference to the managed object.
		inline reference operator*() const {
			return *ptr;
		}

		/// @brief Member access operator.
		/// @return The managed raw pointer.
		inline pointer operator->() const {
			return ptr;
		}

		/// @brief Releases the managed pointer.
		/// @return The released raw pointer.
		inline pointer release() {
			pointer releasedPtr = ptr;
			ptr					= nullptr;
			reset();
			return releasedPtr;
		}

		/// @brief Reset the managed object and decrement the reference count.
		/// @param newPtr The new raw pointer to manage (optional).
		inline void reset(pointer newPtr = nullptr) {
			if (decrement() <= 0) {
				if (refCount) {
					delete refCount;
					refCount = nullptr;
				}
				if (ptr) {
					getDeleter()(ptr);
					ptr = nullptr;
				}
			}
			if (newPtr) {
				ptr = newPtr;
				if (!refCount) {
					refCount = new std::atomic_uint32_t{};
				}
				increment();
			}
		}

		/// @brief Destructor.
		inline ~counted_ptr() {
			reset();
		}

	  protected:
		std::atomic_uint32_t* refCount{ nullptr };///< Pointer to the reference count.
		pointer ptr{ nullptr };///< Pointer to the managed object.

		/// @brief Increment the reference count.
		inline void increment() {
			if (refCount) {
				refCount->fetch_add(1, std::memory_order_release);
			}
		}

		/// @brief Commits a new pointer value and resets the current one.
		/// @param other The new pointer value to commit.
		inline void commit(pointer other) {
			pointer tempPtr = other;
			reset(tempPtr);
		}

		/// @brief Decrement the reference count and return the new count.
		/// @return The new reference count.
		inline uint32_t decrement() {
			if (refCount) {
				return refCount->fetch_sub(1, std::memory_order_release) - 1;
			} else {
				return 0;
			}
		}

		/// @brief Get the deleter object.
		/// @return Reference to the deleter object.
		inline deleter_type& getDeleter() {
			return *this;
		}
	};

	/// @brief Creates a counted_ptr instance for arrays with zero extent.
	/// @tparam value_type The type of array element.
	/// @tparam deleter The deleter type to be used (default: std::default_delete<value_type>).
	/// @param size The size of the array to be created.
	/// @return A counted_ptr instance managing the created array.
	template<typename value_type, typename deleter = std::default_delete<value_type>, std::enable_if_t<std::is_array_v<value_type> && std::extent_v<value_type> == 0, int32_t> = 0>
	inline counted_ptr<value_type, deleter> makeCounted(const uint64_t size) {
		using element_type = std::remove_extent_t<value_type>;
		return counted_ptr<value_type, deleter>(new element_type[size]());
	}

	/// @brief Explicitly deletes the function for arrays with non-zero extent.
	/// @tparam value_type The type of array with non-zero extent.
	/// @tparam ArgTypes The argument types (not applicable for deletion).
	template<typename value_type, typename... ArgTypes, std::enable_if_t<std::extent_v<value_type> != 0, int32_t> = 0> inline void makeCounted(ArgTypes&&...) = delete;

	/**@}*/

}
