#include "medContourInfo.h"

medContourInfo::medContourInfo(QString name, QColor color, bool selected)
{
    mainName = name;
    mainColor = color;
    this->selected = selected;
    score = false;
    secondName = QString();
    secondColor = QColor::Invalid;
    updateName = false;
}

void medContourInfo::setAdditionalNameAndColor(QString name, QColor color)
{
    score = true;
    secondName = name;
    secondColor = color;
}
