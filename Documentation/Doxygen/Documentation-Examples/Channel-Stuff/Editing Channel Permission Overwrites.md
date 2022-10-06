Edit Channel Permission Overwrites {#editingpermissionoverwrites}
=============
- Execute the `DiscordCoreAPI::Channels::editChannelPermissionOverwritesAsync()` function, while passing it a data structure of type `DiscordCoreAPI::EditChannelPermissionOverwritesData`.
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
			DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage;
			dataPackage.allow = PermissionsConverter::addPermissionsToString(
				dataPackage.allow, vector<DiscordCoreAPI::Permission> {DiscordCoreAPI::Permission::Add_Reactions, DiscordCoreAPI::Permission::Manage_Messages});
			dataPackage.deny = PermissionsConverter::removePermissionsFromString(
				dataPackage.deny, vector<DiscordCoreAPI::Permission> {DiscordCoreAPI::Permission::Attach_Files, DiscordCoreAPI::Permission::Embed_Links});
			dataPackage.channelId = args.eventData.getChannelId();
			dataPackage.type = DiscordCoreAPI::PermissionOverwritesType::User;
			dataPackage.roleOrUserId = args.eventData.getAuthorId();

			Channels::editChannelPermissionOverwritesAsync(dataPackage).get();
		}
	};
}
```