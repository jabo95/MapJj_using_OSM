//
// Created by oreste on 03/10/24.
//

#ifndef USINGOSM_OSMWAY_H
#define USINGOSM_OSMWAY_H
#include<QString>
#include<QMap>
#include <QVector>

class OSMWay {
public:
    OSMWay(const QString& id, bool visible, const QString& version, const QString& timestamp,
           const QString& user, const QString& uid, const QVector<QString>& nodeRefs,
           const QMap<QString, QString>& tags)
            : id(id), visible(visible), version(version), timestamp(timestamp),
              user(user), uid(uid), nodeRefs(nodeRefs), tags(tags) {}

    QString id;
    bool visible;
    QString version;
    QString timestamp;
    QString user;
    QString uid;
    QVector<QString> nodeRefs;  // References to nodes
    QMap<QString, QString> tags; // Tags (key-value pairs)
};


#endif //USINGOSM_OSMWAY_H
