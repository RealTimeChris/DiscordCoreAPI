Modifying a Guild Member {#modifyingaguildmember}
============
- Execute the, from the `DiscordCoreAPI::GuildMembers::modifyGuildMemberAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildMemberData`, with a return value of type `DiscordCoreAPI::GuildMember`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

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
			DiscordCoreAPI::ModifyGuildMemberData dataPackage01;
			dataPackage01.currentChannelId = "";
			dataPackage01.deaf = false;
			dataPackage01.mute = false;
			dataPackage01.newVoiceChannelId = "";
			dataPackage01.nick = "TEST NICK";
			dataPackage01.roleIds = vector<string> {""};
			dataPackage01.guildId = args.eventData.getGuildId();
			dataPackage01.guildMemberId = args.eventData.getAuthorId();

			DiscordCoreAPI::GuildMember guildMember01 = DiscordCoreAPI::GuildMembers::modifyGuildMemberAsync(dataPackage01).get();
		}
	};
}
```
