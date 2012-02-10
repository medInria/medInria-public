#ifndef medPacsDataSource_h__
#define medPacsDataSource_h__

#include <medAbstractDataSource.h>
#include <medMoveCommandItem.h>

class medPacsDataSourcePrivate;
class medToolBox;

/**
 * @class Pacs connection datasource that comes with treeview,
 * searchpanel and source selector (DICOM-nodes)
 */
class medPacsDataSource : public medAbstractDataSource
{
    Q_OBJECT
public:

    medPacsDataSource(QWidget* parent = 0);
    ~medPacsDataSource();

    QWidget* mainViewWidget();

    QWidget* sourceSelectorWidget();

    QString tabName();

    QList<medToolBox*> getToolboxes();
	
	QString description(void) const;

private slots:
    void onPacsMove( const QVector<medMoveCommandItem>& cmdList);

private:
    medPacsDataSourcePrivate* d;

};

#endif // medPacsDataSource_h__
