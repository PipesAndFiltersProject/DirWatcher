//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#pragma once

#include <OHARBaseLayer/DataHandler.h>

namespace DirWatcher {
   
   
   class DDirWatcherOutputHandler : public OHARBase::DataHandler {
      
   public:
      DDirWatcherOutputHandler(OHARBase::ProcessorNode & node);
      virtual ~DDirWatcherOutputHandler();
      
      bool consume(OHARBase::Package & data) override;
      
   private:
      OHARBase::ProcessorNode & myNode;
      const static std::string TAG;
   };
   
} // namespace
