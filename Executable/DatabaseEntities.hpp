// DatabaseEntities.hpp - Header for the database classes and structs.
// May 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/write_concern.hpp>
#include <bsoncxx/builder/core.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <bsoncxx/json.hpp>
#include "Index.hpp"

using namespace bsoncxx::v_noabi::builder::basic;
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::builder::basic;
using namespace std;

namespace DiscordCoreAPI {

    struct DiscordUserData {
        bool didWeShutDownProperly{ false };
        std::vector<std::string> botCommanders{};
        std::string userName{ "" };
        std::string prefix{ "!" };
        std::string userId{ "" };
    };

    struct DiscordGuildData {
        std::vector<std::string> musicChannelIds{};
        std::string  borderColor{ "FEFEFE" };
        unsigned int memberCount{ 0 };
        std::string guildName{ "" };
        std::string djRoleId{ "" };
        std::string guildId{ "" };
        Playlist playlist{};
    };

    struct DiscordGuildMemberData {
        std::string guildMemberMention{ "" };
        std::string guildMemberId{ "" };
        std::string displayName{ "" };
        std::string globalId{ "" };
        std::string userName{ "" };
        std::string guildId{ "" };
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
        DatabaseWorkloadType workloadType{};
        DiscordGuildData guildData{};
        DiscordUserData userData{};
        std::string guildMemberId{ "" };
        std::string globalId{ "" };
        std::string guildId{ "" };
    };

    struct DatabaseReturnValue {
        DiscordGuildMemberData discordGuildMember{};
        DiscordGuildData discordGuild{};
        DiscordUserData discordUser{};
    };

    class  DatabaseManagerAgent {
    public:

        DatabaseManagerAgent() {}

        static void initialize(std::string botUserIdNew) {
            DatabaseManagerAgent::botUserId = botUserIdNew;
            DatabaseManagerAgent::client = mongocxx::client(mongocxx::uri{});
            DatabaseManagerAgent::dataBase = DatabaseManagerAgent::client[DatabaseManagerAgent::botUserId];
            DatabaseManagerAgent::collection = DatabaseManagerAgent::dataBase[DatabaseManagerAgent::botUserId];
        }

        DatabaseReturnValue submitWorkloadAndGetResults(DatabaseWorkload workload) {
            this->requestBuffer.send(workload);
            this->run();
            if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ) {
                DatabaseReturnValue newData{};
                DiscordGuildMemberData newData02{};
                this->discordGuildMemberOutputBuffer.tryReceive(newData02);
                newData.discordGuildMember = newData02;
                return newData;
            }
            else if (workload.workloadType == DatabaseWorkloadType::DISCORD_GUILD_READ) {
                DatabaseReturnValue newData{};
                DiscordGuildData newData02{};
                this->discordGuildOutputBuffer.tryReceive(newData02);
                newData.discordGuild = newData02;
                return newData;
            }
            else if (workload.workloadType == DatabaseWorkloadType::DISCORD_USER_READ) {
                DatabaseReturnValue newData{};
                DiscordUserData newData02{};
                this->discordUserOutputBuffer.tryReceive(newData02);
                newData.discordUser = newData02;
                return newData;
            }
            else {
                return DatabaseReturnValue();
            }
        }

    protected:

        static mongocxx::collection collection;
        static mongocxx::database dataBase;
        static mongocxx::instance instance;
        static std::mutex workloadMutex;
        static mongocxx::client client;
        static std::string botUserId;

        UnboundedMessageBlock<DiscordGuildMemberData>discordGuildMemberOutputBuffer{};
        UnboundedMessageBlock<DiscordGuildData>discordGuildOutputBuffer{};
        UnboundedMessageBlock<DiscordUserData>discordUserOutputBuffer{};
        UnboundedMessageBlock<DatabaseWorkload> requestBuffer{};

        bsoncxx::builder::basic::document convertUserDataToDBDoc(DiscordUserData discordUserData) {
            bsoncxx::builder::basic::document buildDoc{};
            try {
                using bsoncxx::builder::basic::kvp;
                buildDoc.append(kvp("_id", discordUserData.userId));
                buildDoc.append(kvp("userId", discordUserData.userId));
                buildDoc.append(kvp("userName", discordUserData.userName));
                buildDoc.append(kvp("didWeShutDownProperly", bsoncxx::types::b_bool(discordUserData.didWeShutDownProperly)));
                buildDoc.append(kvp("prefix", discordUserData.prefix));
                buildDoc.append(kvp("botCommanders", [discordUserData](bsoncxx::builder::basic::sub_array subArray) {
                    for (auto& value : discordUserData.botCommanders) {
                        subArray.append(value);
                    }
                    }));
                return buildDoc;
            }
            catch (...) {
                reportException("DatabaseManagerAgent::convertUserDataToDBDoc()");
                return buildDoc;
            }

        }

