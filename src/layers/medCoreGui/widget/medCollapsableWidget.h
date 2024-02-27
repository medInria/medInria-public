#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medCoreGuiExport.h>

#include <QWidget>

class medSpoilerWidgetPrivate;
class MEDCOREGUI_EXPORT medSpoilerWidget : public QWidget
{
    Q_OBJECT
public:
    enum direction{up, down, left, right};
    explicit medSpoilerWidget(const QString & title = "", const int animationDuration = 333, QWidget *parent = 0);
    virtual ~medSpoilerWidget();

    void setDirection(direction direc);
    void setContentLayout(QLayout & contentLayout);

private:
    medSpoilerWidgetPrivate *d;
};