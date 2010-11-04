#include <QThread>

class dcmtkStoreScu;

/**
 * @class SendThread
 * @author Michael Knopke
 * @brief QT Thread to send a threaded c-store-scu request
 */
class SendThread : public QThread 
{
public:

    SendThread();
    ~SendThread();

    virtual void run();

    void setConnectionParams(const char* peerTitle, const char* peerIP, int peerPort, 
                             const char* ourTitle, const char* ourIP, int ourPort);

    void setScanDirectory(const char* dir);


private:

    dcmtkStoreScu*                  m_storeScu;

};

