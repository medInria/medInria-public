/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui>
#include <QtWidgets>

class medToolBoxBodyPrivate;
class medToolBoxTab;

class MEDCORE_EXPORT medToolBoxBody : public QFrame
{
    Q_OBJECT

public:
     medToolBoxBody(QWidget *parent = 0);
    ~medToolBoxBody();

    void addWidget(QWidget *widget);

    void setTabWidget(medToolBoxTab * tab);
    void clear();

    void setOrientation(Qt::Orientation);

private:
    medToolBoxBodyPrivate *d;
};


