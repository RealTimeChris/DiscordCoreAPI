/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#ifndef TEST
#define TEST

#include "../../Include/Index.hpp"

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
			getData.guildId = args->eventData->getGuildId();
			getData.guildMemberId = args->eventData->getAuthorId();
			GuildMember guildMember = GuildMembers::getGuildMemberAsync(getData).get();
			msgEmbed.setAuthor(guildMember.nick, guildMember.user.avatar);
			msgEmbed.setColor("FeFeFe");
			if (args->commandData.optionsArgs.size() > 0) {
				msgEmbed.setDescription(args->commandData.optionsArgs[0]);
			}
			else {
				msgEmbed.setDescription("THIS IS A TEST MESSAGE!");
			}
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Test Embed**__");
			RespondToInputEventData dataResponse{ *args->eventData };
			dataResponse.setResponseType(InputEventResponseType::Interaction_Response);
			dataResponse.addMessageEmbed(msgEmbed);
			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args->eventData));
			InputEvents::respondToEvent(dataResponse);
			return;			
		}
		virtual ~Test() = default;
	};
}
#endif