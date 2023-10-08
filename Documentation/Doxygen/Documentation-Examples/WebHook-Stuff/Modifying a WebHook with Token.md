Modifying a WebHook with Token {#modifyingawebhookwithtoken}
============
- Execute the, `WebHooks::modifyWebHookWithTokenAsync()` function, while passing in a value of type `ModifyWebHookWithTokenData`, with a return value of type `auto` or `WebHook`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				GetGuildWebHooksData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto newWebHooks = WebHooks::getGuildWebHooksAsync(dataPackage).get();

				ModifyWebHookWithTokenData dataPackage01;
				dataPackage01.webhookToken = newWebHooks[0].token;
				dataPackage01.webHookId = newWebHooks[0].id;
				dataPackage01.name = "NEW TEST NAME";
				dataPackage01.channelId = args.eventData.getChannelId();

				auto newWebHook = WebHooks::modifyWebHookWithTokenAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() = default;
	};
}
```
