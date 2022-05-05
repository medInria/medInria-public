#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

// These functions allow retrieval of external resources (see the
// 'set_external_resources' and 'import_external_resources' cmake commands to
// see how resources are added).
// NOTE: External Qt binary resources (rcc files) must also be registered using
// QResource::registerResource(path_to_resource).


#include <QString>
#include <medCoreLegacyExport.h>

namespace med
{

// Returns the directory containing the external resources for the project or
// one of its libraries. A null string is returned if the directory does not
// exist.
MEDCORELEGACY_EXPORT QString getExternalResourcesDirectory(QString subdirectory = QString(), QString libraryName = QString());

// Returns the path of an external resource. If the resource was imported into a
// library target then the library name must be provided. A null string is
// returned if the resource is not found.
MEDCORELEGACY_EXPORT QString getExternalResourcePath(QString filepath, QString libraryName = QString());

} // namespace med
