#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// The embedded core Python library is loaded and explicitly linked at runtime.
///

namespace med::python
{

bool setupCoreLibrary();
bool teardownCoreLibrary();

} // namespace med::python
