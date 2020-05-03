#include "Texture.h"

#include "RenderBackend.h"

using namespace BLA;

#define FOURCC_DXT1 0x31545844 // "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // "DXT5" in ASCII

Texture2D::Texture2D(blaString name, glm::uint8 dim, blaVector<glm::uint8> data, blaU32 width, blaU32 height) : Asset(name)
{
    this->m_data = data;
    this->m_width = width;
    this->m_height = height;
    this->m_nComponents = dim;

    this->m_dataSize = dim * width * height;
}

Texture2D::~Texture2D()
{}

//TODO: Support nComponent texture
Texture2D* TextureImport::LoadBMP(blaString resourceName, blaString filePath)
{
    printf("Reading image %s\n", filePath.data());

    // Data read from the header of the BMP file
    glm::uint8 header[54];
    blaU32 dataPos;
    blaU32 imageSize;
    blaU32 width, height;

    // Open the file
    FILE* file = nullptr;
	errno_t error = fopen_s(&file, filePath.data(), "rb");
    if (!file)
    {
        printf("%s could not be opened.\n", filePath.data());
        return nullptr;
    }

    // Read the header, i.e. the 54 first bytes

    // If less than 54 bytes are read, problem
    if (fread(header, 1, 54, file) != 54)
    {
        printf("Not a correct BMP file\n");
        return nullptr;
    }
    // A BMP files always begins with "BM"
    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Not a correct BMP file\n");
        return nullptr;
    }
    // Make sure this is a 24bpp file
    if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return nullptr; }
    if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return nullptr; }

    // Read the information about the image
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

    // Create a buffer Actual RGB data
    blaVector<glm::uint8> data = blaVector<glm::uint8>(imageSize);

    // Read the actual data from the file into the buffer
    fread(data.data(), 1, imageSize, file);

    // Everything is in memory now, the file wan be closed
    fclose(file);

    return new Texture2D(resourceName, 3, data, width, height);
}

Texture2D* TextureImport::LoadDDS(blaString name, blaString imagepath)
{
    std::cout << "UNEMPLEMENTED !!! \n";

    unsigned char header[124];

    FILE *fp = nullptr;

    /* try to open the file */
    fopen_s(&fp, imagepath.data(), "rb");
    if (!fp)
        return nullptr;

    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        return 0;
    }

    /* get the surface desc */
    fread(&header, 124, 1, fp);

    unsigned int height = *(unsigned int*)&(header[8]);
    unsigned int width = *(unsigned int*)&(header[12]);
    unsigned int linearSize = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC = *(unsigned int*)&(header[80]);


    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipblaMaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        free(buffer);
        return nullptr;
    }
    return nullptr;
}
