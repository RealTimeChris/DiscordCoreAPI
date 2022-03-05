/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// CommandController.hpp - Wrangler header for the command controller.
/// May 20, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file CommandController.hpp

#pragma once

#include <FoundationEntities.hpp>
#include <CoRoutine.hpp>
#include <SongAPI.hpp>
#include <InputEvents.hpp>
#include <VoiceConnection.hpp>

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll CommandController {
	public:		
		
		CommandController(std::string, DiscordCoreClient*);

		void registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction);

		CoRoutine<void> checkForAndRunCommand(std::unique_ptr<CommandData> commandData);

	protected:

		DiscordCoreClient* discordCoreClient{ nullptr };
		std::string commandPrefix{ "" };

		std::unique_ptr<BaseFunction> createFunction(std::string functionName);

		std::unique_ptr<BaseFunction> getCommand(std::string commandName);

		std::vector<std::string> parseArguments(std::string messageContents);

		std::string parseCommandName(std::string messageContents);
		
	};

};
