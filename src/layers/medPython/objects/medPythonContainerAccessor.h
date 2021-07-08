#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This class represents a container location mapped by a specific key. The
/// AbstractObject it implements is the contained object. It allows the use of
/// the subscript operator such as:
///
///     Object foo = bar["baz"];
///
/// or:
///
///     bar[i] += 2;
///

#include "medPythonExport.h"
#include "medPythonObject.h"

namespace med::python
{

struct ContainerAccessorPrivate;

class MEDPYTHON_EXPORT ContainerAccessor : public AbstractObject
{
public:
    ContainerAccessor(const AbstractObject& container, PyObject* key);

    using AbstractObject::operator=;

protected:
    PyObject* getReference() const override;
    void setReference(PyObject* reference) override;

private:
    ContainerAccessorPrivate* const d;
};

// The following AbstractObject template function is defined here to avoid
// circular dependency issues:

template <class ARG>
ContainerAccessor AbstractObject::operator[](const ARG& key)
{
    PyObject* pythonKey;

    if (!medPythonConvert(key, &pythonKey))
    {
        propagateCurrentError();
    }

    return ContainerAccessor(*this, pythonKey);
}

} // namespace med::python
