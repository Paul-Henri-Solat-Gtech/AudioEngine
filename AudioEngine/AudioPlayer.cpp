#include "AudioPlayer.h"

bool AudioPlayer::Xaudio2Init()
{
    // Initialisation COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) return hr;

    // On créé l'instance de XAudio2 
    //IXAudio2* xaudio2 = nullptr;
    hr = XAudio2Create(&mp_xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return hr;

    // On créé la voix master : elle start Xaudio2 automatiquement (on n'appelle pas xaudio2->Start())
    IXAudio2MasteringVoice* master_voice = nullptr;
    hr = mp_xaudio2->CreateMasteringVoice(&master_voice);
    if (FAILED(hr)) return hr;

    // On créé un WAVEFORMATEX (similaire à la structure d'un fichier WAVE, qui inclut en puissance le Broadcast extension format)
    // Attention WAVEFORMAT (sans EX) est XAUDIO1 (non rétrocompatible)
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 32;
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    // Source voice = un lecteur de buffer audio / playback
    IXAudio2SourceVoice* source_voice = nullptr;
    hr = mp_xaudio2->CreateSourceVoice(&source_voice, &wfx);
    if (FAILED(hr)) return hr;

    // Oscillateur sinusoidal 
    std::vector<float> audio_buffer(wfx.nSamplesPerSec * 5);
    const float freq = 440.0f;
    for (size_t i = 0; i < audio_buffer.size(); ++i) 
    {
        audio_buffer[i] = sinf(2.0f * 3.14159265f * freq * (float)i / wfx.nSamplesPerSec);
    }

    // La structure buffer XAudio2 : on lui donne notre buffer + quelques infos
    XAUDIO2_BUFFER buf = {};
    buf.pAudioData = reinterpret_cast<BYTE*>(audio_buffer.data());
    buf.AudioBytes = audio_buffer.size() * sizeof(float);
    buf.Flags = XAUDIO2_END_OF_STREAM;

    // On donne le buffer à la source 
    source_voice->SubmitSourceBuffer(&buf);

    // Et c'est parti ! 
    source_voice->Start();

    std::cout << "\nInit Xaudio2 succesfull !";

    //----------------------------update
    while (true) {
        XAUDIO2_VOICE_STATE state;
        source_voice->GetState(&state);
        if (state.BuffersQueued == 0) break;
        Sleep(100);
    }
    //----------------------------end
    source_voice->DestroyVoice();
    master_voice->DestroyVoice();
    mp_xaudio2->Release();
    CoUninitialize();

    std::cout << "\nInit Xaudio2 all tasks finished !";

	return true;
}

bool AudioPlayer::Xaudio2PlaySound(std::string& wavFilePath)
{
    // WAV

    ParserWAV newWav;
    newWav.OpenWav(wavFilePath);

    // WAV -> WAVEFORMATEX

    WAVEFORMATEX newWavWfx = {};

    newWavWfx.wFormatTag = newWav.GetWavHeader().chunk.wFormatTag;
    newWavWfx.nChannels = newWav.GetWavHeader().chunk.nChannels;
    newWavWfx.nSamplesPerSec = newWav.GetWavHeader().chunk.nSamplePerSec;
    newWavWfx.wBitsPerSample = newWav.GetWavHeader().chunk.wBitsPerSample;
    newWavWfx.nBlockAlign = newWavWfx.nChannels * (newWavWfx.wBitsPerSample / 8);
    newWavWfx.nAvgBytesPerSec = newWavWfx.nSamplesPerSec * newWavWfx.nBlockAlign;

    // Initialisation COM-----------------------------
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) return hr;
    // On créé l'instance de XAudio2 
    //IXAudio2* xaudio2 = nullptr;
    hr = XAudio2Create(&mp_xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return hr;
    // On créé la voix master : elle start Xaudio2 automatiquement (on n'appelle pas xaudio2->Start())
    IXAudio2MasteringVoice* master_voice = nullptr;
    hr = mp_xaudio2->CreateMasteringVoice(&master_voice);
    if (FAILED(hr)) return hr;
    //---------------------------------------------------

    // WAVEFORMATEX -> hr
    IXAudio2SourceVoice* source_voice = nullptr;
    hr = mp_xaudio2->CreateSourceVoice(&source_voice, &newWavWfx);
    if (FAILED(hr)) return hr;

    // La structure buffer XAudio2 : on lui donne notre buffer + quelques infos------------
    XAUDIO2_BUFFER newBuffer = {};
    newBuffer.pAudioData = reinterpret_cast<BYTE*>(newWav.GetWavData().sampledData.data());
    newBuffer.AudioBytes = newWav.GetWavData().sampledData.size() * newWavWfx.wBitsPerSample; // A CORRIGER
    newBuffer.Flags = XAUDIO2_END_OF_STREAM;
    //---------------------------------------------------

    // Submiting buffer to source_voice
    source_voice->SubmitSourceBuffer(&newBuffer);

    // Start playing sound
    source_voice->Start();

    //----------------------------update
    while (true) 
    {
        XAUDIO2_VOICE_STATE state;
        source_voice->GetState(&state);
        if (state.BuffersQueued == 0) break;
        Sleep(100);
    }
    //----------------------------end
    source_voice->DestroyVoice();
    master_voice->DestroyVoice();
    mp_xaudio2->Release();
    CoUninitialize();

    std::cout << "\nInit Xaudio2 all tasks finished !";

    return true;
}
