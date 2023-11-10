#include "utils/rgba.h"
#include <QPainter>

class ImageReader
{
public:
    ImageReader(const QString &file);
    int m_width = 0;
    int m_height = 0;
    RGBA* m_data;
};
