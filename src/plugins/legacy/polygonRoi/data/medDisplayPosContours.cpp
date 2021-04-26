
#include <medDisplayPosContours.h>

#include <utility>

medDisplayPosContours::medDisplayPosContours()
    :nodes(QVector<QVector2D>()), label(-1)
{

}

medDisplayPosContours::medDisplayPosContours(qint32 label, QVector<QVector2D> coordinates)
    :nodes(std::move(coordinates)), label(label)
{

}

medDisplayPosContours::medDisplayPosContours(const medDisplayPosContours &other)
{
    label = other.label;
    nodes = other.nodes;
}

void medDisplayPosContours::setNodes(QVector<QVector2D>& coords)
{
    nodes = coords;
}

void medDisplayPosContours::setLabel(qint32 iLabel)
{
    label = iLabel;
}
