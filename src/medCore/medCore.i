/* medCore.i --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sat Oct  3 18:27:33 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Mar  5 09:01:25 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDCORE_I
#define MEDCORE_I

%module medcore
%{
#include <QtDebug>
#include <QtCore>
#include <dtkCore/dtkAbstractData.h>

#undef Q_OBJECT
#undef signals
#undef slots

#define Q_OBJECT
#define signals public
#define slots

#include <medDataIndex.h>
#include <medDataManager.h>
%}

// /////////////////////////////////////////////////////////////////
// Preprocessing setup
// /////////////////////////////////////////////////////////////////

#pragma SWIG nowarn=362, 378, 401, 503, 509, 801, 472

// /////////////////////////////////////////////////////////////////
// Macro undefinition
// /////////////////////////////////////////////////////////////////

#undef Q_OBJECT
#undef signals
#undef slots

#define Q_OBJECT
#define signals public
#define slots

#undef  Q_PROPERTY(Type type MODE mode)
#define Q_PROPERTY(Type type MODE mode)

#undef  Q_DECLARE_INTERFACE(IFace, IId)
#define Q_DECLARE_INTERFACE(IFace, IId)

#undef  Q_DECLARE_METATYPE(TYPE)
#define Q_DECLARE_METATYPE(TYPE)

#undef  MEDCORE_EXPORT
#define MEDCORE_EXPORT

// /////////////////////////////////////////////////////////////////
// Ignore rules for operators
// /////////////////////////////////////////////////////////////////

%ignore operator<<;
%ignore operator==;

// /////////////////////////////////////////////////////////////////
// Typemaps
// /////////////////////////////////////////////////////////////////

#ifdef SWIGPYTHON

%typecheck(SWIG_TYPECHECK_STRING) char * {
  $1 = PyString_Check($input) ? 1 : 0;
}

%typemap(typecheck) QString = char *;

%typemap(in) QString { // Python -> C++
    if (PyString_Check($input)) {
         $1 = QString(PyString_AsString($input));
     } else {
         qDebug("QString expected");
     }
}

%typemap(out) QString { // C++ -> Python
    $result = PyString_FromString($1.toAscii().constData());
}
#endif

// /////////////////////////////////////////////////////////////////
// Wrapper input
// /////////////////////////////////////////////////////////////////

%ignore medDataManager::dataAdded(const medDataIndex&);
%ignore medDataManager::dataRemoved(const medDataIndex&);

%include "medDataIndex.h"
%include "medDataManager.h"

#endif
