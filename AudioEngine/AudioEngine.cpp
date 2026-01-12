#include <iostream>

#include "ParserWAV.h"

int main()
{
    ParserWAV newParserWav;

    std::string filePath = "file_example_WAV_10MG.wav";

    newParserWav.OpenWav(filePath);

    std::cout << "Hello World!\n";
}