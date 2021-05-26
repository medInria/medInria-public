#include "medContourSharedInfo.h"

medContourSharedInfo::medContourSharedInfo(QString name, QColor color, bool selected)
{
    mainName = name;
    mainColor = color;
    this->selected = selected;
    scoreState = false;
    secondName = QString();
    secondColor = QColor::Invalid;
    changeName = false;
    updateColor = false;
    checked = false;
    updateDesc = false;
}

void medContourSharedInfo::setAdditionalNameAndColor(QString name, QColor color)
{
    scoreState = true;
    secondName = name;
    secondColor = color;
}

void medContourSharedInfo::setScoreInfo(bool state)
{
    scoreState = state;
}
