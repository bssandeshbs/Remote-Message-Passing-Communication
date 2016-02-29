#include "../Display/Display.h"
#include "../Message/Message.h"

/////////////////////////////////////////////////////////////////////
//  Executive.cpp - Displays the Flow of the application     	   //
//																   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2013, Update 2                       //
//  Platform:      ASUS TP300L, Windows 8.1						   //
//  Application:   Message Passing Communication                   //
//  Author:        Sandesh Bellur Shashidhara, Syracuse University //
//                 (315) 751-4826, sbellurs@syr.edu		           //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_EXECUTIVE
int main() {
	Display::displayInstructions();

	Message msg;
	std::string body = "This is a test body";
	msg.setMessage(body.size(), "FILE_UPLOAD", "localhost:8080", "localhost:8090", body);

	std::string res = msg.getMessage();
	Display::showConfirmation("\n\t\tDemonstrating Requirement 4\n");
	Display::showConfirmation(res);

	Display::showConfirmation("\n  press key to exit: ");
	std::cin.get();
}
#endif