// SongAPI.hpp - Header for the song api related stuff.
// Sep 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "YouTubeAPI.hpp"
#include "SoundCloudAPI.hpp"
#include "GuildEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordGuild;

	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// Class representing the Song APIs. \brief Class representing the Song APIs.
	class DiscordCoreAPI_Dll SongAPI {
	public:

		friend Playlist loadPlaylist(DiscordGuild guild);
		friend void savePlaylist(DiscordGuild guild);
		friend class DiscordCoreClient;
		friend class VoiceConnection;
		friend class Guild;

		SongAPI(string guildId);

		/// Search for a Song to play. \brief Search for a Song to play.
		/// \param searchQuery The Song to search for.
		/// \param guildId The Guild id to search for the Song within.
		/// \returns A vector of Song objects representing the search results.
		static vector<Song> searchForSong(string searchQuery, string guildId);

		/// Adds a Song to the current Playlist's queue. \brief Adds a Song to the current Playlist's queue.
		/// \param guildMember The GuildMember that is adding the Song to the queue.
		/// \param song The Song to be added to the queue.
		/// \returns The Song that was added to the queue.
		static Song addSongToQueue(GuildMember guildMember, Song song);

		/// Checks to see if there are any playable Songs in the current Playlist. \brief Checks to see if there are any playable Songs in the current Playlist.
		/// \param guildId The id of the Guild for which we would like to check its Playlist for Songs.
		/// \returns A bool representing whether there are any playable Songs.
		static bool isThereAnySongs(string guildId);

		/// Send the next playable song off of the current Guild's Playlist to be played. \brief Send the next playable song off of the current Guild's Playlist to be played.
		/// \param guildMember The GuildMember who is requesting the Song to be sent.
		/// \returns A bool suggesting the success status of the send.
		static bool sendNextSong(GuildMember guildMember);

		/// Plays the current Song. (Assuming that you are currently connected to a VoiceConnection). \brief Plays the current Song. (Assuming that you are currently connected to a VoiceConnection).
		/// \param guildId The id of the Guild within which to play the current Song.
		/// \returns A bool suggesting the success or failure of the play command.
		static bool play(string guildId);

		/// Skips to the next Song in the queue, if applicable. \brief Skips to the next Song in the queue, if applicable.
		/// \param guildMember The GuildMember structure of the individual who is skipping the Song.
		/// \returns A bool suggesting the success or failure of the skip command.
		static bool skip(GuildMember guildMember);

		/// Stops the currently playing Song. \brief Stops the currently playing Song.
		/// \param guildId The id of the Guild within which to stop the currently playing music.
		/// \returns A bool suggesting the success or failure of the stop command.
		static bool stop(string guildId);

		/// Toggles pausing on and off. \brief Toggles pausing on and off.
		/// \param guildId The id of the Guild which you would like to pause the Song for.
		/// \returns void
		static void pauseToggle(string guildId);

		/// Checks if there is currently playing music for the current Guild. \brief Checks if there is currently playing music for the current Guild.
		/// \param guildId The id for the desired Guild to check the current playing status.
		/// \returns A bool representing the currently playing status.
		static bool areWeCurrentlyPlaying(string guildId);

		/// Collects the Playlist from the SongAPI. \brief Collects the Playlist from the SongAPI.
		/// \param guildId The Guild for which to collect the Playlist from.
		/// \returns A Playlist.
		static Playlist getPlaylist(string guildId);

		/// Sets the playlist of the desired Guild. \brief Sets the playlist of the desired Guild.
		/// \param playlistNew The new Playlist to be set.
		/// \param guildId The id of the desired Guild to set the Playlist of.
		/// \returns void.
		static void setPlaylist(Playlist playlistNew, string guildId);

		/// Returns the current loop-all status of the current guild's Playlist. \brief Returns the current loop-all status of the current guild's Playlist.
		/// \param guildId The id of the Guild for which you would like to check the loop-all status of.
		/// \returns A bool representing the current loop-all status.
		static bool isLoopAllEnabled(string guildId);

		/// Enables or disables the loop-all status of the playlist. \brief Enables or disables the loop-all status of the playlist.
		/// \param enabled A bool representing whether or not to enable the loop-all status.
		/// \param guildId The guild id for which guild to update this status in.
		/// \returns void
		static void setLoopAllStatus(bool enabled, string guildId);

		/// Returns the current loop-song of the current guild's Playlist. \brief Returns the current loop-song of the current guild's Playlist.
		/// \param guildId The id of the Guild for which you would like to check the loop-song status of.
		/// \returns A bool representing the current loop-song status.
		static bool isLoopSongEnabled(string guildId);

		/// Enables or disables the loop-song status of the playlist. \brief Enables or disables the loop-song status of the playlist.
		/// \param enabled A bool representing whether or not to enable the loop-song status.
		/// \param guildId The guild id for which guild to update this status in.
		/// \returns void
		static void setLoopSongStatus(bool enabled, string guildId);		

		/// Sets the position of a Song in the current Playlist's song queue. \brief Sets the position of a Song in the current Playlist's song queue.
		/// \param firstSongPosition The first Song's initial position.
		/// \param secondSongPosition The first Song's final position.
		/// \param guildId The id of the desired Guild to update the Song positions in.
		/// \returns void.
		static void modifyQueue(__int32 firstSongPosition, __int32 secondSongPosition, string guildId);

		/// Collects the currently playing Song. \brief Collects the currently playing Song.
		/// \param guildId The id for which guild to collect the current Song of.
		/// \returns The current Song for the given Guild.
		static Song getCurrentSong(string guildId);

		/// Sets the currently playing Song. \brief Sets the currently playing Song.
		/// \param song The Song to set as the currently playing Song.
		/// \param guildId The id for which guild to set the current Song of.
		/// \returns void
		static void setCurrentSong(Song song, string guildId);

	protected:

		string guildId{ "" };
		Playlist playlist{};

		bool sendNextSong();
	};
	/**@}*/
};
