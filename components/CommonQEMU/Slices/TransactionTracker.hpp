#ifndef FLEXUS_SLICES__TRANSACTION_TRACKER_HPP_INCLUDED
#define FLEXUS_SLICES__TRANSACTION_TRACKER_HPP_INCLUDED

#ifdef FLEXUS_TransactionTracker_TYPE_PROVIDED
#error "Only one component may provide the Flexus::SharedTypes::TransactionTracker data type"
#endif
#define FLEXUS_TransactionTracker_TYPE_PROVIDED

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>

#include <core/boost_extensions/intrusive_ptr.hpp>
#include <core/types.hpp>
#include <core/flexus.hpp>

#include <components/CommonQEMU/Slices/FillLevel.hpp>
#include <components/CommonQEMU/Slices/FillType.hpp>

namespace Flexus {
namespace SharedTypes {

using boost::intrusive_ptr;
using namespace Flexus::Core;

inline std::istream & operator >> (std::istream & aStream, tFillLevel & aFillLevel) {
  int32_t temp;
  aStream >> temp;
  aFillLevel = tFillLevel(temp);
  return aStream;
}

enum tStateType {
  eInvalid,
  eShared,
  eModified
};

class TransactionTracker;

struct TransactionTracer {
  virtual void trace(TransactionTracker const & aTransaction) = 0;
  virtual ~TransactionTracer() {}
  static boost::shared_ptr<TransactionTracer> createTracer();
};

struct TransactionStatManager {
  virtual ~TransactionStatManager() {};
  virtual void count(TransactionTracker const & aTransaction) = 0;
  static boost::shared_ptr<TransactionStatManager> createTSM();
};

uint64_t getTTGUID();

class TransactionTracker : public boost::counted_base { /*, public FastAlloc*/
  typedef Flexus::SharedTypes::PhysicalMemoryAddress MemoryAddress;

  static boost::shared_ptr<TransactionTracer> theTracer;
  static boost::shared_ptr<TransactionStatManager> theTSM;

  //None of the fields in TransactionTracker are guaranteed to be filled
  //in, so they are all optional

  uint64_t theUniqueId;
  boost::optional<MemoryAddress> theAddress;
  boost::optional<unsigned> theInitiator;
  boost::optional<std::string> theSource;
  boost::optional<unsigned> theResponder;

  uint64_t theStartCycle;
  uint64_t theLastTimestamp;
  boost::optional<uint64_t> theCompletionCycle;
  boost::optional< std::string > theCurrentDelayComponent;
  boost::optional< std::string > theCurrentDelayCause;

  boost::optional<bool> theNetworkTrafficRequired;
  boost::optional<bool> theCriticalPath;
  boost::optional<bool> theWrongPath;
  boost::optional<bool> theOS;
  boost::optional<bool> theInPE;
  boost::optional<bool> theBlockedInMAF;
  boost::optional<tStateType> thePreviousState;
  boost::optional<tFillType> theFillType;
  boost::optional< tFillLevel > theOriginatorLevel;
  boost::optional< tFillLevel > theFillLevel;
  //std::vector< boost::tuple< std::string, std::string, int> > theCycleAccounting;
  boost::optional<bool> theFetch;
  boost::optional<bool> theWrite;
  boost::optional<bool> theSpeculativeAtomicLoad;
  boost::optional<uint64_t> theLogicalTimestamp;
  bool theWasCounted;

public:
  TransactionTracker() {
    theUniqueId = getTTGUID();
    theStartCycle = theLastTimestamp = Flexus::Core::theFlexus->cycleCount();
    theWasCounted = false;
  }
  ~TransactionTracker() {
    /*
    if (!theTracer) {
      theTracer = TransactionTracer::createTracer();
    }
    if (!theTSM) {
      theTSM = TransactionStatManager::createTSM();
    }
    theTracer->trace(*this);
    theTSM->count(*this);
    */
  }

  uint64_t uniqueId() {
    return theUniqueId;
  }

  void setAddress(MemoryAddress anAddress) {
    theAddress.reset(anAddress);
  }
  boost::optional<MemoryAddress> address() const {
    return theAddress;
  }

  void setLogicalTimestamp(uint64_t aTime) {
    theLogicalTimestamp.reset(aTime);
  }
  boost::optional<uint64_t> logicalTimestamp() const {
    return theLogicalTimestamp;
  }

  void setInitiator(unsigned aNode) {
    theInitiator.reset(aNode);
  }
  boost::optional<unsigned> initiator() const {
    return theInitiator;
  }

