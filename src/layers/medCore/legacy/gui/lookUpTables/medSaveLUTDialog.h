#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QInputDialog>
#include <QWidget>

#include <medCoreExport.h>

class MEDCORE_EXPORT medSaveLUTDialog : public QInputDialog
{
    Q_OBJECT
public:
    medSaveLUTDialog(const QString & name="Unknown",QWidget *parent = nullptr);
};


