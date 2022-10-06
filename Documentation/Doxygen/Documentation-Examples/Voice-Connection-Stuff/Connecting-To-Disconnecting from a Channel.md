Connecting To/Disconnecting From a Voice Channel {#connectingtodisconnecting}
============
- Collect/create an instance of `DiscordCoreAPI::Guild`, and also collect the Id of a voice channel to which you would like to connect. (This can be collected from the `DiscordCoreAPI::GuildMember::voiceData` object.)
- Call, the `DiscordCoreAPI::Guild::connectToVoice()` function from the `DiscordCoreAPI::Guild` object, while passing in the channel id of the channel to which you would like to connect.
- To disconnect from the channel, call the `DiscordCoreAPI::Guild::disconnect()` function on the `DiscordCoreAPI::Guild` object.
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
			DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(args.eventData);

			DiscordCoreAPI::GuildMember guildMember =
				DiscordCoreAPI::GuildMembers::getGuildMemberAsync({.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
					.get();

			DiscordCoreAPI::Guild guild = DiscordCoreAPI::Guilds::getGuildAsync({.guildId = args.eventData.getGuildId()}).get();

			auto voiceConnection = guild.connectToVoice(guildMember.voiceData.channelId);

			guild.disconnect();
		}
	};
}
```