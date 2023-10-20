/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "medSQLiteWritingPolicy.h"

medSQLiteWritingPolicy::medSQLiteWritingPolicy()
{
}

void medSQLiteWritingPolicy::setSubstitutionsCharacters(QMap<QString, QString> const & pi_substitutionMap)
{
    m_substitutionMap = QMap<QString, QString>();
}

void medSQLiteWritingPolicy::addSubstitutionSet(QString const & pi_toSubstitute, QString const & pi_substitution)
{
    m_substitutionMap[pi_toSubstitute] = pi_substitution;
}


QString medSQLiteWritingPolicy::computeName(QString const & pi_baseName, QString pi_prefix, QString pi_suffix, QMap<QString, QString> pi_metaData)
{
    Q_UNUSED(pi_metaData);
    return pi_prefix + pi_baseName + pi_suffix;
}

QString medSQLiteWritingPolicy::computePath(QString pi_sourceId, QString pi_suggestedPath, QMap<QString, QString> pi_metaData, bool pi_bTrim)
{
    QStringList pathRes = pi_suggestedPath.split("\r\n", QString::SkipEmptyParts);

    int iLevelToWriteData = 2; // In case of SQLite source, the desired level to write data is 2
    QStringList levelNames;
    levelNames << "John Doe"
               << "Study";

    if (iLevelToWriteData < pathRes.size())
    {
        int iDeltaPathLength = pathRes.size() - iLevelToWriteData;
        for (int i = 0; i < iDeltaPathLength; ++i)
        {
            // In SQLite we always trim the path
            pathRes.pop_back();
        }
    }
    else if (iLevelToWriteData > pathRes.size())
    {
        int iBazeSize = pathRes.size();
        int iDeltaPathLength = iLevelToWriteData - pathRes.size();
        for (int i = 0; i < iDeltaPathLength; ++i)
        {
            pathRes.push_back(levelNames[iBazeSize+i]);
        }
    }

    return pathRes.join("\r\n");
}


bool medSQLiteWritingPolicy::incrementName(QString & pio_name, QStringList pi_alreadyExistName)
{
    bool bRes = true;

    int iCounter = pi_alreadyExistName.contains(pio_name) ? 1 : 0;
    QString leftPart = pio_name;

    QString regExpStr = "(.*)_([0-9]+)";
    QRegExp regExp(regExpStr);


    if (regExp.exactMatch(pio_name))
    {
        leftPart = regExp.cap(1);
        iCounter = regExp.cap(2).toInt();
    }

    regExpStr = "(" + QRegExp::escape(leftPart) + ")_([0-9]+)";
    regExp.setPattern(regExpStr);

    for (auto aName : pi_alreadyExistName)
    {
        if (regExp.exactMatch(aName))
        {
            int iTmp = regExp.cap(2).toInt();
            iCounter = iTmp >= iCounter ? iTmp+1 : iCounter;
        }
    }

    if (iCounter > 0)
    {
        pio_name = leftPart + "_" + QString::number(iCounter);
    }    

    return bRes;
}

bool medSQLiteWritingPolicy::checkUri(QStringList pi_suggestedUri, QStringList * pio_rectifiedUri)
{
    bool bRes = false;

    return bRes;
}
