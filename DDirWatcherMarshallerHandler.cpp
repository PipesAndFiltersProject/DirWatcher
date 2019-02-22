//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//

#include <iostream>
#include <fstream>

#include <g3log/g3log.hpp>

#include <nlohmann/json.hpp>

#include <OHARBaseLayer/ProcessorNode.h>
#include <OHARBaseLayer/Package.h>

#include "DDirWatcherMarshallerHandler.hpp"
#include "DDirWatcherDataItem.hpp"


namespace DirWatcher {
   
   // MARK: XMLMarshaller
   class XMLMarshaller : public DDirWatcherMarshallerHandler::MarshallerInterface {
   public:
      virtual bool prepare(std::list<DDirWatcherDataItem*> & buffer) override {
         const std::string TAG {"XMLMarshaller"};
         using namespace tinyxml2;
         
         document.Clear();
         
         XMLElement * targets = document.NewElement("targets");
         
         for (DDirWatcherDataItem * item : buffer) {
            LOG(INFO) << TAG << "Buffer has " << buffer.size() << " data items.";
            nlohmann::json j = *item;
            std::string str = j.dump();
            LOG(INFO) << str;
            
            XMLElement * target = document.NewElement("target");
            target->SetAttribute("name", item->getId().c_str());
            const std::vector<std::string> userNames = item->getWhoChanged();
            XMLElement * usersElement = document.NewElement("users");
            for (const std::string & user : userNames) {
               XMLElement * userElement = document.NewElement("user");
               userElement->SetText(user.c_str());
               usersElement->InsertEndChild(userElement);
            }
            target->InsertEndChild(usersElement);
            XMLElement * eventsElement = document.NewElement("events");
            const std::vector<std::pair<std::string,int>> events = item->getChangeEvents();
            for (const std::pair<std::string,int> & event : events) {
               XMLElement * eventElement = document.NewElement("event");
               eventElement->SetAttribute("type", event.first.c_str());
               eventElement->SetText(event.second);
               eventsElement->InsertEndChild(eventElement);
            }
            target->InsertEndChild(eventsElement);
            targets->InsertEndChild(target);
         }
         document.InsertEndChild(targets);
         LOG(INFO) << "XML tree created ";
         return true;
      }
      
      virtual bool save(const std::string & fileName) override {
         LOG(INFO) << "Marshalling the data items as XML to " << fileName;
         tinyxml2::XMLError error = document.SaveFile(fileName.c_str());
         return error == tinyxml2::XMLError::XML_SUCCESS;
      }
      
   private:
      tinyxml2::XMLDocument document;
   };
   
   // MARK: - JSONMarshaller
   class JSONMarshaller : public DDirWatcherMarshallerHandler::MarshallerInterface {
   public:
      virtual bool prepare(std::list<DDirWatcherDataItem*> & buffer) override {
         const std::string TAG {"JSONMarshaller"};
         
         using namespace nlohmann;
         jsonObj.clear();
         
         json array_explicit = json::array();
         for (DDirWatcherDataItem * item : buffer) {
            array_explicit.push_back(*item);
         }
         jsonObj.push_back(array_explicit);
         LOG(INFO) << "JSON tree created ";
         return true;
      }
      
      virtual bool save(const std::string & fileName) override {
         LOG(INFO) << "Marshalling the data items as JSON to " << fileName;
         std::ofstream file;
         file.open(fileName, std::ios::trunc | std::ios::out);
         file << jsonObj;
         return true;
      }
      
   private:
      nlohmann::json jsonObj;
   };

   // MARK: - DirWatcherHandler
   const std::string DDirWatcherMarshallerHandler::TAG{"WatcherMarshal "};
   
   DDirWatcherMarshallerHandler::DDirWatcherMarshallerHandler(OHARBase::ProcessorNode & node)
   : myNode(node), bufCapacity(100), marshaller(nullptr)
   {
      std::string bufSize = myNode.getConfigItemValue("bufsize");
      if (bufSize.length() > 0) {
         bufCapacity = std::stoi(bufSize);
      }
      const std::string format = myNode.getConfigItemValue("marshal");
      if (format == "xml") {
         marshaller = std::unique_ptr<MarshallerInterface>(new XMLMarshaller());
      } else if (format == "json") {
         marshaller = std::unique_ptr<MarshallerInterface>(new JSONMarshaller());
      }
   }
   
   DDirWatcherMarshallerHandler::~DDirWatcherMarshallerHandler() {
   }
   
   bool DDirWatcherMarshallerHandler::consume(OHARBase::Package & data) {
      
      DDirWatcherDataItem * item = nullptr;
      
      item = dynamic_cast<DDirWatcherDataItem*>(data.getDataItem());
      LOG(INFO) << "Starting to process incoming data item.";
      addOrUpdateDataItem(item);
      LOG(INFO) << "Preparing the objects for saving.";
      if (marshaller->prepare(buffer)) {
         LOG(INFO) << "Saving the objects.";
         std::string fileName = myNode.getOutputFileName() + myNode.getConfigItemValue("marshal");
         bool success = marshaller->save(fileName);
         if (!success) {
            LOG(WARNING) << "Saving the objects failed.";
         }
      } else {
         LOG(WARNING) << "Preparing the objects failed.";
      }
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
   


} // namespace
