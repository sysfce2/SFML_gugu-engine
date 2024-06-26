////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "DataBinding/DataBindingImpl.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/System/Container.h"

using namespace gugu;

////////////////////////////////////////////////////////////////
// File Implementation

namespace tests {

bool DS_Condition::IsValid(const ConditionContext& context) const
{
    return IsValidImpl(context) == intendedResult;
}

bool DS_ConditionAnd::IsValidImpl(const struct ConditionContext& context) const
{
    for (const DS_Condition* condition : conditions)
    {
        if (!condition->IsValid(context))
        {
            return false;
        }
    }

    return true;
}

bool DS_ConditionPlayerLevel::IsValidImpl(const struct ConditionContext& context) const
{
    return (minLevel < 0 || context.playerLevel >= minLevel)
        && (maxLevel < 0 || context.playerLevel <= maxLevel);
}

bool DS_ConditionCheckFlag::IsValidImpl(const struct ConditionContext& context) const
{
    return StdSetContains(context.raisedFlags, flag) == raised;
}

} // namespace tests
