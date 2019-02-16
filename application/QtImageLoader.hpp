#ifndef QTIMAGELOADER_HPP
#define QTIMAGELOADER_HPP

#include "loader/ImageLoader.hpp"
#include <QImage>

class QtImageLoader : public ImageLoader
{
protected:

    using ImageLoader::width_;
    using ImageLoader::height_;
    using ImageLoader::format_;

    QImage image_;


public:
    QtImageLoader();
    virtual ~QtImageLoader();

    virtual void loadImage(const std::string& path);

    virtual const unsigned char* getPixels()const;
};

#endif // QTIMAGELOADER_HPP
