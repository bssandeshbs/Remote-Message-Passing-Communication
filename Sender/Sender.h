#ifndef SENDER_H
#define SENDER_H

/////////////////////////////////////////////////////////////////////
//  Sender.h - Responsible for sending the header and data         //
//				 to Receiver									   //
//																   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2013, Update 2                       //
//  Platform:      ASUS TP300L, Windows 8.1						   //
//  Application:   Message Passing Communication                   //
//  Author:        Sandesh Bellur Shashidhara, Syracuse University //
//                 (315) 751-4826, sbellurs@syr.edu		           //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This class provides the sender functionalities to send the data for both client and server 
* packages, the sender in server side is responsible for provide acknowldegment , in client 
* side its responsible for uploading the data
*
* Required Files:
* ===============
* AppHelpers.h AppHelpers.cpp Socket.h Socket.cpp Message.h Message.cpp Sender.h Sender.cpp
*
* Build Process:
* ==============
* cl /DTEST_SENDER ../ApplicationHelpers/AppHelpers.cpp ../Message/Message.h 
* ../sockets/sockets.cpp Sender.cpp
*
* Public Interface:
* =================
* void clientFileHandling(Socket& si, std::vector<std::string> messageDetails,std::string path)
* - function to upload a binary/text file from client sender to server receiver
*   Input - Socket to send, messageDetails - Vector of client and server address, path to read
*	void clientFileDownloadHandling(Socket& si, std::vector<std::string> messageDetails)
*        - Sends a request to server to request file
*   Input - Socket to send, messageDetails - Vector of client and server address
*	void clientAckowledgement(Socket& si, Message message);
*       - Sends a acknowledgement of file upload from server sender
*   Input - Socket to send, Message - header information
*	void serverClientUpload(Socket& si, Message message, std::string path);
*        - Responsible for sending the file from server to client 
*    Input - Socket, message - header information, path to store
*
* Usage :
* ==================
*  Sender s;
*  s.clientTestFileHandling(si, messageDetails, "input/");
*
* Maintenance History:
* ====================
* ver 1.0 : 05 Apr 14
* - first release
*/
#include "../sockets/sockets.h"
#include "../ApplicationHelpers/AppHelpers.h"
#include <iostream>
#include "../Message/Message.h"
#include "../Display/Display.h"
#include <string>
#include <fstream>

using namespace ApplicationHelpers;

class Sender {
public:
	void clientFileHandling(Socket& si, std::vector<std::string> messageDetails,std::string path);
	void clientFileDownloadHandling(Socket& si, std::vector<std::string> messageDetails);
	void clientAckowledgement(Socket& si, Message message);
	void serverClientUpload(Socket& si, Message message, std::string path);
	void demoMessage(Socket& si, std::vector<std::string> messageDetails);
private:

};

#endif