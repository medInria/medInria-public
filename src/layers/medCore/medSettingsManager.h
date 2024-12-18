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

#include <medCoreExport.h>

#include <QtCore>

class medSettingsManagerPrivate;

class MEDCORE_EXPORT medSettingsManager : public QObject
{
    Q_OBJECT

public:

  static medSettingsManager * instance();

  static void destroy();

  void setValue(const QString &section, const QString &key,
                const QVariant &value, bool permanentValue = true);

  QVariant value(const QString &section, const QString &key,
                 const QVariant &defaultValue = QVariant(),
                 bool permanentValue = true);

  QStringList keys(const QString &section, bool permanentValue = true);

  void remove(const QString &section, const QString &key,
              bool permanentValue = true);

 signals:
  void settingsChanged(const QString &);

 protected:
  medSettingsManager();
  ~medSettingsManager();

 private:
  static medSettingsManager *s_instance;

  medSettingsManagerPrivate *d;
};
