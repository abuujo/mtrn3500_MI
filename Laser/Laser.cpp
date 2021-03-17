// Includes
#using <System.dll>
#include <conio.h>
#include <SMObject.h>
#include <string>


#include "KeyManager.hpp"

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

	SMObject LSObj(_TEXT("LaserObject"), sizeof(laser_data));			// Laser Data
	LSObj.SMCreate();
	LSObj.SMAccess();

	laser_data* LSObjPtr = (laser_data*)LSObj.pData;

	// Establish a TCP Connection

	int PortNumber = 23000;
	TcpClient^ Client = nullptr;
	array<unsigned char>^ SendData = nullptr;
	array<unsigned char>^ ReadData = nullptr;
	String^ ResponseData = nullptr;

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
	Stream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	// Application Loop
	while (!_kbhit())
	{
		//Check status
		if (PMObjPtr->Shutdown.Flags.laser_mgmt == 1) {
			// kill process
			Stream->Close();
			Client->Close();
			exit(1);
		}

		// Conduct heart beat
		if(PMObjPtr->Heartbeats.Flags.laser_mgmt == 0){
			PMObjPtr->Heartbeats.Flags.laser_mgmt = 1;
			Console::WriteLine(L"Laser Beat : {0}", PMObjPtr->Heartbeats.status);
		} // else check for shut down.

		// Clear Data


		// Read Laser Data
		String^ check_scan = gcnew String("sRN LMDscandata"); // Ask for scan data
		SendData = System::Text::Encoding::ASCII->GetBytes(check_scan);
		Stream->WriteByte(0x02); // STX Signal
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03); // ETX Signal

		// Recieve data
		System::Threading::Thread::Sleep(10);
		Stream->Read(ReadData, 0, ReadData->Length);

		// Check data recieved from the laser
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		array<wchar_t>^ space_delim = {' '};
		array<String^>^ message_split = ResponseData->Split(space_delim);

		// Check first
		String^ check1 = L"LMDscandata";
		if (check1 != message_split[1]) {
			continue;
		}

		// Split into points
		double starting_angle = System::Convert::ToInt32(message_split[23],16);
		double resolution = System::Convert::ToInt32(message_split[24],16)/10000.0;
		int num_points = System::Convert::ToInt32(message_split[25],16);



		// Grab individual points
		array<double> ^data_values = gcnew array<double>(num_points);
		array<double> ^x_values = gcnew array<double>(num_points);
		array<double> ^y_values = gcnew array<double>(num_points);

		//Loop through based on num_points
		for(int point = 0 ; point < num_points ; point++){
			data_values[point] = System::Convert::ToInt32(message_split[26+point],16);
			x_values[point] = data_values[point]*sin(point*resolution * ((180.00) / (3.1415926535)));
			y_values[point] = -data_values[point]*cos(point*resolution * ((180.00) / (3.1415926535)));
		}

		// Store Data in the shared Memory Object (over ride current data)
		LSObjPtr->num_data = num_points;
		LSObjPtr->resolution = resolution;
		LSObjPtr->start_angle = starting_angle;

		// If this doesnt work loop through it.
		for (int i = 0; i < num_points; i++) {
			LSObjPtr->x_coords[i] = (x_values[i] );
			LSObjPtr->y_coords[i] = y_values[i];
		}


		Console::WriteLine("Number of Points -> {0:F3} | Resolution -> {1:F3} | Starting Angle -> {2:F3}", LSObjPtr->num_data, LSObjPtr->resolution, LSObjPtr->start_angle);
		Console::WriteLine("First X -> {0:F3} | First Y -> {1:F3}", LSObjPtr->x_coords[0], LSObjPtr->y_coords[0]);

		System::Threading::Thread::Sleep(100);

	}

	Stream->Close();
	Client->Close();

	//Console::ReadKey();


	return 0;
}