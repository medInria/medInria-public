#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This class represents a an object's attribute. The AbstractObject it
/// implements is the attribute value.
///

#include "medPythonExport.h"
#include "medPythonObject.h"

namespace med::python
{

struct AttributeAccessorPrivate;

class MEDPYTHON_EXPORT AttributeAccessor : public AbstractObject
{
public:
    AttributeAccessor(const AbstractObject& object, QString attribute);
    AttributeAccessor(const Object& object, QString attribute);

    using AbstractObject::operator=;

protected:
    PyObject* getReference() const override;
    void setReference(PyObject* reference) override;

private:
    AttributeAccessorPrivate* const d;
};

} // namespace med::python
