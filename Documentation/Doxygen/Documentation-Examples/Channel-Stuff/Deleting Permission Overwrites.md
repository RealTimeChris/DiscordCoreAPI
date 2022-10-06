Deleting Permission Overwrites {#deletingpermissionoverwrites}
============
- Execute the `DiscordCoreAPI::Channels::deleteChannelPermissionOverwritesAsync()` function, while having passed in a data structure of type `DiscordCoreAPI::DeleteChannelPermissionOverwritesData`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

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
			DiscordCoreAPI::DeleteChannelPermissionOverwritesData dataPackage;
			dataPackage.channelId = args.eventData.getChannelId();
			dataPackage.roleOrUserId = args.eventData.getAuthorId();

			DiscordCoreAPI::Channels::deleteChannelPermissionOverwritesAsync(dataPackage).get();
		}
	};
}
```
