#pragma once

#include <QVector>
#include <QVector2D>

class  medDisplayPosContours
{
public:
    medDisplayPosContours();
    medDisplayPosContours(qint32 label, QVector<QVector2D> coordinates);
    medDisplayPosContours(const medDisplayPosContours& other);
    virtual ~medDisplayPosContours();

    void setNodes(QVector<QVector2D> &coords);
    void setLabel(qint32 label);

    QVector<QVector2D> getNodes(){return nodes;}
    qint32 getLabel(){return label;}

protected:
    QVector<QVector2D> nodes;
    qint32 label;
};


