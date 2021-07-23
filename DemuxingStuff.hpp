// DemuxingStuff.hpp - Header for the demuxing stuff.
// Jul 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DEMUXING_STUFF_
#define _DEMUXING_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI{ 

    string parseWebm(string bufferString) {
		json::array_t newData = json::parse(bufferString);
		int nextPos = 36; // 36 because 4 + 1 + 31 for the EBML header.
		json info = { {"ebml_version", newData.at(5).get<unsigned int>() },
			{"ebmlReadVersion",newData.at(9).get<unsigned int>()},
			{"maxEBMLIDLength",newData.at(13).get<unsigned int>()},
			{"maxEBMLSizeLength",newData.at(17).get<unsigned int>()},
			{"docType",newData.at(21).get<string>()},
			{"docTypeVersion", newData.at(28).get<unsigned int>()},
			{"docTypeReadVersion",newData.at(32).get<unsigned int>()} };
			//{"segment_length",getElementLength(nextPos + 4, buffer);

		// note that the length here is the number of bytes that make up the length of the element (not the actual total length of the element itself)
		// there isn't an additional number to add (i.e. the acutal length of the element) in this case other than the 4 bytes for the header and the num of bytes
		// that represent the element's length because this is the segment element, which contains the top-level elements. 4 is ok here for top-level elements
		// but some lower-level element headers may not be 4 bytes!
		//let nextElementPos = nextPos + 4 + info["segment_length"].length;
		//let nextElement = getElement(buffer, nextElementPos, nextElementPos + 4, EBMLTopLevelElements, info);
			/*
		for (let i = 0; i < 4; i++) { // change to while loop? using 4 just seemed like a good limit for testing
			if (nextElement == = null) {
				break;
			}
			nextElementPos += (nextElement.length + 4 + nextElement.lengthNumBytes);
			nextElement = getElement(buffer, nextElementPos, nextElementPos + 4, EBMLTopLevelElements, info);
		}
		*/
		return info.dump();
	}

    IBuffer demuxWebA(IBuffer bufferToDemux, __int64 currentPosition, __int64 totalByteSize) {
		IBuffer returnBuffer = bufferToDemux;
		DataReader dataReader{ nullptr };
		dataReader = dataReader.FromBuffer(returnBuffer);
		string newString02;
		vector<uint8_t> newVector;
		newVector.resize(36);
		dataReader.ReadBytes(newVector);
		cout << "THE STRING02: " << endl;
		for (auto value : newVector) {
			cout << std::dec << (int)value << " ";
			newString02 += (char)value;
		}
		string newString03 = parseWebm(newString02);

		cout << "THE STRING: " << newString03 << endl;
		return returnBuffer;
    }

}

#endif