/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <undoRedoRegistration.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <registrationFactory.h>
#include <itkImage.h>
#include <medUndoRedoToolBox.h>


class undoRedoRegistrationPrivate
{
public:
    itkProcessRegistration* currentProcess;

};

// /////////////////////////////////////////////////////////////////
// undoRedoRegistration
// /////////////////////////////////////////////////////////////////

undoRedoRegistration::undoRedoRegistration(void) : itkProcessRegistration(), medAbstractUndoRedoProcess(), d(new undoRedoRegistrationPrivate)
{
}

undoRedoRegistration::~undoRedoRegistration(void)
{
}

bool undoRedoRegistration::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("undoRedoRegistration",
                                                                      createUndoRedoRegistration, "itkUndoRedoProcessRegistration");
}

QString undoRedoRegistration::description(void) const
{
    return "undoRedoRegistration";
}

/**
 * @brief
 *
 * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
 * @return bool successful or not.
 */
bool undoRedoRegistration::writeTransform(const QString& file)
{
    return false;
}

itk::Transform<double,3,3>::Pointer undoRedoRegistration::getTransform()
{
    return NULL;
}

QString undoRedoRegistration::getTitleAndParameters()
{
    return QString();
}

void undoRedoRegistration::setCurrentProcess(itkProcessRegistration* process)
{
    d->currentProcess = process;
}

itkProcessRegistration* undoRedoRegistration::currentProcess()
{
    return  d->currentProcess;
}

void undoRedoRegistration::undo()
{
    itk::ImageRegistrationFactory<RegImageType>::Pointer m_factory = registrationFactory::instance()->getItkRegistrationFactory();
    m_factory->Undo();
    generateOutput();
}

void undoRedoRegistration::redo()
{
    itk::ImageRegistrationFactory<RegImageType>::Pointer m_factory = registrationFactory::instance()->getItkRegistrationFactory();
    m_factory->Redo();
    generateOutput();
}

void undoRedoRegistration::reset()
{
    //TODO
}

bool undoRedoRegistration::setInputData(medAbstractData *data, int channel)
{
    bool result = itkProcessRegistration::setInputData(data,channel);
    if (result)
    {
        typedef itk::Image< float, 3 > RegImageType;
        itk::ImageRegistrationFactory<RegImageType>::Pointer m_factory = registrationFactory::instance()->getItkRegistrationFactory();

        if (channel==0)
            m_factory->SetFixedImage((RegImageType*)this->fixedImage().GetPointer());
        else if (channel==1 && this->movingImages().size() > 0)
            m_factory->SetMovingImage((RegImageType*)this->movingImages()[0].GetPointer());

        registrationFactory::instance()->reset();
    }

    return result;
}

void undoRedoRegistration::generateOutput(bool algorithm,itkProcessRegistration * process)
{
    typedef itk::Image< float, 3 > RegImageType;
    itk::ImageRegistrationFactory<RegImageType>::Pointer m_factory = registrationFactory::instance()->getItkRegistrationFactory();
    if (m_factory->GetFixedImage()!=NULL && m_factory->GetMovingImage()!=NULL){
        m_factory->Update();
        itk::ImageBase<3>::Pointer result = m_factory->GetOutput();
        result->DisconnectPipeline();
        if (algorithm && process)
        {
            if (process->output())
                process->output()->setData(result);
        }
        else
            if ((!algorithm) && (this->output()))
                this->output()->setData(result);
    }
}

void undoRedoRegistration::onRegistrationSuccess()
{
    registrationFactory::instance()->addTransformation(d->currentProcess->getTransform(), d->currentProcess->getTitleAndParameters());
    registrationFactory::instance()->getItkRegistrationFactory()->Modified();
    this->generateOutput(true, d->currentProcess);
    //this->parentToolBox()->handleOutput();
}

medToolBox* undoRedoRegistration::toolbox()
{
    return this->undoRedoToolBox();
}

QList<medAbstractParameter*> undoRedoRegistration::parameters()
{
    return QList<medAbstractParameter*>();
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////
/**
 * @brief
 *
 * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
 * @return bool successful or not.
 */
dtkAbstractProcess *createUndoRedoRegistration(void)
{
    return new undoRedoRegistration;
}
