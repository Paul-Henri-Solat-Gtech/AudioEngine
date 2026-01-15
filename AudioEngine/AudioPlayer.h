#pragma once

#include <xaudio2.h>
#include <vector>
#include <iostream>

#include "ParserWAV.h"

class AudioPlayer
{
public:
	bool Xaudio2PlaySound(std::string& wavFilePath);

private:

	// keep sound data in memory
	std::vector<uint8_t> m_audioBuffer;
};

