#include <QtSql>
#include <dtkCore/dtkGlobal.h>

namespace medSqlHelpers
{

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

}
