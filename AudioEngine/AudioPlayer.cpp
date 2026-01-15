#include "AudioPlayer.h"

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
    // On cree l'instance de XAudio2 
    //IXAudio2* xaudio2 = nullptr;
    IXAudio2* mp_xaudio2 = nullptr;
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
    m_audioBuffer = newWav.GetWavData().sampledData;

    XAUDIO2_BUFFER newBuffer = {};
    newBuffer.pAudioData = m_audioBuffer.data();
    newBuffer.AudioBytes = (UINT32)m_audioBuffer.size();
    newBuffer.Flags = XAUDIO2_END_OF_STREAM;
    //---------------------------------------------------

    // Submiting buffer to source_voice
    source_voice->SubmitSourceBuffer(&newBuffer);

    // Start playing sound
    source_voice->Start();
    std::cout << "\n" << wavFilePath << " Start playing !";
    float ms = 0;
    //----------------------------update
    while (true) 
    {
        ms += 1;
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

    std::cout << "\n" << wavFilePath << " Finished playing !" << ms;

    return true;
}
