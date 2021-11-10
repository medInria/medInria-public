/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonConversionUtils.h"

#include "medPythonSWIGCore.h"

namespace med::python
{

const swig_type_info* getSWIGTypeInfo(QString typeName, bool raisePythonError)
{
    const swig_type_info* swigTypeInfo = SWIG_TypeQuery(qUtf8Printable(typeName));

    if (!swigTypeInfo && raisePythonError)
    {
        raiseError<TypeError>(QString("No SWIG type info found for %1").arg(typeName));
    }

    return swigTypeInfo;
}

const swig_type_info* getSWIGTypeInfo(const PyObject* object, bool raisePythonError)
{
    SwigPyObject* swigPyObject = SWIG_Python_GetSwigThis(const_cast<PyObject*>(object));
    const swig_type_info* result = nullptr;

    if (swigPyObject)
    {
        result = swigPyObject->ty;
    }
    else if (raisePythonError)
    {
        raiseError<TypeError>(QString("%1 is not a SWIG object").arg(Py_TYPE(object)->tp_name));
    }

    return result;
}

bool isSWIGWrappedObject(const PyObject* object, QString typeName)
{
    const swig_type_info* swigTypeInfo = getSWIGTypeInfo(object, false);
    bool result = swigTypeInfo;

    if (swigTypeInfo && !typeName.isEmpty())
    {
        const swig_type_info* otherSwigTypeInfo = getSWIGTypeInfo(typeName, false);

        if (otherSwigTypeInfo && (otherSwigTypeInfo == swigTypeInfo))
        {
            result = true;
        }
    }

    return result;
}

void* extractSWIGWrappedObject(const PyObject* object)
{
    const swig_type_info* swigTypeInfo = getSWIGTypeInfo(object);
    void* result = nullptr;

    if (swigTypeInfo)
    {
        int success = SWIG_Python_ConvertPtr(const_cast<PyObject*>(object), &result, const_cast<swig_type_info*>(swigTypeInfo), 0);

        if (!SWIG_IsOK(success))
        {
            QString message = QString("Cannot extract wrapped object from %1").arg(Py_TYPE(object)->tp_name);
            med::python::raiseError<med::python::TypeError>(message);
        }
    }

    return result;
}

PyObject* wrapObjectWithSWIG(const void* object, QString typeName, bool takeOwnership)
{
    if (object == nullptr)
    {
        Py_RETURN_NONE;
    }
    else
    {
        const swig_type_info* swigTypeInfo = getSWIGTypeInfo(typeName);
        PyObject* result = nullptr;

        if (swigTypeInfo)
        {
            result = SWIG_NewPointerObj(const_cast<void*>(object), const_cast<swig_type_info*>(swigTypeInfo), 0);
        }

        return result;
    }
}

PyObject* wrapObjectWithSWIG(const QObject* object, bool takeOwnership)
{
    PyObject* result = nullptr;
    const QMetaObject* metaObject =  object ? object->metaObject() : &QObject::staticMetaObject;

    do
    {
        clearError();
        result = wrapObjectWithSWIG(object, QString(metaObject->className()) + "*", takeOwnership);
        metaObject = metaObject->superClass();
    }
    while (!result && metaObject);

    return result;
}

} // namespace med::python
