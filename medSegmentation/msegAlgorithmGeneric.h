
#ifndef _msegAlgorithmGeneric_h_
#define _msegAlgorithmGeneric_h_

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkSmartPointer.h>

#include <QHash>
#include <QString>

//Forward definitions
class dtkAbstractData;

namespace mseg {

// Override in derived classes.
struct HandlerFunc {
    virtual ~HandlerFunc() {}
    virtual int run(dtkAbstractData * data) = 0;
};

/** Generic segmentation algorithm */
class AlgorithmGeneric : public dtkAbstractProcess {

public:
    AlgorithmGeneric::AlgorithmGeneric();
    virtual ~AlgorithmGeneric();

    // Override dtkAbstractProcess
    void setInput( dtkAbstractData * data);
    virtual int update();

    dtkAbstractData * input() { return m_inputData; }

protected:

    void addHandler( const QString & typeName, HandlerFunc * func );

    int callHandler( dtkAbstractData * data );
    virtual bool isHandled( const QString & dataDescription ) const;

    void setOutput( dtkAbstractData * data);

private:

    typedef QHash< QString, HandlerFunc * > HandlerContainerType;
    HandlerContainerType m_handlers;

    dtkSmartPointer< dtkAbstractData > m_inputData;
    dtkSmartPointer< dtkAbstractData > m_outputData;
};

} // namespace mseg

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

#endif // _msegAlgorithmGeneric_h_
