#include "imagereader.h"
#include <iostream>

ImageReader::ImageReader(const QString &file) {
    QImage myImage;
    if (!myImage.load(file)) {
        std::cout<<"Failed to load in image"<<std::endl;
    }

    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    m_width = myImage.width();
    m_height = myImage.height();
    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

    m_data = new RGBA[m_width*m_height];
    for (int i = 0; i < arr.size() / 4.f; i++){
        m_data[i] = RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]};
    }
}
