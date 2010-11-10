#ifndef SimpleView_H
#define SimpleView_H

// includes
#include <QMainWindow>
#include <iostream>
#include <vector>
#include "dcmtkNode.h"

// Forward Qt class declarations
class Ui_SimpleView;
class QTreeWidgetItem;

class dcmtkEchoScu;
class dcmtkFindDataset;
class dcmtkFindScu;
class dcmtkMoveScu;
class dcmtkStoreScp;
class dcmtkStoreScu;

class LoggerConsoleOutput;
class LoggerFileOutput;
class LoggerWidgetOutput;
class LoggerWidget;

class QProgressDialog;

class SimpleView : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  SimpleView(); 
  ~SimpleView();

protected:

  struct dcmtkConnectionData
  {
      std::string title;
      std::string ip;
      int         port;
  };
   
protected slots:

  void findStudyLevel();
  void findSeriesLevel(QTreeWidgetItem* item);
  void findImageLevel(QTreeWidgetItem* item);
  void move(QTreeWidgetItem * item, int column);
  void store();
  void echo();
  void setSendDirectory();
  void applySettingsSlot();
  void fileAppender(int state);
  void shellAppender(int state);
  void widgetAppender(int state);
  void changeLogLevel(int index);
  void restartServer();
  void quit();
  void addConn();
  void handleConnSelection();
  void inputChanged();
  void fillTreeStudy();
  void setArchiveDirectory();
  void updateServerDir();
  void updateContextMenu(const QPoint& point);
  void moveSelectedItems();
  void queuedMove(QTreeWidgetItem* item);


private:

  void setConnectionParams();
  void storeSettings(int type);
  void retrieveSettings();
  void startServer();
  void stopServer();
  void removeConnection(int index);
  void addConnection(QString peer, QString ip, QString port);
  void printResults(int sum, const char* type);
  
  dcmtkContainer<dcmtkNode*>* getSelectedNodes();
    
  // Designer form
  Ui_SimpleView *ui;

  dcmtkEchoScu*                 m_echoScu;
  dcmtkFindScu*                 m_findScu;
  dcmtkMoveScu*                 m_moveThread;
  dcmtkStoreScp*                m_serverThread;
  dcmtkStoreScu*                m_sendThread;

  std::vector<dcmtkConnectionData> m_nodes;

  std::string m_peerIP, m_peerTitle;
  std::string m_patientName;
  int m_peerPort;

  dcmtkNode                     m_ourNode;

  LoggerConsoleOutput*          m_shellOut;
  LoggerFileOutput*             m_fileOut;
  LoggerWidgetOutput*           m_widgetOut;
  LoggerWidget*                 m_loggerWidget;

  QProgressDialog*              progress;
};

#endif // SimpleView_H
