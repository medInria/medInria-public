/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFirstStartCommon.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

/**
 * @brief Compares two files to see if their content is identical.
 *
 * @param path1 Path to the first file.
 * @param path2 Path to the second file.
 * @return True if the files have the same content, false otherwise.
 */
bool medFirstStartCommon::compareFiles(const QString & path1, const QString & path2)
{
    // Open both files
    QFile file1(path1);
    QFile file2(path2);

    if (!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly))
    {
        return false; // If file opening failed, files are files are considered different
    }

    // Compare both files block by block
    QByteArray block1, block2;
    block1 = file1.readAll();
    block2 = file2.readAll();

    return block1 == block2;
}


/**
 * @brief Copies a file from source to destination.
 *
 * @param pathSource Path to the source file.
 * @param pathDest Path to the destination file.
 * @return True if the copy was successful, false otherwise.
 */
bool medFirstStartCommon::copy(QString const & pathSource, QString const & pathDest)
{
    bool bRes = true;

    QFileInfo fiSource(pathSource);
    if (fiSource.isFile())
    {
        QDir destDir = QFileInfo(pathDest).dir();
        destDir.mkpath(destDir.absolutePath());
        QFile::remove(pathDest);
        bRes = QFile::copy(pathSource, pathDest);
    }
    else if (fiSource.isDir())
    {
        QDirIterator it(pathSource, QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        QDir dir(pathSource);
        const int srcPathLength = dir.canonicalPath().length();

        while (it.hasNext())
        {
            it.next();

            const auto fi = it.fileInfo();
            const QString relPath = fi.canonicalPath().mid(srcPathLength);
            const QString absDstPath = pathDest + relPath;

            if (fi.isFile())
            {
                QFile::remove(absDstPath);
                bRes = bRes && QFile::copy(fi.canonicalPath(), absDstPath);
            }
            else if (fi.isDir())
            {
                bRes = bRes && dir.mkpath(absDstPath);
            }
        }
    }
    else
    {
        bRes = false;
    }

    return bRes;
}
