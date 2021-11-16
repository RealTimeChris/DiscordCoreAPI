### **Getting A list of Guild Emoji:**
---
- Access within the `DiscordCoreAPI` namespace, the `GuildMembers` class.
- Populate a data structure of type `DiscordCoreAPI::SearchGuildMembersData`.
- Select, from the `GuildMembers` class, the `searchGuildMembersAsync()` function and execute it, with a return value of type `auto` or `vector<GuildMember>` while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "Index.hpp"

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
				SearchGuildMembersData dataPackage{};
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.limit = 20;
				dataPackage.query = "Real";

				auto guildMembers = GuildMembers::searchGuildMembersAsync(dataPackage).get();

				for (auto value : guildMembers) {
					cout << "THE NAME: " << value.user.userName << endl;
				}

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
