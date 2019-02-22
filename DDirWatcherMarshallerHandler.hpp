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
      friend class XMLMarshaller;
      friend class JSONMarshaller;
      class MarshallerInterface {
      public:
         virtual ~MarshallerInterface() {};
         virtual bool prepare(std::list<DDirWatcherDataItem*> & buffer) = 0;
         virtual bool save(const std::string & fileName) = 0;
      };
      void addOrUpdateDataItem(DDirWatcherDataItem * item);
      
   private:
      OHARBase::ProcessorNode & myNode;
      const static std::string TAG;
      std::list<DDirWatcherDataItem*> buffer;
      int bufCapacity;
      std::unique_ptr<MarshallerInterface> marshaller;
   };
   
} // namespace
