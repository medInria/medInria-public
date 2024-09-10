#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QString>

namespace medFirstStartCommon
{
    bool comparerFiles(const QString & path1, const QString & path2);
    bool copy(QString const & pathSource, QString const & pathDest);
};

