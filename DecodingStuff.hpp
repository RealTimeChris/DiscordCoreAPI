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
        returnVector.push_back(*(int*)(void*)&valueOne);
        valueOne = 2;
        returnVector.push_back(*(int*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(int*)(void*)&valueOne);
        valueOne = 48000;
        returnVector.push_back(*(int*)(void*)&valueOne >> 24);
        returnVector.push_back(*(int*)(void*)&valueOne >> 16);
        returnVector.push_back(*(int*)(void*)&valueOne >> 8);
        returnVector.push_back(*(int*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(int*)(void*)&valueOne >> 8);
        returnVector.push_back(*(int*)(void*)&valueOne);
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
        returnVector.push_back(*(int*)(void*)&valueOne >> 24);
        returnVector.push_back(*(int*)(void*)&valueOne >> 16);
        returnVector.push_back(*(int*)(void*)&valueOne >> 8);
        returnVector.push_back(*(int*)(void*)&valueOne);
        returnVector.push_back(*(int*)(void*)&"R");
        returnVector.push_back(*(int*)(void*)&"T");
        returnVector.push_back(*(int*)(void*)&"C");
        returnVector.push_back(*(int*)(void*)&"h");
        returnVector.push_back(*(int*)(void*)&"r");
        returnVector.push_back(*(int*)(void*)&"i");
        returnVector.push_back(*(int*)(void*)&"s");
        valueOne = 0;
        returnVector.push_back(*(int*)(void*)&valueOne >> 24);
        returnVector.push_back(*(int*)(void*)&valueOne >> 16);
        returnVector.push_back(*(int*)(void*)&valueOne >> 8);
        returnVector.push_back(*(int*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(int*)(void*)&valueOne >> 24);
        returnVector.push_back(*(int*)(void*)&valueOne >> 16);
        returnVector.push_back(*(int*)(void*)&valueOne >> 8);
        returnVector.push_back(*(int*)(void*)&valueOne);
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
        auto loadBuffer = loadFile(to_hstring(filePath), to_hstring("porter-robinson-look-at-the-sky-_short-remake_-1627573669964519900_2.opus"));
        int totalSize = 0;
        //auto newVector = inputDecodingData.writeToOutputVector();
        totalSize += loadBuffer.Length();
        ogg_sync_state syncState;
        int error;
        char* filePathNew = new  char[filePath.size() + fileName.size()];
        for (unsigned int x = 0; x < filePath.size(); x += 1) {
            filePathNew[x] = filePath[x];
        }
        for (unsigned int x = 0; x < fileName.size(); x += 1) {
            filePathNew[x] = fileName[x];
        }
        OggOpusFile* opusFile = op_open_file(filePathNew, &error);
        if (error != 0) {
            cout << "ERROR CODE: " << error << endl;
            cout << "Failed to open the memory stream of Opus data!" << endl;
        }
        auto returnVector = applyAudioData(inputDecodingData, loadBuffer);
        opusFile = op_open_memory(returnVector.data(), returnVector.size(), &error);
        if (error != 0) {
            cout << "ERROR CODE: " << error << endl;
            cout << "Failed to open the memory stream of Opus data!" << endl;
        }
        ogg_int64_t  pcmCount = op_pcm_total(opusFile, -2);
        cout << "PCM COUNT: " << pcmCount << endl;
        ogg_sync_init(&syncState);
        ogg_stream_state streamState;
        ogg_stream_init(&streamState, 00);
        auto charPtr = ogg_sync_buffer(&syncState, totalSize);
        memcpy(charPtr, loadBuffer.data(), totalSize);
        ogg_sync_wrote(&syncState, totalSize);
        ogg_page oggPage;
        ogg_sync_pageout(&syncState, &oggPage);
        
        //int numOfPackets = ogg_page_packets(&oggPage);
        //cout << "Number of packets on the current page: " << numOfPackets << endl;
        /*
        if (ogg_stream_pagein(&streamState, &oggPage) != 0) {
            cout << "Failed to submit the page to the bitstream!" << endl;
        };
        ogg_packet oggPacket;
        int error = ogg_stream_packetout(&streamState, &oggPacket);
        if (error != 1) {
            cout << error << endl;
            cout << "Failed to decode the Ogg packet!" << endl;
        }
        vector<unsigned char> charVector;
        
        const unsigned char* charArray = new unsigned char[totalSize];
        memcpy((void*)charArray, (void*)newVector.data(), newVector.size());
        error;
        */
        //OggOpusFile* oggOpusMemory = op_open_memory(charArray, totalSize, &error);
        error;
        OggOpusFile* oggOpusFile = op_open_file((filePath + fileName).c_str(), &error);
		if (error != 0) {
            cout << "ERROR CODE: " << error << endl;
            cout << "Failed to open the memory stream of Opus data!" << endl;
		}
        
        return returnVector;

	}

}

#endif
