/*
*
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
/// CoRoutine.cpp - Source file for the CoRoutine stuff.
/// May 15, 2022
/// Chris M.
/// https://discordcoreapi.com
/// \file CoRoutine.cpp

#pragma once

#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	DiscordCoreInternal::CoRoutineThreadPool CoRoutineBase::threadPool{};
}// namespace DiscordCoreAPI