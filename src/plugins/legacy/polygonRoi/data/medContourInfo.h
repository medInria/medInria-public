/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QColor>
#include <QString>
#include <medTagRoiManager.h>

class medContourInfo
{
public:
    medContourInfo(QString name, QColor color, bool selected = false);

    QString getName(){return mainName;}
    QColor getColor(){return mainColor;}
    bool isSelected(){return selected;}
    QString getAdditionalName(){return secondName;}
    QColor getAdditionalColor(){return secondColor;}
    bool scoreState(){return score;}
    bool nameUpdated(){return updateName;}

    void setAdditionalNameAndColor(QString name, QColor color);
    void setScore(bool state){score = state;}
    void setSelected(bool state){selected = state;}
    void setUpdateName(bool state){updateName = state;}
private:
    QString mainName;
    QColor mainColor;
    QString secondName;
    QColor secondColor;
    bool selected;
    bool score;
    bool updateName;
};
