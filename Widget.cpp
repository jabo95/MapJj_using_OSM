//
// Created by oreste on 26/09/24.
//

#include "Widget.h"
#include "OSMWay.h"

Widget::Widget(const QString& fileName,QWidget *parent ) : QWidget(parent) {
    parseOSMFile(fileName);
    findBounds();  // Find min/max lat/lon for scaling
}

void Widget::paintEvent(QPaintEvent *event)  {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Apply the scaling and panning for zooming and dragging
    painter.translate(offset);
    painter.scale(scaleFactor, scaleFactor);

    // Draw nodes
    for (const OSMNode &node : nodes) {
        QPointF point = latLonToXY(node.lat, node.lon);
        //drawNode(painter, point, node);
    }

    // Draw ways
    for (const OSMWay &way : ways) {
        QVector<QPointF> wayPoints;

        // Convert node references to QPointF for drawing
        for (const QString &ref : way.nodeRefs) {
            const OSMNode *node = findNodeById(ref);  // Assume a function to find node by ID
            if (node) {
                QPointF point = latLonToXY(node->lat, node->lon);
                wayPoints.append(point);
            }
        }

        // Draw the way with the points
        drawWay(painter, wayPoints, way);
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
                    type = ""; // key + "=" + value;
                }
            }
            nodes.append(OSMNode(id, lat, lon, type));
        } else if (xml.isStartElement() && xml.name() == "way") {
            // Parse way attributes
            QString wayId = xml.attributes().value("id").toString();
            bool visible = xml.attributes().value("visible").toString() == "true";
            QString version = xml.attributes().value("version").toString();
            QString timestamp = xml.attributes().value("timestamp").toString();
            QString user = xml.attributes().value("user").toString();
            QString uid = xml.attributes().value("uid").toString();

            QVector<QString> nodeRefs;  // Store references to nodes
            QMap<QString, QString> tags; // Store tags

            // Parse inner elements (nd and tag)
            while (!(xml.isEndElement() && xml.name() == "way")) {
                xml.readNext();
                if (xml.isStartElement()) {
                    if (xml.name() == "nd") {
                        QString ref = xml.attributes().value("ref").toString();
                        nodeRefs.append(ref);  // Add node reference
                    } else if (xml.name() == "tag") {
                        QString key = xml.attributes().value("k").toString();
                        QString value = xml.attributes().value("v").toString();
                        tags.insert(key, value);  // Add key-value pair to tags
                    }
                }
            }

            // Now you can create your OSMWay object and store it
            OSMWay way(wayId, visible, version, timestamp, user, uid, nodeRefs, tags);
            //ways.append(way);  // Assuming you have a 'ways' vector
            ways.push_back(way);
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
    painter.drawEllipse(point, 5/scaleFactor, 5/scaleFactor);  // Draw the node as a small circle

    // Optionally display information like type (e.g., tram stop, etc.)
    if (!node.type.isEmpty()) {
        painter.setPen(Qt::blue);
        painter.drawText(point + QPointF(10, 10), node.type);
    }
}

void Widget::drawWay(QPainter &painter, const QVector<QPointF> &points, const OSMWay &way) {
    if (points.isEmpty()) {
        return; // No points to draw
    }

    // Check if the way is closed by comparing the first and last node reference IDs
    bool isClosed = way.nodeRefs.first() == way.nodeRefs.last();

    // Set the color and style based on the type of way
    QString wayType = way.tags.value("highway", way.tags.value("building", way.tags.value("waterway", "")));
    QColor color;

    // Define semi-transparent colors for different types
    if (wayType == "building") {
        color = QColor(150, 75, 0, 150); // Brown with transparency for buildings
    } else if (wayType == "river" || wayType == "waterway") {
        color = QColor(0, 150, 255, 120); // Blue with transparency for rivers/waterways
    } else if (wayType == "road" || way.tags.contains("highway")) {
        color = QColor(128, 128, 128, 100); // Gray with transparency for roads
    } else {
        color = QColor(0, 0, 0, 50); // Default transparent black for other types
    }

    painter.setPen(QPen(color, 2/scaleFactor));  // Set pen width and color

    // Draw the way as a polygon if it's closed, or a polyline if it's open
    if (isClosed) {
        // Closed polygon (e.g., buildings, closed areas)
        painter.setBrush(isClosed ? QBrush(color, Qt::SolidPattern) : Qt::NoBrush);  // Fill if closed, no brush if open
        QPolygonF polygon(points);
        painter.drawPolygon(polygon);
    } else {
        // Open polyline (e.g., roads, paths)
        for (int i = 0; i < points.size() - 1; ++i) {
            painter.drawLine(points[i], points[i + 1]);  // Draw line between consecutive points
        }
    }

    // Optionally, draw the type of the way near the first point
    if (!wayType.isEmpty()) {
        //painter.setPen(Qt::blue);
        //painter.drawText(points.first() + QPointF(10, 10), wayType);
    }
}

// Handle the wheel event for zooming
void Widget::wheelEvent(QWheelEvent* event)  {
    const double zoomInFactor = 1.1;
    const double zoomOutFactor = 1.0 / zoomInFactor;

    // Zoom in or out depending on the direction of the wheel event
    if (event->angleDelta().y() > 0) {
        scaleFactor *= zoomInFactor;  // Zoom in
    } else {
        scaleFactor *= zoomOutFactor; // Zoom out
    }

    // Clamp the scale factor to avoid excessive zoom in or out
    scaleFactor = std::max(0.2, std::min(scaleFactor, 5.0));  // Restrict between 0.2x and 5x zoom

    // Trigger a repaint of the widget
    update();
}

// Handle mouse press event to start dragging
void Widget::mousePressEvent(QMouseEvent* event)  {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos(); // Capture the mouse position when the button is pressed
    }
}

// Handle mouse move event to drag the map
void Widget::mouseMoveEvent(QMouseEvent* event)  {
    if (event->buttons() & Qt::LeftButton) {
        // Calculate the mouse movement
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos(); // Update the last mouse position

        // Adjust the offset by the delta, scaled by the current zoom level
        offset += delta;

        // Trigger a repaint
        update();
    }
}

// Handle mouse release event to stop dragging
void Widget::mouseReleaseEvent(QMouseEvent* event)  {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = QPoint(); // Reset the mouse position
    }
}


const OSMNode* Widget::findNodeById(const QString &id) const {
    for (const OSMNode &node : nodes) {
        if (node.id == id) {
            return &node;
        }
    }
    return nullptr;  // Return null if the node with the ID is not found
}