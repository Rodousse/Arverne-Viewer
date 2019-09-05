#pragma once

#include "../../../src/2D/stbimplementation.h"
#include <memory>

namespace data
{


enum class ImageFormat
{
    //Luminance
    LUM8,
    LUM16,
    LUM32,// Float Luminance
    // 2 Components per pixel
    LUMA8,
    LUMA16,
    LUMA32,// Float RG
    // 3 Components per pixel
    RGB8,
    RGB16,
    RGB32,// Float RGB
    //4 Components per pixel
    RGBA8,
    RGBA16,
    RGBA32,// Float RGBA

    UNDEFINED = -1
};

template <typename PixelType>
class Image
{
    PixelType* data_;
    int width_;
    int height_;
    ImageFormat format_;

public:
    Image(PixelType* data, const int& width, const int& height, const ImageFormat& format):
        data_(data),
        width_(width),
        height_(height),
        format_(format)
    {
    }

    template<typename otherPixelType>
    Image(const Image<otherPixelType>& other) = delete;

    Image(Image&& other)
    {
        data_ = other.data_;
        width_ = other.width();
        height_ = other.height();
        format_ = other.format();
        other.data_ = nullptr;
    }

    ~Image()
    {
        if(data_)
        {
            stbi_image_free(data_);
        }
    }

    const PixelType* data() const
    {
        return data_;
    }

    const ImageFormat& format() const
    {
        return format_;
    }

    const int& height() const
    {
        return height_;
    }

    const int& width() const
    {
        return width_;
    }

};


inline bool is16Bits(const ImageFormat& format)
{
    if(format == data::ImageFormat::LUM16
       || format == data::ImageFormat::LUMA16
       || format == data::ImageFormat::RGB16
       || format == data::ImageFormat::RGB16
       || format == data::ImageFormat::RGBA16)
        return true;

    return false;
}

inline bool is8Bits(const data::ImageFormat& format)
{
    if(format == data::ImageFormat::LUM8
       || format == data::ImageFormat::LUMA8
       || format == data::ImageFormat::RGB8
       || format == data::ImageFormat::RGB8
       || format == data::ImageFormat::RGBA8)
        return true;

    return false;
}

inline bool is32Bits(const data::ImageFormat& format)
{
    if(format == data::ImageFormat::LUM32
       || format == data::ImageFormat::LUMA32
       || format == data::ImageFormat::RGB32
       || format == data::ImageFormat::RGB32
       || format == data::ImageFormat::RGBA32)
        return true;

    return false;
}


}
