### **Removing a Recipient from a Group DM:**
---
- Access within the `DiscordCoreAPI` namespace, the `Users` class.
- Populate a data structure of type `DiscordCoreAPI::RemoveRecipientFromGroupDMData`.
- Select, from the `Users` class, the `removeRecipientFromGroupDMAsync()` function and execute it, with a return value of type `void`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef _TEST_
#define _TEST_

#include <IndexInitial.hpp>

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

		Test* create() {
			return new Test;
		}

		virtual CoRoutine<void> executeAsync(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args->eventData.getAuthorId(), .guildId = args->eventData.getGuildId() }).get();

				RemoveRecipientFromGroupDMData dataPackage{};
				dataPackage.userId = args->eventData.getAuthorId();
				dataPackage.channelId = "DM_CHANNEL_ID_HERE";

				Users::removeRecipientFromGroupDMAsync(dataPackage).get();

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
