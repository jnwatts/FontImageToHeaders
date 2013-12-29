#ifndef FONT_H
#define FONT_H

#include <QMap>
#include <QImage>
#include "char.h"

class Font
{
public:
    Font(QString image_path, QString xml_path);
    
    int size;
    QString family;
    int height;
    QString style;
    
    void toSource(QString c_name);
    void toHeader(QString c_name);
    void translateImage(QTextStream &ts, QImage &image);
    
private:
    char _minChar;
    char _maxChar;
    
    QImage _image;
    QMap<char, Char*> _chars;
};

#endif // FONT_H
