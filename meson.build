
project('DiscordCoreAPI', 'cpp', license: 'AGPL-3.0',
		subproject_dir: 'External', meson_version: '>=0.60',
		default_options: ['optimization=2', 'debug=false',
						  'strip=true', 'b_lto=true',
						  'warning_level=3', 'cpp_std=c++20'])

SRCS = [
	'Source/ApplicationCommandEntities.cpp',
	'Source/AudioDecoder.cpp',
	'Source/AudioEncoder.cpp',
	'Source/ChannelEntities.cpp',
	'Source/CommandController.cpp',
	'Source/DataParsingFunctions.cpp',
	'Source/DiscordCoreClient.cpp',
	'Source/DllMain.cpp',
	'Source/ErlPacker.cpp',
	'Source/EventManager.cpp',
	'Source/FoundationEntities.cpp',
	'Source/GuildEntities.cpp',
	'Source/GuildMemberEntities.cpp',
	'Source/GuildScheduledEventEntities.cpp',
	'Source/Http.cpp',
	'Source/InputEvents.cpp',
	'Source/InteractionEntities.cpp',
	'Source/JSONIfier.cpp',
	'Source/MessageEntities.cpp',
	'Source/ReactionEntities.cpp',
	'Source/RoleEntities.cpp',
	'Source/SSLClients.cpp',
	'Source/SongAPI.cpp',
	'Source/SoundCloudAPI.cpp',
	'Source/StageInstanceEntities.cpp',
	'Source/StickerEntities.cpp',
	'Source/ThreadEntities.cpp',
	'Source/ThreadPool.cpp',
	'Source/UserEntities.cpp',
	'Source/VoiceConnection.cpp',
	'Source/WebHookEntities.cpp',
	'Source/WebSocketEntities.cpp',
	'Source/YouTubeAPI.cpp'
]

DEPS = [
	dependency('opus'),
	dependency('threads'),
	dependency('libsodium'),
	dependency('libavformat'),
	subproject('openssl').get_variable('openssl_dep'),
	subproject('nlohmann_json').get_variable('nlohmann_json_dep')
]

INCLUDE = include_directories('Include')

DiscordCoreAPI_lib = library(
	'DiscordCoreAPI',
	sources : SRCS,
	dependencies : DEPS,
	include_directories : INCLUDE
)

DiscordCoreAPI_dep = declare_dependency(
	link_with : DiscordCoreAPI_lib,
	include_directories : INCLUDE
)