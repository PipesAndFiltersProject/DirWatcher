//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//

#include <OHARBaseLayer/DataItem.h>


class DDirWatcherDataItem : public OHARBase::DataItem {
   
public:
   DDirWatcherDataItem();
   virtual ~DDirWatcherDataItem();

   void setChangedItemName(const std::string & name);
   void setChangeEvents(std::vector<std::string> & events);
   
   const std::string & getChangedItemName() const;
   const std::vector<std::string> getChangeEvents() const;
   
private:

   std::string changedItem;
   std::vector<std::string> changeEvents;
   const static std::string TAG;
};
