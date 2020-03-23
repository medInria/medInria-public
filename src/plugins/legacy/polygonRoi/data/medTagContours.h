#pragma once

#include <medWorldPosContours.h>

#include <QDataStream>
#include <QTextStream>



/**
 * @brief The ContourData class contains all the  labellised contour.
 */
class  medTagContours
{
public:
    medTagContours();
    medTagContours(qint32 lab, QVector<medWorldPosContours> contourVec);
    medTagContours(const medTagContours& other);
    virtual ~medTagContours();

    void setLabel(qint32 lab){label = lab;}
    void setContourNodes(QVector<medWorldPosContours> contourVec){contours = contourVec;}

    qint32 getLabel(){return label;}
    QVector<medWorldPosContours> getContourNodes(){return contours;}

    friend QDataStream& operator>>(QDataStream& in, medTagContours& data);
    friend QDataStream& operator<<(QDataStream& out, const medTagContours& data);
    friend QTextStream& operator<<(QTextStream& out, const medTagContours& data);
protected:
    QVector<medWorldPosContours> contours;
    qint32 label;
//    QColor color;
};

