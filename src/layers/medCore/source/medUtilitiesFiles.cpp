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

#include "medUtilitiesFiles.h"

int findFirstDifference(const QString& str1, const QString& str2)
{
    // Iterate through the shorter of the two strings
    for (int i = 0; i < std::min(str1.size(), str2.size()); ++i)
    {
        if (str1[i] != str2[i])
        {
            return i;
        }
    }

    // If no difference is found within the shorter string's length
    // the longer string has extra characters at the end
    if (str1.size() != str2.size())
    {
        return std::min(str1.size(), str2.size());
    }

    // Strings are equal
    return std::min(str1.size(), str2.size());
}

QString computeRootPathOfListPath(QStringList &fileList, QStringList &relativePathList)
{
    QString rootPath = fileList[0];

    int x = 0;
    if(!fileList.isEmpty())
    {
        for (int i = 1; i < fileList.size(); i++)
        {
            x = findFirstDifference(rootPath, fileList[i]);
            rootPath = rootPath.left(x);
        }

        if (rootPath[rootPath.size() - 1] != '/')
        {
            x = rootPath.lastIndexOf('/') + 1;
            rootPath = rootPath.left(x);
        }

        for (auto aFilePath : fileList)
        {
            relativePathList << aFilePath.right(aFilePath.size() - x);
        }
    }

    return  rootPath;
}

QString fileSysPathToIndex(const QString &path, QStringList files)
{
    QString pathTmp = path;
    pathTmp.replace('\\', '/');
    pathTmp.replace('/', "\r\n");
    pathTmp = "fs:" + pathTmp;

    if (!files.isEmpty())
    {
        if (!pathTmp.endsWith("\r\n"))
        {
            pathTmp += "\r\n";
        }
        for (QString fileName : files)
        {
            pathTmp += fileName + "|";
        }
        if (pathTmp.endsWith("|"))
        {
            pathTmp = pathTmp.left(pathTmp.size() - 1);
        }
    }
    
    return pathTmp;
}

QStringList indexToFileSysPath(const QString &index)
{
    QStringList pathsRes;
    QString basePath;

    QString uri = index;
    if (uri.startsWith("fs:"))
    {
        int sourceDelimterIndex = uri.indexOf(QString(":"));

        QStringList uriAsList = uri.right(uri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));
        QString lastElement = uriAsList.takeLast();
        QStringList files = lastElement.split('|',QString::SkipEmptyParts);
        basePath = uriAsList.join('/');
        for (QString fileName : files)
        {
            pathsRes << basePath + "/" + fileName;
        }
    }

    return pathsRes;
}