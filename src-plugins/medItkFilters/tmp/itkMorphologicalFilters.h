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

class itkMorphologicalFiltersPrivate;

class ITKFILTERSPLUGIN_EXPORT itkMorphologicalFilters : public itkFiltersProcessBase
{
    Q_OBJECT

public:
    itkMorphologicalFilters(itkMorphologicalFilters * parent = 0);
    virtual ~itkMorphologicalFilters(void);

    static bool registered ( void );

public slots:
    int update ( void );
    void setupProcess(QString);

public:
    QList<medAbstractParameter*> parameters();
    medToolBox* toolbox();

private:
    itkMorphologicalFiltersPrivate *d;
};

dtkAbstractProcess * createitkMorphologicalFilters();
