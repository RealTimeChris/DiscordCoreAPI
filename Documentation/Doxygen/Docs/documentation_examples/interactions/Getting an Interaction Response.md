Getting an Interaction Response {#gettinganinteractionresponse}
============
- Execute the, `interactions::getInteractionResponseAsync()` function, while passing in a data structure of type `get_interaction_response_data`, with a return type of `message`.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

	namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				get_interaction_response_data dataPackage;
				dataPackage.applicationId = getBotUser().id;
				dataPackage.interactionToken = args.eventData.getInteractionToken();

				auto interactionResponse = interactions::getInteractionResponseAsync(const& dataPackage).get();


			} catch (...) {
				rethrowException("test::execute() error: ");
			}
		}
	};
}
```
