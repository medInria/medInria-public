/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtSql>
#include <dtkCore/dtkGlobal.h>

namespace medSqlHelpers
{

/** Returns all the ids of the series whose name is the one given by the parameter. */
QList<int> getSeriesIdsWithName(QSqlDatabase db, QString seriesName)
{
    QList<int> seriesIds;

    QSqlQuery query(db);

    query.prepare("SELECT id FROM series WHERE name = :value");
    query.bindValue(":value", seriesName);

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return seriesIds;
    }

    while (query.next())
    {
        seriesIds.append(query.value(0).toInt());
    }

    return seriesIds;
}

/** Returns a string representation of a list of integers, so as we can embed it in a sql query. */
QString listToStringRepresentation(QList<int> intList)
{
    // we know we have at least one element
    QString stringRep = "(" + QString::number(intList[0]);

    for (int i = 1; i < intList.size(); ++i)
        stringRep += ", " + QString::number(intList.at(i));

    stringRep += ")";

    return stringRep;
}

/** For each given series id in the input parameter 'seriesIds', this function
 * will return, inside the output list, an integer indicating how many rows
 * in the image table are associated with that given id. */
QList<int> getImagesCountGroupedBySeriesIds(QSqlDatabase db, QList<int> seriesIds)
{
    QList<int> imagesCount;

    if(seriesIds.isEmpty())
        return imagesCount;

    QString seriesIdsString = listToStringRepresentation(seriesIds);

    QSqlQuery query(db);

    query.prepare("SELECT count(id) FROM image WHERE series in " + seriesIdsString + " GROUP BY series");

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return imagesCount;
    }

    while (query.next())
    {
        imagesCount.append(query.value(0).toInt());
    }

    return imagesCount;
}

/** Returns the amount of rows in the given table. */
int countRowsInTable(QSqlDatabase db, QString tableName)
{
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM " + tableName);

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return -1;
    }

    if (query.first())
    {
        QVariant rowsCount = query.value(0);
        return rowsCount.toInt();
    }
    else
        return -1;
}

/** Returns true if there is at least one row in table 'tableName' whose column 'columnName' is 'value', false otherwise. */
bool checkIfRowExists(QSqlDatabase db, QString tableName, QString columnName, QString value)
{
    QSqlQuery query(db);

    query.prepare("SELECT id FROM " + tableName + " WHERE " + columnName + " = :value");
    query.bindValue(":value", value);

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return false;
    }

    if (query.first())
    {
        QVariant id = query.value(0);
        return true;
    }
    else
        return false;
}

/** Returns true if there is at least one row in table 'tableName' whose pairs column/value
 * matches the ones in the given input map */
bool checkIfRowExists(QSqlDatabase db, QString tableName, QHash<QString, QString> columnValues)
{
    // TODO this way of building the query string is just awful
    // try to find another...
    QString queryString = "SELECT id FROM " + tableName + " WHERE ";

    QHashIterator<QString, QString> it(columnValues);

    bool firstTime = true;
    while (it.hasNext())
    {
        it.next();
        if (!firstTime)
        {
            queryString.append(" AND ");
        }
        else
            firstTime = false;

        queryString.append(it.key()).append(" = :").append(it.key());
    }

    QSqlQuery query(db);

    query.prepare(queryString);

    it.toFront();

    while (it.hasNext())
    {
        it.next();
        query.bindValue(":" + it.key(), it.value());
    }

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return false;
    }

    // see executed query
    //qDebug() << query.executedQuery().toStdString().c_str();
    if (query.first())
    {
        QVariant id = query.value(0);
        return true;
    }
    else
        return false;
}

/** Returns first patient id with given name. */
QString getPatientId(QSqlDatabase db, QString patientName)
{
    QSqlQuery query(db);

    query.prepare("SELECT patientId FROM patient WHERE name = :value");
    query.bindValue(":value", patientName);

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return NULL;
    }

    if (query.first())
    {
        QVariant id = query.value(0);
        return id.toString();
    }
    else
        return NULL;
}

/** Returns first series id with given name. */
QString getSeriesId(QSqlDatabase db, QString seriesName)
{
    QSqlQuery query(db);

    query.prepare("SELECT seriesId FROM series WHERE name = :value");
    query.bindValue(":value", seriesName);

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return NULL;
    }

    if (query.first())
    {
        QVariant id = query.value(0);
        return id.toString();
    }
    else
        return NULL;
}

/** Returns the relative path of the series whose name is 'seriesName'.
 * The value is obtained by looking at the thumbnail path. */
QString getSeriesRelativePathFromThumbnail(QSqlDatabase db, QString seriesName)
{
    QSqlQuery query(db);

    query.prepare("SELECT thumbnail FROM series WHERE name = :value");
    query.bindValue(":value", seriesName);

    if (!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return NULL;
    }

    if (query.first())
    {
        QString id = query.value(0).toString();
        QFileInfo file(id);
        return file.dir().path();
    }
    else
        return NULL;
}

}
