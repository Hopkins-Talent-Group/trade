#include "trading/DecisionMemo.hpp"

namespace trading {

bool isMemoComplete(const DecisionMemo& memo) {
    return !memo.hypothesis.empty() &&
           !memo.invalidation.condition.empty() &&
           memo.feeAdjustedRR > 0.0 &&
           memo.sizeCap.positionSize > 0;
}

} // namespace trading