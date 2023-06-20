#include <iostream>
#include <fstream>
#include <vector>

typedef struct GifHeader
{
    uint8_t signature[3];
    uint8_t version[3];
    uint16_t screenWidth;
    uint16_t screenHeight;
    uint8_t packedFields;
    uint8_t backgroundColorIndex;
    uint8_t pixelAspectRatio;
};

typedef struct GlobalColorTable
{

};

void parse_header(std::ifstream *ifs, GifHeader* header)
{
    printf("%s", "HEADER\n");
    int len = 3;
    ifs->read(reinterpret_cast<char*>(&header->signature), 3);
    ifs->read(reinterpret_cast<char*>(&header->version), 3);
    ifs->read(reinterpret_cast<char*>(&header->screenWidth), 2);
    ifs->read(reinterpret_cast<char*>(&header->screenHeight), 2);
    ifs->read(reinterpret_cast<char*>(&header->packedFields), 1);
    ifs->read(reinterpret_cast<char*>(&header->backgroundColorIndex), 1);
    ifs->read(reinterpret_cast<char*>(&header->pixelAspectRatio), 1);

}

void parse_colortable(std::ifstream *ifs, GlobalColorTable *gct)
{

}

void file_handle(std::string fileName)
{

    std::ifstream ifs;
    GifHeader header;
    GlobalColorTable gct;
    ifs.open(fileName, std::ios::binary);

    ifs.seekg(0, std::ios_base::end);
    std::size_t size = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);
    parse_header(&ifs, &header);
    bool check = header.packedFields & (1 << 0);
    if(check)
        parse_colortable(&ifs, &gct);
    ifs.close();
}