        DiscordUserData parseUserData(bsoncxx::document::value docValue) {
            DiscordUserData userData{};
            try {
                userData.userName = docValue.view()["userName"].get_utf8().value.to_string();
                userData.prefix = docValue.view()["prefix"].get_utf8().value.to_string();
                userData.userId = docValue.view()["userId"].get_utf8().value.to_string();
                userData.didWeShutDownProperly = docValue.view()["didWeShutDownProperly"].get_bool().value;
                auto botCommandersArray = docValue.view()["botCommanders"].get_array();
                std::vector<std::string> newVector;
                for (const auto& value : botCommandersArray.value) {
                    newVector.push_back(value.get_utf8().value.to_string());
                }
                userData.botCommanders = newVector;
                return userData;
            }
            catch (...) {
                reportException("DatabaseManagerAgent::parseUserData()");
                return userData;
            }
        }

        bsoncxx::builder::basic::document convertGuildDataToDBDoc(DiscordGuildData discordGuildData) {
            bsoncxx::builder::basic::document buildDoc{};
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
                        subDocument02.append(kvp("downloadUrls", [discordGuildData](bsoncxx::builder::basic::sub_array subArray01) {
                            for (auto& value : discordGuildData.playlist.currentSong.finalDownloadUrls) {
                                subArray01.append([&](bsoncxx::builder::basic::sub_document subDocument03) {
                                    subDocument03.append(kvp("contentSize", bsoncxx::types::b_int32(value.contentSize)));
                                    subDocument03.append(kvp("UrlPath", value.urlPath)); });
                            }; }));

                        subDocument02.append(kvp("addedByUserId", discordGuildData.playlist.currentSong.addedByUserId), kvp("addedByUserName", discordGuildData.playlist.currentSong.addedByUserName),
                            kvp("contentLength", bsoncxx::types::b_int32(discordGuildData.playlist.currentSong.contentLength)), kvp("description", discordGuildData.playlist.currentSong.description), kvp("secondDownloadUrl", discordGuildData.playlist.currentSong.secondDownloadUrl),
                            kvp("duration", discordGuildData.playlist.currentSong.duration), kvp("songTitle", discordGuildData.playlist.currentSong.songTitle),
                            kvp("firstDownloadUrl", discordGuildData.playlist.currentSong.firstDownloadUrl), kvp("thumbnailUrl", discordGuildData.playlist.currentSong.thumbnailUrl),
                            kvp("type", bsoncxx::types::b_int32((int)discordGuildData.playlist.currentSong.type)), kvp("songId", discordGuildData.playlist.currentSong.songId), kvp("html5Player", discordGuildData.playlist.currentSong.html5Player), kvp("html5PlayerFile", discordGuildData.playlist.currentSong.html5PlayerFile),
                            kvp("viewUrl", discordGuildData.playlist.currentSong.viewUrl)); }));

                    subDocument01.append(kvp("songList", [discordGuildData](bsoncxx::builder::basic::sub_array subArray01) {
                        for (auto& value : discordGuildData.playlist.songQueue) {
                            subArray01.append([&](bsoncxx::builder::basic::sub_document subDocument02) {
                                subDocument02.append(kvp("downloadUrls", [discordGuildData](bsoncxx::builder::basic::sub_array subArray02) {
                                    for (auto& value02 : discordGuildData.playlist.currentSong.finalDownloadUrls) {
                                        subArray02.append([&](bsoncxx::builder::basic::sub_document subDocument03) {
                                            subDocument03.append(kvp("contentSize", bsoncxx::types::b_int32(value02.contentSize)));
                                            subDocument03.append(kvp("UrlPath", value02.urlPath)); });
                                    }; }));
                                subDocument02.append(kvp("addedByUserId", value.addedByUserId), kvp("addedByUserName", value.addedByUserName), kvp("contentLength", bsoncxx::types::b_int32(value.contentLength)), kvp("description", value.description),
                                    kvp("duration", value.duration), kvp("thumbnailUrl", value.thumbnailUrl), kvp("songId", value.songId), kvp("type", bsoncxx::types::b_int32((int)value.type)), kvp("viewUrl", value.viewUrl));
                                subDocument02.append(kvp("songTitle", value.songTitle), kvp("firstDownloadUrl", value.firstDownloadUrl), kvp("html5Player", value.html5Player), kvp("html5PlayerFile", value.html5PlayerFile), kvp("secondDownloadUrl", value.secondDownloadUrl)); });
                        }
                        }));
                    }));
                return buildDoc;
            }

            catch (...) {
                reportException("DatabaseManagerAgent::convertGuildDataToDBDoc()");
                return buildDoc;
            }
        };

        DiscordGuildData parseGuildData(bsoncxx::document::value docValue) {
            DiscordGuildData guildData{};
            try {

                guildData.playlist.currentSong.addedByUserId = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["addedByUserId"].get_utf8().value.to_string();
                guildData.playlist.currentSong.addedByUserName = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["addedByUserName"].get_utf8().value.to_string();
                guildData.playlist.currentSong.firstDownloadUrl = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["firstDownloadUrl"].get_utf8().value.to_string();
                guildData.playlist.currentSong.secondDownloadUrl = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["secondDownloadUrl"].get_utf8().value.to_string();
                guildData.playlist.currentSong.contentLength = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["contentLength"].get_int32().value; ;
                guildData.playlist.currentSong.description = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["description"].get_utf8().value.to_string();
                guildData.playlist.currentSong.thumbnailUrl = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["thumbnailUrl"].get_utf8().value.to_string();
                guildData.playlist.currentSong.duration = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["duration"].get_utf8().value.to_string(); ;
                guildData.playlist.currentSong.songId = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["songId"].get_utf8().value.to_string();
                guildData.playlist.currentSong.songTitle = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["songTitle"].get_utf8().value.to_string();
                guildData.playlist.currentSong.type = (SongType)docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["type"].get_int32().value;
                guildData.playlist.currentSong.viewUrl = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["viewUrl"].get_utf8().value.to_string();
                guildData.playlist.currentSong.html5PlayerFile = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["html5PlayerFile"].get_utf8().value.to_string();
                guildData.playlist.currentSong.html5Player = docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["html5Player"].get_utf8().value.to_string();
                guildData.playlist.isLoopSongEnabled = docValue.view()["playlist"].get_document().value["isLoopSongEnabled"].get_bool().value;
                guildData.playlist.isLoopAllEnabled = docValue.view()["playlist"].get_document().value["isLoopAllEnabled"].get_bool().value;

                for (auto& value02 : docValue.view()["playlist"].get_document().value["currentSong"].get_document().value["downloadUrls"].get_array().value) {
                    DownloadUrl downloadUrl;
                    downloadUrl.contentSize = value02["contentSize"].get_int32().value;
                    downloadUrl.urlPath = value02["UrlPath"].get_utf8().value.to_string();
                    guildData.playlist.currentSong.finalDownloadUrls.push_back(downloadUrl);
                }
                for (auto& value : docValue.view()["playlist"].get_document().view()["songList"].get_array().value) {
                    Song newSong{};
                    for (auto& value02 : value["downloadUrls"].get_array().value) {
                        DownloadUrl downloadUrl;
                        downloadUrl.contentSize = value02["contentSize"].get_int32().value;
                        downloadUrl.urlPath = value02["UrlPath"].get_utf8().value.to_string();
                        newSong.finalDownloadUrls.push_back(downloadUrl);
                    }
                    newSong.addedByUserName = value["addedByUserName"].get_utf8().value.to_string();
                    newSong.description = value["description"].get_utf8().value.to_string();
                    newSong.addedByUserId = value["addedByUserId"].get_utf8().value.to_string();
                    newSong.duration = value["duration"].get_utf8().value.to_string();
                    newSong.thumbnailUrl = value["thumbnailUrl"].get_utf8().value.to_string();
                    newSong.contentLength = value["contentLength"].get_int32().value;
                    newSong.type = (SongType)value["type"].get_int32().value;
                    newSong.songId = value["songId"].get_utf8().value.to_string();
                    newSong.firstDownloadUrl = value["firstDownloadUrl"].get_utf8().value.to_string();
                    newSong.secondDownloadUrl = value["secondDownloadUrl"].get_utf8().value.to_string();
                    newSong.songTitle = value["songTitle"].get_utf8().value.to_string();
                    newSong.viewUrl = value["viewUrl"].get_utf8().value.to_string();
                    newSong.html5PlayerFile = value["html5PlayerFile"].get_utf8().value.to_string();
                    newSong.html5Player = value["html5Player"].get_utf8().value.to_string();
                    guildData.playlist.songQueue.push_back(newSong);
                }
                guildData.djRoleId = docValue.view()["djRoleId"].get_utf8().value.to_string();
                guildData.borderColor = docValue.view()["borderColor"].get_utf8().value.to_string();
                guildData.guildName = docValue.view()["guildName"].get_utf8().value.to_string();
                guildData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                for (auto& value : docValue.view()["musicChannelIds"].get_array().value) {
                    guildData.musicChannelIds.push_back(value.get_utf8().value.to_string());
                }
                guildData.memberCount = docValue.view()["memberCount"].get_int32().value;
                return guildData;
            }
            catch (...) {
                reportException("DatabaseManagerAgent::parseGuildData()");
                return guildData;
            }
        };

        bsoncxx::builder::basic::document convertGuildMemberDataToDBDoc(DiscordGuildMemberData discordGuildMemberData) {
            bsoncxx::builder::basic::document buildDoc{};
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
            catch (...) {
                reportException("DatabaseManagerAgent::convertGuildDataToDBDoc()");
                return buildDoc;
            }
        };

        DiscordGuildMemberData parseGuildMemberData(bsoncxx::document::value docValue) {
            DiscordGuildMemberData guildMemberData{};
            try {
                guildMemberData.guildMemberMention = docValue.view()["guildMemberMention"].get_utf8().value.to_string();
                guildMemberData.guildId = docValue.view()["guildId"].get_utf8().value.to_string();
                guildMemberData.displayName = docValue.view()["displayName"].get_utf8().value.to_string();
                guildMemberData.globalId = docValue.view()["globalId"].get_utf8().value.to_string();
                guildMemberData.guildMemberId = docValue.view()["guildMemberId"].get_utf8().value.to_string();
                guildMemberData.userName = docValue.view()["userName"].get_utf8().value.to_string();
                return guildMemberData;
            }
            catch (...) {
                reportException("DatabaseManagerAgent::parseGuildData()");
                return guildMemberData;
            }
        };

        void run() {
            std::lock_guard<std::mutex> workloadLock{ DatabaseManagerAgent::workloadMutex };
            try {
                DatabaseWorkload workload;
                if (this->requestBuffer.tryReceive(workload)) {
                    switch (workload.workloadType) {
                    case(DatabaseWorkloadType::DISCORD_USER_WRITE): {
                        bsoncxx::builder::basic::document doc = this->convertUserDataToDBDoc(workload.userData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.userData.userId << finalize, doc.view(), options);
                        if (result.get_ptr() == NULL) {
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                            return;
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_USER_READ): {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.userData.userId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordUserData userData = this->parseUserData(result.get());
                            this->discordUserOutputBuffer.send(userData);
                            return;
                        }
                        else {
                            DiscordUserData userData;
                            this->discordUserOutputBuffer.send(userData);
                            return;
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_WRITE): {
                        bsoncxx::builder::basic::document doc = this->convertGuildDataToDBDoc(workload.guildData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.guildData.guildId << finalize, doc.view(), options);
                        if (result.get_ptr() == NULL) {
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                            return;
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_READ): {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.guildData.guildId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildData guildData = this->parseGuildData(result.get());
                            this->discordGuildOutputBuffer.send(guildData);
                            return;
                        }
                        else {
                            DiscordGuildData guildData;
                            this->discordGuildOutputBuffer.send(guildData);
                            return;
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE): {
                        bsoncxx::builder::basic::document doc = this->convertGuildMemberDataToDBDoc(workload.guildMemberData);
                        mongocxx::v_noabi::options::find_one_and_update options;
                        options.return_document(mongocxx::v_noabi::options::return_document::k_after);
                        auto result = DatabaseManagerAgent::collection.find_one_and_update(bsoncxx::builder::stream::document{} << "_id" << workload.guildMemberData.globalId << finalize, doc.view(), options);
                        if (result.get_ptr() == NULL) {
                            DatabaseManagerAgent::collection.insert_one(doc.view());
                            return;
                        }
                        break;
                    }
                    case(DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ): {
                        auto result = DatabaseManagerAgent::collection.find_one(bsoncxx::builder::stream::document{} << "_id" << workload.guildMemberData.globalId << finalize);
                        if (result.get_ptr() != NULL) {
                            DiscordGuildMemberData guildMemberData = this->parseGuildMemberData(result.get());
                            this->discordGuildMemberOutputBuffer.send(guildMemberData);
                            return;
                        }
                        else {
                            DiscordGuildMemberData guildMemberData;
                            this->discordGuildMemberOutputBuffer.send(guildMemberData);
                            return;
                        }
                        break;
                    }
                    }
                }
            }
            catch (...) {
                reportException("DatabaseManagerAgent::run() Error: ");
            }
            return;
        }
    };
 
    class DiscordUser {
    public:

        friend struct DatabaseReturnValue;

        static int guildCount;

        DiscordUserData data{};

        DiscordUser() {};

        DiscordUser(std::string userNameNew, std::string userIdNew) {
            this->data.userId = userIdNew;
            this->getDataFromDB();
            this->data.userName = userNameNew;
            this->writeDataToDB();
        }

        void writeDataToDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_USER_WRITE;
            workload.userData = this->data;
            auto result = requestAgent.submitWorkloadAndGetResults(workload);
        }

        void getDataFromDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_USER_READ;
            workload.userData = this->data;
            auto result = requestAgent.submitWorkloadAndGetResults(workload);
            if (result.discordUser.userId != "") {
                this->data = result.discordUser;
            }
        }
    };

    class DiscordGuild {
    public:

        friend struct DatabaseReturnValue;
        friend class SoundCloudAPI;
        friend class SongAPICore;
        friend class YouTubeAPI;
        friend class YouTubeAPI;
        friend class SongAPI;

        DiscordGuildData data{};

        DiscordGuild() {};

        DiscordGuild(GuildData guildData) {
            this->data.guildId = guildData.id;
            this->getDataFromDB();
            this->data.guildName = guildData.name;
            this->data.memberCount = guildData.memberCount;
            this->writeDataToDB();
        }

        void writeDataToDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_WRITE;
            workload.guildData = this->data;
            requestAgent.submitWorkloadAndGetResults(workload);
        }

        void getDataFromDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_READ;
            workload.guildData = this->data;
            auto result = requestAgent.submitWorkloadAndGetResults(workload);
            if (result.discordGuild.guildId != "") {
                this->data = result.discordGuild;
            }
        }

    };

    class DiscordGuildMember {
    public:
        friend struct DatabaseReturnValue;

        DiscordGuildMemberData data{};

        DiscordGuildMember() {}

        DiscordGuildMember(GuildMemberData guildMemberData) {
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
            this->writeDataToDB();
        }

        void writeDataToDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE;
            workload.guildMemberData = this->data;
            requestAgent.submitWorkloadAndGetResults(workload);
        }

        void getDataFromDB() {
            DatabaseManagerAgent requestAgent{};
            DatabaseWorkload workload{};
            workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ;
            workload.guildMemberData = this->data;
            auto result = requestAgent.submitWorkloadAndGetResults(workload);
            if (result.discordGuildMember.globalId != "") {
                this->data = result.discordGuildMember;
            }
        }

    };

    void savePlaylist(DiscordGuild guild) {
        Playlist playlist = getSongAPIMap()->at(guild.data.guildId)->playlist;
        getSongAPIMap()->at(guild.data.guildId)->playlist = playlist;
        guild.data.playlist = playlist;
        guild.writeDataToDB();
    }

    Playlist loadPlaylist(DiscordGuild guild) {
        guild.getDataFromDB();
        if (getSongAPIMap()->contains(guild.data.guildId)) {
            getSongAPIMap()->at(guild.data.guildId)->playlist = guild.data.playlist;
            return guild.data.playlist;
        }
        else {
            return Playlist();
        }
    }

    mongocxx::collection DatabaseManagerAgent::collection{};
    mongocxx::instance DatabaseManagerAgent::instance{};
    mongocxx::database DatabaseManagerAgent::dataBase{};
    std::string DatabaseManagerAgent::botUserId{ "" };
    std::mutex DatabaseManagerAgent::workloadMutex{};
    mongocxx::client DatabaseManagerAgent::client{};
    int DiscordUser::guildCount{ 0 };
};
