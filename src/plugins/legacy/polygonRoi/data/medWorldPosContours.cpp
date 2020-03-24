
#include <medWorldPosContours.h>

medWorldPosContours::medWorldPosContours()
    :sliceId(-1), orientation(-1),nodes(QVector<QVector3D>())
{

}

medWorldPosContours::medWorldPosContours(qint32 id, qint32 ori, QVector<QVector3D> coordinates)
    :sliceId(id), orientation(ori), nodes(coordinates)
{

}

medWorldPosContours::medWorldPosContours(const medWorldPosContours &other)
{
    nodes = other.nodes;
    sliceId = other.sliceId;
    orientation = other.orientation;
}

void medWorldPosContours::setSlice(qint32 id)
{
    sliceId = id;
}

void medWorldPosContours::setOrientation(qint32 ori)
{
    orientation = ori;
}

void medWorldPosContours::setCoordinates(QVector<QVector3D>& coords)
{
    nodes = coords;
}

QDataStream& operator>>(QDataStream &in, medWorldPosContours &data)
{
    return in >> data.sliceId >> data.orientation >> data.nodes;
}

QDataStream &operator<<(QDataStream &out, const medWorldPosContours &data)
{
    return out << data.sliceId << data.orientation << data.nodes;
}

QTextStream &operator<<(QTextStream &out, const medWorldPosContours &data)
{
    out << "sliceId\n" <<data.sliceId<< "\norientation\n"<<data.orientation<<"\nnodes\n";
    for (QVector3D vec : data.nodes)
    {
        out<<vec.x()<<" "<<vec.y()<<"\n";
    }
    return out;
}
