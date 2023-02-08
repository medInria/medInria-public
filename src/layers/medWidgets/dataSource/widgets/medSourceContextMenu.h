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
#include <medDataHub.h>

#include <QWidget>
#include <QMenu>


class medSourceContextMenu : public QMenu
{
    Q_OBJECT
public:
    medSourceContextMenu(QWidget *parent = nullptr);
    virtual ~medSourceContextMenu();



public slots:


signals:


private slots:


private:

};
