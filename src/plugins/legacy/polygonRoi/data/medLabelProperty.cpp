#include "medLabelProperty.h"

medLabelProperty::medLabelProperty(QString &name, QColor &color, bool selected)
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

medLabelProperty::medLabelProperty(QString &name, QColor &color, int position, bool selected)
{
    mainName = name;
    mainColor = color;
    this->position = position;
    this->selected = selected;
    scoreState = false;
    secondName = QString();
    secondColor = QColor::Invalid;
    changeName = false;
    updateColor = false;
    checked = false;
    updateDesc = false;
}

void medLabelProperty::setAdditionalNameAndColor(QString &name, QColor &color)
{
    scoreState = true;
    secondName = name;
    secondColor = color;
}

//void medLabelProperty::setScoreInfo(bool state)
//{
//    scoreState = state;
//}


