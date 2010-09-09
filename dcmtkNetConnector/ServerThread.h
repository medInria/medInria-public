#include <QThread>

class dcmtkStoreScp;

/**
 * @class ServerThread
 * @author Michael Knopke
 * @brief QT Thread to start the store-scp in background
 */
class ServerThread : public QThread 
{
public:

    ServerThread();
    ~ServerThread();

    virtual void run();

    void setConnectionParams(const char* ourTitle, const char* ourIP, unsigned short ourPort);

private:

    dcmtkStoreScp*                  m_server;
    unsigned short                  m_ourPort;

};

