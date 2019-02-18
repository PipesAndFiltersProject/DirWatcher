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
   
   
   class DDirWatcherInputHandler : public OHARBase::DataHandler {
      
   public:
      DDirWatcherInputHandler(OHARBase::ProcessorNode & node);
      virtual ~DDirWatcherInputHandler();
      
      bool consume(OHARBase::Package & data) override;
      
   private:
      OHARBase::ProcessorNode & myNode;
      const static std::string TAG;
   };
   
} // namespace
