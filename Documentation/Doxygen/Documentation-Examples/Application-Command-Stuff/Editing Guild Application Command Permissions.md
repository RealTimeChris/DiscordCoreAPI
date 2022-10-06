Editing a Guild Application Command's Permissions {#editguildcommandpermissions}
============
- Execute the `DiscordCoreAPI::ApplicationCommands::editGuildApplicationCommandPermissionsAsync()` function, while passing in a data structure of type `DiscordCoreAPI::EditGuildApplicationCommandPermissionsData`, with a return value of type `auto` or `DiscordCoreAPI::GuildApplicationCommandPermissionsData`.
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

			DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage;
			DiscordCoreAPI::ApplicationCommandPermissionData dataPackage02;
			dataPackage02.type = DiscordCoreAPI::ApplicationCommandPermissionType::User;
			dataPackage02.permission = true;
			dataPackage02.id = "859853159115259905";
			dataPackage.commandName = "botinfo";
			dataPackage.permissions.emplace_back(dataPackage02);
			dataPackage.guildId = args.eventData.getGuildId();

			auto returnValue = DiscordCoreAPI::ApplicationCommands::editGuildApplicationCommandPermissionsAsync(dataPackage).get();

			cout << returnValue.applicationId << endl;
		}
	};
}
```