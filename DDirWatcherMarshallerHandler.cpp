//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <g3log/g3log.hpp>

#include <OHARBaseLayer/ProcessorNode.h>
#include <OHARBaseLayer/Package.h>

#include "DDirWatcherMarshallerHandler.hpp"
#include "DDirWatcherDataItem.hpp"

const std::string DDirWatcherMarshallerHandler::TAG{"DirMarshaller"};

DDirWatcherMarshallerHandler::DDirWatcherMarshallerHandler(OHARBase::ProcessorNode & node)
: myNode(node)
{
}

DDirWatcherMarshallerHandler::~DDirWatcherMarshallerHandler() {
}

bool DDirWatcherMarshallerHandler::consume(OHARBase::Package & data) {
   boost::property_tree::ptree tree;
   
   if (load(tree)) {
      DDirWatcherDataItem * item = dynamic_cast<DDirWatcherDataItem*>(data.getDataItem());
      tree.put("events.target", item->getChangedItemName());
      const std::vector<std::string> events = item->getChangeEvents();
      for (const std::string & event : events) {
         tree.put("events.target.eventtype", event);
      }
      if (!save(tree)) {
         // error
      }
   } else {
      // error
   }
   return false;
}

bool DDirWatcherMarshallerHandler::load(boost::property_tree::ptree & tree) {
   const std::string format = myNode.getConfigItemValue("marshal");
   if (format == "xml") {
      boost::property_tree::read_xml(myNode.getDataFileName(), tree);
   } else {
      LOG(WARNING) << TAG << "Format not supported";
      return false;
   }
   return true;
}

bool DDirWatcherMarshallerHandler::save(boost::property_tree::ptree & tree) {
   const std::string format = myNode.getConfigItemValue("marshal");
   if (format == "xml") {
      boost::property_tree::write_xml(myNode.getDataFileName(), tree);
   } else {
      LOG(WARNING) << TAG << "Format not supported";
      return false;
   }
   return true;
}
