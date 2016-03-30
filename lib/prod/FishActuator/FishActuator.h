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
   * @param fishHwId Fish Hardware Id {0..n-1}
   */
  virtual void notifyFishEvent(unsigned int fishHwId, FishEvent event) = 0;

  typedef enum
  {
    ErrFishNotFound,
    ErrFishAlreadyExists,
    ErrFishBusy
  } FishError;

  /**
   * Fish error notification.
   * Occurs when any error occurred.
   * @param fishHwId Fish Hardware Id {0..n-1}
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
   */
  FishActuator();

  /**
   * Destructor.
   */
  virtual ~FishActuator();

  /**
   * Attach specific Fish event and error notification adapter object.
   * @param errorNotifier Specific error notifier
   */
  void attachAdapter(FishNotificationAdapter* adapter);

  /**
   * Add a new Fish to HW port specified by a particular Fish Hardware Id.
   * @param fishHwId Fish Hardware Id {0..n-1}
   */
  void addFishAtHwId(unsigned int fishHwId);

  /**
   * Remove a Fish from HW port specified by a particular Fish Hardware Id.
   * @param fishHwId Fish Hardware Id {0..n-1}
   */
  void delFishAtHwId(unsigned int fishHwId);

  /**
   * Activate a particular Fish.
   * @param fishHwId Fish Hardware Id {0..n-1}
   */
  void activateFish(unsigned int fishHwId);

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
