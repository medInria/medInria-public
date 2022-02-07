#include <medDataModelItem.h>
#include <medSourceItemModel.h>

#include <QVariant>
#include <QList>




QString uriListToString(QStringList const & pi_uri)
{
    QString uriRes;

    int size = pi_uri.size();
    if (size > 0)
    {
        int i;
        uriRes += pi_uri[0] + ":";
        for (i = 1; (i + 1) < size; ++i)
        {
            uriRes += pi_uri[i] + "\r\n";
        }
        uriRes += pi_uri[i];
    }
    return uriRes;
}



QStringList uriStringToList(QString const & pi_uri)
{
    QStringList uriRes;

    int sourceDelimterIndex = pi_uri.indexOf(QString(":"));
    uriRes = pi_uri.right(pi_uri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));
    uriRes.push_front(pi_uri.left(sourceDelimterIndex));

    return uriRes;
}
