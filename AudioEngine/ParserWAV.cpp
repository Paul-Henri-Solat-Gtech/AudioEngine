#include "ParserWAV.h"

bool ParserWAV::OpenWav(std::string& wavFilePath)
{
    // Ouverture fichier WAV
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, wavFilePath.c_str(), "rb");  // Ouvrir en mode binaire
    if (err != 0 || file == nullptr) 
    {
        std::cerr << "Impossible d'ouvrir le fichier." << std::endl;
        return false;
    }

    // Lecture du header
    fread(&header.wave, sizeof(WaveFormat), 1, file);

    if (ferror(file)) 
    {
        std::cerr << "Erreur de lecture du header WAVE." << std::endl;
        fclose(file);
        return false;
    }

    std::cout << "Chunk ID(riff): " << header.wave.ckID << std::endl;
    std::cout << "Size: " << header.wave.cksize << std::endl;
    std::cout << "WAVE ID: " << header.wave.WAVEID << std::endl;

    // Lecture du Chunk Format (fmt)
    fread(&header.chunk, sizeof(ChunkFormat), 1, file);

    if (ferror(file))
    {
        std::cerr << "Erreur de lecture du chunk WAVE." << std::endl;
        fclose(file);
        return false;
    }

    std::cout << "\nChunk ID(fmt): " << header.chunk.ckID << std::endl;
    std::cout << "Size: " << header.chunk.cksize << std::endl;
    std::cout << "Format Tag: " << header.chunk.wFormatTag << std::endl;
    std::cout << "Channels: " << header.chunk.nChannels << std::endl;
    std::cout << "Sample Rate: " << header.chunk.SamplePerSec << " Hz" << std::endl;
    std::cout << "Avg Bytes Per Sec: " << header.chunk.nAvgBytesPerSec << std::endl;
    std::cout << "Block Align: " << header.chunk.nBlockAlign << std::endl;
    std::cout << "Bits Per Sample: " << header.chunk.wBitsPerSample << std::endl;

    // Non utilisable aujourdhui
    //std::cout << "Cb Size: " << header.chunk.cbSize << std::endl;
    //std::cout << "W Valid Bits Per Sample: " << header.chunk.wValidBitsPerSample << std::endl;
    //std::cout << "Dw Channel Mask: " << header.chunk.dwChannelMask << std::endl;
    //std::cout << "Sub Format: " << header.chunk.SubFormat << std::endl;

    fclose(file);  // Fermeture explicite du fichier
    return true;
}
