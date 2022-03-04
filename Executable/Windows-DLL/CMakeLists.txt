if (WIN32)
	add_compile_options(/bigobj)
	set(CURL_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include/curl")
	set(CURL_LIBRARY "${VCPKG_INSTALLED_DIR}/bin/curl.dll")
	list(APPEND CMAKE_MODULE_PATH "${VCPKG_INSTALLED_DIR}/share/curl")
	list(APPEND CMAKE_MODULE_PATH "${VCPKG_INSTALLED_DIR}/share/ffmpeg")
	find_package(CURL REQUIRED)
	find_package(FFMPEG REQUIRED)
	set(OPENSSL_ROOT_DIR "${VCPKG_INSTALLED_DIR}")
	find_package(OpenSSL REQUIRED)

	add_library(DiscordCoreAPI-DLL SHARED "../../Source/DllMain.cpp" "../../Include/Index.hpp" "../../Source/ApplicationCommandEntities.cpp" "../../Source/AudioDecoder.cpp"
		"../../Source/AudioEncoder.cpp" "../../Source/ChannelEntities.cpp"
		"../../Source/CommandController.cpp" "../../Source/DataParsingFunctions.cpp"
		"../../Source/DiscordCoreClient.cpp" "../../Source/DiscordCoreClient.cpp"
		"../../Source/ErlPacker.cpp" "../../Source/EventManager.cpp"
		"../../Source/FoundationEntities.cpp" "../../Source/GuildEntities.cpp"
		"../../Source/GuildMemberEntities.cpp" "../../Source/GuildScheduledEventEntities.cpp"
		"../../Source/Http.cpp" "../../Source/InputEvents.cpp"
		"../../Source/InteractionEntities.cpp" "../../Source/JSONIfier.cpp"
		"../../Source/MessageEntities.cpp" "../../Source/ReactionEntities.cpp"
		"../../Source/RoleEntities.cpp" "../../Source/SongAPI.cpp"
		"../../Source/SoundCloudAPI.cpp" "../../Source/SSLClients.cpp"
		"../../Source/StageInstanceEntities.cpp" "../../Source/StickerEntities.cpp"
		"../../Source/ThreadEntities.cpp" "../../Source/ThreadPool.cpp"
		"../../Source/UserEntities.cpp" "../../Source/VoiceConnection.cpp"
		"../../Source/WebHookEntities.cpp" "../../Source/WebSocketEntities.cpp"
		"../../Source/YouTubeAPI.cpp"
	)

	target_link_libraries(DiscordCoreAPI-DLL PUBLIC OpenSSL::SSL OpenSSL::Crypto)
	target_link_libraries(DiscordCoreAPI-DLL PUBLIC ${FFMPEG_LIBRARIES})
	target_link_libraries(DiscordCoreAPI-DLL PUBLIC "${VCPKG_INSTALLED_DIR}/lib/libcurl.lib")
	target_link_libraries(DiscordCoreAPI-DLL PUBLIC "${VCPKG_INSTALLED_DIR}/lib/libsodium.lib")
	target_link_libraries(DiscordCoreAPI-DLL PUBLIC "${VCPKG_INSTALLED_DIR}/lib/glib-2.0.lib")
	target_link_libraries(DiscordCoreAPI-DLL PUBLIC "${VCPKG_INSTALLED_DIR}/lib/opus.lib")
endif()