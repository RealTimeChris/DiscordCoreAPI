Creating a Stage Instance {#creating_a_stage_instance}
=============
- Execute the, `discord_core_api::stage_instances::createStageInstanceAsync()` function, while passing in a value of type `discord_core_api::create_stage_instance_data`, with a return value of type `auto` or `discord_core_api::stage_instance`.
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
				create_stage_instance_data& dataPackage;
				dataPackage.privacyLevel = stage_instance_privacy_level::GUILD_ONLY;
				dataPackage.channelId = "914726178022101052";
				dataPackage.reason = "testing purposes!";
				dataPackage.topic = "testing!";

				auto responseData = stage_instances::createStageInstanceAsync(dataPackage).get();

				std::cout << "the name: " << responseData.topic << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
