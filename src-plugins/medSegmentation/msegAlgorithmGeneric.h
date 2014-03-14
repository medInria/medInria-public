/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <msegPluginExport.h>

#include <medAbstractProcess.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkObject.h>

#include <QHash>
#include <QString>

//Forward definitions
class medAbstractData;

// Override in derived classes.
struct HandlerFunc {
    virtual ~HandlerFunc() {}
    virtual int run(medAbstractData * data) = 0;
};

class AlgorithmGenericPrivate;
class AlgorithmParametersWidget;
class Controller;



/** Generic segmentation algorithm */
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT AlgorithmGeneric : public medAbstractProcess
{
    Q_OBJECT
public:
    //! Interface name should be used by all dtkAbstractProcesses inheriting from this.
    static QString ms_interfaceName;

public:
    AlgorithmGeneric();
    virtual ~AlgorithmGeneric();

    virtual QString localizedName() = 0;

    //! Override dtkAbstractProcess
    void setInput( medAbstractData * data) MED_OVERRIDE;

    virtual int update() MED_OVERRIDE;
    medAbstractData * output() MED_OVERRIDE;

    //! Getter for the input data.
    medAbstractData * input();

    //! Progress
    virtual void reportItkProgress( const itk::Object * caller , float progress );

    //! Undo support (not implemented yet)
    virtual bool isUndoAble();
    virtual void undo();

protected:

    void addHandler( const QString & typeName, HandlerFunc * func );

    int callHandler( medAbstractData * data );
    virtual bool isHandled( const QString & dataId ) const;

    void setOutput( medAbstractData * data);

    //! Uses the input to set suitable values on the output.
    void setOutputMetadata();

private:
    AlgorithmGenericPrivate *d;
    
    using dtkAbstractProcess::setInput;
};


#define MSEG_ADD_HANDLERS_FOR_ALL_SCALAR_3D_TYPES( handlerName, constructorParameters ) \
    this->addHandler( "itkDataImageChar3", new handlerName<char,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageShort3", new handlerName<short,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageInt3", new handlerName<int,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageLong3", new handlerName<long,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageUChar3", new handlerName<unsigned char,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageUShort3", new handlerName<unsigned short,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageUInt3", new handlerName<unsigned int,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageULong3", new handlerName<unsigned long,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageFloat3", new handlerName<float,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageDouble3", new handlerName<double,3>(constructorParameters) )

#define MSEG_ADD_HANDLERS_FOR_ALL_VECTOR_3D_TYPES( handlerName, constructorParameters ) \
    this->addHandler( "itkDataImageVector3", new handlerName<UCharVector3Type,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageRGB3", new handlerName<RGBPixelType,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageRGBA3", new handlerName<RGBAPixelType,3>(constructorParameters) )


