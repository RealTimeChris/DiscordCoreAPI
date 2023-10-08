Beginner's Notes {#beginnernotes}
============
- <b><u>Important:</u></b> You will need to enable the following settings in your bot's control panel: enter your "developer control panel" and select a bot, then go to the "bot" section-in here, enable "presence intent", "server members intent", "message content intent" - and hit "save".   
<br>
- All of the central class managers are classes that are filled with static methods that are a part of the `discord_core_api` namespace, they are as follows:   

    `discord_core_api::application_commands`

    `discord_core_api::auto_moderation_rules`
  
    `discord_core_api::channels`
    
    `discord_core_api::event_manager`
    
    `discord_core_api::guilds`
    
    `discord_core_api::guild_members`
    
    `discord_core_api::guild_scheduled_events`
    
    `discord_core_api::input_events`
    
    `discord_core_api::interactions`
    
    `discord_core_api::messages`
    
    `discord_core_api::reactions`
    
    `discord_core_api::roles`
    
    `discord_core_api::stage_instances`
    
    `discord_core_api::stickers`
    
    `discord_core_api::threads`
    
    `discord_core_api::users`
    
    `discord_core_api::web_hooks`   

<br>
- Each of the functions in this library takes an argument that is of a type who's name is derived from the function-for example, the data structure that gets passed into `discord_core_api::guilds::getGuildAsync()` is `discord_core_api::get_guild_data`.   
<br>
- Most of the functions in this library are `async`, meaning that they return a value of type `discord_core_api::co_routine<task_type>`, which you call `.get()` on to retrieve the returned value, and to suspend the current thread in the process of waiting for it. alternatively, you have the option of leaving the task's completion up to the background thread that is handling it, by not calling `.get()`.
