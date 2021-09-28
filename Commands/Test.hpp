// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Test* create() {
			return new Test;
		}

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData).get();

			RespondToInputEventData dataPackage{ args->eventData };
			dataPackage.type = DesiredInputEventResponseType::InteractionResponse;
			dataPackage.addButton(false, "test_button", "Test Button", "✅", ButtonStyle::Danger);
			dataPackage.addContent("Test Response");
			auto inputEventData = InputEvents::respondToEvent(dataPackage);

			ButtonCollector buttonCollector(inputEventData);
			buttonCollector.collectButtonData(false, 120000, 1, args->eventData.getAuthorId());

			InputEvents::deleteInputEventResponseAsync(inputEventData).get();

			co_return;
		}
	};
}
#endif
