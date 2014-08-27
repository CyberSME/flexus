#ifndef FLEXUS_TRANSPORTS__INSTRUCTION_TRANSPORT_HPP_INCLUDED
#define FLEXUS_TRANSPORTS__INSTRUCTION_TRANSPORT_HPP_INCLUDED

#include <core/transport.hpp>

namespace Flexus {
namespace SharedTypes {

#ifndef FLEXUS_TAG_ArchitecturalInstructionTag
#define FLEXUS_TAG_ArchitecturalInstructionTag
struct ArchitecturalInstructionTag_t {};
struct ArchitecturalInstruction;
namespace {
ArchitecturalInstructionTag_t ArchitecturalInstructionTag;
}
#endif //FLEXUS_TAG_ArchitecturalInstructionTag

#ifndef FLEXUS_TAG_TransactionTrackerTag
#define FLEXUS_TAG_TransactionTrackerTag
struct TransactionTrackerTag_t {};
struct TransactionTracker;
namespace {
TransactionTrackerTag_t TransactionTrackerTag;
}
#endif //FLEXUS_TAG_TransactionTrackerTag

typedef Transport
< mpl::vector
< transport_entry< ArchitecturalInstructionTag_t, ArchitecturalInstruction >
, transport_entry< TransactionTrackerTag_t, TransactionTracker >
>
> InstructionTransport;

} //namespace SharedTypes
} //namespace Flexus

#endif //FLEXUS_TRANSPORTS__INSTRUCTION_TRANSPORT_HPP_INCLUDED
