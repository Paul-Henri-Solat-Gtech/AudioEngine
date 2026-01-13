#pragma once

#include <fstream>
#include <vector>
#include <iostream>
#include <string>

struct WaveFormat 
{
	char ckID[4]; //riff
	uint32_t cksize; //4+n
	char WAVEID[4]; //wave

	//uint32_t WAVEchunks; olptional
};

struct ChunkFormat
{
	char ckID[4]; //fmt
	uint32_t cksize; //16 or 18 or 40

	uint16_t wFormatTag;
	uint16_t nChannels;
	uint32_t nSamplePerSec;
	uint32_t nAvgBytesPerSec;
	uint16_t nBlockAlign;
	uint16_t wBitsPerSample;

	//WAVE_FORMAT_EXTENSIBLE
	//uint16_t cbSize;
	//uint16_t wValidBitsPerSample;
	//uint32_t dwChannelMask;
	//uint64_t SubFormat;
};

struct ChunkData
{
	char ckID[4];
	uint32_t cksize; 
	std::vector<uint8_t> sampledData;
};

struct WaveHeader 
{
	WaveFormat wave;
	ChunkFormat chunk;
};

class ParserWAV
{
public:
	bool OpenWav(std::string& wavFilePath);
	bool WriteCopy(std::string& outPath);

	bool CreateFile(std::string& fileName);
	bool CreateFileWAV(std::string& fileName, float totalTime);
private:
	WaveHeader header;
	ChunkData data;
};

