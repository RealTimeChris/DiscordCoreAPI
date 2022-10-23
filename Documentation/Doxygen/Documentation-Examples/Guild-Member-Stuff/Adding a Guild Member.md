Adding a Guild Member {#addingaguildmember}
============
- Execute the, `DiscordCoreAPI::GuildMembers::addGuildMemberAsync()` function, while passing in a value of type `DiscordCoreAPI::AddGuildMemberData`, with a return value of type `auto` or `DiscordCoreAPI::GuildMember`.
- Call the function with `.get()` added to the end in order to wait for the results now.

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
			try {
				DiscordCoreAPI::AddGuildMemberData dataPackage;
				dataPackage.accessToken = "YOUR_ACCESS_TOKEN_HERE";
				dataPackage.userId = args.eventData.getAuthorId();
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.nick = "RealTime Chris";

				auto guildMember = DiscordCoreAPI::GuildMembers::addGuildMemberAsync(dataPackage).get();

				cout << "THE NAME: " << guildMember.user.userName << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
