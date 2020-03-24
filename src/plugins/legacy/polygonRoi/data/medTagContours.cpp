#include <medTagContours.h>


medTagContours::medTagContours()
{

}

medTagContours::medTagContours(QString name, QVector<medWorldPosContours> contourVec)
    :labelName(name), contours(contourVec)
{

}

medTagContours::medTagContours(const medTagContours &other)
{
    labelName = other.labelName;
    contours = other.contours;
}

QTextStream &operator<<(QTextStream &out, const medTagContours &data)
{
    out << "labelName\n" <<data.labelName<< "\ncontours\n";
    for (medWorldPosContours nodes : data.contours)
    {
        out << nodes<<"\n";
    }
    return out;
}

QDataStream &operator>>(QDataStream &in, medTagContours &data)
{
    return in >> data.labelName >> data.contours;
}

QDataStream &operator<<(QDataStream &out, const medTagContours &data)
{
    return out << data.labelName << data.contours;
}

