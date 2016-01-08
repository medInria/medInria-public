/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <medCoreExport.h>

class MEDCORE_EXPORT medComboBox: public QComboBox {

    Q_OBJECT

public:

    medComboBox(QWidget* parent=0): QComboBox(parent)
    {
        this->setFocusPolicy(Qt::StrongFocus);
    }
    
    void wheelEvent(QWheelEvent *e)
    {
        if(hasFocus())
        {
            QComboBox::wheelEvent(e);
        }
    }
};
