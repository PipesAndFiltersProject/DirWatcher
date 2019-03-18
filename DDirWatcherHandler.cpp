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
#include "DDirWatcherDataItem.hpp"

namespace DirWatcher {
   
   const std::string DDirWatcherHandler::TAG{"WatcherHandler "};
   
   void fswEventCallback(const std::vector<fsw::event> & events, void * ptr) {
      DDirWatcherHandler * handler = static_cast<DDirWatcherHandler*>(ptr);
      if (handler) {
         handler->processEvents(events);
      }
   }
   
   
   DDirWatcherHandler::DDirWatcherHandler(OHARBase::ProcessorNode & node)
   : myNode(node), directoryMonitor(nullptr), running(false)
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
      monitorThread = new std::thread([this] {
         directoryMonitor->start();
         while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
         }
      });
   }
   
   DDirWatcherHandler::~DDirWatcherHandler() {
      running = false;
      directoryMonitor->stop();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      monitorThread->join();
      delete directoryMonitor;
      delete monitorThread;
   }
   
   bool DDirWatcherHandler::consume(OHARBase::Package & data) {
      return false;
   }
   
   void DDirWatcherHandler::processEvents(const std::vector<fsw::event> & events) {
      LOG(INFO) << TAG << "Event from fsw arrived!";
      try {
         std::vector<std::string> eventFlagNames;
         for (const fsw::event & event : events) {
            std::vector<fsw_event_flag> flags = event.get_flags();
            for (fsw_event_flag flag : flags) {
               eventFlagNames.push_back(fsw::event::get_event_flag_name(flag));
            }
            LOG(INFO) << TAG << "Change in " << event.get_path();
            std::unique_ptr<DDirWatcherDataItem> dataItem(new DDirWatcherDataItem);
            dataItem->setId(event.get_path());
            dataItem->addWhoChanged(myNode.getConfigItemValue("user"));
            dataItem->addChangeEvents(eventFlagNames);
            LOG(INFO) << TAG << "Creating a Package";
            OHARBase::Package package;
            package.setType(OHARBase::Package::Type::Data);
            package.setDataItem(dataItem.release());
            LOG(INFO) << TAG << "Passing package to the handlers";
            myNode.passToNextHandlers(this, package);
         }
      } catch (const std::exception & e) {
         myNode.errorInData(e.what()); // Logging done there no need to LOG here.
      }
   }
   
} // namespace
