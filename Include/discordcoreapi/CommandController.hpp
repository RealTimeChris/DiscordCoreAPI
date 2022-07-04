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
/// CommandController.hpp - Wrangler header for the command controller.
/// May 20, 2021
/// https://discordcoreapi.com
/// \file CommandController.hpp

#pragma once

#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll CommandController {
	  public:
		CommandController& operator=(const CommandController&) = delete;

		CommandController(const CommandController&) = delete;

		CommandController& operator=(CommandController&) = delete;

		CommandController(CommandController&) = delete;

		CommandController(DiscordCoreClient*);

		void registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction);

		std::map<std::vector<std::string>, std::unique_ptr<BaseFunction>>& getFunctions();

		CoRoutine<void> checkForAndRunCommand(const CommandData commandData);

	  protected:
		std::map<std::vector<std::string>, std::unique_ptr<BaseFunction>> functions{};
		DiscordCoreClient* discordCoreClient{ nullptr };

		std::unique_ptr<BaseFunction> createFunction(const std::string& functionName);

		std::unique_ptr<BaseFunction> getCommand(const std::string& commandName);
	};

};// namespace DiscordCoreAPI
