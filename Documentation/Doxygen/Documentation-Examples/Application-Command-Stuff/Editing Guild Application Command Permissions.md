Editing a Guild Application Command's Permissions {#editguildcommandpermissions}
============
- Execute the `ApplicationCommands::editGuildApplicationCommandPermissionsAsync()` function, while passing in a data structure of type `EditGuildApplicationCommandPermissionsData`, with a return value of type `auto` or `GuildApplicationCommandPermissionsData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
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
			InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			EditGuildApplicationCommandPermissionsData dataPackage;
			ApplicationCommandPermissionData dataPackage02;
			dataPackage02.type = ApplicationCommandPermissionType::User;
			dataPackage02.permission = true;
			dataPackage02.id = "859853159115259905";
			dataPackage.commandName = "botinfo";
			dataPackage.permissions.emplace_back(dataPackage02);
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnValue = ApplicationCommands::editGuildApplicationCommandPermissionsAsync(dataPackage).get();

			std::cout << returnValue.applicationId << std::endl;
		}
	};
}
```