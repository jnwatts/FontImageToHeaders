#include <QCoreApplication>
#include <QStringList>
#include <string>
#include <iostream>

#include "font.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    QStringList args = a.arguments();
    if (args.size() < 2) {
        cerr << "Usage: " << args.at(0).toStdString() << " <font basename>" << endl;
        return 1;
    }
    
    QString font_name(args.at(1));
    QString font_image(font_name + ".png");
    QString font_description(font_name + ".xml");

    Font font(font_image, font_description);
    
    font.toSource(font_name);
    font.toHeader(font_name);
    
    return 0;    
}


