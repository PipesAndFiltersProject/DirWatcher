//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//
#include <iostream>
#include <g3log/g3log.hpp>

#include "DDirWatcher.hpp"
#include "DDirWatcherHandler.hpp"

DDirWatcher::DDirWatcher(const std::string & name, const std::string & config)
: nodeName(name), configurationFile(config), node(nullptr)
{
   
}
DDirWatcher::~DDirWatcher() {
   delete node;
}


void DDirWatcher::run() {
   using namespace OHARBase;
   const static std::string TAG = "DDirWatcher";
   
   LOG(INFO) << TAG << "Launching " << nodeName;
   LOG(INFO) << TAG << "Config file: " << configurationFile;
   
   node = new ProcessorNode(nodeName, this);
   node->configure(configurationFile);
   node->addHandler(new DDirWatcherHandler(*node));
   node->start();
   
   while (node->isRunning()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }
   delete node;
   node = nullptr;
}

void DDirWatcher::NodeEventHappened(EventType e, const std::string & message) {
   std::cout << "Event: " << e << " with msg: " << message << std::endl;
}
