//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#pragma once

#include <tinyxml2.h>

#include <OHARBaseLayer/DataHandler.h>

#include "DDirWatcherDataItem.hpp"


namespace DirWatcher {
   
   class DDirWatcherMarshallerHandler : public OHARBase::DataHandler {
      
   public:
      DDirWatcherMarshallerHandler(OHARBase::ProcessorNode & node);
      virtual ~DDirWatcherMarshallerHandler();
      
      bool consume(OHARBase::Package & data) override;
      
   private:
      void addOrUpdateDataItem(DDirWatcherDataItem * item);
      bool prepareTree(tinyxml2::XMLDocument & tree);
      bool save(tinyxml2::XMLDocument & tree);
      
   private:
      OHARBase::ProcessorNode & myNode;
      const static std::string TAG;
      std::list<DDirWatcherDataItem*> buffer;
      int bufCapacity;
   };
   
} // namespace
