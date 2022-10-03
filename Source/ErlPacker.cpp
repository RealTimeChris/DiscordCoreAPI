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
/// ErlPacker.cpp - Source file for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlPacker.cpp

#include <discordcoreapi/ErlPacker.hpp>

namespace DiscordCoreInternal {

	ErlPackError::ErlPackError(const String& message) : std::runtime_error(message.c_str()){};

	String ErlPacker::parseJsonToEtf(String&& dataToParse) {
		this->bufferString.clear();
		this->offSet = 0;
		this->size = 0;
		this->appendVersion();
		dataToParse.reserve(dataToParse.size() + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser theParser{};
		auto theDocument = theParser.iterate(dataToParse.data(), dataToParse.length(), dataToParse.capacity());
		this->singleValueJsonToETF(theDocument);
		return this->bufferString;
	}

	String& ErlPacker::parseEtfToJson(StringView dataToParse) {
		if (this->bufferString.size() < dataToParse.size()) {
			this->bufferString.resize(dataToParse.size() * 2);
		}
		this->offSet = 0;
		this->buffer = dataToParse;
		this->size = dataToParse.size();
		if (this->readBits<Uint8>() != formatVersion) {
			throw ErlPackError{ "ErlPacker::parseEtfToJson() Error: Incorrect format version specified." };
		}
		auto theString = this->singleValueETFToJson();
		memcpy(this->bufferString.data(), theString.data(), theString.size());
		for (size_t x = theString.size(); x < this->bufferString.size(); ++x) {
			this->bufferString[x] = '\0';
		}
		return this->bufferString;
	}

	void ErlPacker::singleValueJsonToETF(simdjson::ondemand::value jsonData) {
		switch (jsonData.type()) {
			case simdjson::ondemand::json_type::array: {
				this->writeArray(jsonData);
				break;
			}
			case simdjson::ondemand::json_type::object: {
				this->writeObject(jsonData);
				break;
			}
			case simdjson::ondemand::json_type::number: {
				this->writeNumber(jsonData);
				break;
			}
			case simdjson::ondemand::json_type::string: {
				this->writeString(jsonData);
				break;
			}
			case simdjson::ondemand::json_type::boolean: {
				this->writeBool(jsonData);
				break;
			}
			case simdjson::ondemand::json_type::null: {
				this->appendNil();
				break;
			}
		}
	}

	void ErlPacker::writeObject(simdjson::ondemand::value jsonData) {
		Bool add_comma{ false };
		this->appendMapHeader(static_cast<Uint32>(jsonData.count_fields().take_value()));
		for (auto field: jsonData.get_object()) {
			if (add_comma) {
			}
			StringStream theStream{};
			theStream << field.key();
			String theKey = theStream.str();

			auto theSize = theKey.size();
			this->appendBinaryExt(theKey, static_cast<Uint32>(theSize));
			this->singleValueJsonToETF(field.value());
			add_comma = true;
		}
	}

	void ErlPacker::writeString(simdjson::ondemand::value jsonData) {
		StringStream theStream{};
		theStream << jsonData.get_string().take_value();
		auto theSize = static_cast<Uint32>(theStream.str().size());
		this->appendBinaryExt(theStream.str(), theSize);
	}

	void ErlPacker::writeNumber(simdjson::ondemand::value jsonData) {
		if (jsonData.get_number_type() == simdjson::ondemand::number_type::floating_point_number) {
			auto theFloat = jsonData.get_double().take_value();
			this->appendNewFloatExt(theFloat);
		} else if (jsonData.get_number_type() == simdjson::ondemand::number_type::unsigned_integer) {
			auto theInt = jsonData.get_uint64().take_value();
			if (theInt <= 255 && theInt >= 0) {
				this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
			} else if (theInt <= std::numeric_limits<Uint32>::max() && theInt >= 0) {
				this->appendIntegerExt(static_cast<Uint32>(theInt));
			} else {
				this->appendUnsignedLongLong(theInt);
			}
		} else if (jsonData.get_number_type() == simdjson::ondemand::number_type::signed_integer) {
			auto theInt = jsonData.get_int64().take_value();
			if (theInt <= 127 && theInt >= -127) {
				this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
			} else if (theInt <= std::numeric_limits<Int32>::max() && theInt >= std::numeric_limits<Int32>::min()) {
				this->appendIntegerExt(static_cast<Uint32>(theInt));
			} else {
				this->appendUnsignedLongLong(static_cast<Uint64>(theInt));
			}
		}
	}

	void ErlPacker::writeArray(simdjson::ondemand::value jsonData) {
		Bool add_comma{ false };
		this->appendListHeader(static_cast<Uint32>(jsonData.count_elements().take_value()));
		for (auto element: jsonData.get_array()) {
			this->singleValueJsonToETF(element.value());
			add_comma = true;
		}
		this->appendNilExt();
	}

	void ErlPacker::writeBool(simdjson::ondemand::value jsonData) {
		auto theBool = jsonData.get_bool().take_value();
		if (theBool) {
			this->appendTrue();
		} else {
			this->appendFalse();
		}
	}

	void ErlPacker::writeToBuffer(const String& bytes) {
		this->bufferString.insert(this->bufferString.end(), bytes.begin(), bytes.end());
		this->offSet += bytes.size();
	}

	void ErlPacker::appendBinaryExt(const String& bytes, Uint32 sizeNew) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Binary_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
		this->writeToBuffer(bytes);
	}

