#include "gpstracker.h"

// Konstruktor mit Parameter

GPSTracker::GPSTracker(QObject *parent) :
    QObject(parent) {
    geoPositionInfoSource = QGeoPositionInfoSource::createDefaultSource(this);
    // emit funktioniert erst nach dem Konstruktor
    if(geoPositionInfoSource) {
        connect(geoPositionInfoSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
        connect(geoPositionInfoSource, SIGNAL(updateTimeout()),
                this, SLOT(updateTimeout()));
        changeGPSStatus(INACTIVE);
    } else {
        changeGPSStatus(ERROR);
    }

    trackingInterval = 500;
    tracking = false;
}

GPSTracker::~GPSTracker() {
    delete geoPositionInfoSource;
}

void GPSTracker::startTracking() {
    startGPS();
    if(isGPSActiv()) {
        tracking = true;
    }
}

void GPSTracker::stopTracking() {
    tracking = false;
}

void GPSTracker::setTrackingInterval(int msec) {
    if(geoPositionInfoSource) {
        geoPositionInfoSource->setUpdateInterval(msec);
        trackingInterval = geoPositionInfoSource->updateInterval();
    } else {
        trackingInterval = msec;
    }
}

void GPSTracker::recordActualPosition() {
    if(isGPSActiv() && geoPositionInfoSource) {
        geoPositionInfoSource->requestUpdate(); // richtig?
        addPosition(geoPositionInfoSource->lastKnownPosition());
    }
}

void GPSTracker::startGPS() {
    if(geoPositionInfoSource) {
        if(!isGPSActiv()){
            setTrackingInterval(trackingInterval);
            geoPositionInfoSource->startUpdates();
            changeGPSStatus(ACTIVE);
        }
    } else {
        changeGPSStatus(ERROR);
    }
}

void GPSTracker::stopGPS() {
    if(isGPSActiv()) {
        geoPositionInfoSource->stopUpdates();
        changeGPSStatus(INACTIVE);
    }
}

bool GPSTracker::isGPSActiv() {
    return gpsStatus == ACTIVE || gpsStatus == RECEIVE || gpsStatus == TIMEOUT;
}

void GPSTracker::changeGPSStatus(GPSStatus status) {
    gpsStatus = status;
    emit gpsStatusChanged(status);
}

void GPSTracker::positionUpdated(QGeoPositionInfo position) {
    if(tracking){
        addPosition(position);
    }
    emit positionUpdated(Point(position));
    horizontalAccuracy = position.attribute(QGeoPositionInfo::HorizontalAccuracy);
    verticalAccuracy   = position.attribute(QGeoPositionInfo::VerticalAccuracy);
    changeGPSStatus(RECEIVE);
}

void GPSTracker::addPosition(QGeoPositionInfo position) {
    Point point(position);
    if(points.rowCount() > 0){
        if(!points.last().equals(point)) {
            points.addPoint(point);
            qDebug("Altitude");
            qDebug(qPrintable(QString::number(point.getAltitude())));
            qDebug("Latitude");
            qDebug(qPrintable(QString::number(point.getLatitude())));
            qDebug("Longitude");
            qDebug(qPrintable(QString::number(point.getLongitude())));

            Point point = points.last();
            // WGS84:
            qDebug("WGS84");
            qDebug(qPrintable(point.text()));
            // ECEF:
            qDebug("ECEF");
            qDebug(qPrintable(Point::WGS2ECEF(point).text()));
            // ENU:
            qDebug("ENU");
            qDebug(qPrintable(Point::WGS2ENU(points.first(), point).text()));
        }
    } else {
        points.addPoint(point);
    }
}

void GPSTracker::updateTimeout() {
    changeGPSStatus(TIMEOUT);
}
