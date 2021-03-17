// Includes
#using <System.dll>
#include <conio.h>//_kbhit()
#include <SMObject.h>
#include <string>

// Namespaces
using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int main()
{
	// Shared Memory Objecs
	SMObject PMObj(_TEXT("PMObj"), sizeof(ProcMgmtMemory));				// PMO
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcMgmtMemory* PMObjPtr = (ProcMgmtMemory*)PMObj.pData;

	SMObject GPSObj(_TEXT("GPS"), sizeof(GPS));			// Laser Data
	GPSObj.SMCreate();
	GPSObj.SMAccess();

	GPS* GPSObjPtr = (GPS*)GPSObj.pData;


	// Establish a TCP Connection
	int PortNumber = 24000;
	TcpClient^ Client = nullptr;
	array<unsigned char>^ SendData = nullptr;
	array<unsigned char>^ ReadData = nullptr;
	String^ ResponseData = nullptr;
	int buffer_size = 2048;

	// Create a TCP Connection
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = buffer_size;
	Client->SendBufferSize = 1024;


	// Attempt to authorise connection
	NetworkStream^ Stream = Client->GetStream();
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(buffer_size);


	// Application Loop
	while (!_kbhit())
	{
		//Check status
		if (PMObjPtr->Shutdown.Flags.gps_mgmt == 1) {
			// kill process
			Stream->Close();
			Client->Close();
			exit(1);
		}

		// Conduct heart beat
		if (PMObjPtr->Heartbeats.Flags.gps_mgmt == 0) {
			PMObjPtr->Heartbeats.Flags.gps_mgmt = 1;
		} // else check for shut down.

		// Read GPS Data
		//ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);


		System::Threading::Thread::Sleep(10);
		if (Stream->DataAvailable) {

			Stream->Read(ReadData, 0, ReadData->Length);
			unsigned int header = 0;
			unsigned int Data = 0;
			int i = 0;
			int start;

			do {
				if (i >= 256) {
					break;
				}
				Data = ReadData[i++];
				header = ((header << 8) | Data);
			} while (header != 0xaa44121c);
			start = i - 4;

			// Fill out the GPS object
			unsigned char* byte_ptr = nullptr;
			byte_ptr = (unsigned char*)GPSObjPtr;

			for (int i = start; i < start + sizeof(GPS); i++) {
				*(byte_ptr++) = ReadData[i];
			}

			Console::WriteLine("Northing -> {0:F3} ", GPSObjPtr->northing);
			Console::WriteLine("Easting -> {0:F3} ", GPSObjPtr->easting);
			Console::WriteLine("Height -> {0:F3} ", GPSObjPtr->height);
		}


		System::Threading::Thread::Sleep(30);
	}

	Stream->Close();
	Client->Close();
	//Console::ReadKey();

	return 0;
}