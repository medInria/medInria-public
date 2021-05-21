#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

// Resources are generally compiled into the executable or library binaries
// using Qt's resources system. Sometimes though it is necessary (or preferable)
// for resources to be placed in separate files. This can be done through the
// add_external_resources macro of the cmake project. External resources are
// stored differently depending on the OS, so this file provides functions to
// retrieve the external resource paths in a platform-independant way.
// NOTE: External Qt binary resources (rcc files) must also be registered using
// QResource::registerResource(path_to_resource).


#include <QString>

namespace med
{

// Returns the directory containing the external resources for the project or
// one of its libraries. A null string is returned if the directory does not
// exist.
QString getExternalResourcesDirectory(QString libraryName = QString());

// Returns the path of an external resource that was added using
// add_external_resources in the cmake project. If the resource was added
// through a library target then the library name must be provided. A null
// string is returned if the resource is not found.
QString getExternalResourcePath(QString filename, QString libraryName = QString());

} // namespace med
