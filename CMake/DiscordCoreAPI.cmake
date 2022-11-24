include(FetchContent)

fetchcontent_declare(
	 DiscordCoreAPI
	URL https://github.com/RealTimeChris/DiscordCoreAPI/archive/refs/tags/v1.20.tar.gz
	GIT_TAG b31b14a87434a0051dbce22d6035e396524971ad
)

fetchcontent_makeavailable(DiscordCoreAPI)