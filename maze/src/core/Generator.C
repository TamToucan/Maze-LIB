#include <assert.h>
#include <vector>
#include <stack>
#include <iterator>
#include <utility>
#include <algorithm>

#include "Generator.h"

#include "BasicHashTable.h"
#include "I_HashTableObject.h"
#include "I_Random.h"
#include "RandSimple.h"
#include "MazeData.h"
#include "CellType.h"
#include "CellLoc.h"
#include "Node.h"

#include "MazeHelper.h"

#include "Debug.h"
#include <iostream>
#if defined(PRT_DEBUG_VERBOSE)
#include "BasicHashTableIterator.h"
#endif

namespace Maze
{

    // Class to hide the implementation from Generator.h
    // Otherwise would need to expose the hash table etc
class Generator::Impl
{
    friend class Generator;

protected:
    Impl() : m_hashedNodes(1000003) { }
    ~Impl() { m_hashedNodes.clearAndDestroy(); }

protected:
    class HashedNode;

    MazeData* generate(unsigned int seed);
    void makeSinglePathMaze(Node* pNode);
    void makeMaze();

    bool validLocation(CellLoc* pLoc) const;

    void addNodeExits(HashedNode* pCurHNode);

    HashedNode* getNode(const CellType& rType, const CellLoc& rLoc,
                        bool* pIsNew);
    bool nodesConnected(const HashedNode* pHNode1,
                        const HashedNode* pHNode2) const;
    void connectNodes(HashedNode* pHNode1, HashedNode* pHNode2);

    void openExit(Node* pFromNode, int fromExit);

    void removeDeadEnds(Node* pRoot);

    void makeOpenPlan(Node* pRoot);

#if defined(PRT_DEBUG_VERBOSE)
    void Generator::Impl::printChains();
#endif

protected:
    MazeData m_mazeData;
    RNG::I_Random*    m_pRNG;

    Util::BasicHashTable<HashedNode> m_hashedNodes;

    // Used to store a list of Exits as defined by
    // the HashedNode and the exit number for that Node
    typedef std::pair<HashedNode*, int> HNodeExitPair;
    typedef std::vector< HNodeExitPair > ExitList;
    ExitList m_exitList;
};

///////////////////////////////////////////////////////////////////////////

    // Class to hold a hashed Node object, plus
    // extra info for generating the maze
class Generator::Impl::HashedNode : public Util::I_HashTableObject
{
public:
    HashedNode(Node* pNode) : m_pNode(pNode), m_pChain(0) { }

    virtual ~HashedNode() { }

    virtual unsigned long hash() const
    {
        const CellLoc& l_rLoc = m_pNode->getCellLoc();
        unsigned long l_hash = 0;
        for (CellLoc::const_iterator l_itr = l_rLoc.begin();
                l_itr != l_rLoc.end();
                ++l_itr)
        {
            l_hash *= *l_itr;
        }
        return l_hash;
    }

    virtual bool isEqual(const I_HashTableObject* pObject) const
    {
        const HashedNode* l_pRHS;
        l_pRHS = dynamic_cast<const HashedNode*>(pObject);
        return (m_pNode->getCellLoc() == l_pRHS->m_pNode->getCellLoc());
    }

    virtual Node* getNode() const { return m_pNode; }

    virtual const HashedNode* getChainEnd() const
    {
        LOG_DEBUG("Maze::getChainEnd " << m_pNode->getCellLoc());
        const HashedNode* l_pEnd = this;
        HashedNode* l_pChain = m_pChain;
        while (l_pChain)
        {
            l_pEnd = l_pChain;
            l_pChain = l_pChain->m_pChain;
            if (l_pChain)
            {
                LOG_DEBUG("    -> " << l_pEnd->m_pNode->getCellLoc());
            }
        }
        LOG_DEBUG(" => " << l_pEnd->m_pNode->getCellLoc());
        return l_pEnd;
    }

    virtual HashedNode* getChainEnd()
    {
        HashedNode* l_pEnd = this;
        HashedNode* l_pChain = m_pChain;
        while (l_pChain)
        {
            l_pEnd = l_pChain;
            l_pChain = l_pChain->m_pChain;
        }
        return l_pEnd;
    }

