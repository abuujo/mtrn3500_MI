// Includes
#using <System.dll>
#include <conio.h>
#include <SMObject.h>
#include <string>
#include <conio.h>
#include <windows.h>
#pragma comment(lib, "user32.lib")

#include "KeyManager.hpp"

// Namespaces
using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int main()
{
	// Shared Memory Objecs 
	SMObject PMObj(_TEXT("PMObj"), sizeof(ProcMgmtMemory));		
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcMgmtMemory* PMObjPtr = (ProcMgmtMemory*)PMObj.pData;

	// Establish a TCP Connection

	int PortNumber = 25000;
	TcpClient^ Client = nullptr;
	array<unsigned char>^ SendData = nullptr;
	array<unsigned char>^ ReadData = nullptr;
	String^ ResponseData = nullptr;

	/// hello

	// Create a TCP Connection
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// Attempt to authorise connection
	NetworkStream^ Stream = Client->GetStream();
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	String^ userID = gcnew String("5017982\n");

	SendData = System::Text::Encoding::ASCII->GetBytes(userID);
	Stream->Write(SendData, 0, SendData->Length);
	System::Threading::Thread::Sleep(10);

	bool switch_command = true;

	// Application Loop
	while (1)
	{
		//Check status
		if (PMObjPtr->Shutdown.Flags.vehicle_mgmt == 1) {
			// kill process
			Stream->Close();
			Client->Close();
			exit(1);
		}

		// Conduct heart beat
		if (PMObjPtr->Heartbeats.Flags.vehicle_mgmt == 0) {
			PMObjPtr->Heartbeats.Flags.vehicle_mgmt = 1;
		} // else check for shut down.

		// Add double conditions
		if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_LEFT)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# -40.000 1.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# -40.000 1.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
		}
		else if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_RIGHT)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# 40.000 1.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# 40.000 1.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
		}
		else if (GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_RIGHT)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# 40.000 -1.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# 40.000 -1.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
		}
		else if (GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_LEFT)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# -40.000 -1.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# -40.000 -1.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ", command);
			}
		}
		else if (GetAsyncKeyState(VK_LEFT)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# -40.000 0.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# -40.000 0.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
		}
		else if (GetAsyncKeyState(VK_UP)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# 0.000 1.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# 0.000 1.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# 40.000 0.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# 40.000 0.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
		}
		else if (GetAsyncKeyState(VK_DOWN)) {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# 0.000 -1.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# 0.000 -1.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
				Console::WriteLine("Command -> {0} ",command );
			}
		}
		
		else {
			if (switch_command == true) {
				switch_command = false;
				String^ command = gcnew String("# 0.000 0.000 0 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
			}
			else {
				switch_command = true;
				String^ command = gcnew String("# 0.000 0.000 1 #");
				SendData = System::Text::Encoding::ASCII->GetBytes(command);
				Stream->Write(SendData, 0, SendData->Length);
				System::Threading::Thread::Sleep(10);
			}
		}
		
		System::Threading::Thread::Sleep(20);

	}

	Stream->Close();
	Client->Close();

	//Console::ReadKey();


	return 0;
}