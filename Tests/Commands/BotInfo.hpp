// bot_info.hpp - Header for the "bot info" command.
// Jun 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include <discordcoreapi/Index.hpp>

namespace discord_core_api {

	class bot_info : public base_function {
	  public:
		bot_info() {
			commandName = "botinfo";
			helpDescription = "Displays some info about this bot.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /botinfo.\n------");
			msgEmbed.setTitle("__**Bot Info Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<bot_info>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				auto guilds = guilds::getAllGuildsAsync();
				int32_t userCount{ 0 };
				for (auto& value: guilds) {
					userCount += value.memberCount;
				}

				embed_data messageEmbed;
				messageEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl<user_image_types::Avatar>());
				messageEmbed.setImage(discord_core_client::getInstance()->getBotUser().getUserImageUrl<user_image_types::Avatar>());
				messageEmbed.setColor("FEFEFE");
				messageEmbed.setTitle("__**Bot Info:**__");
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.addField("__Bot Name:__",
					jsonifier::string{ discord_core_client::getInstance()->getBotUser().userName } + "#" +
						jsonifier::string{ discord_core_client::getInstance()->getBotUser().discriminator },
					true);
				messageEmbed.addField("__Bot ID:__", jsonifier::toString(discord_core_client::getInstance()->getBotUser().id.operator const uint64_t&()), true);
				messageEmbed.addField("__Guild Count:__", jsonifier::toString(guilds.size()), true);
				messageEmbed.addField("__Created At:__", discord_core_client::getInstance()->getBotUser().id.getCreatedAtTimeStamp(), true);
				messageEmbed.addField("__Serving Users:__", jsonifier::toString(userCount), true);
				messageEmbed.addField("__Running On:__", "[discord_core_api Bot Library](https://discordcoreapi.com)", true);
				messageEmbed.addField("__Created By:__", "<@931311002702737418>", true);
				auto timePassed = discord_core_client::getInstance()->getTotalUpTime().count() > 0 ? discord_core_client::getInstance()->getTotalUpTime().count() : 0;
				int64_t millisecondsPerSecond{ 1000 };
				int64_t millisecondsPerMinute{ millisecondsPerSecond * 60 };
				int64_t millisecondsPerHour{ millisecondsPerMinute * 60 };
				int64_t millisecondsPerDay{ millisecondsPerHour * 24 };
				int64_t daysPassed = ( int64_t )trunc(timePassed / millisecondsPerDay);
				int64_t hoursPassed = ( int64_t )trunc((timePassed % millisecondsPerDay) / millisecondsPerHour);
				int64_t minutesPassed = ( int64_t )trunc(((timePassed % millisecondsPerDay) % millisecondsPerHour) / millisecondsPerMinute);

				int64_t secondsPassed =
					( int64_t )trunc((((timePassed % millisecondsPerDay) % millisecondsPerHour) % millisecondsPerMinute) / millisecondsPerSecond);
				jsonifier::string string{ jsonifier::toString(daysPassed) + " Days, " + jsonifier::toString(hoursPassed) + " Hours, " +
					jsonifier::toString(minutesPassed) + " Minutes, " + jsonifier::toString(secondsPassed) + " Seconds."  };
				messageEmbed.addField("__Total Uptime:__", string, true);
				respond_to_input_event_data dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addMessageEmbed(messageEmbed);
				auto eventNew = input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const dca_exception& error) {
				std::cout << "bot_info::execute()" << error.what() << std::endl;
			}
		}
		~bot_info(){};
	};
}
