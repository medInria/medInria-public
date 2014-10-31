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

#include <itkFiltersPluginExport.h>

class itkFiltersProcessBasePrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersProcessBase : public medAbstractFilteringProcess
{
    Q_OBJECT

public:
    itkFiltersProcessBase(itkFiltersProcessBase * parent = 0);
    itkFiltersProcessBase(const itkFiltersProcessBase& other);
    virtual ~itkFiltersProcessBase(void);
    
public:
    itkFiltersProcessBase& operator = (const itkFiltersProcessBase& other);
        
    QString description() const;
    void setDescription(QString description);

    QString filterType() const;
    void setFilterType(QString filterType);

    itk::CStyleCommand::Pointer callback() const;
    void setCallBack(itk::CStyleCommand::Pointer);

    itkFiltersProcessBase *filter() const;
    void setFilter(itkFiltersProcessBase *);

    void emitProgress(int progress);

public:
    virtual QList<medAbstractParameter*> parameters();
    bool isInteractive();


private:
    itkFiltersProcessBasePrivate *d;

};



