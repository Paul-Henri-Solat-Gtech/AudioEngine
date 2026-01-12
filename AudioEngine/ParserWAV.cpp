#include "ParserWAV.h"

bool ParserWAV::OpenWav(std::string& wavFilePath)
{
    // Ouverture fichier WAV
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, wavFilePath.c_str(), "rb");  // rb = read byte
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

    if (std::strncmp(header.wave.ckID, "RIFF", 4) != 0 || std::strncmp(header.wave.WAVEID, "WAVE", 4) != 0)
    {
        std::cerr << "Fichier non valide : RIFF/WAVE manquant." << std::endl;
        fclose(file);
        return false;
    }

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

    if (std::strncmp(header.chunk.ckID, "fmt ", 4) != 0)
    {
        std::cerr << "Chunk fmt manquant ou invalide." << std::endl;
        fclose(file);
        return false;
    }

    // Non utilisable aujourdhui
    //std::cout << "Cb Size: " << header.chunk.cbSize << std::endl;
    //std::cout << "W Valid Bits Per Sample: " << header.chunk.wValidBitsPerSample << std::endl;
    //std::cout << "Dw Channel Mask: " << header.chunk.dwChannelMask << std::endl;
    //std::cout << "Sub Format: " << header.chunk.SubFormat << std::endl;

    // chunk data for copy
    while (true)
    {
        fread(&data.ckID, 4, 1, file);
        fread(&data.cksize, 4, 1, file);

        if (ferror(file))
        {
            std::cerr << "Erreur lecture chunk." << std::endl;
            fclose(file);
            return false;
        }

        if (std::strncmp(data.ckID, "data", 4) == 0)
        {
            break; // trouve
        }

        // Sauter chunk inconnu
        fseek(file, data.cksize, SEEK_CUR);
    }

    // Lecture des samples
    data.sampledData.resize(data.cksize);
    fread(data.sampledData.data(), 1, data.cksize, file);

    if (ferror(file))
    {
        std::cerr << "Erreur lecture data." << std::endl;
        fclose(file);
        return false;
    }

    fclose(file);  // Fermeture explicite du fichier
    return true;
}

bool ParserWAV::WriteCopy(std::string& outPath)
{
    FILE* file = nullptr;
    fopen_s(&file, outPath.c_str(), "wb"); // wb = write byte
    if (!file) return false;

    // RIFF
    fwrite(&header.wave, sizeof(WaveFormat), 1, file);

    // fmt
    fwrite(&header.chunk, sizeof(ChunkFormat), 1, file);

    // data
    fwrite("data", 1, 4, file);
    fwrite(&data.cksize, 4, 1, file);
    fwrite(data.sampledData.data(), 1, data.cksize, file);

    fclose(file);
    return true;
}
