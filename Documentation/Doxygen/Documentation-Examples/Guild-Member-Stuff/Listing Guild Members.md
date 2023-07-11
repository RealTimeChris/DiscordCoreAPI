Listing Guild Members {#listingguildmembers}
============
- Execute the, `GuildMembers::listGuildMembersAsync()` function, while passing in a value of type `ListGuildMembersData`, with a return value of type `auto` or `std::vector<GuildMember>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

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
			try {
				ListGuildMembersData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.limit = 20;

				auto guildMembers = GuildMembers::listGuildMembersAsync(dataPackage).get();

				for (auto value: guildMembers) {
					std::cout << "THE NAME: " << value.user.userName << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
