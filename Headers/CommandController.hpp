// CommandController.hpp - Wrangler header for the command controller.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"
#include "SongAPI.hpp"
#include "InputEvents.hpp"
#include "VoiceConnection.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll CommandController {
	public:

		std::map<std::vector<std::string>, UniquePtrWrapper<BaseFunction>> functions{};
		
		CommandController(std::string, DiscordCoreClient*);

		void registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction);

		CoRoutine<void> checkForAndRunCommand(std::unique_ptr<CommandData> commandData);

		~CommandController();

	protected:

		DiscordCoreClient* discordCoreClient{ nullptr };
		std::string commandPrefix{ "" };

		CoRoutine<void> executeCommand(std::unique_ptr<BaseFunction> ptrFunction, std::unique_ptr<BaseFunctionArguments> args);

		std::unique_ptr<BaseFunction> createFunction(std::string functionName);

		std::unique_ptr<BaseFunction> getCommand(std::string commandName);

		std::vector<std::string> parseArguments(std::string messageContents);

		std::string parseCommandName(std::string messageContents);
		
	};

};
