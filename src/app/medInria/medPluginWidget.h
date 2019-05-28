/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "qobjectdefs.h"  // for Q_OBJECT, signals, slots
#include "qstring.h"      // for QString
#include "qtabwidget.h"   // for QTabWidget
class QObject;
class QTreeWidgetItem;
class QWidget;
class medPluginWidgetPrivate;

class medPluginWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit medPluginWidget(QWidget *parent = 0);
    ~medPluginWidget();

signals:

public slots:
    void onPluginTreeItemActivated(QTreeWidgetItem* item ,int column);
    void onErrorTreeItemActivated(QTreeWidgetItem* item,int column);
protected:
    void reset();

private:
    medPluginWidgetPrivate * d;
};


