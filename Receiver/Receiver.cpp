#include "Receiver.h"

/////////////////////////////////////////////////////////////////////
//  Receiver.cpp - Receives the header and data from Sender		   //
//																   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2013, Update 2                       //
//  Platform:      ASUS TP300L, Windows 8.1						   //
//  Application:   Message Passing Communication                   //
//  Author:        Sandesh Bellur Shashidhara, Syracuse University //
//                 (315) 751-4826, sbellurs@syr.edu		           //
/////////////////////////////////////////////////////////////////////

//----< New thread spanned starts here to perform the operation >-----------------
void Receiver::operator()(Socket& socket_)
{
	while (true) {
		const int BufLen = 120;
		Socket::byte buffer[BufLen];
		bool ok = socket_.recv(BufLen, buffer);
		if (!ok) { // close the socket if no more data is coming
			return;
		} 
		std::string headerInfo;
		for (int i = 0; i < BufLen; ++i)
			headerInfo += buffer[i];
		Message msg;
		msg.setDetails(&(*headerInfo.begin()));

		// perform operation based on command
		if (msg.getCommand() == "FILE_UPLOAD_CONFIRMATION" || msg.getCommand() == "SERVER_FILE_UPLOAD_REQ"
			|| msg.getCommand() == "FILE_UPLOAD" || msg.getCommand() == "FILE_UPLOAD_SERVER" || 
			msg.getCommand() == "DEMO_HEADER_MESSAGE") {
			determineType(msg.getCommand(), socket_, msg, headerInfo);
		}
		else if (msg.getCommand() == "STOP_RECEIVER_THREAD") {
			stopReceiverThread(socket_, msg);
			break;
		} else if (msg.getCommand() == "STOP_THREAD") {
			stopClientHandlerThread(socket_);
			break;
		}
	} 
}

//----< determine the type of operation based on command >-----------------
void Receiver::determineType(std::string command, Socket& socket_, Message msg, std::string headerInfo) {
	if (command == "FILE_UPLOAD_CONFIRMATION") {
		fileUploadConfirm(socket_, msg);
	}
	else if (command == "SERVER_FILE_UPLOAD_REQ") {
		serverFileUploadReq(socket_, msg, headerInfo);
	}
	else if (command == "FILE_UPLOAD") {		
		int contentLength = msg.getContentLength();
		Socket::byte buffer1[1024];
		std::string fileName;
		bool ok = socket_.recv(contentLength, buffer1);
		for (int i = 0; i < contentLength; ++i)
			fileName += buffer1[i];
		std::string message = "Uploading file " + fileName + " from address : " + msg.getSendAdd();
		Display::showConfirmation(message);
		fileUpload(socket_, msg, headerInfo, fileName);
	}
	else if (command == "FILE_UPLOAD_SERVER") {
		fileUploadServer(socket_, msg);
	}
	else if (command == "DEMO_HEADER_MESSAGE") {
		Display::displayDemoHeader(msg.getFormatedHeader());
	}
}

//----< client receiver showing confirmation >-----------------
void Receiver::fileUploadConfirm(Socket& socket_, Message msg) {
	int contentLength = msg.getContentLength();
	Socket::byte buffer1[1024];
	std::string message;
	bool ok = socket_.recv(contentLength, buffer1);
	for (int i = 0; i < contentLength; ++i)
		message += buffer1[i];
	std::string conf = "\nFile " + message + " uploaded succesfully from location " + path +
		"to server at address " + msg.getRecvAdd();
	Display::showAckowledgement(conf);
}

//----< client request to server to upload file to client receiver(Download) >-----------------
void Receiver::serverFileUploadReq(Socket& socket_, Message msg, std::string headerInfo){
	int contentLength = msg.getContentLength();
	Socket::byte buffer1[1024];
	std::string fileName;
	bool ok = socket_.recv(contentLength, buffer1);
	for (int i = 0; i < contentLength; ++i)
		fileName += buffer1[i];
	std::string fName = headerInfo + fileName;
	q1.enQ(fName);
}

//----< upload  file from server>----------------------------
void Receiver::fileUpload(Socket& socket_, Message msg, std::string headerInfo,std::string fileName) {
	if (uploadFile(socket_, fileName)) {
		std::string fName = headerInfo + fileName;
		q1.enQ(fName);
	}
	else { 
		std::string fName = headerInfo + fileName + "Failed"; // Think of sending the message object
		q1.enQ(fName);
	}
}

//----< upload file from server (download scenario)>----------------------------
void Receiver::fileUploadServer(Socket& socket_, Message msg) {
	int contentLength = msg.getContentLength();
	Socket::byte buffer1[1024];
	std::string fileName;
	bool ok = socket_.recv(contentLength, buffer1);
	for (int i = 0; i < contentLength; ++i)
		fileName += buffer1[i];
	std::string rec = socket_.recvString();
	if (rec == "SUCCESS")  {
		uploadFile(socket_, fileName);
	}
	else {
		std::string test = "Unable to download file " + fileName + " from location "
			+ msg.getSendAdd();
		Display::showConfirmation(test);
	}
}

//----< stop receiver thread >----------------------------
void Receiver::stopReceiverThread(Socket& socket_, Message msg){
	Message m;
	m.setMessage(0, "STOP_SERVER_SENDER",msg.getSendAdd(), msg.getRecvAdd(), "");
	q1.enQ(m.getMessage());
	socket_.shutDown();
	socket_.close();
}

//----< stop client handler thread >----------------------------
void Receiver::stopClientHandlerThread(Socket& socket_) {
	socket_.shutDown();
	socket_.close();
}

//----< reads the data from buffer and saves the file in server >----------------------------
bool Receiver::uploadFile(Socket& socket_, std::string fileName)
{
	std::string tempFinal;
	std::ofstream myfile(path+fileName, std::ios::out | std::ios::binary);
	if (!myfile.good()) return false;
	while (true) {
		const int BufLen = 120;
		Socket::byte buffer[BufLen];
		bool ok;
		ok = socket_.recv(BufLen, buffer);
		std::string temp;
		for (int i = 0; i < BufLen; ++i)
			temp += buffer[i];

		Message msg;
		msg.setDetails(&(*temp.begin()));
		int contentLength = msg.getContentLength();

		const int BufLen1 = contentLength;
		Socket::byte buffer1[1024];
		ok = socket_.recv(BufLen1, buffer1);
		if (socket_ == INVALID_SOCKET || !ok)
			return false;

		for (int i = 0; i < contentLength; i++) {
			myfile << buffer1[i];
		}
		if (contentLength < 1024) {
			return true;
		}
	}
	myfile.close();
}

//----< cosmetic function >----------------------------
struct Cosmetic
{
	~Cosmetic()
	{
		std::cout << "\n  press key to exit: ";
		std::cin.get();
		std::cout << "\n\n";
	}
} aGlobalForCosmeticAction;

//----< demonstration >------------------------------------------------------
#ifdef TEST_RECEIVER
int main()
{
	title("Testing Socket Server", '=');

	try
	{
		Verbose v(true);
		SocketSystem ss;
		SocketListener sl(9080, Socket::IP6);
		Receiver cp("output/");

		// This will start Listener thread for the server
		sl.start(cp);

		Verbose::show("press key to exit: ", always);
		std::cout.flush();
		std::cin.get();
	}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("") + ex.what() + "\n\n");
	}
}
#endif
