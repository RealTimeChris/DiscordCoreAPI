// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			GetAuditLogData dataPackage;
			dataPackage.actionType = AuditLogEvent::MEMBER_BAN_ADD;
			dataPackage.guildId = args->eventData.getGuildId();
			dataPackage.limit = 25;
			dataPackage.userId = args->eventData.getAuthorId();
			
			AuditLogData auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync(dataPackage).get();

			for (auto value : auditLogData.auditLogEntries) {
				cout << "ACTION TYPE: " << to_string((int)value.actionType) << " TARGET: <@!" << value.targetId << ">." << endl;
			}

			co_return;
		}
	};
}
#endif