/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>
#include <medCoreExport.h>

class MEDCORE_EXPORT medPlugin : public dtkPlugin
{
    Q_OBJECT

public:
    medPlugin(QObject* parent = 0);

    virtual QString name() const = 0;
    //virtual QString version() const;
    virtual QStringList authorFullNames() const = 0;
    virtual QStringList contributorFullNames() const = 0;
    //virtual QStringList dependencies() const;
    virtual QStringList tags() const = 0;
    virtual QStringList types() const = 0;

private:
    virtual QString contact() const;
    virtual QString description() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;

    QStringList sortByLastName(QStringList names) const;
    static bool nameLessThan(const QString &string1, const QString &string2);
};


