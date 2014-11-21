/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractData.h>

#include <medItkDiffusionGradientListExport.h>

class medItkDiffusionGradientListDataPrivate;

class medItkDiffusionGradientListPlugin_EXPORT medItkDiffusionGradientListData : public medAbstractData
{
    Q_OBJECT
    MED_DATA_INTERFACE("Itk Data Diffusion Gradient List",
                       "Data type to store diffusion gradients.")
public:
             medItkDiffusionGradientListData();
    virtual ~medItkDiffusionGradientListData();

    static bool registered();

    virtual void *data();
    virtual void *data(int channel);
    virtual void setData(void *data);

public slots:
    bool read (const QString& file);
    bool write(const QString& file);

private:
    medItkDiffusionGradientListDataPrivate *d;
};



