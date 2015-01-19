/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractEstimateTransformationProcess.h>
#include <medAbstractData.h>
#include <itkImage.h>
#include <itkTransform.h>
#include <medRegistrationExport.h>


class itkProcessRegistrationPrivate;

/**
 * @brief Base class for all registration plugins.
 *
 * An example of an implementation can be found in medDiffeomorphicDemonsRegistrationProcess.
 *
 * This class provides a dtkAbstractProcess that set inputs for a registration.
 * 2 input channels are used for the fixed and moving images.
 * The output is the registered image, not the transformation, since the for goal is to visualize images. The transformation is however reachable through the write(QStringList) function.
 *
 * The programmer may use at his leasure the Registration Programming Interface (RPI)
 * published there: http://gforge.inria.fr/projects/asclepiospublic/
 * to implement the registration algorithms.
 *
 * @note The process has the following dtk property: "transformType".
 * Each subclass should set accordingly the property with either "rigid", or "nonRigid".
 * Example:
 * @code this->setProperty("transformType","rigid"); @endcode
 *
 *
*/
class MEDREGISTRATIONFACTORY_EXPORT itkProcessRegistration : public medAbstractEstimateTransformationProcess
{
    Q_OBJECT

public:
    itkProcessRegistration();
    virtual ~itkProcessRegistration();

public:
    /**
     * @brief Image types.
     *
     * Each item represents the associated type.
     *
    */
    enum ImageType{
        UCHAR,CHAR,
        USHORT,SHORT,
        UINT,INT,
        ULONG,LONG,
        FLOAT,
        DOUBLE
    };

public:

    int update();
    
    virtual bool setFixedInput(medAbstractData *data);
    virtual bool setMovingInput(medAbstractData *data);

    medAbstractData *output();
    void setOutput(medAbstractData * output);

    itk::ImageBase<3>::Pointer fixedImage();
    QVector<itk::ImageBase<3>::Pointer> movingImages();

    itkProcessRegistration::ImageType fixedImageType();
    itkProcessRegistration::ImageType movingImageType();

    //virtual itk::Transform<double,3,3>::Pointer getTransform() = 0;

    /**
    * @brief Returns a pointer on a QStringList containing the title of the algorithm and its parameters. The first element of the list is the title.
    *
    * @return QString
    */
    //virtual QString getTitleAndParameters() = 0;

public slots:
    virtual bool write(const QStringList& files);
    virtual bool write(const QString& file);

protected :
    virtual bool writeTransform(const QString& file);

    virtual int update(ImageType);

    virtual bool setInputData(medAbstractData *data, int channel);

private:
    itkProcessRegistrationPrivate *d;
};

//Implement this function only in actual implementation of registration Plugins
// This class is abstracted, so we won't register it in the factory.
//dtkAbstractProcess *createItkProcessRegistration();


