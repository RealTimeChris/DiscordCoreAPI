Getting a Guild Vanity Invite {#gettingaguildvanityinvite}
============
- Execute the, `Guilds::getGuildVanityInviteAsync()` function, while passing in a value of type `GetGuildVanityInviteData`, with a return value of type `auto` or `InviteData`.
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
				GetGuildVanityInviteData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto responseData = Guilds::getGuildVanityInviteAsync(dataPackage01).get();

				std::cout << Boolalpha << responseData.code << std::endl;

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
