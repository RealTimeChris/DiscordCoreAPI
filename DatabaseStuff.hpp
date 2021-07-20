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
        int32_t guildCount = 0;
        string prefix = "!";
        string userId = "";
        string userName = "";
    };

    struct DiscordGuildData {
        string guildId = "";
        string guildName = "";
        unsigned int memberCount = 0;
        string  borderColor = "FEFEFE";
        vector<string> musicChannelIds{};
    };

    struct DiscordGuildMemberData {
        string guildMemberMention = "";
        string guildMemberId = "";
        string displayName = "";
        string guildId = "";
        string globalId = "";
        string userName = "";
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
            this->groupId = DatabaseManagerAgent::threadContext->createGroup();
        }

        ~DatabaseManagerAgent() {
            DatabaseManagerAgent::threadContext->releaseGroup(this->groupId);
        }

        static void initialize(string botUserIdNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            DatabaseManagerAgent::botUserId = botUserIdNew;
            DatabaseManagerAgent::threadContext = threadContextNew;
            DatabaseManagerAgent::instance = new mongocxx::instance();
            DatabaseManagerAgent::client = mongocxx::client{ mongocxx::uri{} };
            DatabaseManagerAgent::dataBase = DatabaseManagerAgent::client[DatabaseManagerAgent::botUserId];
            DatabaseManagerAgent::collection = DatabaseManagerAgent::dataBase[DatabaseManagerAgent::botUserId];
            DatabaseManagerAgent::groupId = DatabaseManagerAgent::threadContext->createGroup();
        }

        static void cleanup() {
            DatabaseManagerAgent::threadContext->releaseGroup(DatabaseManagerAgent::groupId);
        }

        bool getError(exception& error) {
            return try_receive(errorBuffer, error);
        }

        static bsoncxx::builder::basic::document convertUserDataToDBDoc(DiscordUserData discordUserData) {
            bsoncxx::builder::basic::document buildDoc;
            try {
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("_id", discordUserData.userId));
                buildDoc.append(kvp("userId", discordUserData.userId));
                buildDoc.append(kvp("userName", discordUserData.userName));
                buildDoc.append(kvp("guildCount", bsoncxx::types::b_int32(discordUserData.guildCount)));
                buildDoc.append(kvp("prefix", discordUserData.prefix));
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
                userData.guildCount = docValue.view()["guildCount"].get_int32();
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
                buildDoc.append(kvp("borderColor", discordGuildData.borderColor));
                buildDoc.append(kvp("musicChannelIds", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
                    for (auto& value : discordGuildData.musicChannelIds) {
                        subArray.append(value);
                    }
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
                guildData.borderColor = docValue.view()["borderColor"].get_utf8().value.to_string();
                for (auto& value : docValue.view()["musicChannelIds"].get_array().value) {
                    guildData.musicChannelIds.push_back(value.get_utf8().value.to_string());

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

