#include "GpxHandler.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QUuid>

QList<Waypoint> GpxHandler::loadFromGpx(const QString &filePath, QString *errorMsg) {
    QList<Waypoint> waypoints;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = QString("Unable to open file: %1").arg(file.errorString());
        return waypoints;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("wpt") || xml.name() == QLatin1String("rtept") || xml.name() == QLatin1String("trkpt")) {
                QXmlStreamAttributes attrs = xml.attributes();
                double lat = attrs.value("lat").toDouble();
                double lon = attrs.value("lon").toDouble();
                double alt = 0.0;
                QString name;
                QString desc;

                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                        (xml.name() == QLatin1String("wpt") || xml.name() == QLatin1String("rtept") || xml.name() == QLatin1String("trkpt")))) {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == QLatin1String("name")) {
                            name = xml.readElementText();
                        } else if (xml.name() == QLatin1String("ele")) {
                            alt = xml.readElementText().toDouble();
                        } else if (xml.name() == QLatin1String("desc")) {
                            desc = xml.readElementText();
                        }
                    }
                }

                if (name.isEmpty()) {
                    name = QString("Waypoint %1").arg(waypoints.count() + 1);
                }

                QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
                waypoints.append(Waypoint(id, name, lat, lon, alt, desc));
            }
        }
    }

    if (xml.hasError()) {
        if (errorMsg) *errorMsg = QString("XML Parse Error: %1").arg(xml.errorString());
    }

    file.close();
    return waypoints;
}

bool GpxHandler::saveToGpx(const QString &filePath, const QList<Waypoint> &waypoints, const QString &routeName, QString *errorMsg) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = QString("Unable to open file for writing: %1").arg(file.errorString());
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement("gpx");
    xml.writeAttribute("version", "1.1");
    xml.writeAttribute("creator", "AppliGPS i.MX8 Navigation Engine");
    xml.writeAttribute("xmlns", "http://www.topografix.com/GPX/1/1");

    xml.writeStartElement("rte");
    xml.writeTextElement("name", routeName);

    for (const auto &wpt : waypoints) {
        xml.writeStartElement("rtept");
        xml.writeAttribute("lat", QString::number(wpt.latitude, 'f', 6));
        xml.writeAttribute("lon", QString::number(wpt.longitude, 'f', 6));

        xml.writeTextElement("name", wpt.name);
        if (wpt.altitude != 0.0) {
            xml.writeTextElement("ele", QString::number(wpt.altitude, 'f', 2));
        }
        if (!wpt.description.isEmpty()) {
            xml.writeTextElement("desc", wpt.description);
        }

        xml.writeEndElement(); // rtept
    }

    xml.writeEndElement(); // rte
    xml.writeEndElement(); // gpx
    xml.writeEndDocument();

    file.close();
    return true;
}
