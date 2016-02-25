//
//  NetworkReader.h
//  PipesAndFiltersFramework
//
//  Created by Antti Juustila on 20.9.2013.
//  Copyright (c) 2013 Antti Juustila. All rights reserved.
//

#ifndef __PipesAndFiltersFramework__NetworkReader__
#define __PipesAndFiltersFramework__NetworkReader__

#include <queue>
#include <sys/socket.h>

#include "Networker.h"
#include "Package.h"

namespace OHARBase {
	
	// Forward declaration.
	class NetworkReaderObserver;
	
	/**
	 This class handles receiving of data to this ProcessorNode from another ProcessorNode.
	 Implementation is based on the Networker class which defines the common functionality of
	 reading and writing of data from/to another ProcessorNode.
	 @author Antti Juustila
	 */
	class NetworkReader : public Networker {
	public:
		NetworkReader(const std::string & hostName, NetworkReaderObserver & obs);
		NetworkReader(const std::string & hostName, int portNumber, NetworkReaderObserver & obs);
		~NetworkReader();
		
		virtual void start() override;
		virtual void stop() override;
		
		Package read();
		
		void threadFunc();
		
	private:
		NetworkReader() = delete;
		NetworkReader(const NetworkReader &) = delete;
		const NetworkReader & operator =(const NetworkReader &) = delete;
		
		
	private:
		/** A queue containing the data as Packages, received from the network.
		 As more data could be received as this node could handle at a time, a queue is necessary to hold
		 the data so that the node can handle them without loosing any data. */
		std::queue<Package> msgQueue;
		/** A mutex guards the access to the queue so that many threads do not manipulate the
		 queue simultaneously. */
		std::mutex guard;
		
		/** The observer of the reader. When the reader receives data from the network,
		 it will notify the reader about the data. The observer can then retrieve the data by using
		 read() and handle it. */
		NetworkReaderObserver & observer;
		
		/** Tag for logging. */
		std::string TAG;
		
		/** Utility method for getting the local IP address of the node. This address can then
		 be used to tell other ProcessorNodes in other machines, what is the address of this node.
		 @param buffer The buffer to where the IP address of the node is written to.
		 @param buflen The length of the buffer. */
		void getPrimaryIp(char* buffer, socklen_t buflen);
	};
	
	
} //namespace

#endif /* defined(__PipesAndFiltersFramework__NetworkReader__) */
