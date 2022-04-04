/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "medDefaultWritingPolicy.h"
#include <medAbstractSource.h>

medDefaultWritingPolicy::medDefaultWritingPolicy()
{
}

void medDefaultWritingPolicy::setSubstitutionsCharacters(QMap<QString, QString> const & pi_substitutionMap)
{
    m_substitutionMap = pi_substitutionMap;
}

void medDefaultWritingPolicy::addSubstitutionSet(QString const & pi_toSubstitute, QString const & pi_substitution)
{
    m_substitutionMap[pi_toSubstitute] = pi_substitution;
}


QString medDefaultWritingPolicy::computeName(QString const & pi_baseName, QString pi_prefix, QString pi_suffix, QMap<QString, QString> pi_metaData)
{
    Q_UNUSED(pi_metaData);
    return pi_prefix + pi_baseName + pi_suffix;
}

QString medDefaultWritingPolicy::computePath(QString pi_sourceId, QString pi_suggestedPath, QMap<QString, QString> pi_metaData, bool pi_bTrim)
{
    QStringList pathRes = pi_suggestedPath.split("\r\n");

    int iLevelToWrite = medAbstractWritingPolicy::levelToWrite(pi_sourceId);
    
    if (iLevelToWrite < pathRes.size())
    {
        int iDeltaPathLength = pathRes.size() - iLevelToWrite;
        for (int i = 0; i < iDeltaPathLength; ++i)
        {
            if (pi_bTrim)
            {
                pathRes.pop_back();
            }
            else
            {
                QString last = pathRes.takeLast();
                int lastPos = pathRes.size() - 1;
                pathRes[lastPos] = pathRes[lastPos] + '_' + last;
            }            
        }
    }
    else if (iLevelToWrite > pathRes.size())
    {
        int iBazeSize = pathRes.size();
        int iDeltaPathLength = iLevelToWrite - pathRes.size();
        QString levelName("Level_");
        for (int i = 0; i < iDeltaPathLength; ++i)
        {
            pathRes.push_back(levelName + QString::number(i + iBazeSize));
        }
    }

    charactersSubstitution(pathRes, &pathRes);

    return pathRes.join("\r\n");
}


bool medDefaultWritingPolicy::incrementName(QString & pio_name, QStringList pi_alreadyExistName)
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

bool medDefaultWritingPolicy::checkUri(QStringList pi_suggestedUri, QStringList * pio_rectifiedUri)
{
    bool bRes = false;

    if (pi_suggestedUri.size() > 1)
    {
        QString sourceInstanceId = pi_suggestedUri[0];
        if (sourceInstanceId.length())
        {
            int iDesiredWritableLevel = levelToWrite(sourceInstanceId);

            if (iDesiredWritableLevel != -1)
            {
                if (pio_rectifiedUri)
                {
                    pio_rectifiedUri->clear();
                }
                
                int uriLength = pi_suggestedUri.size();
                if (islevelWritable(sourceInstanceId, pi_suggestedUri.size()))
                { 
                    bRes = true;
                }
                else
                {
                    if (pio_rectifiedUri)
                    {
                        (*pio_rectifiedUri) = pi_suggestedUri;
                        if (iDesiredWritableLevel > uriLength)
                        {
                            //Create intermediates levels when suggested URI is too short                            
                            for (int i = uriLength; i < iDesiredWritableLevel; ++i)
                            {
                                QString levelNameTmp = "level_" + QString::number(i);
                                pio_rectifiedUri->insert(i, levelNameTmp);
                            }
                        }
                        else
                        {
                            //Remove intermediates levels (by right concatenation of supplementaries levels) when suggested URI is too long
                            QString dataNameTmp = pio_rectifiedUri->takeLast();
                            while (pio_rectifiedUri->size() >= iDesiredWritableLevel)
                            {
                                dataNameTmp = pio_rectifiedUri->takeLast() + "_" + dataNameTmp;
                            }
                            pio_rectifiedUri->push_back(dataNameTmp);                            
                        }
                    }
                }

                if (bRes || pio_rectifiedUri != nullptr)
                {
                    bRes &= charactersSubstitution(pi_suggestedUri, pio_rectifiedUri);
                }
            }
        }
    }

    return bRes;
}


bool medDefaultWritingPolicy::charactersSubstitution(QStringList pi_suggestedUri, QStringList * pio_rectifiedUri)
{
    bool bRes = true;

    if (pio_rectifiedUri)
    {
        pio_rectifiedUri->clear();
    }

    auto avoidCharaters = m_substitutionMap.keys();
    for (auto const uriEntry : pi_suggestedUri)
    {
        auto uriEntryTmp = uriEntry;
        for (auto const & avoidCharater : avoidCharaters)
        {
            uriEntryTmp.replace(avoidCharater, m_substitutionMap[avoidCharater]);
        }

        bRes = bRes && uriEntryTmp == uriEntry;

        if (pio_rectifiedUri)
        {
            pio_rectifiedUri->push_back(uriEntryTmp);
        }
    }

    return bRes;
}
