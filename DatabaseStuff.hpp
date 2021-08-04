// DatabaseStuff.hpp - Database stuff.
// May 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATABASE_STUFF_
#define _DATABASE_STUFF_

#include "../pch.h"
#include "GuildStuff.hpp"
#include "GuildMemberStuff.hpp"

namespace DiscordCoreAPI {

    class DatabaseManagerAgent;

    struct DiscordUserData {
        vector<string> botCommanders = { "", "", "" };
        string currencyName = "MBux";
        int32_t guildCount = 0;
        int32_t hoursOfDrugSaleCooldown = 3;
        int32_t hoursOfDepositCooldown = 24;
        float hoursOfRobberyCooldown = 0.100f;
        string prefix = "!";
        string userId = "";
        string userName = "";
    };

    struct Card {
        string suit = "";
        string type = "";
        unsigned int value = 0;
    };

    class Deck {
    public:
        vector<Card> cards;

        Deck() {
            this->cards.resize(52);

            for (auto x = 0; x < 52; x += 1) {
                this->cards[x].suit = "";
                this->cards[x].type = "";
                this->cards[x].value = 0;

                if (trunc(x / 13) == 0) {
                    this->cards[x].suit = ":hearts:";
                }
                else if (trunc(x / 13) == 1) {
                    this->cards[x].suit = ":diamonds:";
                }
                else if (trunc(x / 13) == 2) {
                    this->cards[x].suit = ":clubs:";
                }
                else if (trunc(x / 13) == 3) {
                    this->cards[x].suit = ":spades:";
                }

                if (x % 13 == 0) {
                    this->cards[x].type = "Ace";
                    this->cards[x].value = 0;
                }
                else if (x % 13 == 1) {
                    this->cards[x].type = "2";
                    this->cards[x].value = 2;
                }
                else if (x % 13 == 2) {
                    this->cards[x].type = "3";
                    this->cards[x].value = 3;
                }
                else if (x % 13 == 3) {
                    this->cards[x].type = "4";
                    this->cards[x].value = 4;
                }
                else if (x % 13 == 4) {
                    this->cards[x].type = "5";
                    this->cards[x].value = 5;
                }
                else if (x % 13 == 5) {
                    this->cards[x].type = "6";
                    this->cards[x].value = 6;
                }
                else if (x % 13 == 6) {
                    this->cards[x].type = "7";
                    this->cards[x].value = 7;
                }
                else if (x % 13 == 7) {
                    this->cards[x].type = "8";
                    this->cards[x].value = 8;
                }
                else if (x % 13 == 8) {
                    this->cards[x].type = "9";
                    this->cards[x].value = 9;
                }
                else if (x % 13 == 9) {
                    this->cards[x].type = "10";
                    this->cards[x].value = 10;
                }
                else if (x % 13 == 10) {
                    this->cards[x].type = "Jack";
                    this->cards[x].value = 10;
                }
                else if (x % 13 == 11) {
                    this->cards[x].type = "Queen";
                    this->cards[x].value = 10;
                }
                else if (x % 13 == 12) {
                    this->cards[x].type = "King";
                    this->cards[x].value = 10;
                }
            }
        }

        // Draws a random card from the Deck.
        Card drawRandomcard() {
            if (this->cards.size() == 0) {
                Card voidCard{ .suit = "",.type = "",.value = 0 };
                voidCard.suit = ":black_large_square:";
                voidCard.type = "null";
                voidCard.value = 0;
                return voidCard;
            }

            unsigned int cardIndex = (unsigned int)trunc(((float)rand() / (float)RAND_MAX) * this->cards.size());
            Card currentCard = this->cards.at(cardIndex);
            this->cards.erase(this->cards.begin() + cardIndex);
            return currentCard;
        }
    };

    struct RouletteBet {
        unsigned int betAmount = 0;
        string betOptions = "";
        string betType = "";
        unsigned int payoutAmount = 0;
        string userId = "";
        vector<string> winningNumbers;
    };

    struct Roulette {
        bool currentlySpinning = false;
        vector<RouletteBet> rouletteBets{};
    };

    struct LargestPayout {
        int amount = 0;
        string timeStamp = "";
        string userId = "";
        string userName = "";
    };

    struct CasinoStats {
        LargestPayout largestBlackjackPayout{};
        int totalBlackjackPayout = 0;
        LargestPayout largestCoinFlipPayout{};
        int totalCoinFlipPayout = 0;
        LargestPayout largestRoulettePayout{};
        int totalRoulettePayout = 0;
        LargestPayout largestSlotsPayout{};
        int totalSlotsPayout = 0;
        int totalPayout = 0;
    };

