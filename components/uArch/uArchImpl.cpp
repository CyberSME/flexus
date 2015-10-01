#include <components/uArch/uArch.hpp>

#define FLEXUS_BEGIN_COMPONENT uArch
#include FLEXUS_BEGIN_COMPONENT_IMPLEMENTATION()

#include <components/Common/Slices/MemoryMessage.hpp>
#include <components/Common/Slices/ExecuteState.hpp>
#include <components/MTManager/MTManager.hpp>

#include "uArchInterfaces.hpp"

#include <core/debug/debug.hpp>
#include "microArch.hpp"

#include <core/qemu/mai_api.hpp>

#define DBG_DefineCategories uArchCat, Special
#define DBG_SetDefaultOps AddCat(uArchCat)
#include DBG_Control()

namespace nuArch {

using namespace Flexus;
using namespace Core;
using namespace SharedTypes;

class uArch_QemuObject_Impl  {
  std::shared_ptr<microArch> theMicroArch;
public:
  uArch_QemuObject_Impl(Flexus::Qemu::API::conf_object_t * /*ignored*/ ) {}

  void setMicroArch(std::shared_ptr<microArch> aMicroArch) {
    theMicroArch = aMicroArch;
  }

  void testCkptRestore() {
    DBG_Assert(theMicroArch);
    theMicroArch->testCkptRestore();
  }
  void printROB() {
    DBG_Assert(theMicroArch);
    theMicroArch->printROB();
  }
  void printMemQueue() {
    DBG_Assert(theMicroArch);
    theMicroArch->printMemQueue();
  }
  void printSRB() {
    DBG_Assert(theMicroArch);
    theMicroArch->printSRB();
  }
  void printMSHR() {
    DBG_Assert(theMicroArch);
    theMicroArch->printMSHR();
  }
  void pregs() {
    DBG_Assert(theMicroArch);
    theMicroArch->pregs();
  }
  void pregsAll() {
    DBG_Assert(theMicroArch);
    theMicroArch->pregsAll();
  }
  void resynchronize() {
    DBG_Assert(theMicroArch);
    theMicroArch->resynchronize();
  }
  void printRegMappings(std::string aRegSet) {
    DBG_Assert(theMicroArch);
    theMicroArch->printRegMappings(aRegSet);
  }
  void printRegFreeList(std::string aRegSet) {
    DBG_Assert(theMicroArch);
    theMicroArch->printRegFreeList(aRegSet);
  }
  void printRegReverseMappings(std::string aRegSet) {
    DBG_Assert(theMicroArch);
    theMicroArch->printRegReverseMappings(aRegSet);
  }
  void printRegAssignments(std::string aRegSet) {
    DBG_Assert(theMicroArch);
    theMicroArch->printAssignments(aRegSet);
  }

};

class uArch_QemuObject : public Qemu::AddInObject <uArch_QemuObject_Impl> {

  typedef Qemu::AddInObject<uArch_QemuObject_Impl> base;
public:
  static const Qemu::Persistence  class_persistence = Qemu::Session;
  //These constants are defined in Simics/simics.cpp
  static std::string className() {
    return "uArch";
  }
  static std::string classDescription() {
    return "uArch object";
  }

  uArch_QemuObject() : base() { }
  uArch_QemuObject(Qemu::API::conf_object_t * aQemuObject) : base(aQemuObject) {}
  uArch_QemuObject(uArch_QemuObject_Impl * anImpl) : base(anImpl) {}

//ALEX - This is adding functions to Simics' Command Line Interface.
//Disabled it, as the interface with QEMU is probably completely different

/*
  template <class Class>
  static void defineClass(Class & aClass) {

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::printROB
      , "print-rob"
      , "Print out ROB contents"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::printSRB
      , "print-srb"
      , "Print out SRB contents"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::printMemQueue
      , "print-memqueue"
      , "Print out MemQueue contents"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::printMSHR
      , "print-mshr"
      , "Print out MSHR contents"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::printRegMappings
      , "print-reg-mappings"
      , "Print out current register map table"
      , "regset"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::printRegFreeList
      , "print-reg-freelist"
      , "Print out current register free list"
      , "regset"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::printRegReverseMappings
      , "print-reg-reversemappings"
      , "Print out reverse register mappings"
      , "regset"
    );

    aClass.addCommand

    ( & uArch_SimicsObject_Impl::printRegAssignments
      , "print-reg-assignment"
      , "Print out ordered list of physical registers for each name"
      , "regset"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::resynchronize
      , "resynchronize"
      , "Resynchronize with Simics"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::pregs
      , "pregs"
      , "Print architectural register contents"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::pregsAll
      , "pregs-all"
      , "Print architectural register contents"
    );

    aClass.addCommand
    ( & uArch_SimicsObject_Impl::testCkptRestore
      , "test-ckpt-restore"
      , "Test checkpoint/restore implementation"
    );

  }
*/
};

Qemu::Factory<uArch_QemuObject> theuArchQemuFactory;

class FLEXUS_COMPONENT(uArch) {
  FLEXUS_COMPONENT_IMPL(uArch);