    virtual void setChain(HashedNode* pEnd)
    {
        LOG_DEBUG("Maze::setChain " << m_pNode->getCellLoc() << " to " << pEnd->m_pNode->getCellLoc());
        if (this != pEnd)
        {
            HashedNode* l_pChain = m_pChain;

            while (l_pChain and l_pChain != pEnd)
            {
                LOG_DEBUG("    also chain " << l_pChain->m_pNode->getCellLoc());
                HashedNode* l_pNext = l_pChain->m_pChain;
                l_pChain->m_pChain = pEnd;
                l_pChain = l_pNext;
            }
            m_pChain = pEnd;
        }
        else
        {
            LOG_DEBUG("  SAME NODE (Dont set)");
        }
    }

#if defined(PRT_DEBUG_VERBOSE)
    virtual void prtChain() const
    {
        const HashedNode* l_pChain = this;
        std::cout << "CHAIN for "
            << "(" << l_pChain->m_pNode->getCellLoc()[0]
            << "," <<l_pChain->m_pNode->getCellLoc()[1] << ")";
        while (l_pChain)
        {
            if (l_pChain->m_pChain)
            {
                std::cout << " ---> "
                    << "(" << l_pChain->m_pChain->m_pNode->getCellLoc()[0]
                    << "," <<l_pChain->m_pChain->m_pNode->getCellLoc()[1] << ")";
            }
            else
            {
                std::cout << " ---> END";
            }
            l_pChain = l_pChain->m_pChain;
        }
        std::cout << "\n";
    }
#endif

protected:
    Node* m_pNode;
    HashedNode* m_pChain;
};

///////////////////////////////////////////////////////////////////////////

Generator::Generator(const MazeData& rMazeData, RNG::I_Random* pRNG) :
    pimpl(new Generator::Impl)
{
    pimpl->m_mazeData = rMazeData;
    setRNG(pRNG);
}

///////////////////////////////////////////////////////////////////////////

Generator::~Generator()
{
    delete pimpl;
}

///////////////////////////////////////////////////////////////////////////

void Generator::setRNG(RNG::I_Random* pRNG)
{
    if (pRNG)
    {
        pimpl->m_pRNG = pRNG;
    }
    else
    {
        static RNG::RandSimple l_simpleRNG(32);
        pimpl->m_pRNG = &l_simpleRNG;
    }
}

///////////////////////////////////////////////////////////////////////////

MazeData* Generator::generate(unsigned int seed)
{
    return pimpl->generate(seed);
}

///////////////////////////////////////////////////////////////////////////

Maze::MazeData* Generator::Impl::generate(unsigned int seed)
{
    MazeData* l_pRetData = new MazeData(m_mazeData);
    l_pRetData->setRoot(0);

    if (seed)
    {
        m_pRNG->initialise(seed);
    }

    //
    // Create the root node and then recursively
    // add all the exits to build a tree
    //
    bool l_isNew;
    Impl::HashedNode* l_pHRoot;
    l_pHRoot = getNode(m_mazeData.getTileData().getFirstCellType(),
                       m_mazeData.getStartLoc(), &l_isNew);
    Node* l_pRoot = l_pHRoot->getNode();
    LOG_INFO("Generator::generate - MAKE EXITS =========");

    //
    // Add the exits for the root and then add the exits for
    // each exit that leads down the tree add it's exits
    //
    // Used to use a recursive function i.e. addNodeExits would
    // call addNodeExits for each connected Node. This took ages
    // and filled up the stack for anything more than 90x90 2D maze
    //
    addNodeExits(l_pHRoot);
    for (unsigned int i=0; i != m_exitList.size(); ++i)
    {
        Impl::HashedNode* l_pHNode = m_exitList[i].first;
        int l_exitNum = m_exitList[i].second;
        if (l_pHNode->getNode()->isDownTree(l_exitNum))
        {
            // Get the Node this exit leads to
            // Original node is in the m_exitList => this is
            // a real exit => then the exit Node must exist
            Node* l_pExitNode = l_pHNode->getNode()->getExitNode(l_exitNum);
            assert(l_pExitNode);

            // Find the hash node for the exit node
            // It must exist in the hash table
            Impl::HashedNode l_findHNode(l_pExitNode);
            Impl::HashedNode* l_pHExitNode;
            l_pHExitNode = m_hashedNodes.find(&l_findHNode);
            assert(l_pHExitNode);

            // Add the exits for the exit Node
            addNodeExits(l_pHExitNode);
        }
    }

    if (m_mazeData.getSinglePath())
    {
        makeSinglePathMaze(l_pHRoot->getNode());
        l_pRetData->setEndLoc(m_mazeData.getEndLoc());
    }
    else
    {
        makeMaze();
    }

#if defined(PRT_DEBUG_VERBOSE)
    printChains();
#endif

    removeDeadEnds(l_pRoot);

    makeOpenPlan(l_pRoot);

    //
    // Tidy up
    //
    m_hashedNodes.clearAndDestroy();
    m_exitList.clear();

    //
    // Set the root node
    //
    l_pRetData->setRoot(l_pRoot);
    return l_pRetData;
}

#if defined(PRT_DEBUG_VERBOSE)
void Generator::Impl::printChains()
{
    std::cout << "\n\nCHAINS\n=========\n";
    Util::BasicHashTableIterator<HashedNode> itr(m_hashedNodes);
    HashedNode* l_pHNode = itr();
    while (l_pHNode)
    {
        l_pHNode->prtChain();
        l_pHNode = itr();
    }
}
#endif

///////////////////////////////////////////////////////////////////////////

//
// Doesn't use the m_exits
//
void Generator::Impl::makeSinglePathMaze(Node* pNode)
{
    // Stack of Node and distance from start
    std::stack< std::pair<Node*,int> > l_nodeStack;
    assert(pNode);
    int l_visited = 1;
    int l_distance = 0;
    int l_longest = 0;
    CellLoc l_endLoc = pNode->getCellLoc();
    const int l_totalCells = m_mazeData.getTotalCells();
    LOG_INFO("Maze::makeSinglePathMaze - Start " << pNode->getCellLoc() << " Total Cells = " << l_totalCells);

    // While not visited all cells
    //
    while (l_visited < l_totalCells)
    {
        LOG_DEBUG("Maze::makeSinglePathMaze - VISITED = " << l_visited);
        std::vector<int> l_possibleExits;
        l_possibleExits.reserve(pNode->getNumExits());

        // Find all neighbors of CurrentCell with walls intact
        for (int i=0; i < pNode->getNumExits(); ++i)
        {
            Node* l_pExitNeighbor = pNode->getExitNode(i);
            if (l_pExitNeighbor)
            {
                bool l_sealed = true;
                // Check all exists are walls => sealed
                for (int j=0; j < l_pExitNeighbor->getNumExits(); ++j)
                {
                    if (l_pExitNeighbor->isOpen(j))
                    {
                        l_sealed = false;
                        break;
                    }
                }
                // Store that exit as a next possible cell
                if (l_sealed)
                {
                    l_possibleExits.push_back(i);
                }
            }
        }
        // If have a sealed neighbor pick one at random and make an exit to it
        LOG_DEBUG("Maze::makeSinglePathMaze - Found " << l_possibleExits.size() << " from node " << pNode->getCellLoc());
        if (not l_possibleExits.empty())
        {
            int l_exit = l_possibleExits[ m_pRNG->getInt(0, l_possibleExits.size()-1) ];
            openExit(pNode, l_exit);
            // push cur node and distance on to the stack
            l_nodeStack.push(std::make_pair(pNode,l_distance));
            // move to the next cell
            pNode = pNode->getExitNode(l_exit);
            assert(pNode);
            // Increase visited and distance count
            ++l_visited;
            ++l_distance;
        }
        // No sealed neighbor => visited all neighbors
        // => Pop the previous cell off the stack
        else
        {
            // See if travelled further
            if (l_distance > l_longest)
            {
                l_endLoc = pNode->getCellLoc();
                l_longest = l_distance;
            }
            // Restore current node and distance from stack
            pNode = l_nodeStack.top().first;
            l_distance = l_nodeStack.top().second;
            assert(pNode);
            l_nodeStack.pop();
        }
    }
    // Might never have never got stuck
    //
    if (0 == l_longest)
    {
        l_endLoc = pNode->getCellLoc();
    }


    LOG_INFO("Maze::makeSinglePathMaze END LOC = " << l_endLoc);
    m_mazeData.setEndLoc(l_endLoc);
}

///////////////////////////////////////////////////////////////////////////

// Relies on the m_exits having been populated
//
void Generator::Impl::makeMaze()
{
    //
    // Randomized the list of all exits
    //
    int l_numExits = m_exitList.size();
    Impl::ExitList l_randomExitList;
    l_randomExitList.reserve(l_numExits);
    for (int i=l_numExits-1; i >= 0; --i)
    {
        int l_randIdx = m_pRNG->getInt(0, i);
        l_randomExitList[i] = m_exitList[l_randIdx];
        m_exitList[l_randIdx] = m_exitList[i];
    }

    if (Util::Debug::instance()->debugOn())
    {
        LOG_DEBUG("Generator::generate - ALL EXITS =========");
        for (Impl::ExitList::iterator l_itr = l_randomExitList.begin();
        l_itr != l_randomExitList.end();
        ++l_itr)
        {
            Node* l_pNode = l_itr->first->getNode();
            //        Node* l_pExitNode = l_pNode->getExitNode(l_itr->second);
            LOG_DEBUG("EXIT " << l_itr->second << ": " << *l_pNode);
        }
    }

    //
    // Go through the randomized list and open exits
    // if it won't connect two already connected Nodes
    //
    LOG_DEBUG("Generator::generate - OPEN EXITS =========");
    for (int i=0; i < l_numExits; ++i)
    {
        // Get the HashNode and exit number
        Impl::HashedNode* l_pHNode1 = l_randomExitList[i].first;
        int l_exitNum1 = l_randomExitList[i].second;
        Node* l_pNode1 = l_pHNode1->getNode();

        // Use them to find the Node this exit connects to
        Node* l_pNode2 = l_pNode1->getExitNode(l_exitNum1);
        assert(l_pNode2);
        Impl::HashedNode l_findHNode(l_pNode2);
        Impl::HashedNode* l_pHNode2 = m_hashedNodes.find(&l_findHNode);

        // The other node must exist in the hash table
        assert(l_pHNode2);

        //
        // Nodes not connected to each other
        // => open up the exit and connect the two Nodes
        //
        if (not nodesConnected(l_pHNode1, l_pHNode2))
        {
            openExit(l_pNode1, l_exitNum1);
            connectNodes(l_pHNode1, l_pHNode2);
        }
    }

    if (Util::Debug::instance()->debugOn())
    {
        LOG_DEBUG("Generator::generate - ALL EXITS =========");
        for (Impl::ExitList::iterator l_itr = l_randomExitList.begin();
        l_itr != l_randomExitList.end();
        ++l_itr)
        {
            Node* l_pNode = l_itr->first->getNode();
            //        Node* l_pExitNode = l_pNode->getExitNode(l_itr->second);
            LOG_DEBUG("EXIT " << l_itr->second << ": " << *l_pNode);
        }
    }
}

///////////////////////////////////////////////////////////////////////////

void Generator::Impl::removeDeadEnds(Node* pRoot)
{
    //
    // Remove any dead-ends
    //
    if (m_mazeData.getNoDeadEnds())
    {
        LOG_INFO("Maze::Generator - REMOVE DEAD-ENDS =========");
        MazeHelper::NodeList l_allNodes;
        l_allNodes.reserve(m_hashedNodes.entries());
        MazeHelper::makeNodeList(pRoot, l_allNodes);
        for (MazeHelper::NodeList::iterator l_itr = l_allNodes.begin();
             l_itr != l_allNodes.end();
             ++l_itr)
        {
            Node* l_pNode = *l_itr;
            int l_numOpenExits = 0;
            std::vector<int> l_possibleExits;
            l_possibleExits.reserve(l_pNode->getNumExits());
            for (int i=0; i < l_pNode->getNumExits(); ++i)
            {
                if (l_pNode->isOpen(i))
                {
                    ++l_numOpenExits;
                }
                else
                if (l_pNode->getExitNode(i))
                {
                    l_possibleExits.push_back(i);
                }
            }
            if ((1 == l_numOpenExits) and (not l_possibleExits.empty()))
            {
                int l_exitIdx = m_pRNG->getInt(0, l_possibleExits.size()-1);
                openExit(l_pNode, l_possibleExits[l_exitIdx]);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////

void Generator::Impl::makeOpenPlan(Node* pRoot)
{
    //
    // Open up random exits
    //
    if (m_mazeData.getOpenPlanChance())
    {
        LOG_INFO("Maze::Generator - MAKE OPEN PLAN =========");
        MazeHelper::NodeList l_allNodes;
        l_allNodes.reserve(m_hashedNodes.entries());
        MazeHelper::makeNodeList(pRoot, l_allNodes);
        for (MazeHelper::NodeList::iterator l_itr = l_allNodes.begin();
             l_itr != l_allNodes.end();
             ++l_itr)
        {
            Node* l_pNode = *l_itr;
            for (int i=0; i < l_pNode->getNumExits(); ++i)
            {
                if (l_pNode->isClosed(i) and l_pNode->getExitNode(i))
                {
                    if (m_pRNG->getInt(0, 99) < m_mazeData.getOpenPlanChance())
                    {
                        openExit(l_pNode, i);
                    }
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////

//
// Creates a new Node and inserts it into the hash table unless
// Node has already been created in which case it returns it
//
Generator::Impl::HashedNode* Generator::Impl::getNode(const CellType& rType,
                                                      const CellLoc& rLoc,
                                                      bool* pIsNew)
{
    Node* l_pNewNode = new Node(rType, rLoc);
    Impl::HashedNode* l_pNewHash = new Impl::HashedNode(l_pNewNode);
    Impl::HashedNode* l_pFoundHash = m_hashedNodes.find(l_pNewHash);
    if (l_pFoundHash)
    {
        *pIsNew = false;
        delete l_pNewHash;
        delete l_pNewNode;
        l_pNewHash = l_pFoundHash;
        l_pNewNode = l_pFoundHash->getNode();
    }
    else
    {
        *pIsNew = true;
        m_hashedNodes.insert(l_pNewHash);
    }
    return l_pNewHash;
}

///////////////////////////////////////////////////////////////////////////

//
// Check that the location is within the boundaries.
// If not then wrap it so it is (if wrapping is on)
//
bool Generator::Impl::validLocation(CellLoc* pLoc) const
{
    CellLoc::const_iterator l_dimItr = m_mazeData.getDimensions().begin();
    for (CellLoc::iterator l_itr = pLoc->begin();
            l_itr != pLoc->end();
            ++l_itr, ++l_dimItr)
    {
        if (m_mazeData.getWrapRoundOn())
        {
            if (*l_itr < 0)          *l_itr += *l_dimItr;
            if (*l_itr >= *l_dimItr) *l_itr -= *l_dimItr;
        }

        if ((*l_itr < 0) or (*l_itr >= *l_dimItr))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////

//
// Add all the exits to this Node and add all the "real" exits
// i.e. those that lead to valid locations, to the m_exitList
// Creates the Nodes that the real exits lead to (or points the
// exit at the existing Node if there is one)
//
void Generator::Impl::addNodeExits(HashedNode* pCurHNode)
{
    //
    // Get the type and location of this Node
    //
    CellType l_curType = pCurHNode->getNode()->getCellType();
    CellLoc l_curLoc = pCurHNode->getNode()->getCellLoc();

    //
    // Add the Connections as exits to this Node
    //
    int l_numCons = m_mazeData.getTileData().getNumConnections(l_curType);
    for (int i=0; i < l_numCons; ++i)
    {
        //
        // Get the Connection
        //
        const TileData::Connection* l_pCon =
                                m_mazeData.getTileData().getConnection(l_curType, i);
        assert(l_pCon);

        //
        // Move to the new location and check it is valid
        // If not then add a "dummy" exit to the Node that
        // doesn't have a Node. This is needed so that the
        // the exit numbers are always the same i.e. exits
        // don't have IDs other than their number, so the
        // same number must correspond to the same exit for
        // a given cell type
        //
        CellLoc l_newLoc = l_curLoc + l_pCon->locChange;
        if (not validLocation(&l_newLoc))
        {
            pCurHNode->getNode()->addExit(Node::Exit(0));
            continue;
        }

        //
        // Create or Find the Node this exit leads to
        // - Store the new exit in the m_exitList
        //
        bool l_isNew;
        HashedNode* l_pNewHNode = getNode(l_pCon->toCellType,
                                          l_newLoc,
                                          &l_isNew);
        m_exitList.push_back( HNodeExitPair(pCurHNode, i) );

        //
        // If created a new Node in the tree then add a DOWNTREE Node
        //
        if (l_isNew)
        {
            pCurHNode->getNode()->addExit(Node::Exit(l_pNewHNode->getNode()));
        }
        //
        // If Node already existed then it is UPTREE from this Node
        //
        else
        {
            pCurHNode->getNode()->addExit(Node::Exit(l_pNewHNode->getNode(),
                                            Node::UPTREE));
        }
    }
}

///////////////////////////////////////////////////////////////////////////

bool Generator::Impl::nodesConnected(const HashedNode* pHNode1,
                                     const HashedNode* pHNode2) const
{
    LOG_DEBUG("Maze::nodesConnected - CHECK CONNECTED ==========");
    return (pHNode1->getChainEnd() == pHNode2->getChainEnd());
}

///////////////////////////////////////////////////////////////////////////

void Generator::Impl::connectNodes(HashedNode* pHNode1, HashedNode* pHNode2)
{
#if defined(PRT_DEBUG_VERBOSE)
    printChains();
#endif
    HashedNode* l_pHEnd1 = pHNode1->getChainEnd();
    pHNode1->setChain(l_pHEnd1);
    pHNode2->setChain(l_pHEnd1);
#if defined(PRT_DEBUG_VERBOSE)
    printChains();
#endif
}

///////////////////////////////////////////////////////////////////////////

void Generator::Impl::openExit(Node* pFromNode, int fromExit)
{
    Node* l_pToNode = pFromNode->getExitNode(fromExit);

    // Find the other exit number from toNode to fromNode
    int l_toExit;
//    LOG_DEBUG("Maze::openExit - TRY OPEN exit " << fromExit << " " << *pFromNode << " TO " << *l_pToNode);
    for (l_toExit=0; l_toExit < l_pToNode->getNumExits(); ++l_toExit)
    {
        if (l_pToNode->getExitNode(l_toExit) == pFromNode)
        {
            // Could have more than one connection to the Node
            // => check that the cell change amount is opposite
            const TileData::Connection* l_pFromCon;
            const TileData::Connection* l_pToCon;
            l_pFromCon = m_mazeData.getTileData().getConnection(pFromNode->getCellType(),
                                                  fromExit);
            l_pToCon = m_mazeData.getTileData().getConnection(l_pToNode->getCellType(),
                                                l_toExit);
            bool l_isOppositeDir = true;
            for (unsigned int i=0; i < l_pFromCon->locChange.size(); ++i)
            {
                if (l_pFromCon->locChange[i] != -l_pToCon->locChange[i])
                {
                    l_isOppositeDir = false;
                    break;
                }
            }
            // If it's loc change is in the opposite direction then
            // this is the correct node
            if (l_isOppositeDir)
            {
                break;
            }
        }
    }
//    LOG_DEBUG("    TO EXIT " << l_toExit << " (NUMX " << l_pToNode->getNumExits() << ")");
    // Must have found the connecting exit
    assert(l_toExit != l_pToNode->getNumExits());

    // Open the exits
    pFromNode->setOpen(fromExit, true);
    l_pToNode->setOpen(l_toExit, true);

    LOG_DEBUG("Maze::openExit - OPEN from exit " << fromExit << " " << *pFromNode
              << "OPEN to exit " << l_toExit << " " << *l_pToNode);
}

///////////////////////////////////////////////////////////////////////////

} // namespace

