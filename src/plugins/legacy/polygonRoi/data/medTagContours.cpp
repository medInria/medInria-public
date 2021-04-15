#include <medTagContours.h>
#include <QVector>

medTagContours::medTagContours()
    : labelName(QString()), labelScore(QString()), target(false), specialityIndex(0)
{

}

medTagContours::medTagContours(QString &name, QVector<medWorldPosContours> &contourVec)
    :contours(contourVec), labelName(name), labelScore(QString()), target(false), specialityIndex(0)
{

}

medTagContours::medTagContours(QString &name, QString &scoreName, bool targetState, QVector<medWorldPosContours> &contourVec)
    : contours(contourVec), labelName(name), labelScore(scoreName), target(targetState), specialityIndex(0)
{

}

medTagContours::medTagContours(const medTagContours &other)
{
    specialityIndex = other.specialityIndex;
    labelName = other.labelName;
    labelScore = other.labelScore;
    target = other.target;
    contours = other.contours;
}

QTextStream &operator<<(QTextStream &out, const medTagContours &data)
{
    for (const medWorldPosContours& nodes : data.contours)
    {
        out << nodes<<"\n";
    }
    return out;
}

QDataStream &operator>>(QDataStream &in, medTagContours &data)
{
    return in >> data.specialityIndex >> data.labelName >> data.labelScore >> data.target >> data.contours;
}

QDataStream &operator<<(QDataStream &out, const medTagContours &data)
{
    return out << data.specialityIndex << data.labelName << data.labelScore << data.target << data.contours;
}

