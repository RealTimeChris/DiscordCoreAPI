if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF fababbe98a5b0158b8168b1657cdaed28bffc374
	SHA512 c43df10ea0d0102a60a8a033cb079c840dc49523f1cda1d18c26c3c224da48a26dd37a298e0688cd7fa66337b86121c99f30add44dc900f9e0cacf9d78c7a929