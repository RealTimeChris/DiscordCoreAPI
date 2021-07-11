// Slots.hpp - Header for the "slots" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SLOTS_
#define _SLOTS

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {
	class Slots :public BaseFunction {
	public:
		Slots() {
			this->commandName = "slots";
			this->helpDescription = "__**Slots Usage:**__ !slots = BETAMOUNT or /slots BETAMOUNT";
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

            bool areWeAllowed = checkIfAllowedGamingInChannel(args->eventData, discordGuild);

            if (!areWeAllowed) {
                co_return;
            }

            if (args->argumentsArray.size()==0) {
                string msgString = "------\n**Please, enter a bet amount as the first argument of the command! (!slots = BETAMOUNT)** \n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                    InputEventManager::deleteInputEventResponse(newEvent, 20000);
                }
                else {
                    CreateEphemeralInteractionResponseData dataPackage(args->eventData);
                    dataPackage.data.embeds.push_back(msgEmbed);
                    InputEventManager::respondToEvent(dataPackage);
                }
                co_return;
            }

            int betAmountOld;
            regex digitRegExp("\\d{1,18}");
            if (stoll(args->argumentsArray[0]) <= 0 || !regex_search(args->argumentsArray.at(0), digitRegExp)) {
                string msgString = "------\n**Please, enter a valid bet amount as the first argument of the command! (!slots = BETAMOUNT)**\n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                    InputEventManager::deleteInputEventResponse(newEvent, 20000);
                }
                else {
                    CreateEphemeralInteractionResponseData dataPackage(args->eventData);
                    dataPackage.data.embeds.push_back(msgEmbed);
                    InputEventManager::respondToEvent(dataPackage);
                }
                co_return;
            }
            else {
                cmatch matchResults;
                regex_search(args->argumentsArray.at(0).c_str(), matchResults, digitRegExp);
                betAmountOld = (int)stoll(matchResults.str());
            }

            GuildMember guildMember = args->eventData.discordCoreClient->guildMembers->getGuildMemberAsync({ .guildId = args->eventData.getGuildId(), .guildMemberId = args->eventData.getAuthorId() }).get();
            DiscordGuildMember discordGuildMember(guildMember.data);

            if ((unsigned int)betAmountOld > discordGuildMember.data.currency.wallet) {
                string msgString = "------\n**Sorry, but you don't have sufficient funds in your wallet for placing that bet!**\n------";
                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor(discordGuild.data.borderColor);
                msgEmbed.setDescription(msgString);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Insufficient Funds:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    ReplyMessageData dataPackage(args->eventData);
                    dataPackage.embeds.push_back(msgEmbed);
                    auto newEvent = InputEventManager::respondToEvent(dataPackage);
                    InputEventManager::deleteInputEventResponse(newEvent, 20000);
                }
                else {
                    CreateEphemeralInteractionResponseData dataPackage(args->eventData);
                    dataPackage.data.embeds.push_back(msgEmbed);
                    InputEventManager::respondToEvent(dataPackage);
                }
                co_return;
            }

            int payoutAmount;
            string gameResultType;
            vector<string> slotReel = { ":crossed_swords:", ":apple:", ":ring:", ":gun:", ":swan:", ":rocket:", ":coin:", ":star:", ":jack_o_lantern:", ":christmas_tree:" };

            srand((unsigned int)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());

            int reelStartIndex1 = (int)trunc(((float)rand() / (float)RAND_MAX) * 10.0f);
            vector<int> reelIndices1;
            reelIndices1.resize(10);
            for (unsigned int x = 0; x < 10; x += 1) {
                reelIndices1[x] = (reelStartIndex1 + x) % 10;
            }
            int reelStartIndex2 = (int)trunc(((float)rand() / (float)RAND_MAX) * 10.0f);
            vector<int> reelIndices2;
            reelIndices2.resize(10);
            for (unsigned int x = 0; x < 10; x += 1) {
                reelIndices2[x] = (reelStartIndex2 + x) % 10;
            }
            int reelStartIndex3 = (int)trunc(((float)rand() / (float)RAND_MAX) * 10.0f);
            vector<int> reelIndices3;
            reelIndices3.resize(10);
            for (unsigned int x = 0; x < 10; x += 1) {
                reelIndices3[x] = (reelStartIndex3 + x) % 10;
            }

            string msgString0 = "__**Slot Results:**__\n[:question:][:question:][:question:]\n[:question:][:question:][:question:]\n[:question:][:question:][:question:]\n\n__**Your Wager:**__ " + to_string(betAmountOld) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName +
                +"\n__**Maximum Payout:**__ " + to_string(15 * betAmountOld) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;

            EmbedData msgEmbed0;
            msgEmbed0.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
            msgEmbed0.setColor("0000FE");
            msgEmbed0.setDescription(msgString0);
            msgEmbed0.setTimeStamp(getTimeAndDate());
            msgEmbed0.setTitle("__**Slots Game:**__");

            DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_DEDICATED,
                DQTAT_COM_ASTA
            };

            ABI::Windows::System::IDispatcherQueueController* ptrNew2{ nullptr };
            check_hresult(CreateDispatcherQueueController(options, &ptrNew2));
            DispatcherQueueController queueController2 = { ptrNew2, take_ownership_from_abi };
            DispatcherQueue queue = queueController2.DispatcherQueue();
            DispatcherQueueTimer queueTimer01 = queue.CreateTimer();
            DispatcherQueueTimer queueTimer02 = queue.CreateTimer();
            DispatcherQueueTimer queueTimer03 = queue.CreateTimer();

            InputEventData newEvent01;

            if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                ReplyMessageData dataPackage(args->eventData);
                dataPackage.embeds.push_back(msgEmbed0);
                newEvent01 = InputEventManager::respondToEvent(dataPackage);
            }
            else {
                CreateInteractionResponseData dataPackage(args->eventData);
                dataPackage.data.embeds.push_back(msgEmbed0);
                newEvent01 = InputEventManager::respondToEvent(dataPackage);
            }

            queueTimer01.Interval(chrono::milliseconds(3000));
            queueTimer01.Tick([&](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args2) {
                string msgString1 = "__**Slot Results:**__\n[" + slotReel[reelIndices1[7]] + "][:question:][:question:]\n[" + slotReel[reelIndices1[8]] + "][:question:][:question:]\n[" + slotReel[reelIndices1[9]] + "][:question:][:question:]\n\n__**Your Wager:**__ " +
                    to_string(betAmountOld) + " " +args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Maximum Payout:**__ " + to_string(15 * betAmountOld) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;

                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor("0000FE");
                msgEmbed.setDescription(msgString1);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Slots Game:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    EditMessageData dataPackage(newEvent01);
                    dataPackage.embeds.push_back(msgEmbed);
                    newEvent01 = InputEventManager::respondToEvent(dataPackage);
                }
                else {
                    EditInteractionResponseData dataPackage(newEvent01);
                    dataPackage.embeds.push_back(msgEmbed);
                    newEvent01 = InputEventManager::respondToEvent(dataPackage);
                }
                queueTimer01.Stop();
                });
            queueTimer01.Start();

            queueTimer02.Interval(chrono::milliseconds(6000));
            queueTimer02.Tick([&](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args2) {
                string msgString2 = "__**Slot Results:**__\n[" + slotReel[reelIndices1[7]] + "][" + slotReel[reelIndices2[7]] + "][:question:]\n[" + slotReel[reelIndices1[8]] + "][" + slotReel[reelIndices2[8]] + "][:question:]\n"
                    + "[" + slotReel[reelIndices1[9]] + "][" + slotReel[reelIndices2[9]] + "][:question:]\n\n__**Your Wager:**__ " + to_string(betAmountOld) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Maximum Payout:**__ " +
                    to_string(15 * betAmountOld) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName;

                EmbedData msgEmbed;
                msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                msgEmbed.setColor("0000FE");
                msgEmbed.setDescription(msgString2);
                msgEmbed.setTimeStamp(getTimeAndDate());
                msgEmbed.setTitle("__**Slots Game:**__");
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    EditMessageData dataPackage(newEvent01);
                    dataPackage.embeds.push_back(msgEmbed);
                    newEvent01 = InputEventManager::respondToEvent(dataPackage);
                }
                else {
                    EditInteractionResponseData dataPackage(newEvent01);
                    dataPackage.embeds.push_back(msgEmbed);
                    newEvent01 = InputEventManager::respondToEvent(dataPackage);
                }
                queueTimer02.Stop();
                });
            queueTimer02.Start();

            queueTimer03.Interval(chrono::milliseconds(9000));
            queueTimer03.Tick([&](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args2) {
                if (slotReel[reelIndices1[8]] ==  slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] ==  slotReel[reelIndices3[8]]) {
                    gameResultType = "Triple Straight";
                    payoutAmount = betAmountOld * 15;
                }
                else if ((slotReel[reelIndices1[9]] ==  slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] == slotReel[reelIndices3[7]]) ||
                    (slotReel[reelIndices1[7]] ==  slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] ==  slotReel[reelIndices3[9]])) {
                    gameResultType = "Triple Diagonal";
                    payoutAmount = betAmountOld * 7;
                }
                else if (slotReel[reelIndices1[8]] ==  slotReel[reelIndices2[8]] || slotReel[reelIndices3[8]] ==  slotReel[reelIndices2[8]]) {
                    gameResultType = "Double Straight";
                    payoutAmount = betAmountOld * 1;
                }
                else if ((slotReel[reelIndices1[9]] ==  slotReel[reelIndices2[8]]) || (slotReel[reelIndices1[7]] ==  slotReel[reelIndices2[8]]) ||
                    (slotReel[reelIndices3[9]] ==  slotReel[reelIndices2[8]]) || (slotReel[reelIndices3[7]] ==  slotReel[reelIndices2[8]])) {
                    gameResultType = "Double Diagonal";
                    payoutAmount = betAmountOld * 1;
                }
                else {
                    gameResultType = "Loss";
                    payoutAmount = -1 * betAmountOld;
                }

                discordGuildMember.getDataFromDB();
                discordGuild.data.casinoStats.totalPayout += payoutAmount;
                discordGuild.data.casinoStats.totalSlotsPayout += payoutAmount;
                if (payoutAmount > discordGuild.data.casinoStats.largestSlotsPayout.amount) {
                    discordGuild.data.casinoStats.largestSlotsPayout.amount = payoutAmount;
                    discordGuild.data.casinoStats.largestSlotsPayout.timeStamp = getTimeAndDate();
                    discordGuild.data.casinoStats.largestSlotsPayout.userId = guildMember.data.user.id;
                    discordGuild.data.casinoStats.largestSlotsPayout.userName = guildMember.data.user.username;
                }
                discordGuild.writeDataToDB();

                discordGuildMember.getDataFromDB();
                if (betAmountOld > (int)discordGuildMember.data.currency.wallet) {
                    string msgString3 = "__**Slot Results:**__\n[:x:][:x:][:x:]\n[:x:][:x:][:x:]\n[:x:][:x:][:x:]\n------\n__**Your Wager:**__ " + to_string(betAmountOld) + "\n__**Maximum Payout:**__ " + to_string(15 * betAmountOld) +" "+
                        args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Game Results:**__\n__**Payout:**__ NSF__**Result Type:**__Non-Sufficient Funds\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) +
                        +" " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";
                    ;
                    EmbedData msgEmbed;
                    msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                    msgEmbed.setColor("0000FE");
                    msgEmbed.setDescription(msgString3);
                    msgEmbed.setTimeStamp(getTimeAndDate());
                    msgEmbed.setTitle("__**Slots Game:**__");
                    if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                        EditMessageData dataPackage(newEvent01);
                        dataPackage.embeds.push_back(msgEmbed);
                        newEvent01 = InputEventManager::respondToEvent(dataPackage);
                    }
                    else {
                        EditInteractionResponseData dataPackage(newEvent01);
                        dataPackage.embeds.push_back(msgEmbed);
                        newEvent01 = InputEventManager::respondToEvent(dataPackage);
                    }
                    return;
                }
                discordGuildMember.data.currency.wallet += payoutAmount;
                discordGuildMember.writeDataToDB();

                string msgString4 = "__**Slot Results:**__\n[" + slotReel[reelIndices1[7]] + "][" + slotReel[reelIndices2[7]] + "][" + slotReel[reelIndices3[7]] + "]\n[" + slotReel[reelIndices1[8]] + "][" + slotReel[reelIndices2[8]] + "][" + slotReel[reelIndices3[8]] + "]\n" +
                    "[" + slotReel[reelIndices1[9]] + "][" + slotReel[reelIndices2[9]] + "][" + slotReel[reelIndices3[9]] + "]\n------\n__**Your Wager:**__ " + to_string(betAmountOld) + "\n__**Maximum Payout:**__ " + to_string(15 * betAmountOld) + " "
                    + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n__**Game Results:**__\n__**Payout:**__ " + to_string(payoutAmount) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + " __**Result Type:**__ " + gameResultType +
                    "\n__**Your New Wallet Balance:**__ " + to_string(discordGuildMember.data.currency.wallet) + " " + args->eventData.discordCoreClient->discordUser->data.currencyName + "\n------";

                EmbedData msgEmbed;
                if (gameResultType ==  "Loss") {
                    msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                    msgEmbed.setColor("FE0000");
                    msgEmbed.setDescription(msgString4);
                    msgEmbed.setTimeStamp(getTimeAndDate());
                    msgEmbed.setTitle("__**Slots Game:**__");
                }
                else {
                    msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
                    msgEmbed.setColor("00FE00");
                    msgEmbed.setDescription(msgString4 );
                    msgEmbed.setTimeStamp(getTimeAndDate());
                    msgEmbed.setTitle("__**Slots Game:**__");
                }
                if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
                    EditMessageData dataPackage(newEvent01);
                    dataPackage.embeds.push_back(msgEmbed);
                    newEvent01 = InputEventManager::respondToEvent(dataPackage);
                }
                else {
                    EditInteractionResponseData dataPackage(newEvent01);
                    dataPackage.embeds.push_back(msgEmbed);
                    newEvent01 = InputEventManager::respondToEvent(dataPackage);
                }
                queueTimer03.Stop();
                });
            queueTimer03.Start();
            concurrency::wait(9500);
			co_return;
		}
	};
	Slots slots;
}

#endif

