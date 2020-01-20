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

#include "DDirWatcherDataItem.hpp"

namespace DirWatcher {
   
   
   const std::string DDirWatcherDataItem::TAG{"DWDataItem "};
   
   DDirWatcherDataItem::DDirWatcherDataItem() {
   }
   
   DDirWatcherDataItem::DDirWatcherDataItem(const DDirWatcherDataItem & another)
   : DataItem(another) {
      whoChanged = another.whoChanged;
      changeEvents = another.changeEvents;
   }
   
   DDirWatcherDataItem::~DDirWatcherDataItem() {
   }
   
   void DDirWatcherDataItem::setWhoChanged(const std::vector<std::string> & names) {
      whoChanged = names;
   }
   
   void DDirWatcherDataItem::addWhoChanged(const std::string & name) {
      if (std::find(whoChanged.begin(), whoChanged.end(), name) == whoChanged.end()) {
         whoChanged.push_back(name);
      }
   }
   
   void DDirWatcherDataItem::setChangeEvents(const std::vector<std::pair<std::string,int>> & events) {
      changeEvents = events;
   }
   
   void DDirWatcherDataItem::addChangeEvents(const std::vector<std::pair<std::string,int>> & events) {
      bool found = false;
      for (const std::pair<std::string,int> & item : events) {
         for (std::pair<std::string,int> & localItem : changeEvents) {
            if (item.first == localItem.first) {
               localItem.second += item.second;
               found = true;
               break;
            }
         }
         if (!found) {
            changeEvents.push_back(item);
         }
         found = false;
      }
   }
   
   void DDirWatcherDataItem::addChangeEvent(const std::string & event) {
      std::vector<std::pair<std::string,int>>::iterator iterator = std::find(changeEvents.begin(), changeEvents.end(), std::pair<std::string,int>({event,0}));
      if (iterator == changeEvents.end()) {
         changeEvents.push_back(std::pair<std::string,int>(event, 1));
      } else {
         iterator->second++;
      }
   }
   
   void DDirWatcherDataItem::addChangeEvents(const std::vector<std::string> & events) {
      for (const std::string & event : events) {
         addChangeEvent(event);
      }
   }
   
   const std::vector<std::string> & DDirWatcherDataItem::getWhoChanged() const {
      return whoChanged;
   }
   
   const std::vector<std::pair<std::string,int>> DDirWatcherDataItem::getChangeEvents() const {
      return changeEvents;
   }
   
   bool DDirWatcherDataItem::parse(const std::string & , const std::string & ) {
      return true;
   }
   
   bool DDirWatcherDataItem::addFrom(const OHARBase::DataItem &another) {
      const DDirWatcherDataItem * item = dynamic_cast<const DDirWatcherDataItem*>(&another);
      if (item) {
         const std::vector<std::string> names = item->getWhoChanged();
         for (const std::string & str : names) {
            addWhoChanged(str);
         }
         addChangeEvents(item->getChangeEvents());
         return true;
      }
      return false;
   }
   
   std::unique_ptr<OHARBase::DataItem> DDirWatcherDataItem::clone() const {
      return std::make_unique<DDirWatcherDataItem>(*this);
   }

   
   void to_json(nlohmann::json & j, const DDirWatcherDataItem & data) {
      j = nlohmann::json{{"target", data.getId()}};
      
      const std::vector<std::string> & users = data.getWhoChanged();
      if (users.size() > 0) {
         nlohmann::json j_vec(users);
         j["users"] = j_vec;
      }

      const std::vector<std::pair<std::string,int>> & events = data.getChangeEvents();
      if (events.size() > 0) {
         nlohmann::json j_vec(events);
         j["events"] = j_vec;
      }
   }
   
   void from_json(const nlohmann::json & j, DDirWatcherDataItem & data) {
      LOG(INFO) << "asdf PARSE ";
      data.setId(j.at("target"));
      
      if (j.find("users") != j.end()) {
         std::vector<std::string> users = j["users"];
         data.setWhoChanged(users);
      }
      
      if (j.find("events") != j.end()) {
         std::vector<std::pair<std::string,int>> events = j["events"];
         data.setChangeEvents(events);
      }
   }

   
} // namespace
