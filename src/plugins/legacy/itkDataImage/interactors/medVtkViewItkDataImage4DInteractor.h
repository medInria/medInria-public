/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkViewItkDataImageInteractor.h>

class medVtkViewItkDataImage4DInteractorPrivate;

class medVtkViewItkDataImage4DInteractor : public medVtkViewItkDataImageInteractor
{
    Q_OBJECT

public:
    medVtkViewItkDataImage4DInteractor(medAbstractView* parent);
    ~medVtkViewItkDataImage4DInteractor();

public:
    virtual QString description() const;

    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setInputData(medAbstractData *data);
    virtual bool SetViewInput(medAbstractData* data, int layer);
    virtual medAbstractData *inputData() const;

public slots:
    virtual void setCurrentTime (double time);

private:
    static QStringList dataHandled();
    template <typename IMAGE> bool SetViewInput(const char* type, medAbstractData* data, int layer);
    template <typename volumeType> inline void SetITKDataSet(typename itk::Image<volumeType, 4> *dataset);
    void SetITKDataSet(typename itk::Image<float, 4> *dataset);

private:
    medVtkViewItkDataImage4DInteractorPrivate *d;
    vtkItkConversionInterface *m_poConv;
};


