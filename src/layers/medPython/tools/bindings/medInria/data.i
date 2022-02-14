%{
#include <dtkCoreSupport/dtkAbstractData.h>
#include <dtkCoreSupport/dtkAbstractObject.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractMeshData.h>
#include <medDataIndex.h>
#include <medDataManager.h>
%}

%inline
%{
void debug(QString message)
{
    qDebug() << message;
}
%}

%rename(DataIndex) medDataIndex;
%include "medDataIndex.h"

%qListTypemaps(medDataIndex, SWIG_TYPECHECK_OBJECT_ARRAY)

%ignore dtkAbstractData::draw;
%ignore dtkAbstractData::output;
%ignore dtkAbstractData::parameter;
%ignore dtkAbstractData::setParameter;
%ignore dtkAbstractData::update;
%ignore dtkAbstractData::thumbnail;
%ignore dtkAbstractData::thumbnails;
%ignore dtkAbstractData::toVariant;
%ignore dtkAbstractData::fromVariant;

%feature("ref") dtkAbstractObject "$this->retain();";
%feature("unref") dtkAbstractObject "$this->release();";

class dtkAbstractObject : public QObject
{
Q_OBJECT

public:
    virtual QString description() const;
    virtual QString identifier() const;
    virtual QString name() const;

    int count() const;
    int retain() const;
    int release() const;

    void enableDeferredDeletion(bool value);
    bool isDeferredDeletionEnabled() const;

    bool hasProperty(const QString& key) const;
    void addProperty(const QString& key, const QStringList& values);
    void addProperty(const QString& key, const QString& value);
    void setProperty(const QString& key, const QString& value);
    QStringList propertyList() const;
    QStringList propertyValues(const QString& key) const;
    QString property(const QString& key) const;

    bool hasMetaData(const QString& key) const;
    void addMetaData(const QString& key, const QStringList& values);
    void addMetaData(const QString& key, const QString& value);
    void setMetaData(const QString& key, const QStringList& values);
    void setMetaData(const QString& key, const QString& value);
    QStringList metaDataList() const;
    QStringList metaDataValues(const QString& key) const;
    QString metadata(const QString& key) const;
    QStringList metadatas(const QString& key) const;
    void copyMetaDataFrom(const dtkAbstractObject *obj);
};

%include "dtkCoreSupport/dtkSmartPointer.h"
%include "dtkCoreSupport/dtkAbstractData.h"

%feature("director") medAbstractData;
%rename(AbstractData) medAbstractData;
%include "medAbstractData.h"

%feature("novaluewrapper") dtkSmartPointer<medAbstractData>;
%template() dtkSmartPointer<medAbstractData>;

%pythoncode
%{

    def data_interface(name, description):
        def decorator_data(cls):
            cls.staticIdentifier = classmethod(lambda _cls : sys.modules[cls.__module__].__package__ + '.' + cls.__name__)
            cls.staticName = staticmethod(lambda : name)
            cls.staticDescription = staticmethod(lambda : description)
            cls.identifier = lambda self : self.staticIdentifier()
            cls.name = lambda self : self.staticName()
            cls.description = lambda self : self.staticDescription()
            return cls
        return decorator_data

%}

%rename(AbstractMeshData) medAbstractMeshData;
%include "medAbstractMeshData.h"

%rename(AbstractImageData) medAbstractImageData;
%include "medAbstractImageData.h"

%fragment("typecheck"{medAbstractData*}, "header")
{
    int medAbstractData_Check(PyObject* input)
    {
        if (!SWIG_IsOK(SWIG_ConvertPtr(input, nullptr, $descriptor(medAbstractData*), 0)))
        {
            if (!SWIG_IsOK(SWIG_ConvertPtr(input, nullptr, $descriptor(dtkSmartPointer<medAbstractData>*), 0)))
            {
                return false;
            }
        }

        return true;
    }
}

%typecheck(3000, fragment = "typecheck"{medAbstractData*}) medAbstractData*
{
    $1 = medAbstractData_Check($input) ? 1 : 0;
}

%typecheck(3001, fragment = "typecheck"{medAbstractData*}) dtkSmartPointer<medAbstractData>
{
    $1 = medAbstractData_Check($input) ? 1 : 0;
}

%medPythonTypemaps(medAbstractData*);

%typemap(in) medAbstractData* (QObject* temp)
{
    medPythonConvert($input, &temp);
    med::python::propagateErrorIfOccurred();
    *$1 = dynamic_cast<medAbstractData*>(temp);
}

%apply medAbstractData* { dtkSmartPointer<medAbstractData> };

%typemap(in) dtkSmartPointer<medAbstractData>
{
    $1 = ($1_ltype::ObjectType*)med::python::extractSWIGWrappedObject($input);
    med::python::propagateErrorIfOccurred();
}

%typemap(in, numinputs = 0) dtkSmartPointer<medAbstractData>* OUTPUT (dtkSmartPointer<medAbstractData> temp)
{
    $1 = &temp;
}

%typemap(argout) dtkSmartPointer<medAbstractData>* OUTPUT
{
    PyObject* output;
    medPythonConvert($1->data(), &output);
    $result = SWIG_Python_AppendOutput($result, output);
}

%include "medAbstractImageData.h"
%include "medAbstractMeshData.h"

%rename(DataManager) medDataManager;
%include "medDataManager.h"
