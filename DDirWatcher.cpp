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
      input.str("close");
   }
   
   DDirWatcher::~DDirWatcher() {
      delete node;
   }
   
   
   void DDirWatcher::run() {
      using namespace OHARBase;
      const static std::string TAG = "DDirWatcher ";
      
      LOG(INFO) << TAG << "Launching " << nodeName;
      LOG(INFO) << TAG << "Config file: " << configurationFile;
      
      node = new ProcessorNode(this);
      LOG(INFO) << "Configuring the node.";
      node->configure(configurationFile);
      if (configurationFile.find("LastNode") != std::string::npos) {
         LOG(INFO) << "Adding Input handler.";
         node->addHandler(new DDirWatcherInputHandler());
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
      
      LOG(INFO) << "Starting the Node";
      std::cout << "Node starting. When you want to shut it down, type \"quit\" or \"shutdown\" (closing all nodes) press <enter>." << std::endl;
      std::cout << " typing \"ping\" <enter> sends a ping message to next node (if any)." << std::endl;
      node->start();
      
      // Start the input reading thread as a lambda function.
      std::thread inputThread ( [this] {
         while (node->isRunning()) {
            int c = std::cin.peek();
            if (c != EOF) {
               LOG(INFO) << "Something in keyboard input buffer, reading it.";
               std::string input;
               std::cin >> input;
               std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
               std::cin.clear();
               LOG(INFO) << "Input was: " << input;
               if (input =="quit" || input == "shutdown" || input == "ping") {
                  LOG(INFO) << "Input was a command so handling it.";
                  stdCoutMutex.lock();
                  std::cout << "Now " << input << "'ing the Node..." << std::endl;
                  stdCoutMutex.unlock();
                  node->handleCommand(input);
                  std::this_thread::sleep_for(std::chrono::milliseconds(500));
               } else if (input == "close") {
                  stdCoutMutex.lock();
                  std::cout << std::endl << "...closing..." << std::endl;
                  stdCoutMutex.unlock();
               } else {
                  std::cout << "Unrecognized command." << std::endl;
               }
            }
            std::cin.clear();
            
         }
      });
      // Continue to check if there is a ping setting.
      std::string pingTimeStr = node->getConfigItemValue("ping");
      int pingTime = 0;
      int pingCounter = 0;
      if (pingTimeStr.length() > 0) {
         pingTime = std::stoi(pingTimeStr);
      }
      bool nodeHasOutput = node->getConfigItemValue("output").length() > 0;
      while (node->isRunning()) {
         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
         if (nodeHasOutput && node->isRunning()) {
            if (pingTime > 0 && ++pingCounter > pingTime) {
               node->handleCommand("ping");
               pingCounter = 0;
            }
         }
      }
      LOG(INFO) << "Deleting node and exiting DirWatcher::run()";
      inputThread.join();
      delete node;
      node = nullptr;
      std::cout << "Node deleted." << std::endl;
   }
   
   void DDirWatcher::NodeEventHappened(EventType e, const std::string & message) {
      stdCoutMutex.lock();
      std::cout << "Event: " << e << " with msg: " << message << std::endl;
      stdCoutMutex.unlock();
      if (e == ProcessorNodeObserver::EventType::ShutDownEvent) {
         LOG(INFO) << "Shutdown event";
         //std::streambuf* orig = std::cin.rdbuf();
         LOG(INFO) << "Redirect cin to streambuf";
         std::cin.rdbuf(input.rdbuf());
         std::cin.sync();
         stdCoutMutex.lock();
         std::cout << "**********************" <<std::endl << "Node has stopped. Press Enter to exit service." << std::endl;
         stdCoutMutex.unlock();
         // TODO: Find out how to force a <enter> to std::cin instead of asking user to do it.
      }
   }
   
} // namespace
