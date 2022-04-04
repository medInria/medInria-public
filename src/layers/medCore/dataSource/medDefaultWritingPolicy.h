#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWritingPolicy.h>

class medDefaultWritingPolicy : public medAbstractWritingPolicy
{
public:
    medDefaultWritingPolicy();
    inline ~medDefaultWritingPolicy() override = default;

    // inherits from medAbstractWritingPolicy	
	void setSubstitutionsCharacters(QMap<QString, QString> const &pi_substitutionMap) override;
	void addSubstitutionSet(QString const & pi_toSubstitute, QString const & pi_substitution) override;

    QString computeName(QString const & pi_baseName, QString pi_prefix, QString pi_suffix, QMap<QString, QString> pi_metaData) override;
    QString computePath(QString pi_sourceId, QString pi_suggestedPath, QMap<QString, QString> pi_metaData, bool pi_bTrim = true) override;

    bool incrementName(QString &pio_name, QStringList pi_alreadyExistName) override;
	bool checkUri(QStringList pi_suggestedUri, QStringList * pio_rectifiedUri = nullptr) override;

private:
    bool charactersSubstitution(QStringList pi_suggestedUri, QStringList * pio_rectifiedUri);

private:
    QMap<QString, QString> m_substitutionMap;
};
