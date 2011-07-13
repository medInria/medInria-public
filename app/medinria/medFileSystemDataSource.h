#ifndef medFileSystemDataSource_h__
#define medFileSystemDataSource_h__

#include <medAbstractDataSource.h>

class medFileSystemDataSourcePrivate;

/**
 * File-system explorer working as a concrete data source
 * Let's the user pick folders or data and import or view it
 */
class medFileSystemDataSource : public medAbstractDataSource
{
    Q_OBJECT

public:
    medFileSystemDataSource(QWidget* parent = 0);
    ~medFileSystemDataSource();

    QWidget* mainViewWidget();

    QWidget* sourceSelectorWidget();

    QString tabName();

    QList<medToolBox*> getToolboxes();

signals:
    void open(QString);

private slots:
    void onFileSystemImportClicked();
    void onFileSystemViewClicked();


private:
    medFileSystemDataSourcePrivate* d;
};

#endif // medFileSystemDataSource_h__
