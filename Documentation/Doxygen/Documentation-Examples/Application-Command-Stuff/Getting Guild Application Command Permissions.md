Getting Guild Application Command Permissions {#getguildcommandpermissions}
============
- Execute the `ApplicationCommands::getGuildApplicationCommandPermissionsAsync()` function, while passing in an argument of type `GetGuildApplicationCommandPermissionsData`, with a return value of type `auto` or `std::vector<GuildApplicationCommandPermissionData>`.
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

			GetGuildApplicationCommandPermissionsData dataPackage;
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnVector = ApplicationCommands::getGuildApplicationCommandPermissionsAsync(dataPackage).get();

			for (auto value: returnVector) {
				std::cout << value.applicationId << std::endl;
			}
		}
	};
}
```