  std::shared_ptr<microArch> theMicroArch;
  uArch_QemuObject theuArchObject;

public:
  FLEXUS_COMPONENT_CONSTRUCTOR(uArch)
    : base( FLEXUS_PASS_CONSTRUCTOR_ARGS )
  {}

  bool isQuiesced() const {
    return !theMicroArch || theMicroArch->isQuiesced();
  }

  void initialize() {
    uArchOptions_t
    options;

    options.ROBSize              = cfg.ROBSize;
    options.SBSize               = cfg.SBSize;
    options.NAWBypassSB          = cfg.NAWBypassSB;
    options.NAWWaitAtSync        = cfg.NAWWaitAtSync;
    options.retireWidth          = cfg.RetireWidth;
    options.numMemoryPorts       = cfg.MemoryPorts;
    options.numSnoopPorts        = cfg.SnoopPorts;
    options.numStorePrefetches   = cfg.StorePrefetches;
    options.prefetchEarly        = cfg.PrefetchEarly;
    options.spinControlEnabled   = cfg.SpinControl;
    options.consistencyModel     = (nuArch::eConsistencyModel)cfg.ConsistencyModel;
    options.coherenceUnit        = cfg.CoherenceUnit;
    options.breakOnResynchronize = cfg.BreakOnResynchronize;
    options.validateMMU          = cfg.ValidateMMU;
    options.speculativeOrder     = cfg.SpeculativeOrder;
    options.speculateOnAtomicValue   = cfg.SpeculateOnAtomicValue;
    options.speculateOnAtomicValuePerfect   = cfg.SpeculateOnAtomicValuePerfect;
    options.speculativeCheckpoints = cfg.SpeculativeCheckpoints;
    options.checkpointThreshold   = cfg.CheckpointThreshold;
    options.earlySGP             = cfg.EarlySGP;    /* CMU-ONLY */
    options.trackParallelAccesses = cfg.TrackParallelAccesses; /* CMU-ONLY */
    options.inOrderMemory        = cfg.InOrderMemory;
    options.inOrderExecute       = cfg.InOrderExecute;
    options.onChipLatency        = cfg.OnChipLatency;
    options.offChipLatency       = cfg.OffChipLatency;
    options.name                 = statName();
    options.node                 = flexusIndex();

    options.numIntAlu = cfg.NumIntAlu;
    options.intAluOpLatency = cfg.IntAluOpLatency;
    options.intAluOpPipelineResetTime = cfg.IntAluOpPipelineResetTime;

    options.numIntMult = cfg.NumIntMult;
    options.intMultOpLatency = cfg.IntMultOpLatency;
    options.intMultOpPipelineResetTime = cfg.IntMultOpPipelineResetTime;
    options.intDivOpLatency = cfg.IntDivOpLatency;
    options.intDivOpPipelineResetTime = cfg.IntDivOpPipelineResetTime;

    options.numFpAlu = cfg.NumFpAlu;
    options.fpAddOpLatency = cfg.FpAddOpLatency;
    options.fpAddOpPipelineResetTime = cfg.FpAddOpPipelineResetTime;
    options.fpCmpOpLatency = cfg.FpCmpOpLatency;
    options.fpCmpOpPipelineResetTime = cfg.FpCmpOpPipelineResetTime;
    options.fpCvtOpLatency = cfg.FpCvtOpLatency;
    options.fpCvtOpPipelineResetTime = cfg.FpCvtOpPipelineResetTime;

    options.numFpMult = cfg.NumFpMult;
    options.fpMultOpLatency = cfg.FpMultOpLatency;
    options.fpMultOpPipelineResetTime = cfg.FpMultOpPipelineResetTime;
    options.fpDivOpLatency = cfg.FpDivOpLatency;
    options.fpDivOpPipelineResetTime = cfg.FpDivOpPipelineResetTime;
    options.fpSqrtOpLatency = cfg.FpSqrtOpLatency;
    options.fpSqrtOpPipelineResetTime = cfg.FpSqrtOpPipelineResetTime;

    theMicroArch = microArch::construct ( options
                                          , [this](auto x){ return this->squash(x);}
                                          , [this](auto x, auto y){ return this->redirect(x,y);}
                                          , [this](auto x, auto y){ return this->changeState(x,y);}
                                          , [this](auto x){ return this->feedback(x);}
                                          , [this](auto x, auto y, auto z){ return this->notifyTMS(x,y,z);}/* CMU-ONLY */
                                          , [this](auto x){ return this->signalStoreForwardingHit(x);}
                                        );

    theuArchObject = theuArchQemuFactory.create( (std::string("uarch-") + boost::padded_string_cast < 2, '0' > (flexusIndex())).c_str() );
    theuArchObject->setMicroArch(theMicroArch);

  }

