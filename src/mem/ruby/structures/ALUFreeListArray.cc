#include "mem/ruby/structures/ALUFreeListArray.hh"

#include "base/intmath.hh"
#include "mem/ruby/system/RubySystem.hh"
#include "sim/cur_tick.hh"

#include <stdio.h>


namespace gem5
{

namespace ruby
{

/*
* Behavior:
*   Requests will go through unless one/both of the following are met:
*       - There have been more than [alus] requests in the current cycle
*       - The same line has been accessed in the past accessLatency cycles
*/

ALUFreeListArray::ALUFreeListArray(unsigned int alus, Cycles accessLatency,
                                   RubySystem *rs)
    : m_ruby_system(rs)
{
    this->alus = alus;
    this->accessLatency = accessLatency;
}

bool ALUFreeListArray::tryAccess(int64_t idx)
{
    uint32_t accesses_this_tick = 0;

    // Remove requests from the tail of the queue that occured more than
    // accessLatency ticks ago
    Tick oldestValidRecordStart = curTick() -
                                ((this->accessLatency) *
                                m_ruby_system->clockPeriod());

    while (accessQueue.size() > 0 &&
         (accessQueue.back().startTick < oldestValidRecordStart)){
        accessQueue.pop_back();
    }

    for (AccessRecord& record : accessQueue) {
        // Block access if we would be using more ALUs than we have in a
        // single tick
        if (record.startTick == curTick() &&
          (++accesses_this_tick > this->alus)) {
            return false;
        }

        // Block access if the index is already beign used
        if(record.idx == idx) {
            return false;
        }
    }

    return true;
}

void ALUFreeListArray::reserve(int64_t idx)
{
    // Only called after tryAccess, so we know queue is up to date and that
    // the access is valid

    // Add record to queue
    accessQueue.push_front(AccessRecord(idx, curTick()));
}

} // namespace ruby
} // namespace gem5
