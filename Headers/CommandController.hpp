// CommandController.hpp - Wrangler header for the command controller.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "DiscordCoreClient.hpp"
#include "CoRoutine.hpp"
#include "SongAPI.hpp"
#include "InputEvents.hpp"
#include "VoiceConnection.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// Base arguments for the command classes. \brief Base arguments for the command classes.
	class DiscordCoreAPI_Dll BaseFunctionArguments {
	public:

		vector<string> argumentsArray{};///< A vector of string arguments.
		unique_ptr<InputEventData> eventData{ make_unique<InputEventData>() };///< InputEventData representing the input event that triggered the command.

		BaseFunctionArguments(BaseFunctionArguments& args) {
			this->eventData = make_unique<InputEventData>(*args.eventData);
			this->argumentsArray = args.argumentsArray;
		}

		BaseFunctionArguments(InputEventData inputEventData) {
			this->eventData = make_unique<InputEventData>(inputEventData);
		}

		BaseFunctionArguments(const BaseFunctionArguments& other) {
			this->eventData = make_unique<InputEventData>(*other.eventData);
			this->argumentsArray = other.argumentsArray;
		}

		virtual ~BaseFunctionArguments() {};
	};

	/// Base class DiscordCoreAPI_Dll for the command classes. \brief Base class DiscordCoreAPI_Dll for the command classes.
	class DiscordCoreAPI_Dll BaseFunction {
	public:
		string helpDescription{ "" };///< Description of the command for the Help command.
		string commandName{ "" };///< Name of the command for calling purposes.
		EmbedData helpEmbed{};///< A Message embed for displaying the command via the Help command.

		/// The base function for the command's execute function.
		/// \param args A unique_ptr containing a copy of BaseFunctionArguments.
		/// \returns A CoRoutine containing void.
		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) = 0;
		virtual unique_ptr<BaseFunction> create() = 0;
		virtual ~BaseFunction() = default;
	};
	/**@}*/

	class DiscordCoreAPI_Dll CommandController {
	public:
		friend class DiscordCoreClient;

		static map<vector<string>, unique_ptr<BaseFunction>> functions;

		static void registerFunction(vector<string> functionNames, unique_ptr<BaseFunction> baseFunction);

		static void checkForAndRunCommand(CommandData commandData);

	protected:

		static unique_ptr<BaseFunction> getCommand(string commandName, CommandData commandData);

		static CoRoutine<void> executeCommand(unique_ptr<BaseFunction> ptrFunction, BaseFunctionArguments args);

		static string parseCommandName(string messageContents, CommandData commandData);

		static unique_ptr<BaseFunction> createFunction(string functionName);

		static vector<string> parseArguments(string messageContents);

		static void cleanup();
	};

};
