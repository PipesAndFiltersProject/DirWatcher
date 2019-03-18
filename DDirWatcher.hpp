//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//

#pragma once

#include <OHARBaseLayer/ProcessorNode.h>
#include <OHARBaseLayer/ProcessorNodeObserver.h>

namespace DirWatcher {
   
   class DDirWatcher : public OHARBase::ProcessorNodeObserver {
      
   public:
      DDirWatcher(const std::string & name, const std::string & config);
      virtual ~DDirWatcher();
      
      void run();
      void NodeEventHappened(EventType e, const std::string & message) override;
      
   private:
      const std::string nodeName;
      const std::string configurationFile;
      OHARBase::ProcessorNode * node;
      std::mutex stdCoutMutex;
      std::istringstream input;

   };
   
} // namespace
