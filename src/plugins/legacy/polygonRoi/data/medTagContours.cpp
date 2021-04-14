#include <medTagContours.h>
#include <QVector>

medTagContours::medTagContours()
    : labelName(QString()), labelScore(QString()), target(false)
{

}

medTagContours::medTagContours(QString &name, QVector<medWorldPosContours> &contourVec)
    : labelName(name), labelScore(QString()), target(false), contours(contourVec)
{

}

medTagContours::medTagContours(QString &name, QString &scoreName, bool targetState, QVector<medWorldPosContours> &contourVec)
    : labelName(name), labelScore(scoreName), target(targetState), contours(contourVec)
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
    out << "specialityIndex\n" << data.specialityIndex << "labelName\n" << data.labelName << "labelScore\n" << data.labelScore << "\ncontours\n";
    for (medWorldPosContours nodes : data.contours)
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

