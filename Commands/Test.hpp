// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#ifndef TEST
#define TEST

#include "../Input Files/Include/Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(std::unique_ptr<BaseFunctionArguments> args) {
			EmbedData msgEmbed{};
			GetGuildMemberData getData{};
			getData.guildId = args->eventData.getGuildId();
			getData.guildMemberId = args->eventData.getAuthorId();
			GuildMember guildMember = GuildMembers::getGuildMemberAsync(getData).get();
			msgEmbed.setAuthor(guildMember.nick, guildMember.user.avatar);
			msgEmbed.setColor("FeFeFe");
			if (args->argumentsArray.size() > 0) {
				msgEmbed.setDescription(args->argumentsArray[0]);
			}
			else {
				msgEmbed.setDescription("THIS IS A TEST MESSAGE!");
			}
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Test Embed**__");
			RespondToInputEventData dataResponse{ args->eventData };
			if (args->eventData.eventType == InputEventType::Regular_Message) {
				dataResponse.type = InputEventResponseType::Regular_Message;
			}
			else {
				dataResponse.type = InputEventResponseType::Interaction_Response;
			}
			dataResponse.addMessageEmbed(msgEmbed);
			InputEvents::deleteInputEventResponseAsync(args->eventData);
			InputEvents::respondToEvent(dataResponse);
			return;
		}
		virtual ~Test() {};
	};
}
#endif