// Test.hpp - Header for the "test" command.
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
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			try {

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor("000000");
				msgEmbed.setDescription(args->argumentsArray.at(0));
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Welcome:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					ReplyMessageData responseData(args->eventData);
					responseData.messageReference.channelId = args->eventData.getChannelId();
					responseData.messageReference.messageId = args->eventData.getMessageId();
					responseData.embeds.push_back(msgEmbed);
					InputEventManager::respondToEvent(responseData);
				}
				DiscordCoreAPI::GetAuditLogData dataPackage;
				dataPackage.actionType = DiscordCoreAPI::AuditLogEvent::ROLE_UPDATE;
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.limit = 25;
				dataPackage.userId = args->eventData.getAuthorId();
				AuditLogData auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync(dataPackage).get();

				for (auto value : auditLogData.auditLogEntries) {
					for (auto value2 : value.changes) {
						cout << value2.newValueString << endl;
						cout << value2.oldValueString << endl;
					}
				}
				auto messages = args->eventData.discordCoreClient->messages->fetchMessagesAsync({ .channelId = args->eventData.getChannelId(), .limit = 100, .beforeThisId = args->eventData.getMessageId() }).get();
				vector<string> messageIds;
				for (auto value : *messages) {
					messageIds.push_back(value.data.id);
				}
				DeleteMessagesBulkData dataPackage2;
				dataPackage2.channelId = true;
				dataPackage2.messageIds = messageIds;
				args->eventData.discordCoreClient->messages->deleteMessasgeBulkAsync(dataPackage2).get();

				co_return;
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
	Test test{};
}
#endif