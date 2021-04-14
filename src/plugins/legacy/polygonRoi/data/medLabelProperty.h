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
#include <polygonLabel.h>

class medLabelProperty
{
public:
    medLabelProperty(QString &name, QColor &color, bool selected = false);
    medLabelProperty(QString &name, QColor &color, int position, bool selected);

//    QString & getName(){return mainName;}
//    QColor & getColor(){return mainColor;}
//    bool isSelected(){return selected;}
//    QString & getAdditionalName(){return secondName;}
//    QColor & getAdditionalColor(){return secondColor;}
//    bool hasScore(){return scoreState;}
//    bool nameChanged(){return changeName;}
//    bool descUpdated(){return updateDesc;}
//    bool colorUpdated(){return updateColor;}
//    bool checkState(){return checked;}
//    int getPosition(){return position;}
//
    void setAdditionalNameAndColor(QString &name, QColor &color);
//    void setScoreInfo(bool state);
//    void setSelected(bool state){selected = state;}
//    void setChangeName(bool state){changeName = state;}
//    void setUpdateDesc(bool state){updateDesc = state;}
//    void setUpdateColor(bool state){updateColor = state;}
//    void setCheckState(bool state){checked = state;}

//private:
    QString mainName;
    QColor mainColor;
    QString secondName;
    QColor secondColor;
    int position;
    bool selected;
    bool scoreState;
    bool changeName;
    bool updateColor;
    bool checked;
    bool updateDesc;
};
