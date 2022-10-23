Batch Editing Guild Application Command's Permissions {#batcheditguildcommandpermissions}
============
- Execute the `DiscordCoreAPI::ApplicationCommands::batchEditGuildApplicationCommandPermissionsAsync()` function, while passing in a data structure of type `DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData`, with a return value of type `auto` or `std::vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData).get();

			DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData dataPackage01;
			std::vector<DiscordCoreAPI::EditGuildApplicationCommandPermissionsData> dataPackage02 {
				{.permissions = {{.type = ApplicationCommandPermissionType::User, .permission = false, .id = "859853159115259905"}},
				 .commandName = "selldrugs"}};
			dataPackage01.guildId = args.eventData.getGuildId();
			dataPackage01.permissions = dataPackage02;

			auto returnVector = DiscordCoreAPI::ApplicationCommands::batchEditGuildApplicationCommandPermissionsAsync(dataPackage01).get();

			for (auto value: returnVector) {
				cout << value.applicationId << endl;
			}
		}
	};
}
```
