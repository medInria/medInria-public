/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkFiltersProcessBase.h>
#include <medAbstractData.h>

#include <medItkFiltersPluginExport.h>

class medItkBasicFiltersProcessPrivate;

class medItkFiltersPlugin_EXPORT medItkBasicFiltersProcess : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkBasicFiltersProcess(medItkBasicFiltersProcess * parent = 0);
    virtual ~medItkBasicFiltersProcess(void);

    static bool registered ( void );

public slots:
    int update ( void );
    void setupProcess(QString);

public:
    QList<medAbstractParameter*> parameters();
    medToolBox* toolbox();

protected slots:
    void handleInput();

private:
    medItkBasicFiltersProcessPrivate *d;
};

dtkAbstractProcess * createmedItkBasicFiltersProcess();