  void finalize() {}

public:
  FLEXUS_PORT_ALWAYS_AVAILABLE(DispatchIn);
  void push( interface::DispatchIn const &, boost::intrusive_ptr< AbstractInstruction > & anInstruction ) {
    theMicroArch->dispatch(anInstruction);
  }

  FLEXUS_PORT_ALWAYS_AVAILABLE(AvailableDispatchOut);
  std::pair<int, bool> pull(AvailableDispatchOut const &) {
    return std::make_pair( theMicroArch->availableROB(), theMicroArch->isSynchronized() );
  }

  FLEXUS_PORT_ALWAYS_AVAILABLE(Stalled);
  bool pull(Stalled const &) {
    return theMicroArch->isStalled();
  }

  FLEXUS_PORT_ALWAYS_AVAILABLE(ICount);
  int32_t pull(ICount const &) {
    return theMicroArch->iCount();
  }

  FLEXUS_PORT_ALWAYS_AVAILABLE(MemoryIn);
  void push( interface::MemoryIn const &, MemoryTransport & aTransport) {
    handleMemoryMessage(aTransport);
  }

  FLEXUS_PORT_ALWAYS_AVAILABLE( WritePermissionLost );
  void push( interface::WritePermissionLost const &, PhysicalMemoryAddress & anAddress) {
    theMicroArch->writePermissionLost(anAddress);
  }

public:
  //The FetchDrive drive interface sends a commands to the Feeder and then fetches instructions,
  //passing each instruction to its FetchOut port.
  void drive(interface::uArchDrive const &) {
    doCycle();
  }

private:
  struct ResynchronizeWithSimicsException {};

  void squash(eSquashCause aSquashReason) {
    FLEXUS_CHANNEL( SquashOut ) << aSquashReason;
  }

  void changeState(int32_t aTL, int32_t aPSTATE) {
    CPUState state;
    state.theTL = aTL;
    state.thePSTATE = aPSTATE;
    FLEXUS_CHANNEL( ChangeCPUState ) << state;
  }

  void redirect(VirtualMemoryAddress aPC, VirtualMemoryAddress aNextPC) {
    std::pair< VirtualMemoryAddress, VirtualMemoryAddress> redirect_addr = std::make_pair( aPC, aNextPC);
    FLEXUS_CHANNEL( RedirectOut ) << redirect_addr;
  }

  void feedback(boost::intrusive_ptr<BranchFeedback> aFeedback) {
    FLEXUS_CHANNEL( BranchFeedbackOut ) << aFeedback;
  }

  void signalStoreForwardingHit(bool garbage) {
    bool value = true;
    FLEXUS_CHANNEL( StoreForwardingHitSeen ) << value;
  }

