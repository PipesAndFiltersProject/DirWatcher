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

#include "DDirWatcherMarshallerHandler.hpp"
#include "DDirWatcherDataItem.hpp"

namespace DirWatcher {
   
   const std::string DDirWatcherMarshallerHandler::TAG{"WatcherMarshal "};
   
   DDirWatcherMarshallerHandler::DDirWatcherMarshallerHandler(OHARBase::ProcessorNode & node)
   : myNode(node), bufCapacity(100)
   {
      std::string bufSize = myNode.getConfigItemValue("bufsize");
      if (bufSize.length() > 0) {
         bufCapacity = std::stoi(bufSize);
      }
   }
   
   DDirWatcherMarshallerHandler::~DDirWatcherMarshallerHandler() {
   }
   
   bool DDirWatcherMarshallerHandler::consume(OHARBase::Package & data) {
      
      DDirWatcherDataItem * item = nullptr;
      
      item = dynamic_cast<DDirWatcherDataItem*>(data.getDataItem());
      tinyxml2::XMLDocument document;
      LOG(INFO) << "Starting to process incoming data item.";
      addOrUpdateDataItem(item);
      LOG(INFO) << "Preparing the XML tree for saving.";
      prepareTree(document);
      LOG(INFO) << "Saving the XML tree.";
      save(document);
      
      return false;
   }
   
   void DDirWatcherMarshallerHandler::addOrUpdateDataItem(DDirWatcherDataItem * item) {
      bool didFind = false;
      for (DDirWatcherDataItem * bufItem : buffer) {
         if (*bufItem == * item) {
            LOG(INFO) << "Data item already existed, updating.";
            bufItem->addFrom(*item);
            didFind = true;
            break;
         }
      }
      if (!didFind) {
         if (buffer.size() >= bufCapacity) {
            LOG(INFO) << "Buffer full, removing oldest data";
            item = buffer.front();
            delete item;
            buffer.pop_front();
         }
         LOG(INFO) << "Adding new data item to buffer.";
         buffer.push_back(item);
      }
   }
   
   bool DDirWatcherMarshallerHandler::prepareTree(tinyxml2::XMLDocument & tree) {
      using namespace tinyxml2;

      XMLElement * targets = tree.NewElement("targets");

      for (DDirWatcherDataItem * item : buffer) {
         LOG(INFO) << TAG << "Buffer has " << buffer.size() << " data items.";
         nlohmann::json j = *item;
         std::string str = j.dump();
         LOG(INFO) << str;
         
         XMLElement * target = tree.NewElement("target");
         target->SetAttribute("name", item->getId().c_str());
         const std::vector<std::string> userNames = item->getWhoChanged();
         XMLElement * usersElement = tree.NewElement("users");
         for (const std::string & user : userNames) {
            XMLElement * userElement = tree.NewElement("user");
            userElement->SetText(user.c_str());
            usersElement->InsertEndChild(userElement);
         }
         target->InsertEndChild(usersElement);
         XMLElement * eventsElement = tree.NewElement("events");
         const std::vector<std::pair<std::string,int>> events = item->getChangeEvents();
         for (const std::pair<std::string,int> & event : events) {
            XMLElement * eventElement = tree.NewElement("event");
            eventElement->SetAttribute("type", event.first.c_str());
            eventElement->SetText(event.second);
            eventsElement->InsertEndChild(eventElement);
         }
         target->InsertEndChild(eventsElement);
         targets->InsertEndChild(target);
      }
      tree.InsertEndChild(targets);
      LOG(INFO) << "XML tree created ";
      return true;
   }
   
   bool DDirWatcherMarshallerHandler::save(tinyxml2::XMLDocument & tree) {
      const std::string format = myNode.getConfigItemValue("marshal");
      LOG(INFO) << "Marshalling the data items as " << format << " to " << myNode.getOutputFileName();
      if (format == "xml") {
         tree.SaveFile(myNode.getOutputFileName().c_str());
      } else {
         LOG(WARNING) << TAG << "Format not supported";
         return false;
      }
      return true;
   }
   
} // namespace
