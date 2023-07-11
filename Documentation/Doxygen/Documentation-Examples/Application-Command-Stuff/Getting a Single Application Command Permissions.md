Getting a Single Guild Application Command's Permissions {#getsinglecommandpermissions}
============
- Execute the `ApplicationCommands::getApplicationCommandPermissionsAsync()` function, while passing in an argument of type `GetApplicationCommandPermissionsData`, with a return value of type `auto` or `GuildApplicationCommandPermissionData`.
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

			GetGuildApplicationCommandPermissionsData dataPackage02;
			dataPackage02.guildId = args.eventData.getGuildId();
			dataPackage02.applicationId = args.discordCoreClient->getBotUser().id;

			auto returnValue = ApplicationCommands::getApplicationCommandPermissionsAsync(dataPackage02).get();

			std::cout << returnValue.applicationId << std::endl;
		}
	};
}
```