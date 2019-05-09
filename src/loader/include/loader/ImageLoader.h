/*
*
*
*  Abstract class loading an image, depending on the technology the user of the renderer uses
*the class has to be reimplemented
*
*
*/

#pragma once

#include <iostream>

enum E_ImageFormat
{
    R,
    RGB,
    RGBA
};

class ImageLoader
{
protected:
    uint32_t width_;
    uint32_t height_;
    E_ImageFormat format_;

public:
    ImageLoader();
    virtual ~ImageLoader();

    void setFormat(E_ImageFormat format);
    void loadImage(const std::string& path, E_ImageFormat format);
    virtual void loadImage(const std::string& path) = 0;

    virtual const unsigned char* getPixels()const = 0;

    E_ImageFormat getFormat() const;
    uint32_t width()const;
    uint32_t height()const;

};
