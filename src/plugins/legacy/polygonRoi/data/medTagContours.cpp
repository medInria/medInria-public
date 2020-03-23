#include <medTagContours.h>


medTagContours::medTagContours()
{

}

medTagContours::medTagContours(qint32 lab, QVector<medWorldPosContours> contourVec)
    :label(lab), contours(contourVec)
{

}

medTagContours::medTagContours(const medTagContours &other)
{
    label = other.label;
    contours = other.contours;
}

medTagContours::~medTagContours()
{

}

QTextStream &operator<<(QTextStream &out, const medTagContours &data)
{
    out << "label\n" <<data.label<< "\ncontours\n";
    for (medWorldPosContours nodes : data.contours)
        out << nodes<<"\n";
    return out;
}

QDataStream &operator>>(QDataStream &in, medTagContours &data)
{
    return in >> data.label >> data.contours;
}

QDataStream &operator<<(QDataStream &out, const medTagContours &data)
{
    return out << data.label << data.contours;
}

