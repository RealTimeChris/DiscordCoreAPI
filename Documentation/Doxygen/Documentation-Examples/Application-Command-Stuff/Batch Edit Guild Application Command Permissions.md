Batch Editing Guild Application Command's Permissions {#batcheditguildcommandpermissions}
============
- Execute the `ApplicationCommands::batchEditGuildApplicationCommandPermissionsAsync()` function, while passing in a data structure of type `BatchEditGuildApplicationCommandPermissionsData`, with a return value of type `auto` or `std::vector<GuildApplicationCommandPermissionsData>`.
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

			BatchEditGuildApplicationCommandPermissionsData dataPackage01;
			std::vector<EditGuildApplicationCommandPermissionsData> dataPackage02 {
				{.permissions = {{.type = ApplicationCommandPermissionType::User, .permission = false, .id = "859853159115259905"}},
				 .commandName = "selldrugs"}};
			dataPackage01.guildId = args.eventData.getGuildId();
			dataPackage01.permissions = dataPackage02;

			auto returnVector = ApplicationCommands::batchEditGuildApplicationCommandPermissionsAsync(dataPackage01).get();

			for (auto value: returnVector) {
				std::cout << value.applicationId << std::endl;
			}
		}
	};
}
```
