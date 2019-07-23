#ifndef MEDBOUTIQUESSEARCHTOOLS_H
#define MEDBOUTIQUESSEARCHTOOLS_H

#include <vector>
#include <QWidget>
#include <QProcess>

#define BOSH_PATH "../bosh"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLineEdit;
class QGroupBox;
class QLabel;
class QTableWidget;
class QTextEdit;
QT_END_NAMESPACE

using namespace std;

struct SearchResult {
    string id;
    string title;
    string description;
    int downloads;
};

class medBoutiquesSearchTools : public QWidget
{
    Q_OBJECT
public:
    explicit medBoutiquesSearchTools(QWidget *parent = nullptr);

    SearchResult *getSelectedTool();

private:
    void createTable();
    void createProcesses();

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

private:
    QLineEdit* searchLineEdit;
    QPushButton* button;
    QGroupBox* searchGroupBox;
    QLabel* loadingLabel;
    QTableWidget* table;
    QLabel* infoLabel;
    QTextEdit* info;
    QProcess* searchProcess;
    QProcess* pprintProcess;
    std::vector<SearchResult> searchResults;
};

#endif // MEDBOUTIQUESSEARCHTOOLS_H
