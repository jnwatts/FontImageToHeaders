#ifndef CHAR_H
#define CHAR_H

#include <QtXml/QDomElement>
#include <QTextStream>

class Char
{
public:
    Char(QDomElement c);
    bool operator< (const Char& other) const { return code < other.code; }
    
    static Char empty();
    
    int width;
    int offset[2];
    int rect[4];
    char code;
    
private:
    Char();
};
QTextStream& operator<< (QTextStream& os, const Char& c);

#endif // CHAR_H
