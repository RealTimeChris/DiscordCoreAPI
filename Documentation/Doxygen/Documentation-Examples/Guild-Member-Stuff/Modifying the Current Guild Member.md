Modifying the Current Guild Member {#modifyingthecurrentguildmember}
============
- Execute the, from the `GuildMembers::modifyCurrentGuildMemberAsync()` function, while passing in a value of type `ModifyCurrentGuildMemberData`, with a return value of type `auto` or `GuildMember`.
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
				ModifyCurrentGuildMemberData dataPackage;
				dataPackage.reason = "TESTING PURPOSES";
				dataPackage.nick = "TEST NICK!";
				dataPackage.guildId = args.eventData.getGuildId();

				auto guildMember = GuildMembers::modifyCurrentGuildMemberAsync(dataPackage).get();

				std::cout << "THE NAME: " << guildMember.user.userName << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
