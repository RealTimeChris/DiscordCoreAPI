// DatabaseEntities.hpp - Database stuff.
// May 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DATABASE_ENTITIES_
#define _DATABASE_ENTITIES_

#include "../pch.h"
#include "GuildEntities.hpp"
#include "GuildMemberEntities.hpp"

namespace DiscordCoreAPI {

    struct DiscordUserData {
        vector<string> botCommanders{ "", "", "" };
        bool didWeShutDownProperly{ false };
        int32_t guildCount{ 0 };
        string userName{ "" };
        string prefix{ "!" };
        string userId{ "" };
    };

    struct DiscordGuildData {
        vector<string> musicChannelIds{};
        string  borderColor{ "FEFEFE" };
        unsigned int memberCount{ 0 };
        string guildName{ "" };
        DBPlaylist playlist{};
        string djRoleId{ "" };
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

        static bsoncxx::builder::basic::document convertUserDataToDBDoc(DiscordUserData discordUserData) {
            bsoncxx::builder::basic::document buildDoc;
            try {
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("_id", discordUserData.userId));
                buildDoc.append(kvp("userId", discordUserData.userId));
                buildDoc.append(kvp("userName", discordUserData.userName));
                buildDoc.append(kvp("guildCount", bsoncxx::types::b_int32(discordUserData.guildCount)));
                buildDoc.append(kvp("didWeShutDownProperly", bsoncxx::types::b_bool(discordUserData.didWeShutDownProperly)));
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
                userData.didWeShutDownProperly = docValue.view()["didWeShutDownProperly"].get_bool().value;
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
                buildDoc.append(kvp("playlist", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument01) {
                    subDocument01.append(kvp("isLoopAllEnabled", bsoncxx::types::b_bool(discordGuildData.playlist.isLoopAllEnabled)));
                    subDocument01.append(kvp("isLoopSongEnabled", bsoncxx::types::b_bool(discordGuildData.playlist.isLoopSongEnabled)));
                    subDocument01.append(kvp("currentSong", [&](bsoncxx::builder::basic::sub_document subDocument02) {
                        subDocument02.append(kvp("downloadURLs", [discordGuildData](bsoncxx::builder::basic::sub_array subArray01) {
                            for (auto value : discordGuildData.playlist.currentSong.finalDownloadURLs) {
                                subArray01.append([&](bsoncxx::builder::basic::sub_document subDocument03) {
                                    subDocument03.append(kvp("contentSize", bsoncxx::types::b_int32(value.contentSize)));
                                    subDocument03.append(kvp("urlPath", value.urlPath)); });
                            }; }));

                        subDocument02.append(kvp("addedByUserId", discordGuildData.playlist.currentSong.addedByUserId), kvp("addedByUserName", discordGuildData.playlist.currentSong.addedByUserName),
                            kvp("contentLength", bsoncxx::types::b_int32(discordGuildData.playlist.currentSong.contentLength)), kvp("description", discordGuildData.playlist.currentSong.description), kvp("secondDownloadURL", discordGuildData.playlist.currentSong.secondDownloadURL),
                            kvp("duration", discordGuildData.playlist.currentSong.duration), kvp("songTitle", discordGuildData.playlist.currentSong.songTitle),
                            kvp("firstDownloadURL", discordGuildData.playlist.currentSong.firstDownloadURL), kvp("thumbnailURL", discordGuildData.playlist.currentSong.thumbnailURL),
                            kvp("type", bsoncxx::types::b_int32((int)discordGuildData.playlist.currentSong.type)), kvp("songId", discordGuildData.playlist.currentSong.songId), kvp("viewURL", discordGuildData.playlist.currentSong.viewURL)); }));
                    
                    subDocument01.append(kvp("songList", [discordGuildData](bsoncxx::builder::basic::sub_array subArray01) {
                        for (auto value : discordGuildData.playlist.songList) {
                            subArray01.append([&](bsoncxx::builder::basic::sub_document subDocument02) {
                                subDocument02.append(kvp("downloadURLs", [discordGuildData](bsoncxx::builder::basic::sub_array subArray02) {
                                    for (auto value02 : discordGuildData.playlist.currentSong.finalDownloadURLs) {
                                        subArray02.append([&](bsoncxx::builder::basic::sub_document subDocument03) {
                                            subDocument03.append(kvp("contentSize", bsoncxx::types::b_int32(value02.contentSize)));
                                            subDocument03.append(kvp("urlPath", value02.urlPath)); });
                                    }; }));
                                subDocument02.append(kvp("addedByUserId", value.addedByUserId), kvp("addedByUserName", value.addedByUserName), kvp("contentLength", bsoncxx::types::b_int32(value.contentLength)), kvp("description", value.description),
                                    kvp("duration", value.duration), kvp("thumbnailURL", value.thumbnailURL), kvp("songId", value.songId), kvp("type", bsoncxx::types::b_int32((int)value.type)), kvp("viewURL", value.viewURL));
                                subDocument02.append(kvp("songTitle", value.songTitle), kvp("firstDownloadURL", value.firstDownloadURL), kvp("secondDownloadURL", value.secondDownloadURL)); });
                        }
                        }));
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

                guildData.playlist.currentSong.addedByUserId = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["addedByUserId"].get_utf8().value.to_string();
                guildData.playlist.currentSong.addedByUserName = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["addedByUserName"].get_utf8().value.to_string();
                guildData.playlist.currentSong.firstDownloadURL = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["firstDownloadURL"].get_utf8().value.to_string();
                guildData.playlist.currentSong.secondDownloadURL = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["secondDownloadURL"].get_utf8().value.to_string();
                guildData.playlist.currentSong.contentLength = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["contentLength"].get_int32().value; ;
                guildData.playlist.currentSong.description = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["description"].get_utf8().value.to_string();
                guildData.playlist.currentSong.thumbnailURL = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["thumbnailURL"].get_utf8().value.to_string();
                guildData.playlist.currentSong.duration = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["duration"].get_utf8().value.to_string(); ;
                guildData.playlist.currentSong.songId = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["songId"].get_utf8().value.to_string();
                guildData.playlist.currentSong.songTitle = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["songTitle"].get_utf8().value.to_string();
                guildData.playlist.currentSong.type = (SongType)docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["type"].get_int32().value;
                guildData.playlist.currentSong.viewURL = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["viewURL"].get_utf8().value.to_string();
                guildData.playlist.isLoopSongEnabled = docValue.view()["playlist"].get_document().value["isLoopSongEnabled"].get_bool().value;
                guildData.playlist.isLoopAllEnabled = docValue.view()["playlist"].get_document().value["isLoopAllEnabled"].get_bool().value;
                
                for (auto value02 : docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["downloadURLs"].get_array().value) {
                    DownloadURL downloadURL;
                    downloadURL.contentSize = value02["contentSize"].get_int32().value;
                    downloadURL.urlPath = value02["urlPath"].get_utf8().value.to_string();
                    guildData.playlist.currentSong.finalDownloadURLs.push_back(downloadURL);
                }
                for (auto value : docValue.view()["playlist"].get_document().view()["songList"].get_array().value) {
                    Song newSong{};
                    for (auto value02 : value["downloadURLs"].get_array().value) {
                        DownloadURL downloadURL;
                        downloadURL.contentSize = value02["contentSize"].get_int32().value;
                        downloadURL.urlPath = value02["urlPath"].get_utf8().value.to_string();
                        newSong.finalDownloadURLs.push_back(downloadURL);
                    }
                    newSong.addedByUserName = value["addedByUserName"].get_utf8().value.to_string();
                    newSong.description = value["description"].get_utf8().value.to_string();
                    newSong.addedByUserId = value["addedByUserId"].get_utf8().value.to_string();
                    newSong.duration = value["duration"].get_utf8().value.to_string();
                    newSong.thumbnailURL = value["thumbnailURL"].get_utf8().value.to_string();
                    newSong.contentLength = value["contentLength"].get_int32().value;
                    newSong.type = (SongType)value["type"].get_int32().value;
                    newSong.songId = value["songId"].get_utf8().value.to_string();
                    newSong.firstDownloadURL = value["firstDownloadURL"].get_utf8().value.to_string();
                    newSong.secondDownloadURL = value["secondDownloadURL"].get_utf8().value.to_string();
                    newSong.songTitle = value["songTitle"].get_utf8().value.to_string();
                    newSong.viewURL = value["viewURL"].get_utf8().value.to_string();
                    guildData.playlist.songList.push_back(newSong);
                }
                guildData.borderColor = docValue.view()["borderColor"].get_utf8().value.to_string();
                guildData.guildName = docValue.view()["guildName"].get_utf8().value.to_string();
                guildData.djRoleId = docValue.view()["djRoleId"].get_utf8().value.to_string();
                guildData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                for (auto& value : docValue.view()["musicChannelIds"].get_array().value) {
                    guildData.musicChannelIds.push_back(value.get_utf8().value.to_string());
                }
                guildData.memberCount = docValue.view()["memberCount"].get_int32().value;
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
            catch (...) {
                rethrowException("DatabaseManagerAgent::run() Error: ");
            }
            done();
        }
    };

    class DiscordUser {
    public:

        DiscordUserData data{};

        DiscordUser& operator=(DiscordUser&) {
            this->getDataFromDB();
            this->writeDataToDB();
            return *this;
        }

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

        friend class SoundCloudAPICore;
        friend class YouTubeAPICore;
        friend class SoundCloudAPI;
        friend class SongAPICore;
        friend class YouTubeAPI;
        friend class SongAPI;

        DiscordGuildData data{};

        DiscordGuild operator=(DiscordGuild newGuild){
            this->getDataFromDB();
            this->writeDataToDB();
            return *this;
        }

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

        DiscordGuildMember operator=(DiscordGuildMember newGuildMember) {
            this->getDataFromDB();
            this->writeDataToDB();
            return *this;
        }

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

