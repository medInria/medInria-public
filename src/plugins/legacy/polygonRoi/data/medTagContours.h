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
    medTagContours(QString &name, QVector<medWorldPosContours> &contourVec);
    medTagContours(QString &name, QString &scoreName, bool targetState, QVector<medWorldPosContours> &contourVec);
    medTagContours(const medTagContours& other);

    void setLabelName(QString &name){ labelName = name;}
    void setScore(QString &score){ labelScore = score;}
    void setTarget(bool state){target = state;}
    void setSpecialityIndex(qint32 index){specialityIndex = index;}
    void setContourNodes(QVector<medWorldPosContours> &contourVec){ contours = contourVec;}

    QString & getLabelName(){return labelName;}
    QString & getScore(){return labelScore;}
    bool isTarget() const{return target;}
    int getSpecialityIndex() const{return specialityIndex;}
    QVector<medWorldPosContours> & getContourNodes(){return contours;}

    friend QDataStream& operator>>(QDataStream& in, medTagContours& data);
    friend QDataStream& operator<<(QDataStream& out, const medTagContours& data);
    friend QTextStream& operator<<(QTextStream& out, const medTagContours& data);
protected:
    QVector<medWorldPosContours> contours;
    QString labelName;
    QString labelScore;
    bool target;
    qint32 specialityIndex;
};

