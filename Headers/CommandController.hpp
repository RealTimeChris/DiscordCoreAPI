// CommandController.hpp - Wrangler header for the command controller.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// Base arguments for the command classes. \brief Base arguments for the command classes.
	class DiscordCoreAPI_Dll BaseFunctionArguments {
	public:
		BaseFunctionArguments(BaseFunctionArguments& args) {
			this->argumentsArray = args.argumentsArray;
			this->eventData = args.eventData;
		}
		BaseFunctionArguments(InputEventData inputEventData) {
			this->eventData = inputEventData;
		}
		vector<string> argumentsArray{};///< A vector of string arguments.
		InputEventData eventData{};///< InputEventData representing the input event that triggered the command.
	};

	/// Base class for the command classes. \brief Base class for the command classes.
	class DiscordCoreAPI_Dll BaseFunction {
	public:
		string helpDescription{ "" };///< Description of the command for the Help command.
		string commandName{ "" };///< Name of the command for calling purposes.
		EmbedData helpEmbed{};///< A message embed for displaying the command via the Help command.

		/// The base function for the command's execute function.
		/// \param args A shared_ptr containing a copy of BaseFunctionArguments.
		/// \returns A CoRoutine containing void.
		virtual CoRoutine<void> executeAsync(shared_ptr<BaseFunctionArguments>args) = 0;
		virtual BaseFunction* create() = 0;
		virtual ~BaseFunction() {};
	};
	/**@}*/

	class DiscordCoreAPI_Dll CommandController {
	public:
		static map<string, BaseFunction*> functions;

		static void registerFunction(vector<string> functionNames, BaseFunction* baseFunction);

		static void checkForAndRunCommand(CommandData commandData);

	protected:

		static shared_ptr<BaseFunction> createFunction(string functionName);

		static shared_ptr<BaseFunction> getCommand(string commandName, CommandData commandData);

		static string parseCommandName(string messageContents, CommandData commandData);

		static vector<string> parseArguments(string messageContents);

	};

};
