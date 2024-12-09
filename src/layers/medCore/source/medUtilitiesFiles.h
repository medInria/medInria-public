#pragma once
/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
 
 #include <medCoreExport.h>
 
 #include <algorithm>
 
 #include <QString>
 #include <QStringList>
 
MEDCORE_EXPORT QString computeRootPathOfListPath(QStringList &fileList, QStringList &relativePathList);
MEDCORE_EXPORT QString fileSysPathToIndex(const QString& path, QStringList files = {});
MEDCORE_EXPORT QStringList indexToFileSysPath(const QString &index);
