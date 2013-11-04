/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once
#define ITKPROCESSEXAMPLEDENOISINGTOOLBOX_H

#include <medFilteringAbstractToolBox.h>

class ITKProcessExampleToolBoxPrivate;

class ITKProcessExampleToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT

public:
     ITKProcessExampleToolBox(QWidget *parentToolBox = 0);
    ~ITKProcessExampleToolBox();

    dtkPlugin*          plugin();

public:
    static bool registered();
    dtkAbstractData* processOutput();
    dtkPlugin* plugin();

signals:
    void success();
    void failure();

public slots:

    void run();

private:
    ITKProcessExampleToolBoxPrivate *d;
};



