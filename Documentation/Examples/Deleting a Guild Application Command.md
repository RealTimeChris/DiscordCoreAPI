
### **Deleting a Guild Application Command:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `deleteGuildApplicationCommand()` function and execute it, while passing in a data structure of type `DeleteGuildApplicationCommandData`, with no return value.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <../DiscordCoreClient02.hpp>

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
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

		virtual  task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {

			InputEvents::deleteInputEventResponseAsync(args->eventData);

			DiscordCoreAPI::DeleteGuildApplicationCommandData dataPackage01;
			dataPackage01.name = "botinfo";
			dataPackage01.guildId = args->eventData.getGuildId();

			ApplicationCommands::deleteGuildApplicationCommand(dataPackage01);

			co_return;

		}
	};
}
#endif
```
