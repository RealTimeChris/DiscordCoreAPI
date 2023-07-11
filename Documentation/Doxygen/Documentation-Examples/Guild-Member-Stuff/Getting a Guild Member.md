Getting a Guild Member {#gettingaguildmember}
============
- Execute the, from the `GuildMembers::getCachedGuildMember()` (which collects it from the cache), or `GuildMembers::getGuildMemberAsync()` (which collects it from the Discord servers) function, while passing to it a value of type `GetGuildMemberData`.
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
				GetGuildMemberData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.guildMemberId = args.eventData.getAuthorId();

				auto guildMember01 = GuildMembers::getCachedGuildMember(dataPackage).get();

				auto guildMember02 = GuildMembers::getGuildMemberAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
