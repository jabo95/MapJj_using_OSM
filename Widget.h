//
// Created by oreste on 26/09/24.
//

#ifndef USINGOSM_WIDGET_H
#define USINGOSM_WIDGET_H
#include<QWidget>
#include <QWidget>
#include <QPainter>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include<cmath>
#include <QMouseEvent>
#include <QWheelEvent>
class OSMNode {
public:
    QString id;
    double lat;
    double lon;
    QString type;  // To store the tag information like "tram_stop", "railway", etc.

    OSMNode(QString id, double lat, double lon, QString type = "")
            : id(id), lat(lat), lon(lon), type(type) {}
};



class Widget : public QWidget {
Q_OBJECT

private:
    QList<OSMNode> nodes;  // Store parsed nodes from OSM file
    std::unordered_map<QString, QPointF> nodeMap;  // Store all the nodes extracted from the OSM file
    std::vector<std::vector<QString>> ways;  // Store the paths/ways extracted from the OSM file
    double minLat, maxLat, minLon, maxLon;
    double scaleFactor =1.0 ;  // Scale factor for zooming
    QPoint lastMousePos;       // For tracking the last mouse position
    QPoint offset;             // Offset for panning
    //QPixmap backgroundImage;

public:
    Widget(const QString& fileName="map.osm",QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // Step 1: Parse OSM file and extract nodes
    void parseOSMFile(const QString &fileName);

    // Step 2: Find bounding box for scaling the map (min/max lat/lon)
    void findBounds();

    // Step 3: Convert latitude and longitude to widget coordinates (X, Y)
    QPointF latLonToXY(double lat, double lon);

    // Step 4: Draw node at specific coordinates
    void drawNode(QPainter &painter, const QPointF &point, const OSMNode &node);

    void wheelEvent(QWheelEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;
};



#endif //USINGOSM_WIDGET_H
