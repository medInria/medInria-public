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

#include <medAbstractWorkspace.h>

class medTabbedViewContainers;
class medRegistrationWorkspacePrivate;

class medRegistrationWorkspace : public medAbstractWorkspace
{
    Q_OBJECT

public:
     medRegistrationWorkspace(QWidget *parent = 0);
    ~medRegistrationWorkspace();

    virtual QString identifier()  const;
    virtual QString description() const;
    static bool isUsable();
    void setupViewContainerStack ();

public slots:
    void patientChanged(int patientId);

private:
    medRegistrationWorkspacePrivate *d;
};


