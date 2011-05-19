
#ifndef _msegAlgorithmGeneric_h_
#define _msegAlgorithmGeneric_h_

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkSmartPointer.h>

#include <QHash>
#include <QString>

//Forward definitions
class dtkAbstractData;

namespace mseg {

// Override in derived classes. MUST be copy-constructable.
struct HandlerFunc {
    virtual int run(dtkAbstractData * data) {return DTK_FAILURE;}
};

/** Generic segmentation algorithm */
class AlgorithmGeneric : public dtkAbstractProcess {

public:
    AlgorithmGeneric::AlgorithmGeneric();
    virtual ~AlgorithmGeneric();

    // Override dtkAbstractProcess
    void setInput( dtkAbstractData * data);

protected:

    void addHandler( const QString & typeName, HandlerFunc & func );

    int callHandler( dtkAbstractData * data );
    virtual bool isHandled( const QString & dataDescription ) const;

    dtkAbstractData * input() { return m_inputData; }

private:

    typedef QHash< QString, HandlerFunc > HandlerContainerType;
    HandlerContainerType m_handlers;

    dtkSmartPointer< dtkAbstractData > m_inputData;
};

} // namespace mseg

#define MSEG_ADD_HANDLERS_FOR_ALL_SCALAR_3D_TYPES( handlerName, constructorParameters ) \
    this->addHandler( "itkDataImageChar3", handlerName<char,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageShort3", handlerName<short,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageInt3", handlerName<int,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageLong3", handlerName<long,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageUChar3", handlerName<unsigned char,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageUShort3", handlerName<unsigned short,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageUInt3", handlerName<unsigned int,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageULong3", handlerName<unsigned long,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageFloat3", handlerName<float,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageDouble3", handlerName<double,3>(constructorParameters) )

#define MSEG_ADD_HANDLERS_FOR_ALL_VECTOR_3D_TYPES( handlerName, constructorParameters ) \
    this->addHandler( "itkDataImageVector3", handlerName<UCharVector3Type,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageRGB3", handlerName<RGBPixelType,3>(constructorParameters) ); \
    this->addHandler( "itkDataImageRGBA3", handlerName<RGBAPixelType,3>(constructorParameters) )

#endif // _msegAlgorithmGeneric_h_
