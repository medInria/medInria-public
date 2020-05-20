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

class medContourSharedInfo
{
public:
    medContourSharedInfo(QString name, QColor color, bool selected = false);

    QString getName(){return mainName;}
    QColor getColor(){return mainColor;}
    bool isSelected(){return selected;}
    QString getAdditionalName(){return secondName;}
    QColor getAdditionalColor(){return secondColor;}
    bool hasScore(){return scoreState;}
    bool nameUpdated(){return updateName;}
    bool checkState(){return checked;}

    void setAdditionalNameAndColor(QString name, QColor color);
    void setScoreInfo(bool state);
    void setSelected(bool state){selected = state;}
    void setUpdateName(bool state){updateName = state;}
    void setCheckState(bool state){checked = state;}

private:
    QString mainName;
    QColor mainColor;
    QString secondName;
    QColor secondColor;
    bool selected;
    bool scoreState;
    bool updateName;
    bool checked;
};
