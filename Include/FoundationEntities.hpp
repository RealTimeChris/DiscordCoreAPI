// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifdef _WIN32
#ifdef DISCORDCOREAPIDLL_EXPORTS
#define DiscordCoreAPI_Dll __declspec(dllexport)
#else
#define DiscordCoreAPI_Dll __declspec(dllimport)
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "glib-2.0.lib")
#else
#ifndef DiscordCoreAPI_Dll
#define DiscordCoreAPI_Dll
#endif
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <ctime>
#endif

#include <cpp-base64/base64.h>
#include <condition_variable>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <curl/curl.h>
#include <functional>
#include <concepts>
#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <mutex>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

/**
* \defgroup main_endpoints Main Endpoints
* \brief For all of the Discord API's endpoints.
*/

/**
* \defgroup voice_connection Voice Connection
* \brief For all of the voice connection related stuff.
*/

/**
* \defgroup discord_events Discord Events
* \brief For all of the events that could be sent by the Discord API.
*/

/**
* \defgroup utilities Utilities
* \brief For utility classes/functions.
*/

/**
* \defgroup foundation_entities Foundation Entities
* \brief For all of the building blocks of the main endpoints.
*/

namespace DiscordCoreInternal {

    class VoiceSocketAgent;
    class BaseSocketAgent;
    class HttpClient;
    class DataParser;
}

namespace DiscordCoreAPI {

    struct RecurseThroughMessagePagesData;
    struct DeleteInteractionResponseData;
    struct DeleteFollowUpMessageData;
    struct OnInteractionCreationData;
    struct GetGuildMemberRolesData;
    struct BaseFunctionArguments;
    struct GetRolesData;
    struct CommandData;

    class CreateEphemeralInteractionResponseData;
    class CreateDeferredInteractionResponseData;
    class CreateEphemeralFollowUpMessageData;
    class CreateInteractionResponseData;
    class EditInteractionResponseData;
    class DeferComponentResponseData;
    class CreateFollowUpMessageData;
    class SoundCloudRequestBuilder;
    class RespondToInputEventData;
    class EditFollowUpMessageData;
    class YouTubeRequestBuilder;
    class SelectMenuCollector;
    class DiscordCoreClient;
    class CreateMessageData;
    class VoiceConnection;
    class EditMessageData;
    class ButtonCollector;
    class ModalCollector;
    class SoundCloudAPI;
    class Interactions;
    class EventManager;
    class EventHandler;
    class GuildMember;
    class ChannelData;
    class InputEvents;
    class EventWaiter;
    class SendDMData;
    class YouTubeAPI;
    class Reactions;
    class Messages;
    class WebHooks;
    class SongAPI;
    class BotUser;
    class Guilds;
    class Roles;
    class Guild;
    class Test;

    template<typename ReturnType, typename ...ArgTypes>
    class Event;
    template<typename ReturnType, typename ...ArgTypes>
    class EventDelegate;
    template<typename ReturnType>
    class CoRoutine;

    struct DiscordCoreAPI_Dll CURLWrapper {

        struct DiscordCoreAPI_Dll CURLDeleter {
            void operator()(CURL* other) {
                if (other != nullptr) {
                    curl_easy_cleanup(other);
                    other = nullptr;
                }
            }
        };

        CURLWrapper& operator=(CURL* other) {
            this->thePtr.reset(other);
            return *this;
        }

        CURLWrapper(CURL* other) {
            *this = other;
        }

        operator CURL*() {
            return this->thePtr.get();
        }

        CURLWrapper(nullptr_t other) {
            *this = other;
        }

    protected:
        std::unique_ptr<CURL, CURLDeleter> thePtr{ nullptr , CURLDeleter{} };
    };

    struct DiscordCoreAPI_Dll CURLUWrapper {

        struct DiscordCoreAPI_Dll CURLUDeleter {
            void operator()(CURLU* other) {
                if (other != nullptr) {
                    curl_url_cleanup(other);
                    other = nullptr;
                }
            }
        };

        CURLUWrapper& operator=(CURLU* other) {
            this->thePtr.reset(other);
            return *this;
        }

        CURLUWrapper(CURLU* other) {
            *this = other;
        }

        operator CURLU*() {
            return this->thePtr.get();
        }

        CURLUWrapper(nullptr_t other) {
            *this = other;
        }

    protected:
        std::unique_ptr<CURLU, CURLUDeleter> thePtr{ nullptr , CURLUDeleter{} };
    };

    struct DiscordCoreAPI_Dll CURLCharWrapper {

        struct DiscordCoreAPI_Dll CURLCharDeleter {
            void operator()(char* other) {
                if (other != nullptr) {
                    curl_free(other);
                    other = nullptr;
                }
            }
        };

        CURLCharWrapper& operator=(char* other) {
            this->thePtr.reset(other);
            return *this;
        }

        CURLCharWrapper(char* other) {
            *this = other;
        }

        operator std::string() {
            std::string returnValue = this->thePtr.get();
            return returnValue;
        }

        CURLCharWrapper(nullptr_t other) { 
            *this = other;
        }

    protected:
        std::unique_ptr<char, CURLCharDeleter> thePtr{ nullptr, CURLCharDeleter{} };
    };

    bool operator==(CURLCharWrapper& lhs, const  std::string& rhs);

    template<typename ObjectType>
    class ReferenceCountingPtr {
    public:

        class DiscordCoreAPI_Dll ObjectTypeWrapper {
        public:

            template<typename ObjectType02>
            friend class ReferenceCountingPtr;

            ObjectTypeWrapper& operator=(ObjectType* other) {
                this->thePtr = other;
                return *this;
            }

            ObjectTypeWrapper(ObjectType* other) {
                *this = other;
            }

            ObjectTypeWrapper() = default;

            void incrementCount() const {
                this->refCount += 1;
            }

            void release() const {
                assert(this->refCount > 0);
                this->refCount -= 1;
                if (this->refCount == 0) {
                    delete this;
                };
            }

            virtual ~ObjectTypeWrapper() {};

        protected:
            ObjectType* thePtr{ nullptr };
            mutable int32_t refCount{ 0 };
        };

        ReferenceCountingPtr& operator=(ObjectType* ptr) {
            ObjectTypeWrapper* newObject{ new ObjectTypeWrapper{ptr} };
            if (newObject != nullptr) {
                newObject->incrementCount();
            }
            if (this->thePtr != nullptr) {
                this->thePtr->release();
            }
            this->thePtr = newObject;
            return *this;
        }

        ReferenceCountingPtr(ObjectType* ptr = nullptr) {
            ObjectTypeWrapper* newObject{ new ObjectTypeWrapper{ptr} };
            if (newObject != nullptr) {
                this->thePtr = newObject;
                newObject->incrementCount();
            }
        }

        ReferenceCountingPtr& operator=(const ReferenceCountingPtr& ptr) {
            *this = ptr.thePtr->thePtr;
            return *this;
        }

        ObjectType* operator->() const {
            return this->thePtr->thePtr;
        }

        ObjectType& operator*() const {
            return *this->thePtr->thePtr;
        }

        ObjectType* get() const {
            return this->thePtr->thePtr;
        }

        ~ReferenceCountingPtr() {
            if (this->thePtr != nullptr) {
                this->thePtr->release();
            }
        }

    protected:
        ObjectTypeWrapper* thePtr{ nullptr };
    };

    template<typename ObjectType>
    class UniquePtrWrapper {
    public:

        UniquePtrWrapper<ObjectType>& operator=(const UniquePtrWrapper<ObjectType>& other) {
            this->thePtr.reset(other.thePtr.get());
            other.thePtr.~unique_ptr();
            return *this;
        }

        UniquePtrWrapper(const UniquePtrWrapper<ObjectType>& other) {
            *this = other;
        }

        UniquePtrWrapper<ObjectType>& operator=(UniquePtrWrapper<ObjectType>& other) {
            this->thePtr.reset(other.thePtr.release());
            return *this;
        }

        UniquePtrWrapper(UniquePtrWrapper<ObjectType>& other) {
            *this = other;
        }

        UniquePtrWrapper<ObjectType>& operator=(std::unique_ptr<ObjectType> other) {
            this->thePtr.reset(other.release());
            return *this;
        }

        UniquePtrWrapper(std::unique_ptr<ObjectType> other) {
            *this = std::move(other);
        }

        UniquePtrWrapper() = default;

        ObjectType* operator->() {
            return this->thePtr.get();
        }

        ObjectType* operator*() {
            return this->thePtr.get();
        }

        ObjectType* get() {
            return this->thePtr.get();
        }

    protected:

        std::unique_ptr<ObjectType> thePtr{ nullptr };

    };

    template<typename ObjectType>
    concept Copyable = std::copyable<ObjectType>;

    /// A messaging block for data-structures. \brief A messaging block for data-structures.
   /// \param ObjectType The type of object that will be sent over the message block.
    template<Copyable ObjectType>
    class UnboundedMessageBlock {
    public:

        UnboundedMessageBlock<ObjectType>& operator=(UnboundedMessageBlock<ObjectType>&& other) {
            if (this != &other) {
                this->theArray = std::move(other.theArray);
                other.theArray = std::queue<ObjectType>{};
            }
            return *this;
        }

        UnboundedMessageBlock(UnboundedMessageBlock<ObjectType>&& other) {
            *this = std::move(other);
        }

        UnboundedMessageBlock<ObjectType>& operator=(const UnboundedMessageBlock<ObjectType>&) = delete;

        UnboundedMessageBlock(const UnboundedMessageBlock<ObjectType>&) = delete;

        UnboundedMessageBlock<ObjectType>& operator=(UnboundedMessageBlock<ObjectType>&) = delete;

        UnboundedMessageBlock(UnboundedMessageBlock<ObjectType>&) = delete;

        UnboundedMessageBlock() = default;

        void send(ObjectType theObject) {
            this->theArray.push(theObject);
        }

        void clearContents() {
            this->theArray = std::queue<ObjectType>{};
        }

        bool tryReceive(ObjectType& theObject) {
            if (this->theArray.size() == 0) {
                return false;
            }
            else {
                theObject = this->theArray.front();
                this->theArray.pop();
                return true;
            }
        }

    protected:

        std::queue<ObjectType> theArray{};

    };

    /// A thread-safe messaging block for data-structures. \brief A thread-safe messaging block for data-structures.
    /// \param ObjectType The type of object that will be sent over the message block.
    template<Copyable ObjectType>
    class TSUnboundedMessageBlock {
    public:

        TSUnboundedMessageBlock<ObjectType>& operator=(TSUnboundedMessageBlock<ObjectType>&& other) noexcept {
            if (this != &other) {
                this->theArray = std::move(other.theArray);
                other.theArray = std::queue<ObjectType>{};
            }
            return *this;
        }

        TSUnboundedMessageBlock(TSUnboundedMessageBlock<ObjectType>&& other) noexcept {
            *this = std::move(other);
        }

        TSUnboundedMessageBlock<ObjectType>& operator=(const TSUnboundedMessageBlock<ObjectType>&) = delete;

        TSUnboundedMessageBlock(const TSUnboundedMessageBlock<ObjectType>&) = delete;

        TSUnboundedMessageBlock<ObjectType>& operator=(TSUnboundedMessageBlock<ObjectType>&) = delete;

        TSUnboundedMessageBlock(TSUnboundedMessageBlock<ObjectType>&) = delete;

        TSUnboundedMessageBlock() = default;

        void send(ObjectType theObject) {
            std::lock_guard<std::mutex> accessLock{ this->accessMutex };
            this->theArray.push(theObject);
        }

        void clearContents() {
            std::lock_guard<std::mutex> accessLock{ this->accessMutex };
            this->theArray = std::queue<ObjectType>{};
        }

        bool tryReceive(ObjectType& theObject) {
            std::lock_guard<std::mutex> accessLock{ this->accessMutex };
            if (this->theArray.size() == 0) {
                return false;
            }
            else {
                theObject = this->theArray.front();
                this->theArray.pop();
                return true;
            }
        }

    protected:

        std::mutex accessMutex{ std::mutex() };
        std::queue<ObjectType> theArray{};

    };

    class DiscordCoreAPI_Dll Time {
    public:

        Time(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second) {
            this->second = second;
            this->minute = minute;
            this->month = month;
            this->year = year;
            this->hour = hour;
            this->day = day;
        };

        int64_t getTime() {
            int64_t theValue{};
            for (int32_t x = 1970; x < this->year; x += 1) {
                theValue += this->secondsInJan;
                theValue += this->secondsInFeb;
                theValue += this->secondsInMar;
                theValue += this->secondsInApr;
                theValue += this->secondsInMay;
                theValue += this->secondsInJun;
                theValue += this->secondsInJul;
                theValue += this->secondsInAug;
                theValue += this->secondsInSep;
                theValue += this->secondsInOct;
                theValue += this->secondsInNov;
                theValue += this->secondsInDec;
                if (x % 4 == 0) {
                    theValue += this->secondsPerDay;
                }
            }
            if (this->month > 0) {
                theValue += static_cast<int64_t>((this->day - 1) * this->secondsPerDay);
                theValue += static_cast<int64_t>(this->hour * this->secondsPerHour);
                theValue += static_cast<int64_t>(this->minute * this->secondsPerMinute);
                theValue += this->second;
            }
            if (this->month > 1) {
                theValue += this->secondsInJan;
            }
            if (this->month > 2) {
                theValue += this->secondsInFeb;
            }
            if (this->month > 3) {
                theValue += this->secondsInMar;
            }
            if (this->month > 4) {
                theValue += this->secondsInApr;
            }
            if (this->month > 5) {
                theValue += this->secondsInMay;
            }
            if (this->month > 6) {
                theValue += this->secondsInJun;
            }
            if (this->month > 7) {
                theValue += this->secondsInJul;
            }
            if (this->month > 8) {
                theValue += this->secondsInAug;
            }
            if (this->month > 9) {
                theValue += this->secondsInSep;
            }
            if (this->month > 10) {
                theValue += this->secondsInOct;
            }
            if (this->month > 11) {
                theValue += this->secondsInNov;
            }
            return theValue;
        }

    protected:
        int32_t year{ 0 };
        int32_t month{ 0 };
        int32_t day{ 0 };
        int32_t hour{ 0 };
        int32_t minute{ 0 };
        int32_t second{ 0 };
        const int32_t secondsInJan{ 31 * 24 * 60 * 60 };
        const int32_t secondsInFeb{ 28 * 24 * 60 * 60 };
        const int32_t secondsInMar{ 31 * 24 * 60 * 60 };
        const int32_t secondsInApr{ 30 * 24 * 60 * 60 };
        const int32_t secondsInMay{ 31 * 24 * 60 * 60 };
        const int32_t secondsInJun{ 30 * 24 * 60 * 60 };
        const int32_t secondsInJul{ 31 * 24 * 60 * 60 };
        const int32_t secondsInAug{ 31 * 24 * 60 * 60 };
        const int32_t secondsInSep{ 30 * 24 * 60 * 60 };
        const int32_t secondsInOct{ 31 * 24 * 60 * 60 };
        const int32_t secondsInNov{ 30 * 24 * 60 * 60 };
        const int32_t secondsInDec{ 31 * 24 * 60 * 60 };
        const int32_t secondsPerMinute{ 60 };
        const int32_t secondsPerHour{ 60 * 60 };
        const int32_t secondsPerDay{ 60 * 60 * 24 };
    };

    template<typename TimeType>
    class StopWatch {
    public:

        StopWatch() = delete;

        StopWatch(TimeType maxNumberOfMsNew) {
            this->maxNumberOfMs = maxNumberOfMsNew.count();
            this->startTime = static_cast<int64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
        }

        int64_t totalTimePassed() {
            int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
            int64_t elapsedTime = currentTime - this->startTime;
            return elapsedTime;
        }

        bool hasTimePassed() {
            int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
            int64_t elapsedTime = currentTime - this->startTime;
            if (elapsedTime >= this->maxNumberOfMs) {
                return true;
            }
            else {
                return false;
            }
        }

        void resetTimer() {
            this->startTime = static_cast<int64_t>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
        }

    protected:

        int64_t maxNumberOfMs{ 0 };
        int64_t startTime{ 0 };

    };

