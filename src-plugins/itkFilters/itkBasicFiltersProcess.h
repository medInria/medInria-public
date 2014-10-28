/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersProcessBase.h>
#include <medAbstractData.h>

#include <itkFiltersPluginExport.h>

class itkBasicFiltersProcessPrivate;

class ITKFILTERSPLUGIN_EXPORT itkBasicFiltersProcess : public itkFiltersProcessBase
{
    Q_OBJECT

public:
    itkBasicFiltersProcess(itkBasicFiltersProcess * parent = 0);
    virtual ~itkBasicFiltersProcess(void);

    static bool registered ( void );

public:
    virtual void setInputImage ( medAbstractData *data );
    medAbstractData *output ( void );

public slots:
    int update ( void );
    void setupProcess(QString);

public:
    QList<medAbstractParameter*> parameters();
    medToolBox* toolbox();

private:
    itkBasicFiltersProcessPrivate *d;
};

dtkAbstractProcess * createitkBasicFiltersProcess();
