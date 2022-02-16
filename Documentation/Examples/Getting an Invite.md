### **Getting an Invite:**
---
- Access within the `DiscordCoreAPI` namespace, the `Guilds` class.
- Populate a data structure of type `DiscordCoreAPI::GetInviteData`
- Select, from the `Guilds` class, the `getInviteAsync()` function and execute it, with a return value of type `auto` or `InviteData` while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

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

		unique_ptr<BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) {
			try {
				GetGuildInvitesData dataPackage{};
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseVector = Guilds::getGuildInvitesAsync(dataPackage).get();

				GetInviteData dataPackage01{};
				dataPackage01.withExpiration = true;
				dataPackage01.withCount = true;
				dataPackage01.inviteId = responseVector[0].code;

				auto responseData = Guilds::getInviteAsync(dataPackage01).get();

				cout << "THE CODE: " << responseData.code << endl;

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
