#ifndef __MEM_RUBY_STRUCTURES_ALUFREELISTARRAY_HH__
#define __MEM_RUBY_STRUCTURES_ALUFREELISTARRAY_HH__

#include <deque>

#include "mem/ruby/common/TypeDefines.hh"
#include "mem/ruby/system/RubySystem.hh"

namespace gem5
{

namespace ruby
{

class ALUFreeListArray
{
  private:
    unsigned int alus;
    Cycles accessLatency;
    RubySystem *m_ruby_system;

    class AccessRecord
    {
      public:
        AccessRecord() : idx(0), startTick(0) {}
        AccessRecord(int64_t idx, Tick startTick) {
          this->idx = idx;
          this->startTick = startTick;
        }

        int64_t idx;
        Tick startTick;
    };

    // Queue of accesses from past accessLatency cycles
    std::deque<AccessRecord> accessQueue;

  public:
    ALUFreeListArray(unsigned int alus, Cycles accessLatency, RubySystem *rs);

    bool tryAccess(int64_t idx);

    void reserve(int64_t idx);

    Cycles getLatency() const { return accessLatency; }
};

} // namespace ruby
} // namespace gem5

#endif
