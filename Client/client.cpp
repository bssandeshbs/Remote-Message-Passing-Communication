#include "Client.h"
/////////////////////////////////////////////////////////////////////
//  Client.cpp - Enquees the upload request and spaws client and   //
//             and server threads                                  //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2013, Update 2                       //
//  Platform:      ASUS TP300L, Windows 8.1						   //
//  Application:   Message Passing Communication                   //
//  Author:        Sandesh Bellur Shashidhara, Syracuse University //
//                 (315) 751-4826, sbellurs@syr.edu		           //
/////////////////////////////////////////////////////////////////////

//----< wrapper to enquee an Item >-------------------------------------
void Client::enQuee(std::string operation, std::string address, int port, std::string fileName) {
	std::string res = operation + "|" + address + "|" + std::to_string(port) + "|" + fileName + "|";
	q1.enQ(res);
}

//----< wrapper to Dequee an Item >-------------------------------------
std::string Client::dequee() {
	return q1.deQ();
}

//----< Utility function to split the string >-------------------------------------
std::vector<std::string> Client::getDetails(const std::string str, const char delim) {
	typedef std::string::const_iterator iter;
	iter beg = str.begin();
	std::vector<std::string> tokens;
	while (beg != str.end()) {
		iter temp = std::find(beg, str.end(), delim);
		if (beg != str.end())
			tokens.push_back(std::string(beg, temp));
		beg = temp;
		while ((beg != str.end()) && (*beg == delim))
			beg++;
	}
	return tokens;
}

//----< utility function to read files >-------------------------------------
void Client::readFile(std::string path) {
	std::ifstream file(path);
	std::string str;
	while (std::getline(file, str))
	{
		std::vector<std::string> vec = getDetails(str, '|');
		if (vec.size()>3) enQuee(vec[0], vec[1], stoi(vec[2]), vec[3]);
	}
}

//----< start client receiver >-------------------------------------
void Client::startReceiver() {
	try
	{
		Display::titleDisplay("Client Receiver Thread Starting");
		Verbose v(true);
		SocketSystem ss;
		SocketListener sl(port, Socket::IP6);

		Receiver cp(clientPath);
		sl.start(cp);

		std::cout.flush();
		std::cin.get();
	}
	catch (std::exception& ex)
	{
		Display::showConfirmation("Exception caught:");
		Display::showConfirmation(std::string("\n  ") + ex.what() + "\n\n");
	}
}

//----< Start Client Sender >-------------------------------------
void Client::startSender() {
	Display::titleDisplay("Client Sender Thread Starting");
	Sender s; Verbose v(true);
	SocketSystem ss; SocketConnecter si; int prevPort = 0; bool first = false;
	while (true) {
		std::string item = dequee();
		std::vector<std::string> vec = getDetails(item, '|');
		int portNumb = std::stoi(vec[2]);
		try { if (vec[0] == "shutdown") { //showdown client sender and server threads
				Message msg;
				msg.setHeader(0, "STOP_RECEIVER_THREAD", serverAddress + ":"+std::to_string(this->port),
					vec[1] + ":" + vec[2]);
				si.send(120, msg.getHeader());
				::Sleep(600); si.shutDownSend(); break;
			} else {
				if (portNumb != prevPort) {
					if (!first) first = true;
					else {
						Message msg;
						msg.setHeader(0, "STOP_THREAD", serverAddress + ":"+ std::to_string(this->port),
							vec[1] + ":" + vec[2]);
						si.send(120, msg.getHeader());
						::Sleep(600); si.shutDownSend();						
					} while (!si.connect(vec[1], portNumb)){
						Display::showConfirmation("client waiting to connect"); ::Sleep(100);			
					} 
					Display::showConfirmation("Connected to adddress " + vec[1] + " and port " + vec[2]);
					prevPort = portNumb;
				} if (vec[0] == "upload") {
					vec.push_back(serverAddress);
					vec.push_back(std::to_string(this->port));
					s.clientFileHandling(si, vec, clientPath);
				} else if (vec[0] == "download") {
					vec.push_back(serverAddress);
					vec.push_back(std::to_string(this->port));
					s.clientFileDownloadHandling(si, vec);
				} else if (vec[0] == "demoHeader") {
					vec.push_back(serverAddress);
					vec.push_back(std::to_string(this->port));
					s.demoMessage(si, vec);
				} } } catch (std::exception& ex){
			Display::showConfirmation("Exception caught:");
			Display::showConfirmation(std::string("\n  ") + ex.what() + "\n\n");
		}}
}

#ifdef CLIENT_1
//----< Entry point to the client which starts client sender and receiver >----------
int main(int argc, char* argv[]) {

	if (argc < 5){
		Display::showConfirmation("\n  Please provide all the fields like address,port and path and input files \n\n");
		return 0;
	}
	else {
		std::string address = argv[1];
		int port = std::stoi(argv[2]);
		std::string clientPath = argv[3];
		std::string inputFiles = argv[4];

		Display::titleDisplay("CLIENT DETAILS");
		std::string details = "\n Address:" + address + "\tPort:" + argv[2]+
			"\tPath:"+clientPath+"\n\n";

		Display::showConfirmation(details);

		Client client(address, port, clientPath);
		client.readFile(clientPath + inputFiles);
		std::thread receiver(&Client::startReceiver, client);

		std::thread sender(&Client::startSender, client);

		sender.join();
		receiver.join();
	}
}
#endif