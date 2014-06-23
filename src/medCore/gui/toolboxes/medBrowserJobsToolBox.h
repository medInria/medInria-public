/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>

class medProgressionStack;
class medBrowserJobsToolBoxPrivate;

class MEDCORE_EXPORT medBrowserJobsToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserJobsToolBox(QWidget *parent = 0);
    ~medBrowserJobsToolBox();

    medProgressionStack *stack();

private:
    medBrowserJobsToolBoxPrivate *d;
};


