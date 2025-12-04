#ifndef MAZE_NODE_H
#define MAZE_NODE_H

#include <iostream>

#include <vector>

#include "CellLoc.h"
#include "CellType.h"

//
// This is used to represent a Node (cell/room/whatever) in the
// generated maze. See Maze::Generator for a description.
//
namespace Maze {

    class Generator;

class Node
{
public:
    Node(const CellType& rType, const CellLoc& rLoc);
    virtual ~Node();

    virtual const CellType& getCellType() const;
    virtual const CellLoc&  getCellLoc() const;

    virtual int   getNumExits() const;
    virtual bool  isOpen(int exitNum) const;
    virtual bool  isClosed(int exitNum) const;
    virtual bool  isUpTree(int exitNum) const;
    virtual bool  isDownTree(int exitNum) const;
    virtual Node* getExitNode(int exitNum) const;

    //
    // The following methods are used to create Nodes
    // They are not needed if just using the Generator to create
    // the maze and then reading it
    //
public:
    enum {
        OPEN=0x1,
        UPTREE=0x2
    };

    struct Exit {
        Node*        m_pNode;
        unsigned int m_flags;
        Exit(Node* pNode=0, int flags=0) :
            m_pNode(pNode), m_flags(flags) { }
    };

    void addExit(const Exit& rExit);
    void setOpen(int exitNum, bool isOpen);

protected:
    CellType           m_type;
    CellLoc            m_location;
    std::vector<Exit>  m_exits;
};

// For Debug
std::ostream& operator<<(std::ostream& os, const Node& rNode);
std::ostream& operator<<(std::ostream& os, const CellLoc& rCellLoc);

} // namespace

#endif
