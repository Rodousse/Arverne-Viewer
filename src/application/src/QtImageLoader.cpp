#include "application/QtImageLoader.h"

QtImageLoader::QtImageLoader():
    ImageLoader ()
{

}


void QtImageLoader::loadImage(const std::string& path)
{
    QImage::Format qtFormat;
    image_.load(QString::fromStdString(path));

    if(image_.isNull()){
        PLOGE << ("failed to load the image") << '\n';
    }
    switch(format_)
    {
    case R:
        qtFormat = QImage::Format_Grayscale8;
        break;
    case RGB:
        qtFormat = QImage::Format_RGB888;
        break;
    case RGBA:
        qtFormat = QImage::Format_RGBA8888_Premultiplied;
        break;
    }
    image_ = image_.convertToFormat(qtFormat);
}

const unsigned char* QtImageLoader::getPixels()const
{
    return image_.bits();
}


QtImageLoader::~QtImageLoader()
{

}
