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

    QString description(void) const;

signals:

    /**
    * Signal emitted when the user chooses to load
    * a file or directory.
    * @param path - the path of the image or directory
    **/
    void load(QString path);

    /**
    * Signal emitted when the user chooses to open
    * a file or directory.
    * @param path - the path of the image or directory
    **/
    void open(QString path);

private slots:
    void onFileSystemImportRequested();
    void onFileSystemIndexRequested();
    void onFileSystemLoadRequested();
    void onFileSystemViewRequested();
    void onFileDoubleClicked(const QString& filename);
    void onFileClicked(const QFileInfo& info);
    void onNothingSelected(void);

private:

    /** This function takes a list of paths as an input and creates
     * another list by removing the paths that are subpaths of others. */
    QStringList removeNestedPaths(const QStringList& paths);

private:
    medFileSystemDataSourcePrivate* d;

    QString formatByteSize(qint64 bytes);
};

#endif // medFileSystemDataSource_h__
