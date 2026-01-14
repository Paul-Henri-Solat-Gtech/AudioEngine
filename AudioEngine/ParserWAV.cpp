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
    std::cout << "Sample Rate: " << header.chunk.nSamplePerSec << " Hz" << std::endl;
    std::cout << "Avg Bytes Per Sec: " << header.chunk.nAvgBytesPerSec << std::endl;
    std::cout << "Block Align: " << header.chunk.nBlockAlign << std::endl;
    std::cout << "Bits Per Sample: " << header.chunk.wBitsPerSample << std::endl;

    if (std::strncmp(header.chunk.ckID, "fmt ", 4) != 0)
    {
        std::cerr << "Chunk fmt manquant ou invalide." << std::endl;
        fclose(file);
        return false;
    }

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

    std::cout << "\nCopy Succesfull !" << std::endl;

    return true;
}

bool ParserWAV::CreateFileWAV(std::string& fileName)
{
    //Fichier vide
    FILE* file = nullptr;
    fopen_s(&file, fileName.c_str(), "wb"); // wb = write byte
    if (!file) return false;

    //Header
    WaveHeader newWaveheader;

    //Wave
    memcpy(newWaveheader.wave.ckID, "RIFF", 4);
    newWaveheader.wave.cksize = 36 + 0; // header sans data
    memcpy(newWaveheader.wave.WAVEID, "WAVE", 4);
    //Chunk
    memcpy(newWaveheader.chunk.ckID, "fmt ", 4);
    newWaveheader.chunk.cksize = 16; // 16 = PCM standard
    newWaveheader.chunk.wFormatTag = 1; // PCM
    newWaveheader.chunk.nChannels = 1;
    newWaveheader.chunk.nSamplePerSec = 44100;
    newWaveheader.chunk.nAvgBytesPerSec = newWaveheader.chunk.nSamplePerSec * 2; // 44100 hz
    newWaveheader.chunk.nBlockAlign = 2;
    newWaveheader.chunk.wBitsPerSample = 16;
    
    //Data
    ChunkData newChunkData;
    memcpy(newChunkData.ckID, "data", 4);

    // Test Audio
    const float freq = 440.0f; // 440 hz -> tonalite du tel 
    const float nSamplesPerSec = 44100.0f; // 440 hz -> tonalite du tel 

    size_t nSamples = static_cast<size_t>(nSamplesPerSec * 26.0f); // 26 seconde
    newChunkData.sampledData.resize(nSamples * 2); // 16-bit PCM = 2 bytes par sample
    for (size_t i = 0; i < nSamples; ++i)
    {
        int16_t sample = static_cast<int16_t>(0.3f * 32767.0f * sinf(2.0f * 3.14159265f * freq * i / nSamplesPerSec));
        newChunkData.sampledData[i * 2 + 0] = static_cast<uint8_t>(sample & 0xFF);
        newChunkData.sampledData[i * 2 + 1] = static_cast<uint8_t>((sample >> 8) & 0xFF);
    }
    newChunkData.cksize = static_cast<uint32_t>(newChunkData.sampledData.size());
    newWaveheader.wave.cksize = 36 + newChunkData.cksize;

    //Write Header
    fwrite(&newWaveheader, sizeof(WaveHeader), 1, file);

    // Write Data Chunk
    fwrite(newChunkData.ckID, 4, 1, file);
    fwrite(&newChunkData.cksize, 4, 1, file);
    if (!newChunkData.sampledData.empty()) 
    {
        fwrite(newChunkData.sampledData.data(), 1, newChunkData.cksize, file);
    }


    fclose(file);
    std::cout << "\nCreation Succesfull !" << std::endl;
    return true;
}

