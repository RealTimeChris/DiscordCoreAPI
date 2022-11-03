/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.
	Copyright 2021, 2022 Chris M. (RealTimeChris)
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.
	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// Colors.hpp - Header for premade color constants
/// and structs. Nov 03, 2022 DJ::Oetzi
/// https://discordcoreapi.com
/// \file Colors.hpp
#pragma once
#ifndef COLORS

#include <string>

namespace DiscordCoreAPI {
	/// Several premade color constants ready to use for embeds
	namespace Colors {
		const std::string
			White = "FFFFFF",
			DiscordWhite = "FFFFFE",
			LightGray = "C0C0C0",
			Gray = "808080",
			DarkGray = "404040",
			Black = "000000",
			DiscordBlack = "000001",
			Red = "FF0000",
			Pink = "FFAFAF",
			Orange = "FFC800",
			Yellow = "FFFF00",
			Green = "00FF00",
			Magenta = "FF00FF",
			Cyan = "00FFFF",
			Blue = "0000FF",
			LightSeaGreen = "1ABC9C",
			MediumSeaGreen = "2ECC71",
			SummerSky = "3498DB",
			DeepLilac = "9B59B6",
			Ruby = "E91E63",
			MoonYellow = "F1C40F",
			TahitiGold = "E67E22",
			Cinnabar = "E74C3C",
			Submarine = "95A5A6",
			BlueAquamarine = "607D8B",
			DeepSea = "11806A",
			SeaGreen = "1F8B4C",
			Endeavour = "206694",
			VividViolet = "71368A",
			JazzberryJam = "AD1457",
			DarkGoldenrod = "C27C0E",
			Rust = "A84300",
			Brown = "992D22",
			GrayChateau = "979C9F",
			Bismark = "546E7A",
			StiBlue = "0E4BEF",
			WrxBlue = "00247D",
			RalliArtCrimson = "E60012",
			Lime = "00FF00",
			ForestGreen = "228B22",
			CadmiumGreen = "097969",
			Aquamarine = "7FFFD4",
			BlueGreen = "088F8F",
			Raspberry = "E30B5C",
			ScarletRed = "FF2400";
	};
};
#endif
