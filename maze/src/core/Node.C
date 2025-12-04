#include <vector>

#include "Node.h"
#include "CellLoc.h"
#include "CellType.h"

#include "Debug.h"
#include <iostream>

namespace Maze {

///////////////////////////////////////////////////////////////////////////

Node::Node(const CellType& rType, const CellLoc& rLoc) :
    m_type(rType), m_location(rLoc)
{
}

///////////////////////////////////////////////////////////////////////////

Node::~Node()
{
}

///////////////////////////////////////////////////////////////////////////

const CellType& Node::getCellType() const
{
    return m_type;
}

///////////////////////////////////////////////////////////////////////////

const CellLoc& Node::getCellLoc() const
{
    return m_location;
}

///////////////////////////////////////////////////////////////////////////

int Node::getNumExits() const
{
    return m_exits.size();
}

///////////////////////////////////////////////////////////////////////////

bool Node::isOpen(int exitNum) const
{
    return (exitNum < getNumExits()) and (m_exits[exitNum].m_flags&OPEN);
}

///////////////////////////////////////////////////////////////////////////

bool Node::isClosed(int exitNum) const
{
    return not isOpen(exitNum);
}

///////////////////////////////////////////////////////////////////////////

bool  Node::isUpTree(int exitNum) const
{
    return (exitNum < getNumExits()) and (m_exits[exitNum].m_flags&UPTREE);
}

///////////////////////////////////////////////////////////////////////////

bool Node::isDownTree(int exitNum) const
{
    return not isUpTree(exitNum);
}

///////////////////////////////////////////////////////////////////////////

Node* Node::getExitNode(int exitNum) const
{
    return (exitNum < getNumExits())
        ? m_exits[exitNum].m_pNode
        : 0;
}

///////////////////////////////////////////////////////////////////////////

void Node::addExit(const Exit& rExit)
{
    m_exits.push_back(rExit);
    LOG_DEBUG("ADD EXIT: " << *this);
}

///////////////////////////////////////////////////////////////////////////


//
// Set or Clear the OPEN flag of the exit
//
void Node::setOpen(int exitNum, bool isOpen)
{
    if (exitNum < getNumExits())
    {
        if (isOpen)
        {
            m_exits[exitNum].m_flags |= OPEN;
        }
        else
        {
            m_exits[exitNum].m_flags &= ~OPEN;
        }
    }
}

///////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Node& rNode)
{
    os << "NODE: " << rNode.getCellLoc() << " EXITS " << rNode.getNumExits();

    for (int i=0; i < rNode.getNumExits(); ++i)
    {
        Node* l_pExitNode = rNode.getExitNode(i);
        if (l_pExitNode)
        {
            os << "\n   exit: " << i
               << " " << (rNode.isUpTree(i) ? "UP" : "DN")
               << " " << (rNode.isOpen(i) ? "OPEN" : "CLSD")
               << " TO: " << l_pExitNode->getCellLoc();
        }
    }
    return os;
}

///////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const CellLoc& rCellLoc)
{
    return os << "(" << rCellLoc[0] << "," << rCellLoc[1] << ")";
}

} // namespace

