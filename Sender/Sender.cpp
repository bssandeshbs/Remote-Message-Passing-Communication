#include "Sender.h"

/////////////////////////////////////////////////////////////////////
//  Sender.cpp - Responsible for sending the header and data       //
//				 to Receiver									   //
//																   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2013, Update 2                       //
//  Platform:      ASUS TP300L, Windows 8.1						   //
//  Application:   Message Passing Communication                   //
//  Author:        Sandesh Bellur Shashidhara, Syracuse University //
//                 (315) 751-4826, sbellurs@syr.edu		           //
/////////////////////////////////////////////////////////////////////

//----< performs the uploading of file from client to server >-------------------------------------
void Sender::clientFileHandling(Socket& si, std::vector<std::string> messageDetails,std::string path) 
{	const int BufLen = 1025; 
	char buffer[BufLen];
	std::fstream fin(path+messageDetails[3], std::ios::in | std::ios::binary);
	buffer[BufLen - 1] = '\0';
	if (!fin) { // Show failure if not able to open file
		std::string fileError = "Unable to open file " + messageDetails[3] + " at the location " + messageDetails[4] + ":" + messageDetails[5];
		Display::showConfirmation(fileError);
		return;
	}
	else {
		Message message;
		std::string sendAddr = messageDetails[4] + ":" + messageDetails[5];
		std::string recvAddr = messageDetails[1] + ":" + messageDetails[2];
		message.setMessage(messageDetails[3].length(), "FILE_UPLOAD", sendAddr, recvAddr, messageDetails[3]);
		si.send(message.getSize(), message.getMessage());
		char bufferdata[BufLen]; //let's say read by 1024 char block
		fin.seekg(0, fin.end);
		int totalSize = (unsigned int)fin.tellg(); //get the size of the file
		fin.seekg(0, fin.beg);
		bool flag = false;
		while (true) {
			int bSize = 1024;
			if (totalSize < 1024) {
				bSize = totalSize;
				char bufferdata2[1025];
				fin.read(bufferdata2, bSize);
				bufferdata2[BufLen - 1] = '\0';
				message.setHeader(bSize, "FILE_UPLOAD", sendAddr, recvAddr);
				si.send(120, message.getHeader());
				si.send(bSize, bufferdata2);
				flag = true;
			} else {
				totalSize = totalSize - 1024;
				fin.read(bufferdata, bSize);
				bufferdata[BufLen - 1] = '\0';
				message.setMessage(bSize, "FILE_UPLOAD", sendAddr, recvAddr, bufferdata);
				si.send(120, message.getHeader());
				si.send(bSize, bufferdata);
			}
			if (flag) {
				break;
			} } }
}

//----< Send a request to upload a File from server sender >-------------------------------------
void Sender::clientFileDownloadHandling(Socket& si, std::vector<std::string> messageDetails)
{
	Message message;
	std::string sendAddr = messageDetails[4] + ":" + messageDetails[5];
	std::string recvAddr = messageDetails[1] + ":" + messageDetails[2];
	message.setMessage(messageDetails[3].length(), "SERVER_FILE_UPLOAD_REQ", sendAddr, recvAddr, messageDetails[3]);
	si.send(message.getSize(), message.getMessage());
}

//----< Send a request to upload a File from server sender >-------------------------------------
void Sender::demoMessage(Socket& si, std::vector<std::string> messageDetails)
{
	Message message;
	std::string sendAddr = messageDetails[4] + ":" + messageDetails[5];
	std::string recvAddr = messageDetails[1] + ":" + messageDetails[2];
	message.setMessage(messageDetails[3].length(), "DEMO_HEADER_MESSAGE", sendAddr, recvAddr, messageDetails[3]);
	si.send(message.getSize(), message.getMessage());
}

//----< provides a acknowledgmet after succesfully uploading >-------------------------------------
void Sender::clientAckowledgement(Socket& si, Message message)  {
	si.send(message.getSize(), message.getMessage());
}

//----< uploads the file from server sender to client(Downloading)>--------------------------------
void Sender::serverClientUpload(Socket& si, Message message,std::string path) {
	const int BufLen = 1025; //let's say read by 1024 char block
	char buffer[BufLen];
	std::fstream fin(path + message.getBody(), std::ios::in | std::ios::binary);
	buffer[BufLen - 1] = '\0';
	if (!fin) {
		si.send(message.getSize(), message.getMessage());
		message.setBody("File Not Found");
		si.sendString("FAILURE");
		return;
	} else {	
		std::string sendAddr = message.getSendAdd();
		std::string recvAddr = message.getRecvAdd();
		si.send(message.getSize(), message.getMessage());
		si.sendString("SUCCESS");

		char bufferdata[BufLen]; 
		fin.seekg(0, fin.end);
		int totalSize = (unsigned int)fin.tellg();
		fin.seekg(0, fin.beg);
		bool flag = false;
		while (true) {
			int bSize = 1024;
			if (totalSize < 1024) {   // Send last time if content length is less than 1024
				bSize = totalSize;
				char bufferdata2[1025];
				fin.read(bufferdata2, bSize);
				bufferdata2[BufLen - 1] = '\0';
				message.setHeader(bSize, "FILE_UPLOAD_SERVER", sendAddr, recvAddr);
				si.send(120, message.getHeader());
				si.send(bSize, bufferdata2);
				flag = true;
			} else {
				totalSize = totalSize - 1024; // Subtract from total length and send 1024 
				fin.read(bufferdata, bSize);
				bufferdata[BufLen - 1] = '\0'; // Sender header and then send buffer
 				message.setMessage(bSize, "FILE_UPLOAD_SERVER", sendAddr, recvAddr, bufferdata);
				si.send(120, message.getHeader());
				si.send(bSize, bufferdata);
			}
			if (flag) break;
		}
	}
}

#ifdef TEST_SENDER
//----< Test Stub to test the sender>--------------------------------
int main() {
	title("Message Passing Communication", '=');
	try
	{
		Sender s;
		Verbose v(true);
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 9080))
		{
			Verbose::show("client waiting to connect");
			::Sleep(100);
		}

		title("Starting string test on client");
		std::vector<std::string> messageDetails;
		messageDetails.push_back("upload");
		messageDetails.push_back("localhost");
		messageDetails.push_back("9080");
		messageDetails.push_back("Delay.pdf");
		messageDetails.push_back("localhost");
		messageDetails.push_back("9081");

		s.clientFileHandling(si, messageDetails, "input/");
		si.shutDownSend();
	}
	catch (std::exception& ex)
	{
		Verbose::show("Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
}

#endif
