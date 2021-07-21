/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonAttributeAccessor.h"

#include "medPythonCoreFunction.h"

namespace med::python
{

struct AttributeAccessorPrivate
{
    Object object;
    QString attribute;
};

AttributeAccessor::AttributeAccessor(const AbstractObject& object, QString attribute) :
    d(new AttributeAccessorPrivate)
{
    d->object = object;
    d->attribute = attribute;
}

AttributeAccessor::AttributeAccessor(const Object& object, QString attribute) :
    d(new AttributeAccessorPrivate)
{
    d->object = object;
    d->attribute = attribute;
}

PyObject* AttributeAccessor::getReference() const
{
    return coreFunction(PyObject_GetAttrString, *d->object, qUtf8Printable(d->attribute));
}

void AttributeAccessor::setReference(PyObject* reference)
{
    coreFunction(PyObject_SetAttrString, *d->object, qUtf8Printable(d->attribute), reference);
}

} // namespace med::python
