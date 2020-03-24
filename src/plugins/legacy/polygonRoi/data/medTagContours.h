#pragma once

#include <medWorldPosContours.h>

#include <QDataStream>
#include <QTextStream>



/**
 * @brief The medTagContours class contains all the  labeled contour.
 */
class  medTagContours
{
public:
    medTagContours();
    medTagContours(QString name, QVector<medWorldPosContours> contourVec);
    medTagContours(const medTagContours& other);

    void setLabelName(QString name){labelName = name;}
    void setContourNodes(QVector<medWorldPosContours> contourVec){contours = contourVec;}

    QString getLabelName(){return labelName;}
    QVector<medWorldPosContours> getContourNodes(){return contours;}

    friend QDataStream& operator>>(QDataStream& in, medTagContours& data);
    friend QDataStream& operator<<(QDataStream& out, const medTagContours& data);
    friend QTextStream& operator<<(QTextStream& out, const medTagContours& data);
protected:
    QVector<medWorldPosContours> contours;
    QString labelName;
};

