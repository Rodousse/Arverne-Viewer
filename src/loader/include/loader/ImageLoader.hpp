#pragma once

#include <iostream>
#include "data/2D/Image.hpp"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif


namespace
{

constexpr int deduceSTBIType(const data::ImageFormat& format)
{
    using data::ImageFormat;

    switch(format)
    {
        case ImageFormat::LUM8:
        case ImageFormat::LUM16:
        case ImageFormat::LUM32:
            return STBI_grey;

        case ImageFormat::LUMA8:
        case ImageFormat::LUMA16:
        case ImageFormat::LUMA32:
            return STBI_grey_alpha;

        case ImageFormat::RGB8:
        case ImageFormat::RGB16:
        case ImageFormat::RGB32:
            return STBI_rgb;

        case ImageFormat::RGBA8:
        case ImageFormat::RGBA16:
        case ImageFormat::RGBA32:
            return STBI_rgb_alpha;
    }

}


template <typename PixelType>
data::Image<PixelType> loadImage(const std::string& path, const data::ImageFormat& format);

template <>
data::Image<float> loadImage<float>(const std::string& path, const data::ImageFormat& format)
{
    int width;
    int height;
    int channels;
    float* data = stbi_loadf(path.data(), &width, &height, &channels, deduceSTBIType(format));

    data::Image<float> image(data, width, height, format);
    return std::move(image);

}

template <>
data::Image<unsigned char> loadImage<unsigned char>(const std::string& path,
        const data::ImageFormat& format)
{
    int width;
    int height;
    int channels;
    unsigned char* data = stbi_load(path.data(), &width, &height, &channels, deduceSTBIType(format));

    data::Image<unsigned char> image(data, width, height, format);
    return std::move(image);
}


template <>
data::Image<unsigned short> loadImage<unsigned short>(const std::string& path,
        const data::ImageFormat& format)
{
    int width;
    int height;
    int channels;
    unsigned short* data = stbi_load_16(path.data(), &width, &height, &channels,
                                        deduceSTBIType(format));

    data::Image<unsigned short> image(data, width, height, format);
    return std::move(image);
}



template <data::ImageFormat pixelFormat>
struct PixelTypeTrait
{
    typedef void type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::LUM8>
{
    typedef unsigned char type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::LUMA8>
{
    typedef unsigned char type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::RGB8>
{
    typedef unsigned char type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::RGBA8>
{
    typedef unsigned char type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::LUM16>
{
    typedef unsigned short type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::LUMA16>
{
    typedef unsigned short type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::RGB16>
{
    typedef unsigned short type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::RGBA16>
{
    typedef unsigned short type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::LUM32>
{
    typedef float type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::LUMA32>
{
    typedef float type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::RGB32>
{
    typedef float type;
};

template <>
struct PixelTypeTrait<data::ImageFormat::RGBA32>
{
    typedef float type;
};


}


namespace loader
{

template <data::ImageFormat pixelFormat>
data::Image<typename ::PixelTypeTrait<pixelFormat>::type> loadImage(const std::string& path)
{
    return ::loadImage<typename ::PixelTypeTrait<pixelFormat>::type>(path, pixelFormat);
}

}






