//
// Created by oreste on 26/09/24.
//

#include "Widget.h"
Widget::Widget(const QString& fileName,QWidget *parent ) : QWidget(parent) {
    parseOSMFile(fileName);
    findBounds();  // Find min/max lat/lon for scaling
}

void Widget::paintEvent(QPaintEvent *event)  {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const OSMNode &node : nodes) {
        QPointF point = latLonToXY(node.lat, node.lon);
        drawNode(painter, point, node);
    }
}


void Widget::parseOSMFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << fileName;
        return;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == "node") {
            QString id = xml.attributes().value("id").toString();
            double lat = xml.attributes().value("lat").toDouble();
            double lon = xml.attributes().value("lon").toDouble();
            QString type;

            // Check for tags
            while (!(xml.isEndElement() && xml.name() == "node")) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == "tag") {
                    QString key = xml.attributes().value("k").toString();
                    QString value = xml.attributes().value("v").toString();
                    // You can store the tags in a more sophisticated way
                    type = "";//key + "=" + value;
                }
            }
            nodes.append(OSMNode(id, lat, lon, type));
        }
    }

    if (xml.hasError()) {
        qDebug() << "XML Parsing Error:" << xml.errorString();
    }

    file.close();
}

void Widget::findBounds() {
    if (nodes.isEmpty()) return;

    minLat = maxLat = nodes.first().lat;
    minLon = maxLon = nodes.first().lon;

    for (const OSMNode &node : nodes) {
        if (node.lat < minLat) minLat = node.lat;
        if (node.lat > maxLat) maxLat = node.lat;
        if (node.lon < minLon) minLon = node.lon;
        if (node.lon > maxLon) maxLon = node.lon;
    }
}

QPointF Widget::latLonToXY(double lat, double lon) {
    double width = this->width();
    double height = this->height();

    double x = (lon - minLon) / (maxLon - minLon) * width;
    double y = height - (lat - minLat) / (maxLat - minLat) * height;

    return QPointF(x, y);
}

void Widget::drawNode(QPainter &painter, const QPointF &point, const OSMNode &node) {
    painter.setPen(Qt::black);
    painter.setBrush(Qt::red);
    painter.drawEllipse(point, 5, 5);  // Draw the node as a small circle

    // Optionally display information like type (e.g., tram stop, etc.)
    if (!node.type.isEmpty()) {
        painter.setPen(Qt::blue);
        painter.drawText(point + QPointF(10, 10), node.type);
    }
}