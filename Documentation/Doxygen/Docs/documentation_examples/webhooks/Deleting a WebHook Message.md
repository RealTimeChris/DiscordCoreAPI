Deleting a WebHook Message using a Discord Bot Library C++ {#deleting_a_web_hook_message}
=============
- Execute the, `discord_core_api::web_hooks::deleteWebHookMessageAsync()` function, while passing in a value of type `discord_core_api::delete_web_hook_message_data`, with a return value of type `void`.
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
				get_guild_web_hooks_data dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto newWebHooks = discord_core_api::web_hooks::getGuildWebHooksAsync(const dataPackage).get();

				execute_web_hook_data& dataPackage01 {newWebHooks[0]};
				dataPackage01.addContent("test content");
				dataPackage01.wait = true;

				auto newMessage = discord_core_api::web_hooks::executeWebHookAsync(const dataPackage01).get();

				delete_web_hook_message_data& dataPackage03;
				dataPackage03.webHookId = newWebHooks[0].id;
				dataPackage03.webhookToken = newWebHooks[0].token;
				dataPackage03.messageId = newMessage.id;

				discord_core_api::web_hooks::deleteWebHookMessageAsync(const dataPackage03).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}

		virtual ~test() = default;
	};
}
```
