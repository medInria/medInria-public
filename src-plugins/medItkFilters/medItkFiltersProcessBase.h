/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractFilteringProcess.h>
#include <medAbstractData.h>

#include <itkCommand.h>

#include <medItkFiltersPluginExport.h>

class medItkFiltersProcessBasePrivate;

class medItkFiltersPlugin_EXPORT medItkFiltersProcessBase : public medAbstractFilteringProcess
{
    Q_OBJECT

public:
    medItkFiltersProcessBase(medItkFiltersProcessBase * parent = 0);
    medItkFiltersProcessBase(const medItkFiltersProcessBase& other);
    virtual ~medItkFiltersProcessBase(void);

public:
    medItkFiltersProcessBase& operator = (const medItkFiltersProcessBase& other);

    QString description() const;
    void setDescription(QString description);

    QString filterType() const;
    void setFilterType(QString filterType);

    itk::CStyleCommand::Pointer callback() const;
    void setCallBack(itk::CStyleCommand::Pointer);

    medItkFiltersProcessBase *filter() const;
    void setFilter(medItkFiltersProcessBase *);

    void emitProgress(int progress);

public:
    virtual QList<medAbstractParameter*> parameters();
    bool isInteractive() const;


private:
    medItkFiltersProcessBasePrivate *d;

};



