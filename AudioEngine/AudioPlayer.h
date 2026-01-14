#pragma once

#include <xaudio2.h>
#include <vector>
#include <iostream>

#include "ParserWAV.h"

class AudioPlayer
{
public:

	bool Xaudio2Init();
	bool Xaudio2PlaySound(std::string& wavFilePath);

private:

	// Instance Xaudio
	IXAudio2* mp_xaudio2 = nullptr;

};

