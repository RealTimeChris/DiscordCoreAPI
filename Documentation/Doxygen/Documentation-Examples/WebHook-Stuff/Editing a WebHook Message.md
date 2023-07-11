Editing a WebHook Message {#editingawebhookmessage}
============
- Execute the, `WebHooks::editWebHookMessageAsync()` function, while passing in a value of type `EditWebHookMessageData`, with a return value of type `auto` or `Message`.
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

				ExecuteWebHookData dataPackage01 {newWebHooks[0]};
				dataPackage01.addContent("TEST CONTENT");
				dataPackage01.wait = true;

				auto newMessage = WebHooks::executeWebHookAsync(dataPackage01).get();

				GetWebHookMessageData dataPackage02;
				dataPackage02.webHookId = newWebHooks[0].id;
				dataPackage02.webhookToken = newWebHooks[0].token;
				dataPackage02.messageId = newMessage.id;

				auto newMessage02 = WebHooks::getWebHookMessageAsync(dataPackage02).get();

				std::cout << "MESSAGE ID: " << newMessage02.id << std::endl;

				EditWebHookMessageData dataPackage03 {newWebHooks[0]};
				dataPackage03.addContent("NEWER CONTENTS!");
				dataPackage03.messageId = newMessage02.id;

				auto newMessage03 = WebHooks::editWebHookMessageAsync(dataPackage03).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
