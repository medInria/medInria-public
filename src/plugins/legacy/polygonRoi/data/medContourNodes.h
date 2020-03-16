#pragma once

#include <QDataStream>
#include <QTextStream>
#include <QVector3D>

/**
 * @brief The ContourData class contains the nodes coordinates which defines a contour on a slice.
 */
class  medContourNodes
{
public:
    medContourNodes();
    medContourNodes(qint32 id, qint32 ori, QVector<QVector3D> coordinates);
    medContourNodes(const medContourNodes& other);
    virtual ~medContourNodes();

    friend QDataStream& operator>>(QDataStream& in, medContourNodes& data);
    friend QDataStream& operator<<(QDataStream& out, const medContourNodes& data);
    friend QTextStream& operator<<(QTextStream& out, const medContourNodes& data);

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


