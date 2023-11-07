Getting Channel Invites {#getting_channel_invites}
============
- Execute the `discord_core_api::channels::getChannelInvitesAsync()` function, while passing in a data structure of type `discord_core_api::get_channel_invites_data`, with a return value of `auto` or `jsonifier::vector<discord_core_api::invite_data>`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				get_channel_invites_data& dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();

				vector<discord_core_api::invite_data> channelInvites = discord_core_api::channels::getChannelInvitesAsync(const& dataPackage).get();

				std::cout << "the invites: " << std::endl;
				for (const auto& value: channelInvites) {
					std::cout << value.inviter.userName << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
