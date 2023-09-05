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
/// UniquePtr.hpp - Header for the "UniquePtr" related stuff.
/// Dec 18, 2021
/// https://discordcoreapi.com
/// \file UniquePtr.hpp

#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// @brief Concept to check if two types of pointers are related.
	template<typename ValueType01, typename ValueTypeNew>
	concept IsRelatedPtr = std::derived_from<ValueType01, ValueTypeNew> || std::is_base_of_v<ValueType01, ValueTypeNew> || std::same_as<ValueType01, ValueTypeNew>;

	/// @brief A smart pointer class that provides unique ownership semantics.
	/// @tparam ValueType The type of the managed object.
	/// @tparam Deleter The type of the deleter used to destroy the object.
	template<typename ValueType, typename Deleter = std::default_delete<ValueType>> class UniquePtr : protected Deleter {
	  public:
		using element_type = std::remove_extent_t<ValueType>;
		using pointer	   = element_type*;
		using deleter_type = Deleter;
		using reference	   = element_type&;

		/// @brief Default constructor.
		inline UniquePtr() : ptr{ nullptr } {};

		inline UniquePtr& operator=(const UniquePtr&) = delete;
		inline UniquePtr(const UniquePtr&)			  = delete;

		/// @brief Move assignment operator for related pointers.
		/// @param other The other UniquePtr to move from.
		/// @return UniquePtr The new managed object inside a UniquePtr.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew>>
		inline UniquePtr& operator=(UniquePtr<ValueTypeNew, OtherDeleter>&& other) {
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

		/// @brief Move constructor for related pointers.
		/// @param other The other UniquePtr to move from.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew>>
		inline UniquePtr(UniquePtr<ValueTypeNew, OtherDeleter>&& other) {
			*this = std::move(other);
		}

		/// @brief Move assignment operator for std::unique_ptr.
		/// @param other The std::unique_ptr to move from.
		/// @return UniquePtr The new managed object inside a UniquePtr.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew>>
		inline UniquePtr& operator=(std::unique_ptr<ValueTypeNew, OtherDeleter>&& other) {
			reset(nullptr);
			try {
				commit(other.release());
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		/// @brief Move constructor for std::unique_ptr.
		/// @param other The std::unique_ptr to move from.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew>>
		inline UniquePtr(std::unique_ptr<ValueTypeNew, OtherDeleter>&& other) {
			*this = std::move(other);
		}

		/// @brief Move assignment operator for raw pointer.
		/// @param newPtr The new ptr to manage.
		/// @return UniquePtr The new managed object inside a UniquePtr.
		inline UniquePtr& operator=(pointer newPtr) {
			reset(nullptr);
			try {
				commit(newPtr);
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		/// @brief Constructor from a raw pointer.
		/// @param newPtr The new ptr to manage.
		inline UniquePtr(pointer newPtr) {
			*this = newPtr;
		}

		/// @brief Gets the managed raw pointer.
		/// @return The managed raw pointer.
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
			if (!ptr) {
				throw DCAException{ "Sorry, but you attempted to access a UniquePtr that is nullptr." };
			}
			return *ptr;
		}

		/// @brief Member access operator.
		/// @return The managed raw pointer.
		inline pointer operator->() const {
			if (!ptr) {
				throw DCAException{ "Sorry, but you attempted to access a UniquePtr that is nullptr." };
			}
			return ptr;
		}

		/// @brief Releases the managed pointer.
		/// @return The released raw pointer.
		inline pointer release() {
			pointer releasedPtr = ptr;
			ptr					= nullptr;
			return releasedPtr;
		}

		/// @brief Resets the managed pointer and invokes the deleter.
		/// @param newPtr The new raw pointer to manage.
		inline void reset(pointer newPtr) {
			pointer oldPtr = std::exchange(ptr, newPtr);
			if (oldPtr) {
				getDeleter()(oldPtr);
			}
		}

		/// @brief Swaps the contents of two UniquePtr objects.
		/// @param other The other UniquePtr to swap with.
		inline void swap(UniquePtr& other) {
			std::swap(ptr, other.ptr);
		}

		/// @brief Destructor that releases the managed object.
		inline ~UniquePtr() {
			reset(nullptr);
		}

	  protected:
		pointer ptr{ nullptr };///< The stored object.

		/// @brief Commits a new pointer value and resets the current one.
		/// @param other The new pointer value to commit.
		inline void commit(pointer other) {
			pointer tempPtr = other;
			reset(tempPtr);
		}

		/// @brief Gets the deleter associated with the managed object.
		/// @return The associated deleter.
		inline deleter_type& getDeleter() {
			return *static_cast<deleter_type*>(this);
		}
	};

	/// @brief Specialization of UniquePtr for arrays.
	/// @tparam ValueType The type of the managed object.
	/// @tparam Deleter The type of the deleter used to destroy the object.
	template<typename ValueType, typename Deleter> class UniquePtr<ValueType[], Deleter> {
	  public:
		using element_type = std::remove_extent_t<ValueType>;
		using pointer	   = element_type*;
		using deleter_type = Deleter;
		using reference	   = element_type&;

		/// @brief Default constructor.
		inline UniquePtr() : ptr{ nullptr } {};

		inline UniquePtr& operator=(const UniquePtr&) = delete;
		inline UniquePtr(const UniquePtr&)			  = delete;

		/// @brief Move assignment operator for related arrays.
		/// @param other The other UniquePtr to move from.
		/// @return UniquePtr The new managed object inside a UniquePtr.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew[]>>
		inline UniquePtr& operator=(UniquePtr<ValueTypeNew[], OtherDeleter>&& other) {
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
		/// @param other The other UniquePtr to move from.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew[]>>
		inline UniquePtr(UniquePtr<ValueTypeNew[], OtherDeleter>&& other) {
			*this = std::move(other);
		}

		/// @brief Move assignment operator for std::unique_ptr arrays.
		/// @param other The other UniquePtr to move from.
		/// @return UniquePtr The new managed object inside a UniquePtr.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew[]>>
		inline UniquePtr& operator=(std::unique_ptr<ValueTypeNew[], OtherDeleter>&& other) {
			reset(nullptr);
			try {
				commit(other.release());
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		/// @brief Move constructor for std::unique_ptr arrays.
		/// @param other The other UniquePtr to move from.
		template<IsRelatedPtr<element_type> ValueTypeNew, typename OtherDeleter = std::default_delete<ValueTypeNew[]>>
		inline UniquePtr(std::unique_ptr<ValueTypeNew[], OtherDeleter>&& other) {
			*this = std::move(other);
		}

		/// @brief Move assignment operator for raw pointer.
		/// @param newPtr The pointer to be managed.
		/// @return UniquePtr The new managed object inside a UniquePtr.
		inline UniquePtr& operator=(pointer newPtr) {
			reset(nullptr);
			try {
				commit(newPtr);
			} catch (...) {
				reset(nullptr);
				throw;
			}
			return *this;
		}

		/// @brief Constructor from a raw pointer.
		/// @param newPtr The pointer to be managed.
		inline UniquePtr(pointer newPtr) {
			*this = newPtr;
		}

		/// @brief Gets the managed raw pointer.
		/// @return pointer The managed raw pointer.
		inline pointer get() const {
			return ptr;
		}

		/// @brief Square bracket operator for accessing elements of this array.
		/// @param index The index which is to be accessed.
		/// @return reference A reference to the object that was accessed.
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
			if (!ptr) {
				throw DCAException{ "Sorry, but you attempted to access a UniquePtr that is nullptr." };
			}
			return *ptr;
		}

		/// @brief Member access operator.
		/// @return The managed raw pointer.
		inline pointer operator->() const {
			if (!ptr) {
				throw DCAException{ "Sorry, but you attempted to access a UniquePtr that is nullptr." };
			}
			return ptr;
		}

		/// @brief Releases the managed pointer.
		/// @return The released raw pointer.
		inline pointer release() {
			pointer releasedPtr = ptr;
			ptr					= nullptr;
			return releasedPtr;
		}

		/// @brief Resets the managed pointer and invokes the deleter.
		/// @param newPtr The new raw pointer to manage.
		inline void reset(pointer newPtr) {
			pointer oldPtr = std::exchange(ptr, newPtr);
			if (oldPtr) {
				getDeleter()(oldPtr);
			}
		}

		/// @brief Swaps the contents of two UniquePtr objects.
		/// @param other The other UniquePtr to swap with.
		inline void swap(UniquePtr& other) {
			std::swap(ptr, other.ptr);
		}

		/// @brief Destructor that releases the managed object.
		inline ~UniquePtr() {
			reset(nullptr);
		}

	  protected:
		pointer ptr{ nullptr };///< The stored object.

		/// @brief Commits a new pointer value and resets the current one.
		/// @param other The new pointer value to commit.
		inline void commit(pointer other) {
			pointer tempPtr = other;
			reset(tempPtr);
		}

		/// @brief Gets the deleter associated with the managed object.
		/// @return The associated deleter.
		inline deleter_type& getDeleter() {
			return *static_cast<deleter_type*>(this);
		}
	};

	/// @brief Helper function to create a UniquePtr for a non-array object.
	/// @param args The arguments to construct the new object from.
	/// @tparam ValueType The type of value to store in the UniquePtr.
	/// @tparam Deleter The type of deleter to use for the stored object.
	/// @tparam ArgTypes The types of arguments for constructing the object.
	/// @return UniquePtr<ValueType, Deleter> The managed object.
	template<typename ValueType, typename Deleter = std::default_delete<ValueType>, typename... ArgTypes, std::enable_if_t<!std::is_array_v<ValueType>, int32_t> = 0>
	inline UniquePtr<ValueType, Deleter> makeUnique(ArgTypes&&... args) {
		return UniquePtr<ValueType, Deleter>(new ValueType(std::forward<ArgTypes>(args)...));
	}

	/// @brief Helper function to create a UniquePtr for a dynamic array.
	/// @param size The size to allocate for the array.
	/// @tparam ValueType The type of value to store in the UniquePtr.
	/// @tparam Deleter The type of deleter to use for the stored object.
	/// @return UniquePtr<ValueType, Deleter> The managed object.
	template<typename ValueType, typename Deleter = std::default_delete<ValueType>, std::enable_if_t<std::is_array_v<ValueType> && std::extent_v<ValueType> == 0, int32_t> = 0>
	inline UniquePtr<ValueType, Deleter> makeUnique(const uint64_t size) {
		using element_type = std::remove_extent_t<ValueType>;
		return UniquePtr<ValueType, Deleter>(new element_type[size]());
	}

	/// @brief Deleted overload for creating UniquePtr for static arrays.
	template<typename ValueType, typename... ArgTypes, std::enable_if_t<std::extent_v<ValueType> != 0, int32_t> = 0> inline void makeUnique(ArgTypes&&...) = delete;

	/**@}*/

}