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
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /botinfo.\n------");
			msgEmbed.setTitle("__**Bot Info Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<BotInfo>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				auto guilds = Guilds::getAllGuildsAsync();
				int32_t userCount{ 0 };
				for (auto& value: guilds) {
					userCount += value.memberCount;
				}

				EmbedData messageEmbed;
				messageEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(UserImageTypes::Avatar));
				messageEmbed.setImage(DiscordCoreClient::getInstance()->getBotUser().getUserImageUrl(UserImageTypes::Avatar));
				messageEmbed.setColor("FEFEFE");
				messageEmbed.setTitle("__**Bot Info:**__");
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.addField("__Bot Name:__",
					jsonifier::string{ DiscordCoreClient::getInstance()->getBotUser().userName } + "#" +
						jsonifier::string{ DiscordCoreClient::getInstance()->getBotUser().discriminator },
					true);
				messageEmbed.addField("__Bot ID:__", jsonifier::toString(DiscordCoreClient::getInstance()->getBotUser().id.operator const uint64_t&()), true);
				messageEmbed.addField("__Guild Count:__", jsonifier::toString(guilds.size()), true);
				messageEmbed.addField("__Created At:__", DiscordCoreClient::getInstance()->getBotUser().id.getCreatedAtTimeStamp(), true);
				messageEmbed.addField("__Serving Users:__", jsonifier::toString(userCount), true);
				messageEmbed.addField("__Running On:__", "[DiscordCoreAPI Bot Library](https://discordcoreapi.com)", true);
				messageEmbed.addField("__Created By:__", "<@931311002702737418>", true);
				auto timePassed = DiscordCoreClient::getInstance()->getTotalUpTime().count() > 0 ? DiscordCoreClient::getInstance()->getTotalUpTime().count() : 0;
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
					jsonifier::toString(minutesPassed) + " Minutes, " + jsonifier::toString(secondsPassed) + " Seconds." };
				messageEmbed.addField("__Total Uptime:__", string, true);
				RespondToInputEventData dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(messageEmbed);
				auto eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const DCAException& error) {
				std::cout << "BotInfo::execute()" << error.what() << std::endl;
			}
		}
		~BotInfo(){};
	};
}
