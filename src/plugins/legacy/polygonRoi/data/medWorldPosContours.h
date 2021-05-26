#pragma once

#include <QDataStream>
#include <QTextStream>
#include <QVector>
#include <QVector3D>


class  medWorldPosContours
{
public:
    medWorldPosContours();
    medWorldPosContours(qint32 id, qint32 ori, QVector<QVector3D> coordinates);
    medWorldPosContours(const medWorldPosContours& other);

    friend QDataStream& operator>>(QDataStream& in, medWorldPosContours& data);
    friend QDataStream& operator<<(QDataStream& out, const medWorldPosContours& data);
    friend QTextStream& operator<<(QTextStream& out, const medWorldPosContours& data);

    void setSlice(qint32 id);
    void setOrientation(qint32 ori);
    void setCoordinates(QVector<QVector3D> &coords);

    qint32 getSlice(){return sliceId;}
    qint32 getOrientation(){return orientation;}
    QVector<QVector3D> getNodes(){return nodes;}

protected:
    qint32 sliceId;
    qint32 orientation;
    QVector<QVector3D> nodes;
};