  void setResponder(unsigned aNode) {
    theResponder.reset(aNode);
  }
  boost::optional<unsigned> responder() const {
    return theResponder;
  }

  void setSource(std::string const & aSource) {
    theSource.reset(aSource);
  }
  boost::optional<std::string> source() const {
    return theSource;
  }

  uint64_t startCycle() const {
    return theStartCycle;
  }

  void complete() {
    theCompletionCycle.reset(Flexus::Core::theFlexus->cycleCount());
    if (*theCompletionCycle != theLastTimestamp) {
      recordDelay();
    }
  }
  boost::optional<uint64_t> completionCycle() const {
    return theCompletionCycle;
  }

  void setNetworkTrafficRequired(bool aFlag) {
    theNetworkTrafficRequired.reset(aFlag);
  }
  boost::optional<bool> networkTrafficRequired() const {
    return theNetworkTrafficRequired;
  }

  void setInPE(bool aFlag) {
    theInPE.reset(aFlag);
  }
  boost::optional<bool> inPE() const {
    return theInPE;
  }

  void setBlockedInMAF(bool aFlag) {
    theBlockedInMAF.reset(aFlag);
  }
  boost::optional<bool> blockedInMAF() const {
    return theBlockedInMAF;
  }

  void setSpeculativeAtomicLoad(bool aFlag) {
    theSpeculativeAtomicLoad.reset(aFlag);
  }
  boost::optional<bool> speculativeAtomicLoad() const {
    return theSpeculativeAtomicLoad;
  }

  void setCriticalPath(bool aFlag) {
    theCriticalPath.reset(aFlag);
  }
  boost::optional<bool> criticalPath() const {
    return theCriticalPath;
  }

  void setWrongPath(bool aFlag) {
    theWrongPath.reset(aFlag);
  }
  boost::optional<bool> wrongPath() const {
    return theWrongPath;
  }

  void setOS(bool aFlag) {
    theOS.reset(aFlag);
  }
  boost::optional<bool> OS() const {
    return theOS;
  }

  void setPreviousState(tStateType aStateType) {
    thePreviousState.reset(aStateType);
  }
  boost::optional<tStateType> previousState() const {
    return thePreviousState;
  }

  void setFillType(tFillType aFillType) {
    theFillType.reset(aFillType);
  }
  boost::optional<tFillType> fillType() const {
    return theFillType;
  }

  void setOriginatorLevel(tFillLevel aLevel) {
    theOriginatorLevel.reset(aLevel);
  }
  boost::optional<tFillLevel> originatorLevel() const {
    return theOriginatorLevel;
  }

  void setFillLevel(tFillLevel aLevel) {
    theFillLevel.reset(aLevel);
  }
  boost::optional<tFillLevel> fillLevel() const {
    return theFillLevel;
  }

  void setFetch(bool aFetch) {
    theFetch.reset(aFetch);
  }
  boost::optional<bool> isFetch() const {
    return theFetch;
  }

  bool wasCounted() const {
    return theWasCounted;
  }
  void setWasCounted() {
    theWasCounted = true;
  }

  void setDelayCause(std::string const & aComponent, std::string const & aCause) {
    /*
    if ( ! theCurrentDelayCause
        || *theCurrentDelayCause != aCause
        || *theCurrentDelayComponent != aComponent
        ) {
      if (Flexus::Core::theFlexus->cycleCount() != theLastTimestamp) {
         recordDelay();
         theLastTimestamp = Flexus::Core::theFlexus->cycleCount();
      }
      theCurrentDelayComponent.reset(aComponent);
      theCurrentDelayCause.reset(aCause);
    }
    */
  }
  void recordDelay() {
    /*
    if ( theCurrentDelayCause ) {
      theCycleAccounting.push_back( boost::make_tuple(*theCurrentDelayComponent, *theCurrentDelayCause, Flexus::Core::theFlexus->cycleCount() - theLastTimestamp) );
    } else {
      theCycleAccounting.push_back( boost::make_tuple(std::string("Unknown Component"), std::string("Unknown Cause"), Flexus::Core::theFlexus->cycleCount() - theLastTimestamp) );
    }
    */
  }
  /*
  std::vector< boost::tuple< std::string, std::string, int> > const & cycleAccounting() const {
    return theCycleAccounting;
  }
  */

};

} //End SharedTypes
} //End Flexus

#endif //FLEXUS_SLICES__TRANSACTION_TRACKER_HPP_INCLUDED