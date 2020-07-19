// Copyright 2020, University of Freiburg
// Authors: Axel Lehmann <lehmann@cs.uni-freiburg.de>.

#include "osm2ttl/util/DispatchQueue.h"

#include <pthread.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

// ____________________________________________________________________________
osm2ttl::util::DispatchQueue::DispatchQueue(size_t threadCount,
                                            const std::string& name)
  : _threads(threadCount) {
  _queueName = name;
  _quit = false;
  _die = false;
  for (size_t i = 0; i < _threads.size(); ++i) {
    _threads[i] = std::thread(&osm2ttl::util::DispatchQueue::handler, this);
    pthread_setname_np(_threads[i].native_handle(), _queueName.c_str());
  }
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::limit() {
  limit(std::max(_threads.size(), _queue.size()));
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::limit(size_t maxSize) {
  // Allow atleast one entry in the queue -> single threaded.
  std::cerr << "(" << _queueName << ") update limit: " << _maxSize;
  _maxSize = std::max(size_t(1), maxSize);
  std::cerr << " -> " << _maxSize << std::endl;
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::checkFreeRam() {
  if (_insertCount++ != 0) {
    return;
  }
  const size_t kilo = 1024;
  const size_t mega = kilo * kilo;
  const size_t giga = kilo * mega;
  const size_t dangerRam = 2 * giga;
  const size_t lowRam = 3 * giga;
  const size_t enoughRam = 4 * giga;
  size_t freeRam = sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
  if (_maxSize == std::numeric_limits<size_t>::max() && freeRam < lowRam) {
    // Low ram, ... try to keep current usage.
    limit();
  }
  if (_maxSize != std::numeric_limits<size_t>::max()) {
    if (freeRam > enoughRam) {
      // We have enough ram again, use it before somebody else does.
      unlimit();
    } else if (freeRam < dangerRam) {
      // Dangerously low ram, try to half our queues to reduce memory usage
      // after workers have taken items.
      limit(_maxSize / 2);
    }
  }
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::unlimit() {
  limit(std::numeric_limits<size_t>::max());
  // Increased storage capacity -> inform all inserting calls.
  _conditionVariableIn.notify_all();
}

// ____________________________________________________________________________
osm2ttl::util::DispatchQueue::~DispatchQueue() {
  std::unique_lock<std::mutex> lock(_lockOut);
  _die = true;
  lock.unlock();
  _conditionVariableOut.notify_all();
  quit();
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::quit() {
  std::unique_lock<std::mutex> lock(_lockOut);
  _quit = true;
  lock.unlock();
  _conditionVariableOut.notify_all();

  for (size_t i = 0; i < _threads.size(); ++i) {
    if (_threads[i].joinable()) {
      _threads[i].join();
    }
  }
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::dispatch(
  const std::function<void(void)>& op) {
  std::unique_lock<std::mutex> lockIn(_lockIn);
  _conditionVariableIn.wait(lockIn, [this]{
    return (_queue.size() < _maxSize);
  });
  checkFreeRam();
  std::unique_lock<std::mutex> lockOut(_lockOut);
  _queue.push(op);
  lockOut.unlock();
  lockIn.unlock();
  _conditionVariableOut.notify_one();
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::dispatch(std::function<void(void)>&& op) {
  std::unique_lock<std::mutex> lockIn(_lockIn);
  _conditionVariableIn.wait(lockIn, [this]{
    return (_queue.size() < _maxSize);
  });
  checkFreeRam();
  std::unique_lock<std::mutex> lockOut(_lockOut);
  _queue.push(op);
  lockOut.unlock();
  lockIn.unlock();
  _conditionVariableOut.notify_one();
}

// ____________________________________________________________________________
void osm2ttl::util::DispatchQueue::handler(void) {
  std::unique_lock<std::mutex> lockOut(_lockOut);
  do {
    _conditionVariableOut.wait(lockOut, [this]{
      return (!_queue.empty() || _quit || _die);
    });
    if (!_die && !_queue.empty()) {
      auto op = std::move(_queue.front());
      _queue.pop();
      lockOut.unlock();
      op();
      lockOut.lock();
      _conditionVariableIn.notify_one();
    }
  } while (!_die && (!_quit || !_queue.empty()));
}
