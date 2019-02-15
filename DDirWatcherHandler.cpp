//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#include <iostream>
#include <g3log/g3log.hpp>

#include <libfswatch/c++/monitor_factory.hpp>
#include <libfswatch/c++/monitor.hpp>
#include <libfswatch/c++/event.hpp>

#include <OHARBaseLayer/ProcessorNode.h>
#include <OHARBaseLayer/Package.h>

#include "DDirWatcherHandler.hpp"

const std::string DDirWatcherHandler::TAG{"DirWatcher"};

void fswEventCallback(const std::vector<fsw::event> & events, void * ptr) {
   DDirWatcherHandler * handler = static_cast<DDirWatcherHandler*>(ptr);
   if (handler) {
      handler->processEvents(events);
   }
}


DDirWatcherHandler::DDirWatcherHandler(OHARBase::ProcessorNode & node)
: myNode(node), directoryMonitor(nullptr)
{
   using namespace fsw;
   userToReport = myNode.getConfigItemValue("user");
   pathToObserve = myNode.getConfigItemValue("target");
   LOG(INFO) << TAG << "Starting to observe user: " << userToReport << " modify directory " << pathToObserve;
   std::vector<std::string> paths;
   paths.push_back(pathToObserve);
   LOG(INFO) << TAG << "Creating a fsw::monitor";
   directoryMonitor = monitor_factory::create_monitor(fsw_monitor_type::system_default_monitor_type,
                                                    paths,
                                                    &fswEventCallback, this);
   directoryMonitor->start();
}

DDirWatcherHandler::~DDirWatcherHandler() {
   directoryMonitor->stop();
   delete directoryMonitor;
}

bool DDirWatcherHandler::consume(OHARBase::Package & data) {
   return DataHandler::consume(data);
}

void DDirWatcherHandler::processEvents(const std::vector<fsw::event> & events) {
   LOG(INFO) << TAG << "Event from fsw arrived!";
   std::vector<std::pair<std::string, std::string>> notifications;
   std::vector<std::string> eventFlagNames;
   for (const fsw::event & event : events) {
      std::vector<fsw_event_flag> flags = event.get_flags();
      for (fsw_event_flag flag : flags) {
         eventFlagNames.push_back(fsw::event::get_event_flag_name(flag));
      }
      LOG(INFO) << TAG << "Change in " << event.get_path();
   }
}
