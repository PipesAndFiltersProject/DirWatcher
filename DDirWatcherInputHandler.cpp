//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#include <iostream>
#include <g3log/g3log.hpp>

#include <ProcessorNode/ProcessorNode.h>
#include <ProcessorNode/Package.h>

#include "DDirWatcherInputHandler.hpp"
#include "DDirWatcherDataItem.hpp"

namespace DirWatcher {
   
   const std::string DDirWatcherInputHandler::TAG{"WatcherInput "};
   
   
   DDirWatcherInputHandler::DDirWatcherInputHandler()
   {
   }
   
   DDirWatcherInputHandler::~DDirWatcherInputHandler() {
   }
   
   bool DDirWatcherInputHandler::consume(OHARBase::Package & data) {
      using namespace OHARBase;
      if (data.getType() == Package::Data && data.getPayloadString().length() > 0) {
         LOG(INFO) << TAG << "** data received, handling! **";
         // parse data to a student data object
         nlohmann::json j = nlohmann::json::parse(data.getPayloadString());
         std::unique_ptr<DDirWatcherDataItem> item = std::make_unique<DDirWatcherDataItem>(j.get<DirWatcher::DDirWatcherDataItem>());
         
         data.setPayload(std::move(item));
      }
      return false; // Always let others handle this data package too.
   }
   
   
} // namespace
