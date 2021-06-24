// CasinoStats.hpp - Header for the "Casino Stats" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CASINO_STATS_
#define _CASINO_STATS_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class CasinoStatsFunction :public BaseFunction {
	public:
		CasinoStatsFunction() {
			this->commandName = "casinostats";
			this->helpDescription = "__**Casino Stats Usage: !casinostats or /casinostats**__";
		}
		
		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);

			bool areWeAllowedHere = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

			if (!areWeAllowedHere) {
				co_return;
			}

			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
			vector<EmbedFieldData> fields;
			EmbedFieldData field1;
			field1.name = "__**Largest Coinflip Payout:**__";
			field1.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestCoinFlipPayout.userId + "> " + discordGuild.data.casinoStats.largestCoinFlipPayout.userName + "\n__Amount:__ " +
				to_string(discordGuild.data.casinoStats.largestCoinFlipPayout.amount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestCoinFlipPayout.timeStamp;
			field1.Inline = true;
			fields.push_back(field1);
			EmbedFieldData field2;
			field2.name = "__**Largest Roulette Payout:**__";
			field2.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestRoulettePayout.userId + "> " + discordGuild.data.casinoStats.largestRoulettePayout.userName + "\n__Amount:__ " +
				to_string(discordGuild.data.casinoStats.largestRoulettePayout.amount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestRoulettePayout.timeStamp;
			field2.Inline = true;
			fields.push_back(field2);
			EmbedFieldData field3;
			field3.name = "__**Largest Blackjack Payout:**__";
			field3.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestBlackjackPayout.userId + "> " + discordGuild.data.casinoStats.largestBlackjackPayout.userName + "\n__Amount:__ " +
				to_string(discordGuild.data.casinoStats.largestBlackjackPayout.amount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp;
			field3.Inline = true;
			fields.push_back(field3);
			EmbedFieldData field4;
			field4.name = "__**Largest Slots Payout:**__";
			field4.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestSlotsPayout.userId + "> " + discordGuild.data.casinoStats.largestSlotsPayout.userName + "\n__Amount:__ " +
				to_string(discordGuild.data.casinoStats.largestSlotsPayout.amount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestSlotsPayout.timeStamp;
			field4.Inline = true;
			fields.push_back(field4);
			EmbedFieldData field5;
			field5.name = "__ **Net Coinflip Payout:**__";
			field5.value = "__Amount:__ " + to_string(discordGuild.data.casinoStats.totalCoinFlipPayout) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			field5.Inline = true;
			fields.push_back(field5);
			EmbedFieldData field6;
			field6.name = "__**Net Roulette Payout:**__";
			field6.value = "__Amount:__ " + to_string(discordGuild.data.casinoStats.totalRoulettePayout) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			field6.Inline = true;
			fields.push_back(field6);
			EmbedFieldData field7;
			field7.name = "__**Net Blackjack Payout:**__";
			field7.value = "__Amount:__ " + to_string(discordGuild.data.casinoStats.totalBlackjackPayout) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			field7.Inline = true;
			fields.push_back(field7);
			EmbedFieldData field8;
			field8.name = "__**Net Slots Payout:**__";
			field8.value = "__Amount:__ " + to_string(discordGuild.data.casinoStats.totalSlotsPayout) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;
			field8.Inline = true;
			fields.push_back(field8);

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("__**Net Casino Payout:**__\n__Amount:__ " + to_string(discordGuild.data.casinoStats.totalPayout) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Server Casino Stats:**__");
			msgEmbed.fields = fields;
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.embeds.push_back(msgEmbed);
				InputEventManager::respondToEvent(dataPackage);
			}
			else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				CreateInteractionResponseData dataPackage(args->eventData);
				dataPackage.data.embeds.push_back(msgEmbed);
				InputEventManager::respondToEvent(dataPackage);
			}
			co_return;
		};
	};
	CasinoStatsFunction casinoStats;
}
#endif
