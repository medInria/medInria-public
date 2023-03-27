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

    void setAdditionalNameAndColor(QString &name, QColor &color);

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
