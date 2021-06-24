// RemoveObject.hpp - Header for the "remove object" command.
// Jun 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _REMOVE_OBJECT_
#define _REMOVE_OBJECT_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class RemoveObject :public BaseFunction {
	public:
		RemoveObject() {
			this->commandName = "removeobject";
			this->helpDescription = "__**Remove Object Usage:**__ !removeobject = OBJECTNAME or /removeobject OBJECTNAME";
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

		}
	};
	RemoveObject removeObject;
}

#endif