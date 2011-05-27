#ifndef MSEGALGORITHMGENERIC_H
#define MSEGALGORITHMGENERIC_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkObject.h>

#include <QHash>
#include <QString>

// override is a C++0x feature implemented in MSVC
#if defined(_MSC_VER)
  #define MED_OVERRIDE override
#else 
  // Not in gcc (yet?)
  #define MED_OVERRIDE
#endif

//Forward definitions
class dtkAbstractData;

namespace mseg {

// Override in derived classes.
struct HandlerFunc {
    virtual ~HandlerFunc() {}
    virtual int run(dtkAbstractData * data) = 0;
};

class AlgorithmGenericPrivate;
class AlgorithmParametersWidget;
class Controller;

class AlgorithmParametersWidget : public QWidget {
public:
    AlgorithmParametersWidget( Controller * controller, QWidget * parent ) : 
      QWidget(parent), 
          m_controller(controller) {}
    virtual ~AlgorithmParametersWidget() {}

protected:
    Controller * controller() { return m_controller; }

private:
    Controller * m_controller;
};

/** Generic segmentation algorithm */
class AlgorithmGeneric : public dtkAbstractProcess {
    Q_OBJECT;
public:
    //! Interface name should be used by all dtkAbstractProcesses inheriting from this.
    static QString ms_interfaceName;

public:
    AlgorithmGeneric();
    virtual ~AlgorithmGeneric();

    virtual QString localizedName() = 0;

    //! Override dtkAbstractProcess
    void setInput( dtkAbstractData * data) MED_OVERRIDE;
    virtual int update() MED_OVERRIDE;

    //! Getter for the input data.
    dtkAbstractData * input();

    //! Progress
    virtual void reportItkProgress( const itk::Object * caller , float progress );

    //! Undo support (not implemented yet)
    virtual bool isUndoAble();
    virtual void undo();

    virtual AlgorithmParametersWidget *createParametersWidget(Controller * controller, QWidget *parent);

protected:

    void addHandler( const QString & typeName, HandlerFunc * func );

    int callHandler( dtkAbstractData * data );
    virtual bool isHandled( const QString & dataDescription ) const;

    void setOutput( dtkAbstractData * data);

private:
    AlgorithmGenericPrivate *d;

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

#endif // MSEGALGORITHMGENERIC_H
