### **Adding a Recipient to a Group DM:**
---
- Access within the `DiscordCoreAPI` namespace, the `Users` class.
- Populate a data structure of type `DiscordCoreAPI::AddRecipientToGroupDMData`.
- Select, from the `Users` class, the `addRecipientToGroupDMAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void executeAsync(unique_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args->eventData.getAuthorId(), .guildId = args->eventData.getGuildId() }).get();

				AddRecipientToGroupDMData dataPackage{};
				dataPackage.userId = args->eventData.getAuthorId();
				dataPackage.channelId = "DM_CHANNEL_ID_HERE";
				dataPackage.token = "YOUR_ACCESS_TOKEN_HERE";
				dataPackage.nick = guildMember.nick;

				Users::addRecipientToGroupDMAsync(dataPackage).get();

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
