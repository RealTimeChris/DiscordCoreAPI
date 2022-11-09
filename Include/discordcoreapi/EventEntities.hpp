/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// EventEntities.hpp - Header for the "Event-related stuff".
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file EventEntities.hpp

#pragma once

#ifndef EVENT_ENTITIES
	#define EVENT_ENTITIES

	#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreInternal {

	/**
	 * \addtogroup discord_events
	 * @{
	 */

	/// Event-delegate token, representing an event. \brief Event-delegate token, representing an event.
	struct DiscordCoreAPI_Dll EventDelegateToken {
		template<typename RTy, typename... ArgTypes> friend class EventDelegate;

		template<typename RTy, typename... ArgTypes> friend class Event;

		friend inline bool operator==(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

		friend inline bool operator<(const EventDelegateToken& lhs, const EventDelegateToken& rhs);

		EventDelegateToken() noexcept = default;

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
	template<typename RTy, typename... ArgTypes> class EventDelegate {
	  public:
		template<typename RTy02, typename... ArgTypes02> friend class Event;

		EventDelegate<RTy, ArgTypes...>& operator=(EventDelegate<RTy, ArgTypes...>&& other) noexcept {
			if (this != &other) {
				this->function.swap(other.function);
				other.function = std::function<RTy(ArgTypes...)>{};
			}
			return *this;
		}

		EventDelegate(EventDelegate<RTy, ArgTypes...>&& other) noexcept {
			*this = std::move(other);
		}

		EventDelegate<RTy, ArgTypes...>& operator=(const EventDelegate<RTy, ArgTypes...>& other) = delete;

		EventDelegate(const EventDelegate<RTy, ArgTypes...>& other) = delete;

		EventDelegate<RTy, ArgTypes...>& operator=(std::function<RTy(ArgTypes...)> functionNew) {
			this->function = functionNew;
			return *this;
		}

		/// Constructor, taking a std::function<RTy(ArgTypes..)> as an argument. \brief Constructor, taking a std::function<RTy(ArgTypes..)> as an argument.
		EventDelegate(std::function<RTy(ArgTypes...)> functionNew) {
			*this = functionNew;
		}

		EventDelegate<RTy, ArgTypes...>& operator=(RTy (*functionNew)(ArgTypes...)) {
			this->function = functionNew;
			return *this;
		}

		/// Constructor, taking a pointer to a function of type RTy(*)(ArgTypes...) as an argument. \brief Constructor, taking a pointer to a function of type RTy(*)(ArgTypes...) as an argument.
		EventDelegate(RTy (*functionNew)(ArgTypes...)) {
			*this = functionNew;
		}

		EventDelegate() noexcept = default;

	  protected:
		std::function<RTy(ArgTypes...)> function{};
	};

	/**@}*/

	template<typename RTy, typename... ArgTypes> class Event {
	  public:
		std::map<EventDelegateToken, EventDelegate<RTy, ArgTypes...>> functions{};

		Event<RTy, ArgTypes...>& operator=(Event<RTy, ArgTypes...>&& other) noexcept {
			if (this != &other) {
				this->functions = std::move(other.functions);
				other.functions = std::map<EventDelegateToken, EventDelegate<RTy, ArgTypes...>>{};
				this->eventId = std::move(other.eventId);
				other.eventId = std::string{};
			}
			return *this;
		}

		Event(Event<RTy, ArgTypes...>&& other) noexcept {
			*this = std::move(other);
		}

		Event<RTy, ArgTypes...>& operator=(const Event<RTy, ArgTypes...>&) = delete;

		Event(const Event<RTy, ArgTypes...>&) = delete;

		Event() {
			this->eventId =
				std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
		}

		EventDelegateToken add(EventDelegate<RTy, ArgTypes...> eventDelegate) {
			EventDelegateToken eventToken{};
			eventToken.handlerId =
				std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
			eventToken.eventId = this->eventId;
			this->functions[eventToken] = std::move(eventDelegate);
			return eventToken;
		}

		void remove(EventDelegateToken eventToken) {
			if (eventToken.eventId == this->eventId) {
				if (this->functions.contains(eventToken)) {
					this->functions.erase(eventToken);
				}
			}
		}

		std::vector<RTy> operator()(ArgTypes&... args) {
			std::vector<RTy> vector{};
			for (auto& [key, value]: this->functions) {
				vector.emplace_back(value.function(args...));
			}
			return vector;
		}

	  protected:
		std::string eventId{};
	};

}// namespace DiscordCoreAPI
#endif