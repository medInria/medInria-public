#ifndef medDatabaseDataSource_h__
#define medDatabaseDataSource_h__

#include <medCore/medAbstractDataSource.h>
#include <medCore/medDataIndex.h>

class medDatabaseDataSourcePrivate;

/**
 * Not a classical data source per se, as it does not import data
 * Still it fits to the design of data sources
 */
class medDatabaseDataSource : public medAbstractDataSource
{
    Q_OBJECT

public:
    medDatabaseDataSource(QWidget* parent = 0);
    ~medDatabaseDataSource();

    QWidget* mainViewWidget();

    QWidget* sourceSelectorWidget();

    QString tabName();

    QList<medToolBox*> getToolboxes();

    void update();

signals:
    void open(const medDataIndex&);

protected slots:
    void onFilter(const QString &text, int column);


private:
    medDatabaseDataSourcePrivate* d;

};

#endif // medDatabaseDataSource_h__

