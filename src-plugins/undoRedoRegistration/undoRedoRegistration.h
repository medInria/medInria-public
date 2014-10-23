/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkProcessRegistration.h>
#include <medAbstractUndoRedoProcess.h>
#include <undoRedoRegistrationPluginExport.h>
#include <itkImage.h>

class undoRedoRegistrationPrivate;

/**
 * @brief Registration process.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medCore/gui.
 *
 */
class UNDOREDOREGISTRATIONPLUGIN_EXPORT undoRedoRegistration : public itkProcessRegistration, public medAbstractUndoRedoProcess
{
    Q_OBJECT
    
public:

    typedef itk::Image< float, 3 > RegImageType;

    undoRedoRegistration(void);
    virtual ~undoRedoRegistration(void);
    
public:
    virtual QString description(void) const;
    static bool registered(void);

public:
    medToolBox* toolbox();
    QList<medAbstractParameter*> parameters();

public:
    void setCurrentProcess(itkProcessRegistration* process);
    itkProcessRegistration* currentProcess();

public:
    virtual itk::Transform<double,3,3>::Pointer getTransform();
    virtual QString getTitleAndParameters();

public slots:
    void undo(); 
    void redo();
    void reset();

    void generateOutput(bool algorithm = false,itkProcessRegistration *process=NULL);

protected :
    virtual bool writeTransform(const QString& file);
    virtual bool setInputData(medAbstractData *data, int channel);

protected slots:
    void onRegistrationSuccess();

signals:
    void registrationDone();
    void stackReset();

private:
    undoRedoRegistrationPrivate *d;
    
};

dtkAbstractProcess *createUndoRedoRegistration(void);


