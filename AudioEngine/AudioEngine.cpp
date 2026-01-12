#include <iostream>

#include "ParserWAV.h"

int main()
{
    ParserWAV newParserWav;

    std::string filePath = "file_example_WAV_10MG.wav";
    std::string filePathNew = "test.wav";

    newParserWav.OpenWav(filePath);

    newParserWav.WriteCopy(filePathNew);

    std::cout << "Hello World!\n";
}