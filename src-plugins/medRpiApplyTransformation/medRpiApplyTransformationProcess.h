/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medRpiApplyTransformationExport.h>
#include <medAbstractApplyTransformationProcess.h>

#include <itkImage.h>


class medAbstractTransformation;

class medRpiApplyTransformationProcessPrivate;
class MEDRPIAPPLYTRANSFORMATIONPLUGIN_EXPORT medRpiApplyTransformationProcess : public medAbstractApplyTransformationProcess
{
    Q_OBJECT

public:

    typedef itk::Image< float, 3 > RegImageType;

    medRpiApplyTransformationProcess();
    virtual ~medRpiApplyTransformationProcess();

public:
    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

public:
    virtual bool isInteractive() const;

public:
    virtual int update();

public:
    QList<medAbstractParameter*> parameters();

    void addTransformation(medAbstractTransformation *transfo);

private slots:
    void addDisplFieldTransfo(const medDataIndex &);
    void handleEndOfCommand();

private:
    medRpiApplyTransformationProcessPrivate *d;
};


dtkAbstractProcess *createmedRpiApplyTransformation();


