//
// Copyright (C) 2010-2011 Eitan Zahavi, The Technion EE Department
// Copyright (C) 2010-2011 Yaniv Ben-Itzhak, The Technion EE Department
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __HNOCS_ECHOAPP_H_
#define __HNOCS_ECHOAPP_H_

#include <omnetpp.h>
#include <NoCs_m.h>
#include <App_m.h>

//
// This demo Application will generates messages with embedded text and send them to Source module
// then when it receives messages from other applications it will echo them
//
class EchoApp: public cSimpleModule {
private:
	// parameters:
	int appId;
	int srcId;
	double genDelay_s;        // the delay to next message
	simtime_t statStartTime;  // when to start collecting stat in sec
	int curVC;                // the VC to send on

	// state:
	int sendCredits;   // number of messages it can send
	cMessage *genMsg; // used to gen next message
	cQueue Q;          // Q of outstanding messages

	// Statistics
	int numGenMsgs;    // number of generated messages
	int numSentMsgs;   // number of sent messages to all destinations
	int numRecvMsgs;   // number of received messages

	// methods
	void sendMsgs();   // send all Q msgs until no credits
	void handleGenMsg();
	void handleRecvMsg(AppTestMsg *msg);
	void handleCreditMsg(NoCCreditMsg *msg);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

public:
    virtual ~EchoApp();
};

#endif
