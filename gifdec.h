#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <bitset>
#include <iterator>

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

typedef struct ApplicationExtension
{
    uint8_t applicationIdentifier[8];
    uint8_t applicationAuthCode[3];
    std::vector<uint8_t> applicationData;
};

typedef struct CommentExtension
{

};

typedef struct GraphicControlExtension
{
    uint8_t packedFields;
    uint16_t delayTime;
    uint8_t transparentColorIndex;
};

struct GlobalColorTable
{
    uint8_t* red;
    uint8_t* green;
    uint8_t* blue;

    GlobalColorTable(int len)
    {
        red = new uint8_t[len];
        green = new uint8_t[len];
        blue = new uint8_t[len];
    }
    GlobalColorTable(){}
};

void parse_header(std::ifstream* ifs, GifHeader* header)
{
    ifs->read(reinterpret_cast<char*>(&header->signature), 3);
    ifs->read(reinterpret_cast<char*>(&header->version), 3);
    ifs->read(reinterpret_cast<char*>(&header->screenWidth), 2);
    ifs->read(reinterpret_cast<char*>(&header->screenHeight), 2);
    ifs->read(reinterpret_cast<char*>(&header->packedFields), 1);
    ifs->read(reinterpret_cast<char*>(&header->backgroundColorIndex), 1);
    ifs->read(reinterpret_cast<char*>(&header->pixelAspectRatio), 1);
}

void get_colortable(std::ifstream* ifs, GlobalColorTable* gct, uint8_t gctSize)
{
    int len = pow(2,gctSize+1);
    *gct = GlobalColorTable(len);
    unsigned char* buffer;
    buffer = new unsigned char[3*len];

    ifs->read((char*)buffer, 3*len);
    std::cout << "buffer?:\n";
    for(int i = 0; i < 3*len; i++)
    {
        std::cout << std::hex << (int)buffer[i] << " ";
        if((i+1) % 3 == 0)
            std::cout << '\n';
    }
    for(int i = 0; i < len; i++)
    {
        gct->red[i] = buffer[(3*i)];
        gct->green[i] = buffer[(3*i)+1];
        gct->blue[i] = buffer[(3*i)+2];
    }
    /*
    for(int i = 0; i < len; i++)
    {
        std::cout << "r:" << (int)gct->red[i] << " g:" << (int)gct->green[i]
        << " b:" << (int)gct->blue[i] << '\n';
    }
    */
    delete[] buffer;
}

ApplicationExtension get_appextension(std::ifstream* ifs)
{
    unsigned char c[1];
    ApplicationExtension appExtension;
    ifs->read((char*)c, 1);
    ifs->read(reinterpret_cast<char*>(&appExtension.applicationIdentifier), 8);
    ifs->read(reinterpret_cast<char*>(&appExtension.applicationAuthCode), 3);
    ifs->read((char*)c, 1);
    while(c[0] != 0x0)
    {
        unsigned char buffer[c[0]];
        ifs->read((char *) buffer, c[0]);
        appExtension.applicationData.insert(appExtension.applicationData.end(), buffer, buffer + c[0]);
        ifs->read((char*)c, 1);
    }
    return appExtension;
}

void get_graphicextension(std::ifstream* ifs, GraphicControlExtension* gce)
{
    ifs->ignore();
    ifs->read(reinterpret_cast<char*>(&gce->packedFields), 1);
    ifs->read(reinterpret_cast<char*>(&gce->delayTime), 2);
    ifs->read(reinterpret_cast<char*>(&gce->transparentColorIndex), 1);
    ifs->ignore();
}

void file_handle(std::string fileName)
{

    std::ifstream ifs;
    GifHeader header;
    GlobalColorTable gct;
    GraphicControlExtension gce;
    std::vector<ApplicationExtension> appExtensions;
    ifs.open(fileName, std::ios::binary);

    ifs.seekg(0, std::ios_base::end);
    std::size_t size = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);
    parse_header(&ifs, &header);

    bool check = header.packedFields & (1 << 0);
    if(check)
    {
        std::cout << "yes" << '\n';
        get_colortable(&ifs, &gct, header.packedFields & 0x7);
    }
    while(ifs.tellg() != size)
    {
        uint8_t c;
        ifs.read((char*)&c, 1);
        std::cout << std::hex << (int)c << '\n';
        char debug;
        std::cin >> debug;
        if(c == 0x21)
        {
            ifs.read((char*)&c, 1);
            if(c == 0xff)
            {
                ApplicationExtension temp = get_appextension(&ifs);
                appExtensions.emplace_back(temp);
            }
            else if(c == 0xf9)
            {
                get_graphicextension(&ifs, &gce);
            }
        }
        else if(c == 0x2c)
        {

        }
    }
    ifs.close();
}