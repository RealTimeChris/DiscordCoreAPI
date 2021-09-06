// Test.hpp - Header for the "test" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Test* create() {
			return new Test;
		}
		
		virtual  task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			InputEvents::deleteInputEventResponseAsync(args->eventData);

			Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**ADD OR REMOVE FUNTIME:**__");
			msgEmbed.setDescription("------\n__**Welcome, and have fun!**__\n------");

			InputEventData autoEvent = args->eventData;
			if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				ReplyMessageData dataPackage(args->eventData);
				dataPackage.addMessageEmbed(msgEmbed);
				dataPackage.addButton(false, "connect", "Connect!", "✅", ButtonStyle::Success);
				dataPackage.addButton(false, "disconnect", "Disconnect!", "❌", ButtonStyle::Success);
				dataPackage.addButton(false, "exit", "Exit!", "❌", ButtonStyle::Danger);
				autoEvent = InputEvents::respondToEvent(dataPackage);
			}
			else {
				CreateEphemeralInteractionResponseData dataPackage(args->eventData);
				dataPackage.addMessageEmbed(msgEmbed);
				dataPackage.addButton(false, "connect", "Connect!", "✅", ButtonStyle::Success);
				dataPackage.addButton(false, "disconnect", "Disconnect!", "❌", ButtonStyle::Success);
				dataPackage.addButton(false, "exit", "Exit!", "❌", ButtonStyle::Danger);
				autoEvent = InputEvents::respondToEvent(dataPackage);
			}
			int counter = 0;
			while (1) {
				counter += 1;
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					EditMessageData dataPackage(autoEvent);
					msgEmbed.setDescription("------\n__**Welcome, and have fun!\nCounter: " + to_string(counter) + "**__\n------");
					dataPackage.addMessageEmbed(msgEmbed);
					dataPackage.addButton(false, "connect", "Connect!", "✅", ButtonStyle::Success);
					dataPackage.addButton(false, "disconnect", "Disconnect!", "❌", ButtonStyle::Success);
					dataPackage.addButton(false, "exit", "Exit!", "❌", ButtonStyle::Danger);
					autoEvent = InputEvents::respondToEvent(dataPackage);
				}
				else {
					EditInteractionResponseData dataPackage(autoEvent);
					msgEmbed.setDescription("------\n__**Welcome, and have fun!\nCounter: " + to_string(counter) + "**__\n------");
					dataPackage.addMessageEmbed(msgEmbed);
					dataPackage.addButton(false, "connect", "Connect!", "✅", ButtonStyle::Success);
					dataPackage.addButton(false, "disconnect", "Disconnect!", "❌", ButtonStyle::Success);
					dataPackage.addButton(false, "exit", "Exit!", "❌", ButtonStyle::Danger);
					autoEvent = InputEvents::respondToEvent(dataPackage);
				}
				Button button(autoEvent);
				auto buttonData = button.collectButtonData(false, 60000, args->eventData.getAuthorId());

				if (buttonData.size() == 0 || buttonData.at(0).buttonId == "exit") {
					break;
				}
				else if (buttonData.at(0).buttonId == "connect") {
				}
				else if (buttonData.at(0).buttonId == "disconnect") {
					guild.disconnect();
				}
				
			}

			InputEvents::deleteInputEventResponseAsync(autoEvent).get();

			co_return;
		}
	};
}
#endif
