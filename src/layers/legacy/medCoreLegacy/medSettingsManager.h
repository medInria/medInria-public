#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>

#include <medCoreLegacyExport.h>

#include <memory>

class medSettingsManagerPrivate;

class MEDCORELEGACY_EXPORT medSettingsManager : public QObject
{
    Q_OBJECT

public:
    ~medSettingsManager();

    static medSettingsManager &instance();

    void setValue( const QString & section, const QString & key, const QVariant & value );

    QVariant value ( const QString & section, const QString & key, const QVariant & defaultValue = QVariant() );
    QStringList keys ( const QString & section);

    void remove (const QString & section, const QString & key);

signals:
    void settingsChanged( const QString & );

private:
    medSettingsManager();
    
    static std::unique_ptr<medSettingsManager> s_instance;

    medSettingsManagerPrivate *d;
};
