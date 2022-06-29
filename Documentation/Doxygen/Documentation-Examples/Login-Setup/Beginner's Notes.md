Beginner's Notes {#beginnernotes}
============

- <B><U>IMPORTANT:</U></B> You will need to enable the following settings in your bot's control panel: Enter your "Developer Control Panel" and select a bot, then go to the "Bot" section-in here, enable "Presence Intent", "Server Members Intent", "Message Content Intent" - and hit "Save".   
<BR>
- All of the central class managers are classes that are filled with static methods that are a part of the `DiscordCoreAPI` namespace, they are as Follows:   
    `DiscordCoreAPI::ApplicationCommands`

    `DiscordCoreAPI::AutoModerationRules`
  
    `DiscordCoreAPI::Channels`
    
    `DiscordCoreAPI::EventManager`
    
    `DiscordCoreAPI::Guilds`
    
    `DiscordCoreAPI::GuildMembers`
    
    `DiscordCoreAPI::GuildScheduledEvents`
    
    `DiscordCoreAPI::InputEvents`
    
    `DiscordCoreAPI::Interactions`
    
    `DiscordCoreAPI::Messages`
    
    `DiscordCoreAPI::Reactions`
    
    `DiscordCoreAPI::Roles`
    
    `DiscordCoreAPI::StageInstances`
    
    `DiscordCoreAPI::Stickers`
    
    `DiscordCoreAPI::Threads`
    
    `DiscordCoreAPI::Users`
    
    `DiscordCoreAPI::WebHooks`   
<BR>
- Each of the functions in this library takes an argument that is of a type who's name is derived from the function-for example, the data structure that gets passed into `DiscordCoreAPI::Guilds::getGuildAsync()` is `DiscordCoreAPI::GetGuildData`.   
<BR>
- Most of the functions in this library are `Async`, meaning that they return a value of type `DiscordCoreAPI::CoRoutine<TaskType>`, which you call `.get()` on to retrieve the returned value, and to suspend the current thread in the process of waiting for it. Alternatively, you have the option of leaving the task's completion up to the background thread that is handling it, by not calling `.get()`.
