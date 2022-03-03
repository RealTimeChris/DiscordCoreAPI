/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// FoundationEntities.cpp - Source file for the foundation entities.
/// Oct 1, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file FoundationEntities.cpp

#include <FoundationEntities.hpp>
#include <GuildEntities.hpp>
#include <GuildMemberEntities.hpp>
#include <InteractionEntities.hpp>
#include <RoleEntities.hpp>
#include <CoRoutine.hpp>
#include <InputEvents.hpp>

namespace DiscordCoreAPI {

    bool operator==(CURLCharWrapper& lhs, const  std::string& rhs) {
        if (static_cast<std::string>(lhs) == rhs) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string getISO8601TimeStamp(std::string year, std::string month, std::string day, std::string hour, std::string minute, std::string second) {
        std::string theTimeStamp{};
        theTimeStamp += year + "-";
        if (month.size() < 2) {
            theTimeStamp += "0" + month + "-";
        }
        else {
            theTimeStamp += month + "-";
        }
        if (day.size() < 2) {
            theTimeStamp += "0" + day;
        }
        else {
            theTimeStamp += day;
        }
        if (hour.size() < 2) {
            theTimeStamp += "T0" + hour;
        }
        else {
            theTimeStamp += "T" + hour;
        }
        if (minute.size() < 2) {
            theTimeStamp += ":0" + minute;
        }
        else {
            theTimeStamp += ":" + minute;
        }
        if (second.size() < 2) {
            theTimeStamp += ":0" + second;
        }
        else {
            theTimeStamp += ":" + second;
        }
        return theTimeStamp;
    }

    void reportException(std::string stackTrace, UnboundedMessageBlock<std::exception>* sendBuffer, bool rethrow) {
        try {
            auto currentException = std::current_exception();
            if (currentException) {
                std::rethrow_exception(currentException);
            }
        }
        catch (const std::exception& e) {
            if (sendBuffer != nullptr) {
                sendBuffer->send(e);
            }
            else {
                std::cout << shiftToBrightRed() << stackTrace + " Error: " << e.what() << reset() << "\n\n";
            }
            if (rethrow) {
                std::rethrow_exception(std::current_exception());
            }
        }
    }

    std::string convertTimeInMsToDateTimeString(int64_t timeInMs, TimeFormat timeFormat) {
        int64_t timeValue = timeInMs / 1000;
        time_t rawTime(timeValue);
        tm timeInfo = *localtime(&rawTime);
        std::string timeStamp{};
        timeStamp.resize(48);
        switch (timeFormat) {
        case TimeFormat::LongDate: {
            size_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G", &timeInfo);
            timeStamp.resize(sizeResponse);
            break;
        }
        case TimeFormat::LongDateTime: {
            size_t sizeResponse = strftime(timeStamp.data(), 48, "%a %b %d %Y %X", &timeInfo);
            timeStamp.resize(sizeResponse);
            break;
        }
        case TimeFormat::LongTime: {
            size_t sizeResponse = strftime(timeStamp.data(), 48, "%T", &timeInfo);
            timeStamp.resize(sizeResponse);
            break;
        }
        case TimeFormat::ShortDate: {
            size_t sizeResponse = strftime(timeStamp.data(), 48, "%d/%m/%g", &timeInfo);
            timeStamp.resize(sizeResponse);
            break;
        }
        case TimeFormat::ShortDateTime: {
            size_t sizeResponse = strftime(timeStamp.data(), 48, "%d %B %G %R", &timeInfo);
            timeStamp.resize(sizeResponse);
            break;
        }
        case TimeFormat::ShortTime: {
            size_t sizeResponse = strftime(timeStamp.data(), 48, "%R", &timeInfo);
            timeStamp.resize(sizeResponse);
            break;
        }
        default: {
            break;
        }
        }
        return timeStamp;
    }

    std::string convertMsToDurationString(int32_t durationInMs) {
        std::string newString{ "" };
        int32_t msPerSecond{ 1000 };
        int32_t secondsPerMinute{ 60 };
        int32_t minutesPerHour{ 60 };
        int32_t msPerMinute{ msPerSecond * secondsPerMinute };
        int32_t msPerHour{ msPerMinute * minutesPerHour };
        int32_t hoursLeft = static_cast<int32_t>(trunc(durationInMs / msPerHour));
        int32_t minutesLeft = static_cast<int32_t>(trunc((durationInMs % msPerHour) / msPerMinute));
        int32_t secondsLeft = static_cast<int32_t>(trunc(((durationInMs % msPerHour) % msPerMinute) / msPerSecond));
        if (hoursLeft >= 1) {
            newString += std::to_string(hoursLeft) + " Hours, ";
            newString += std::to_string(minutesLeft) + " Minutes, ";
            newString += std::to_string(secondsLeft) + " Seconds.";
        }
        else if (minutesLeft >= 1) {
            newString += std::to_string(minutesLeft) + " Minutes, ";
            newString += std::to_string(secondsLeft) + " Seconds.";
        }
        else {
            newString += std::to_string(secondsLeft) + " Seconds.";
        }
        return newString;
    }

    std::string convertToLowerCase(std::string stringToConvert) {
        std::string newString;
        for (auto& value : stringToConvert) {
            if (isupper(static_cast<unsigned char>(value))) {
                newString += static_cast<char>(tolower(static_cast<unsigned char>(value)));
            }
            else {
                newString += value;
            }
        }
        return newString;
    }

    int64_t convertTimestampToMsInteger(std::string timeStamp) {
        Time timeValue = Time(stoi(timeStamp.substr(0, 4)), stoi(timeStamp.substr(5, 6)), stoi(timeStamp.substr(8, 9)),
            stoi(timeStamp.substr(11, 12)), stoi(timeStamp.substr(14, 15)), stoi(timeStamp.substr(17, 18)));
        return timeValue.getTime() * 1000;
    }

    std::string base64Encode(std::string theString, bool url) {
        const char* base64_chars[2] = {
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789"
             "+/",

             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789"
             "-_" };

        size_t len_encoded = (theString.size() + 2) / 3 * 4;

        unsigned char trailing_char = url ? '.' : '=';

        const char* base64_chars_ = base64_chars[url];

        std::string returnString{};
        returnString.reserve(len_encoded);

        unsigned int pos = 0;

        while (pos < theString.size()) {
            returnString.push_back(base64_chars_[(theString[pos + 0] & 0xfc) >> 2]);

            if (pos + 1 < theString.size()) {
                returnString.push_back(base64_chars_[((theString[pos + 0] & 0x03) << 4) + ((theString[pos + 1] & 0xf0) >> 4)]);

                if (pos + 2 < theString.size()) {
                    returnString.push_back(base64_chars_[((theString[pos + 1] & 0x0f) << 2) + ((theString[pos + 2] & 0xc0) >> 6)]);
                    returnString.push_back(base64_chars_[theString[pos + 2] & 0x3f]);
                }
                else {
                    returnString.push_back(base64_chars_[(theString[pos + 1] & 0x0f) << 2]);
                    returnString.push_back(trailing_char);
                }
            }
            else {

                returnString.push_back(base64_chars_[(theString[pos + 0] & 0x03) << 4]);
                returnString.push_back(trailing_char);
                returnString.push_back(trailing_char);
            }

            pos += 3;
        }

        return returnString;
    }

    std::string urlDecode(std::string inputString) {
        CURLUWrapper urlHandle = curl_url();
        int32_t outLength{ 0 };
        CURLCharWrapper charString = curl_easy_unescape(urlHandle, inputString.c_str(), static_cast<int32_t>(inputString.length()), &outLength);
        std::string returnString = charString;
        return returnString;
    }

    std::string urlEncode(std::string inputString) {
        CURLWrapper curl = curl_easy_init();
        CURLCharWrapper output{ nullptr };
        output = curl_easy_escape(curl, inputString.c_str(), 0);
        std::string returnString = output;
        return returnString;
    }

    void spinLock(int64_t timeInNsToSpinLockFor) {
        int64_t startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        int64_t timePassed{ 0 };
        while (timePassed < timeInNsToSpinLockFor) {
            timePassed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTime;
        }
    }

    std::string getCurrentISO8601TimeStamp() {
        std::time_t result = std::time(nullptr);
        auto resultTwo = std::localtime(&result);
        std::string resultString = getISO8601TimeStamp(std::to_string(resultTwo->tm_year + 1900), std::to_string(resultTwo->tm_mon), std::to_string(resultTwo->tm_mday), std::to_string(resultTwo->tm_hour), std::to_string(resultTwo->tm_min), std::to_string(resultTwo->tm_sec));
        return resultString;
    }

    std::string generate64BaseEncodedKey() {
        std::string returnString{};
        returnString.resize(16);
        std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()) };
        for (uint32_t x = 0; x < 16; x += 1) {
            returnString[x] = static_cast<uint8_t>((static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max())) * 255.0f);
        }
        returnString = base64Encode(returnString, false);
        return returnString;
    }

    std::string shiftToBrightGreen() {
        return std::string("\033[1;40;92m");
    }

    std::string shiftToBrightBlue() {
        return std::string("\033[1;40;94m");
    }

    std::string shiftToBrightRed() {
        return std::string("\033[1;40;91m");
    }

    bool nanoSleep(int64_t ns) {
#ifdef _WIN32
        HANDLE timer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
        LARGE_INTEGER largeInt{ .QuadPart = -ns / 100 };
        if (!timer) {
            return false;
        }

        if (!SetWaitableTimerEx(timer, &largeInt, 0, NULL, NULL, NULL, 0)) {
            CloseHandle(timer);
            return false;
        }
        WaitForSingleObjectEx(timer, INFINITE, false);
        CloseHandle(timer);
#else
        std::this_thread::sleep_for(std::chrono::nanoseconds{ ns });
#endif
        return true;
    }

    std::string reset() {
        return std::string("\033[0m");
    }

    bool hasTimeElapsed(std::string timeStamp, int64_t days, int64_t hours, int64_t minutes) {
        int64_t startTimeRaw = convertTimestampToMsInteger(timeStamp);
        auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        int64_t secondsPerMinute = 60;
        int64_t secondsPerHour = secondsPerMinute * 60;
        int64_t secondsPerDay = secondsPerHour * 24;
        auto targetElapsedTime = ((days * secondsPerDay) + ((hours - 4) * secondsPerHour) + (minutes * secondsPerMinute)) * 1000;
        auto actualElapsedTime = currentTime - startTimeRaw;
        if (actualElapsedTime >= targetElapsedTime) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string getFutureISO8601TimeStamp(int32_t minutesToAdd, int32_t hoursToAdd, int32_t daysToAdd, int32_t monthsToAdd, int32_t yearsToAdd) {
        std::time_t result = std::time(nullptr);
        auto resultTwo = std::localtime(&result);
        int32_t newYearValue{ resultTwo->tm_year + yearsToAdd + 1900 };
        int32_t newMonthValue{ resultTwo->tm_mon + monthsToAdd };
        if (newMonthValue > 12) {
            newMonthValue -= 12;
        }
        int32_t newDayValue{ resultTwo->tm_mday + daysToAdd };
        if (newDayValue > 28) {
            newDayValue -= 28;
        }
        int32_t newHourValue{ resultTwo->tm_hour + hoursToAdd };
        if (newHourValue > 24) {
            newHourValue -= 24;
        }
        int32_t newMinuteValue{ resultTwo->tm_min + minutesToAdd };
        if (newMinuteValue > 60) {
            newMinuteValue -= 60;
        }
        std::string resultString{};
        if (!resultTwo->tm_isdst) {
            resultString = getISO8601TimeStamp(std::to_string(newYearValue), std::to_string(newMonthValue + 1), std::to_string(newDayValue), std::to_string(newHourValue + 5), std::to_string(newMinuteValue), std::to_string(resultTwo->tm_sec));
        }
        else {
            resultString = getISO8601TimeStamp(std::to_string(newYearValue), std::to_string(newMonthValue + 1), std::to_string(newDayValue), std::to_string(newHourValue + 4), std::to_string(newMinuteValue), std::to_string(resultTwo->tm_sec));
        }
        std::cout << "THE STRING: " << resultString << std::endl;
        return resultString;
    }

    std::string getTimeAndDate() {
        const time_t now = std::time(nullptr);
        tm time = *std::localtime(&now);
        std::string timeStamp{};
        timeStamp.resize(48);
        if (time.tm_isdst) {
            if (time.tm_hour + 4 >= 24) {
                time.tm_hour = 0 + time.tm_hour + 4 - 24;

            }
            else {
                time.tm_hour = time.tm_hour + 4;
            }
        }
        else {
            if (time.tm_hour + 5 >= 24) {
                time.tm_hour = 0 + time.tm_hour + 5 - 24;
            }
            else {
                time.tm_hour = time.tm_hour + 5;
            }
        }
        size_t size = strftime(timeStamp.data(), 48, "%F %R", &time);
        timeStamp.resize(size);
        return timeStamp;
    }

    std::string DiscordEntity::getCreatedAtTimestamp(TimeFormat timeFormat) {
        std::string returnString;
        int64_t timeInMs = (stoll(this->id) >> 22) + 1420070400000;
        returnString = convertTimeInMsToDateTimeString(timeInMs, timeFormat);
        return returnString;
    }

    std::vector<ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(std::vector<ApplicationCommandInteractionDataOption> originalOptions) {
        std::vector<ApplicationCommandInteractionDataOption> newVector;
        for (auto& value : originalOptions) {
            ApplicationCommandInteractionDataOption newItem = value;
            newItem.options = convertAppCommandInteractionDataOptions(value.options);
            newVector.push_back(newItem);
        }
        return newVector;
    }

    Song::operator YouTubeSong() {
        YouTubeSong newData{};
        newData.trackAuthorization = this->trackAuthorization;
        newData.finalDownloadUrls = this->finalDownloadUrls;
        newData.secondDownloadUrl = this->secondDownloadUrl;
        newData.firstDownloadUrl = this->firstDownloadUrl;
        newData.addedByUserName = this->addedByUserName;
        newData.html5PlayerFile = this->html5PlayerFile;
        newData.playerResponse = this->playerResponse;
        newData.contentLength = this->contentLength;
        newData.addedByUserId = this->addedByUserId;
        newData.doWeGetSaved = this->doWeGetSaved;
        newData.thumbnailUrl = this->thumbnailUrl;
        newData.description = this->description;
        newData.html5Player = this->html5Player;
        newData.songTitle = this->songTitle;
        newData.duration = this->duration;
        newData.viewUrl = this->viewUrl;
        newData.format = this->format;
        newData.songId = this->songId;
        newData.type = this->type;
        return newData;
    }

    Song::operator SoundCloudSong() {
        SoundCloudSong newData{};
        newData.trackAuthorization = this->trackAuthorization;
        newData.finalDownloadUrls = this->finalDownloadUrls;
        newData.secondDownloadUrl = this->secondDownloadUrl;
        newData.firstDownloadUrl = this->firstDownloadUrl;
        newData.addedByUserName = this->addedByUserName;
        newData.html5PlayerFile = this->html5PlayerFile;
        newData.playerResponse = this->playerResponse;
        newData.contentLength = this->contentLength;
        newData.addedByUserId = this->addedByUserId;
        newData.doWeGetSaved = this->doWeGetSaved;
        newData.thumbnailUrl = this->thumbnailUrl;
        newData.description = this->description;
        newData.html5Player = this->html5Player;
        newData.songTitle = this->songTitle;
        newData.duration = this->duration;
        newData.viewUrl = this->viewUrl;
        newData.format = this->format;
        newData.songId = this->songId;
        newData.type = this->type;
        return newData;
    }

    void Permissions::addPermissions(std::vector<Permission> permissionsToAdd) {
        if (*this == "") {
            this->push_back('0');
        }
        int64_t permissionsInteger = stoll(*this);
        for (auto value : permissionsToAdd) {
            permissionsInteger |= static_cast<int64_t>(value);
        }
        std::stringstream sstream;
        sstream << permissionsInteger;
        *this = sstream.str();
    }

    void Permissions::removePermissions(std::vector<Permission> permissionsToRemove) {
        if (*this == "") {
            this->push_back('0');
        }
        int64_t permissionsInteger = stoll(*this);
        for (auto value : permissionsToRemove) {
            permissionsInteger &= ~static_cast<int64_t>(value);
        }
        std::stringstream sstream;
        sstream << permissionsInteger;
        *this = sstream.str();
    }

    std::vector<std::string> Permissions::displayPermissions() {
        std::vector<std::string> returnVector{};
        if (*this == "") {
            this->push_back('0');
        }
        int64_t permissionsInteger = stoll(*this);
        if (permissionsInteger & 1 << 3) {
            for (int64_t x = 0; x < 41; x += 1) {
                permissionsInteger |= 1ll << x;
            }
        }
        if (permissionsInteger & (1ll << 0)) {
            returnVector.push_back("Create Instant Invite");
        }
        if (permissionsInteger & (1ll << 1)) {
            returnVector.push_back("Kick Members");
        }
        if (permissionsInteger & (1ll << 2)) {
            returnVector.push_back("Ban Members");
        }
        if (permissionsInteger & (1ll << 3)) {
            returnVector.push_back("Administrator");
        }
        if (permissionsInteger & (1ll << 4)) {
            returnVector.push_back("Manage Channels");
        }
        if (permissionsInteger & (1ll << 5)) {
            returnVector.push_back("Manage Guild");
        }
        if (permissionsInteger & (1ll << 6)) {
            returnVector.push_back("Add Reactions");
        }
        if (permissionsInteger & (1ll << 7)) {
            returnVector.push_back("View Audit Log");
        }
        if (permissionsInteger & (1ll << 8)) {
            returnVector.push_back("Priority Speaker");
        }
        if (permissionsInteger & (1ll << 9)) {
            returnVector.push_back("Stream");
        }
        if (permissionsInteger & (1ll << 10)) {
            returnVector.push_back("View Channel");
        }
        if (permissionsInteger & (1ll << 11)) {
            returnVector.push_back("Send Messages");
        }
        if (permissionsInteger & (1ll << 12)) {
            returnVector.push_back("Send TTS Messages");
        }
        if (permissionsInteger & (1ll << 13)) {
            returnVector.push_back("Manage Messages");
        }
        if (permissionsInteger & (1ll << 14)) {
            returnVector.push_back("Embed Links");
        }
        if (permissionsInteger & (1ll << 15)) {
            returnVector.push_back("Attach Files");
        }
        if (permissionsInteger & (1ll << 16)) {
            returnVector.push_back("Read Message History");
        }
        if (permissionsInteger & (1ll << 17)) {
            returnVector.push_back("Mention Everyone");
        }
        if (permissionsInteger & (1ll << 18)) {
            returnVector.push_back("Use External Emoji");
        }
        if (permissionsInteger & (1ll << 19)) {
            returnVector.push_back("View Guild Insights");
        }
        if (permissionsInteger & (1ll << 20)) {
            returnVector.push_back("Connect");
        }
        if (permissionsInteger & (1ll << 21)) {
            returnVector.push_back("Speak");
        }
        if (permissionsInteger & (1ll << 22)) {
            returnVector.push_back("Mute Members");
        }
        if (permissionsInteger & (1ll << 23)) {
            returnVector.push_back("Deafen Members");
        }
        if (permissionsInteger & (1ll << 24)) {
            returnVector.push_back("Move Members");
        }
        if (permissionsInteger & (1ll << 25)) {
            returnVector.push_back("Use VAD");
        }
        if (permissionsInteger & (1ll << 26)) {
            returnVector.push_back("Change Nickname");
        }
        if (permissionsInteger & (1ll << 27)) {
            returnVector.push_back("Manage Nicknames");
        }
        if (permissionsInteger & (1ll << 28)) {
            returnVector.push_back("Manage Roles");
        }
        if (permissionsInteger & (1ll << 29)) {
            returnVector.push_back("Manage Webhooks");
        }
        if (permissionsInteger & (1ll << 30)) {
            returnVector.push_back("Manage Emojis And Stickers");
        }
        if (permissionsInteger & (1ll << 31)) {
            returnVector.push_back("Use Application Commands");
        }
        if (permissionsInteger & (1ll << 32)) {
            returnVector.push_back("Request To Speak");
        }
        if (permissionsInteger & (1ll << 33)) {
            returnVector.push_back("Manage Events");
        }
        if (permissionsInteger & (1ll << 34)) {
            returnVector.push_back("Manage Threads");
        }
        if (permissionsInteger & (1ll << 35)) {
            returnVector.push_back("Create Public Threads");
        }
        if (permissionsInteger & (1ll << 36)) {
            returnVector.push_back("Create Private Threads");
        }
        if (permissionsInteger & (1ll << 37)) {
            returnVector.push_back("Use External Stickers");
        }
        if (permissionsInteger & (1ll << 38)) {
            returnVector.push_back("Send Messages In Threads");
        }
        if (permissionsInteger & (1ll << 39)) {
            returnVector.push_back("Start Embedded Activities");
        }
        if (permissionsInteger & (1ll << 40)) {
            returnVector.push_back("Moderate Members");
        }
        return returnVector;
    }

    std::string Permissions::getAllPermissions() {
        int64_t allPerms{ 0 };
        for (int64_t x = 0; x < 41; x += 1) {
            allPerms |= 1ll << x;
        }
        std::stringstream stream{};
        stream << allPerms;
        return stream.str();
    }

    std::string Permissions::getCurrentChannelPermissions(GuildMember guildMember, ChannelData channel) {
        std::string permsString = Permissions::computePermissions(guildMember, channel);
        return permsString;
    }

    bool Permissions::checkForPermission(GuildMember guildMember, ChannelData channel, Permission permission) {
        std::string permissionsString = Permissions::computePermissions(guildMember, channel);
        if ((stoll(permissionsString) & static_cast<int64_t>(permission)) == static_cast<int64_t>(permission)) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string Permissions::getCurrentGuildPermissions(GuildMember guildMember) {
        std::string permissions = Permissions::computeBasePermissions(guildMember);
        return permissions;
    }

    std::string Permissions::computeBasePermissions(GuildMember guildMember) {
        Guild guild = Guilds::getCachedGuildAsync({ .guildId = guildMember.guildId }).get();
        if (guild.ownerId == guildMember.user.id) {
            return Permissions::getAllPermissions();
        }

        RoleData roleEveryone{};
        for (auto& [key, value] : guild.roles) {
            if (value.id == guild.id) {
                roleEveryone = value;
            }
        }
        int64_t permissions{};
        if (roleEveryone.permissions != "") {
            permissions = stoll(roleEveryone.permissions);
        }
        GetGuildMemberRolesData getRolesData{};
        getRolesData.guildMember = guildMember;
        getRolesData.guildId = guildMember.guildId;
        auto guildMemberRoles = Roles::getGuildMemberRolesAsync(getRolesData).get();
        for (auto& value : guildMemberRoles) {
            permissions |= stoll(value.permissions);
        }

        if ((permissions & static_cast<int64_t>(Permission::Administrator)) == static_cast<int64_t>(Permission::Administrator)) {
            return Permissions::getAllPermissions();
        }

        return std::to_string(permissions);
    }

    std::string Permissions::getCurrentPermissionString() {
        return *this;
    }

    std::string Permissions::computeOverwrites(std::string basePermissions, GuildMember guildMember, ChannelData channel) {
        if ((stoll(basePermissions) & static_cast<int64_t>(Permission::Administrator)) == static_cast<int64_t>(Permission::Administrator)) {
            return Permissions::getAllPermissions();
        }

        int64_t permissions = stoll(basePermissions);
        if (channel.permissionOverwrites.contains(guildMember.guildId)) {
            OverWriteData overWritesEveryone = channel.permissionOverwrites[guildMember.guildId];
            permissions &= ~stoll(overWritesEveryone.deny);
            permissions |= stoll(overWritesEveryone.allow);
        }

        auto guildMemberRoles = Roles::getGuildMemberRolesAsync({ .guildMember = guildMember, .guildId = guildMember.guildId }).get();
        int64_t allow{ 0 };
        int64_t deny{ 0 };
        for (auto& value : guildMemberRoles) {
            if (channel.permissionOverwrites.contains(value.id)) {
                OverWriteData currentChannelOverwrites = channel.permissionOverwrites[value.id];
                allow |= stoll(currentChannelOverwrites.allow);
                deny |= stoll(currentChannelOverwrites.deny);
            }
        }
        permissions &= ~deny;
        permissions |= allow;
        if (channel.permissionOverwrites.contains(guildMember.user.id)) {
            OverWriteData currentOverWrites = channel.permissionOverwrites[guildMember.user.id];
            permissions &= ~stoll(currentOverWrites.deny);
            permissions |= stoll(currentOverWrites.allow);

        }
        return std::to_string(permissions);
    }

    std::string Permissions::computePermissions(GuildMember guildMember, ChannelData channel) {
        std::string permissions = Permissions::computeBasePermissions(guildMember);
        permissions = Permissions::computeOverwrites(permissions, guildMember, channel);
        return permissions;
    }

    RecurseThroughMessagePagesData recurseThroughMessagePages(std::string userID, std::unique_ptr<InputEventData> originalEvent, uint32_t currentPageIndex, std::vector<EmbedData> messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult) {
        std::unique_ptr<RecurseThroughMessagePagesData> returnData{ std::make_unique<RecurseThroughMessagePagesData>() };
        try {
            uint32_t newCurrentPageIndex = currentPageIndex;
            std::unique_ptr<RespondToInputEventData> dataPackage{ std::make_unique<RespondToInputEventData>(*originalEvent) };
            dataPackage->addMessageEmbed(messageEmbeds[currentPageIndex]);
            if (returnResult) {
                dataPackage->addButton(false, "select", "Select", ButtonStyle::Success, "✅");
            }
            dataPackage->addButton(false, "backwards", "Prev Page", ButtonStyle::Primary, "◀️");
            dataPackage->addButton(false, "forwards", "Next Page", ButtonStyle::Primary, "▶️");
            dataPackage->addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
            if (originalEvent->eventType == InputEventType::Regular_Message) {
                dataPackage->type = InputEventResponseType::Regular_Message_Edit;
            }
            else if (originalEvent->eventType == InputEventType::Application_Command_Interaction) {
                dataPackage->type = InputEventResponseType::Interaction_Response_Edit;
            }
            *originalEvent = InputEvents::respondToEvent(*dataPackage.get());
            while (true) {
                std::unique_ptr<ButtonCollector> button{ std::make_unique<ButtonCollector>(*originalEvent) };
                std::unique_ptr<std::vector<ButtonResponseData>> buttonIntData{ std::make_unique<std::vector<ButtonResponseData>>(button->collectButtonData(false, waitForMaxMs, 1, originalEvent->getRequesterId()).get()) };
                if (buttonIntData->size() == 0 || buttonIntData->at(0).buttonId == "empty" || buttonIntData->at(0).buttonId == "exit") {
                    if (buttonIntData->at(0).buttonId == "empty") {
                        dataPackage = std::make_unique<RespondToInputEventData>(*originalEvent);
                    }
                    else {
                        dataPackage = std::make_unique<RespondToInputEventData>(buttonIntData->at(0));
                    }

                    dataPackage->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
                    for (auto& value : originalEvent->getComponents()) {
                        for (auto& value02 : value.components) {
                            value02.disabled = true;
                        }
                        dataPackage->addComponentRow(value);
                    }
                    if (deleteAfter == true) {
                        InputEvents::deleteInputEventResponseAsync(*originalEvent);
                    }
                    else {
                        if (originalEvent->eventType == InputEventType::Regular_Message) {
                            dataPackage->type = InputEventResponseType::Regular_Message_Edit;
                        }
                        else {
                            dataPackage->type = InputEventResponseType::Interaction_Response_Edit;
                        }
                        InputEvents::respondToEvent(*dataPackage.get());
                    }
                    std::unique_ptr<RecurseThroughMessagePagesData> dataPackage02{ std::make_unique<RecurseThroughMessagePagesData>() };
                    dataPackage02->inputEventData = *originalEvent;
                    dataPackage02->buttonId = "exit";
                    return *dataPackage02;
                }
                else if (buttonIntData->at(0).buttonId == "forwards" || buttonIntData->at(0).buttonId == "backwards") {
                    if (buttonIntData->at(0).buttonId == "forwards" && (newCurrentPageIndex == (messageEmbeds.size() - 1))) {
                        newCurrentPageIndex = 0;
                    }
                    else if (buttonIntData->at(0).buttonId == "forwards" && (newCurrentPageIndex < messageEmbeds.size())) {
                        newCurrentPageIndex += 1;
                    }
                    else if (buttonIntData->at(0).buttonId == "backwards" && (newCurrentPageIndex > 0)) {
                        newCurrentPageIndex -= 1;
                    }
                    else if (buttonIntData->at(0).buttonId == "backwards" && (newCurrentPageIndex == 0)) {
                        newCurrentPageIndex = static_cast<uint8_t>(messageEmbeds.size()) - 1;
                    }
                    dataPackage = std::make_unique<RespondToInputEventData>(buttonIntData->at(0));
                    if (originalEvent->eventType == InputEventType::Regular_Message) {
                        dataPackage->type = InputEventResponseType::Regular_Message_Edit;
                    }
                    else if (originalEvent->eventType == InputEventType::Application_Command_Interaction) {
                        dataPackage->type = InputEventResponseType::Interaction_Response_Edit;
                    }
                    for (auto& value : originalEvent->getComponents()) {
                        dataPackage->addComponentRow(value);
                    }
                    dataPackage->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
                    InputEvents::respondToEvent(*dataPackage.get());
                }
                else if (buttonIntData->at(0).buttonId == "select") {
                    if (deleteAfter == true) {
                        InputEvents::deleteInputEventResponseAsync(*originalEvent);
                    }
                    else {
                        dataPackage = std::make_unique<RespondToInputEventData>(buttonIntData->at(0));
                        if (originalEvent->eventType == InputEventType::Regular_Message) {
                            dataPackage->type = InputEventResponseType::Regular_Message_Edit;
                        }
                        else if (originalEvent->eventType == InputEventType::Application_Command_Interaction) {
                            dataPackage->type = InputEventResponseType::Interaction_Response_Edit;
                        }
                        dataPackage->addMessageEmbed(messageEmbeds[newCurrentPageIndex]);
                        for (auto& value : originalEvent->getComponents()) {
                            for (auto& value02 : value.components) {
                                value02.disabled = true;
                            }
                            dataPackage->addComponentRow(value);
                        }
                        InputEvents::respondToEvent(*dataPackage.get());
                    }
                    returnData->currentPageIndex = newCurrentPageIndex;
                    returnData->inputEventData = *originalEvent;
                    returnData->buttonId = buttonIntData->at(0).buttonId;
                    return *returnData;
                }
            };
        }
        catch (...) {
            reportException("recurseThroughMessagePages()");
        }
        return *returnData;
    };
};