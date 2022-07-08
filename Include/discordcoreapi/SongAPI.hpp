/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// SongAPI.hpp - Header for the song api related stuff.
/// Sep 17, 2021
/// https://discordcoreapi.com
/// \file SongAPI.hpp

#pragma once

#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup voice_connection
	 * @{
	 */
	/// A class DiscordCoreAPI_Dll representing the Song APIs. \brief A class DiscordCoreAPI_Dll representing the Song APIs.
	class DiscordCoreAPI_Dll SongAPI {
	  public:
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class VoiceConnection;

		DiscordCoreInternal::Event<CoRoutine<void>, SongCompletionEventData> onSongCompletionEvent{};
		DiscordCoreInternal::EventDelegateToken eventToken{};
		Playlist playlist{};

		SongAPI(const uint64_t& guildId);

		/// For setting up behavior in response to a completed song. \brief For setting up behavior in response to a completed song
		/// \param handler A delegate taking a SongCompletionEventData structure as an argument.
		/// \param guildId The id of the Guild for which you would like to instantiate this event.
		static void onSongCompletion(std::function<CoRoutine<void>(SongCompletionEventData)> handler, const uint64_t& guildId);

		/// Search for a Song to play. \brief Search for a Song to play.
		/// \param searchQuery The Song to search for.
		/// \param guildId The Guild id to search for the Song within.
		/// \returns A vector of Song objects representing the search results.
		static std::vector<Song> searchForSong(const std::string& searchQuery, const uint64_t& guildId);

		/// Adds a Song to the current Playlist's queue. \brief Adds a Song to the current Playlist's queue.
		/// \param guildMember The GuildMember that is adding the Song to the queue.
		/// \param song The Song to be added to the queue.
		/// \returns The Song that was added to the queue.
		static Song addSongToQueue(const GuildMember& guildMember, Song& song);

		/// Checks to see if there are any playable Songs in the current Playlist. \brief Checks to see if there are any playable Songs in the current Playlist.
		/// \param guildId The id of the Guild for which we would like to check its Playlist for Songs.
		/// \returns A bool representing whether there are any playable Songs.
		static bool isThereAnySongs(const uint64_t& guildId);

		/// Send the next playable song off of the current Guild's Playlist to be played.
		/// \brief Send the next playable song off of the current Guild's Playlist to be played.
		/// \param guildMember The GuildMember who is requesting the Song to be sent.
		/// \returns A bool suggesting the success status of the send.
		static bool sendNextSong(const GuildMember& guildMember);

		/// Plays the current Song. (Assuming that you are currently connected to a VoiceConnection). \brief Plays the current Song. (Assuming that you are currently connected to a VoiceConnection).
		/// \param guildId The id of the Guild within which to play the current Song.
		/// \returns A bool suggesting the success or failure of the play command.
		static bool play(const uint64_t& guildId);

		/// Skips to the next Song in the queue, if applicable. \brief Skips to the next Song in the queue, if applicable.
		/// \param guildMember The GuildMember structure of the individual who is skipping the Song.
		static void skip(const GuildMember& guildMember);

		/// Stops the currently playing Song. \brief Stops the currently playing Song.
		/// \param guildId The id of the Guild within which to stop the currently playing music.
		static void stop(const uint64_t& guildId);

		/// Toggles pausing on and off. \brief Toggles pausing on and off.
		/// \param guildId The id of the Guild which you would like to pause the Song for.
		static void pauseToggle(const uint64_t& guildId);

		/// Checks if there is currently playing music for the current Guild. \brief Checks if there is currently playing music for the current Guild.
		/// \param guildId The id for the desired Guild to check the current playing status.
		/// \returns A bool representing the currently playing status.
		static bool areWeCurrentlyPlaying(const uint64_t& guildId);

		/// Collects the Playlist from the SongAPI. \brief Collects the Playlist from the SongAPI.
		/// \param guildId The Guild for which to collect the Playlist from.
		/// \returns A Playlist.
		static Playlist getPlaylist(const uint64_t& guildId);

		/// Sets the playlist of the desired Guild. \brief Sets the playlist of the desired Guild.
		/// \param playlistNew The new Playlist to be set.
		/// \param guildId The id of the desired Guild to set the Playlist of.
		static void setPlaylist(const Playlist& playlistNew, const uint64_t& guildId);

		/// Returns the current loop-all status of the current Guild's Playlist. \brief Returns the current loop-all status of the current Guild's Playlist.
		/// \param guildId The id of the Guild for which you would like to check the loop-all status of.
		/// \returns A bool representing the current loop-all status.
		static bool isLoopAllEnabled(const uint64_t& guildId);

		/// Enables or disables the loop-all status of the playlist. \brief Enables or disables the loop-all status of the playlist.
		/// \param enabled A bool representing whether or not to enable the loop-all status.
		/// \param guildId The Guild id for which Guild to update this status in.
		static void setLoopAllStatus(bool enabled, const uint64_t& guildId);

		/// Returns the current loop-song of the current Guild's Playlist. \brief Returns the current loop-song of the current Guild's Playlist.
		/// \param guildId The id of the Guild for which you would like to check the loop-song status of. \returns A bool representing the current loop-song status.
		static bool isLoopSongEnabled(const uint64_t& guildId);

		/// Enables or disables the loop-song status of the playlist. \brief Enables or disables the loop-song status of the playlist.
		/// \param enabled A bool representing whether or not to enable the loop-song status.
		/// \param guildId The Guild id for which Guild to update this status in.
		static void setLoopSongStatus(bool enabled, const uint64_t& guildId);

		/// Sets the position of a Song in the current Playlist's song queue. \brief Sets the position of a Song in the current Playlist's song queue.
		/// \param firstSongPosition The first Song's initial position.
		/// \param secondSongPosition The first Song's final position.
		/// \param guildId The id of the desired Guild to update the Song positions in.
		static void modifyQueue(int32_t firstSongPosition, int32_t secondSongPosition, const uint64_t& guildId);

		/// Collects the currently playing Song. \brief Collects the currently playing Song.
		/// \param guildId The id for which Guild to collect the current Song of.
		/// \returns The current Song for the given Guild.
		static Song getCurrentSong(const uint64_t& guildId);

		/// Sets the currently playing Song. \brief Sets the currently playing Song.
		/// \param song The Song to set as the currently playing Song.
		/// \param guildId The id for which Guild to set the current Song of.
		static void setCurrentSong(const Song& song, const uint64_t& guildId);

		~SongAPI();

	  protected:
		static std::mutex accessMutex;

		std::unique_ptr<std::jthread> taskThread{ nullptr };
		bool areWeInstantiated{ false };
		Snowflake guildId{};

		void sendNextSongFinal(const GuildMember& guildMember);

		bool sendNextSong();
	};
	/**@}*/
};// namespace DiscordCoreAPI