    template <typename ObjectType>
    bool waitForTimeToPass(UnboundedMessageBlock<ObjectType>& outBuffer, ObjectType& argOne, int32_t timeInMsNew) {
        StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds(timeInMsNew) };
        bool doWeBreak{ false };
        while (!outBuffer.tryReceive(argOne)) {
            std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
            if (stopWatch.hasTimePassed()) {
                doWeBreak = true;
                break;
            }
        };
        return doWeBreak;
    }

    template <typename ObjectType>
    bool waitForTimeToPass(TSUnboundedMessageBlock<ObjectType>& outBuffer, ObjectType& argOne, int32_t timeInMsNew) {
        StopWatch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds(timeInMsNew) };
        bool doWeBreak{ false };
        while (!outBuffer.tryReceive(argOne)) {
            std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
            if (stopWatch.hasTimePassed()) {
                doWeBreak = true;
                break;
            }
        };
        return doWeBreak;
    }

    /// Time formatting methods. \brief Time formatting methods.
    enum class TimeFormat {
        LongDate = 'D',		/// "20 April 2021" - Long Date
        LongDateTime = 'F',		/// "Tuesday, 20 April 2021 16:20" - Long Date/Time
        LongTime = 'T',		/// "16:20:30" - Long Time
        ShortDate = 'd',		/// "20/04/2021" - Short Date
        ShortDateTime = 'f',		/// "20 April 2021 16:20" - Short Date/Time
        ShortTime = 't',		/// "16:20" - Short Time
    };

    DiscordCoreAPI_Dll std::string getISO8601TimeStamp(std::string year, std::string month, std::string day, std::string hour, std::string minute, std::string second);

    DiscordCoreAPI_Dll void reportException(std::string stackTrace, UnboundedMessageBlock<std::exception>* sendBuffer = nullptr, bool rethrow = false);

    DiscordCoreAPI_Dll bool hasTimeElapsed(std::string timeStamp, int64_t days = 0, int64_t hours = 0, int64_t minutes = 0);

    DiscordCoreAPI_Dll std::string convertTimeInMsToDateTimeString(int64_t timeInMs, TimeFormat timeFormat);

    DiscordCoreAPI_Dll std::string convertMsToDurationString(int32_t durationInMs);

    DiscordCoreAPI_Dll std::string convertToLowerCase(std::string stringToConvert);

    DiscordCoreAPI_Dll int64_t convertTimestampToInteger(std::string timeStamp);

    DiscordCoreAPI_Dll std::string urlDecode(std::string inputString);

    DiscordCoreAPI_Dll std::string urlEncode(std::string inputString);

    DiscordCoreAPI_Dll void spinLock(int64_t timeInNsToSpinLockFor);

    DiscordCoreAPI_Dll std::string generateX64BaseEncodedKey();

    DiscordCoreAPI_Dll std::string getTimeAndDate();

    DiscordCoreAPI_Dll bool nanoSleep(int64_t ns);    

    class DiscordCoreAPI_Dll TimeStamp {
    public:

        TimeStamp(std::string originalTimeStampNew) {
            this->originalTimeStamp = originalTimeStampNew;
        }

        std::string getDateTimeStamp(TimeFormat timeFormat) {
            this->timeStampInMs = convertTimestampToInteger(this->originalTimeStamp);
            std::string newString = convertTimeInMsToDateTimeString(this->timeStampInMs, timeFormat);
            return newString;
        }

        std::string getOriginalTimeStamp() {
            return this->originalTimeStamp;
        }

    protected:

        std::string originalTimeStamp{ "" };
        int64_t timeStampInMs{ 0 };
    };

    /**
    * \addtogroup utilities
    * @{
    */

    /// Permission values, for a given Channel, by Role or GuildMember. \brief Permission values, for a given Channel, by Role or GuildMember.
    enum class Permission : int64_t {
        Create_Instant_Invite = 0x0000000000000001,///< Create Instant Invite.
        Kick_Members = 0x0000000000000002,///< Kick Members.
        Ban_Members = 0x0000000000000004,///< Ban Members.
        Administrator = 0x0000000000000008,///< Administrator.
        Manage_Channels = 0x0000000000000010,///< Manage Channels.
        Manage_Guild = 0x0000000000000020,///< Manage Guild.
        Add_Reactions = 0x0000000000000040,///< Add Reactions.
        View_Audit_Log = 0x0000000000000080,///< View Audit Log.
        Priority_Speaker = 0x0000000000000100,///< Priority Speaker.
        Stream = 0x0000000000000200,///< Stream.
        View_Channel = 0x0000000000000400,///< View Channel.
        Send_Messages = 0x0000000000000800,///< Send Messages.
        Send_Tts_Messages = 0x0000000000001000,///< Send TTS Messages.
        Manage_Messages = 0x0000000000002000,///< Manage Messages.
        Embed_Links = 0x0000000000004000,///< Embed Links.
        Attach_Files = 0x0000000000008000,///< Attach Files.
        Read_Message_History = 0x0000000000010000,///< Read Message History.
        Mention_Everyone = 0x0000000000020000,///< Mention Everyone.
        Use_External_Emojis = 0x0000000000040000,///< Use External Emoji.
        View_Guild_Insights = 0x0000000000080000,///< View Guild Insights.
        Connect = 0x0000000000100000,///< Connect.
        Speak = 0x0000000000200000,///< Speak.
        Mute_Members = 0x0000000000400000,///< Mute Members.
        Deafen_Members = 0x0000000000800000,///< Deafen Members.
        Move_Members = 0x0000000001000000,///< Move Members.
        Use_Vad = 0x0000000002000000,///< Use VAD.
        Change_Nickname = 0x0000000004000000,///< Change Nickname.
        Manage_Nicknames = 0x0000000008000000,///< Manage Nicknames.
        Manage_Roles = 0x0000000010000000,///< Manage Roles.
        Manage_Webhooks = 0x0000000020000000,///< Manage Webhooks.
        Manage_Emojis_And_Stickers = 0x0000000040000000,///< Manage Emojis And Stickers.
        Use_Application_Commands = 0x0000000080000000,///< Use Application Commands.
        Request_To_Speak = 0x0000000100000000,///< Request To Speak.
        Manage_Events = 0x0000000200000000,///< Manage Events.
        Manage_Threads = 0x0000000400000000,///< Manage Threads.
        Create_Public_Threads = 0x0000000800000000,///< Create Public Threads.
        Create_Private_Threads = 0x0000001000000000,///< Create Private Threads.
        Use_External_Stickers = 0x0000002000000000,///< Use External Stickers.
        Send_Messages_In_Threads = 0x0000004000000000,///< Send Messages In Threads.
        Start_Embedded_Activities = 0x0000008000000000,///< Start Embedded Activities.
        Moderate_Members = 0x0000010000000000///< Moderate Members. 0x0000010000000000///< Moderate Members.
    };

    /// Permissions class DiscordCoreAPI_Dll, for representing and manipulating Permission values. \brief Permissions class DiscordCoreAPI_Dll, for representing and manipulating Permission values.
    class DiscordCoreAPI_Dll Permissions : public std::string {
    public:

        Permissions& operator=(std::string& other) {
            if (other.size() == 0) {
                this->push_back('0');
            }
            else {
                for (auto& value : other) {
                    this->push_back(value);
                }
            }
            return *this;
        }

        Permissions(std::string permsNew) {
            if (permsNew.size() == 0) {
                this->push_back('0');
            }
            else {
                for (auto& value : permsNew) {
                    this->push_back(value);
                }
            }
        }

        operator const char*() {
            return this->data();
        }

        /// Adds one or more Permissions to the current Permissions value. \brief Adds one or more Permissions to the current Permissions value.
        /// \param permissionsToAdd A std::vector containing the Permissions you wish to add.
        void addPermissions(std::vector<Permission> permissionsToAdd);

        /// Removes one or more Permissions from the current Permissions value. \brief Removes one or more Permissions from the current Permissions value.
        /// \param permissionsToRemove A std::vector containing the Permissions you wish to remove.
        void removePermissions(std::vector<Permission> permissionsToRemove);

        /// Displays the currently present Permissions in a std::string, and returns a std::vector with each of them stored in std::string format. \brief Displays the currently present Permissions in a std::string, and returns a std::vector with each of them stored in std::string format.
        /// \returns A std::vector full of strings of the Permissions that are in the input std::string's value.
        std::vector<std::string> displayPermissions();

        /// Returns a std::string containing ALL of the possible Permissions. \brief Returns a std::string containing ALL of the possible Permissions.
        /// \returns A std::string containing all of the possible Permissions.
        static std::string getAllPermissions();

        /// Returns a std::string containing the currently held Permissions. \brief Returns a std::string containing the currently held Permissions.
        /// \returns A std::string containing the current Permissions.
        std::string getCurrentPermissionString();

        /// Returns a std::string containing the currently held Permissions in a given Guild. \brief Returns a std::string containing the currently held Permissions in a given Guild.
        /// \param guildMember The GuildMember who's Permissions are to be evaluated.
        /// \returns A std::string containing the current Permissions.
        static std::string getCurrentGuildPermissions(GuildMember guildMember);

        /// Returns a std::string containing all of a given User's Permissions for a given Channel. \brief Returns a std::string containing all of a given User's Permissions for a given Channel.
        /// \param guildMember The GuildMember who's Permissions to analyze.
        /// \param channel The Channel withint which to check for Permissions.
        /// \returns A std::string containing the final Permission's value for a given Channel.
        static std::string getCurrentChannelPermissions(GuildMember guildMember, ChannelData channel);

        /// Checks for a given Permission in a chosen Channel, for a specific User. \brief Checks for a given Permission in a chosen Channel, for a specific User.
        /// \param guildMember The GuildMember who to check the Permissions of.
        /// \param channel The Channel within which to check for the Permission's presence.
        /// \param permission A Permission to check the current Channel for.
        /// \returns A bool suggesting the presence of the chosen Permission.
        bool checkForPermission(GuildMember guildMember, ChannelData channel, Permission permission);

    protected:

        static std::string computeBasePermissions(GuildMember guildMember);

        static std::string computeOverwrites(std::string basePermissions, GuildMember guildMember, ChannelData channel);

        static std::string computePermissions(GuildMember guildMember, ChannelData channel);
    };

    /**@}*/

    /**
    * \addtogroup foundation_entities
    * @{
    */

    /// For ids of DiscordEntities. \brief For ids of DiscordEntities.
    using Snowflake = std::string;

    /// Base class DiscordCoreAPI_Dll for all Discord entities. \brief Base class DiscordCoreAPI_Dll for all Discord entities.
    class DiscordCoreAPI_Dll DiscordEntity {
    public:
        Snowflake id{ "" };///< The identifier "snowflake" of the given entity.
        /// Converts the snowflake-id into a time and date stamp. \brief Converts the snowflake-id into a time and date stamp.
        /// \returns std::string A std::string containing the timestamp.
        std::string getCreatedAtTimestamp(TimeFormat timeFormat);

        virtual ~DiscordEntity() {};
    };

    /// Role tags data. \brief Role tags data.
    struct DiscordCoreAPI_Dll RoleTagsData {
        std::string premiumSubscriber{ "" };///< Are they a premium subscriber?
        std::string integrationId{ "" };///< What is the integration id?
        std::string botId{ "" };///< What is the bot id?
    };

    /// Data structure representing a single Role. \brief Data structure representing a single Role.
    class DiscordCoreAPI_Dll RoleData : public DiscordEntity {
    public:
        std::string unicodeEmoji{ "" };///< Emoji representing the Role.
        Permissions permissions{ "" };///< The Role's base Guild Permissions.
        bool mentionable{ false };///< Is ths Role mentionable?
        std::string name{ "" };///< The Role's name.
        std::string icon{ "" };///< Icon representing the Role.
        int32_t position{ 0 };///< Its position amongst the rest of the Guild's roles.
        bool managed{ false };///< is it a managed Role?
        bool hoist{ false };///< Is it hoisted above the other roles?
        RoleTagsData tags{};///< Role tags for the Role.
        int32_t color{ 0 };///< The Role's color.

        virtual ~RoleData() {};
    };

    /// Data structure representing a single User. \brief Data structure representing a single User.
    class DiscordCoreAPI_Dll UserData : public DiscordEntity {
    public:
        std::string discriminator{ "" };///< The # next to their User name.
        std::string createdAt{ "" };///< When the User was created.
        std::string userName{ "" };///< Their username.
        bool mfaEnabled{ false };///< MFA enabled?
        int32_t premiumType{ 0 };///< Their premium nitro status.
        int32_t publicFlags{ 0 };///< Public flags.
        std::string avatar{ "" };///< Their avatar url.
        std::string locale{ "" };///< The region they are from/in.
        std::string email{ "" };///< Their email address.
        bool verified{ false };///< Verified?
        bool system{ false };///< Are they a partof the system?
        int32_t flags{ 0 };///< Flags.
        bool bot{ false };///< Are they a bot?

        virtual ~UserData() {};
    };

    /// Attachment data. \brief Attachment data.
    class DiscordCoreAPI_Dll AttachmentData : public DiscordEntity {
    public:
        std::string contentType{ "" };///< Type of content for the attachment.
        std::string filename{ "" };///< The file name of the attachment.
        std::string proxyUrl{ "" };///< The proxy url for the attachment.
        std::string url{ "" };///< The url for the attachment.
        int32_t height{ 0 };///< The height of the attachment.
        int32_t width{ 0 };///< The width of the attachment.
        int32_t size{ 0 };///< The size of the attachment.

        virtual ~AttachmentData() {};
    };

    /// Sticker format types. \brief Sticker format types.
    enum class StickerFormatType {
        Png = 1,///< Png.
        Apng = 2,///< Apng.
        Lottie = 3///< Lottie
    };

    /// Embed footer data. \brief Embed footer data.
    struct DiscordCoreAPI_Dll EmbedFooterData {
        std::string proxyIconUrl{ "" };///< Proxy icon url.
        std::string iconUrl{ "" };///< Icon url.
        std::string text{ "" };///< Footer text.
    };

    /// Embed image data. \brief Embed image data.
    struct DiscordCoreAPI_Dll EmbedImageData {
        std::string proxyUrl{ "" };///< Proxy url.
        std::string url{ "" };///< Image url.
        int32_t height{ 0 };///< Image height.
        int32_t width{ 0 };///< Image width.
    };

    /// Embed thumbnail data. \brief Embed thumbnail data.
    struct DiscordCoreAPI_Dll EmbedThumbnailData {
        std::string proxyUrl{ "" };///< Proxy url.
        std::string url{ "" };///< Image url.
        int32_t height{ 0 };///< Image height.
        int32_t width{ 0 };///< Image width.
    };

    /// Embed video data. \brief Embed video data.
    struct DiscordCoreAPI_Dll EmbedVideoData {
        std::string proxyUrl{ "" };///< Proxy url.
        std::string url{ "" };///< Image url.
        int32_t height{ 0 };///< Image height.
        int32_t width{ 0 };///< Image width.
    };

    /// Embed provider data. \brief Embed provider data.
    struct DiscordCoreAPI_Dll EmbedProviderData {
        std::string name{ "" };///< Name.
        std::string url{ "" };///< Url.
    };

    /// Embed author data. \brief Embed author data.
    struct DiscordCoreAPI_Dll EmbedAuthorData {
        std::string proxyIconUrl{ "" };///< Proxy icon url.
        std::string iconUrl{ "" };///< Icon url.
        std::string name{ "" };///< Name.
        std::string url{ "" };///< Url.
    };

    /// Embed field data. \brief Embed field data.
    struct DiscordCoreAPI_Dll EmbedFieldData {
        std::string value{ "" };///< The text on the field.
        std::string name{ "" };///< The title of the field.
        bool Inline{ false };///< Is the field inline with the rest of them?
    };

    /// Embed types. \brief Embed types.
    enum class EmbedType {
        Rich = 0,///< Rich.
        Image = 1,///< Image.
        Video = 2,///< Video.
        Gifv = 3,///< Gifv.
        Article = 4,///< Article.
        link = 5///< Link.
    };

    /// Embed data. \brief Embed data.
    class DiscordCoreAPI_Dll EmbedData {
    public:
        std::string hexColorValue{ "000000" };///< Hex color value of the embed.
        std::vector<EmbedFieldData> fields{};///< Array of embed fields.
        EmbedThumbnailData thumbnail{};///< Embed thumbnail data.
        std::string description{ "" };///< Description of the embed.
        EmbedProviderData provider{};///< Embed provider data.
        std::string timestamp{ "" };///< Timestamp to be placed on the embed.
        EmbedFooterData footer{};///< Embed footer data.
        EmbedAuthorData author{};///< Embed author data.
        std::string title{ "" };///< Title of the embed.
        std::string type{ "" };///< Type of the embed.
        EmbedImageData image{};///< Embed image data.
        EmbedVideoData video{};///< Embed video data.
        std::string url{ "" };///< Url for the embed.

        /// Sets the author's name and avatar for the embed. \brief Sets the author's name and avatar for the embed.
        /// \param authorName The author's name.
        /// \param authorAvatarUrl The url to their avatar.
        /// \returns A pointer to this embed.
        EmbedData& setAuthor(std::string authorName, std::string authorAvatarUrl = "") {
            this->author.name = authorName;
            this->author.iconUrl = authorAvatarUrl;
            return *this;
        }

        /// Sets the footer's values for the embed. \brief Sets the footer's values for the embed.
        /// \param footerText The footer's text.
        /// \param footerIconUrlText Url to the footer's icon.
        /// \returns A pointer to this embed.
        EmbedData& setFooter(std::string footerText, std::string footerIconUrlText = "") {
            this->footer.text = footerText;
            this->footer.iconUrl = footerIconUrlText;
            return *this;
        }

        /// Sets the timestamp on the embed. \brief Sets the timestamp on the embed.
        /// \param timeStamp The timestamp to be set.
        /// \returns A pointer to this embed.
        EmbedData& setTimeStamp(std::string timeStamp) {
            this->timestamp = timeStamp;
            return *this;
        }

        /// Adds a field to the embed. \brief Adds a field to the embed.
        /// \param name The title of the embed field.
        /// \param value The contents of the embed field.
        /// \param Inline Is it inline with the rest of the fields on the embed?
        /// \returns A pointer to this embed.
        EmbedData& addField(std::string name, std::string value, bool Inline = true) {
            EmbedFieldData embedFieldData;
            embedFieldData.name = name;
            embedFieldData.Inline = Inline;
            embedFieldData.value = value;
            this->fields.push_back(embedFieldData);
            return *this;
        }

        /// Sets the description (the main contents) of the embed. \brief Sets the description (the main contents) of the embed.
        /// \param descriptionNew The contents of the description to set.
        /// \returns A pointer to this embed.
        EmbedData& setDescription(std::string descriptionNew) {
            this->description = descriptionNew;
            return *this;
        }

        /// Sets the color of the embed, by applying a hex-color value. \brief Sets the color of the embed, by applying a hex-color value.
        /// \param hexColorValueNew A std::string containing a hex-number value (Between 0x00 and 0xFFFFFF).
        /// \returns A pointer to this embed.
        EmbedData& setColor(std::string hexColorValueNew) {
            this->hexColorValue = hexColorValueNew;
            return *this;
        }

        /// Sets the thumbnail of the embed. \brief Sets the thumbnail of the embed.
        /// \param thumbnailUrl The url to the thumbnail to be used.
        /// \returns A pointer to this embed.
        EmbedData& setThumbnail(std::string thumbnailUrl) {
            this->thumbnail.url = thumbnailUrl;
            return *this;
        }

        /// Sets the title of the embed. \brief Sets the title of the embed.
        /// \param titleNew A std::string containing the desired title.
        /// \returns A pointer to this embed.
        EmbedData& setTitle(std::string titleNew) {
            this->title = titleNew;
            return *this;
        }

        /// Sets the image of the embed. \brief Sets the image of the embed.
        /// \param imageUrl The url of the image to be set on the embed.
        /// \returns A pointer to this embed.
        EmbedData& setImage(std::string imageUrl) {
            this->image.url = imageUrl;
            return *this;
        }
    };

    /// Message reference data.\brief Message reference data.
    struct DiscordCoreAPI_Dll MessageReferenceData {
        bool failIfNotExists{ false };///< Fail if the Message doesn't exist?
        std::string messageId{ "" };///< Id of the Message to reference.
        std::string channelId{ "" };///< Id of the Channel that the referenced Message was sent in.
        std::string guildId{ "" };///< Id of the Guild that the referenced Message was sent in.
    };

    /// Permission overwrites types. \brief Permission overwrites types.
    enum class PermissionOverwritesType {
        Role = 0,///< Role.
        User = 1///< User.
    };

    /// A Permission overwrite, for a given Channel. \brief A Permission overwrite, for a given Channel.
    class DiscordCoreAPI_Dll OverWriteData : public DiscordEntity {
    public:
        PermissionOverwritesType type{};///< Role or User type.
        std::string channelId{ "" };///< Channel id for which Channel this overwrite beint64_ts to.
        Permissions allow{ "" };///< Collection of Permissions to allow.
        Permissions deny{ "" };///< Collection of Permissions to deny.

        virtual ~OverWriteData() {};
    };

    /// Channel types. \brief Channel types.
    enum class ChannelType {
        Guild_Text = 0,///< Guild text.
        Dm = 1,///< Direct-Message.
        Guild_Voice = 2,/// Guild voice.
        Group_Dm = 3,///< Group direct-Message.
        Guild_Category = 4,///< Guild category.
        Guild_News = 5,///< Guild news.
        Guild_Store = 6,///< Guild store.
        Guild_News_Thread = 10,///< Guild news Thread.
        Guild_Public_Thread = 11,///< Guild public Thread.
        Guild_Private_Thread = 12,///< Guild private Thread.
        Guild_Stage_Voice = 13///< Guild stage-voice.
    };

    /// Meta data for a Thread type of Channel. \brief Meta data for a Thread type of Channel.
    struct DiscordCoreAPI_Dll ThreadMetadataData {
        int32_t autoArchiveDuration{ 0 };///< How int64_t before archiving this Thread.
        TimeStamp archiveTimestamp{ "" };///< (Where applicable) the time at which this Thread was archived.
        bool invitable{ false };///< The id of the individual who archived this Thread.
        bool archived{ false };///< Whether or not this Thread is currently archived.
        bool locked{ false };///< Whether or not this Thread is currently locked.
    };

    /// Data for a single member of a Thread. \brief Data for a single member of a Thread.
    class DiscordCoreAPI_Dll ThreadMemberData : public DiscordEntity {
    public:
        TimeStamp joinTimestamp{ "" };///< The time at which the member joined this Thread.
        std::string userId{ "" };///< The User's id.
        int32_t flags{ 0 };///< Flags.

        virtual ~ThreadMemberData() {};
    };

    /// Thread types. \brief Thread types.
    enum class ThreadType {
        Guild_News_Thread = 10,///< Guild news Thread.
        Guild_Public_Thread = 11,///< Guild public Thread.
        Guild_Private_Thread = 12///< Guild private Thread.
    };

    /// Automatic Thread archiving durations. \brief Automatic Thread archiving durations.
    enum class ThreadAutoArchiveDuration :int32_t {
        Shortest = 60,///< Shortest.
        Short = 1440,///< Short.
        Long = 4320,///< Long.
        Longest = 10080///< Longest.
    };

    /// Data structure representing a single Channel. \brief Data structure representing a single Channel.
    class DiscordCoreAPI_Dll ChannelData : public DiscordEntity {
    public:
        std::unordered_map<std::string, OverWriteData> permissionOverwrites{};///< Permission overwrites for the given Channel.
        std::unordered_map<std::string, UserData> recipients{};///< Recipients, in the case of a group Dm or Dm.
        int32_t defaultAutoArchiveDuration{ 0 };
        ThreadMetadataData threadMetadata{};///< Metadata in the case that this Channel is a Thread.
        ChannelType type{ ChannelType::Dm };///< The type of the Channel.
        TimeStamp lastPinTimestamp{ "" };///< Timestamp of the last pinned Message.
        std::string lastMessageId{ "" };///< Id of the last Message.
        std::string applicationId{ "" };///< Application id of the current application.
        int32_t videoQualityMode{ 0 };///< Video quality mode.
        int32_t rateLimitPerUser{ 0 };///< Amount of seconds a User has to wait before sending another Message.
        std::string rtcRegion{ "" };///< Real-time clock region.
        std::string parentId{ "" };///< Id of the parent Channel, if applicable.
        std::string guildId{ "" };///< Id of the Channel's Guild, if applicable.
        std::string ownerId{ "" };///< Id of the Channel's owner.
        ThreadMemberData member{};///< Thread member object for the current User, if they have joined the Thread.
        int32_t messageCount{ 0 };///< An approximate count of Messages in a Thread, stops counting at 50.
        int32_t memberCount{ 0 };///< Count of members active in the Channel.
        std::string topic{ "" };///< The Channel's topic.
        int32_t userLimit{ 0 };///< User limit, in the case of voice channels.
        std::string name{ "" };///< Name of the Channel.
        std::string icon{ "" };///< Icon for the Channel, if applicable.
        int32_t position{ 0 };///< The position of the Channel, in the Guild's Channel list.
        int32_t bitrate{ 0 };///< Bitrate of the Channel, if it is a voice Channel.
        bool nsfw{ false };///< Whether or not it is nsfw.        

        virtual ~ChannelData() {};
    };

    /// Voice data for a given GuildMember. \brief Voice data for a given GuildMember.
    struct DiscordCoreAPI_Dll VoiceData {
        TimeStamp requestToSpeakTimestamp{ "" };///< The time at which the User requested to speak.
        std::string channelId{ "" };///< The Channel id this User is connected to.
        std::string sessionId{ "" };///< The session id for this voice state.
        std::string guildId{ "" };///< The Guild id this voice state is for.
        std::string userId{ "" };///< The User id this voice state is for.
        bool selfStream{ false };///< Whether this User is streaming using "Go Live".
        bool selfVideo{ false };///< Whether this User's camera is enabled.
        bool selfDeaf{ false };///< Whether this User is locally deafened.
        bool selfMute{ false };///< Whether this User is locally muted.
        bool suppress{ false };///< Whether this User is muted by the current User.
        bool deaf{ false };///< Whether this User is deafened by the server.
        bool mute{ false };///< Whether this User is muted by the server.
    };

    /// Data structure representing a single GuildMember. \brief Data structure representing a single GuildMember.
    class DiscordCoreAPI_Dll GuildMemberData : public DiscordEntity {
    public:
        std::vector<std::string> roles{};///< The Guild roles that they have.
        std::string premiumSince{ "" };///< If applicable, when they first boosted the server.
        Permissions permissions{ "" };///< Their base-level Permissions in the Guild.
        std::string userMention{ "" };///< What to enter to get them mentioned in a Message.
        std::string guildId{ "" };///< The current Guild's id.
        TimeStamp joinedAt{ "" };///< When they joined the Guild.
        std::string nick{ "" };///< Their nick/display name.
        bool pending{ false };///< Are they waiting at the entry screen?
        VoiceData voiceData{};///< Info about this GuildMember's voice state.
        bool deaf{ false };///< Are they server deafened?
        bool mute{ false };///< Are they server muted?
        UserData user{};///< User data for the current GuildMember.

        virtual ~GuildMemberData() {};
    };

    /// Voice state data. \brief Voice state data.
    struct DiscordCoreAPI_Dll VoiceStateData {
        TimeStamp requestToSpeakTimestamp{ "" };///< The time at which the User requested to speak.
        std::string channelId{ "" };///< The Channel id this User is connected to.
        std::string sessionId{ "" };///< The session id for this voice state.
        std::string guildId{ "" };///< The Guild id this voice state is for.
        std::string userId{ "" };///< The User id this voice state is for.
        GuildMemberData member{};///< The Guild member this voice state is for.
        bool selfStream{ false };///< Whether this User is streaming using "Go Live".
        bool selfVideo{ false };///< Whether this User's camera is enabled.
        bool selfDeaf{ false };///< Whether this User is locally deafened.
        bool selfMute{ false };///< Whether this User is locally muted.
        bool suppress{ false };///< Whether this User is muted by the current User.
        bool deaf{ false };///< Whether this User is deafened by the server.
        bool mute{ false };///< Whether this User is muted by the server.
        operator VoiceData() {
            VoiceData newData{};
            newData.requestToSpeakTimestamp = this->requestToSpeakTimestamp;
            newData.selfStream = this->selfStream;
            newData.selfVideo = this->selfVideo;
            newData.sessionId = this->sessionId;
            newData.channelId = this->channelId;
            newData.suppress = this->suppress;
            newData.selfDeaf = this->selfDeaf;
            newData.selfMute = this->selfMute;
            newData.guildId = this->guildId;
            newData.userId = this->userId;
            newData.deaf = this->deaf;
            newData.mute = this->mute;
            return newData;
        }
    };

    /// Data representing an active Thread. \brief Data representing an active Thread.
    struct DiscordCoreAPI_Dll ActiveThreadsData {
        std::vector<ThreadMemberData> members{};
        std::vector<ChannelData> threads{};
        bool hasMore{ false };
    };

    /// Data representing an archived Thread. \brief Data representing an archived Thread.
    struct DiscordCoreAPI_Dll ArchivedThreadsData : public ActiveThreadsData {};

    /// Application command-option types. \brief Application command-option types.
    enum class ApplicationCommandOptionType {
        Sub_Command = 1,///< Sub-command.
        Sub_Command_Group = 2,///< Sub-command group.
        String = 3,///< String.
        Integer = 4,///< Integer.
        Boolean = 5,///< Boolean.
        User = 6,///< User.
        Channel = 7,///< Channel.
        Role = 8,///< Role.
        Mentionable = 9,///< Mentionable.
        Number = 10///< Number.
    };

    /// Application command permission-types. \brief Application command permission-types.
    enum class ApplicationCommandPermissionType {
        Role = 1,///< Role.
        User = 2///< User.
    };

    /// Permissions data for an ApplicationCommand. \brief Permissions data for an ApplicationCommand.
    class DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordEntity {
    public:
        ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role };///< The type of Permission.
        bool permission{ false };///< Whether the Permission is active or not.

        virtual ~ApplicationCommandPermissionData() {}
    };

    /// Represents the Permissions for accessing an ApplicationCommand from within a Guild. \brief Represents the Permissions for accessing an ApplicationCommand from within a Guild.
    class DiscordCoreAPI_Dll GuildApplicationCommandPermissionsData : public DiscordEntity {
    public:
        std::vector<ApplicationCommandPermissionData> permissions{};///< The Permissions.
        std::string applicationId{ "" };///< The application's id.
        std::string guildId{ "" };///< The Guild's id.

        virtual ~GuildApplicationCommandPermissionsData() {}
    };

    /// Data structure representing a single emoji. \brief Data structure representing a single emoji.
    class DiscordCoreAPI_Dll EmojiData : public DiscordEntity {
    public:
        std::vector<RoleData> roles{};///< Roles that are allowed to use this emoji.
        bool requireColons{ false };///< Require colons to render it?
        bool available{ true };///< Is it available to be used?
        bool animated{ false };///< Is it animated?
        std::string name{ "" };///< What is its name?
        bool managed{ false };///< Is it managed?
        UserData user{};///< User that created this emoji.

        virtual ~EmojiData() {};
    };

    /// For updating/modifying a given Channel's properties. \brief For updating/modifying a given Channel's properties.
    struct DiscordCoreAPI_Dll UpdateChannelData {
        std::unordered_map<std::string, OverWriteData> permissionOverwrites{};
        int32_t defaultAutoArchiveDuration{ 10080 };
        int32_t videoQualityMode{ 1 };
        int32_t rateLimitPerUser{ 0 };
        std::string parentId{ "" };
        std::string rtcRgion{ "" };
        int32_t bitrate{ 48000 };
        std::string topic{ "" };
        int32_t userLimit{ 0 };
        std::string name{ "" };
        int32_t position{ 0 };
        ChannelType type{};
        bool nsfw{ false };
    };

    /// Data structure representing a single reaction. \brief /// Data structure representing a single reaction.
    class DiscordCoreAPI_Dll ReactionData : public DiscordEntity {
    public:
        std::string channelId{ "" };///< The id of the Channel where it was placed.
        std::string messageId{ "" };///< The id of the Message upon which it was placed.
        std::string guildId{ "" };///< The id of the Guild where it was placed.
        std::string userId{ "" };///< The id of the User who placed the reaction.
        GuildMemberData member{};///< The GuildMember who placed the reaction.
        int32_t count{ 0 };///< The number of times this particular emoji was placed as a reaction to the given Message.
        EmojiData emoji{};///< The emoji that was placed as a reaction.
        bool me{ false };///< Whether or not I (The bot) placed it.

        virtual ~ReactionData() {};
    };

    /// Structure representing Voice Region Data. \brief Structure representing Voice Region Data.
    struct DiscordCoreAPI_Dll VoiceRegionData {
        bool deprecated{ false };///< Whether this is a deprecated voice region(avoid switching to these).
        std::string name{ "" };///< Name of the region.
        bool optimal{ false };///< True for a single server that is closest to the current User's client.
        bool custom{ false };///< Whether this is a custom voice region(used for events / etc).
        std::string id{ "" };///< Unique ID for the region.
    };

    /// Message activity types. \brief Message activity types.
    enum class MessageActivityType {
        Join = 1,///< Join.
        Spectate = 2,///< Spectate.
        Listen = 3,///< Listen.
        Join_Request = 5///< Join-request.
    };

    /// Message activity data. \brief Message activity data.
    struct DiscordCoreAPI_Dll MessageActivityData {
        MessageActivityType type{ MessageActivityType::Join };///< Message activity type.
        std::string partyId{ "" };///< Party id.
    };

    /// Ban data. \brief Ban data.
    struct DiscordCoreAPI_Dll BanData {
        bool failedDueToPerms{ false };///< Failed due to perms?
        std::string reason{ "" };///< Reason for the ban.
        UserData user{};///< User that was banned.
    };

    /// Team members object data. \brief Team members object data.
    struct DiscordCoreAPI_Dll TeamMembersObjectData {
        std::vector<Permissions> permissions{};///< Permissions for the team.
        int32_t membershipState{ 0 };///< Current state.
        std::string teamId{ "" };///< Id of the current team.
        UserData user{};///< User data of the current User.
    };

    /// For updating the current voice state. \brief For updating the current voice state.
    struct DiscordCoreAPI_Dll UpdateVoiceStateData {
        std::string channelId{ "" };///< Id of the desired voice Channel. Leave blank to disconnect.
        std::string guildId{ "" };///< The id of the Guild fo which we would like to establish a voice connection.
        bool selfMute{ false };///< Whether or not we self-mute ourselves.
        bool selfDeaf{ false };///< Whether or not we self-deafen ourselves.
    };

    /// Team object data. \brief Team object data.
    class DiscordCoreAPI_Dll TeamObjectData : public DiscordEntity {
    public:
        std::vector<TeamMembersObjectData> members{};///< Array of team members object data.
        std::string ownerUserId{ "" };///< User id of the team owner.
        std::string icon{ "" };///<Icon for the team.

        virtual ~TeamObjectData() {};
    };

    /// Application data. \brief Application data.
    class DiscordCoreAPI_Dll ApplicationData : public DiscordEntity {
    public:
        std::vector<std::string> rpcOrigins{};///< Array of RPC origin strings.
        std::string termsOfServiceUrl{ "" };///< Terms of service Url.
        std::string privacyPolicyUrl{ "" };///< Privacy policy Url.
        bool botRequireCodeGrant{ false };///< Does the bot require a code grant?
        std::string primarySkuId{ "" };///< Primary SKU Id.
        std::string description{ "" };///< Description of the application.
        std::string coverImage{ "" };///< The cover image.
        std::string verifyKey{ "" };///< The verification key.
        std::string  summary{ "" };///< Summary of the application.
        std::string guildId{ "" };///< Guild id.
        bool botPublic{ false };///< Is the bot public?
        std::string slug{ "" };///< Sluhg.
        std::string name{ "" };///< Application's name.
        std::string icon{ "" };///< Application's icon.
        TeamObjectData team{};///< Team object data.
        int32_t flags{ 0 };///< Application flags.
        UserData owner{};///< Application's owner.
        

        virtual ~ApplicationData() {};
    };

    /// Authorization info structure. \brief Authorization info structure.
    struct DiscordCoreAPI_Dll AuthorizationInfoData {
        std::vector<std::string> scopes{};///< Array of strings - the scopes the User has authorized the application for.
        ApplicationData application{};///< Partial application object the current application.
        std::string expires{ "" };///< When the access token expires.
        UserData user{};/// The User who has authorized, if the User has authorized with the identify scope.
    };

    /// Account data. \brief Account data.
    class DiscordCoreAPI_Dll AccountData : public DiscordEntity {
    public:
        std::string name{ "" };///< Name of the account.
        virtual ~AccountData() {}
    };

    /// Guild Widget Data. \brief Guild Widget Data.
    struct DiscordCoreAPI_Dll GuildWidgetData {
        std::string channelId{ "" };///< The widget Channel id.
        bool enabled{ false };///< Whether the widget is enabled.
    };

    /// Widget style options. \brief Widget style options.
    enum class WidgetStyleOptions {
        Shield = 0,///< Shield
        Banner1 = 1,///< Banner1
        Banner2 = 2,///< Banner2
        Banner3 = 3,///< Banner3
        Banner4 = 4///< Banner4
    };

    /// Guild widget image data. \brief Guild widget image data.
    struct DiscordCoreAPI_Dll GuildWidgetImageData {
        std::string url{ "" };
    };

    /// Integration data. \brief Integration data.
    class DiscordCoreAPI_Dll IntegrationData : public DiscordEntity {
    public:
        int32_t expireGracePeriod{ 0 };///< How int64_t before the integration expires.
        ApplicationData application{};///<Application data.
        int32_t subscriberCount{ 0 };///< Number of current subscribers.
        bool enableEmoticons{ true };///<Emoticons enabled?
        int32_t expireBehavior{ 0 };///< What to do upon expiry.
        TimeStamp syncedAt{ "" };///< Time it was last synced at.
        std::string roleId{ "" };///< Role Id.
        std::string name{ "" };///< Name of the integration.
        std::string type{ "" };///< Type of integration.
        bool enabled{ false };///< Enabled?
        bool syncing{ false };///< Is it syncing?
        AccountData account{};///< Account data.
        bool revoked{ false };///< Has it been revoked?
        UserData user{};///< User data for the integration.

        virtual ~IntegrationData() {}
    };

    /// Audit log events. \brief Audit log events.
    enum class AuditLogEvent {
        Guild_Update = 1,
        Channel_Create = 10,
        Channel_Update = 11,
        Channel_Delete = 12,
        Channel_Overwrite_Create = 13,
        Channel_Overwrite_Update = 14,
        Channel_Overwrite_Delete = 15,
        Member_Kick = 20,
        Member_Prune = 21,
        Member_Ban_Add = 22,
        Member_Ban_Remove = 23,
        Member_Update = 24,
        Member_Role_Update = 25,
        Member_Move = 26,
        Member_Disconnect = 27,
        Bot_Add = 28,
        Role_Create = 30,
        Role_Update = 31,
        Role_Delete = 32,
        Invite_Create = 40,
        Invite_Update = 41,
        Invite_Delete = 42,
        Webhook_Create = 50,
        Webhook_Update = 51,
        Webhook_Delete = 52,
        Emoji_Create = 60,
        Emoji_Update = 61,
        Emoji_Delete = 62,
        Message_Delete = 72,
        Message_Bulk_Delete = 73,
        Message_Pin = 74,
        Message_Unpin = 75,
        Integration_Create = 80,
        Integration_Update = 81,
        Integration_Delete = 82,
        Stage_Instance_Create = 83,
        Stage_Instance_Update = 84,
        Stage_Instance_Delete = 85,
        Sticker_Create = 90,
        Sticker_Update = 91,
        Sticker_Delete = 92,
        Guild_Scheduled_Event_Create = 100,
        Guild_Scheduled_Event_Update = 101,
        Guild_Scheduled_Event_Delete = 102,
        Thread_Create = 110,
        Thread_Update = 111,
        Thread_Delete = 112
    };

    /// Audit log entry info data \brief Audit log entry info data.
    class DiscordCoreAPI_Dll AuditLogEntryInfoData : public DiscordEntity {
    public:
        std::string deleteMemberDays{ "" };///<Number of days for which the member's Messages were deleted.
        std::string membersRemoved{ "" };///< Number of members that were removed upon a prune.
        std::string channelId{ "" };///< Channel Id.
        std::string messageId{ "" };///< Message Id.
        std::string roleName{ "" };///< Role name.
        std::string count{ "" };///<Count.
        std::string type{ "" };///< Type.

        virtual ~AuditLogEntryInfoData() {}
    };

    /// Audit log change data. \brief Audit log change data.
    struct DiscordCoreAPI_Dll AuditLogChangeData {
        std::string newValueString{ "" };///< New value, if it's a std::string.
        std::string oldValueString{ "" };///< Old value, if it's a std::string.
        bool newValueBool{ false };///< New value, if it's a bool.
        bool oldValueBool{ false };///< Old value, if it's a bool.
        int32_t newValueInt{ 0 };///< New value, if it's an int32_t.
        int32_t oldValueInt{ 0 };///< Old value, if it's an int32_t.
        std::string key{ "" };///< The key of the audit log change.
    };

    /// Guild prune count data. \brief Guild prune count data.
    struct DiscordCoreAPI_Dll GuildPruneCountData {
        int32_t count{ 0 };
    };

    /// Audit log entry data. \brief Audit log entry data.
    class DiscordCoreAPI_Dll AuditLogEntryData : public DiscordEntity {
    public:
        std::vector<AuditLogChangeData> changes{};///< Array of audit log change data.
        std::string createdTimeStamp{ "" };///< Time at which this entry was created.
        AuditLogEntryInfoData options{};///< Audit log entry info data.
        AuditLogEvent actionType{};///< Audit log action type.
        std::string targetId{ "" };///< Id of the target User.
        std::string userId{ "" };///< Id of the executing User.
        std::string reason{ "" };///< The reason that was entered for the given change.

        virtual ~AuditLogEntryData() {}
    };

    /// Party data. \brief Party data.
    class DiscordCoreAPI_Dll PartyData : public DiscordEntity {
    public:
        std::vector<int32_t> size{ 0, 0 };///< The size of the party.

        virtual ~PartyData() {}
    };

    /// Assets data. \brief Party data.
    struct DiscordCoreAPI_Dll AssetsData {
        std::string largeImage{ "" };///< Keyname of an asset to display.
        std::string smallImage{ "" };///< Keyname of an asset to display.
        std::string largeText{ "" };///< Hover text for the large image.
        std::string smallText{ "" };///< Hover text for the small image.
    };

    /// Secrets data. \brief Secrets data.
    struct DiscordCoreAPI_Dll SecretsData {
        std::string spectate{ "" };///< Unique hash for the given match context.
        std::string match{ "" };	///< Unique hash for Spectate button.
        std::string join{ "" };	///< Unique hash for chat invitesand Ask to Join.
    };

    /// Timestamp data. \brief Timestamp data.
    struct DiscordCoreAPI_Dll TimestampData {
        int64_t start{ 0 };///< Unix timestamp - Send this to have an "elapsed" timer.
        int64_t end{ 0 };///< Unix timestamp - send this to have a "remaining" timer.
    };

    /// Button data. \brief Button data.
    struct DiscordCoreAPI_Dll ButtonData {
        std::string label{ "" };///< Visible label of the button.
        std::string url{ "" };///< Url to display on the button.
    };

    /// Activity types. \brief Activity types.
    enum class ActivityType {
        Game = 0,///< Game.
        Streaming = 1,///< Streaming.
        Listening = 2,///< Listening.
        Watching = 3,///< Watching.
        Custom = 4,///< Custom.
        Competing = 5///< Competing.
    };

    /// Activity data. \brief Activity data.
    struct DiscordCoreAPI_Dll ActivityData {
        std::string applicationId{ "" };///< Application id for the current application.
        TimestampData timestamps{};///< Timestamp data.
        std::string details{ "" };///< Details about the activity.
        std::string state{ "" };///< The player's current party status.
        int32_t createdAt{ 0 };///< Timestamp of when the activity began.
        bool instance{ false };///< Whether this activity is an instanced context, like a match.
        std::string name{ "" };///< Name of the activity.
        std::string url{ "" };///< Url associated with the activity.
        SecretsData secrets{};///< Secrets data.
        ButtonData buttons{};///< Button Data.
        ActivityType type{};///< Activity data.
        AssetsData assets{};///< Assets data.
        int32_t flags{ 0 };///< Flags.
        EmojiData emoji{};///< Emoji associated with the activity.
        PartyData party{};///< Party data.
    };

    /// Client status data. \brief Client status data.
    struct DiscordCoreAPI_Dll ClientStatusData {
        std::string desktop{ "" };///< Desktop name.
        std::string mobile{ "" };///< Mobile name.
        std::string web{ "" };///< Web link.
    };

    /// Premium tier levels. \brief Premium tier levels.
    enum class PremiumTier {
        None = 0,///< None.
        Tier_1 = 1,///< Tier 1.
        Tier_2 = 2,///< Tier 2.
        Tier_3 = 3///< Tier 3.
    };

    /// Default Message notification levels. \brief Default Message notification levels.
    enum class DefaultMessageNotificationLevel {
        All_Messages = 0,///< All messages.
        Only_Mentions = 1///< Only mentions.
    };

    /// Explicit content filter levels. \brief Explicit content filter levels.
    enum class ExplicitContentFilterLevel {
        Disabled = 0,///< Disabled.
        Members_Without_Roles = 1,///< Members without roles.
        All_Members = 2///< All members.
    };

    /// MFA levels. \brief MFA levels.
    enum class MFALevel {
        None = 0,///< None.
        Elevated = 1///< Elevated.
    };

    /// Verification levels. \brief /// Verification levels.
    enum class VerificationLevel {
        None = 0,///< None.
        Low = 1,///< Low.
        Medium = 2,///< Medium.
        High = 3,///< High.
        Very_High = 4///< Very high.
    };

    /// Welcome screen Channel data. \brief Welcome screen Channel data.
    struct DiscordCoreAPI_Dll WelcomeScreenChannelData {
        std::string description{ "" };///< Description of the welcome Channel.
        std::string channelId{ "" };///< Id of the welcome Channel.
        std::string emojiName{ "" };///< Emoji name for the Channel.
        std::string emojiId{ "" };///< Emoji id for the Channel.
    };

    /// Welcome screen data. \brief Welcome screen data.
    struct DiscordCoreAPI_Dll WelcomeScreenData {
        std::vector<WelcomeScreenChannelData> welcomeChannels{};///< Welcome screen Channel data.
        std::string description{ "" };///< Description of the welcome screen.
    };

    /// Presence update data. \brief Presence update data.
    struct DiscordCoreAPI_Dll PresenceUpdateData {
        std::vector<ActivityData> activities{};///< Array of activities.
        ClientStatusData clientStatus{};///< Current client status.
        std::string guildId{ "" };///< Guild id for the current presence.
        std::string status{ "" };///< Status of the current presence.
        UserData user{};///< User data for the current presence.
    };

    /// Stage instance privacy levels. \brief Stage instance privacy levels.
    enum class StageInstancePrivacyLevel {
        Public = 1,///< Public.
        Guild_Only = 2///< Guild only.
    };

    /// Stage instance data. \brief Stage instance data.
    class DiscordCoreAPI_Dll StageInstanceData : public DiscordEntity {
    public:
        StageInstancePrivacyLevel privacyLevel{ 0 };///< Privacy level of the Channel.
        bool discoverableDisabled{ false };///< Is it discoverable?
        std::string channelId{ "" };///< The Channel's id.
        std::string guildId{ "" };///< The Guild id for which the Channel exists in.
        std::string topic{ "" };///< The topic of the StageInstance.

        virtual ~StageInstanceData() {}
    };

    /// Sticker types. \brief Sticker types.
    enum class StickerType {
        Standard = 1,///< Standard.
        Guild = 2///< Guild.
    };

    /// Data representing a single Sticker. \brief Data representing a single Sticker.
    class DiscordCoreAPI_Dll StickerData : public DiscordEntity {
    public:
        StickerFormatType formatType{};///< Format type.
        std::string description{ "" };///< Description of the Sticker.
        std::string guildId{ "" };///< The Guild id for which the Sticker exists in.
        std::string packId{ "" };///< Pack id of the Sticker.
        bool available{ false };///< Is it available for use?
        std::string asset{ "" };///< Asset value for the Sticker.
        int32_t nsfwLevel{ 0 };///< NSFW warning level.
        int32_t sortValue{ 0 };///< Where in the stack of stickers it resides.
        std::string name{ "" };///< The Sticker's name.
        std::string tags{ "" };///< Tags for the Sticker to use.
        StickerType type{};///< The type of Sticker.
        UserData user{};///< The User that uploaded the Guild Sticker.

        virtual ~StickerData() {};
    };

    /// Data representing a single Guild preview. \brief Data representing a single Guild preview.
    struct DiscordCoreAPI_Dll GuildPreviewData {
        int32_t approximatePresenceCount{ 0 };
        std::vector<std::string> features{};
        int32_t approximateMemberCount{ 0 };
        std::string discoverySplash{ "" };
        std::vector<EmojiData> emojis{};
        std::string description{ "" };
        std::string splash{ "" };
        std::string name{ "" };
        std::string icon{ "" };
        std::string id{ "" };
    };

    /// Afk timeout durations. \brief Afk timeout durations.
    enum class AfkTimeOutDurations {
        Shortest = 60,///< Shortest.
        Short = 300,///< Short.
        Medium = 900,///< Medium.
        Long = 1800,///< Long.
        Longest = 3600///< Longest.
    };

    /// Data structure representing a single Guild. \brief Data structure representing a single Guild.
    class DiscordCoreAPI_Dll GuildData : public DiscordEntity {
    public:
        std::unordered_map<std::string, PresenceUpdateData> presences{};///< Array of presences for each GuildMember.
        DefaultMessageNotificationLevel defaultMessageNotifications{};///<Default Message notification level.
        std::unordered_map<std::string, VoiceStateData> voiceStates{};///< Array of Guild-member voice-states.
        std::unordered_map<std::string, GuildMemberData> members{};///< Array of GuildMembers.
        std::unordered_map<std::string, StickerData> stickers{};///< Array of Guild stickers.
        std::unordered_map<std::string, ChannelData> channels{};///< Array of Guild channels.
        std::unordered_map<std::string, ChannelData> threads{};///< Array of Guild threads.
        std::unordered_map<std::string, EmojiData> emoji{};///< Array of Guild emojis.
        ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filtering level, by default.
        std::unordered_map<std::string, RoleData> roles{};///< Array of Guild roles.
        std::vector<StageInstanceData> stageInstances{};///< Array of stage instances.
        std::string publicUpdatesChannelId{ "" };///< Id of the public updates Channel.
        int32_t premiumSubscriptionCount{ 0 };///< Premium subscription count.
        int32_t approximatePresenceCount{ 0 };///< Approximate quantity of presences.
        VerificationLevel verificationLevel{};///< Verification level required.
        std::vector<std::string> features{};///< List of Guild features.
        int32_t approximateMemberCount{ 0 };///< Approximate member count.
        std::string discoverySplash{ "" };///< Link to the discovery image's splash.
        std::string preferredLocale{ "" };///< Preferred locale, for voice chat servers.
        std::string widgetChannelId{ "" };///< Channel id for the Guild's widget.
        std::string systemChannelId{ "" };///< Channel id for the Guild's system Channel.
        WelcomeScreenData welcomeScreen{};///< Welcome screen for the Guild.
        int32_t maxVideoChannelUsers{ 0 };///< Maximum quantity of users per video Channel.
        std::string rulesChannelId{ "" };///< Channel id for the Guild's rules Channel.
        AfkTimeOutDurations afkTimeOut{};///< Time for an individual to time out as afk.
        std::string vanityUrlCode{ "" };///< Vanity Url code, if applicable.
        std::string applicationId{ "" };///< The current application id.
        int32_t systemChannelFlags{ 0 };///< System Channel flags.
        std::string afkChannelId{ "" };///< Channel if of the "afk" Channel,
        Permissions permissions{ "" };///< Current Permissions for the bot in the Guild.
        std::string description{ "" };///< Description of the Guild.
        std::string createdAt{ "" };///< When was the Guild created?
        bool widgetEnabled{ false };///< Is the widget enabled?
        std::string iconHash{ "" };///< Url to the Guild's icon.
        bool unavailable{ false };///< Is the Guild currently available to the bot?
        PremiumTier premiumTier{};///< What is the premium tier?
        int32_t maxPresences{ 0 };///< Max number of presences allowed.
        std::string ownerId{ "" };///< User id of the Guild's owner.
        TimeStamp joinedAt{ "" };///< When the bot joined this Guild.        
        int32_t memberCount{ 0 };///< Member count.
        std::string region{ "" };///< Region of the world where the Guild's servers are.
        std::string splash{ "" };///< Url to the Guild's splash.
        std::string banner{ "" };///< Url to the Guild's banner.       
        int32_t maxMembers{ 0 };///< Max quantity of members
        int32_t nsfwLevel{ 0 };///< NSFW warning level.
        std::string icon{ "" };///< Url to the Guild's icon.
        std::string name{ "" };///< The Guild's name.
        bool owner{ false };///< Is the bot the owner?        
        MFALevel mfaLevel{};///< MFA level.
        bool large{ false };///< Is this Guild "large"?

        GuildData() {};

        virtual ~GuildData() {};
    };

    /// Guild scheduled event privacy levels. \brief Guild scheduled event privacy levels.
    enum class GuildScheduledEventPrivacyLevel {
        Public = 1,///< Public.
        Guild_Only = 2///< Guild only.
    };

    /// GuildScheduledEventStatus. \brief GuildScheduledEventStatus.
    enum class GuildScheduledEventStatus {
        Scheduled = 1,///< Scheduled.
        Active = 2,///< Active.
        Completed = 3,///< Completed.
        Canceled = 4///< Cancelled.
    };

    /// Guild scheduled event entity types. /brief Guild scheduled event entity types.
    enum class GuildScheduledEventEntityType {
        None = 0,///< None.
        State_Instance = 1,///< Stage instance.
        Voice = 2,///< Voice.
        External = 3///< External.
    };

    /// Guild scheduled event entity metadata. \brief Guild scheduled event entity metadata.
    struct DiscordCoreAPI_Dll GuildScheduledEventMetadata {
        std::string location{ "" };
    };

    /// Data representing a Guild Scheduled Event. \brief Data representing a Guild Scheduled Event.
    class DiscordCoreAPI_Dll GuildScheduledEventData : public DiscordEntity {
    public:
        GuildScheduledEventPrivacyLevel privacyLevel{};///< The privacy level of the scheduled event.
        GuildScheduledEventMetadata entityMetadata{};///< Additional metadata for the Guild scheduled event.
        GuildScheduledEventEntityType entityType{};///< The type of the scheduled event.
        std::string scheduledStartTime{ "" };///< The time the scheduled event will start.
        std::string scheduledEndTime{ "" };///< The time the scheduled event will end, required if entity_type is External.
        GuildScheduledEventStatus status{};///< The status of the scheduled event.
        std::string description{ "" };///< The description of the scheduled event(1 - 1000 characters.
        std::string channelId{ "" };///< The Channel id in which the scheduled event will be hosted, or null if scheduled entity type is External.
        std::string creatorId{ "" };///< The id of the User that created the scheduled event *.
        std::string entityId{ "" };///< The id of an entity associated with a Guild scheduled event.
        std::string guildId{ "" };///< The Guild id which the scheduled event belongs to.
        uint32_t userCount{ 0 };///< The number of users subscribed to the scheduled event.
        std::string name{ "" };///< The name of the scheduled event(1 - 100 characters).
        UserData creator{};///< The User that created the scheduled event.

        virtual ~GuildScheduledEventData() {};
    };

    /// Data representing a single GuildScheduledEventUser. \brief Data representing a single GuildScheduledEventUser.
    struct DiscordCoreAPI_Dll GuildScheduledEventUserData {
        std::string guildScheduledEventId{ "" };///< The scheduled event id which the User subscribed to/
        GuildMemberData member{};///< Guild member data for this User for the Guild which this event belongs to, if any.
        UserData user{};///< User which subscribed to an event.
    };

    /// Invite data. \brief Invite data.
    struct DiscordCoreAPI_Dll InviteData {
        GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
        int32_t approximatePresenceCount{ 0 };///< Approximate presence count.
        ApplicationData targetApplication{};///< Application data.
        int32_t approximateMemberCount{ 0 };///< Approximate member count.
        StageInstanceData stageInstance{};///< Stage instance data.
        std::string guildId{ "" };///< The Guild this invite is for.
        TimeStamp createdAt{ "" };///< Time it was created at.
        TimeStamp expiresAt{ "" };///< When the invite expires.
        int32_t targetType{ 0 };///< Target type.
        bool temporary{ false };///< Is it temporary?
        std::string code{ "" };///< Unique invite code.
        UserData targetUser{};///< Target User of the invite.
        ChannelData channel{};///< Channel data of the Channel that the invite is for.
        int32_t maxUses{ 0 };///< Max number of uses.
        int32_t maxAge{ 0 };///< Maximum age of the invite.
        UserData inviter{};///< The User who created the invite.
        GuildData guild{};///< Guild data of the Channel that the invite is for.
        int32_t uses{ 0 };///< Maximum nunmber of uses.
    };

    /// Represents a Guild Template. \brief Represents a Guild Template.
    struct DiscordCoreAPI_Dll GuildTemplateData {
        GuildData serializedSourceGuild{};///< The Guild snapshot this template contains.
        std::string sourceGuildId{ "" };///< The ID of the Guild this template is based on.
        std::string description{ "" };///< The description for the template.
        std::string creatorId{ "" };///< The ID of the User who created the template.
        std::string createdAt{ "" };///< When this template was created.
        std::string updatedAt{ "" };///< When this template was last synced to the source Guild.
        uint32_t usageCount{ 0 };///< Number of times this template has been used.
        std::string code{ "" };///< The template code(unique ID).
        std::string name{ "" };///< Template name.
        bool isDirty{ false };///< Whether the template has unsynced changes.
        UserData creator{};///< The User who created the template.
    };

    /// Invite target types. \brief Invite target types.
    enum class InviteTargetTypes {
        Stream = 1,///< Stream.
        Embedded_Application = 2///< Embedded application.
    };

    /// WebHook types. \brief WebHook types.
    enum class WebHookType {
        Incoming = 1,///< Incoming.
        Channel_Follower = 2,///< Channel follower.
        Application = 3///< Application.
    };

    /// WebHook data. \brief WebHook data.
    class DiscordCoreAPI_Dll WebHookData : public DiscordEntity {
    public:
        std::string applicationId{ "" };///< Application id.
        ChannelData sourceChannel{};///< Channel for which the WebHook was issued.
        std::string channelId{ "" };///< Channel id for which the WebHook was issued.
        std::string guildId{ "" };///< Guild id for which the WebHook was issued.
        std::string avatar{ "" };///< Avatar of the WebHook.
        GuildData sourceGuild{};///< Source Guild id.
        std::string token{ "" };///< Token of the WebHook.
        std::string name{ "" };///< Name of the WebHook.
        std::string url{ "" };///< Url of the WebHook.
        WebHookType type{ 0 };///< Type of WebHook.
        UserData user{};///< User which create the WebHook.

        virtual ~WebHookData() {};
    };

    /// Audit log data. \brief Audit log data.
    class DiscordCoreAPI_Dll AuditLogData {
    public:
        auto getAuditLogData(std::string userIdOfChanger, AuditLogEvent auditLogType) {
            for (auto& value : this->auditLogEntries) {
                if (value.id == userIdOfChanger && value.actionType == auditLogType) {
                    return value;
                }
            }
            return AuditLogEntryData();
        }
        auto getAuditLogData(AuditLogEvent auditLogType, std::string userIdOfTarget) {
            for (auto& value : this->auditLogEntries) {
                if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
                    return value;
                }
            }
            return AuditLogEntryData();
        }
        std::vector<AuditLogEntryData> auditLogEntries{};///< Array of audit log entries.
        std::vector<IntegrationData> integrations{};///< Integration data.
        std::vector<WebHookData> webhooks{};///<Array of WebHook data.
        std::vector<ChannelData> threads{};///< Array of Channel data.
        std::vector<UserData> users{};///< Array of u
    };

    /// For removing a reaction. \brief For removing a reaction.
    struct DiscordCoreAPI_Dll ReactionRemoveData {
        std::string channelId{ "" };
        std::string messageId{ "" };
        std::string guildId{ "" };
        std::string userId{ "" };
        EmojiData emoji{};
    };

    /// For storing key-Interaction-related values. \brief For storing key-Interaction-related values.
    struct DiscordCoreAPI_Dll InteractionPackageData {
        std::string interactionToken{ "" };
        std::string applicationId{ "" };
        std::string interactionId{ "" };
    };

    /// For storing key-Message-related values. \brief For storing key-Message-related values.
    struct DiscordCoreAPI_Dll MessagePackageData {
        std::string channelId{ "" };
        std::string messageId{ "" };
    };

    /// Data structure representing an ApplicationCommand's option choice. \brief Data structure representing an ApplicationCommand's option choice.
    struct DiscordCoreAPI_Dll ApplicationCommandOptionChoiceData {
        std::string valueString{ "" };///< The value, if the given choice is a std::string.
        float valueFloat{ 0.0f };///< The value, if the given choice is a float.
        std::string name{ "" };///< The name of the current choice.
        int32_t	valueInt{ 0 };///< The value, if the given choice is an int32_t.
    };

    /// Data structure representing an ApplicationCommand's option. \brief Data structure representing an ApplicationCommand's option.
    struct DiscordCoreAPI_Dll ApplicationCommandOptionData {
        std::vector<ApplicationCommandOptionChoiceData>	choices{};///< A std::vector of possible choices for the current ApplicationCommand option.
        std::vector<ApplicationCommandOptionData> options{};///< A std::vector of possible options for the current ApplicationCommand option.
        std::vector<ChannelType> channelTypes{};///< Set when the ApplicationCommand option type is set to Channel.
        ApplicationCommandOptionType type{};///< The type of command option.
        std::string description{ "" };///< A description of the current ApplicationCommand option.
        bool required{ false };///< Whether this option is required to enter the command or not.
        std::string name{ "" };///< Name of the current ApplicationCommand option.
    };

    /// Representing "TypingStart" data. \brief Representing "TypingStart" data.
    struct DiscordCoreAPI_Dll TypingStartData {
        std::string channelId{ "" };
        std::string guildId{ "" };
        std::string userId{ "" };
        GuildMemberData member{};
        int32_t timestamp{ 0 };
    };

    /// YouTube format data. \brief YouTube format data.
    struct DiscordCoreAPI_Dll YouTubeFormat {
        std::string signatureCipher{ "" };
        std::string audioSampleRate{ "" };
        std::string audioQuality{ "" };
        std::string downloadUrl{ "" };
        std::string signature{ "" };
        int32_t averageBitrate{ 0 };
        std::string mimeType{ "" };        
        int32_t contentLength{ 0 };
        bool doWeGetSaved{ false };
        std::string quality{ "" };
        std::string codecs{ "" };
        std::string aitags{ "" };
        int32_t bitrate{ 0 };
        int32_t height{ 0 };
        int32_t width{ 0 };
        int32_t itag{ 0 };
        int32_t fps{ 0 };
    };

    /// Application command types. \brief Application command types.
    enum class ApplicationCommandType {
        Chat_Input = 1,///< Chat input.
        User = 2,///< User.
        Message = 3///< Message.
    };

    /// User command Interaction data. \brief User command Interaction data.
    struct DiscordCoreAPI_Dll UserCommandInteractionData {
        std::string targetId{ "" };///< The target User's id.
    };

    /// Message command interacction data. \brief Message command interacction data.
    struct DiscordCoreAPI_Dll MessageCommandInteractionData {
        std::string targetId{ "" };///< The target Message's id.
    };

    /// Component types. \brief Component types.
    enum class ComponentType {
        ActionRow = 1,///< A container for other components.
        Button = 2,///< A button object.
        SelectMenu = 3,///< A select menu for picking from choices.
        TextInput = 4///<A text input object
    };

    /// Component Interaction data. \brief Component Interaction data.
    struct DiscordCoreAPI_Dll ComponentInteractionData {
        std::vector<std::string> values{};///< The values of the components.
        ComponentType componentType{};///< The type of component.
        std::string customId{ "" };///< The custom id of the Interaction entity.
    };

    struct DiscordCoreAPI_Dll ModalInteractionData {
        std::string customIdSmall{ "" };///< The custom id of a particular modal input.
        std::string customId{ "" };///< The custom id of the Interaction entity.
        std::string value{ "" };///< The input value of the modal.
    };

    /// Allowable mentions for a Message. \brief Allowable mentions for a Message.
    struct DiscordCoreAPI_Dll AllowedMentionsData {
        std::vector<std::string> parse{};///< A std::vector of allowed mention types to parse from the content.
        std::vector<std::string> roles{};///< Array of role_ids to mention (Max size of 100)
        std::vector<std::string> users{};///<Array of user_ids to mention (Max size of 100)
        bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).
    };

    /// Interaction types. \brief Interaction types.
    enum class InteractionType {
        Ping=1,///< Ping.
        Application_Command=2,///< Application command.
        Message_Component = 3,///< Message component.
        Application_Command_Autocomplete = 4,///< Application command autocomplete.
        Modal_Submit=5///<Modal submission.
    };

    /// Represents a single selection from a select-menu. \brief Represents a single selection from a select-menu.
    struct DiscordCoreAPI_Dll SelectOptionData {
        std::string description{ "" };///< Description of the select-menu-option.
        std::string label{ "" };///< A visible label for the select-menu-option.
        std::string value{ "" };///< A value for identifying the option.
        bool _default{ false };///<Is it the default option?
        EmojiData emoji{};///< An optional emoji to put on it.
    };

    /// Button styles. \brief Button styles.
    enum class ButtonStyle {
        Primary = 1,///< Primary.
        Success = 3,///< Success.
        Secondary = 2,///< Secondary.
        Danger = 4,///< Danger.
        Link = 5///< Link.
    };

    /// Represents a single Message-component. \brief Represents a single Message-component.
    struct DiscordCoreAPI_Dll ComponentData {
        std::vector<SelectOptionData>options{};///< Aray of select options the choices in the select, max 25.
        std::string placeholder{ "" };///< Custom placeholder text if nothing is selected, max 100 characters.
        std::string customId{ "" };///< A developer-defined identifier for the component, max 100 characters.
        std::string label{ "" };///< The label for this component.
        std::string value{ "" };///< A pre-filled value for this component.
        std::string title{ "" };///< Url, for url types.
        int32_t minValues{ 0 };///< The minimum number of items that must be chosen; default 1, min 0, max 25.
        int32_t maxValues{ 0 };///< The maximum number of items that can be chosen; default 1, max 25.
        bool disabled{ false };///< Whether the component is disabled, default false.
        int32_t minLength{ 0 };///< The minimum input length for a text input.
        int32_t maxLength{ 0 };///< The maximum input length for a text input.
        bool required{ false };///< Whether this component is required to be filled.
        std::string url{ "" };///< Url, for url types.
        ComponentType type{};///< Integer component type.
        EmojiData emoji{};///< Emoji name, id, and animated.
        int32_t style{};///< One of button styles.
    };

    /// Action row data of Message components. \brief Action row data of Message components.
    struct DiscordCoreAPI_Dll ActionRowData {
        std::vector<ComponentData> components{};///< Array of components to make up the action-row.
    };

    /// Interaction callback types. \brief Interaction callback types.
    enum class InteractionCallbackType {
        Pong = 1,///<	ACK a Ping
        ChannelMessageWithSource = 4,///< Respond to an interaction with a message.
        DeferredChannelMessageWithSource = 5,///< ACK an interaction and edit a response later, the user sees a loading state.
        DeferredUpdateMessage = 6,///< For components, ACK an interactionand edit the original message later; the user does not see a loading state.
        UpdateMessage = 7,///< For components, edit the message the component was attached to.
        ApplicationCommandAutocompleteResult = 8,///< Respond to an autocomplete interaction with suggested choices.
        Modal = 9///<	Respond to an interaction with a popup modal.
    };

    /// Interaction ApplicationCommand callback data. \brief Interaction ApplicationCommand callback data.
    struct DiscordCoreAPI_Dll InteractionApplicationCommandCallbackData {
        std::vector<ActionRowData> components{};///< Message components.
        AllowedMentionsData allowedMentions{};///< Allowed mentions data.
        std::vector<std::string> choices{};///< Autocomplete choices(max of 25 choices.
        std::vector<EmbedData> embeds{};///< Message embeds.
        std::string customId{ "" };///< A developer-defined identifier for the component, max 100 characters.
        std::string content{ "" };///< Message content.
        std::string title{ "" };///< The title of the popup modal.
        int32_t flags{ 0 };///< Flags.
        bool tts{ false };///< Is it TTS?
    };

    /// Interaction response data. \brief Interaction response data.
    struct DiscordCoreAPI_Dll InteractionResponseData {
        InteractionApplicationCommandCallbackData data{};///< Interaction ApplicationCommand callback data.
        InteractionCallbackType type{};///< Interaction callback type.
    };

    /// Data structure representing an ApplicationCommand. \brief Data structure representing an ApplicationCommand.
    class DiscordCoreAPI_Dll ApplicationCommandData : public DiscordEntity {
    public:
        std::vector<ApplicationCommandOptionData> options{};///< A std::vector of possible options for the current ApplicationCommand.
        bool defaultPermission{ false };///< Whether or not the default Permission in the Guild is to have access to this command or not.
        std::string applicationId{ "" };///< The current application id.
        std::string description{ "" };///< A description of the current ApplicationCommand.
        ApplicationCommandType type{};///< The type of ApplicationCommand.
        std::string guildId{ "" };///< (Where applicable) a Guild id for which to assign this ApplicationCommand to.
        std::string version{ "" };///< An autoincremented version.
        std::string name{ "" };///< Name of the current ApplicationCommand.

        virtual ~ApplicationCommandData() {}
    };

    /// Function data for repeated functions to be loaded. \brief Function data for repeated functions to be loaded.
    struct DiscordCoreAPI_Dll RepeatedFunctionData {
        std::function<void(DiscordCoreClient*)> function{ nullptr };///< The std::function pointer to be loaded.
        int32_t intervalInMs{ 0 };///< The time interval at which to call the std::function.
        bool repeated{ false };///< Whether or not the std::function is repeating.
    };

    /// Channel mention data. \brief Channel mention data.
    class DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity {
    public:
        std::string guildId{ "" };///< The id of the Guild where it took place.
        std::string name{ "" };///< The name of the Channel that was mentioned.
        ChannelType type{};///< The type of Channel that was mentioned.

        virtual ~ChannelMentionData() {}
    };

    /// Data for when some Channel pins are updated. \brief Data for when some Channel pins are updated.
    struct DiscordCoreAPI_Dll ChannelPinsUpdateEventData {
        TimeStamp lastPinTimestamp{ "" };///< The time of the last pinned Message.
        std::string channelId{ "" };///< The id of the Channel within which the Message was pinned.
        std::string guildId{ "" };///< The id of the Guild within which the Message was pinned.
    };

    /// Data for when threads are synced. \brief Data for when threads are synced.
    struct DiscordCoreAPI_Dll ThreadListSyncData {
        std::vector<ThreadMemberData> members{};///< Array of members that are a part of the Thread.
        std::vector<std::string> channelIds{};///< The parent Channel ids whose threads are being synced. If omitted, then threads were synced for the entire Guild. This array may contain channel_ids that have no active threads as well, so you know to clear that data.
        std::vector<ChannelData> threads{};///< All active threads in the given channels that the current User can access.
        std::string guildId{ "" };///< The id of the Guild for which the threads are being synced.
    };

    /// Represents a Thread-members-update. \brief Represents a Thread-members-update.
    class DiscordCoreAPI_Dll ThreadMembersUpdateData : public DiscordEntity {
    public:
        std::vector<ThreadMemberData> addedMembers{};///< New members added to the Thread.
        std::vector<std::string> removedMemberIds{};///< Members who have been removed.
        std::string guildId{ "" };///< Guild id of the Thread.
        int32_t memberCount{ 0 };///< Number of Guild-members in the Thread.

        virtual ~ThreadMembersUpdateData() {}
    };

    /// Message Interaction data. \brief Message Interaction data.
    class DiscordCoreAPI_Dll MessageInteractionData : public DiscordEntity {
    public:
        InteractionType type{};
        std::string name{ "" };
        UserData user{};

        virtual ~MessageInteractionData() {}
    };

    /// Message types. \brief Message types.
    enum class MessageType {
        Default = 0,///< Default.
        Recipient_Add = 1,///< Recipient add.
        Recipient_Remove = 2,///< Recipient remove.
        Call = 3,///< Call.
        Channel_Name_Change = 4,///< Channel name change.
        Channel_Icon_Change = 5,///< Channel icon change.
        Channel_Pinned_Message = 6,///< Channel pinned Message.
        Guild_Member_Join = 7,///< Guild memeber join.
        User_Premium_Guild_Subscription = 8,///< User premium Guild subscription.
        User_Premium_Guild_Subscription_Tier_1 = 9,///< User premium Guild subscription tier 1.
        User_Premium_Guild_Subscription_Tier_2 = 10,///< User premium Guild subscription tier 2.
        User_Premium_Guild_Subscription_Tier_3 = 11,///< User premium Guild subscription tier 3.
        Channel_Follow_Add = 12,///< Channel follow add.
        Guild_Discovery_Disqualified = 14,///< Guild discovery disqualified.
        Guild_Discovery_Requalified = 15,///< Guild discovery requalified.
        Guild_Discovery_Grace_Period_Initial_Warning = 16,///< Guild discovery grade period initial warning.
        Guild_Discovery_Grace_Period_Final_Warning = 17,///< Guild discovery grade period final warning.
        Thread_Created = 18,///< Thread created.
        Reply = 19,///< Reply.
        Chat_Input_Command = 20,///< Chat input command.
        Thread_Starter_Message = 21,///< Thread starter Message.
        Guild_Invite_Reminder = 22,///< Guild invite reminder.
        Context_Menu_Command = 23///< Context menu command.
    };

    /// Message flags. \brief Message flags.
    enum class MessageFlags {
        Crossposted = 1ull << 0,///< Crossposted.
        Is_Crosspost = 1ull << 1,///< Is crosspost.
        Suppress_Embeds = 1ull << 2,///< Supress embeds.
        Source_Message_Deleted = 1ull << 3,///< Source Message deleted.
        Urgent = 1ull << 4,///< Urgent.
        Has_Thread = 1ull << 5,///< Has Thread.
        Ephemeral = 1ull << 6,///< Ephemeral.
        Loading = 1ull << 7///< Loading.
    };

    /// Sticker item types. \brief Sticker item types.
    enum class StickerItemType {
        Png = 1,///< Png.
        Apng = 2,///< Apng.
        Lottie = 3///< Lottie.
    };

    /// Message Sticker item data. \brief Message Sticker item data.
    class DiscordCoreAPI_Dll StickerItemData : public DiscordEntity {
    public:
        StickerItemType formatType{};///< Message Sticker item type.
        std::string name{ "" };///< The name of the Sticker.

        virtual ~StickerItemData() {}
    };

    /// The core of a Message's data structure. \brief The core of a Message's data structure.
    class DiscordCoreAPI_Dll MessageDataOld : public DiscordEntity {
    public:
        std::vector<ChannelMentionData> mentionChannels{};///< array of Channel mention data.
        std::vector<StickerItemData> stickerItems{};///< Array of Message Sticker item data.
        std::vector<AttachmentData> attachments{};///< Array of attachment data.
        MessageReferenceData messageReference{};///< Message reference data.
        std::vector<std::string> mentionRoles{};///< Vector of "mention roles" ids.
        std::vector<ActionRowData> components{};///< Array of action row data.
        std::vector<ReactionData> reactions{};//< Array of reaction data.
        MessageInteractionData interaction{};///< Message Interaction data.
        std::vector<StickerData> stickers{};///< Array of Message Sticker data.
        std::vector<UserData> mentions{};///< Array of User data, for individual's that were mentioned.
        std::vector<EmbedData> embeds{};///< Array of Message embeds.
        std::string applicationId{ "" };///< Application id.
        TimeStamp editedTimestamp{ "" };///< The time at which it was edited.        
        MessageActivityData activity{};///< Message activity data.
        bool mentionEveryone{ false };///< Does the Message mention everyone?
        ApplicationData application{};///< Application data.
        std::string channelId{ "" };///< The Channel it was sent in.
        std::string webhookId{ "" };///< WebHook id of the Message, if applicable.
        TimeStamp timestamp{ "" };///< The timestamp of when the Message was created.
        std::string guildId{ "" };///< The id of the Guild the Message was sent in.
        std::string content{ "" };///< The Message's content.
        GuildMemberData member{};///< The author's Guild member data.
        std::string nonce{ "" };///< Nonce.
        ChannelData thread{};///< The Thread that the Message was sent in, if applicable.
        bool pinned{ false };///< Is it pinned?        
        int32_t flags{ 0 };///< Flags.
        MessageType type{};///< Message type.
        UserData author{};///< The author's User data.        
        bool tts{ false };///< Is it a text-to-speech Message?

        virtual ~MessageDataOld() {};

    };

    /// Data structure representing a single Message. \brief Data structure representing a single Message.
    class DiscordCoreAPI_Dll MessageData : public MessageDataOld {
    public:
        MessageDataOld referencedMessage{};///< The referenced Message, to reply to.
        std::string requesterId{ "" };///< Requester's id, of who sent this Message.

        virtual ~MessageData() {};
    };

    /// Resolved data. \brief Resolved data.
    struct DiscordCoreAPI_Dll ResolvedData {
        std::unordered_map<std::string, GuildMemberData> members{};///< Map full of GuildMemeberData.
        std::unordered_map<std::string, MessageData> messages{};///< Map full of MessageData.
        std::unordered_map<std::string, ChannelData> channels{};///< Map full of ChannelData.
        std::unordered_map<std::string, UserData> users{};///< Map full of UserData.
        std::unordered_map<std::string, RoleData> roles{};///< Map full of RoleData.
    };

    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption;

    DiscordCoreAPI_Dll std::vector<ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(std::vector<ApplicationCommandInteractionDataOption> originalOptions);

    /// Represents a Sticker pack. \brief Represents a Sticker pack.
    struct DiscordCoreAPI_Dll StickerPackData {
        std::vector<StickerData> stickers{};///< Array of Sticker objects	the stickers in the pack.
        std::string coverStickerId{ "" };///< Id of a Sticker in the pack which is shown as the pack's icon.
        std::string bannerAssetId{ "" };///< Id of the Sticker pack's banner image.
        std::string description{ "" };///< Description of the Sticker pack.
        std::string skuId{ "" };///< Id of the pack's SKU.
        std::string name{ "" };///< Name of the Sticker pack.
        std::string Id{ "" };///< Id of the Sticker pack.
    };

    /// Connection visibility types. \brief Connection visibility types.
    enum class ConnectionVisibilityTypes {
        None = 0,///< None.
        Everyone = 1///< Everyone.
    };

    /// Represents a single User Connection. \brief Represents a single User Connection.
    struct DiscordCoreAPI_Dll ConnectionData {
        std::vector<IntegrationData> integrations{};///< An array of partial server integrations.
        ConnectionVisibilityTypes visibility{};///< Visibility of this connection.
        bool showActivity{ false };///< Whether activities related to this connection will be shown in presence updates.
        bool friendSync{ false };///< Whether friend sync is enabled for this connection.
        bool verified{ false };///< Whether the connection is verified.
        std::string name{ "" };///< The username of the connection account.
        std::string type{ "" };///< The service of the connection(twitch, youtube)
        bool revoked{ false };///< Whether the connection is revoked.
        std::string id{ "" };///< Id of the connection account.
    };

    /// ApplicationCommand Interaction data option. \brief ApplicationCommand Interaction data option.
    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption {
        std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
        ApplicationCommandOptionType type{};///< The type of ApplicationCommand options.
        std::string valueString{ "" };///< The value if it's a std::string.
        bool valueBool{ false };///< the value if it's a bool.
        std::string name{ "" };///< The name of the current option.
        int32_t valueInt{ 0 };///< The value if it's an int32_t.
        bool focused{ false };///< 	True if this option is the currently focused option for autocomplete
    };

    /// ApplicationCommand Interaction data.
    class DiscordCoreAPI_Dll ApplicationCommandInteractionData : public DiscordEntity {
    public:
        std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
        ApplicationCommandType type{};///< The type of ApplicationCommand.
        ResolvedData resolved{};///< Resolved data.
        std::string name{ "" };///< The name of the command.

        virtual ~ApplicationCommandInteractionData() {}
    };

    /// Interaction data data. \brief Interaction data data.
    struct DiscordCoreAPI_Dll InteractionDataData {
        ApplicationCommandInteractionData applicationCommanddata{};///< ApplicationCommand Interaction data.
        MessageCommandInteractionData messageInteractionData{};///< Message command Interaction data.
        UserCommandInteractionData userInteractionData{};///< User command Interaction data.
        ComponentInteractionData componentData{};///< Component Interaction data.
        ModalInteractionData modalData{};///< Modal Interaction data.
    };

    /// Interaction data. \brief Interaction data.
    class DiscordCoreAPI_Dll InteractionData : public DiscordEntity {
    public:
        std::string applicationId{ "" };///< The application's id.
        std::string requesterId{ "" };///< The id of the sender of the Interaction.
        std::string channelId{ "" };///< The Channel the Interaction was sent in.
        InteractionDataData data{};///< The Interaction's data.
        std::string guildId{ "" };///< The Guild id of the Guild it was sent in.
        GuildMemberData member{};///< The data of the Guild member who sent the Interaction, if applicable.
        nlohmann::json rawData{};///< The Interaction's raw data.
        std::string token{ "" };///< The Interaction token.
        InteractionType type{};///< The type of Interaction.
        MessageData message{};///< The Message that the Interaction came through on, if applicable.
        int32_t version{ 0 };///< The Interaction version.
        UserData user{};///< The User data of the sender of the Interaction.
        
        InteractionData() {};
        InteractionData(std::string requesterId) {
            this->requesterId = requesterId;
        }

        virtual ~InteractionData() {}
    };

    /// Data from the SessionStart info. \brief Data from the SessionStart info.
    struct DiscordCoreAPI_Dll SessionStartData {
        uint32_t maxConcurrency{ 0 };///< The number of identify requests allowed per 5 seconds.
        uint32_t resetAfter{ 0 };///< The number of std::chrono::milliseconds after which the limit resets.
        uint32_t remaining{ 0 };///< The remaining number of session starts the current User is allowed.
        uint32_t total{ 0 };///< The total number of session starts the current User is allowed.
    };

    /// Data from the GetGatewatBot endpoint. \brief Data from the GetGatewatBot endpoint.
    struct DiscordCoreAPI_Dll GatewayBotData {
        SessionStartData sessionStartLimit{};///< Information on the current session start limit.
        std::string url{ "" };///< The WSS Url that can be used for connecting to the gateway.
        uint32_t shards{ 0 };///< The recommended number of shards to use when connecting.
    };

    /// Text input style for modals. \brief Text input style for modals.
    enum class TextInputStyle {
        Short = 1,///< A single-line input.
        Paragraph = 2///< A multi-line input.
    };

    /// Input event response types. \brief Input event response types.
    enum class InputEventResponseType {
        Unset = 0,///< Unset.
        Deferred_Response = 1,///< Deferred response.
        Interaction_Response = 2,///< Interaction response.
        Interaction_Response_Edit = 3,///< Interaction response edit.
        Ephemeral_Interaction_Response = 4,///< Ephemeral Interaction response.
        Regular_Message = 5,///< Regular Message.
        Regular_Message_Edit = 6,///< Regular Message edit.
        Follow_Up_Message = 7,///< Follow-up Message.
        Follow_Up_Message_Edit = 8,///< Follow-up Message edit.
        Ephemeral_Follow_Up_Message = 9,///< Ephemeral follow-up Message.
        Send_Dm = 10///< Send Dm.
    };

    /// Input event types. \brief Input event types.
    enum class InputEventType {
        Unset = 0,///< Unset.
        Application_Command_Interaction = 1,///< Slash-command Interaction.
        Button_Interaction = 2,///< Button Interaction.
        Regular_Message = 3,///< Regular Message.
        Select_Menu_Interaction = 4,///< Select-menu Interaction.
        Message_Command_Interaction = 5,///< Message-command Interaction.
        User_Command_Interaction = 6,///< User-command Interaction.
        Modal_Interaction = 7///< Modal Interaction.
    };

    /// Data representing a Guild Emoji Update event. \brief Data representing a Guild Emoji Update event.
    struct DiscordCoreAPI_Dll GuildEmojisUpdateEventData {
        std::vector<EmojiData> emojis{};
        std::string guildId{ "" };
    };

    /// Data representing a Guild Sticker Update event. \brief Data representing a Guild Stickers Update event.
    struct DiscordCoreAPI_Dll GuildStickersUpdateEventData {
        std::vector<StickerData> stickers{};
        std::string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GuildMembersChunkEventData {
        std::vector<PresenceUpdateData> presences{};
        std::vector<GuildMemberData> members{};
        std::vector<std::string> notFound{};
        std::string guildId{ "" };
        int32_t chunkIndex{ 0 };
        int32_t chunkCount{ 0 };
        std::string nonce{ "" };
    };

    /// Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input. \brief Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input.
    class DiscordCoreAPI_Dll InputEventData {
    public:

        friend RecurseThroughMessagePagesData;
        friend OnInteractionCreationData;
        friend RespondToInputEventData;
        friend BaseFunctionArguments;
        friend DiscordCoreClient;
        friend EventHandler;
        friend CommandData;
        friend InputEvents;

        InputEventResponseType responseType{};///< The type of response that this input value represents.
        InputEventType eventType{};///< The type of input-event that is represented by this structure.

        InputEventData() {};

        InputEventData(MessageData messageData, InteractionData interactionData, InputEventType eventType) {
            this->interactionData = interactionData;
            this->messageData = messageData;
            this->eventType = eventType;
            if (this->messageData.channelId == "") {
                this->messageData.channelId = this->interactionData.channelId;
            }
            else {
                this->interactionData.channelId = this->messageData.channelId;
            }
            if (this->interactionData.id == "") {
                this->interactionData.id = this->messageData.interaction.id;
            }
            if (this->messageData.guildId == "") {
                this->messageData.guildId = this->interactionData.guildId;
            }
            else {
                this->interactionData.guildId = this->messageData.guildId;
            }
            if (this->messageData.id == "") {
                this->messageData.id = this->interactionData.message.id;
            }
            else {
                this->interactionData.message.id = this->messageData.id;
            }
            if (this->messageData.member.user.id == "") {
                this->messageData.member = this->interactionData.member;
            }
            else {
                this->interactionData.member = this->messageData.member;
            }
            if (this->interactionData.user.id != "") {
                this->messageData.author = this->interactionData.user;
            }
            else {
                this->interactionData.user = this->messageData.author;
            }
            if (this->messageData.requesterId != "") {
                this->requesterId = this->messageData.requesterId;
            }
            else {
                this->requesterId = this->interactionData.requesterId;
            }
        }

        /// Returns the username of the last User to trigger this input-event. \brief Returns the username of the last User to trigger this input-event.
        /// \returns A std::string containing the User name.
        std::string getUserName() {
            if (this->messageData.author.userName == "" && this->interactionData.member.user.userName != "") {
                return this->interactionData.member.user.userName;
            }
            else if (this->interactionData.member.user.userName == "" && this->interactionData.user.userName != "") {
                return this->interactionData.user.userName;
            }
            else if (this->messageData.author.userName != "") {
                return this->messageData.author.userName;
            }
            else {
                return std::string();
            }
        }

        /// Gets the avatar Url of the last User to trigger this input-event. \brief Gets the avatar Url of the last User to trigger this input-event.
        /// \returns A std::string containing the avatar Url.
        std::string getAvatarUrl() {
            if (this->messageData.author.avatar == "" && this->interactionData.member.user.avatar != "") {
                return this->interactionData.member.user.avatar;
            }
            else if (this->interactionData.member.user.avatar == "" && this->interactionData.user.avatar != "") {
                return this->interactionData.user.avatar;
            }
            else if (this->messageData.author.avatar != "") {
                return this->messageData.author.avatar;
            }
            else {
                return std::string();
            }
        }

        /// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
        /// \returns A std::vector containing the EmbedData.
        std::vector<EmbedData> getEmbeds() {
            if (this->interactionData.message.embeds.size() > 0) {
                if (this->interactionData.message.embeds.at(0).description != "") {
                    return this->interactionData.message.embeds;
                }
                else if (this->interactionData.message.embeds.at(0).fields.size() > 0) {
                    if (this->interactionData.message.embeds.at(0).fields.at(0).value != "") {
                        return this->interactionData.message.embeds;
                    }
                }
            }
            else {
                return this->messageData.embeds;
            }
            return std::vector<EmbedData>();
        }

        /// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
        /// \returns A std::vector containing ActionRowData.
        std::vector<ActionRowData> getComponents() {
            if (this->interactionData.message.components.size() > 0) {
                if (this->interactionData.message.components.at(0).components.at(0).customId != "") {
                    return this->interactionData.message.components;
                }
            }
            else {
                return this->messageData.components;
            }
            return std::vector<ActionRowData>();
        }

        /// Returns the User id of the last requester of this input-event. \brief Returns the User id of the last requester of this input-event.
        /// \returns A std::string containing the author's id.
        std::string getAuthorId() {
            if (this->messageData.author.id == "") {
                return this->interactionData.user.id;
            }
            else if (this->messageData.author.id != "") {
                return this->messageData.author.id;
            }
            else {
                return std::string();
            }
        }

        /// Returns the Interaction id, if appplicable, of this input-event. \brief Returns the Interaction id, if appplicable, of this input-event.
        /// \returns A std::string containing the Interaction id.
        std::string getInteractionId() {
            if (this->interactionData.id == "") {
                return this->messageData.interaction.id;
            }
            else if (this->interactionData.id != "") {
                return this->interactionData.id;
            }
            else if (this->interactionData.message.interaction.id != "") {
                return this->interactionData.message.interaction.id;
            }
            else {
                return std::string();
            }
        }

        /// Returns the application id. \brief Returns the application id.
        /// \returns A std::string containing the application id.
        std::string getApplicationId() {
            if (this->interactionData.applicationId == "") {
                return this->messageData.application.id;
            }
            else if (this->interactionData.applicationId != "") {
                return this->interactionData.applicationId;
            }
            else {
                return std::string();
            }
        }

        /// Returns the Channel id of this input-event. \brief Returns the Channel id of this input-event.
        /// \returns A std::string containing the Channel id.
        std::string getChannelId() {
            if (this->interactionData.channelId == "") {
                return this->messageData.channelId;
            }
            else if (this->interactionData.channelId != "") {
                return this->interactionData.channelId;
            }
            else {
                return std::string();
            }
        }

        /// Returns the Interaction token, if applicable, of this input-event. \brief Returns the Interaction token, if applicable, of this input-event.
        /// \returns A std::string containing the Interaction token.
        std::string getInteractionToken() {
            if (this->interactionData.token != "") {
                return this->interactionData.token;
            }
            else {
                return std::string();
            }
        }

        /// Returns the Guild id, of this input-event. \brief Returns the Guild id, of this input-event.
        /// \returns A std::string containing the Guild id.
        std::string getGuildId() {
            if (this->messageData.guildId == "") {
                return this->interactionData.guildId;
            }
            else if (this->messageData.guildId != "") {
                return this->messageData.guildId;
            }
            else {
                return std::string();
            }
        }

        /// Returns the Message content, if applicable, of this input-event. \brief Returns the Message content, if applicable, of this input-event.
        /// \returns A std::string containing the Message's content.
        std::string getMessageContent() {
            if (this->interactionData.message.content != "") {
                return this->interactionData.message.content;
            }
            else {
                return this->messageData.content;
            }
        }

        /// Returns the Message id, if applicable, of this input-event. \brief Returns the Message id, if applicable, of this input-event.
        /// \returns A std::string containing the Message id.
        std::string getMessageId() {
            if (this->messageData.id != "") {
                return this->messageData.id;

            }
            else if (this->interactionData.message.id != "") {
                return this->interactionData.message.id;
            }
            else {
                return std::string();
            }
        }

        /// Returns the Interaction data, if applicable, of this input-event. \brief Returns the InteractionData, if applicable, of this input-event.
        /// \returns An InteractionData structure.
        InteractionData getInteractionData() {
            return this->interactionData;
        }

        /// Returns the Message data, if applicable, of this input-event. \brief Returns the Message data, if applicable, of this input-event.
        /// \returns A MessageData structure.
        MessageData getMessageData() {
            if (this->messageData.id != "") {
                return this->messageData;

            }
            else {
                return this->interactionData.message;
            }
        }

        /// Returns the requester id, of this input-event. \brief Returns the requester id, of this input-event.
        /// \returns A std::string containing the requester's id.
        std::string getRequesterId() {
            return this->requesterId;
        };

        virtual ~InputEventData() {};

    protected:
        InteractionData interactionData{};
        std::string requesterId{ "" };
        MessageData messageData{};
    };

    /// \brief Data for responding to an input-event.
    class DiscordCoreAPI_Dll RespondToInputEventData {
    public:

        friend CreateEphemeralInteractionResponseData;
        friend CreateDeferredInteractionResponseData;
        friend CreateEphemeralFollowUpMessageData;
        friend DeleteInteractionResponseData;
        friend CreateInteractionResponseData;
        friend EditInteractionResponseData;
        friend DeferComponentResponseData;
        friend DeleteFollowUpMessageData;
        friend CreateFollowUpMessageData;
        friend EditFollowUpMessageData;
        friend CreateMessageData;
        friend EditMessageData;
        friend InputEvents;
        friend SendDMData;

        InputEventResponseType type{};///< The type of response to make.

        RespondToInputEventData(std::string channelIdNew) {
            this->channelId = channelIdNew;
            this->type = InputEventResponseType::Regular_Message;
        }

        RespondToInputEventData(InteractionData dataPackage) {
            this->type = InputEventResponseType::Interaction_Response;
            this->applicationId = dataPackage.applicationId;
            this->requesterId = dataPackage.requesterId;
            this->interactionToken = dataPackage.token;
            this->messageId = dataPackage.message.id;
            this->channelId = dataPackage.channelId;
            this->interactionId = dataPackage.id;
            this->eventType = dataPackage.type;
        };

        RespondToInputEventData(InputEventData dataPackage) {
            this->interactionToken = dataPackage.getInteractionToken();
            this->applicationId = dataPackage.getApplicationId();
            this->interactionId = dataPackage.getInteractionId();
            this->requesterId = dataPackage.getRequesterId();
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->type = InputEventResponseType::Interaction_Response;
        }

        /// Adds a button to the response Message. \brief Adds a button to the response Message.
        /// \param disabled Whether the button is active or not.
        /// \param customIdNew A custom id to give for identifying the button.
        /// \param buttonLabel A visible label for the button.
        /// \param buttonStyle The style of the button.
        /// \param emojiName An emoji name, if desired.        
        /// \param emojiId An emoji id, if desired.
        /// \param url A url, if applicable.
        RespondToInputEventData& addButton(bool disabled, std::string customIdNew, std::string buttonLabel, ButtonStyle buttonStyle, std::string emojiName = "", std::string emojiId = "", std::string url = "") {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.type = ComponentType::Button;
                    component.emoji.name = emojiName;
                    component.label = buttonLabel;
                    component.style = static_cast<int32_t>(buttonStyle);
                    component.customId = customIdNew;
                    component.disabled = disabled;
                    component.emoji.id = emojiId;
                    component.url = url;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
            return *this;
        }

        /// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
        /// \param disabled Whether the select-menu is active or not.
        /// \param customIdNew A custom id to give for identifying the select-menu.
        /// \param options A std::vector of select-menu-options to offer.
        /// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
        /// \param maxValues Maximum number of selections that are possible.
        /// \param minValues Minimum required number of selections that are required.
        RespondToInputEventData& addSelectMenu(bool disabled, std::string customIdNew, std::vector<SelectOptionData> options, std::string placeholder, int32_t maxValues, int32_t minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    componentData.disabled = disabled;
                    componentData.customId = customIdNew;
                    componentData.options = options;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }

            }
            return *this;
        }

        /// Adds a modal to the response Message. \brief Adds a modal to the response Message.
        /// \param topTitleNew A title for the modal.
        /// \param topCustomIdNew A custom id to give for the modal.
        /// \param titleNew A title for the modal's individual input.
        /// \param customIdNew A custom id to give for the modal's individual input.
        /// \param required Is it a required response?
        /// \param minLength Minimum length.
        /// \param maxLength Maximum length.
        /// \param inputStyle The input style.
        /// \param label A label for the modal.
        /// \param placeholder A placeholder for the modal.
        RespondToInputEventData& addModal(std::string topTitleNew, std::string topCustomIdNew, std::string titleNew, std::string customIdNew, bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, std::string label = "", std::string placeholder = "") {
            this->title = topTitleNew;
            this->customId = topCustomIdNew;
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component{};
                    component.type = ComponentType::TextInput;
                    component.customId = customIdNew;
                    component.style = static_cast<int32_t>(inputStyle);
                    component.title = titleNew;
                    component.maxLength = maxLength;
                    component.minLength = minLength;
                    component.label = label;
                    component.required = required;
                    component.placeholder = placeholder;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
            return *this;
        }

        /// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
        /// \param dataPackage An AllowedMentionsData structure.
        RespondToInputEventData& addAllowedMentions(AllowedMentionsData dataPackage) {
            this->allowedMentions = dataPackage;
            return *this;
        }

        /// For setting the components in a response. \brief For setting the components in a response. 
        /// \param dataPackage An ActionRowData structure.
        RespondToInputEventData& addComponentRow(ActionRowData dataPackage) {
            this->components.push_back(dataPackage);
            return *this;
        }

        /// For setting the embeds in a response. \brief For setting the embeds in a response.
        /// \param dataPackage An EmbedData structure.
        RespondToInputEventData& addMessageEmbed(EmbedData dataPackage) {
            this->embeds.push_back(dataPackage);
            return *this;
        }

        /// For setting the Message content in a response. \brief For setting the Message content in a response.
        /// \param dataPackage A std::string, containing the content.
        RespondToInputEventData& addContent(std::string dataPackage) {
            this->content = dataPackage;
            return *this;
        }

        /// For setting the tts status of a response. \brief For setting the tts status of a response.
        /// \param enabledTTs A bool.
        RespondToInputEventData& setTTSStatus(bool enabledTTs) {
            this->tts = enabledTTs;
            return *this;
        }

        /// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
        /// \param targetUserIdNew A std::string, containging the target User's id.
        RespondToInputEventData& setTargetUserID(std::string targetUserIdNew) {
            this->targetUserId = targetUserIdNew;
            return *this;
        }

    protected:

        std::vector<ActionRowData> components{};
        AllowedMentionsData allowedMentions{};
        std::string interactionToken{ "" };
        std::vector<EmbedData> embeds{};
        std::string interactionId{ "" };
        std::string applicationId{ "" };
        std::string targetUserId{ "" };
        std::string requesterId{ "" };
        InteractionType eventType{};
        std::string channelId{ "" };
        std::string messageId{ "" };
        std::string customId{ "" };
        std::string content{ "" };
        std::string title{ "" };
        int32_t flags{ 0 };
        bool tts{ false };
    };

    struct DiscordCoreAPI_Dll CommandData {
    public:

        std::vector<std::string> optionsArgs{};
        std::string commandName{ "" };
        InputEventData eventData{};

        CommandData() = default;

        CommandData(InputEventData inputEventData) {
            this->eventData = inputEventData;
            if (inputEventData.interactionData.data.applicationCommanddata.name != "") {
                this->commandName = inputEventData.interactionData.data.applicationCommanddata.name;
            }
            if (inputEventData.interactionData.data.messageInteractionData.targetId != "") {
                this->optionsArgs.push_back(inputEventData.interactionData.data.messageInteractionData.targetId);
            }
            else if (inputEventData.interactionData.data.userInteractionData.targetId != "") {
                this->optionsArgs.push_back(inputEventData.interactionData.data.userInteractionData.targetId);
            }
        }
    };

    /// For selecting the caching style of the library. \brief For selecting the caching style of the library.
    struct DiscordCoreAPI_Dll CacheOptions {
        bool cacheGuildMembers{ false };///< Do we cache GuildMembers?
        bool cacheChannels{ false };///< Do we cache Channels?
        bool cacheGuilds{ false };///< Do we cache Guilds?
        bool cacheRoles{ false };///< Do we cache Roles?
        bool cacheUsers{ false };///< Do we cache Users?
    };

    /// Guild application command permissions data. \brief /// Guild application command permissions data.
    class DiscordCoreAPI_Dll GuildApplicationCommandPermissionData : public DiscordEntity {
    public:
        std::vector<ApplicationCommandPermissionData> permissions{};
        std::string applicationId{ "" };
        std::string guildId{ "" };

        virtual ~GuildApplicationCommandPermissionData() {}
    };

    /**@}*/

    /// Song types. \brief Song types.
    enum class SongType {
        YouTube = 0,///< YouTube.
        SoundCloud = 1///< SoundCloud.
    };

    /// Represents a download Url. \brief Represents a download Url.
    struct DiscordCoreAPI_Dll DownloadUrl {
        InputEventData eventData{};
        std::string urlPath{ "" };
        int32_t contentSize{ 0 };
    };

    /**
    * \addtogroup voice_connection
    * @{
    */

    /// Represents a single frame of raw audio data. \brief Represents a single frame of raw audio data.
    struct DiscordCoreAPI_Dll RawFrameData {
        std::vector<uint8_t> data{};///< The audio data.
        int32_t sampleCount{ -1 };///< The number of samples per this frame.
        RawFrameData& operator=(RawFrameData&& other) noexcept {
            if (this != &other) {
                this->sampleCount = other.sampleCount;
                other.sampleCount = -1;
                this->data = std::move(other.data);
                other.data = std::vector<uint8_t>{};
            }
            return *this;
        }
        RawFrameData(RawFrameData&& other) noexcept {
            *this = std::move(other);
        }
        RawFrameData& operator=(const RawFrameData& other) {
            this->sampleCount = other.sampleCount;
            this->data = other.data;
            return *this;
        }
        RawFrameData(const RawFrameData& other) {
            *this = other;
        }
        RawFrameData() {};
    };

    /// Represents a single frame of encoded audio data. \brief Represents a single frame of encoded audio data.
    struct DiscordCoreAPI_Dll EncodedFrameData {
        std::vector<uint8_t> data{};///< The audio data.
        int32_t sampleCount{ -1 };///< The number of samples per this frame.
        EncodedFrameData& operator=(EncodedFrameData&& other) noexcept {
            if (this != &other) {
                this->sampleCount = other.sampleCount;
                other.sampleCount = -1;
                this->data = std::move(other.data);
                other.data = std::vector<uint8_t>{};
            }
            return *this;
        }
        EncodedFrameData(EncodedFrameData&& other) noexcept {
            *this = std::move(other);
        }
        EncodedFrameData& operator=(const EncodedFrameData& other) {
            this->sampleCount = other.sampleCount;
            this->data = other.data;
            return *this;
        }
        EncodedFrameData(const EncodedFrameData& other) {
            *this = other;
        }
        EncodedFrameData() {}
    };

    /// Audio frame types. \brief Audio frame types.
    enum class AudioFrameType {
        Unset = 0,///< Unset.
        Encoded = 1,///< Encoded.
        RawPCM = 2,///< Raw PCM.
        Skip = 3///< Skip.
    };

    /// Represents a single frame of audio data. \brief Represents a single frame of audio data.
    struct DiscordCoreAPI_Dll AudioFrameData {
        AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
        EncodedFrameData encodedFrameData{};///< To be filled if it's already encoded.
        std::string guildMemberId{ "" };///< The Id of the GuildMember from which it was sent.
        RawFrameData rawFrameData{};///< To be filled if it's raw audio data.
        AudioFrameData& operator=(AudioFrameData&& other) noexcept {
            if (this != &other) {
                this->guildMemberId = std::move(other.guildMemberId);
                other.guildMemberId = "";
                this->encodedFrameData = std::move(other.encodedFrameData);
                other.encodedFrameData = EncodedFrameData{};
                this->rawFrameData = std::move(other.rawFrameData);
                other.rawFrameData = RawFrameData{};
                this->type = other.type;
                other.type = AudioFrameType::Unset;
            }
            return *this;
        }
        AudioFrameData(AudioFrameData&& other) noexcept {
            *this = std::move(other);
        }
        AudioFrameData& operator=(const AudioFrameData& other) {
            this->encodedFrameData = other.encodedFrameData;
            this->guildMemberId = other.guildMemberId;
            this->rawFrameData = other.rawFrameData;
            this->type = other.type;
            return *this;
        }
        AudioFrameData(const AudioFrameData& other) {
            *this = other;
        }
        AudioFrameData() {};
    };

    class DiscordCoreAPI_Dll SoundCloudSong;
    class DiscordCoreAPI_Dll YouTubeSong;

    /// A song from the various platforms. \brief A song from the various platforms.
    struct DiscordCoreAPI_Dll Song {
    public:

        friend DiscordCoreInternal::DataParser;
        friend SoundCloudRequestBuilder;
        friend YouTubeRequestBuilder;
        friend SoundCloudSong;
        friend SoundCloudAPI;
        friend YouTubeSong;
        friend YouTubeAPI;
        friend SongAPI;

        SongType type{ SongType::SoundCloud };///< The type of song.

        std::vector<DownloadUrl> finalDownloadUrls{};
        std::string secondDownloadUrl{ "" };
        std::string firstDownloadUrl{ "" };
        std::string html5PlayerFile{ "" };
        std::string addedByUserName{ "" };///< The User name of the individual who added this Song to the playlist.
        std::string addedByUserId{ "" };///< The User id of the individual who added this Song to the playlist.
        std::string thumbnailUrl{ "" };///< The Url of the thumbnail image of this Song.
        std::string html5Player{ "" };
        std::string description{ "" };///< A description of the Song.
        std::string songTitle{ "" };///< The title of the Song.
        std::string duration{ "" };///< The duration of the Song.
        int32_t contentLength{ 0 };
        std::string viewUrl{ "" };///< The url for listening to this Song through a browser.
        std::string songId{ "" };

        virtual ~Song() {};

        operator SoundCloudSong();

        operator YouTubeSong();

    protected:

        std::string trackAuthorization{ "" };
        std::string playerResponse{ "" };
        bool doWeGetSaved{ false };
        YouTubeFormat format{};

    };

    /// Song completion event data. \brief Song completion event data.
    struct DiscordCoreAPI_Dll SongCompletionEventData {
        VoiceConnection* voiceConnection{};///< A pointer to the current VoiceConnection.
        bool wasItAFail{ false };///< Is this a replay? (Did a track recently fail to play?)
        GuildMemberData guildMember{};///< The sending GuildMember.
        Song previousSong{};///< The previously played Song.
        GuildData guild{};///< The sending Guild.
    };

    /// Playlist of songs and other variables. \brief Playlist of songs and other variables.
    struct DiscordCoreAPI_Dll Playlist {
    public:
        bool isLoopSongEnabled{ false };///< Is looping of Songs currently enabled?
        bool isLoopAllEnabled{ false };///< Is looping of the entire Playlist currently enabled?
        std::vector<Song> songQueue{};///< The list of Songs that are stored to be played.
        Song currentSong{};///< The current Song that is playing.
    };

    /// Base arguments for the command classes. \brief Base arguments for the command classes.
    struct DiscordCoreAPI_Dll BaseFunctionArguments {
    public:

        DiscordCoreClient* discordCoreClient{ nullptr };
        std::vector<std::string> argumentsArray{};///< A std::vector of std::string arguments.
        InputEventData eventData{};///< InputEventData representing the input event that triggered the command.

        BaseFunctionArguments() = default;

        BaseFunctionArguments(InputEventData inputEventData, DiscordCoreClient* discordCoreClientNew) {
            this->discordCoreClient = discordCoreClientNew;
            this->eventData = inputEventData;
        }

        virtual ~BaseFunctionArguments() {};
    };

    /// Base class DiscordCoreAPI_Dll for the command classes. \brief Base class DiscordCoreAPI_Dll for the command classes.
    class DiscordCoreAPI_Dll BaseFunction {
    public:
        std::string helpDescription{ "" };///< Description of the command for the Help command.
        std::string commandName{ "" };///< Name of the command for calling purposes.
        EmbedData helpEmbed{};///< A Message embed for displaying the command via the Help command.

        BaseFunction() = default;

        /// The base function for the command's execute function.
        /// \param args A unique_ptr containing a copy of BaseFunctionArguments.
        /// \returns void.
        virtual void execute(std::unique_ptr<BaseFunctionArguments> args) = 0;
        virtual std::unique_ptr<BaseFunction> create() = 0;
        virtual ~BaseFunction() = default;
    };

    /**@}*/

    class DiscordCoreAPI_Dll YouTubeSong : public Song {};

    class DiscordCoreAPI_Dll SoundCloudSong : public Song {};

    /**
    * \addtogroup utilities
    * @{
    */

    struct DiscordCoreAPI_Dll RecurseThroughMessagePagesData {
        InputEventData inputEventData{};
        uint32_t currentPageIndex{};
        std::string buttonId{};
    };

    DiscordCoreAPI_Dll RecurseThroughMessagePagesData recurseThroughMessagePages(std::string userID, std::unique_ptr<InputEventData> originalEvent, uint32_t currentPageIndex, std::vector<EmbedData> messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult = false);
    /**@}*/

};