  /* CMU-ONLY-BLOCK-BEGIN */
  void notifyTMS(PredictorMessage::tPredictorMessageType aType, PhysicalMemoryAddress anAddress, boost::intrusive_ptr<TransactionTracker> aTracker) {
    //Inform TMS that we are completing read
    boost::intrusive_ptr<PredictorMessage> pmsg (new PredictorMessage(aType, flexusIndex(), anAddress));
    PredictorTransport pt;
    pt.set(PredictorMessageTag, pmsg);
    pt.set(TransactionTrackerTag, aTracker);
    FLEXUS_CHANNEL(NotifyTMS) << pt;
  }
  /* CMU-ONLY-BLOCK-END */

  void doCycle() {
    if (cfg.Multithread) {
      if (nMTManager::MTManager::get()->runThisEX(flexusIndex())) {
        theMicroArch->cycle();
      } else {
        theMicroArch->skipCycle();
      }
    } else {
      theMicroArch->cycle();
    }
    sendMemoryMessages();
  }

  void sendMemoryMessages() {
    while (FLEXUS_CHANNEL( MemoryOut_Request ).available()) {
      boost::intrusive_ptr< MemOp > op(theMicroArch->popMemOp());
      if (! op ) break;

      DBG_( Iface, ( << "Send Request: " << *op) );

      MemoryTransport transport;
      boost::intrusive_ptr<MemoryMessage> operation;

      if (op->theNAW) {
        DBG_Assert( op->theOperation == kStore );
        operation = new MemoryMessage(MemoryMessage::NonAllocatingStoreReq, op->thePAddr, op->thePC);
      } else {

        switch ( op->theOperation ) {
          case kLoad:
            //pc = Simics::Processor::getProcessor(flexusIndex())->translateInstruction(op->thePC);
            operation = MemoryMessage::newLoad(op->thePAddr, op->thePC);
            break;

          case kAtomicPreload:
            //pc = Simics::Processor::getProcessor(flexusIndex())->translateInstruction(op->thePC);
            operation = MemoryMessage::newAtomicPreload(op->thePAddr, op->thePC);
            break;

          case kStorePrefetch:
            //pc = Simics::Processor::getProcessor(flexusIndex())->translateInstruction(op->thePC);
            operation = MemoryMessage::newStorePrefetch(op->thePAddr, op->thePC, DataWord(op->theValue));
            break;

          case kStore:
            //pc = Simics::Processor::getProcessor(flexusIndex())->translateInstruction(op->thePC);
            operation = MemoryMessage::newStore(op->thePAddr, op->thePC, DataWord(op->theValue));
            break;

          case kRMW:
            //pc = Simics::Processor::getProcessor(flexusIndex())->translateInstruction(op->thePC);
            operation = MemoryMessage::newRMW(op->thePAddr, op->thePC, DataWord(op->theValue));
            break;

          case kCAS:
            //pc = Simics::Processor::getProcessor(flexusIndex())->translateInstruction(op->thePC);
            operation = MemoryMessage::newCAS(op->thePAddr, op->thePC, DataWord(op->theValue));
            break;

          default:
            DBG_Assert( false,  ( << "Unknown memory operation type: " << op->theOperation ) );
        }
      }
      operation->reqSize() = op->theSize;
      if (op->theTracker) {
        transport.set(TransactionTrackerTag, op->theTracker);
      } else {
        boost::intrusive_ptr<TransactionTracker> tracker = new TransactionTracker;
        tracker->setAddress( op->thePAddr );
        tracker->setInitiator(flexusIndex());
        tracker->setSource("uArch");
        tracker->setOS(false); //TWENISCH - need to set this properly
        transport.set(TransactionTrackerTag, tracker);
        op->theTracker = tracker;
      }

      transport.set(MemoryMessageTag, operation);
      transport.set(uArchStateTag, op);

      if (op->theNAW && (op->thePAddr & 63) != 0) {
        //Auto-reply to the unaligned parts of NAW
        transport[MemoryMessageTag]->type() = MemoryMessage::NonAllocatingStoreReply;
        handleMemoryMessage( transport );
      } else {
        FLEXUS_CHANNEL( MemoryOut_Request) << transport;
      }
    }

    while (FLEXUS_CHANNEL( MemoryOut_Snoop).available()) {
      boost::intrusive_ptr< MemOp > op(theMicroArch->popSnoopOp());
      if (! op ) break;

      DBG_( Iface, ( << "Send Snoop: " << *op) );

      MemoryTransport transport;
      boost::intrusive_ptr<MemoryMessage> operation;

      PhysicalMemoryAddress pc;

      switch ( op->theOperation ) {
        case kInvAck:
          DBG_( Verb, ( << "Send InvAck.") );
          operation = new MemoryMessage(MemoryMessage::InvalidateAck, op->thePAddr);
          break;

        case kDowngradeAck:
          DBG_( Verb, ( << "Send DowngradeAck.") );
          operation = new MemoryMessage(MemoryMessage::DowngradeAck, op->thePAddr);
          break;

        case kProbeAck:
          operation = new MemoryMessage(MemoryMessage::ProbedNotPresent, op->thePAddr);
          break;

        case kReturnReply:
          operation = new MemoryMessage(MemoryMessage::ReturnReply, op->thePAddr );
          break;

        default:
          DBG_Assert( false,  ( << "Unknown memory operation type: " << op->theOperation ) );
      }

      operation->reqSize() = op->theSize;
      if (op->theTracker) {
        transport.set(TransactionTrackerTag, op->theTracker);
      } else {
        boost::intrusive_ptr<TransactionTracker> tracker = new TransactionTracker;
        tracker->setAddress( op->thePAddr );
        tracker->setInitiator(flexusIndex());
        tracker->setSource("uArch");
        transport.set(TransactionTrackerTag, tracker);
      }

      transport.set(MemoryMessageTag, operation);

      FLEXUS_CHANNEL( MemoryOut_Snoop) << transport;
    }

  }

