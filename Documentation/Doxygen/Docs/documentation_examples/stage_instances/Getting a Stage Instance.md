Getting a Stage Instance {#getting_a_stage_instance}
============
- Execute the, `discord_core_api::stage_instances::getStageInstanceAsync()` function, while passing in a value of type `discord_core_api::get_stage_instance_data`, with a return value of type `auto` or `discord_core_api::stage_instance`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

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
				get_stage_instance_data& dataPackage;
				dataPackage.channelId = "914726178022101052";

				auto responseData = stage_instances::getStageInstanceAsync(const& dataPackage).get();

				std::cout << "the topic: " << responseData.topic << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
