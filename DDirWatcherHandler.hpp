//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#pragma once

#include <ProcessorNode/DataHandler.h>

// Forward declaration
namespace fsw {
   class monitor;
   class event;
}

namespace DirWatcher {
   
   
   class DDirWatcherHandler : public OHARBase::DataHandler {
      
   public:
      DDirWatcherHandler(OHARBase::ProcessorNode & node);
      virtual ~DDirWatcherHandler();
      
      bool consume(OHARBase::Package & data) override;
      
      void processEvents(const std::vector<fsw::event> & events);
      
   private:
      OHARBase::ProcessorNode & myNode;
      fsw::monitor * directoryMonitor;
      std::string userToReport;
      std::string pathToObserve;
      const static std::string TAG;
      std::thread * monitorThread;
      bool running;
   };
   
} // namespace
