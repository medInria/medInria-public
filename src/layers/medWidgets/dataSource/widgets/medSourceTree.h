#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medWidgetsExport.h>
#include <medDataModel.h>

#include <QWidget>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

class medSourceItemModelPresenter;

class medSourceTree : public QTreeView
{
    Q_OBJECT
public:
    medSourceTree(QWidget *parent = nullptr);
    virtual ~medSourceTree();



public slots:


signals:


private slots:


private:

};
