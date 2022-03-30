#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

#include <medCoreExport.h>


struct MEDCORE_EXPORT medWritingPolicyData
{
public:
    QString                baseName;
    //QStringList            relativeDirDst;
    QStringList            relativeDirDstPath;
    QStringList            relativeDirDstUri;
    //QList< QPair<QString, QString> > relativeDirDst; //List<Pair<path elem, uri elem>>
    QMap<QString, QString> relativeDirDst;
    QString                prefix;
    QString                suffix;
    QMap<QString, QString> metaData;
};

class MEDCORE_EXPORT medAbstractWritingPolicy 
{
public:
    inline medAbstractWritingPolicy() = default;
	inline virtual ~medAbstractWritingPolicy() = default;
	
	virtual void setSubstitutionsCharacters(QMap<QString, QString> const &pi_substitutionMap) = 0;
	virtual void addSubstitutionSet(QString const & pi_toSubstitute, QString const & pi_substitution) = 0;
    
    virtual QString computeName(QString const & pi_baseName, QString pi_prefix, QString pi_suffix, QMap<QString, QString> pi_metaData) = 0;
    virtual QString computePath(QString pi_sourceId, QString pi_suggestedPath, QMap<QString, QString> pi_metaData, bool pi_bTrim = true) = 0;

    //virtual QStringList computeRelativePathDst(QString const & pi_baseName, QStringList pi_relativeDirDst) = 0;
    //virtual QStringList computeRelativePathDst(QString const & pi_baseName, QStringList pi_relativeDirDst, QString pi_prefix, QString pi_suffix, QMap<QString, QString> pi_metaData) = 0;
    //virtual QStringList computeHumanUri(QStringList const & pi_relativePathDst, QStringList pi_basePathDst) = 0;
    //virtual QStringList computeHumanUri(QStringList const & pi_relativePathDst, QStringList pi_basePathDst, QMap<QString, QString> pi_metaData ) = 0;

    virtual bool incrementName(QString &pio_name, QStringList pi_alreadyExistName) = 0;

	virtual bool checkUri(QStringList pi_suggestedUri, QStringList * pio_rectifiedUri = nullptr) = 0;


    static int levelToWrite(QString pi_sourceId);
    static bool islevelWritable(QString pi_sourceId, unsigned int pi_uiLevel);
    static QStringList basePathDstFromOrigin(QString sourceDstId, QStringList pi_originPath);
};

