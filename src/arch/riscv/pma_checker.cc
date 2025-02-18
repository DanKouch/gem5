/*
 * Copyright (c) 2021 Huawei International
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "arch/riscv/pma_checker.hh"

#include "base/addr_range.hh"
#include "base/types.hh"
#include "mem/packet.hh"
#include "mem/request.hh"
#include "params/PMAChecker.hh"
#include "sim/sim_object.hh"

namespace gem5
{

namespace RiscvISA
{

PMAChecker::PMAChecker(const Params &params) :
BasePMAChecker(params),
uncacheable(params.uncacheable.begin(), params.uncacheable.end())
{
}

void
PMAChecker::check(const RequestPtr &req)
{
    if (isUncacheable(req->getPaddr(), req->getSize())) {
        req->setFlags(Request::UNCACHEABLE | Request::STRICT_ORDER);
    }
}

bool
PMAChecker::isUncacheable(const AddrRange &range)
{
    for (auto const &uncacheable_range: uncacheable) {
        if (range.isSubset(uncacheable_range)) {
            return true;
        }
    }
    return false;
}

bool
PMAChecker::isUncacheable(const Addr &addr, const unsigned size)
{
    AddrRange range(addr, addr + size);
    return isUncacheable(range);
}

bool
PMAChecker::isUncacheable(PacketPtr pkt)
{
    return isUncacheable(pkt->getAddrRange());
}

void
PMAChecker::takeOverFrom(BasePMAChecker *old)
{
    PMAChecker* derived_old = dynamic_cast<PMAChecker*>(old);
    assert(derived_old != nullptr);
    uncacheable = derived_old->uncacheable;
}

} // namespace RiscvISA
} // namespace gem5
