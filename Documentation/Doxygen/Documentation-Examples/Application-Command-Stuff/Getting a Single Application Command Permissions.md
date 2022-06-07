Getting a Single Guild Application Command's Permissions {#getsinglecommandpermissions}
============
- Execute the `DiscordCoreAPI::ApplicationCommands::getApplicationCommandPermissionsAsync()` function, while passing in an argument of type `DiscordCoreAPI::GetApplicationCommandPermissionsData`, with a return value of type `auto` or `DiscordCoreAPI::GuildApplicationCommandPermissionData`.
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

			DiscordCoreAPI::GetGuildApplicationCommandPermissionsData dataPackage02;
			dataPackage02.guildId = args.eventData.getGuildId();
			dataPackage02.applicationId = args.discordCoreClient->getBotUser().id;

			auto returnValue = DiscordCoreAPI::ApplicationCommands::getApplicationCommandPermissionsAsync(dataPackage02).get();

			cout << returnValue.applicationId << endl;
		}
	};
}
```