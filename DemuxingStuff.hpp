// DemuxingStuff.hpp - Header for the demuxing stuff.
// Jul 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DEMUXING_STUFF_
#define _DEMUXING_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI{ 

	__int8 collectBinaryValue(uint8_t* value) {
		stringstream stream;
		stream << std::dec << std::bitset<8>(*(int*)value).to_ulong();
		return (__int8)stoull(stream.str());
	}

	__int32 collectIdLength(uint8_t value) {
		__int32 idLength = 0;
		if ((value >> 7) == 0b00000001) {
			idLength = 1;
		}
		else if ((value >> 6) == 0b00000001) {
			idLength = 2;
		}
		else if ((value >> 5) == 0b00000001) {
			idLength = 3;
		}
		else if ((value >> 4) == 0b00000001) {
			idLength = 4;
		}
		return idLength;
	}

	__int32 collectContentSizeLength(uint8_t value) {
		__int32 contentSizeLength = 0;
		if ((value >> 7) == 0b00000001) {
			contentSizeLength = 1;
		}
		else if ((value >> 6) == 0b00000001) {
			contentSizeLength = 2;
		}
		else if ((value >> 5) == 0b00000001) {
			contentSizeLength = 3;
		}
		else if ((value >> 4) == 0b00000001) {
			contentSizeLength = 4;
		}
		else if ((value >> 3) == 0b00000001) {
			contentSizeLength = 5;
		}
		else if ((value >> 2) == 0b00000001) {
			contentSizeLength = 6;
		}
		else if ((value >> 1) == 0b00000001) {
			contentSizeLength = 7;
		}
		else if ((value >> 0) == 0b00000001) {
			contentSizeLength = 8;
		}
		return contentSizeLength;
	}

	__int32 computeIdValue(vector<__int8> binaryValueNew) {
		__int32 binaryValue;

		bitset<8 * sizeof(binaryValueNew.data())> bitValue02;
		__int32 counterX = 0;
		for (int x = (int)binaryValueNew.size() - 1; x >= 0; x -= 1) {
			bitset<8> newBitValue(binaryValueNew.at(x));
			__int32 counterY = 0;
			for (int y = 0; y < newBitValue.size(); y += 1) {
				bitValue02[counterX * 8 + counterY] = newBitValue[y];
				counterY += 1;
			}
			counterX += 1;
		}
		binaryValue = (__int32)bitValue02.to_ulong();
		return binaryValue;
	}

	__int32 computeSizeValue(vector<__int8> binaryValueNew) {
		__int32 binaryValue;

		bitset<8 * sizeof(binaryValueNew.data())> bitValue02;
		__int32 counterX = 0;
		for (int x = (int)binaryValueNew.size() - 1; x >= 0; x -= 1) {
			bitset<8> newBitValue(binaryValueNew.at(x));
			__int32 counterY = 0;
			for (int y = 0; y < newBitValue.size(); y += 1) {
				bitValue02[counterX * 8 + counterY] = newBitValue[y];
				counterY += 1;
			}
			counterX += 1;
		}
		bitValue02[binaryValueNew.size() * 7] = 0;
		binaryValue = (__int32)bitValue02.to_ullong();
		return binaryValue;
	}

	class FileElement {
	public:
		FileElement() {};

		FileElement(vector<uint8_t>* inputVector,  __int32* startingPosition, bool isCombinedNew, string elementNameNew, bool isData){
			this->elementName = elementNameNew;
			this->elementStartingPosition = *startingPosition;
			this->isCombined = isCombinedNew;
			this->elementIdLength = collectIdLength(inputVector->at(*startingPosition));
			for (int x = 0; x < this->elementIdLength; x += 1) {
				this->elementIdRaw.push_back(inputVector->at(*startingPosition));
				*startingPosition += 1;
			}
			this->elementId = computeIdValue(this->elementIdRaw);
			this->elementContentSizeLength = collectContentSizeLength(inputVector->at(*startingPosition));
			for (int x = 0; x < this->elementContentSizeLength; x += 1) {
				this->elementContentSizeRaw.push_back(inputVector->at(*startingPosition));
				*startingPosition += 1;
			}
			this->elementContentSize = computeSizeValue(this->elementContentSizeRaw);
			if (isData) {
				for (int x = 0; x < this->elementContentSize; x += 1) {
					this->theDataRaw.push_back(inputVector->at(*startingPosition));
					*startingPosition += 1;
				}
			}			
			this->elementSize = this->elementIdLength + this->elementContentSizeLength + this->elementContentSize;
			this->showElementIdAndContentSize();
		}

		void showElementIdAndContentSize(){
			cout << "Element Name: " + elementName + ", ";
			cout << "Element Id: " << dec << this->elementId << ", ";
			cout << "Content Size: " << dec << this->elementContentSize << " Bytes." << endl;
			
		}

		void showCharElement() {
			if (!isCombined) {
				int currentIndex = 0;
				cout << "Data: ";
				for (auto value : this->theDataRaw) {
					if (currentIndex < (theDataRaw.size() - 1)) {
						cout << (char)bitset<8>(value).to_ulong();
					}
					else {
						cout << (char)bitset<8>(value).to_ulong() << endl;
					}
					currentIndex += 1;
				}
			}
			else {
				bitset<8 * sizeof(theDataRaw.data())>bitValue;
				__int64 counterX = 0;
				for (__int64 x = (__int64)theDataRaw.size() - 1; x >= 0; x -= 1) {
					bitset<8> newBitValue(theDataRaw.at(x));
					__int64 counterY = 0;
					for (int y = 0; y < newBitValue.size(); y += 1) {
						bitValue[counterX * 8 + counterY] = newBitValue[y];
						counterY += 1;
					}
					counterX += 1;
				}
				unsigned long long newInt = bitValue.to_ullong();
				cout << "Data: ";
				cout << (char)newInt << endl;
			}
		}

		void showHexElement() {
			if (!isCombined) {
				int currentIndex = 0;
				std::cout << "Data: ";
				cout << theDataRaw.size() << endl;
				for (auto value : theDataRaw) {
					if (currentIndex < (theDataRaw.size() - 1)) {
						std::cout << "0x" << std::hex << bitset<8>(value).to_ulong() << ", ";
					}
					else {
						std::cout << "0x" << std::hex << bitset<8>(value).to_ulong() << endl;
					}
					currentIndex += 1;
				}
			}
			else {
				bitset<8 * sizeof(theDataRaw.data())> bitValue;
				__int64 counterX = 0;
				for (__int64 x = (__int64)theDataRaw.size() - 1; x >= 0; x -= 1) {
					bitset<8> newBitValue(theDataRaw.at(x));
					__int64 counterY = 0;
					for (int y = 0; y < newBitValue.size(); y += 1) {
						bitValue[counterX * 8 + counterY] = newBitValue[y];
						counterY += 1;
					}
					counterX += 1;
				}
				unsigned long long newInt = bitValue.to_ullong();
				std::cout << "Data: ";
				std::cout << "0x" << hex << newInt << endl;

			}
		}

		void showFloatElement() {
			if (!isCombined) {
				int currentIndex = 0;
				std::cout << "Data: ";
				for (auto value : theDataRaw) {
					__int32* newInt = new __int32;
					if (currentIndex < (theDataRaw.size() - 1)) {
						*newInt = bitset<8>(value).to_ulong();
						std::cout << *(float*)(void*)newInt;
					}
					else {
						*newInt = bitset<8>(value).to_ulong();
						std::cout << *(float*)(void*)newInt << endl;
					}
					currentIndex += 1;
				}
			}
			else {
				bitset<8 * sizeof(theDataRaw.data())> bitValue;
				__int64 counterX = 0;
				for (__int64 x = (__int64)theDataRaw.size() - 1; x >= 0; x -= 1) {
					bitset<8> newBitValue(theDataRaw.at(x));
					__int64 counterY = 0;
					for (int y = 0; y < newBitValue.size(); y += 1) {
						bitValue[counterX * 8 + counterY] = newBitValue[y];
						counterY += 1;
					}
					counterX += 1;
				}
				__int32 newInt = (__int32)bitValue.to_ulong();
				std::cout << "Data: ";
				__int32* newIntPtr = new __int32;
				*newIntPtr = newInt;
				std::cout << *(float*)(void*)newIntPtr << endl;

			}
		}

		template<typename T>
		void showIntElement() {
			if (theDataRaw.size() <= 8) {
				if (!isCombined) {
					int currentIndex = 0;
					cout << "Data: ";
					for (auto value : theDataRaw) {
						if (currentIndex < (theDataRaw.size() - 1)) {
							cout << dec << (T)bitset<8>(value).to_ullong() << ", ";
						}
						else {
							cout << dec << (T)bitset<8>(value).to_ullong() << endl;
						}
						currentIndex += 1;
					}
				}
				else {
					bitset<8 * sizeof(theDataRaw.data())> bitValue;
					__int64 counterX = 0;
					for (int x = (int)theDataRaw.size() - 1; x >= 0; x -= 1) {
						bitset<8> newBitValue(theDataRaw.at(x));
						__int64 counterY = 0;
						for (int y = 0; y < newBitValue.size(); y += 1) {
							bitValue[counterX * 8 + counterY] = newBitValue[y];
							counterY += 1;
						}
						counterX += 1;
					}
					unsigned long long newInt = bitValue.to_ullong();
					cout << "Data: ";
					cout << dec << (T)newInt << endl;
				}
			}
			return;
		}

		void writeToOutputVector(vector<uint8_t>* outputVector, int* bytesWritten) {
			for (auto value : this->elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->theDataRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
		}

		bool isCombined{ false };
		__int32 elementStartingPosition{ 0 };
		__int32 elementIdLength{ 0 };
		__int32 elementId{ 0 };
		__int32 elementContentSizeLength{ 0 };
		__int32 elementContentSize{ 0 };
		__int32 elementSize{ 0 };
		string elementName{ "" };
		vector<__int8> elementIdRaw{};
		vector<__int8> elementContentSizeRaw{};
		vector<__int8> theDataRaw{};
	};
	
	struct EBMLHead : FileElement {
		EBMLHead(vector<uint8_t>* inputVector, int* startingPosition) :FileElement(inputVector, startingPosition, false, "EBML Head", false) {
			this->ebmlVersion = FileElement(inputVector, startingPosition, false, "EBML Version", true);
			this->ebmlVersion.showIntElement<int>();
			this->ebmlReadVersion = FileElement(inputVector, startingPosition, false, "EBML Read Version", true);
			this->ebmlReadVersion.showIntElement<int>();
			this->maxEbmlIdLength = FileElement(inputVector, startingPosition, false, "Max EBML Id Length", true);
			this->maxEbmlIdLength.showIntElement<int>();
			this->maxEbmlSizeLength = FileElement(inputVector, startingPosition, false, "Max EBML Size Length", true);
			this->maxEbmlSizeLength.showIntElement<int>();
			this->documentType = FileElement(inputVector, startingPosition, false, "Document Type", true);
			this->documentType.showCharElement();
			this->documentTypeVersion = FileElement(inputVector, startingPosition, false, "Document Type Version", true);
			this->documentTypeVersion.showIntElement<int>();
			this->documentTypeReadVersion = FileElement(inputVector, startingPosition, false, "Document Type Version", true);
			this->documentTypeReadVersion.showIntElement<int>();
		}
		FileElement ebmlVersion;
		FileElement ebmlReadVersion;
		FileElement maxEbmlIdLength;
		FileElement maxEbmlSizeLength;
		FileElement documentType;
		FileElement documentTypeVersion;
		FileElement documentTypeReadVersion;
	};
	
	struct SeekEntry01: FileElement {
		SeekEntry01(vector<uint8_t>* inputVector, int* startingPosition) : FileElement(inputVector, startingPosition, false, "Seek Entry 01", false) {
			this->seekId = FileElement(inputVector, startingPosition, false, "Seek Id 01", true);
			this->seekId.showHexElement();
			this->seekPosition = FileElement(inputVector, startingPosition, true, "Seek Position 01", true);
			this->seekPosition.showIntElement<__int64>();
		}
		void writeToOutputVector(vector<uint8_t>* outputVector, int* bytesWritten) {
			for (auto value : this->elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.theDataRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.theDataRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
		}
		FileElement seekId;
		FileElement seekPosition;
	};

	struct SeekEntry02 : FileElement {
		SeekEntry02(vector<uint8_t>* inputVector, int* startingPosition) : FileElement(inputVector, startingPosition, false, "Seek Entry 02", false) {
			this->seekId = FileElement(inputVector, startingPosition, false, "Seek Id 02", true);
			this->seekId.showHexElement();
			this->seekPosition = FileElement(inputVector, startingPosition, true, "Seek Position 02", true);
			this->seekPosition.showIntElement<__int64>();
		}
		void writeToOutputVector(vector<uint8_t>* outputVector, int* bytesWritten) {
			for (auto value : this->elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.theDataRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.theDataRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
		}
		FileElement seekId;
		FileElement seekPosition;
	};

	struct SeekEntry03 : FileElement {
		SeekEntry03(vector<uint8_t>* inputVector, int* startingPosition) : FileElement(inputVector, startingPosition, false, "Seek Entry 03", false) {
			this->seekId = FileElement(inputVector, startingPosition, false, "Seek Id 03", true);
			this->seekId.showHexElement();
			this->seekPosition = FileElement(inputVector, startingPosition, true, "Seek Position 03", true);
			this->seekPosition.showIntElement<__int64>();
		}
		void writeToOutputVector(vector<uint8_t>* outputVector, int* bytesWritten) {
			for (auto value : this->elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekId.theDataRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.elementIdRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.elementContentSizeRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
			for (auto value : this->seekPosition.theDataRaw) {
				outputVector->push_back(value);
				*bytesWritten += 1;
			}
		}
		FileElement seekId;
		FileElement seekPosition;
	};

	struct SeekHead: FileElement {
		SeekHead(vector<uint8_t>* inputVector, int* startingPosition) : FileElement(inputVector, startingPosition, false, "Seek Head", false), seekEntry01(inputVector, startingPosition), seekEntry02(inputVector, startingPosition), seekEntry03(inputVector, startingPosition) {
		}
		SeekEntry01 seekEntry01;
		SeekEntry02 seekEntry02;
		SeekEntry03 seekEntry03;
	};

	struct SegmentInformation: FileElement {
		SegmentInformation(vector<uint8_t>* inputVector, int* startingPosition): FileElement(inputVector, startingPosition,false , "Segment Information", false) {
			this->timestampScale = FileElement(inputVector, startingPosition, true, "Timestamp Scale", true);
			this->timestampScale.showIntElement<__int32>();
			this->duration = FileElement(inputVector, startingPosition, true, "Duration", true);
			this->duration.showFloatElement();
			this->mulitplexingApp = FileElement(inputVector, startingPosition, false, "Multiplexing Application", true);
			this->mulitplexingApp.showCharElement();
			this->writingApp = FileElement(inputVector, startingPosition, false, "Writing Application", true);
			this->writingApp.showCharElement();
		}

		FileElement timestampScale;
		FileElement duration;
		FileElement mulitplexingApp;
		FileElement writingApp;
	};

	struct AudioTrack: FileElement {
		AudioTrack() {}
		AudioTrack(vector<uint8_t>* inputVector, int* startingPosition) : FileElement(inputVector, startingPosition, false, "Audio Track", false) {
			this->samplingFrequency = FileElement(inputVector, startingPosition, true, "Sample Frequency", true);
			this->samplingFrequency.showIntElement<int>();
			this->channels = FileElement(inputVector, startingPosition, false, "Channels", true);
			this->channels.showIntElement<int>();
			this->bitDepth = FileElement(inputVector, startingPosition, false, "Bit Depth", true);
			this->bitDepth.showIntElement<int>();
		}
		FileElement samplingFrequency;
		FileElement channels;
		FileElement bitDepth;
	};

	struct Track : FileElement {
		Track(vector<uint8_t>* inputVector, int* startingPosition): FileElement(inputVector, startingPosition,false,  "Track", false) {
			this->trackNumber = FileElement(inputVector, startingPosition, false, "Track Number", true);
			this->trackNumber.showIntElement<int>();
			this->trackUID = FileElement(inputVector, startingPosition, true, "Track UID", true);
			this->trackUID.showIntElement<__int64>();
			this->trackType = FileElement(inputVector, startingPosition, false, "Track Type", true);
			this->trackType.showCharElement();
			this->lacingFlag = FileElement(inputVector, startingPosition, false, "Lacing Flag", true);
			this->lacingFlag.showIntElement<int>();
			this->language = FileElement(inputVector, startingPosition, false, "Language", true);
			if (this->language.elementId != 0x22b59c) {
				*startingPosition -= (this->language.theDataRaw.size() + (int)this->language.elementIdRaw.size() + (int)this->language.elementContentSizeRaw.size());
				this->language.elementContentSizeRaw.resize(0);
				this->language.elementIdRaw.resize(0);
				this->language.theDataRaw.resize(0);
			}
			this->language.showCharElement();
			this->codecId = FileElement(inputVector, startingPosition, false, "Codec Id", true);
			this->codecId.showCharElement();
			this->codecPrivateData = FileElement(inputVector, startingPosition, false, "Codec's Private Data", true);
			this->codecPrivateData.showCharElement();
			this->codecInherentDelay = FileElement(inputVector, startingPosition, true, "Codec's Inherent Delay", true);
			this->codecInherentDelay.showIntElement<int>();
			this->seekPreRoll = FileElement(inputVector, startingPosition, true, "Seek Pre-Roll", true);
			this->seekPreRoll.showIntElement<int>();
			if (this->seekPreRoll.elementId != 0x56bb) {
				this->theDataRaw.clear();
			}
			this->audioTrack = AudioTrack(inputVector, startingPosition);

		}
		FileElement trackNumber;
		FileElement trackUID;
		FileElement trackType;
		FileElement lacingFlag;
		FileElement language;
		FileElement codecId;
		FileElement codecPrivateData;
		FileElement codecInherentDelay;
		FileElement seekPreRoll;
		AudioTrack audioTrack;
	};

	struct Tracks : FileElement {
		Tracks(vector<uint8_t>* inputVector, int* startingPosition) : FileElement(inputVector, startingPosition, false, "Tracks", false), track(inputVector, startingPosition) {
		}
		Track track;
	};

	struct Cue : FileElement {
		Cue(vector<uint8_t>* inputVector, int* startingPosition): FileElement(inputVector, startingPosition, false, "Cue", true) {
			this->showHexElement();
		}
	};

	struct Cues: FileElement {
		Cues(){}
		Cues(vector<uint8_t>* inputVector, int* startingPosition): FileElement(inputVector, startingPosition,false, "Cues", false) {
			int currentPosition = 0;
			while (currentPosition < this->elementContentSize) {
				Cue currentCueCollector = Cue(inputVector, startingPosition);
				this->cues.push_back(currentCueCollector);
				currentPosition += currentCueCollector.elementSize;
			}
		}
		vector<Cue> cues;
	};

	struct SimpleBlock : FileElement {
		SimpleBlock(vector<uint8_t>* inputVector, int* startingPosition) :FileElement(inputVector, startingPosition,false, "Simple Block", true) {
			if (this->elementId != 231 && this->elementId == 0xa3) {
				int currentPosition = 0;
				this->trackNumber.push_back(collectBinaryValue((uint8_t*)&this->theDataRaw.at(currentPosition)));
				currentPosition += 1;
				for (unsigned int x = 0; x < 2; x += 1) {
					this->relativeTimestamp.push_back(collectBinaryValue((uint8_t*)&this->theDataRaw.at(currentPosition)));
					currentPosition += 1;
				}
				this->flags.push_back(collectBinaryValue((uint8_t*)&this->theDataRaw.at(currentPosition)));
				currentPosition += 1;
				for (int x = currentPosition; x < this->elementContentSize; x += 1) {
					this->frame.push_back(collectBinaryValue((uint8_t*)&this->theDataRaw.at(currentPosition)));
					currentPosition += 1;
				}
				this->elementSize = this->elementIdLength + this->elementContentSizeLength + (int)this->frame.size() + (int)this->trackNumber.size() + (int)this->flags.size() + (int)this->relativeTimestamp.size();
			}
		}
		vector<__int8> trackNumber;
		vector<__int8> relativeTimestamp;
		vector<__int8> flags;
		vector<__int8> frame;
	};

	struct Cluster : FileElement {
		Cluster() {}
		Cluster(vector<uint8_t>* inputVector, int* startingPosition):FileElement(inputVector, startingPosition, false, "Cluster", false) {
			int currentPosition = 0;
			currentPosition += this->elementIdLength + this->elementContentSizeLength;
			this->clusterTimestamp = FileElement(inputVector, startingPosition, false, "Cluster Timestamp", true);
			while (currentPosition < this->elementContentSize) {
				SimpleBlock newSimpleBlock(inputVector, startingPosition);
				simpleBlocks.push_back(newSimpleBlock);
				currentPosition += newSimpleBlock.elementSize;
			}
		}
		FileElement clusterTimestamp;
		vector<SimpleBlock> simpleBlocks;
	};

	struct Segment: FileElement {
		Segment(vector<uint8_t>* inputVector, int* startingPosition) : FileElement(inputVector, startingPosition, false, "Segment", false), seekHead(inputVector, startingPosition), segmentInformation(inputVector, startingPosition), tracks(inputVector, startingPosition), cues(inputVector, startingPosition) {
			int currentPosition = 0;
			currentPosition += this->elementIdLength + this->elementContentSizeLength + this->seekHead.elementSize + this->segmentInformation.elementSize + this->tracks.elementSize + this->cues.elementSize;
			int lastElementSize = 0;
			while (currentPosition < this->elementSize) {
				Cluster newCluster = Cluster(inputVector, startingPosition);
				this->clusters.push_back(newCluster);
				currentPosition += newCluster.elementSize;
				lastElementSize = newCluster.elementSize;
				cout << "INPUT VECTOR SIZE: " << inputVector->size() << endl;
				cout << "STARTING POSITION: " << *startingPosition << endl;
			}
		}
		SeekHead seekHead;
		SegmentInformation segmentInformation;
		Tracks tracks;
		Cues cues;
		vector<Cluster> clusters;
	};

	struct WebAFile {
		WebAFile(vector<uint8_t>* inputVector, int* startingPosition) :  ebmlHead(inputVector, startingPosition), segment(inputVector, startingPosition) {

		}

		vector<uint8_t> writeToOutputVector() {
			int bytesWritten;
			this->ebmlHead.writeToOutputVector(&outputVector, &bytesWritten);
			this->ebmlHead.ebmlVersion.writeToOutputVector(&outputVector, &bytesWritten);
			this->ebmlHead.ebmlReadVersion.writeToOutputVector(&outputVector, &bytesWritten);
			this->ebmlHead.maxEbmlIdLength.writeToOutputVector(&outputVector, &bytesWritten);
			this->ebmlHead.maxEbmlSizeLength.writeToOutputVector(&outputVector, &bytesWritten);
			this->ebmlHead.documentType.writeToOutputVector(&outputVector, &bytesWritten);
			this->ebmlHead.documentTypeVersion.writeToOutputVector(&outputVector, &bytesWritten);
			this->ebmlHead.documentTypeReadVersion.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.seekHead.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.seekHead.seekEntry01.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.seekHead.seekEntry02.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.seekHead.seekEntry03.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.segmentInformation.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.segmentInformation.timestampScale.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.segmentInformation.duration.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.segmentInformation.mulitplexingApp.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.segmentInformation.writingApp.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.trackNumber.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.trackUID.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.trackType.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.lacingFlag.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.language.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.codecId.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.codecPrivateData.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.codecInherentDelay.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.seekPreRoll.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.audioTrack.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.audioTrack.samplingFrequency.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.audioTrack.channels.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.tracks.track.audioTrack.bitDepth.writeToOutputVector(&outputVector, &bytesWritten);
			this->segment.cues.writeToOutputVector(&outputVector, &bytesWritten);
			for (auto value : this->segment.cues.cues) {
				value.writeToOutputVector(&outputVector, &bytesWritten);
			}
			for (auto value : this->segment.clusters) {
				value.writeToOutputVector(&outputVector, &bytesWritten);
				value.clusterTimestamp.writeToOutputVector(&outputVector, &bytesWritten);
				for (auto value2 : value.simpleBlocks) {
					value2.writeToOutputVector(&outputVector, &bytesWritten);
				}
			}
			return outputVector;
		}

		EBMLHead ebmlHead;
		Segment segment;
		vector<uint8_t> outputVector;
	};

    WebAFile demuxWebA(vector<uint8_t> bufferVector) {
		stringstream stringStream;
		int startingPosition = 0;
		WebAFile webmFile(&bufferVector, &startingPosition);
		return webmFile;
	}

	static int get_format_from_sample_fmt(const char** fmt,
		enum AVSampleFormat sample_fmt)
	{
		int i;
		struct sample_fmt_entry {
			enum AVSampleFormat sample_fmt; const char* fmt_be, * fmt_le;
		} sample_fmt_entries[] = {
			{ AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
			{ AV_SAMPLE_FMT_S16, "s16be", "s16le" },
			{ AV_SAMPLE_FMT_S32, "s32be", "s32le" },
			{ AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
			{ AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
		};
		*fmt = NULL;

		for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
			struct sample_fmt_entry* entry = &sample_fmt_entries[i];
			if (sample_fmt == entry->sample_fmt) {
				*fmt = AV_NE(entry->fmt_be, entry->fmt_le);
				return 0;
			}
		}

		fprintf(stderr,
			"sample format %s is not supported as output format\n",
			av_get_sample_fmt_name(sample_fmt));
		return -1;
	}

	static void decode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame,
		FILE* outfile)
	{
		int i, ch;
		int ret, data_size;

		/* send the packet with the compressed data to the decoder */
		ret = avcodec_send_packet(dec_ctx, pkt);
		if (ret < 0) {
			fprintf(stderr, "Error submitting the packet to the decoder\n");
			exit(1);
		}

		/* read all the output frames (in general there may be any number of them */
		while (ret >= 0) {
			ret = avcodec_receive_frame(dec_ctx, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return;
			else if (ret < 0) {
				fprintf(stderr, "Error during decoding\n");
				exit(1);
			}
			data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
			if (data_size < 0) {
				/* This should not occur, checking just for paranoia */
				fprintf(stderr, "Failed to calculate data size\n");
				exit(1);
			}
			for (i = 0; i < frame->nb_samples; i++)
				for (ch = 0; ch < dec_ctx->channels; ch++)
					fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);
		}
	}
	/*
	void decodeWebAFile(WebAFile fileToDecode)
	{
		const int AUDIO_REFILL_THRESH = 4096;
		int AUDIO_INBUF_SIZE = 0;
		for (auto value : fileToDecode.segment.clusters) {
			AUDIO_INBUF_SIZE += value.elementContentSize;
		}
		const char* outfilename;
		const AVCodec* codec;
		AVCodecContext* c = NULL;
		AVCodecParserContext* parser = NULL;
		int len, ret;
		FILE* outfile;
		uint8_t* inbuf = new uint8_t[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
		uint8_t* data;
		size_t   data_size;
		AVPacket* pkt;
		AVFrame* decoded_frame = NULL;
		enum AVSampleFormat sfmt;
		int n_channels = 0;
		const char* fmt;
		string outputFile = "C:/Users/Chris/Downloads/Test.pcm";
		if (1<= 2) {
			fprintf(stderr, "Usage: %s <input file> <output file>\n", outputFile.c_str());
		}
		outfilename = outputFile.c_str();

		pkt = av_packet_alloc();

		 find the MPEG audio decoder 
		codec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
		if (!codec) {
			fprintf(stderr, "Codec not found\n");
			exit(1);
		}

		parser = av_parser_init(codec->id);
		if (!parser) {
			fprintf(stderr, "Parser not found\n");
			exit(1);
		}

		c = avcodec_alloc_context3(codec);
		if (!c) {
			fprintf(stderr, "Could not allocate audio codec context\n");
			exit(1);
		}

		// open it 
		if (avcodec_open2(c, codec, NULL) < 0) {
			fprintf(stderr, "Could not open codec\n");
			exit(1);
		}

		outfile = fopen(outfilename, "wb");
		if (!outfile) {
			av_free(c);
			exit(1);
		}

		// decode until eof 
		data = inbuf;
		int totalSize = 0;
		for (auto value : fileToDecode.segment.clusters) {
			for (auto value2 : value.simpleBlocks) {
				memcpy(inbuf, &value2.elementId, value2.elementIdLength);
				totalSize += value2.elementIdLength;
				cout << "ELEMENT ID: " << value2.elementId << endl;
				memcpy(inbuf, &value2.elementContentSize, value2.elementContentSizeLength);
				totalSize += value2.elementContentSizeLength;
				cout << "ELEMENT CONTENT SIZE: " << value2.elementContentSize << endl;
				memcpy(inbuf, &value2.trackNumber, value2.trackNumber.size());
				totalSize += (int)value2.trackNumber.size();
				for (auto value3 : value2.trackNumber) {
					cout << "TRACK NUMBER: " << value3 << endl;
				}
				memcpy(inbuf, &value2.relativeTimestamp, value2.relativeTimestamp.size());
				totalSize += (int)value2.relativeTimestamp.size();
				for (auto value3 : value2.relativeTimestamp) {
					cout << "RELATIVE TIME STAMP: " << value3 << endl;
				}
				memcpy(inbuf, &value2.flags, value2.flags.size());
				totalSize += (int)value2.flags.size();
				for (auto value3 : value2.flags) {
					cout << "FLAGS: " << value3 << endl;
				}
				memcpy(inbuf, &value2.frame, value2.frame.size());
				totalSize += (int)value2.frame.size();
			}
		}
		data_size = totalSize;

		while (data_size > 0) {
			if (!decoded_frame) {
				if (!(decoded_frame = av_frame_alloc())) {
					fprintf(stderr, "Could not allocate audio frame\n");
					exit(1);
				}
			}

			ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
				data, data_size,
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
			if (ret < 0) {
				fprintf(stderr, "Error while parsing\n");
				exit(1);
			}
			data += ret;
			data_size -= ret;

			if (pkt->size)
				decode(c, pkt, decoded_frame, outfile);

			if (data_size < AUDIO_REFILL_THRESH) {
				memmove(inbuf, data, data_size);
				data = inbuf;
				memcpy(data + data_size, inbuf, AUDIO_INBUF_SIZE - data_size);
				len = AUDIO_INBUF_SIZE - data_size;
				if (len > 0)
					data_size += len;
			}
		}

		// flush the decoder 
		pkt->data = NULL;
		pkt->size = 0;
		decode(c, pkt, decoded_frame, outfile);

		// print output pcm infomations, because there have no metadata of pcm 
		sfmt = c->sample_fmt;

		if (av_sample_fmt_is_planar(sfmt)) {
			const char* packed = av_get_sample_fmt_name(sfmt);
			printf("Warning: the sample format the decoder produced is planar "
				"(%s). This example will output the first channel only.\n",
				packed ? packed : "?");
			sfmt = av_get_packed_sample_fmt(sfmt);
		}

		n_channels = c->channels;
		if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
			goto end;

		printf("Play the output audio file with the command:\n"
			"ffplay -f %s -ac %d -ar %d %s\n",
			fmt, n_channels, c->sample_rate,
			outfilename);
	end:
		fclose(outfile);

		avcodec_free_context(&c);
		av_parser_close(parser);
		av_frame_free(&decoded_frame);
		av_packet_free(&pkt);

		return;
	}
	*/
	void decodeWebAFile(WebAFile fileToDecode)
	{
		string filename = "C:/Users/Chris/Downloads/porter robinson - look at the sky (short remake) 1627533675529554600.webm";
		ifstream in_file(filename.c_str(), ios::binary);
		in_file.seekg(0, ios::end);
		int AUDIO_INBUF_SIZE = (int)in_file.tellg();
		cout << "AUDIO INBUF SIZE: " << AUDIO_INBUF_SIZE << endl;
		int AUDIO_REFILL_THRESH = 4096;
		in_file.close();
		const AVCodec* codec;
		AVCodecContext* c = NULL;
		AVCodecParserContext* parser = NULL;
		int len, ret;
		FILE* f, * outfile;
		uint8_t* inbuf = new uint8_t[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
		uint8_t* data;
		size_t   data_size;
		AVPacket* pkt;
		AVFrame* decoded_frame = NULL;
		enum AVSampleFormat sfmt;
		int n_channels = 0;
		const char* fmt;

		if constexpr (1==1){
			fprintf(stderr, "Usage: %s <input file> <output file>\n", filename.c_str());
		}
		
		string outfilename = "C:/Users/Chris/Downloads/TestOutput.pcm";

		pkt = av_packet_alloc();

		/* find the MPEG audio decoder */
		codec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
		if (!codec) {
			fprintf(stderr, "Codec not found\n");
			exit(1);
		}

		parser = av_parser_init(codec->id);
		if (!parser) {
			fprintf(stderr, "Parser not found\n");
			exit(1);
		}

		c = avcodec_alloc_context3(codec);
		if (!c) {
			fprintf(stderr, "Could not allocate audio codec context\n");
			exit(1);
		}

		/* open it */
		if (avcodec_open2(c, codec, NULL) < 0) {
			fprintf(stderr, "Could not open codec\n");
			exit(1);
		}

		auto error = fopen_s(&f, filename.c_str(), "rb");
		if (error < 0) {
			fprintf(stderr, "Could not open %s\n", filename.c_str());
			exit(1);
		}
		error = fopen_s(&outfile, outfilename.c_str(), "wb");
		if (error < 0) {
			av_free(c);
			exit(1);
		}

		/* decode until eof */
		data = inbuf;
		data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE, f);

		while (data_size > 0) {
			if (!decoded_frame) {
				if (!(decoded_frame = av_frame_alloc())) {
					fprintf(stderr, "Could not allocate audio frame\n");
					exit(1);
				}
			}

			ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, (int)data_size, (int64_t)AV_NOPTS_VALUE, (int64_t)AV_NOPTS_VALUE, (int64_t)0);
			if (ret < 0) {
				fprintf(stderr, "Error while parsing\n");
				exit(1);
			}
			data += ret;
			data_size -= ret;

			if (pkt->size)
				decode(c, pkt, decoded_frame, outfile);

			if (data_size < AUDIO_REFILL_THRESH) {
				memmove(inbuf, data, data_size);
				data = inbuf;
				len = (int)fread(data + data_size, 1, (int)AUDIO_INBUF_SIZE - (int)data_size, f);
				if (len > 0){
					data_size += len;
				}
					
			}
		}

		/* flush the decoder */
		pkt->data = NULL;
		pkt->size = 0;
		decode(c, pkt, decoded_frame, outfile);

		/* print output pcm infomations, because there have no metadata of pcm */
		sfmt = c->sample_fmt;

		if (av_sample_fmt_is_planar(sfmt)) {
			const char* packed = av_get_sample_fmt_name(sfmt);
			printf("Warning: the sample format the decoder produced is planar "
				"(%s). This example will output the first channel only.\n",
				packed ? packed : "?");
			sfmt = av_get_packed_sample_fmt(sfmt);
		}

		n_channels = c->channels;
		if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
			goto end;

		printf("Play the output audio file with the command:\nffplay -f %s -ac %d -ar %d %s\n", fmt, n_channels, c->sample_rate, outfilename.c_str());
	end:
		fclose(outfile);
		fclose(f);

		avcodec_free_context(&c);
		av_parser_close(parser);
		av_frame_free(&decoded_frame);
		av_packet_free(&pkt);

		return;
	}
}

#endif