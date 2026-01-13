#include <iostream>

#include "ParserWAV.h"

int main()
{
    ParserWAV newParserWav;

    std::string filePath = "file_example_WAV_10MG.wav";
    std::string filePathTest = "test.wav";
    std::string filePathNewCreation = "NewCreation.wav";

    newParserWav.OpenWav(filePath);
    newParserWav.WriteCopy(filePathTest);
    newParserWav.CreateFile(filePathNewCreation);

    std::cout << "Hello World!\n";
}