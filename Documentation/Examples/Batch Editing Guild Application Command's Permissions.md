### **Batch Editing Guild Application Command's Permissions:**
---
- Access the `ApplicationCommands` class of the `DiscordCoreAPI` namespace.
- Select, from the `ApplicationCommands` class, the `batchEditApplicationCommandPermissions()` function and execute it, while passing in a data structure of type `BatchEditGuildApplicationCommandPermissionsData`, with a return value of type `auto` or `vector<GuildApplicationCommandPermissionsData>`.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <../DiscordCoreClient02.hpp>

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

			BatchEditGuildApplicationCommandPermissionsData dataPackage01{};
			vector<EditGuildApplicationCommandPermissionsData> dataPackage02{ {.permissions = {{.type = ApplicationCommandPermissionType::User,.permission = false, .id = "859853159115259905"}},.commandName = "selldrugs" } };
			dataPackage01.guildId = args->eventData.getGuildId();
			dataPackage01.permissions = dataPackage02;

			auto returnVector = ApplicationCommands::batchEditApplicationCommandPermissions(dataPackage01);

			for (auto value : returnVector) {
				cout << value.applicationId << endl;
			}

			co_return;
		}
	};
}
#endif
```