  void handleMemoryMessage( MemoryTransport & aTransport) {
    boost::intrusive_ptr< MemOp > op;
    boost::intrusive_ptr<MemoryMessage> msg (aTransport[MemoryMessageTag]);

    // For Invalidates and Downgrades, the uArchState isn't for us, it's for the original requester
    // So in those cases we always want to construct a new MemOp based on the MemoryMesage
    if (aTransport[uArchStateTag] && msg->type() != MemoryMessage::Invalidate && msg->type() != MemoryMessage::Downgrade) {
      op = aTransport[uArchStateTag];
    } else {
      op = new MemOp();
      op->thePAddr = msg->address();
      op->theSize = eSize(msg->reqSize());
      op->theTracker = aTransport[TransactionTrackerTag];
    }

    switch (msg->type()) {
      case MemoryMessage::LoadReply:
        op->theOperation = kLoadReply;
        break;

      case MemoryMessage::AtomicPreloadReply:
        op->theOperation = kAtomicPreloadReply;
        break;

      case MemoryMessage::StoreReply:
        op->theOperation = kStoreReply;
        break;

      case MemoryMessage::NonAllocatingStoreReply:
        op->theOperation = kStoreReply;
        break;

      case MemoryMessage::StorePrefetchReply:
        op->theOperation = kStorePrefetchReply;
        break;

      case MemoryMessage::Invalidate:
        op->theOperation = kInvalidate;
        break;

      case MemoryMessage::Downgrade:
        op->theOperation = kDowngrade;
        break;

      case MemoryMessage::Probe:
        op->theOperation = kProbe;
        break;

      case MemoryMessage::RMWReply:
        op->theOperation = kRMWReply;
        break;

      case MemoryMessage::CmpxReply:
        op->theOperation = kCASReply;
        break;

      case MemoryMessage::ReturnReq:
        op->theOperation = kReturnReq;
        break;

      default:
        DBG_Assert( false,  ( << "Unhandled Memory Message type: " << msg->type() ) );
    }

    theMicroArch->pushMemOp(op);
  }
};

}//End namespace nuArch

FLEXUS_COMPONENT_INSTANTIATOR(uArch, nuArch);

#include FLEXUS_END_COMPONENT_IMPLEMENTATION()
#define FLEXUS_END_COMPONENT uArch

#define DBG_Reset
#include DBG_Control()
