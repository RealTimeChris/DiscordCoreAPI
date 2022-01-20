// CommandController.hpp - Wrangler header for the command controller.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"
#include "SongAPI.hpp"
#include "InputEvents.hpp"
#include "VoiceConnection.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll CommandController {
	public:

		map<vector<string>, UniquePtrWrapper<BaseFunction>> functions{};
		
		CommandController(string, DiscordCoreClient*);

		void registerFunction(vector<string> functionNames, unique_ptr<BaseFunction> baseFunction);

		CoRoutine<void> checkForAndRunCommand(unique_ptr<CommandData> commandData);

		~CommandController();

	protected:

		DiscordCoreClient* discordCoreClient{ nullptr };
		string commandPrefix{ "" };

		CoRoutine<void> executeCommand(unique_ptr<BaseFunction> ptrFunction, unique_ptr<BaseFunctionArguments> args);

		unique_ptr<BaseFunction> createFunction(string functionName);

		unique_ptr<BaseFunction> getCommand(string commandName);

		vector<string> parseArguments(string messageContents);

		string parseCommandName(string messageContents);
		
	};

};
