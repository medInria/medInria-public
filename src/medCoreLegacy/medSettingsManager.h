/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <medCoreExport.h>

class medSettingsManagerPrivate;



class MEDCORE_EXPORT medSettingsManager : public QObject
{
    Q_OBJECT

public:
    static medSettingsManager *instance();

    static void destroy();

    void setValue( const QString & section, const QString & key, const QVariant & value );

    QVariant value ( const QString & section, const QString & key, const QVariant & defaultValue = QVariant() );
    QStringList keys ( const QString & section);

    void remove (const QString & section, const QString & key);

signals:
    void settingsChanged( const QString & );

protected:
    medSettingsManager();
    ~medSettingsManager();

private:
    static medSettingsManager *s_instance;

    medSettingsManagerPrivate *d;
};

