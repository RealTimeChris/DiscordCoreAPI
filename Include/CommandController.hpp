/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

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
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file CommandController.hpp

#pragma once

#include <FoundationEntities.hpp>
#include <CoRoutine.hpp>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll CommandController {
	public:		

		CommandController(DiscordCoreClient*);

		void registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction);

		std::map<std::vector<std::string>, std::unique_ptr<BaseFunction>>& getFunctions();

		CoRoutine<void> checkForAndRunCommand(std::unique_ptr<CommandData> commandData);

	protected:

		DiscordCoreClient* discordCoreClient{ nullptr };

		std::unique_ptr<BaseFunction> createFunction(std::string functionName);

		std::unique_ptr<BaseFunction> getCommand(std::string commandName);
		
	};

};
