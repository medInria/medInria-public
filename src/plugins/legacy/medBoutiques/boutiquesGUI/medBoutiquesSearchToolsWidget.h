#ifndef MEDBOUTIQUESSEARCHTOOLSWIDGET_H
#define MEDBOUTIQUESSEARCHTOOLSWIDGET_H

#include <vector>
#include <QWidget>
#include <QProcess>
#include <QJsonObject>
#include <QJsonArray>
#include "medBoutiquesConfiguration.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLineEdit;
class QGroupBox;
class QLabel;
class QTableWidget;
class QTreeView;
class QStandardItemModel;
class QSortFilterProxyModel;
class QTextEdit;
QT_END_NAMESPACE

using namespace std;

struct ToolDescription {
    QString id;
    QString title;
    QString description;
    int nDownloads;
};

class medBoutiquesSearchToolsWidget : public QWidget
{
    Q_OBJECT

private:
    QLineEdit* searchLineEdit;
    QPushButton* button;
    QGroupBox* searchGroupBox;
    QLabel* loadingLabel;
    QTableWidget* table;
    QStandardItemModel *toolModel;
    QSortFilterProxyModel *proxyToolModel;
    QTreeView *searchView;
    QLabel* infoLabel;
    QTextEdit* info;
    QProcess* searchProcess;
    QProcess* pprintProcess;
    QProcess* toolDatabaseProcess;
    vector<ToolDescription> searchResults;
    vector<ToolDescription> allTools;
    QJsonArray descriptors;
    bool toolDatabaseUpdated;
    bool ignorePPrintError;

public:
    explicit medBoutiquesSearchToolsWidget(QWidget *parent = nullptr);

    ToolDescription *getSelectedTool();

private:
    void createSearchView();
    void createProcesses();
    void downloadToolDatabase();
    void loadToolDatabase();
    QStringList readSearchResults(QString &output);
    void parseSearchResults(const QStringList &lines, vector<ToolDescription> &searchResults);
    void addSearchResult(const ToolDescription &toolDescription, const unsigned int toolDescriptionIndex);
    void displaySearchResults();

signals:
    void toolSelected();
    void toolDeselected();

public slots:
    void selectionChanged();
    void searchBoutiquesTools();
    void errorOccurred(QProcess::ProcessError error);
    void searchProcessStarted();
    void searchProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void pprintProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void createToolDatabase(int exitCode, QProcess::ExitStatus exitStatus);
    void pullProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
protected slots:
    void searchChanged();
};

#endif // MEDBOUTIQUESSEARCHTOOLSWIDGET_H
