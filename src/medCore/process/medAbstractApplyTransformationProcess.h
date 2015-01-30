/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <medAbstractProcess.h>

class medAbstractData;
class medAbstractImageData;
class medAbstractTransformation;

class medAbstractApplyTransformationProcessPrivate;
class MEDCORE_EXPORT medAbstractApplyTransformationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractApplyTransformationProcess(medAbstractProcess *parent = NULL);
    virtual ~medAbstractApplyTransformationProcess();

public:
    virtual bool isInteractive() const = 0;
    virtual int update() = 0;

    virtual void addTransformation(medAbstractTransformation *transfo);
    void addTransformations(QList<medAbstractTransformation *> transfo);
    QList<medAbstractTransformation *> transformationStack() const;

    virtual medToolBox* toolbox();

    void setGeometry(medAbstractImageData *geometry);
    medAbstractImageData*  geometry() const;

    void setInputImage(medAbstractImageData *imageData);
    medAbstractImageData* inputImage() const;

    QUndoStack *commandStack();
    QUndoView *commandView();

    virtual QList<medAbstractParameter*> parameters();

protected slots:
    void addSVFTransfoDialog();
    void resetTransformationStack();

signals:
    void addSVFTransfoRequest(medAbstractImageData *data);

private:
    medAbstractApplyTransformationProcessPrivate* d;

};
