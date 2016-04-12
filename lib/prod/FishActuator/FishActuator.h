/*
 * FishActuator.h
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#ifndef PROD_FISHACTUATOR_FISHACTUATOR_H_
#define PROD_FISHACTUATOR_FISHACTUATOR_H_

class DbgCli_Topic;

//-----------------------------------------------------------------------------

class FishNotificationAdapter
{
public:
  typedef enum
  {
    EvtFishActivated,
    EvtFishStopped,
    EvtFishAdded,
    EvtFishDeleted
  } FishEvent;

  /**
   * Fish activation event notification.
   * Occurs when a fish gets activated successfully.
   * @param fishHwId Fish Hardware Id {0..n-1}, n = 992
   */
  virtual void notifyFishEvent(unsigned int fishHwId, FishEvent event) = 0;

  typedef enum
  {
    ErrFishNotFound,
    ErrFishHwIdOutOfRange,
    ErrFishQueueFull,
    ErrFishQueueCorrupt,
    ErrFishAlreadyExists,
    ErrFishBusy
  } FishError;

  /**
   * Fish error notification.
   * Occurs when any error occurred.
   * @param fishHwId Fish Hardware Id {0..n-1}, n = 992
   */
  virtual void notifyFishError(unsigned int fishHwId, FishError error) = 0;

  virtual ~FishNotificationAdapter() { }

protected:
  // Interface, constructor must not be accessible.
  FishNotificationAdapter() { }

private:  // forbidden functions
  FishNotificationAdapter(const FishNotificationAdapter& src);              // copy constructor
  FishNotificationAdapter& operator = (const FishNotificationAdapter& src); // assignment operator
};

//-----------------------------------------------------------------------------

class FishCollection;

//-----------------------------------------------------------------------------

class FishActuator
{
public:
  /**
   * Constructor.
   * @param adapter Specific Fish Notification Adapter (optional)
   */
  FishActuator(FishNotificationAdapter* adapter = 0);

  /**
   * Destructor.
   */
  virtual ~FishActuator();

  /**
   * Attach specific Fish event and error notification adapter object.
   * @param adapter Specific Fish Notification Adapter
   */
  void attachAdapter(FishNotificationAdapter* adapter);

  /**
   * Check if a particular Fish is configured or not.
   * @param fishHwId Fish Hardware Id {0..n-1}, n = 992
   * @return isFishConfigured true: Fish is configured, false: Fish is not configured.
   */
  bool isFishConfigured(unsigned int fishHwId);

  /**
   * Add a new Fish to HW port specified by a particular Fish Hardware Id.
   * @param fishHwId Fish Hardware Id {0..n-1}, n = 992
   */
  void addFishAtHwId(unsigned int fishHwId);

  /**
   * Remove a Fish from HW port specified by a particular Fish Hardware Id.
   * @param fishHwId Fish Hardware Id {0..n-1}, n = 992
   */
  void delFishAtHwId(unsigned int fishHwId);

  /**
   * Activate a particular Fish.
   * @param fishHwId Fish Hardware Id {0..n-1}, n = 992
   */
  void activateFish(unsigned int fishHwId);

  /**
   * Emergency Stop: all Fish motion gets stopped immediately.
   */
  void stopFish();

  /**
   * Get Debug CLI Topic object.
   * @return dbgCliTopic Pointer to the Debug CLI Topic object.
   */
  DbgCli_Topic* dbgCliTopic();

private:
  FishCollection* m_fishCollection;  /// Pointer to the Fish Collection component object.
  DbgCli_Topic* m_dbgCliTopic;

private:  // forbidden functions
  FishActuator(const FishActuator& src);              // copy constructor
  FishActuator& operator = (const FishActuator& src); // assignment operator
};

//-----------------------------------------------------------------------------

#endif /* PROD_FISHACTUATOR_FISHACTUATOR_H_ */
