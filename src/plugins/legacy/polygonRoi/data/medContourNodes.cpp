
#include <medContourNodes.h>

medContourNodes::medContourNodes()
{

}

medContourNodes::medContourNodes(qint32 id, qint32 ori, QVector<QVector3D> coordinates)
    :sliceId(id), orientation(ori), nodes(coordinates)
{

}

medContourNodes::medContourNodes(const medContourNodes &other)
{
    nodes = other.nodes;
    sliceId = other.sliceId;
    orientation = other.orientation;
}

medContourNodes::~medContourNodes()
{

}

void medContourNodes::setSlice(qint32 id)
{
    sliceId = id;
}

void medContourNodes::setOrientation(qint32 ori)
{
    orientation = ori;
}

void medContourNodes::setCoordinates(QVector<QVector3D>& coords)
{
    nodes = coords;
}

QDataStream& operator>>(QDataStream &in, medContourNodes &data)
{
    return in >> data.sliceId >> data.orientation >> data.nodes;
}

QDataStream &operator<<(QDataStream &out, const medContourNodes &data)
{
    return out << data.sliceId << data.orientation << data.nodes;
}

QTextStream &operator<<(QTextStream &out, const medContourNodes &data)
{
    out << "sliceId\n" <<data.sliceId<< "\norientation\n"<<data.orientation<<"\nnodes\n";
    for (QVector3D vec : data.nodes)
        out<<vec.x()<<" "<<vec.y()<<"\n";
    return out;
}
