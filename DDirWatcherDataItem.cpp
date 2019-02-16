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

#include "DDirWatcherDataItem.hpp"

const std::string DDirWatcherDataItem::TAG{"DWDataItem"};


DDirWatcherDataItem::DDirWatcherDataItem()
{
}

DDirWatcherDataItem::~DDirWatcherDataItem() {
}

void DDirWatcherDataItem::setChangedItemName(const std::string & name) {
   changedItem = name;
}

void DDirWatcherDataItem::setChangeEvents(std::vector<std::string> & events) {
   changeEvents = std::move(events);
}

const std::string & DDirWatcherDataItem::getChangedItemName() const {
   return changedItem;
}

const std::vector<std::string> DDirWatcherDataItem::getChangeEvents() const {
   return changeEvents;
}
