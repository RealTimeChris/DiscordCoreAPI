/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file EventEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	/**
	 * \addtogroup discord_events
	 * @{
	 */

	/// Event-delegate token, representing an event. \brief Event-delegate token, representing an event.
	struct DiscordCoreAPI_Dll EventDelegateToken {
		template<typename ReturnType, typename... ArgTypes> friend class EventDelegate;

		template<typename ReturnType, typename... ArgTypes> friend class Event;

		friend inline bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

		friend inline bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

		EventDelegateToken() = default;

	  protected:
		std::string handlerId{};
		std::string eventId{};
	};

	inline bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs) {
		if (lhs.eventId == rhs.eventId && lhs.handlerId == rhs.handlerId) {
			return true;
		} else {
			return false;
		}
	}

	inline bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs) {
		if (stoll(lhs.handlerId) < stoll(rhs.handlerId)) {
			return true;
		} else {
			return false;
		}
	}

	/// Event-delegate, for representing an event-function to be executed. \brief Event-delegate, for representing an event-function to be executed.
	template<typename ReturnType, typename... ArgTypes> class EventDelegate {
	  public:
		template<typename ReturnType02, typename... ArgTypes02> friend class Event;

		EventDelegate<ReturnType, ArgTypes...>& operator=(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
			if (this != &other) {
				this->theFunction.swap(other.theFunction);
				other.theFunction = std::function<ReturnType(ArgTypes...)>{};
			}
			return *this;
		}

		EventDelegate(EventDelegate<ReturnType, ArgTypes...>&& other) noexcept {
			*this = std::move(other);
		}

		EventDelegate<ReturnType, ArgTypes...>& operator=(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

		EventDelegate(const EventDelegate<ReturnType, ArgTypes...>& other) = delete;

		EventDelegate<ReturnType, ArgTypes...>& operator=(EventDelegate<ReturnType, ArgTypes...>& other) = delete;

		EventDelegate(EventDelegate<ReturnType, ArgTypes...>& other) = delete;

		EventDelegate<ReturnType, ArgTypes...>& operator=(std::function<ReturnType(ArgTypes...)> theFunctionNew) {
			this->theFunction = theFunctionNew;
			return *this;
		}

		/// Constructor, taking a std::function<ReturnType(ArgTypes..)> as an argument. \brief Constructor, taking a std::function<ReturnType(ArgTypes..)> as an argument.
		EventDelegate(std::function<ReturnType(ArgTypes...)> theFunctionNew) {
			*this = theFunctionNew;
		}

		EventDelegate<ReturnType, ArgTypes...>& operator=(ReturnType (*theFunctionNew)(ArgTypes...)) {
			this->theFunction = theFunctionNew;
			return *this;
		}

		/// Constructor, taking a pointer to a function of type ReturnType(*)(ArgTypes...) as an argument. \brief Constructor, taking a pointer to a function of type ReturnType(*)(ArgTypes...) as an argument.
		EventDelegate(ReturnType (*theFunctionNew)(ArgTypes...)) {
			*this = theFunctionNew;
		}

		EventDelegate() = default;

	  protected:
		std::function<ReturnType(ArgTypes...)> theFunction{};
	};

	/**@}*/

	template<typename ReturnType, typename... ArgTypes> class Event {
	  public:
		Event<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&& other) noexcept {
			if (this != &other) {
				this->theFunctions.swap(other.theFunctions);
				other.theFunctions = std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>>{};
				this->eventId = std::move(other.eventId);
				other.eventId = std::string{};
			}
			return *this;
		}

		Event(Event<ReturnType, ArgTypes...>&& other) noexcept {
			*this = std::move(other);
		}

		Event<ReturnType, ArgTypes...>& operator=(const Event<ReturnType, ArgTypes...>&) = delete;

		Event(const Event<ReturnType, ArgTypes...>&) = delete;

		Event<ReturnType, ArgTypes...>& operator=(Event<ReturnType, ArgTypes...>&) = delete;

		Event(Event<ReturnType, ArgTypes...>&) = delete;

		Event() {
			this->eventId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		}

		EventDelegateToken add(EventDelegate<ReturnType, ArgTypes...> eventDelegate) {
			EventDelegateToken eventToken{};
			eventToken.handlerId = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			eventToken.eventId = this->eventId;
			this->theFunctions[eventToken] = std::move(eventDelegate);
			return eventToken;
		}

		void remove(EventDelegateToken eventToken) {
			if (this->theFunctions.contains(eventToken)) {
				this->theFunctions.erase(eventToken);
			}
		}

		void operator()(ArgTypes... args) {
			std::lock_guard theLock{ this->theMutex };
			for (auto& [key, value]: this->theFunctions) {
				value.theFunction(args...);
			}
		}

	  protected:
		std::map<EventDelegateToken, EventDelegate<ReturnType, ArgTypes...>> theFunctions{};
		std::string eventId{};
		std::mutex theMutex{};
	};

}// namespace DiscordCoreAPI
