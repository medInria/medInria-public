/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <medWorkspace.h>


class medVisualizationWorkspacePrivate;

class medVisualizationWorkspace : public medWorkspace
{
    Q_OBJECT

public:
     medVisualizationWorkspace(QWidget *parent = 0);
    ~medVisualizationWorkspace();

    virtual void setupViewContainerStack();

    virtual QString identifier()  const;
    virtual QString description() const;
    static bool isUsable();

private:
    medVisualizationWorkspacePrivate *d;
};


