//
//  main.cpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//

#include <iostream>
#include <memory>

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

#include "DDirWatcher.hpp"

int main(int argc, const char * argv[])
{
   std::unique_ptr<g3::LogWorker> logworker{ g3::LogWorker::createLogWorker() };
   auto defaultHandler = logworker->addDefaultLogger(argv[0], "./");
   g3::initializeLogging(logworker.get());

   if (argc < 2) {
      LOG(WARNING) << "DDirWatcher" << "No config file specified! Give config file name as startup parameter.";
   } else {
      try {
         using namespace DirWatcher;
         DDirWatcher watcher(argv[0], argv[1]);
         watcher.run();
      } catch (const std::exception & e) {
         LOG(INFO) << "Something went wrong: " << e.what();
      }
   }
   
   LOG(INFO) << "DDirWatcher" << "--- Node closed ---";
   return 0;
}