	void ErlPacker::appendUnsignedLongLong(Uint64 value) {
		String bufferNew{};
		bufferNew.resize(static_cast<Uint64>(1) + 2 + sizeof(Uint64));
		bufferNew[0] = static_cast<Uint8>(ETFTokenType::Small_Big_Ext);
		DiscordCoreAPI::StopWatch theStopWatch{ 1500ms };
		Uint8 bytesToEncode = 0;
		while (value > 0) {
			if (theStopWatch.hasTimePassed()) {
				break;
			}
			bufferNew[static_cast<size_t>(3) + bytesToEncode] = value & 0xF;
			value >>= 8;
			bytesToEncode++;
		}
		bufferNew[1] = bytesToEncode;
		bufferNew[2] = 0;
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendSmallIntegerExt(Uint8 value) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendIntegerExt(Uint32 value) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Integer_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, value);
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendListHeader(Uint32 sizeNew) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::List_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendMapHeader(Uint32 sizeNew) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Map_Ext) };
		DiscordCoreAPI::storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNewFloatExt(Double FloatValue) {
		String bufferNew{};
		bufferNew.push_back(static_cast<unsigned char>(ETFTokenType::New_Float_Ext));

		void* punner{ &FloatValue };
		DiscordCoreAPI::storeBits(bufferNew, *static_cast<Uint64*>(punner));
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendVersion() {
		String bufferNew{};
		bufferNew.push_back(static_cast<char>(formatVersion));
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNilExt() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Nil_Ext) };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendFalse() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 5, static_cast<Uint8>('f'), static_cast<Uint8>('a'), static_cast<Uint8>('l'),
			static_cast<Uint8>('s'), static_cast<Uint8>('e') };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendTrue() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 4, static_cast<Uint8>('t'), static_cast<Uint8>('r'), static_cast<Uint8>('u'),
			static_cast<Uint8>('e') };
		this->writeToBuffer(bufferNew);
	}

	void ErlPacker::appendNil() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 3, static_cast<Uint8>('n'), static_cast<Uint8>('i'), static_cast<Uint8>('l') };
		this->writeToBuffer(bufferNew);
	}

	size_t ErlPacker::readString(Uint32 length) {
		if (this->offSet + static_cast<Uint64>(length) > this->size) {
			throw ErlPackError{ "this->readString() Error: readString() past end of buffer.\n\n" };
		}
		if (this->bufferString.size() <= length * 2) {
			this->bufferString.resize(length * 2);
		}
		size_t theFinalSize{};
		char* theStringNew = ( char* )this->buffer.data() + this->offSet;
		size_t theIndex{};
		for (Uint32 x = 0; x < length; ++x) {
			switch (static_cast<char>(theStringNew[x])) {
				case 0x00: {
					break;
				}
				case 0x27: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('\'');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x22: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('"');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x5c: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('\\');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x07: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('a');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x08: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('b');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x0C: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('f');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x0A: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('n');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x0D: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('r');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x0B: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('v');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				case 0x09: {
					this->bufferString[theIndex] = static_cast<char>('\\');
					this->bufferString[theIndex + 1] = static_cast<char>('t');
					theFinalSize += 2;
					theIndex += 2;
					break;
				}
				default: {
					this->bufferString[theIndex] = theStringNew[x];
					theFinalSize++;
					theIndex++;
					break;
				}
			}
		}
		this->offSet += length;
		return theFinalSize;
	}

	String ErlPacker::processAtom(const char* atom, Uint32 length) {
		if (atom == nullptr) {
			return String{};
		}
		if (length >= 3 && length <= 5) {
			if (length == 3 && strncmp(atom, "nil", 3) == 0) {
				return "null";
			} else if (length == 4 && strncmp(atom, "null", 4) == 0) {
				return "null";
			} else if (length == 4 && strncmp(atom, "true", 4) == 0) {
				return "true";
			} else if (length == 5 && strncmp(atom, "false", 5) == 0) {
				return "false";
			}
		}
		String theValue{ "\"" };
		theValue += String{ atom, length };
		theValue += "\"";
		return theValue;
	}

	String ErlPacker::singleValueETFToJson() {
		if (this->offSet >= this->size) {
			throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Read past end of ETF buffer.\n\n" };
		}
		Uint8 type = this->readBits<Uint8>();
		switch (static_cast<ETFTokenType>(type)) {
			case ETFTokenType::Small_Integer_Ext: {
				return this->parseSmallIntegerExt();
			}
			case ETFTokenType::Integer_Ext: {
				return this->parseIntegerExt();
			}
			case ETFTokenType::New_Float_Ext: {
				return this->parseNewFloatExt();
			}
			case ETFTokenType::Float_Ext: {
				return this->parseFloatExt();
			}
			case ETFTokenType::Atom_Ext: {
				return this->parseAtomUtf8Ext();
			}
			case ETFTokenType::Small_Tuple_Ext: {
				return this->parseSmallTupleExt();
			}
			case ETFTokenType::Large_Tuple_Ext: {
				return this->parseLargeTupleExt();
			}
			case ETFTokenType::Nil_Ext: {
				return this->parseNilExt();
			}
			case ETFTokenType::String_Ext: {
				return this->parseStringAsList();
			}
			case ETFTokenType::List_Ext: {
				return this->parseListExt();
			}
			case ETFTokenType::Binary_Ext: {
				return this->parseBinaryExt();
			}
			case ETFTokenType::Small_Big_Ext: {
				return this->parseSmallBigExt();
			}
			case ETFTokenType::Large_Big_Ext: {
				return this->parseLargeBigExt();
			}
			case ETFTokenType::Small_Atom_Ext: {
				return this->parseSmallAtomExt();
			}
			case ETFTokenType::Map_Ext: {
				return this->parseMapExt();
			}
			case ETFTokenType::Atom_Utf8_Ext: {
				return this->parseAtomUtf8Ext();
			}
			default: {
				throw ErlPackError{ "ErlPacker::singleValueETFToJson() Error: Unknown data type in ETF.\n\n" };
			}
		}
	}

	String ErlPacker::parseSmallIntegerExt() {
		return std::to_string(this->readBits<Uint8>());
	}

	String ErlPacker::parseBigint(Uint32 digits) {
		Uint8 sign = this->readBits<Uint8>();
		if (digits > 8) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Integers larger than 8 bytes are not supported.\n\n" };
		}
		Uint64 value = 0;
		Uint64 b = 1;
		for (Uint32 x = 0; x < digits; ++x) {
			Uint8 digit = this->readBits<Uint8>();
			Uint64 digitNew = digit;
			value += digitNew * b;
			b <<= 8;
		}
		if (digits <= 4) {
			if (sign == 0) {
				return std::to_string(value);
			}
			const Bool isSignBitAvailable = (value & 1ull << 31ull) == 0;
			if (isSignBitAvailable) {
				return std::to_string(-static_cast<Int32>(value));
			}
		}
		char outBuffer[32] = { 0 };
		const char* formatString = sign == 0 ? "%llu" : "-%ll";
		auto theValue = sign == 0 ? static_cast<Uint64>(value) : static_cast<Int64>(value);
		const Int32 res = sprintf(outBuffer, formatString, theValue);
		if (res < 0) {
			throw ErlPackError{ "ErlPacker::parseBigint() Error: Parse big integer failed.\n\n" };
		}
		const Uint8 length = static_cast<Uint8>(res);
		String theReturnValue = String{ outBuffer, length };
		return theReturnValue;
	}

	String ErlPacker::parseArray(const Uint32 length) {
		String array{};
		for (Uint32 x = 0; x < length; x++) {
			array += std::move(this->singleValueETFToJson());
			if (x < length - 1) {
				array += ",";
			}
		}

		return array;
	}

	String ErlPacker::parseTuple(const Uint32 length) {
		return this->parseArray(length);
	}

	String ErlPacker::parseSmallTupleExt() {
		return this->parseTuple(this->readBits<Uint8>());
	}

	String ErlPacker::parseLargeTupleExt() {
		return this->parseTuple(this->readBits<Uint32>());
	}

	String ErlPacker::parseSmallAtomExt() {
		Uint8 length = this->readBits<Uint8>();
		auto lengthNew = this->readString(length);
		return this->processAtom(this->bufferString.data(), length);
	};

	String ErlPacker::parseStringAsList() {
		String theString{ "\"" };
		Uint16 length = this->readBits<Uint16>();
		if (static_cast<Uint64>(this->offSet) + length > this->size) {
			throw ErlPackError{ "ErlPacker::parseStringAsList() Error: String list past end of buffer.\n\n" };
		}
		theString.reserve(theString.size() + length);
		for (Uint16 x = 0; x < length; ++x) {
			theString.push_back(this->parseSmallIntegerExt()[0]);
		}
		theString += "\"";
		return theString;
	}

	String ErlPacker::parseNewFloatExt() {
		Uint64 theValue = readBits<Uint64>();
		void* thePtr{ &theValue };
		String theValueNew = std::to_string(*static_cast<Double*>(thePtr));
		return theValueNew;
	}

	String ErlPacker::parseSmallBigExt() {
		String theString{ "\"" };
		theString += this->parseBigint(this->readBits<Uint8>());
		theString += "\"";
		return theString;
	}

	String ErlPacker::parseLargeBigExt() {
		return this->parseBigint(this->readBits<Uint32>());
	}

	String ErlPacker::parseAtomUtf8Ext() {
		Uint32 length = this->readBits<Uint16>();
		auto lengthNew = static_cast<Uint32>(this->readString(length));
		return this->processAtom(this->bufferString.data(), lengthNew);
	}

	String ErlPacker::parseIntegerExt() {
		return std::to_string(this->readBits<Uint32>());
	}

	String ErlPacker::parseBinaryExt() {
		String theString{ "\"" };
		Uint32 length = this->readBits<Uint32>();
		auto lengthNew = this->readString(length);
		theString += String{ this->bufferString.data(), lengthNew };
		theString += "\"";
		return theString;
	}

	String ErlPacker::parseFloatExt() {
		const Uint8 FloatLength = 31;
		size_t FloatString = readString(FloatLength);

		if (FloatString == 0) {
			return String{};
		}

		Double number{};
		String nullTerminated{};
		nullTerminated.insert(nullTerminated.begin(), this->bufferString.data(), this->bufferString.data() + FloatLength);

		auto count = sscanf(nullTerminated.data(), "%lf", &number);

		if (count != 1) {
			return String{};
		}

		String returnValue = std::to_string(number);
		return returnValue;
	}

	String ErlPacker::parseListExt() {
		Uint32 length = this->readBits<Uint32>();
		String theArray{};
		theArray += "[";
		theArray += std::move(this->parseArray(length));
		theArray += "]";
		Uint8 theValue = this->readBits<Uint8>();
		if (static_cast<ETFTokenType>(theValue) != ETFTokenType::Nil_Ext) {
			return String{};
		}
		return theArray;
	}

	String ErlPacker::parseNilExt() {
		return "[]";
	}

	String ErlPacker::parseMapExt() {
		Uint32 length = readBits<Uint32>();
		String map{};
		map += "{";
		for (Uint32 i = 0; i < length; ++i) {
			map += std::move(this->singleValueETFToJson());
			map += ":";
			map += std::move(this->singleValueETFToJson());
			if (i < length - 1) {
				map += ",";
			}
		}
		map += "}";
		return map;
	}
}
