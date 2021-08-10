// DecodingStuff.hpp - Header for the "decoding stuff".
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DECODING_STUFF_
#define _DECODING_STUFF_

#include "../pch.h"
#include "DemuxingStuff.hpp"

namespace DiscordCoreAPI {

    vector<uint8_t> constructOpusHeader() {
        vector<uint8_t> returnVector;

        returnVector.push_back(*(__int8*)(void*)&"O");
        returnVector.push_back(*(__int8*)(void*)&"p");
        returnVector.push_back(*(__int8*)(void*)&"u");
        returnVector.push_back(*(__int8*)(void*)&"s");
        returnVector.push_back(*(__int8*)(void*)&"H");
        returnVector.push_back(*(__int8*)(void*)&"e");
        returnVector.push_back(*(__int8*)(void*)&"a");
        returnVector.push_back(*(__int8*)(void*)&"d");
        int valueOne = 1;
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 2;
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 48000;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&"O");
        returnVector.push_back(*(__int8*)(void*)&"p");
        returnVector.push_back(*(__int8*)(void*)&"u");
        returnVector.push_back(*(__int8*)(void*)&"s");
        returnVector.push_back(*(__int8*)(void*)&"T");
        returnVector.push_back(*(__int8*)(void*)&"a");
        returnVector.push_back(*(__int8*)(void*)&"g");
        returnVector.push_back(*(__int8*)(void*)&"s");
        valueOne = 7;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        returnVector.push_back(*(__int8*)(void*)&"R");
        returnVector.push_back(*(__int8*)(void*)&"T");
        returnVector.push_back(*(__int8*)(void*)&"C");
        returnVector.push_back(*(__int8*)(void*)&"h");
        returnVector.push_back(*(__int8*)(void*)&"r");
        returnVector.push_back(*(__int8*)(void*)&"i");
        returnVector.push_back(*(__int8*)(void*)&"s");
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        for (auto value : returnVector) {
            cout << "CURRENT VALUE: " << value << endl;
        }
        return returnVector;
    }

    IBuffer loadFile(hstring filePath, hstring fileName) {
        auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
        winrt::Windows::Storage::StorageFile storageFile = folder.GetFileAsync(fileName).get();
        auto returnBuffer = winrt::Windows::Storage::FileIO::ReadBufferAsync(storageFile).get();
        return returnBuffer;
    }

    void saveFile(hstring filePath, hstring fileName, IBuffer readBuffer) {
        auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
        winrt::Windows::Storage::StorageFile storageFile = folder.CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting).get();
        winrt::Windows::Storage::FileIO::WriteBufferAsync(storageFile, readBuffer).get();
    }

    vector<uint8_t> applyAudioData(WebAFile webaFile, IBuffer originalHeaderData) {
        vector<uint8_t> returnVector;
        for (unsigned int x = 0; x < 262; x += 1) {
            returnVector.push_back(originalHeaderData.data()[x]);
        }        
        for (auto value : webaFile.segment.clusters) {
            for (auto value2 : value.simpleBlocks) {
                for (auto value3 : value2.frame) {
                    returnVector.push_back(value3);
                }
            }
        }
        return returnVector;
    }

	vector<uint8_t> decodeOpusData(WebAFile inputDecodingData, string filePath, string fileName) {
        vector<uint8_t> returnVector;

        return returnVector;

	}

}

#endif
