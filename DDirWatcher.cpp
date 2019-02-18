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
#include "DDirWatcherInputHandler.hpp"
#include "DDirWatcherOutputHandler.hpp"
#include "DDirWatcherMarshallerHandler.hpp"

namespace DirWatcher {
   
   DDirWatcher::DDirWatcher(const std::string & name, const std::string & config)
   : nodeName(name), configurationFile(config), node(nullptr)
   {
      
   }
   
   DDirWatcher::~DDirWatcher() {
      delete node;
   }
   
   
   void DDirWatcher::run() {
      using namespace OHARBase;
      const static std::string TAG = "DDirWatcher ";
      
      LOG(INFO) << TAG << "Launching " << nodeName;
      LOG(INFO) << TAG << "Config file: " << configurationFile;
      
      node = new ProcessorNode(nodeName, this);
      LOG(INFO) << "Configuring the node.";
      node->configure(configurationFile);
      if (configurationFile.find("LastNode") != std::string::npos) {
         LOG(INFO) << "Adding Input handler.";
         node->addHandler(new DDirWatcherInputHandler(*node));
      }
      if (node->getConfigItemValue("target").length() > 0) {
         LOG(INFO) << "Has target dir, adding dir watcher handler.";
         node->addHandler(new DDirWatcherHandler(*node));
      }
      if (configurationFile.find("LastNode") != std::string::npos) {
         LOG(INFO) << "Is last node, adding marshaller handler.";
         node->addHandler(new DDirWatcherMarshallerHandler(*node));
      } else if (configurationFile.find("FirstNode") != std::string::npos) {
         LOG(INFO) << "Is first node, adding output handler.";
         node->addHandler(new DDirWatcherOutputHandler(*node));
      }

      node->start();
      
      std::string pingTimeStr = node->getConfigItemValue("ping");
      int pingTime = 0;
      int pingCounter = 0;
      if (pingTimeStr.length() > 0) {
         pingTime = std::stoi(pingTimeStr);
      }
      while (node->isRunning()) {
         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
         if (pingTime > 0 && ++pingCounter > pingTime) {
            node->handleCommand("ping");
            pingCounter = 0;
         }
      }
      delete node;
      node = nullptr;
   }
   
   void DDirWatcher::NodeEventHappened(EventType e, const std::string & message) {
      std::cout << "Event: " << e << " with msg: " << message << std::endl;
   }
   
} // namespace
