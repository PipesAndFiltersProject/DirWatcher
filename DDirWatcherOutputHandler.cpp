//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#include <iostream>
#include <g3log/g3log.hpp>

#include <OHARBaseLayer/ProcessorNode.h>
#include <OHARBaseLayer/Package.h>

#include "DDirWatcherOutputHandler.hpp"
#include "DDirWatcherDataItem.hpp"

namespace DirWatcher {
   
   const std::string DDirWatcherOutputHandler::TAG{"WatcherOut "};
   
   
   DDirWatcherOutputHandler::DDirWatcherOutputHandler(OHARBase::ProcessorNode & node)
   : myNode(node)
   {
   }
   
   DDirWatcherOutputHandler::~DDirWatcherOutputHandler() {
   }
   
   bool DDirWatcherOutputHandler::consume(OHARBase::Package & data) {
      LOG(INFO) << TAG << "Starting to send a package";
      if (data.getType() == OHARBase::Package::Data) {
         OHARBase::DataItem * item = data.getPayloadObject();
         // If the package contains the binary data object...
         if (item) {
            const DDirWatcherDataItem * eventItem = dynamic_cast<const DDirWatcherDataItem*>(item);
            // ...and it was a student data item object...
            if (eventItem) {
               // ...stream the data into a string payload...
               LOG(INFO) << TAG << "It is a student so creating payload";

               myNode.logAndShowUIMessage("Sending watch event data to next node: " + eventItem->getId());
               LOG(INFO) << TAG << "Data is converted to JSON... ";
               nlohmann::json j = *eventItem;
               std::string payload = j.dump();
               // ... set it as the data for the Package...
               data.setPayload(payload);
               LOG(INFO) << TAG << "...and telling the processornode to send it.";
               // ... and ask the Node to send the data to the next Node.
               myNode.sendData(data);
            }
         }
         return true; // data consumed, sent away. No need to pass along anymore.
      } else if (data.getType() == OHARBase::Package::Control)  {
         LOG(INFO) << TAG << "Forwarding a command: " << data.getPayloadString();
         myNode.sendData(data);
         return true;
      }
      return false; // Always let others handle this data package too.
   }
   
   
} // namespace
