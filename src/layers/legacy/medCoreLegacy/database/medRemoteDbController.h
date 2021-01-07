/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>

#include <medSqlDbController.h>
#include <medCoreLegacyExport.h>

/**
 * Specialization of Concrete dbController implementation which allow to connect to remote postgres database
 */
class MEDCORELEGACY_EXPORT medRemoteDbController : public medSqlDbController
{
    Q_OBJECT

public:
    static medRemoteDbController *instance();

    bool createConnection();
    bool closeConnection() override;

    QList<medDataIndex> patients() const override;
    QList<QList<QVariant>> requestDatabaseForModel() const;

protected:
    medRemoteDbController();

private:
    static medRemoteDbController *s_instance;
};