    struct InventoryItem {
        string emoji = "";
        unsigned int itemCost = 0;
        string itemName = "";
        int oppMod = 0;
        unsigned int selfMod = 0;
    };

    struct InventoryRole {
        unsigned int roleCost = 0;
        string roleId = "";
        string roleName = "";
    };

    struct GuildShop {
        vector<InventoryItem> items{};
        vector<InventoryRole> roles{};
    };

    struct Currency {
        unsigned int bank = 10000;
        unsigned int wallet = 10000;
        unsigned int timeOfLastDeposit = 0;
    };

    struct DiscordGuildData {
        string guildId = "";
        string guildName = "";
        unsigned int memberCount = 0;
        vector<Card> blackjackStack{};
        string  borderColor = "FEFEFE";
        CasinoStats casinoStats{};
        vector<string> gameChannelIds{};
        GuildShop guildShop{};
        Roulette rouletteGame{};
    };

    struct DiscordGuildMemberData {
        string guildMemberMention = "";
        string guildMemberId = "";
        string displayName = "";
        string guildId = "";
        string globalId = "";
        string userName = "";
        Currency currency{};
        vector<InventoryItem> items{};
        vector<InventoryRole> roles{};
        unsigned int lastTimeRobbed = 0;
        unsigned int lastTimeWorked = 0;
    };

    enum class DatabaseWorkloadType {
        DISCORD_USER_WRITE = 0,
        DISCORD_USER_READ = 1,
        DISCORD_GUILD_WRITE = 2,
        DISCORD_GUILD_READ = 3,
        DISCORD_GUILD_MEMBER_WRITE = 4,
        DISCORD_GUILD_MEMBER_READ = 5
    };

    struct DatabaseWorkload {
        DiscordGuildData guildData;
        DiscordUserData userData;
        DiscordGuildMemberData guildMemberData;
        string globalId = "";
        string guildId = "";
        string guildMemberId = "";
        DatabaseWorkloadType workloadType;
    };

    class DatabaseManagerAgent : public agent {
    protected:
        friend class DiscordCoreClient;
        friend class DiscordUser;
        friend class DiscordGuild;
        friend class DiscordGuildMember;
        static string botUserId;
        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static mongocxx::instance* instance;
        static mongocxx::collection collection;
        static mongocxx::database dataBase;
        static mongocxx::client client;
        static unsigned int groupId;
        unbounded_buffer<DatabaseWorkload> requestBuffer;
        unbounded_buffer<DiscordUserData>discordUserOutputBuffer;
        unbounded_buffer<DiscordGuildData>discordGuildOutputBuffer;
        unbounded_buffer<DiscordGuildMemberData>discordGuildMemberOutputBuffer;
        unbounded_buffer<exception>errorBuffer;

        DatabaseManagerAgent()
            : agent(*DatabaseManagerAgent::threadContext->scheduler) {
            this->botUserId = DatabaseManagerAgent::botUserId;
        }

        ~DatabaseManagerAgent() {
        }

        static void initialize(string botUserIdNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            DatabaseManagerAgent::botUserId = botUserIdNew;
            DatabaseManagerAgent::threadContext = threadContextNew;
            DatabaseManagerAgent::instance = new mongocxx::instance();
            DatabaseManagerAgent::client = mongocxx::client{ mongocxx::uri{} };
            DatabaseManagerAgent::dataBase = DatabaseManagerAgent::client[DatabaseManagerAgent::botUserId];
            DatabaseManagerAgent::collection = DatabaseManagerAgent::dataBase[DatabaseManagerAgent::botUserId];
        }

        static void cleanup() {
            DatabaseManagerAgent::threadContext->releaseGroup();
        }

        bool getError(exception& error) {
            if (try_receive(errorBuffer, error)) {
                return true;
            }
            return false;
        }

        static bsoncxx::builder::basic::document convertUserDataToDBDoc(DiscordUserData discordUserData) {
            bsoncxx::builder::basic::document buildDoc;
            try {
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("_id", discordUserData.userId));
                buildDoc.append(kvp("userId", discordUserData.userId));
                buildDoc.append(kvp("userName", discordUserData.userName));
                buildDoc.append(kvp("guildCount", bsoncxx::types::b_int32(discordUserData.guildCount)));
                buildDoc.append(kvp("currencyName", discordUserData.currencyName));
                buildDoc.append(kvp("prefix", discordUserData.prefix));
                buildDoc.append(kvp("hoursOfDepositCooldown", bsoncxx::types::b_int32(discordUserData.hoursOfDepositCooldown)));
                buildDoc.append(kvp("hoursOfDrugSaleCooldown", bsoncxx::types::b_int32(discordUserData.hoursOfDrugSaleCooldown)));
                buildDoc.append(kvp("hoursOfRobberyCooldown", bsoncxx::types::b_double(discordUserData.hoursOfRobberyCooldown)));
                buildDoc.append(kvp("botCommanders", [discordUserData](bsoncxx::builder::basic::sub_array subArray) {
                    for (auto& value : discordUserData.botCommanders) {
                        subArray.append(value);
                    }
                    }));
                return buildDoc;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManagerAgent::convertUserDataToDBDoc() Error: " << e.what() << endl;
                return buildDoc;
            }

        }

