// BotInfo.hpp - Header for the "bot info" command.
// Jun 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include <discordcoreapi/Index.hpp>

namespace DiscordCoreAPI {
	class BotInfo : public BaseFunction {
	  public:
		BotInfo() {
			commandName = "botinfo";
			helpDescription = "Displays some info about this bot.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /botinfo.\n------");
			msgEmbed.setTitle("__**Bot Info Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		DiscordCoreAPI::UniquePtr<BaseFunction> create() {
			return DiscordCoreAPI::makeUnique<BotInfo>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				auto guilds = Guilds::getAllGuildsAsync().get();
				int32_t userCount{ 0 };
				for (auto& value: guilds) {
					userCount += value.memberCount;
				}
				EmbedData messageEmbed;
				messageEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getAvatarUrl());
				messageEmbed.setImage(argsNew.discordCoreClient->getBotUser().getAvatarUrl());
				messageEmbed.setColor("FEFEFE");
				messageEmbed.setTitle("__**Bot Info:**__");
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.addField("__Bot Name:__",
					argsNew.discordCoreClient->getBotUser().userName + "#" + std::string{ argsNew.discordCoreClient->getBotUser().discriminator },
					true);
				messageEmbed.addField("__Bot ID:__", std::to_string(argsNew.discordCoreClient->getBotUser().id.operator size_t()), true);
				messageEmbed.addField("__Guild Count:__", std::to_string(guilds.size()), true);
				messageEmbed.addField("__Created At:__", argsNew.discordCoreClient->getBotUser().id.getCreatedAtTimeStamp(TimeFormat::LongDateTime),
					true);
				messageEmbed.addField("__Serving Users:__", std::to_string(userCount), true);
				messageEmbed.addField("__Running On:__", "[DiscordCoreAPI Bot Library](https://discordcoreapi.com)", true);
				messageEmbed.addField("__Created By:__", "RealTime Chris#3627", true);
				auto timePassed = argsNew.discordCoreClient->getTotalUpTime().count() > 0 ? argsNew.discordCoreClient->getTotalUpTime().count() : 0;
				int64_t millisecondsPerSecond{ 1000 };
				int64_t millisecondsPerMinute{ millisecondsPerSecond * 60 };
				int64_t millisecondsPerHour{ millisecondsPerMinute * 60 };
				int64_t millisecondsPerDay{ millisecondsPerHour * 24 };
				int64_t daysPassed = ( int64_t )trunc(timePassed / millisecondsPerDay);
				int64_t hoursPassed = ( int64_t )trunc((timePassed % millisecondsPerDay) / millisecondsPerHour);
				int64_t minutesPassed = ( int64_t )trunc(((timePassed % millisecondsPerDay) % millisecondsPerHour) / millisecondsPerMinute);

				int64_t secondsPassed =
					( int64_t )trunc((((timePassed % millisecondsPerDay) % millisecondsPerHour) % millisecondsPerMinute) / millisecondsPerSecond);
				std::string string{ std::to_string(daysPassed) + " Days, " + std::to_string(hoursPassed) + " Hours, " +
					std::to_string(minutesPassed) + " Minutes, " + std::to_string(secondsPassed) + " Seconds." };
				messageEmbed.addField("__Total Uptime:__", string, true);
				RespondToInputEventData dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(messageEmbed);
				auto eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (...) {
				reportException("BotInfo::execute()");
			}
		}
		~BotInfo(){};
	};
}
