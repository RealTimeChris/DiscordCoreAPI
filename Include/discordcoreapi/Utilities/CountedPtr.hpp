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
/// CountedPtr.hpp - Header for the "CountedPtr" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file CountedPtr.hpp

#pragma once

#include <discordcoreapi/Utilities/UniquePtr.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// @brief A smart pointer class that maintains a reference count for shared ownership.
	/// @tparam ValueType The type of the managed object.
	/// @tparam Deleter The type of the deleter used to destroy the managed object (optional).
	template<typename ValueType, typename Deleter = std::default_delete<ValueType>> class CountedPtr : protected Deleter {
	  public:
		using element_type = ValueType;
		using pointer	   = element_type*;
		using deleter_type = Deleter;
		using reference	   = element_type&;

		/// @brief Default constructor.
		inline CountedPtr() : refCount(new std::atomic_uint32_t{}) {
		}

		/// @brief Move assignment operator for related arrays.
		/// @param other The other CountedPtr to move from.
		/// @return CountedPtr The new managed object inside a CountedPtr.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr& operator=(ValueTypeNew&& other) {
			if (this != static_cast<void*>(&other)) {
				reset(nullptr);
				try {
					commit(other.release());
				} catch (...) {
					reset(nullptr);
					throw;
				}
			}
			return *this;
		}

		/// @brief Move constructor for related arrays.
		/// @param other The other CountedPtr to move from.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr(ValueTypeNew&& other) : Deleter{} {
			*this = std::move(other);
		}

		/// @brief Copy assignment operator.
		/// @param other The other CountedPtr to copy from.
		/// @return Reference to this CountedPtr after assignment.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr& operator=(const ValueTypeNew& other) {
			if (this != static_cast<void*>(&other)) {
				reset(nullptr);
				try {
					commit(other.get());
					refCount = other.refCount;
					if (ptr) {
						increment();
					}
				} catch (...) {
					reset(nullptr);
					throw;
				}
			}
			return *this;
		}

		/// @brief Copy constructor.
		/// @param other The other CountedPtr to copy from.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr(const ValueTypeNew& other) : Deleter{} {
			*this = other;
		}

		/// @brief Move assignment operator for raw pointer.
		/// @param newPtr The pointer to be managed.
		/// @return CountedPtr The new managed object inside a CountedPtr.
		template<pointer_t value_type> inline CountedPtr& operator=(value_type newPtr) {
			reset(nullptr);
			try {
				commit(newPtr);
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		/// @brief Move construction operator for raw pointer.
		/// @param newPtr The pointer to be managed.
		template<pointer_t value_type> inline CountedPtr(value_type newPtr) : Deleter{} {
			*this = newPtr;
		}

		/// @brief Swap the contents of this CountedPtr with another.
		/// @param other The other CountedPtr to swap with.
		inline void swap(CountedPtr& other) {
			std::swap(refCount, other.refCount);
			std::swap(ptr, other.ptr);
		}

		/// @brief Get the raw pointer managed by this CountedPtr.
		/// @return The raw pointer.
		inline pointer get() const {
			return ptr;
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
			reset(nullptr);
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
				ptr		 = newPtr;
				refCount = new std::atomic_uint32_t{};
				increment();
			}
		}

		/// @brief Destructor.
		inline ~CountedPtr() {
			reset(nullptr);
		}

	  protected:
		std::atomic_uint32_t* refCount{ nullptr };///< Pointer to the reference count.
		pointer ptr{ nullptr };///< Pointer to the managed object.

		/// @brief Increment the reference count.
		inline void increment() {
			if (refCount) {
				std::atomic_fetch_add(refCount, 1);
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
				return std::atomic_fetch_sub(refCount, 1) - 1;
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

	/// @brief A smart pointer class that maintains a reference count for shared ownership.
	/// @tparam ValueType The type of the managed object.
	/// @tparam Deleter The type of the deleter used to destroy the managed object (optional).
	template<typename ValueType, typename Deleter> class CountedPtr<ValueType[], Deleter> : protected Deleter {
	  public:
		using element_type = std::remove_extent_t<ValueType>;
		using pointer	   = element_type*;
		using deleter_type = Deleter;
		using reference	   = element_type&;

		/// @brief Default constructor.
		inline CountedPtr() : refCount(new std::atomic_uint32_t{}) {
		}

		/// @brief Move assignment operator for related arrays.
		/// @param other The other CountedPtr to move from.
		/// @return CountedPtr The new managed object inside a CountedPtr.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr& operator=(ValueTypeNew&& other) {
			if (this != static_cast<void*>(&other)) {
				reset(nullptr);
				try {
					commit(other.release());
				} catch (...) {
					reset(nullptr);
					throw;
				}
			}
			return *this;
		}

		/// @brief Move constructor for related arrays.
		/// @param other The other CountedPtr to move from.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr(ValueTypeNew&& other) : Deleter{} {
			*this = std::move(other);
		}

		/// @brief Copy assignment operator.
		/// @param other The other CountedPtr to copy from.
		/// @return Reference to this CountedPtr after assignment.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr& operator=(const ValueTypeNew& other) {
			if (this != static_cast<void*>(&other)) {
				reset(nullptr);
				try {
					commit(other.get());
					refCount = other.refCount;
					if (ptr) {
						increment();
					}
				} catch (...) {
					reset(nullptr);
					throw;
				}
			}
			return *this;
		}

		/// @brief Copy constructor.
		/// @param other The other CountedPtr to copy from.
		template<jsonifier_internal::shared_ptr_t ValueTypeNew> inline CountedPtr(const ValueTypeNew& other) : Deleter{} {
			*this = other;
		}

		/// @brief Move assignment operator for raw pointer.
		/// @param newPtr The pointer to be managed.
		/// @return CountedPtr The new managed object inside a CountedPtr.
		template<pointer_t value_type> inline CountedPtr& operator=(value_type newPtr) {
			reset(nullptr);
			try {
				commit(newPtr);
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		/// @brief Move construction operator for raw pointer.
		/// @param newPtr The pointer to be managed.
		template<pointer_t value_type> inline CountedPtr(value_type newPtr) : Deleter{} {
			*this = newPtr;
		}

		/// @brief Swap the contents of this CountedPtr with another.
		/// @param other The other CountedPtr to swap with.
		inline void swap(CountedPtr& other) {
			std::swap(refCount, other.refCount);
			std::swap(ptr, other.ptr);
		}

		/// @brief Get the raw pointer managed by this CountedPtr.
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
			reset(nullptr);
			return releasedPtr;
		}

		/// @brief Reset the managed object and decrement the reference count.
		/// @param newPtr The new raw pointer to manage (optional).
		inline void reset(pointer newPtr) {
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
				ptr		 = newPtr;
				refCount = new std::atomic_uint32_t{};
				increment();
			}
		}

		/// @brief Destructor.
		inline ~CountedPtr() {
			reset(nullptr);
		}

	  protected:
		std::atomic_uint32_t* refCount{ nullptr };///< Pointer to the reference count.
		pointer ptr{ nullptr };///< Pointer to the managed object.

		/// @brief Increment the reference count.
		inline void increment() {
			if (refCount) {
				std::atomic_fetch_add(refCount, 1);
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
				return std::atomic_fetch_sub(refCount, 1) - 1;
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

	/// @brief Creates a CountedPtr instance for non-array types.
	/// @tparam ValueType The type of value to be managed.
	/// @tparam Deleter The deleter type to be used (default: std::default_delete<ValueType>).
	/// @tparam ArgTypes The argument types for constructing the value.
	/// @param args The arguments for constructing the value.
	/// @return A CountedPtr instance managing the created value.
	template<typename ValueType, typename Deleter = std::default_delete<ValueType>, typename... ArgTypes, std::enable_if_t<!std::is_array_v<ValueType>, int32_t> = 0>
	inline CountedPtr<ValueType, Deleter> makeCounted(ArgTypes&&... args) {
		return CountedPtr<ValueType, Deleter>(new ValueType(std::forward<ArgTypes>(args)...));
	}

	/// @brief Creates a CountedPtr instance for arrays with zero extent.
	/// @tparam ValueType The type of array element.
	/// @tparam Deleter The deleter type to be used (default: std::default_delete<ValueType>).
	/// @param size The size of the array to be created.
	/// @return A CountedPtr instance managing the created array.
	template<typename ValueType, typename Deleter = std::default_delete<ValueType>, std::enable_if_t<std::is_array_v<ValueType> && std::extent_v<ValueType> == 0, int32_t> = 0>
	inline CountedPtr<ValueType, Deleter> makeCounted(const uint64_t size) {
		using element_type = std::remove_extent_t<ValueType>;
		return CountedPtr<ValueType, Deleter>(new element_type[size]());
	}

	/// @brief Explicitly deletes the function for arrays with non-zero extent.
	/// @tparam ValueType The type of array with non-zero extent.
	/// @tparam ArgTypes The argument types (not applicable for deletion).
	template<typename ValueType, typename... ArgTypes, std::enable_if_t<std::extent_v<ValueType> != 0, int32_t> = 0> inline void makeCounted(ArgTypes&&...) = delete;

	/**@}*/

}
