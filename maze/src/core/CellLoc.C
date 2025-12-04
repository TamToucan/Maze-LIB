#include <vector>
#include <iterator>

#include "CellLoc.h"

namespace Maze {

CellLoc operator+(const CellLoc& rLHS, const CellLoc& rRHS)
{
    CellLoc l_ret;
    l_ret.reserve(rLHS.size());
    CellLoc::const_iterator l_lhsItr = rLHS.begin();
    CellLoc::const_iterator l_rhsItr = rRHS.begin();
    while (l_lhsItr != rLHS.end())
    {
        l_ret.push_back(*l_lhsItr + *l_rhsItr);
        ++l_lhsItr;
        ++l_rhsItr;
    }
    return l_ret;
}

CellLoc& operator+=(CellLoc& rLHS, const CellLoc rRHS)
{
    CellLoc::iterator l_lhsItr = rLHS.begin();
    CellLoc::const_iterator l_rhsItr = rRHS.begin();
    while (l_lhsItr != rLHS.end())
    {
        *l_lhsItr += *l_rhsItr;
        ++l_lhsItr;
        ++l_rhsItr;
    }
    return rLHS;
}

} // namespace

