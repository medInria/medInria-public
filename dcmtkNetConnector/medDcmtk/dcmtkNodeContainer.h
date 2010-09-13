
#include <vector>
#include "dcmtkFindDataset.h"

struct ConnData
{
    std::string title;
    std::string ip;
    unsigned int port;
};

class DatasetContainer
{
private:

    std::vector<dcmtkFindDataset> m_findDs;
    std::vector<dcmtkFindDataset>::iterator m_findIter;

public:

    ~DatasetContainer();

    void addDataset(dcmtkFindDataset* ds){m_findDs.push_back(*ds);};

    void clear();

    dcmtkFindDataset* getFirst();
    dcmtkFindDataset* getNext();
    dcmtkFindDataset* getAtPos(unsigned int position);
};

class Node
{
private:
    ConnData m_connData;
    DatasetContainer m_dsContainer;

public:

    void addConnData(ConnData node){m_connData = node;};

    ConnData getConnData(){return m_connData;};

    DatasetContainer* getDatasetContainer(){return &m_dsContainer;};

};

class dcmtkNodeContainer
{
private:
    std::vector<Node> m_nodeArr;
    std::vector<Node>::iterator m_nodeIter;
public:

    ~dcmtkNodeContainer();

    void addNode(Node* nd){m_nodeArr.push_back(*nd);};

    void clear();

    Node* getFirst();
    Node* getNext();
    Node* getAtPos(unsigned int position);

};