        static DiscordUserData parseUserData(bsoncxx::document::value docValue) {
            DiscordUserData userData;
            try {
                userData.userName = docValue.view()["userName"].get_utf8().value.to_string();
                userData.currencyName = docValue.view()["currencyName"].get_utf8().value.to_string();
                userData.guildCount = docValue.view()["guildCount"].get_int32();
                userData.hoursOfDepositCooldown = docValue.view()["hoursOfDepositCooldown"].get_int32();
                userData.hoursOfDrugSaleCooldown = docValue.view()["hoursOfDrugSaleCooldown"].get_int32();
                userData.hoursOfRobberyCooldown = (float)docValue.view()["hoursOfRobberyCooldown"].get_double();
                userData.prefix = docValue.view()["prefix"].get_utf8().value.to_string();
                userData.userId = docValue.view()["userId"].get_utf8().value.to_string();
                auto botCommandersArray = docValue.view()["botCommanders"].get_array();
                vector<string> newVector;
                for (const auto& value : botCommandersArray.value) {
                    newVector.push_back(value.get_utf8().value.to_string());
                }
                userData.botCommanders = newVector;
                return userData;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManagerAgent::parseUserData() Error: " << e.what() << endl;
                return userData;
            }
        }

        static bsoncxx::builder::basic::document convertGuildDataToDBDoc(DiscordGuildData discordGuildData) {
            bsoncxx::builder::basic::document buildDoc;
            try {
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("_id", discordGuildData.guildId));
                buildDoc.append(kvp("guildId", discordGuildData.guildId));
                buildDoc.append(kvp("guildName", discordGuildData.guildName));
                buildDoc.append(kvp("memberCount", bsoncxx::types::b_int32(discordGuildData.memberCount)));
                buildDoc.append(kvp("blackjackStack",
                    [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                        for (auto& value : discordGuildData.blackjackStack) {
                            subArray.append([value](bsoncxx::builder::basic::sub_document subDocument) {
                                subDocument.append(kvp("suit", value.suit),
                                    kvp("type", value.type),
                                    kvp("value", bsoncxx::types::b_int32(value.value)));
                                });
                        }
                    }));
                buildDoc.append(kvp("borderColor", discordGuildData.borderColor));
                buildDoc.append(kvp("gameChannelIds", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                    for (auto& value : discordGuildData.gameChannelIds) {
                        subArray.append(value);
                    }
                    }));
                buildDoc.append(kvp("guildShop", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
                    subDocument.append(kvp("items", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                        for (const auto& value : discordGuildData.guildShop.items) {
                            subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
                                subDocument2.append(kvp("itemName", value.itemName),
                                    kvp("itemCost", bsoncxx::types::b_int32(value.itemCost)),
                                    kvp("selfMod", bsoncxx::types::b_int32(value.selfMod)),
                                    kvp("oppMod", bsoncxx::types::b_int32(value.oppMod)),
                                    kvp("emoji", value.emoji));
                                });
                        }
                        })),
                        subDocument.append(kvp("roles", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                            for (const auto& value : discordGuildData.guildShop.roles) {
                                subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
                                    subDocument2.append(kvp("roleName", value.roleName));
                                    subDocument2.append(kvp("roleId", value.roleId));
                                    subDocument2.append(kvp("roleCost", bsoncxx::types::b_int32(value.roleCost)));
                                    });
                            }
                            }));
                    }));
                buildDoc.append(kvp("casinoStats", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
                    subDocument.append(kvp("largestBlackjackPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
                        subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestBlackjackPayout.amount)));
                        subDoc2.append(kvp("userId", discordGuildData.casinoStats.largestBlackjackPayout.userId));
                        subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestBlackjackPayout.timeStamp));
                        subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestBlackjackPayout.userName));
                        }));
                    subDocument.append(kvp("totalBlackjackPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalBlackjackPayout)));
                    subDocument.append(kvp("largestCoinFlipPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
                        subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestCoinFlipPayout.amount)));
                        subDoc2.append(kvp("userId", discordGuildData.casinoStats.largestCoinFlipPayout.userId));
                        subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestCoinFlipPayout.timeStamp));
                        subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestCoinFlipPayout.userName));
                        }));
                    subDocument.append(kvp("totalCoinFlipPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalCoinFlipPayout)));
                    subDocument.append(kvp("largestRoulettePayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
                        subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestRoulettePayout.amount)));
                        subDoc2.append(kvp("userId", discordGuildData.casinoStats.largestRoulettePayout.userId));
                        subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestRoulettePayout.timeStamp));
                        subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestRoulettePayout.userName));
                        }));
                    subDocument.append(kvp("totalRoulettePayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalRoulettePayout)));
                    subDocument.append(kvp("largestSlotsPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
                        subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestSlotsPayout.amount)));
                        subDoc2.append(kvp("userId", discordGuildData.casinoStats.largestSlotsPayout.userId));
                        subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestSlotsPayout.timeStamp));
                        subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestSlotsPayout.userName));
                        }));
                    subDocument.append(kvp("totalSlotsPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalSlotsPayout)));
                    subDocument.append(kvp("totalPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalPayout)));

                    }));
                buildDoc.append(kvp("rouletteGame", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
                    subDocument.append(kvp("rouletteBets", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                        for (auto& value : discordGuildData.rouletteGame.rouletteBets) {
                            subArray.append([value](bsoncxx::builder::basic::sub_document subDoc2) {
                                subDoc2.append(kvp("betAmount", bsoncxx::types::b_int32(value.betAmount)));
                                subDoc2.append(kvp("betOptions", value.betOptions));
                                subDoc2.append(kvp("betType", value.betType));
                                subDoc2.append(kvp("userId", value.userId));
                                subDoc2.append(kvp("payoutAmount", bsoncxx::types::b_int32(value.payoutAmount)));
                                subDoc2.append(kvp("winningNumbers", [value](bsoncxx::builder::basic::sub_array subArray2) {
                                    for (auto& value2 : value.winningNumbers) {
                                        subArray2.append(value2);
                                    }
                                    }));
                                });
                        }
                        }), kvp("currentlySpinning", bsoncxx::types::b_bool(discordGuildData.rouletteGame.currentlySpinning)));

                    }));
                return buildDoc;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManagerAgent::convertGuildDataToDBDoc() Error: " << e.what() << endl;
                return buildDoc;
            }
        };

        DiscordGuildData parseGuildData(bsoncxx::document::value docValue) {
            DiscordGuildData guildData;
            try {
                guildData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                guildData.guildName = docValue.view()["guildName"].get_utf8().value.to_string();
                guildData.memberCount = docValue.view()["memberCount"].get_int32().value;
                for (auto& value : docValue.view()["blackjackStack"].get_array().value) {
                    Card blackjackCard;
                    blackjackCard.suit = value.get_document().view()["suit"].get_utf8().value.to_string();
                    blackjackCard.type = value.get_document().view()["type"].get_utf8().value.to_string();
                    blackjackCard.value = value.get_document().view()["value"].get_int32().value;
                    guildData.blackjackStack.push_back(blackjackCard);
                }
                guildData.borderColor = docValue.view()["borderColor"].get_utf8().value.to_string();
                for (auto& value : docValue.view()["gameChannelIds"].get_array().value) {
                    guildData.gameChannelIds.push_back(value.get_utf8().value.to_string());
                }
                for (auto& value : docValue.view()["guildShop"].get_document().value["items"].get_array().value) {
                    InventoryItem item;
                    item.emoji = value["emoji"].get_utf8().value.to_string();
                    item.itemCost = value["itemCost"].get_int32().value;
                    item.itemName = value["itemName"].get_utf8().value.to_string();
                    item.oppMod = value["oppMod"].get_int32().value;
                    item.selfMod = value["selfMod"].get_int32().value;
                    guildData.guildShop.items.push_back(item);
                }
                for (auto& value : docValue.view()["guildShop"].get_document().value["roles"].get_array().value) {
                    InventoryRole role;
                    role.roleCost = value["roleCost"].get_int32().value;
                    role.roleId = value["roleId"].get_utf8().value.to_string();
                    role.roleName = value["roleName"].get_utf8().value.to_string();
                    guildData.guildShop.roles.push_back(role);
                }
                guildData.casinoStats.largestBlackjackPayout.amount = docValue.view()["casinoStats"].get_document().value["largestBlackjackPayout"].get_document().value["amount"].get_int32().value;
                guildData.casinoStats.largestBlackjackPayout.timeStamp = docValue.view()["casinoStats"].get_document().value["largestBlackjackPayout"].get_document().value["timeStamp"].get_utf8().value.to_string();
                guildData.casinoStats.largestBlackjackPayout.userId = docValue.view()["casinoStats"].get_document().value["largestBlackjackPayout"].get_document().value["userId"].get_utf8().value.to_string();
                guildData.casinoStats.largestBlackjackPayout.userName = docValue.view()["casinoStats"].get_document().value["largestBlackjackPayout"].get_document().value["userName"].get_utf8().value.to_string();
                guildData.casinoStats.totalBlackjackPayout = docValue.view()["casinoStats"].get_document().value["totalBlackjackPayout"].get_int32().value;
                guildData.casinoStats.largestCoinFlipPayout.amount = docValue.view()["casinoStats"].get_document().value["largestCoinFlipPayout"].get_document().value["amount"].get_int32().value;
                guildData.casinoStats.largestCoinFlipPayout.timeStamp = docValue.view()["casinoStats"].get_document().value["largestCoinFlipPayout"].get_document().value["timeStamp"].get_utf8().value.to_string();
                guildData.casinoStats.largestCoinFlipPayout.userId = docValue.view()["casinoStats"].get_document().value["largestCoinFlipPayout"].get_document().value["userId"].get_utf8().value.to_string();
                guildData.casinoStats.largestCoinFlipPayout.userName = docValue.view()["casinoStats"].get_document().value["largestCoinFlipPayout"].get_document().value["userName"].get_utf8().value.to_string();
                guildData.casinoStats.totalCoinFlipPayout = docValue.view()["casinoStats"].get_document().value["totalCoinFlipPayout"].get_int32().value;
                guildData.casinoStats.largestRoulettePayout.amount = docValue.view()["casinoStats"].get_document().value["largestRoulettePayout"].get_document().value["amount"].get_int32().value;
                guildData.casinoStats.largestRoulettePayout.timeStamp = docValue.view()["casinoStats"].get_document().value["largestRoulettePayout"].get_document().value["timeStamp"].get_utf8().value.to_string();
                guildData.casinoStats.largestRoulettePayout.userId = docValue.view()["casinoStats"].get_document().value["largestRoulettePayout"].get_document().value["userId"].get_utf8().value.to_string();
                guildData.casinoStats.largestRoulettePayout.userName = docValue.view()["casinoStats"].get_document().value["largestRoulettePayout"].get_document().value["userName"].get_utf8().value.to_string();
                guildData.casinoStats.totalRoulettePayout = docValue.view()["casinoStats"].get_document().value["totalRoulettePayout"].get_int32().value;
                guildData.casinoStats.largestSlotsPayout.amount = docValue.view()["casinoStats"].get_document().value["largestSlotsPayout"].get_document().value["amount"].get_int32().value;
                guildData.casinoStats.largestSlotsPayout.timeStamp = docValue.view()["casinoStats"].get_document().value["largestSlotsPayout"].get_document().value["timeStamp"].get_utf8().value.to_string();
                guildData.casinoStats.largestSlotsPayout.userId = docValue.view()["casinoStats"].get_document().value["largestSlotsPayout"].get_document().value["userId"].get_utf8().value.to_string();
                guildData.casinoStats.largestSlotsPayout.userName = docValue.view()["casinoStats"].get_document().value["largestSlotsPayout"].get_document().value["userName"].get_utf8().value.to_string();
                guildData.casinoStats.totalSlotsPayout = docValue.view()["casinoStats"].get_document().value["totalSlotsPayout"].get_int32().value;
                guildData.casinoStats.totalPayout = docValue.view()["casinoStats"].get_document().value["totalPayout"].get_int32().value;
                guildData.rouletteGame.currentlySpinning = docValue.view()["rouletteGame"].get_document().value["currentlySpinning"].get_bool().value;
                for (auto& value : docValue.view()["rouletteGame"].get_document().value["rouletteBets"].get_array().value) {
                    RouletteBet rouletteBet{};
                    rouletteBet.betAmount = value.get_document().value["betAmount"].get_int32().value;
                    rouletteBet.betOptions = value.get_document().value["betOptions"].get_utf8().value.to_string();
                    rouletteBet.betType = value.get_document().value["betType"].get_utf8().value.to_string();
                    rouletteBet.payoutAmount = value.get_document().value["payoutAmount"].get_int32().value;
                    rouletteBet.userId = value.get_document().value["userId"].get_utf8().value.to_string();
                    for (auto& value2 : value.get_document().value["winningNumbers"].get_array().value) {
                        rouletteBet.winningNumbers.push_back(value2.get_utf8().value.to_string());
                    }
                    guildData.rouletteGame.rouletteBets.push_back(rouletteBet);
                }
                return guildData;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManagerAgent::parseGuildData() Error: " << e.what() << endl;
                return guildData;
            }
        };

        static bsoncxx::builder::basic::document convertGuildMemberDataToDBDoc(DiscordGuildMemberData discordGuildMemberData) {
            bsoncxx::builder::basic::document buildDoc;
            try {
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("guildMemberMention", discordGuildMemberData.guildMemberMention));
                buildDoc.append(kvp("_id", discordGuildMemberData.globalId));
                buildDoc.append(kvp("guildId", discordGuildMemberData.guildId));
                buildDoc.append(kvp("guildMemberId", discordGuildMemberData.guildMemberId));
                buildDoc.append(kvp("globalId", discordGuildMemberData.globalId));
                buildDoc.append(kvp("userName", discordGuildMemberData.userName));
                buildDoc.append(kvp("displayName", discordGuildMemberData.displayName));
                buildDoc.append(kvp("lastTimeRobbed", bsoncxx::types::b_int32(discordGuildMemberData.lastTimeRobbed)));
                buildDoc.append(kvp("lastTimeWorked", bsoncxx::types::b_int32(discordGuildMemberData.lastTimeWorked)));
                buildDoc.append(kvp("currency", [discordGuildMemberData](bsoncxx::builder::basic::sub_document subDocument) {
                    subDocument.append(kvp("bank", bsoncxx::types::b_int32(discordGuildMemberData.currency.bank)),
                        kvp("wallet", bsoncxx::types::b_int32(discordGuildMemberData.currency.wallet)),
                        kvp("timeOfLastDeposit", bsoncxx::types::b_int32(discordGuildMemberData.currency.timeOfLastDeposit)));
                    }));
                buildDoc.append(kvp("items", [discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
                    for (unsigned int x = 0; x < discordGuildMemberData.items.size(); x += 1) {
                        subArray.append([discordGuildMemberData, x](bsoncxx::builder::basic::sub_document subDocument) {
                            subDocument.append(kvp("itemName", discordGuildMemberData.items.at(x).itemName),
                                kvp("itemCost", bsoncxx::types::b_int32(discordGuildMemberData.items.at(x).itemCost)),
                                kvp("selfMod", bsoncxx::types::b_int32(discordGuildMemberData.items.at(x).selfMod)),
                                kvp("oppMod", bsoncxx::types::b_int32(discordGuildMemberData.items.at(x).oppMod)),
                                kvp("emoji", discordGuildMemberData.items.at(x).emoji));

                            });
                    }
                    })),
                    buildDoc.append(kvp("roles", [discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
                        for (unsigned int x = 0; x < discordGuildMemberData.roles.size(); x += 1) {
                            subArray.append([discordGuildMemberData, x](bsoncxx::builder::basic::sub_document subDocument) {
                                subDocument.append(kvp("roleName", discordGuildMemberData.roles.at(x).roleName),
                                    kvp("roleId", discordGuildMemberData.roles.at(x).roleId),
                                    kvp("roleCost", bsoncxx::types::b_int32(discordGuildMemberData.roles.at(x).roleCost)));
                                });
                        }
                        }));

                    return buildDoc;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManagerAgent::convertGuildDataToDBDoc() Error: " << e.what() << endl;
                return buildDoc;
            }
        };

        DiscordGuildMemberData parseGuildMemberData(bsoncxx::document::value docValue) {
            DiscordGuildMemberData guildMemberData;
            try {
                guildMemberData.guildMemberMention = docValue.view()["guildMemberMention"].get_utf8().value.to_string();
                guildMemberData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                guildMemberData.displayName = docValue.view()["displayName"].get_utf8().value.to_string();
                guildMemberData.globalId = docValue.view()["globalId"].get_utf8().value.to_string();
                guildMemberData.guildMemberId = docValue.view()["guildMemberId"].get_utf8().value.to_string();
                guildMemberData.userName = docValue.view()["userName"].get_utf8().value.to_string();
                guildMemberData.lastTimeWorked = docValue.view()["lastTimeWorked"].get_int32().value;
                guildMemberData.lastTimeRobbed = docValue.view()["lastTimeRobbed"].get_int32().value;
                guildMemberData.currency.bank = docValue.view()["currency"].get_document().value["bank"].get_int32().value;
                guildMemberData.currency.wallet = docValue.view()["currency"].get_document().value["wallet"].get_int32().value;
                guildMemberData.currency.timeOfLastDeposit = docValue.view()["currency"].get_document().value["timeOfLastDeposit"].get_int32().value;
                for (auto& value : docValue.view()["items"].get_array().value) {
                    InventoryItem item;
                    item.emoji = value["emoji"].get_utf8().value.to_string();
                    item.itemName = value["itemName"].get_utf8().value.to_string();
                    item.itemCost = value["itemCost"].get_int32().value;
                    item.oppMod = value["oppMod"].get_int32().value;
                    item.selfMod = value["selfMod"].get_int32().value;
                    guildMemberData.items.push_back(item);
                }
                for (auto& value : docValue.view()["roles"].get_array().value) {
                    InventoryRole role;
                    role.roleCost = value["roleCost"].get_int32().value;
                    role.roleId = value["roleId"].get_utf8().value.to_string();
                    role.roleName = value["roleName"].get_utf8().value.to_string();
                    guildMemberData.roles.push_back(role);
                }
                return guildMemberData;
            }
            catch (bsoncxx::v_noabi::exception& e) {
                cout << "DatabaseManagerAgent::parseGuildData() Error: " << e.what() << endl;
                return guildMemberData;
            }
        };

        void run() {
            try {
                DatabaseWorkload workload;
                if (try_receive(this->requestBuffer, workload)) {
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_USER_WRITE) {
                        bsoncxx::builder::basic::document doc = DatabaseManagerAgent::convertUserDataToDBDoc(workload.userData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.userData.userId << finalize, doc.view(), options);
                        if (result.get_ptr() != NULL) {
                            //cout << "USER WRITE 01: " << bsoncxx::to_json(result.get().view()) << endl << endl;
                        }
                        if (result.get_ptr() == NULL) {
                            //cout << "USER WRITE 02: " << bsoncxx::to_json(doc.view()) << endl << endl;
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_USER_READ) {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.userData.userId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordUserData userData = DatabaseManagerAgent::parseUserData(result.get());
                            send(this->discordUserOutputBuffer, userData);
                        }
                        else {
                            DiscordUserData userData;
                            send(this->discordUserOutputBuffer, userData);
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_WRITE) {
                        bsoncxx::builder::basic::document doc = DatabaseManagerAgent::convertGuildDataToDBDoc(workload.guildData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.guildData.guildId << finalize, doc.view(), options);
                        if (result.get_ptr() != NULL) {
                            //cout << "GUILD WRITE 01: " << bsoncxx::to_json(result.get().view()) << endl << endl;
                        }
                        if (result.get_ptr() == NULL) {
                            //cout << "GUILD WRITE 02: " << bsoncxx::to_json(doc.view()) << endl << endl;
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_READ) {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.guildData.guildId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildData guildData = DatabaseManagerAgent::parseGuildData(result.get());
                            send(this->discordGuildOutputBuffer, guildData);
                        }
                        else {
                            DiscordGuildData guildData;
                            send(this->discordGuildOutputBuffer, guildData);
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE) {
                        bsoncxx::builder::basic::document doc = DatabaseManagerAgent::convertGuildMemberDataToDBDoc(workload.guildMemberData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.guildMemberData.globalId << finalize, doc.view(), options);
                        if (result.get_ptr() != NULL) {
                            //cout << "GUILDMEMBER WRITE 01: " << bsoncxx::to_json(result.get().view()) << endl << endl;
                        }
                        if (result.get_ptr() == NULL) {
                            //cout << "GUILDMEMBER WRITE 02: " << bsoncxx::to_json(doc.view()) << endl << endl;
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                        }
                    }
                    if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ) {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.guildMemberData.globalId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildMemberData guildMemberData = DatabaseManagerAgent::parseGuildMemberData(result.get());
                            send(this->discordGuildMemberOutputBuffer, guildMemberData);
                        }
                        else {
                            DiscordGuildMemberData guildMemberData;
                            send(this->discordGuildMemberOutputBuffer, guildMemberData);
                        }
                    }
                }
            }
            catch (const exception& e) {
                send(errorBuffer, e);
            }
            done();
        }
    };

    class DiscordUser {
    public:
        DiscordUserData data;
        DiscordUser(string userNameNew, string userIdNew) {
            this->data.userId = userIdNew;
            this->getDataFromDB();
            this->data.guildCount = 0;
            this->data.userName = userNameNew;
        }

        void writeDataToDB() {
            DatabaseManagerAgent databaseManager;
            DatabaseWorkload workload;
            workload.workloadType = DatabaseWorkloadType::DISCORD_USER_WRITE;
            workload.userData = this->data;
            send(databaseManager.requestBuffer, workload);
            databaseManager.start();
            agent::wait(&databaseManager);
            exception error;
            while (databaseManager.getError(error)) {
                cout << "DiscordUser::writeDataToDB() Error: " << error.what() << endl << endl;
            }
            return;
        }

        void getDataFromDB() {
            DatabaseManagerAgent databaseManager;
            DatabaseWorkload workload;
            workload.workloadType = DatabaseWorkloadType::DISCORD_USER_READ;
            workload.userData = this->data;
            send(databaseManager.requestBuffer, workload);
            databaseManager.start();
            agent::wait(&databaseManager);
            exception error;
            while (databaseManager.getError(error)) {
                cout << "DiscordUser::getDataFromDB() Error: " << error.what() << endl << endl;
            }
            DiscordUserData userData;
            try_receive(databaseManager.discordUserOutputBuffer, userData);
            if (userData.userId != "") {
                this->data = userData;
            }
            return;
        }

    };

    class DiscordGuild {
    public:
        DiscordGuildData data;
        DiscordGuild(GuildData guildData) {
            this->data.guildId = guildData.id;
            this->getDataFromDB();
            this->data.guildName = guildData.name;
            this->data.memberCount = guildData.memberCount;
        }

        void writeDataToDB() {
            DatabaseManagerAgent databaseManager;
            DatabaseWorkload workload;
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_WRITE;
            workload.guildData = this->data;
            send(databaseManager.requestBuffer, workload);
            databaseManager.start();
            agent::wait(&databaseManager);
            exception error;
            while (databaseManager.getError(error)) {
                cout << "DiscordGuild::writeDataToDB() Error: " << error.what() << endl << endl;
            }
            return;
        }

        void getDataFromDB() {
            DatabaseManagerAgent databaseManager;
            DatabaseWorkload workload;
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_READ;
            workload.guildData = this->data;
            send(databaseManager.requestBuffer, workload);
            databaseManager.start();
            agent::wait(&databaseManager);
            exception error;
            while (databaseManager.getError(error)) {
                cout << "DiscordGuild::getDataFromDB() Error: " << error.what() << endl << endl;
            }
            DiscordGuildData guildData;
            try_receive(databaseManager.discordGuildOutputBuffer, guildData);
            if (guildData.guildId != "") {
                this->data = guildData;
            }
            return;

        }
    };

    class DiscordGuildMember {
    public:
        DiscordGuildMemberData data;
        DiscordGuildMember(DiscordCoreInternal::GuildMemberData guildMemberData) {
            this->data.guildMemberId = guildMemberData.user.id;
            this->data.guildId = guildMemberData.guildId;
            this->data.globalId = this->data.guildId + " + " + this->data.guildMemberId;
            this->getDataFromDB();
            if (guildMemberData.nick == "") {
                this->data.displayName = guildMemberData.user.username;
            }
            else {
                this->data.displayName = guildMemberData.nick;
            }
            this->data.userName = guildMemberData.user.username;
            this->data.guildMemberMention = guildMemberData.userMention;
        }

        void writeDataToDB() {
            DatabaseManagerAgent databaseManager;
            DatabaseWorkload workload;
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE;
            workload.guildMemberData = this->data;
            send(databaseManager.requestBuffer, workload);
            databaseManager.start();
            agent::wait(&databaseManager);
            exception error;
            while (databaseManager.getError(error)) {
                cout << "DiscordGuildMember::writeDataToDB() Error: " << error.what() << endl << endl;
            }
            return;
        }

        void getDataFromDB() {
            DatabaseManagerAgent databaseManager;
            DatabaseWorkload workload;
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ;
            workload.guildMemberData = this->data;
            send(databaseManager.requestBuffer, workload);
            databaseManager.start();
            agent::wait(&databaseManager);
            exception error;
            while (databaseManager.getError(error)) {
                cout << "DiscordGuildMember::getDataFromDB() Error: " << error.what() << endl << endl;
            }
            DiscordGuildMemberData guildMemberData;
            try_receive(databaseManager.discordGuildMemberOutputBuffer, guildMemberData);
            if (guildMemberData.globalId != "") {
                this->data = guildMemberData;
            }
            return;
        }
    };
    string DatabaseManagerAgent::botUserId;
    mongocxx::instance* DatabaseManagerAgent::instance;
    mongocxx::collection DatabaseManagerAgent::collection;
    mongocxx::database DatabaseManagerAgent::dataBase;
    mongocxx::client DatabaseManagerAgent::client;
    shared_ptr<DiscordCoreInternal::ThreadContext> DatabaseManagerAgent::threadContext;
    unsigned int DatabaseManagerAgent::groupId;
};
#endif
