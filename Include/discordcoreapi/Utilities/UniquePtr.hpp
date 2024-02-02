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
/// UniquePtr.hpp - Header for the "unique_ptr" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file UniquePtr.hpp
#pragma once

#include <jsonifier/Index.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// @brief A smart pointer class that provides unique ownership semantics.
	/// @tparam value_type the type of the managed object.
	/// @tparam deleter the type of the deleter used to destroy the object.
	template<typename value_type, typename deleter = std::default_delete<value_type>> class unique_ptr : protected deleter {
	  public:
		using element_type = std::remove_extent_t<value_type>;
		using pointer	   = element_type*;
		using deleter_type = deleter;
		using reference	   = element_type&;

		DCA_INLINE unique_ptr& operator=(const unique_ptr&) = delete;
		DCA_INLINE unique_ptr(const unique_ptr&)			= delete;

		/// @brief Default constructor.
		DCA_INLINE unique_ptr() : ptr{ nullptr } {};

		/// @brief Move assignment operator for related pointers.
		/// @param other the other unique_ptr to move from.
		/// @return unique_ptr the new managed object inside a unique_ptr.
		template<jsonifier::concepts::unique_ptr_t value_type_new> DCA_INLINE unique_ptr& operator=(value_type_new&& other) {
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

		/// @brief Move constructor for related pointers.
		/// @param other the other unique_ptr to move from.
		template<jsonifier::concepts::unique_ptr_t value_type_new> DCA_INLINE unique_ptr(value_type_new&& other) {
			*this = std::move(other);
		}

		/// @brief Move assignment operator for raw pointer.
		/// @param newPtr the new ptr to manage.
		/// @return unique_ptr the new managed object inside a unique_ptr.
		template<jsonifier::concepts::pointer_t value_type_new> DCA_INLINE unique_ptr& operator=(value_type_new newPtr) {
			reset();
			try {
				commit(newPtr);
			} catch (...) {
				reset();
				throw;
			}
			return *this;
		}

		/// @brief Constructor from a raw pointer.
		/// @param newPtr the new ptr to manage.
		template<jsonifier::concepts::pointer_t value_type_new> DCA_INLINE unique_ptr(value_type_new newPtr) {
			*this = newPtr;
		}

		/// @brief Gets the managed raw pointer.
		/// @return the managed raw pointer.
		DCA_INLINE pointer get() const {
			return ptr;
		}

		/// @brief Conversion operator to check if the pointer is valid.
		/// @return `true` if the pointer is valid, `false` otherwise.
		DCA_INLINE operator bool() const {
			return ptr != nullptr;
		}

		/// @brief Dereference operator.
		/// @return a reference to the managed object.
		DCA_INLINE reference operator*() const {
			if (!ptr) {
				throw dca_exception{ "Sorry, but you attempted to access a unique_ptr that is nullptr." };
			}
			return *ptr;
		}

		/// @brief Member access operator.
		/// @return the managed raw pointer.
		DCA_INLINE pointer operator->() const {
			if (!ptr) {
				throw dca_exception{ "Sorry, but you attempted to access a unique_ptr that is nullptr." };
			}
			return ptr;
		}

		/// @brief Releases the managed pointer.
		/// @return the released raw pointer.
		DCA_INLINE pointer release() {
			pointer releasedPtr = ptr;
			ptr					= nullptr;
			return releasedPtr;
		}

		/// @brief Resets the managed pointer and invokes the deleter.
		/// @param newPtr the new raw pointer to manage.
		DCA_INLINE void reset(pointer newPtr = nullptr) {
			pointer oldPtr = std::exchange(ptr, newPtr);
			if (oldPtr) {
				getDeleter()(oldPtr);
			}
		}

		/// @brief Swaps the contents of two unique_ptr objects.
		/// @param other the other unique_ptr to swap with.
		DCA_INLINE void swap(unique_ptr& other) {
			std::swap(ptr, other.ptr);
		}

		/// @brief Destructor that releases the managed object.
		DCA_INLINE ~unique_ptr() {
			reset(nullptr);
		}

	  protected:
		pointer ptr{ nullptr };///< The stored object.

		/// @brief Commits a new pointer value and resets the current one.
		/// @param other the new pointer value to commit.
		DCA_INLINE void commit(pointer other) {
			pointer tempPtr = other;
			reset(tempPtr);
		}

		/// @brief Gets the deleter associated with the managed object.
		/// @return the associated deleter.
		DCA_INLINE deleter_type& getDeleter() {
			return *static_cast<deleter_type*>(this);
		}
	};

	/// @brief Specialization of unique_ptr for arrays.
	/// @tparam value_type the type of the managed object.
	/// @tparam deleter the type of the deleter used to destroy the object.
	template<typename value_type, typename deleter> class unique_ptr<value_type[], deleter> : public deleter {
	  public:
		using element_type = std::remove_extent_t<value_type>;
		using pointer	   = element_type*;
		using deleter_type = deleter;
		using reference	   = element_type&;

		DCA_INLINE unique_ptr& operator=(const unique_ptr&) = delete;
		DCA_INLINE unique_ptr(const unique_ptr&)			= delete;

		/// @brief Default constructor.
		DCA_INLINE unique_ptr() : ptr{ nullptr } {};

		/// @brief Move assignment operator for related pointers.
		/// @param other the other unique_ptr to move from.
		/// @return unique_ptr the new managed object inside a unique_ptr.
		template<jsonifier::concepts::unique_ptr_t value_type_new> DCA_INLINE unique_ptr& operator=(value_type_new&& other) {
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

		/// @brief Move constructor for related pointers.
		/// @param other the other unique_ptr to move from.
		template<jsonifier::concepts::unique_ptr_t value_type_new> DCA_INLINE unique_ptr(value_type_new&& other) {
			*this = std::move(other);
		}

		/// @brief Move assignment operator for raw pointer.
		/// @param newPtr the new ptr to manage.
		/// @return unique_ptr the new managed object inside a unique_ptr.
		template<jsonifier::concepts::pointer_t value_type_new> DCA_INLINE unique_ptr& operator=(value_type_new newPtr) {
			reset();
			try {
				commit(newPtr);
			} catch (...) {
				reset();
				throw;
			}
			return *this;
		}

		/// @brief Constructor from a raw pointer.
		/// @param newPtr the new ptr to manage.
		template<jsonifier::concepts::pointer_t value_type_new> DCA_INLINE unique_ptr(value_type_new newPtr) {
			*this = newPtr;
		}

		/// @brief Gets the managed raw pointer.
		/// @return pointer the managed raw pointer.
		DCA_INLINE pointer get() const {
			return ptr;
		}

		/// @brief Square bracket operator for accessing elements of this array.
		/// @param index the index which is to be accessed.
		/// @return reference a reference to the object that was accessed.
		DCA_INLINE reference operator[](std::ptrdiff_t index) const {
			return ptr[index];
		}

		/// @brief Conversion operator to check if the pointer is valid.
		/// @return `true` if the pointer is valid, `false` otherwise.
		DCA_INLINE operator bool() const {
			return ptr != nullptr;
		}

		/// @brief Dereference operator.
		/// @return a reference to the managed object.
		DCA_INLINE reference operator*() const {
			if (!ptr) {
				throw dca_exception{ "Sorry, but you attempted to access a unique_ptr that is nullptr." };
			}
			return *ptr;
		}

		/// @brief Member access operator.
		/// @return the managed raw pointer.
		DCA_INLINE pointer operator->() const {
			if (!ptr) {
				throw dca_exception{ "Sorry, but you attempted to access a unique_ptr that is nullptr." };
			}
			return ptr;
		}

		/// @brief Releases the managed pointer.
		/// @return the released raw pointer.
		DCA_INLINE pointer release() {
			pointer releasedPtr = ptr;
			ptr					= nullptr;
			return releasedPtr;
		}

		/// @brief Resets the managed pointer and invokes the deleter.
		/// @param newPtr the new raw pointer to manage.
		DCA_INLINE void reset(pointer newPtr = nullptr) {
			pointer oldPtr = std::exchange(ptr, newPtr);
			if (oldPtr) {
				getDeleter()(oldPtr);
			}
		}

		/// @brief Swaps the contents of two unique_ptr objects.
		/// @param other the other unique_ptr to swap with.
		DCA_INLINE void swap(unique_ptr& other) {
			std::swap(ptr, other.ptr);
		}

		/// @brief Destructor that releases the managed object.
		DCA_INLINE ~unique_ptr() {
			reset();
		}

	  protected:
		pointer ptr{ nullptr };///< The stored object.

		/// @brief Commits a new pointer value and resets the current one.
		/// @param other the new pointer value to commit.
		DCA_INLINE void commit(pointer other) {
			pointer tempPtr = other;
			reset(tempPtr);
		}

		/// @brief Gets the deleter associated with the managed object.
		/// @return the associated deleter.
		DCA_INLINE deleter_type& getDeleter() {
			return *static_cast<deleter_type*>(this);
		}
	};

	/// @brief Helper function to create a unique_ptr for a non-array object.
	/// @param args the arguments to construct the new object from.
	/// @tparam value_type the type of value to store in the unique_ptr.
	/// @tparam deleter the type of deleter to use for the stored object.
	/// @tparam arg_types the types of arguments for constructing the object.
	/// @return unique_ptr<value_type, deleter> the managed object.
	template<typename value_type, typename deleter = std::default_delete<value_type>, typename... arg_types, std::enable_if_t<!std::is_array_v<value_type>, int32_t> = 0>
	DCA_INLINE unique_ptr<value_type, deleter> makeUnique(arg_types&&... args) {
		return unique_ptr<value_type, deleter>(new value_type(std::forward<arg_types>(args)...));
	}

	/// @brief Helper function to create a unique_ptr for a dynamic array.
	/// @param size the size to allocate for the array.
	/// @tparam value_type the type of value to store in the unique_ptr.
	/// @tparam deleter the type of deleter to use for the stored object.
	/// @return unique_ptr<value_type, deleter> the managed object.
	template<typename value_type, typename deleter = std::default_delete<value_type>, std::enable_if_t<std::is_array_v<value_type> && std::extent_v<value_type> == 0, int32_t> = 0>
	DCA_INLINE unique_ptr<value_type, deleter> makeUnique(const uint64_t size) {
		using element_type = std::remove_extent_t<value_type>;
		return unique_ptr<value_type, deleter>(new element_type[size]());
	}

	/// @brief Deleted overload for creating unique_ptr for static arrays.
	template<typename value_type, typename... arg_types, std::enable_if_t<std::extent_v<value_type> != 0, int32_t> = 0> DCA_INLINE void makeUnique(arg_types&&...) = delete;

	/**@}*/

}
