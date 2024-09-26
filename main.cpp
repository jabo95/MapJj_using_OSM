#include <QApplication>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include<QWidget>
#include "Widget.h"








int main(int argc, char *argv[]) {
    QApplication app(argc, argv);


    QString fileName = "map.osm";  // Update this path to your OSM file
    Widget widget(fileName);
    //parseOSMFile(fileName);
    widget.show();
    return app.exec();
}
