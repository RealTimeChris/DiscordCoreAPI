Edit Channel Permission Overwrites {#editingpermissionoverwrites}
=============
- Execute the `Channels::editChannelPermissionOverwritesAsync()` function, while passing it a data structure of type `EditChannelPermissionOverwritesData`.
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
			EditChannelPermissionOverwritesData dataPackage;
			dataPackage.allow = PermissionsConverter::addPermissionsToString(
				dataPackage.allow, vector<Permission> {Permission::Add_Reactions, Permission::Manage_Messages});
			dataPackage.deny = PermissionsConverter::removePermissionsFromString(
				dataPackage.deny, vector<Permission> {Permission::Attach_Files, Permission::Embed_Links});
			dataPackage.channelId = args.eventData.getChannelId();
			dataPackage.type = PermissionOverwritesType::User;
			dataPackage.roleOrUserId = args.eventData.getAuthorId();

			Channels::editChannelPermissionOverwritesAsync(dataPackage).get();
		}
	};
}
```