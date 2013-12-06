/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>
#include <itkImage.h>
#include "itkTransform.h"
#include "medRegistrationExport.h"


class itkProcessRegistrationPrivate;

/**
 * @brief Base class for all registration plugins.
 *
 * An example of an implementation can be found in itkProcessRegistrationDiffeomorphicDemons.
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
class MEDREGISTRATIONFACTORY_EXPORT itkProcessRegistration : public dtkAbstractProcess
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param void
     */
    itkProcessRegistration();

    /**
     * @brief
     *
     * @param void
    */
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
    /**
     * @brief Runs the process.
     *
     * @param void
     * @return int 0 if it succeeded, any other value is an error. (could be used as error code)
    */
    int update();

    /**
     * @brief Sets the fixed or moving image paramters of the process.
     *
     * Directly converts the inputs in <float, 3> images.
     * The output is allocated and is also a <float,3 image>
     * @param data: Pointer to an itkDataImageXXY.
     * @param channel: 0 for the fixed image, 1 for the moving one.
    */
    virtual void setInput(dtkAbstractData *data, int channel);

    /**
     * @brief Gets the registered image.
     *
     * @param void
     * @return dtkAbstractData *: itkDataImageXXY, same type as the moving input image.
    */
    dtkAbstractData *output();

    void setOutput(dtkAbstractData * output);

    /**
     * @brief Gets an itk smart pointer to the fixed image.
     *
     * It uses the itkImageBase class to avoid a templated method.
     * Using the fixedImageType() method will give the type necessary for a down cast.
     *
     * @return itk::ImageBase<int> NULL if none is set yet.
    */
    itk::ImageBase<3>::Pointer fixedImage();

    /**
     * @brief Gets an itk smart pointer to the moving image.
     *
     * it uses the itkImageBase class to avoid a templated method.
     * Using the movingImageType() method will give the type necessary for a down cast.
     *
     * @return itk::ImageBase<int> NULL if none is set yet.
    */
    QVector<itk::ImageBase<3>::Pointer> movingImages();

    /**
     * @brief Gets the fixed image ImageType.
     *
     * @return itkProcessRegistration::ImageType
    */
    itkProcessRegistration::ImageType fixedImageType();

    /**
     * @brief Gets the moving image ImageType.
     *
     * @return itkProcessRegistration::ImageType
    */
    itkProcessRegistration::ImageType movingImageType();

    virtual itk::Transform<double,3,3>::Pointer getTransform() = 0;

    /**
    * @brief Returns a pointer on a QStringList containing the title of the algorithm and its parameters. The first element of the list is the title.
    * 
    * @return QString
    */
    virtual QString getTitleAndParameters() = 0;


public slots:
    /**
     * @brief Writes to a file an image or a transformation.
     *
     * This function is inherited from dtk.
     * @warning This function writes the image in the first file,
     * and the transformation in the second. Otheritems in the list are ignored.
     * An empty string as a first element with a path as the second only writes the transformation.
     * A single element in the list means only the image will be written.
     *
     * This function is usualy called from the generic registration toolbox.
     *
     * @param files: list of File path. Here files[0] is a path to the image,
     * and files[1] a path to a transformation.
     * @return bool: true on successful operation, false otherwise.
    */
    virtual bool write(const QStringList& files);

    /**
     * @brief Writes the resulting image to a file.
     *
     * Uses a dtkAbstractDataWriter to export it the file.
     * If no suitable writer is found by the factory, the method returns false.
     * @param file: path to the file.
     * @return bool: true on successful operation, false otherwise.
    */
    virtual bool write(const QString& file);

protected :
    /**
     * @brief Writes a transformation to a file.
     *
     * Given the registration (rigid, non-rigid),
     * this could be a simple matrix or a displacement field.
     *
     * @param file: path to the file.
     * @return bool: true on successful operation, false otherwise.
    */
    virtual bool writeTransform(const QString& file);

    /**
     * @brief Apply the update on a specified fixed image type.
     *
     * Called by update(). The image type is the fixed image type.
     *
     * @param ImageType: the fixed image type
     * @return int
    */
    virtual int update(ImageType);

private:
    itkProcessRegistrationPrivate *d;

};

//Implement this function only in actual implementation of registration Plugins
// This class is abstracted, so we won't register it in the factory.
//dtkAbstractProcess *createItkProcessRegistration();


