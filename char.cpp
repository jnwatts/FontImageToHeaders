#include <QStringList>
#include <QChar>
#include "char.h"

Char::Char(QDomElement c)
{
    this->width = c.attribute("width").toInt();
    QStringList list = c.attribute("offset").split(' ');
    this->offset[0] = list.at(0).toInt();
    this->offset[1] = list.at(1).toInt();
    list = c.attribute("rect").split(' ');
    this->rect[0] = list.at(0).toInt();    
    this->rect[1] = list.at(1).toInt();    
    this->rect[2] = list.at(2).toInt();    
    this->rect[3] = list.at(3).toInt();    
    this->code = c.attribute("code").at(0).toAscii();
    
    //    qDebug("Char: '%c' at %d,%d", this->code, this->rect[0], this->rect[1]);
}

QTextStream &operator<<(QTextStream &os, const Char& c)
{
    os << "{";
    os << c.width << ", ";
    os << "{" << c.offset[0] << ", " << c.offset[1] << "}, ";
    os << "{" << c.rect[0] << ", " << c.rect[1] << ", " << c.rect[2] << ", " << c.rect[3] << "}";
    os << "}";
    return os;
}


Char Char::empty()
{
    return Char();
}

Char::Char()
{
    width = 0;
    offset[0] = 0;
    offset[1] = 0;
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0;
    rect[3] = 0;
    code = 0;
}
