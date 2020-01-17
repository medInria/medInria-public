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

// The data structure to store search results
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
    QStandardItemModel *toolModel;              // Tool model containing all tools
    QSortFilterProxyModel *proxyToolModel;      // Tool model used to filter tools according to the user search input
                                                // (automatically filters the items show in the search view)
    QTreeView *searchView;
    QLabel* infoLabel;
    QLabel* infoShortDescription;
    QPushButton* moreInfoButton;
    QTextEdit* info;
    QProcess* searchProcess;
    QProcess* pprintProcess;
    QProcess* toolDatabaseProcess;
    vector<ToolDescription> allTools;           // All available tools in Boutiques (from the files in ~/.cache/boutiques)
    vector<ToolDescription> searchResults;      // Tools from the search results if the entire database all-descriptors.json is not loaded, otherwise equals allTools
                                                // (necessary to avoid overwriting search results when the database finishes loading in the background)
    QJsonObject descriptors;                     // All descriptors from all tools (read from "all-descriptors.json")
    bool toolDatabaseUpdated;                   // True when the tool database was just updated and toolModel must be updated
    bool ignorePPrintError;                     // Ignore the pprint (to display the tool help) error when killing the pprint process
                                                // (when selecting a new tool while the previous pprint was not terminated)

public:
    explicit medBoutiquesSearchToolsWidget(QWidget *parent = nullptr);

    // Get selected tool from toolModel
    ToolDescription *getSelectedTool();
    // Get selected tool descriptor
    // Either from the database "~/.cache/boutiques/all-descriptors.json" or, if not found in the databse, from the file "~/.cache/boutiques/zenodo-TOOL_ID.json"
    const QJsonObject getSelectedToolDescriptor();

private:
    void createSearchView();
    void createProcesses();
    void downloadToolDatabase();
    void loadToolDatabase();
    QStringList readSearchResults(QString &output);
    void parseSearchResults(const QStringList &lines, vector<ToolDescription> &searchResults);
    void addSearchResult(const ToolDescription &toolDescription, const unsigned int toolDescriptionIndex);
    void displaySearchResults();
    void prettyPrint();
    // Get selected tool descriptor from tool id
    const QJsonObject getToolDescriptor(const QString &toolId);

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
    void toggleMoreInfo();
};

#endif // MEDBOUTIQUESSEARCHTOOLSWIDGET_H