namespace  DiscordCoreInternal {

    enum class HttpWorkloadClass {
        Get = 0,
        Put = 1,
        Post = 2,
        Patch = 3,
        Delete = 4
    };

    enum class HttpWorkloadType {
        Unset = 0,
        Get_Global_Application_Commands = 1,
        Post_Global_Application_Command = 2,
        Get_Global_Application_Command = 3,
        Patch_Global_Application_Command = 4,
        Delete_Global_Application_Command = 5,
        Bulk_Put_Global_Application_Commands = 6,
        Get_Guild_Application_Commands = 7,
        Post_Guild_Application_Command = 8,
        Get_Guild_Application_Command = 9,
        Patch_Guild_Application_Command = 10,
        Delete_Guild_Application_Command = 11,
        Bulk_Put_Guild_Application_Commands = 12,
        Get_Guild_Application_Commands_Permissions = 13,
        Get_Guild_Application_Command_Permissions = 14,
        Put_Guild_Application_Command_Permissions = 15,
        Batch_Put_Guild_Application_Command_Permissions = 16,
        Post_Interaction_Response = 17,
        Get_Interaction_Response = 18,
        Patch_Interaction_Response = 19,
        Delete_Interaction_Response = 20,
        Post_Followup_Message = 21,
        Get_Followup_Message = 22,
        Patch_Followup_Message = 23,
        Delete_Followup_Message = 24,
        Get_Guild_Audit_Logs = 25,
        Get_Channel = 26,
        Patch_Channel = 27,
        Delete_Channel = 28,
        Get_Messages = 29,
        Get_Message = 30,
        Post_Message = 31,
        Crosspost_Message = 32,
        Put_Reaction = 33,
        Delete_Own_Reaction = 34,
        Delete_User_Reaction = 35,
        Get_Reactions = 36,
        Delete_All_Reactions = 37,
        Delete_Reactions_By_Emoji = 38,
        Patch_Message = 39,
        Delete_Message = 40,
        Delete_Message_Old = 41,
        Bulk_Delete_Messages = 42,
        Put_Channel_Permission_Overwrites = 43,
        Get_Channel_Invites = 44,
        Post_Channel_Invite = 45,
        Delete_Channel_Permission_Overwrites = 46,
        Post_Follow_News_Channel = 47,
        Post_Trigger_Typing_Indicator = 48,
        Get_Pinned_Messages = 49,
        Put_Pin_Message = 50,
        Delete_Pin_Message = 51,
        Put_Recipient_To_Group_Dm = 52,
        Delete_Recipient_From_Group_Dm = 53,
        Post_Thread_With_Message = 54,
        Post_Thread_Without_Message = 55,
        Put_Self_In_Thread = 56,
        Put_Thread_Member = 57,
        Delete_Self_From_Thread = 58,
        Delete_Thread_Member = 59,
        Get_Thread_Member = 60,
        Get_Thread_Members = 61,
        Get_Active_Threads = 62,
        Get_Public_Archived_Threads = 63,
        Get_Private_Archived_Threads = 64,
        Get_Joined_Private_Archived_Threads = 65,
        Get_Emoji_List = 66,
        Get_Guild_Emoji = 67,
        Post_Guild_Emoji = 68,
        Patch_Guild_Emoji = 69,
        Delete_Guild_Emoji = 70,
        Post_Guild = 71,
        Get_Guild = 72,
        Get_Guild_Preview = 73,
        Patch_Guild = 74,
        Delete_Guild = 75,
        Get_Guild_Channels = 76,
        Post_Guild_Channel = 77,
        Patch_Guild_Channel_Positions = 78,
        Get_Guild_Active_Threads = 79,
        Get_Guild_Member = 80,
        Get_Guild_Members = 81,
        Get_Search_Guild_Members = 82,
        Put_Guild_Member = 83,
        Patch_Guild_Member = 84,
        Patch_Current_Guild_Member = 85,
        Put_Guild_Member_Role = 86,
        Delete_Guild_Member_Role = 87,
        Delete_Guild_Member = 88,
        Get_Guild_Bans = 89,
        Get_Guild_Ban = 90,
        Put_Guild_Ban = 91,
        Delete_Guild_Ban = 92,
        Get_Guild_Roles = 93,
        Post_Guild_Role = 94,
        Patch_Guild_Role_Positions = 95,
        Patch_Guild_Role = 96,
        Delete_Guild_Role = 97,
        Get_Guild_Prune_Count = 98,
        Post_Guild_Prune = 99,
        Get_Guild_Voice_Regions = 100,
        Get_Guild_Invites = 101,
        Get_Guild_Integrations = 102,
        Delete_Guild_Integration = 103,
        Get_Guild_Widget_Settings = 104,
        Patch_Guild_Widget = 105,
        Get_Guild_Widget = 106,
        Get_Vanity_Invite = 107,
        Get_Guild_Widget_Image = 108,
        Get_Guild_Welcome_Screen = 109,
        Patch_Guild_Welcome_Screen = 110,
        Patch_Current_User_Voice_State = 111,
        Patch_User_Voice_State = 112,
        Get_Guild_Scheduled_Events = 113,
        Post_Guild_Scheduled_Event = 114,
        Get_Guild_Scheduled_Event = 115,
        Patch_Guild_Scheduled_Event = 116,
        Delete_Guild_Scheduled_Event = 117,
        Get_Guild_Scheduled_Event_Users = 118,
        Get_Guild_Template = 119,
        Post_Guild_From_Guild_Template = 120,
        Get_Guild_Templates = 121,
        Post_Guild_Template = 122,
        Put_Guild_Template = 123,
        Patch_Guild_Template = 124,
        Delete_Guild_Template = 125,
        Get_Invite = 126,
        Delete_Invite = 127,
        Post_Stage_Instance = 128,
        Get_Stage_Instance = 129,
        Patch_Stage_Instance = 130,
        Delete_Stage_Instance = 131,
        Get_Sticker = 132,
        Get_Nitro_Sticker_Packs = 133,
        Get_Guild_Stickers = 134,
        Post_Guild_Sticker = 135,
        Patch_Guild_Sticker = 136,
        Delete_Guild_Sticker = 137,
        Get_Current_User = 138,
        Get_User = 139,
        Patch_Current_User = 140,
        Get_Current_User_Guilds = 141,
        Delete_Leave_Guild = 142,
        Post_Create_User_Dm = 143,
        Get_User_Connections = 144,
        Get_Voice_Regions = 145,
        Post_Webhook = 146,
        Get_Channel_Webhooks = 147,
        Get_Guild_Webhooks = 148,
        Get_Webhook = 149,
        Get_Webhook_With_Token = 150,
        Patch_Webhook = 151,
        Patch_Webhook_With_Token = 152,
        Delete_Webhook = 153,
        Delete_Webhook_With_Token = 154,
        Post_Execute_Webhook = 155,
        Get_Webhook_Message = 156,
        Patch_Webhook_Message = 157,
        Delete_Webhook_Message = 158,
        Get_Application_Info = 159,
        Get_Authorization_Info = 160,
        Get_Gateway_Bot = 161,
        LAST = 162
    };

    struct DiscordCoreAPI_Dll HttpWorkloadData {
        std::unordered_map<std::string, std::string> headersToInsert{};
        HttpWorkloadClass workloadClass{};
        HttpWorkloadType workloadType{};
        std::string relativePath{ "" };
        std::string callStack{ "" };
        std::string content{ "" };
        std::string baseUrl{ "" };
    };

    struct DiscordCoreAPI_Dll VoiceConnectInitData {
        std::string channelId{ "" };
        std::string guildId{ "" };
        std::string userId{ "" };
        bool selfDeaf{ false };
        bool selfMute{ false };
    };

    struct DiscordCoreAPI_Dll VoiceConnectionData {
        std::string voiceEncryptionMode{ "" };
        std::string externalIp{ "" };
        std::string secretKey{ "" };
        std::string voicePort{ "" };
        std::string sessionId{ "" };
        std::string endPoint{ "" };
        std::string voiceIp{ "" };
        std::string token{ "" };
        uint32_t audioSSRC{ 0 };
    };

};
