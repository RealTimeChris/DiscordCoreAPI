### **Beginner's Notes:**
---
- All of the central class managers are classes that are filled with static methods that are a part of the `DiscordCoreAPI` namespace.
  ##### They are as Follows:
    [`DiscordCoreAPI::ApplicationCommands`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#application-command-stuff)
  
    [`DiscordCoreAPI::Channels`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#channel-stuff)
    
    `DiscordCoreAPI::EventManager`
    
    [`DiscordCoreAPI::Guilds`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#guild-stuff)
    
    [`DiscordCoreAPI::GuildMembers`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#guildmember-stuff)
    
    [`DiscordCoreAPI::GuildScheduledEvents`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#guild-scheduled-event-stuff)
    
    [`DiscordCoreAPI::InputEvents`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#input-event-stuff)
    
    [`DiscordCoreAPI::Interactions`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#interaction-stuff)
    
    [`DiscordCoreAPI::Messages`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#message-stuff)
    
    [`DiscordCoreAPI::Reactions`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#reaction-stuff)
    
    [`DiscordCoreAPI::Roles`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#role-stuff)
    
    [`DiscordCoreAPI::StageInstances`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#stage-instance-stuff)
    
    [`DiscordCoreAPI::Stickers`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#sticker-stuff)
    
    [`DiscordCoreAPI::Threads`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#thread-stuff)
    
    [`DiscordCoreAPI::Users`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#user-stuff)
    
    [`DiscordCoreAPI::WebHooks`](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md#webhook-stuff)
    
    
- Each of the functions in this library takes an argument that is of a type who's name is derived from the function - for example, the data structure that gets passed into `DiscordCoreAPI::Guilds::getGuildAsync()` is `DiscordCoreAPI::GetGuildData`.
- Most of the functions in this library are `Async`, meaning that they return a value of type `CoRoutine<TaskType>`, which you call `.get()` on to retrieve the returned value, and to suspend the current thread in the process of waiting for it. Alternatively, you have the option of leaving the task's completion up to the background thread that is handling it, by not calling `.get()`.
