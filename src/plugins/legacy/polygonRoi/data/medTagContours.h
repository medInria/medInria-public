#pragma once

#include "medContourNodes.h"

#include <QDataStream>
#include <QTextStream>



/**
 * @brief The ContourData class contains all the  labellised contour.
 */
class  medTagContours
{
public:
    medTagContours();
    medTagContours(qint32 lab, QVector<medContourNodes> contourVec);
    medTagContours(const medTagContours& other);
    virtual ~medTagContours();

    void setLabel(qint32 lab){label = lab;}
    void setContourNodes(QVector<medContourNodes> contourVec){contours = contourVec;}

    qint32 getLabel(){return label;}
    QVector<medContourNodes> getContourNodes(){return contours;}

    friend QDataStream& operator>>(QDataStream& in, medTagContours& data);
    friend QDataStream& operator<<(QDataStream& out, const medTagContours& data);
    friend QTextStream& operator<<(QTextStream& out, const medTagContours& data);
protected:
    QVector<medContourNodes> contours;
    qint32 label;
//    QColor color;
};

