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

    struct DiscordUserData {
        vector<string> botCommanders{ "", "", "" };
        int32_t guildCount{ 0 };
        string userName{ "" };
        string prefix{ "!" };
        string userId{ "" };
    };

    DBPlaylist::operator Playlist() {
        Playlist newData;
        newData.isLoopAllEnabled = this->isLoopAllEnabled;
        newData.isLoopSongEnabled = this->isLoopSongEnabled;
        newData.currentSong = this->currentSong;
        newData.songQueue = this->songList;
        return newData;
    }

    struct DiscordGuildData {
        vector<string> musicChannelIds{};
        string  borderColor{ "FEFEFE" };
        unsigned int memberCount{ 0 };
        string guildName{ "" };
        string djRoleId{ "" };
        DBPlaylist playlist{};
        string guildId{ "" };        
    };

    struct DiscordGuildMemberData {
        string guildMemberMention{ "" };
        string guildMemberId{ "" };
        string displayName{ "" };
        string globalId{ "" };
        string userName{ "" };
        string guildId{ "" };
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
        DiscordGuildMemberData guildMemberData{};
        DatabaseWorkloadType workloadType;
        DiscordGuildData guildData;
        DiscordUserData userData;
        string guildMemberId;
        string globalId;
        string guildId;
    };

    class DatabaseManagerAgent : agent {
    protected:

        friend class DiscordGuildMember;
        friend class DiscordCoreClient;
        friend class DiscordUser;
        friend class DiscordGuild;

        static shared_ptr<DiscordCoreInternal::ThreadContext> threadContext;
        static mongocxx::collection collection;
        static mongocxx::instance* instance;
        static mongocxx::database dataBase;
        static mongocxx::client* client;
        static string botUserId;

        unbounded_buffer<DiscordGuildMemberData> discordGuildMemberOutputBuffer{ nullptr };
        unbounded_buffer<DiscordGuildData> discordGuildOutputBuffer{ nullptr };
        unbounded_buffer<DiscordUserData>discordUserOutputBuffer{ nullptr };
        unbounded_buffer<DatabaseWorkload> requestBuffer{ nullptr };
        unbounded_buffer<exception> errorBuffer{ nullptr };

        DatabaseManagerAgent()
            : agent(*DatabaseManagerAgent::threadContext->scheduler->scheduler) {
            this->botUserId = DatabaseManagerAgent::botUserId;
        }

        static void initialize(string botUserIdNew, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) {
            DatabaseManagerAgent::botUserId = botUserIdNew;
            DatabaseManagerAgent::threadContext = threadContextNew;
            DatabaseManagerAgent::instance = new mongocxx::instance();
            DatabaseManagerAgent::client = new mongocxx::client{ mongocxx::uri{} };
            DatabaseManagerAgent::dataBase = (*DatabaseManagerAgent::client)[DatabaseManagerAgent::botUserId];
            DatabaseManagerAgent::collection = DatabaseManagerAgent::dataBase[DatabaseManagerAgent::botUserId];
        }

        static void cleanup() {
            DatabaseManagerAgent::threadContext->releaseGroup();
        }

        void getError(string stackTrace) {
            exception error;
            while (try_receive(errorBuffer, error)) {
                cout << stackTrace << "Error: " << error.what() << endl << endl;
            }
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
                buildDoc.append(kvp("djRoleId", discordGuildData.djRoleId));
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

        static DiscordGuildData parseGuildData(bsoncxx::document::value docValue) {
            DiscordGuildData guildData;
            try {
                guildData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                guildData.guildName = docValue.view()["guildName"].get_utf8().value.to_string();
                guildData.memberCount = docValue.view()["memberCount"].get_int32().value;
                guildData.borderColor = docValue.view()["borderColor"].get_utf8().value.to_string();
                guildData.djRoleId = docValue.view()["djRoleId"].get_utf8().value.to_string();
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

        static DiscordGuildMemberData parseGuildMemberData(bsoncxx::document::value docValue) {
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
                    switch (workload.workloadType) {
                    case(DatabaseWorkloadType::DISCORD_USER_WRITE): {
                        bsoncxx::builder::basic::document doc = DatabaseManagerAgent::convertUserDataToDBDoc(workload.userData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.userData.userId << finalize, doc.view(), options);
                        if (result.get_ptr() == NULL) {
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_USER_READ): {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.userData.userId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordUserData userData = DatabaseManagerAgent::parseUserData(result.get());
                            send(this->discordUserOutputBuffer, userData);
                        }
                        else {
                            DiscordUserData userData;
                            send(this->discordUserOutputBuffer, userData);
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_WRITE): {
                        bsoncxx::builder::basic::document doc = DatabaseManagerAgent::convertGuildDataToDBDoc(workload.guildData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.guildData.guildId << finalize, doc.view(), options);
                        if (result.get_ptr() == NULL) {
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_READ): {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.guildData.guildId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildData guildData = DatabaseManagerAgent::parseGuildData(result.get());
                            send(this->discordGuildOutputBuffer, guildData);
                        }
                        else {
                            DiscordGuildData guildData;
                            send(this->discordGuildOutputBuffer, guildData);
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE): {
                        bsoncxx::builder::basic::document doc = DatabaseManagerAgent::convertGuildMemberDataToDBDoc(workload.guildMemberData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.guildMemberData.globalId << finalize, doc.view(), options);
                        if (result.get_ptr() == NULL) {
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ): {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.guildMemberData.globalId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildMemberData guildMemberData = DatabaseManagerAgent::parseGuildMemberData(result.get());
                            send(this->discordGuildMemberOutputBuffer, guildMemberData);
                        }
                        else {
                            DiscordGuildMemberData guildMemberData;
                            send(this->discordGuildMemberOutputBuffer, guildMemberData);
                        }
                        break;
                    }
                    default: {
                        break;
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

        DiscordUserData data{};

        DiscordUser(string userNameNew, string userIdNew) {
            this->data.userId = userIdNew;
            this->getDataFromDB();
            this->data.guildCount = 0;
            this->data.userName = userNameNew;
        }

        void writeDataToDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_USER_WRITE;
            workload.userData = this->data;
            send(requestAgent.requestBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("DiscordUser::WriteDataToDB() ");
            return;
        }

        void getDataFromDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_USER_READ;
            workload.userData = this->data;
            send(requestAgent.requestBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("DiscordUser::getDataFromDB() ");
            DiscordUserData userData;
            try_receive(requestAgent.discordUserOutputBuffer, userData);
            if (userData.userId != "") {
                this->data = userData;
            }
            return;
        }

    };

    class DiscordGuild {
    public:

        friend class SoundCloudAPI;        
        friend class SongAPICore;
        friend class YouTubeAPI;
        friend class YouTubeAPI;
        friend class SongAPI;

        DiscordGuildData data{};

        DiscordGuild(GuildData guildData) {
            this->data.guildId = guildData.id;
            this->getDataFromDB();
            this->data.guildName = guildData.name;
            this->data.memberCount = guildData.memberCount;
        }

        void writeDataToDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_WRITE;
            workload.guildData = this->data;
            send(requestAgent.requestBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("DiscordGuild::writeDataToDB() ");
            return;
        }

        void getDataFromDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_READ;
            workload.guildData = this->data;
            send(requestAgent.requestBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("DiscordGuild::getDataFromDB() ");
            DiscordGuildData guildData;
            try_receive(requestAgent.discordGuildOutputBuffer, guildData);
            if (guildData.guildId != "") {
                this->data = guildData;
            }
            return;
        }

    protected:

        DiscordGuild() {};

    };

    class DiscordGuildMember {
    public:

        DiscordGuildMemberData data{};

        DiscordGuildMember(DiscordCoreInternal::GuildMemberData guildMemberData) {
            this->data.guildMemberId = guildMemberData.user.id;
            this->data.guildId = guildMemberData.guildId;
            this->data.globalId = this->data.guildId + " + " + this->data.guildMemberId;
            this->getDataFromDB();
            if (guildMemberData.nick == "") {
                this->data.displayName = guildMemberData.user.userName;
            }
            else {
                this->data.displayName = guildMemberData.nick;
            }
            this->data.userName = guildMemberData.user.userName;
            this->data.guildMemberMention = guildMemberData.userMention;
        }

        void writeDataToDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE;
            workload.guildMemberData = this->data;
            send(requestAgent.requestBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("DiscordGuildMember::writeDataToDB() ");
            return;
        }

        void getDataFromDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ;
            workload.guildMemberData = this->data;
            send(requestAgent.requestBuffer, workload);
            requestAgent.start();
            agent::wait(&requestAgent);
            requestAgent.getError("DiscordGuildMember::getDataFromDB() ");
            DiscordGuildMemberData guildMemberData;
            try_receive(requestAgent.discordGuildMemberOutputBuffer, guildMemberData);
            if (guildMemberData.globalId != "") {
                this->data = guildMemberData;
            }
            return;
        }
    };
    shared_ptr<DiscordCoreInternal::ThreadContext> DatabaseManagerAgent::threadContext{ nullptr };
    mongocxx::instance* DatabaseManagerAgent::instance{ nullptr };
    mongocxx::client* DatabaseManagerAgent::client{ nullptr };
    mongocxx::collection DatabaseManagerAgent::collection{};
    mongocxx::database DatabaseManagerAgent::dataBase{};
    string DatabaseManagerAgent::botUserId{ "" };
};
#endif

