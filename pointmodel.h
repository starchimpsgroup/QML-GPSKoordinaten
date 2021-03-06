#ifndef POINTMODEL_H
#define POINTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "point.h"

class PointModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PointModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Point &last() {return points.last();}

    Point &first() {return points.first();}

    void setFormat(Point::GPSFormat format) {
        this->format = format;
    }

signals:

public slots:
    void addPoint(const Point &point);

private:
    QList<Point> points;
    Point::GPSFormat format;
};

#endif // POINTMODEL_H
