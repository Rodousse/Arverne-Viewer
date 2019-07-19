#include "loader/ImageLoader.h"

ImageLoader::ImageLoader()
{

}

void ImageLoader::setFormat(E_ImageFormat format)
{
    format_ = format;
}

void ImageLoader::loadImage(const std::string& path, E_ImageFormat format)
{
    setFormat(format);
    loadImage(path);
}

E_ImageFormat ImageLoader::getFormat() const
{
    return format_;
}

uint32_t ImageLoader::width() const
{
    return width_;
}

uint32_t ImageLoader::height() const
{
    return height_;
}

ImageLoader::~ImageLoader()
{

}
