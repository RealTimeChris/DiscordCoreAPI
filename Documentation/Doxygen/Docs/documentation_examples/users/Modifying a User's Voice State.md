Modifying a User's Voice State {#modifying_a_user_voice_state}
============
- Execute the, `discord_core_api::users::modifyUserVoiceStateAsync()` function, while passing in a value of type `discord_core_api::modify_user_voice_state_data`, with a return value of type `void`.
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
				modify_user_voice_state_data dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.suppress = false;
				dataPackage.userId = args.eventData.getAuthorId();

				users::modifyUserVoiceStateAsync(const dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
