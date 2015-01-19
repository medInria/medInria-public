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


class medAbstractTransformation;

class medRpiApplyTransformationProcessPrivate;
class MEDRPIAPPLYTRANSFORMATIONPLUGIN_EXPORT medRpiApplyTransformationProcess : public medAbstractApplyTransformationProcess
{
    Q_OBJECT

public:

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

private slots:
    void onSVFDropped(const medDataIndex &);

private:
    medRpiApplyTransformationProcessPrivate *d;
};


dtkAbstractProcess *createmedRpiApplyTransformation();


