
#include <medDisplayPosContours.h>

medDisplayPosContours::medDisplayPosContours()
    :label(-1), nodes(QVector<QVector2D>())
{

}

medDisplayPosContours::medDisplayPosContours(qint32 label, QVector<QVector2D> coordinates)
    :label(label), nodes(coordinates)
{

}

medDisplayPosContours::medDisplayPosContours(const medDisplayPosContours &other)
{
    label = other.label;
    nodes = other.nodes;
}

medDisplayPosContours::~medDisplayPosContours()
{

}

void medDisplayPosContours::setNodes(QVector<QVector2D>& coords)
{
    nodes = coords;
}
