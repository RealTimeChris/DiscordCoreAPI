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

		map<vector<string>, shared_ptr<BaseFunction>> functions{};
		
		CommandController(string, shared_ptr<DiscordCoreClient>);

		void registerFunction(vector<string> functionNames, shared_ptr<BaseFunction> baseFunction);

		CoRoutine<void> checkForAndRunCommand(CommandData commandData);

		~CommandController();

	protected:

		shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
		string commandPrefix{ "" };

		CoRoutine<void> executeCommand(unique_ptr<BaseFunction> ptrFunction, BaseFunctionArguments args);

		unique_ptr<BaseFunction> getCommand(string commandName, CommandData commandData);

		string parseCommandName(string messageContents, CommandData commandData);

		unique_ptr<BaseFunction> createFunction(string functionName);

		vector<string> parseArguments(string messageContents);
	};

};
