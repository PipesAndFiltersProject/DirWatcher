//
//  DDirWatcher.hpp
//  DDirWatcher
//
//  Created by Antti Juustila on 15.2.2018.
//  Copyright (c) 2018 Antti Juustila. All rights reserved.
//

#include <OHARBaseLayer/DataHandler.h>

class DDirWatcherMarshallerHandler : public OHARBase::DataHandler {
   
public:
   DDirWatcherMarshallerHandler(OHARBase::ProcessorNode & node);
   virtual ~DDirWatcherMarshallerHandler();
   
   bool consume(OHARBase::Package & data) override;
   
private:
   bool load(boost::property_tree::ptree & tree);
   bool save(boost::property_tree::ptree & tree);
   
private:
   OHARBase::ProcessorNode & myNode;
   const static std::string TAG;
};
