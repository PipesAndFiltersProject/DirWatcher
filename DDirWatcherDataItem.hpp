//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#pragma once

#include <OHARBaseLayer/DataItem.h>

#include <nlohmann/json.hpp>

namespace DirWatcher {
   
   
   class DDirWatcherDataItem : public OHARBase::DataItem {
      
   public:
      DDirWatcherDataItem();
      DDirWatcherDataItem(const DDirWatcherDataItem & another);
      virtual ~DDirWatcherDataItem();
      
      bool parse(const std::string &, const std::string &) override;
      bool addFrom(const OHARBase::DataItem &another) override;
      std::unique_ptr<OHARBase::DataItem> clone() const override;
      
      void setWhoChanged(const std::vector<std::string> & names);
      void addWhoChanged(const std::string & name);
      void setChangeEvents(const std::vector<std::pair<std::string,int>> & events);
      void addChangeEvents(const std::vector<std::pair<std::string,int>> & events);
      void addChangeEvents(const std::vector<std::string> & events);
      void addChangeEvent(const std::string & event);
      
      const std::vector<std::string> & getWhoChanged() const;
      const std::vector<std::pair<std::string,int>> getChangeEvents() const;
      
   private:
      
      std::vector<std::string> whoChanged;
      std::vector<std::pair<std::string,int>> changeEvents;
      const static std::string TAG;
   };
   
   void to_json(nlohmann::json & j, const DDirWatcherDataItem & data);
   void from_json(const nlohmann::json & j, DDirWatcherDataItem & data);
   
} // namespace
