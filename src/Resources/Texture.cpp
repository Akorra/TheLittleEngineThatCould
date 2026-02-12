#include "TLETC/Resources/Texture.h"

#include <stb_image.h>

namespace TLETC
{

bool Texture::LoadFromImage(const std::string& filepath)
{
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &ch, 0);

    if (!data) 
        return false;

    TextureFormat fmt;
    switch (ch) {
        case 1: fmt = TextureFormat::R; break;
        case 2: fmt = TextureFormat::RG; break;
        case 3: fmt = TextureFormat::RGB; break;
        case 4: fmt = TextureFormat::RGBA; break;
        default:
            stbi_image_free(data);
            return false;
    }

    filepath_ = filepath;
    width_ = w;
    height_ = h;
    format_ = fmt;
    pixelData_.assign(data, data + (w * h * ch));

    stbi_image_free(data);

    return true;
}

} // namespace TLETC

