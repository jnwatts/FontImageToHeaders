#include <stdint.h>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QImage>
#include <QImageReader>
#include "font.h"

Font::Font(QString image_path, QString xml_path)
{
    QFile f(xml_path);
    QString errorStr("");
    int errorLine(0);
    int errorColumn(0);
    QDomDocument doc;
    QDomElement root;
    
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qCritical("ERROR: Failed to open config file \"%s\": %s",
                  xml_path.toUtf8().constData(),
                  f.errorString().toUtf8().constData()
                  );
        return;
    }
    
    if (!doc.setContent(&f, false, &errorStr, &errorLine, &errorColumn)) {
        qCritical("ERROR: Failed to parse config file \"%s\" at line %d, column %d:\n%s",
                  xml_path.toUtf8().constData(),
                  errorLine,
                  errorColumn,
                  errorStr.toUtf8().constData()
                  );
        return;
    }
    
    root = doc.documentElement();
    if (root.tagName() != "Font") {
        qCritical("ERROR: Unexpected root element \"%s\" at line %d, column %d",
                  root.tagName().toUtf8().constData(),
                  root.lineNumber(),
                  root.columnNumber());
        return;
    }
    
    this->size = root.attribute("size").toInt();
    this->family = root.attribute("family");
    this->height = root.attribute("height").toInt();
    this->style = root.attribute("style");
    
//    qDebug("Font: %d, %s, %d, %s", this->size, this->family.toUtf8().constData(), this->height, this->style.toUtf8().constData());
    
    _minChar = 127;
    _maxChar = 0;
    QDomElement childElement = root.firstChildElement();
    while (!childElement.isNull()) {
        QString tagName = childElement.tagName();
        if (tagName == "Char") {
            Char *c = new Char(childElement);
            this->_chars[c->code] = c;
            if (c->code > _maxChar)
                _maxChar = c->code;
            if (c->code < _minChar)
                _minChar = c->code;
        }
        childElement = childElement.nextSiblingElement();
    }
    
    QImageReader image_reader(image_path);
    this->_image = image_reader.read();
}

void Font::toSource(QString c_name)
{
    QFile f(c_name + ".c");
    f.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream ts(&f);
    
    ts << "#include \"" << c_name << ".h\"" << endl;
    ts << endl;
    ts << "static CharInfo _" << c_name << "_char_info[] = {" << endl;
    for (char c = _minChar; c < _maxChar; ++c) {
        ts << "\t";
        Char *C = 0;
        if (_chars.contains(c)) {
            QMap<char, Char*>::Iterator iter = _chars.find(c);
            C = iter.value();
            ts << *C;
        } else {
            ts << Char::empty();
        }
        ts << ",";
        if (C) {
            ts << " // '" << C->code << "'" << endl;
        } else {
            ts << " // NULL" << endl;
        }
    }
    ts << "};" << endl;
    ts << endl;
    ts << "static uint8_t _" << c_name << "_char_data[] = {" << endl;
    translateImage(ts, _image);
    ts.setIntegerBase(10);
    ts << "};" << endl;
    ts << endl;
    ts << "FontInfo font_" << c_name << " = {" << endl;
    ts << "\t'" << _minChar << "'," << endl;
    ts << "\t'" << _maxChar << "'," << endl;
    ts << "\t_" << c_name << "_char_info," << endl;
    ts << "\t_" << c_name << "_char_data," << endl;
    ts << "\t" << _image.width() << "," << endl;
    ts << "\t" << size << "," << endl;
    ts << "\t" << height << endl;
    ts << "};" << endl;
    ts.flush();
    f.close();
}

void Font::toHeader(QString c_name)
{
    QFile f(c_name + ".h");
    f.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream ts(&f);
    QString defblock = "__" + c_name.toUpper() + "_H";

    ts << "#ifndef " << defblock << endl;
    ts << "#define " << defblock << endl;
    ts << endl;
    ts << "#include <stdint.h>" << endl;
    ts << "#include \"font_info.h\"" << endl;
    ts << "#ifdef __cplusplus" << endl;
    ts << "extern \"C\" {" << endl;
    ts << "#endif" << endl;
    ts << endl;
    ts << "/*" << endl;
    ts << " * Family: " << this->family << endl;
    ts << " * Size: " << this->size << endl;
    ts << " * Height: " << this->height << endl;
    ts << " * Style: " << this->style << endl;
    ts << " * Supported characters: ";
    for (char c = _minChar; c < _maxChar; ++c) {
        if (_chars.contains(c)) {
            QMap<char, Char*>::Iterator iter = _chars.find(c);
            Char *c = iter.value();
            ts << c->code;
        }
    }
    ts << endl;
    ts << " */" << endl;
    ts << endl;
    ts << "extern FontInfo font_" << c_name << ";" << endl;
    ts << endl;
    ts << "#ifdef __cplusplus" << endl;
    ts << "} // extern \"C\"" << endl;
    ts << "#endif" << endl;
    ts << "#endif // " << defblock << endl;    
    
    ts.flush();
    f.close();
}

void Font::translateImage(QTextStream &ts, QImage &image)
{
    int page = 0;
    uint8_t line[image.width()];
    for (int x = 0; x < image.width(); ++x) {
        line[x] = 0;
    }
    for (int y = 0; y < image.height(); ++y) {
        int shift = (y % 8);
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixel(x, y) == qRgb(255,255,255)) {
                line[page * image.width() + x] |= (1 << shift);
            }
        }
        if (shift == 7 || (y + 1) == image.height()) {
            ts << "\t";
            for (int x = 0; x < image.width(); ++x) {
                ts << QString("0x%1").arg(line[x], 2, 16, QChar('0')) << ",";
                line[x] = 0;
            }
            ts << endl;
        }
    }
}
