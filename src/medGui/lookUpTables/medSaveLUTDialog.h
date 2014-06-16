/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QInputDialog>
#include <QWidget>

class medSaveLUTDialog : public QInputDialog
{
    Q_OBJECT
public:
    medSaveLUTDialog(const QString & name="Unknown",QWidget *parent = 0);

signals:

public slots:

};


