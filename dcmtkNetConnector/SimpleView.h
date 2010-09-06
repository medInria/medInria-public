#ifndef SimpleView_H
#define SimpleView_H

#include <QMainWindow>

// Forward Qt class declarations
class Ui_SimpleView;
class QTreeWidgetItem;

class dcmtkEchoScu;
class dcmtkMoveScu;
class dcmtkStoreScu;

#include "dcmtkFindScu.h"

#include <iostream>
#include <vector>

class SimpleView : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  SimpleView(); 
  ~SimpleView();

protected:

  
   
protected slots:

  virtual void search();
  virtual void move(QTreeWidgetItem * item, int column);
  virtual void testConnection();

private:

  void setConnectionParams();

  void storeSettings();
  void retrieveSettings();
    
  // Designer form
  Ui_SimpleView *ui;

  dcmtkEchoScu*                 m_echoScu;
  dcmtkFindScu*                 m_findScu;
  dcmtkMoveScu*                 m_moveScu;
  dcmtkStoreScu*                m_storeScu;

  std::vector<dcmtkFindDataset*> m_resultSet;
  std::vector<dcmtkFindDataset*>::iterator iter;

  std::string m_peerIP, m_peerTitle;
  std::string m_ourIP, m_ourTitle;
  std::string m_patientName;
  int m_peerPort, m_ourPort;
};

#endif // SimpleView_H
