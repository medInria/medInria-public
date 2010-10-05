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

class ServerThread;
class SendThread;

class dcmtkEchoScu;
class dcmtkMoveScu;
class dcmtkFindDataset;
class dcmtkFindScu;

class LoggerConsoleOutput;
class LoggerFileOutput;
class LoggerWidgetOutput;
class LoggerWidget;

class SimpleView : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  SimpleView(); 
  ~SimpleView();

protected:

  
   
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
  dcmtkMoveScu*                 m_moveScu;

  ServerThread*                 m_serverThread;
  SendThread*                   m_sendThread;

  std::vector<dcmtkNode::dcmtkConnectionData> m_nodes;

  std::string m_peerIP, m_peerTitle;
  std::string m_ourIP, m_ourTitle;
  std::string m_patientName;

  int m_peerPort, m_ourPort;

  LoggerConsoleOutput*          m_shellOut;
  LoggerFileOutput*             m_fileOut;
  LoggerWidgetOutput*           m_widgetOut;
  LoggerWidget*                 m_loggerWidget;
};

#endif // SimpleView_H
