// GamehouseOptions.hpp - Header for the "gamehouse options" command.
// Jun 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GAMEHOUSE_OPTIONS_
#define _GAMEHOUSE_OPTIONS_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class GamehouseOptions : public BaseFunction {
	public:
		GamehouseOptions() {
			this->commandName = "gamehouseoptions";
			this->helpDescription = "__**Gamehouse Options Usage:**__ !gamehouseoptions or /gamehouseoptions.";
		};

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			Channel channel = args->eventData.discordCoreClient->channels->getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

			bool areWeInADm = areWeInADM(args->eventData, channel);

			if (areWeInADm) {
				co_return;
			}

			InputEventManager::deleteInputEventResponse(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild.data);
			GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
			bool doWeHaveAdminPerms = doWeHaveAdminPermissions(args->eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPerms == false) {
				co_return;
			}

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**GameHouse Options:**__");
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("**Enter '!help = COMMANDNAME to get instructions for each option!**");

			vector<EmbedFieldData> fields;
			string resultIcon = "❌";
			if (discordGuild.data.guildShop.items.size() > 0) {
				resultIcon = "✅";
			}
			EmbedFieldData shopItemsField; shopItemsField.name = "__**Shop Items:**__";
			shopItemsField.value = "__Active:__ " + resultIcon + "\n__Command(s):__ '!addshopitem', '!removeshopitem'";
			shopItemsField.Inline = true;

			fields.push_back(shopItemsField);

			resultIcon = "❌";
			if (discordGuild.data.guildShop.roles.size() > 0) {
				resultIcon = "✅";
			}
			EmbedFieldData shopRolesField;
			shopRolesField.name = "__**Shop Roles:**__";
			shopRolesField.value = "__Active:__ " + resultIcon + "\n__Command(s):__ '!addshoprole', '!removeshoprole'";
			shopRolesField.Inline = true;

			fields.push_back(shopRolesField);

			resultIcon = "❌";
			if (discordGuild.data.gameChannelIds.size() > 0) {
				resultIcon = "✅";
			}
			EmbedFieldData gameChannelsField;
			gameChannelsField.name = "__**Restricted Game Activity To Specific Channels:**__";
			gameChannelsField.value = "__Active:__ " + resultIcon + "\n__Command(s):__ '!setgamechannel'";
			gameChannelsField.Inline = true;
			fields.push_back(gameChannelsField);

			msgEmbed.fields = fields;
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.embeds.push_back(msgEmbed);
				InputEventManager::respondToEvent(dataPackage);
			}
			else {
				CreateInteractionResponseData dataPackage(args->eventData);
				dataPackage.data.embeds.push_back(msgEmbed);
				InputEventManager::respondToEvent(dataPackage);
			}

			co_return;
		}
	};

	GamehouseOptions gamehouseOptions;
}

#endif 