bool ParserWAV::CreateFileWAV(std::string& fileName, float totalTime, int numberOfChannels = 1, int samplePerSec = 44100)
{
    //Fichier vide
    FILE* file = nullptr;
    fopen_s(&file, fileName.c_str(), "wb"); // wb = write byte
    if (!file) return false;

    //Header
    WaveHeader newWaveheader;

    //Wave
    memcpy(newWaveheader.wave.ckID, "RIFF", 4);
    newWaveheader.wave.cksize = 36 + 0; // header sans data
    memcpy(newWaveheader.wave.WAVEID, "WAVE", 4);

    //Chunk
    memcpy(newWaveheader.chunk.ckID, "fmt ", 4);
    newWaveheader.chunk.cksize = 16; // 16 = PCM standard
    newWaveheader.chunk.wFormatTag = 1; // PCM
    newWaveheader.chunk.nChannels = numberOfChannels;
    newWaveheader.chunk.nSamplePerSec = samplePerSec;
    newWaveheader.chunk.nAvgBytesPerSec = newWaveheader.chunk.nSamplePerSec * 2; // 44100 hz
    newWaveheader.chunk.nBlockAlign = 2;
    newWaveheader.chunk.wBitsPerSample = 16;

    //Data
    ChunkData newChunkData;
    memcpy(newChunkData.ckID, "data", 4);

    // Test Audio
    const float freq = 440.0f; // 440 hz -> tonalite du tel 
    const float nSamplesPerSec = 44100.0f; // 440 hz -> tonalite du tel 
    
    size_t nSamples = static_cast<size_t>(nSamplesPerSec * totalTime);
    newChunkData.sampledData.resize(nSamples * 2); // 16-bit PCM = 2 bytes par sample

    for (size_t i = 0; i < nSamples; ++i)
    {
        int16_t sample = static_cast<int16_t>(0.3f * 32767.0f * sinf(2.0f * 3.14159265f * freq * i / nSamplesPerSec));
        newChunkData.sampledData[i * 2 + 0] = static_cast<uint8_t>(sample & 0xFF);
        newChunkData.sampledData[i * 2 + 1] = static_cast<uint8_t>((sample >> 8) & 0xFF);
    }
    newChunkData.cksize = static_cast<uint32_t>(newChunkData.sampledData.size());
    newWaveheader.wave.cksize = 36 + newChunkData.cksize;

    //Write Header
    fwrite(&newWaveheader, sizeof(WaveHeader), 1, file);

    // Write Data Chunk
    fwrite(newChunkData.ckID, 1, 4, file);
    fwrite(&newChunkData.cksize, 4, 1, file);
    if (!newChunkData.sampledData.empty())
    {
        fwrite(newChunkData.sampledData.data(), 1, newChunkData.cksize, file);
    }

    fclose(file);
    std::cout << "\nCreation Succesfull !" << std::endl;
    return true;
}

void ParserWAV::LocateByteInWav(std::string& fileName, int frameNum)
{
    // Ouverture fichier WAV
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, fileName.c_str(), "rb");  // rb = read byte
    if (err != 0 || file == nullptr)
    {
        std::cerr << "Impossible d'ouvrir le fichier." << std::endl;
    }

    // Lecture du header
    fread(&header.wave, sizeof(WaveFormat), 1, file);

    if (ferror(file))
    {
        std::cerr << "Erreur de lecture du header WAVE." << std::endl;
        fclose(file);
    }
    if (std::strncmp(header.wave.ckID, "RIFF", 4) != 0 || std::strncmp(header.wave.WAVEID, "WAVE", 4) != 0)
    {
        std::cerr << "Fichier non valide : RIFF/WAVE manquant." << std::endl;
        fclose(file);
    }

    // Lecture du Chunk Format (fmt)
    fread(&header.chunk, sizeof(ChunkFormat), 1, file);

    if (ferror(file))
    {
        std::cerr << "Erreur de lecture du chunk WAVE." << std::endl;
        fclose(file);
    }

    //search = frameNum + data size
    fseek(file, frameNum, SEEK_SET);

    // allocate memory to contain the whole file:
    long size = ftell(file);
    char* bufferFile = (char*)malloc(sizeof(char) * size);

    fread(bufferFile, header.chunk.cksize, 1,file);

    fclose(file);
    free(bufferFile);

    std::cout << "\Search Succesfull !" << std::endl;
}