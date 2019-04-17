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
         using namespace tinyxml2;
         
         document.Clear();
         // It is easier to marshal objects to JSON (below in JSONMarshaller) since
         // JSON support is already build in into DDirWatcherDataItem. So here
         // we need to build the XML structure 'manually'.
         XMLElement * targets = document.NewElement("targets");
         
         for (DDirWatcherDataItem * item : buffer) {
            LOG(INFO) << TAG << "Buffer has " << buffer.size() << " data items.";
            
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
      static const std::string TAG;
   };
   
   const std::string XMLMarshaller::TAG {"XMLMarshaller"};

   
   // MARK: - JSONMarshaller
   class JSONMarshaller : public DDirWatcherMarshallerHandler::MarshallerInterface {
   public:
      virtual bool prepare(std::list<DDirWatcherDataItem*> & buffer) override {
         
         using namespace nlohmann;
         jsonObj.clear();
         
         for (DDirWatcherDataItem * item : buffer) {
            json object = *item;
            jsonObj.push_back(object);
         }
         LOG(INFO) << "JSON structure created ";
         return true;
      }
      
      virtual bool save(const std::string & fileName) override {
         LOG(INFO) << "Marshalling the data items as JSON to " << fileName;
         std::ofstream file;
         file.open(fileName, std::ios::trunc | std::ios::out);
         file << jsonObj;
         file.close();
         return true;
      }
      
   private:
      nlohmann::json jsonObj;
      static const std::string TAG;
   };

   const std::string JSONMarshaller::TAG {"JSONMarshaller"};
   
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
      LOG(INFO) << "Using marshaller " << format;
      if (format == "xml") {
         marshaller = std::unique_ptr<MarshallerInterface>(new XMLMarshaller());
      } else if (format == "json") {
         marshaller = std::unique_ptr<MarshallerInterface>(new JSONMarshaller());
      } else {
         LOG(WARNING) << "Unsupported or not specified marshaller in settings.";
         throw std::runtime_error("No marshaller or unsupported marshaller");
      }
   }
   
   DDirWatcherMarshallerHandler::~DDirWatcherMarshallerHandler() {
      while (buffer.size() > 0) {
         auto item = buffer.back();
         delete item;
         buffer.pop_back();
      }
   }
   
   bool DDirWatcherMarshallerHandler::consume(OHARBase::Package & data) {
      
      DDirWatcherDataItem * item = nullptr;
      
      item = dynamic_cast<DDirWatcherDataItem*>(data.getPayloadObject()->clone().release());
      if (item) {
         LOG(INFO) << "Starting to process incoming data item.";
         addOrUpdateDataItem(item);
         LOG(INFO) << "Preparing the objects for saving.";
         if (marshaller->prepare(buffer)) {
            LOG(INFO) << "Saving the objects.";
            std::string fileName = myNode.getOutputFileName();
            if (fileName[fileName.length()-1] != '.') {
               fileName += ".";
            }
            fileName += myNode.getConfigItemValue("marshal");
            bool success = marshaller->save(fileName);
            if (!success) {
               LOG(WARNING) << "Saving the objects failed.";
            }
         } else {
            LOG(WARNING) << "Preparing the objects failed.";
         }
      }
      return false;
   }
   
   void DDirWatcherMarshallerHandler::addOrUpdateDataItem(DDirWatcherDataItem * item) {
      auto iter = std::find(buffer.begin(), buffer.end(), item);
      if (iter != buffer.end()) {
         LOG(INFO) << "Data item already existed, updating.";
         (*iter)->addFrom(*item);
         delete item;
         item = nullptr;
      } else {
         if (buffer.size() >= bufCapacity) {
            LOG(INFO) << "Buffer full, removing oldest data";
            DDirWatcherDataItem * oldItem = buffer.back();
            delete oldItem;
            buffer.pop_back();
         }
         LOG(INFO) << "Adding new data item to buffer.";
         buffer.push_front(item);
      }
   }
   


} // namespace
