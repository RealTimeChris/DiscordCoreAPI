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

	template<typename value_type, typename deleter_type_new> class Ptr : public deleter_type_new {
	  public:
		using pointer	   = value_type*;
		using deleter_type = deleter_type_new;

		constexpr Ptr() noexcept = default;

		Ptr(pointer p) : _impl_t{} {
			_impl_ptr() = p;
		}

		template<typename Del> Ptr(pointer p, Del&& d) : _impl_t{ p, std::forward<Del>(d) } {
		}

		~Ptr() noexcept = default;

		pointer& _impl_ptr() {
			return _impl_t;
		}
		pointer _impl_ptr() const {
			return _impl_t;
		}
		deleter_type& _impl_deleter() {
			return *this;
		}
		const deleter_type& _impl_deleter() const {
			return *this;
		}

	  private:
		pointer _impl_t;
	};

	// control block for reference counting of counted_ptr and weak_ptr

	/**
     * NOT implemented: custom allocator support.
     *
     * The allocator is intended to be used to allocate and deallocate
     *  internal counted_ptr details, not the object.
     */

	template<typename value_type> class counted_ptr;

	template<typename value_type, typename deleter_type_new = std::default_delete<value_type>> class control_block {
	  public:
		using element_type = value_type;
		using deleter_type = deleter_type_new;

		// Constructors

		control_block(value_type* p) : _impl{ p } {
		}

		control_block(value_type* p, deleter_type_new d) : _impl{ p, d } {
		}


		// Destructor

		~control_block() {
		}

		inline element_type* get() {
			return _impl._impl_ptr();
		}

		// Modifiers

		void inc_ref() noexcept {
			++_use_count;
		}

		void inc_wref() noexcept {
			++_weak_use_count;
		}

		void dec_ref() noexcept {
			auto _ptr	   = _impl._impl_ptr();
			auto& _deleter = _impl._impl_deleter();
			if (--_use_count == 0) {
				if (_ptr)
					_deleter(_ptr);// destroy the object _ptr points to
				dec_wref();
			}
		}

		void dec_wref() noexcept {
			if (--_weak_use_count == 0) {
				delete this;// destroy control_block itself
			}
		}

		// Observers

		long use_count() const noexcept// Returns #counted_ptr
		{
			return _use_count;
		}

		bool unique() const noexcept {
			return _use_count == 1;
		}

		long weak_use_count() const noexcept// Returns #weak_ptr
		{
			return _weak_use_count - ((_use_count > 0) ? 1 : 0);
		}

		bool expired() const noexcept {
			return _use_count == 0;
		}

		void* get_deleter() noexcept// Type erasure for storing deleter
		{
			return reinterpret_cast<void*>(std::addressof(_impl._impl_deleter()));
		}

	  private:
		std::atomic<long> _use_count{ 1 };
		std::atomic<long> _weak_use_count{ 1 };// Note: _weak_use_count = #weak_ptrs + (#counted_ptr > 0) ? 1 : 0
		Ptr<value_type, deleter_type_new> _impl;
	};

	/**
     * NOT implemented: custom allocator support.
     *
     * The allocator is intended to be used to allocate and deallocate
     *  internal counted_ptr details, not the object.
     */

	template<typename value_type> class counted_ptr {
	  public:
		template<typename U> friend class counted_ptr;

		template<typename U> friend class weak_ptr;

		template<typename deleter_type_new, typename U> friend deleter_type_new* get_deleter(const counted_ptr<U>&) noexcept;

		using element_type = value_type;
		using pointer	   = element_type*;

		// 20.7.2.2.1, constructors

		/// Default constructor, creates a counted_ptr with no managed object
		/// Postconditions: use_count() == 0 && get() == 0.
		constexpr counted_ptr() noexcept : _control_block{} {
		}

		/// Constructs a counted_ptr with no managed object
		/// Postconditions: use_count() == 0 && get() == 0.
		constexpr counted_ptr(std::nullptr_t) noexcept : _control_block{} {
		}

		/// Constructs a counted_ptr with p as the pointer to the managed object
		/// Postconditions: use_count() == 1 && get() == p.
		template<typename U> explicit counted_ptr(U* p) : _control_block{ new control_block<U>{ p } } {
		}

		/// Constructs a counted_ptr with p as the pointer to the managed object,
		///     supplied with custom deleter
		/// Postconditions: use_count() == 1 && get() == p.
		template<typename U, typename deleter_type_new> counted_ptr(U* p, deleter_type_new d) : _control_block{ new control_block<U, deleter_type_new>{ p, std::move(d) } } {
		}

		/// Constructs a counted_ptr with p as the pointer to the managed object,
		///     supplied with custom deleter and allocator
		/// Postconditions: use_count() == 1 && get() == p.
		template<typename U, typename deleter_type_new, typename A> counted_ptr(U* p, deleter_type_new d, A a) = delete;

		/// Constructs a counted_ptr with no managed object,
		///     supplied with custom deleter
		/// Postconditions: use_count() == 1 && get() == 0.
		template<typename deleter_type_new> counted_ptr(std::nullptr_t p, deleter_type_new d)
			: _control_block{ new control_block<value_type, deleter_type_new>{ p, std::move(d) } } {
		}

		/// Constructs a counted_ptr with no managed object,
		///     supplied with custom deleter and allocator
		/// Postconditions: use_count() == 1 && get() == 0.
		template<typename deleter_type_new, typename A> counted_ptr(std::nullptr_t p, deleter_type_new d, A a) = delete;

		/// Aliasing constructor: constructs a counted_ptr instance that
		///     stores p and shares ownership with sp
		/// Postconditions: use_count() == sp.use_count() && get() == p.
		template<typename U> counted_ptr(const counted_ptr<U>& sp, value_type* p) noexcept : _control_block{ sp._control_block } {
			if (_control_block)
				_control_block->inc_ref();
		}

		/// Copy constructor: shares ownership of the object managed by sp
		/// Postconditions: use_count() == sp.use_count() && get() == sp.get().
		counted_ptr(const counted_ptr& sp) noexcept : _control_block{ sp._control_block } {
			if (_control_block)
				_control_block->inc_ref();
		}

		/// Copy constructor: shares ownership of the object managed by sp
		/// Postconditions: use_count() == sp.use_count() && get() == sp.get().
		template<typename U> counted_ptr(const counted_ptr<U>& sp) noexcept : _control_block{ sp._control_block } {
			if (_control_block)
				_control_block->inc_ref();
		}

		/// Move constructor: Move-constructs a counted_ptr from sp
		/// Postconditions: *this shall contain the old value of sp.
		///     sp shall be empty. sp.get() == 0.
		counted_ptr(counted_ptr&& sp) noexcept : _control_block{ std::move(sp._control_block) } {
			sp._control_block = nullptr;
		}

		/// Move constructor: Move-constructs a counted_ptr from sp
		/// Postconditions: *this shall contain the old value of sp.
		///     sp shall be empty. sp.get() == 0.
		template<typename U> counted_ptr(counted_ptr<U>&& sp) noexcept : _control_block{ sp._control_block } {
			sp._control_block = nullptr;
		}

		/// Constructs a counted_ptr object that obtains ownership from up
		/// Postconditions: use_count() == 1. up shall be empty. up.get() = 0.
		template<typename U, typename deleter_type_new> counted_ptr(unique_ptr<U, deleter_type_new>&& up) : counted_ptr{ up.release(), up.get_deleter() } {
		}

		// 20.7.2.2.2, destructor

		~counted_ptr() {
			if (_control_block)
				_control_block->dec_ref();
		}

		// 20.7.2.2.3, assignment

		/// Copy assignment
		counted_ptr& operator=(const counted_ptr& sp) noexcept {
			counted_ptr{ sp }.swap(*this);
			return *this;
		}

		/// Copy assignment
		template<typename U> counted_ptr& operator=(const counted_ptr<U>& sp) noexcept {
			counted_ptr{ sp }.swap(*this);
			return *this;
		}

		/// Move assignment
		counted_ptr& operator=(counted_ptr&& sp) noexcept {
			counted_ptr{ std::move(sp) }.swap(*this);
			return *this;
		}

		/// Move assignment
		template<typename U> counted_ptr& operator=(counted_ptr<U>&& sp) noexcept {
			counted_ptr{ std::move(sp) }.swap(*this);
			return *this;
		}

		/// Move assignment from a unique_ptr
		template<typename U, typename deleter_type_new> counted_ptr& operator=(unique_ptr<U, deleter_type_new>&& up) noexcept {
			counted_ptr{ std::move(up) }.swap(*this);
			return *this;
		}

		// 20.7.2.2.4, modifiers

		/// Exchanges the contents of *this and sp
		void swap(counted_ptr& sp) noexcept {
			using std::swap;
			swap(_control_block, sp._control_block);
		}

		/// Resets *this to empty
		void reset() noexcept {
			counted_ptr{}.swap(*this);
		}

		/// Resets *this with p as the pointer to the managed object
		template<typename U> void reset(U* p) {
			counted_ptr{ p }.swap(*this);
		}

		/// Resets *this with p as the pointer to the managed object,
		///     supplied with custom deleter
		template<typename U, typename deleter_type_new> void reset(U* p, deleter_type_new d) {
			counted_ptr{ p, d }.swap(*this);
		}

		/// Resets *this with p as the pointer to the managed object,
		///     supplied with custom deleter and allocator
		template<typename U, typename deleter_type_new, typename A> void reset(U* p, deleter_type_new d, A a) = delete;

		// 20.7.2.2.5, observers

		/// Gets the stored pointer
		element_type* get() const noexcept {
			return _control_block->get();
		}

		inline pointer operator->() {
			return _control_block->get();
		}

		/* implemented in shared_ptr_access<value_type> */

		// /// Dereferences pointer to the managed object
		// element_type&
		// operator*() const noexcept
		// { return *_ptr; }

		// /// Dereferences pointer to the managed object
		// element_type*
		// operator->() const noexcept
		// { return _ptr; }

		/// Gets use_count
		long use_count() const noexcept {
			return (_control_block) ? _control_block->use_count() : 0;
		}

		/* deprecated in C++17, removed in C++20 */
		/// Checks if use_count == 1
		bool unique() const noexcept {
			return (_control_block) ? _control_block->unique() : false;
		}

		/// Checks if there is a managed object
		explicit operator bool() const noexcept {
			return (_control_block->get()) ? true : false;
		}

		/// Checks whether this counted_ptr precedes other in owner-based order
		/// Implemented by comparing the address of control_block
		template<typename U> bool owner_before(counted_ptr<U> const& sp) const {
			return std::less<control_block<element_type>*>()(_control_block, sp._control_block);
		}

	  private:
		control_block<element_type>* _control_block;
	};

	template<typename value_type, typename... Args> counted_ptr<value_type> makeCounted(Args&&... args) {
		return counted_ptr<value_type>(new value_type(std::forward<Args>(args)...));
	}

	/// @brief Creates a counted_ptr instance for arrays with zero extent.
	/// @tparam value_type The type of array element.
	/// @tparam deleter The deleter type to be used (default: std::default_delete<value_type>).
	/// @param size The size of the array to be created.
	/// @return A counted_ptr instance managing the created array.
	template<typename value_type, typename deleter = std::default_delete<value_type>, std::enable_if_t<std::is_array_v<value_type> && std::extent_v<value_type> == 0, uint64_t> = 0>
	inline counted_ptr<value_type> makeCounted(const uint64_t size) {
		using element_type = std::remove_extent_t<value_type>;
		return counted_ptr<value_type>(new element_type[size]());
	}

	/// @brief Explicitly deletes the function for arrays with non-zero extent.
	/// @tparam value_type The type of array with non-zero extent.
	/// @tparam ArgTypes The argument types (not applicable for deletion).
	template<typename value_type, typename... ArgTypes, std::enable_if_t<std::extent_v<value_type> != 0, uint64_t> = 0> inline void makeCounted(ArgTypes&&...) = delete;
	/**@}*/

}
