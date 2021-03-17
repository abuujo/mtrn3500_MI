//Compile in a C++ CLR empty project
#using <System.dll>

#include <conio.h>//_kbhit()
#include <SMObject.h>
#include <TLhelp32.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

const int NUM_PROCESSES = 4;

// We can create a list of processes and their meta data
struct ProcessMetaData{
	bool isCritical;
};

ProcessMetaData processes[NUM_PROCESSES] = {
	{true},		// Laser
	{false},	// OpenGL
	{false},		// GPS
	{true}
};



// check if a process is critical
bool isCritical(int process) {
	return processes[process].isCritical;
}


// Check HeartBeat
// flags are: 1 - HeartBeat was changed
// 2 - Shut down all
// 3 - restart and maybe kill
int checkHeartBeat(int process, ProcMgmtMemory* PMObjPtr){
	switch (process){
		case 0:
			if(PMObjPtr->Heartbeats.Flags.laser_mgmt == 1){
				PMObjPtr->Heartbeats.Flags.laser_mgmt = 0;
				return 1;
			}
			else {
				// Wait for a bit and check again
				System::Threading::Thread::Sleep(50);
				if (PMObjPtr->Heartbeats.Flags.laser_mgmt == 1) {
					PMObjPtr->Heartbeats.Flags.laser_mgmt = 0;
					return 1;
				} else if (isCritical(process)) {
					// The process is not responding
					return 2;
				}
				else {
					return 3;
				}
			}
		case 1: // Open GL
			/*if (PMObjPtr->Heartbeats.Flags.laser_mgmt == 1) {
				PMObjPtr->Heartbeats.Flags.laser_mgmt = 0;
				return 1;
			}
			else {
				// Wait for a bit and check again
				System::Threading::Thread::Sleep(50);
				if (PMObjPtr->Heartbeats.Flags.laser_mgmt == 1) {
					PMObjPtr->Heartbeats.Flags.laser_mgmt = 0;
					return 1;
				}
				else if (isCritical(process)) {
					// The process is not responding
					return 2;
				}
				else {
					return 3;
				}
			}*/
		case 2: // GPS
			if (PMObjPtr->Heartbeats.Flags.gps_mgmt == 1) {
				PMObjPtr->Heartbeats.Flags.gps_mgmt = 0;
				return 1;
			}
			else {
				// Wait for a bit and check again
				System::Threading::Thread::Sleep(50);
				if (PMObjPtr->Heartbeats.Flags.gps_mgmt == 1) {
					PMObjPtr->Heartbeats.Flags.gps_mgmt = 0;
					return 1;
				}
				else if (isCritical(process)) {
					// The process is not responding
					return 2;
				}
				else {
					return 3;
				}
			}
		case 3: // Vehicle
			if (PMObjPtr->Heartbeats.Flags.vehicle_mgmt == 1) {
				PMObjPtr->Heartbeats.Flags.vehicle_mgmt = 0;
				return 1;
			}
			else {
				// Wait for a bit and check again
				System::Threading::Thread::Sleep(200);
				if (PMObjPtr->Heartbeats.Flags.vehicle_mgmt == 1) {
					PMObjPtr->Heartbeats.Flags.vehicle_mgmt = 0;
					return 1;
				}
				else if (isCritical(process)) {
					// The process is not responding
					return 2;
				}
				else {
					return 3;
				}
			}
	}
}

void checkProcessesExited(array<Process^>^ ProcessList) {
	for (int i = 0; i < NUM_PROCESSES; i++) {

		if (!ProcessList[i]->HasExited) {
			System::Threading::Thread::Sleep(50);
			if (!ProcessList[i]->HasExited) {
				ProcessList[i]->Kill();
			}
		} 

	}
}

// Main Function
int main()
{
	// INIT shared Memory
	SMObject PMObj(_TEXT("PMObj"), sizeof(ProcMgmtMemory));
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcMgmtMemory* PMObjPtr = (ProcMgmtMemory*)PMObj.pData;
	PMObjPtr->Heartbeats.status = 0xFF;
	PMObjPtr->Shutdown.status = 0x00;

	array<String^>^ names = gcnew array<String^>{ "LaserA.exe", "OpenGL.exe", "GPS.exe" , "VehicleManagement.exe"};

	// Start Processes
	array<Process^>^ ProcessList = gcnew array<Process^>(NUM_PROCESSES);

	for (int i = 0; i < NUM_PROCESSES; i++) {
		ProcessList[i] = gcnew Process();
		ProcessList[i]->StartInfo->FileName = names[i];
		ProcessList[i]->Start();
	}


	System::Threading::Thread::Sleep(200);
	
	// process memory loop:
	// While no key is pressed
	while (!_kbhit()) {
		// Re-set beats to 0
		Console::WriteLine(L"Current Heart Beats: ");
		Console::WriteLine(L"Laser Beat		: {0}", PMObjPtr->Heartbeats.Flags.laser_mgmt);
		Console::WriteLine(L"GPS Beat		: {0}", PMObjPtr->Heartbeats.Flags.gps_mgmt);
		Console::WriteLine(L"Vehicle Beat   : {0}", PMObjPtr->Heartbeats.Flags.vehicle_mgmt);


		for (int i = 0; i < NUM_PROCESSES; i++) {
			int flagHB = checkHeartBeat(i, PMObjPtr); // Just for laser now
			Console::WriteLine(L"Check Flag (Looking for 1)  : {0}", flagHB);

			if (flagHB == 2) {
				PMObjPtr->Shutdown.status = 0xFF;
				checkProcessesExited(ProcessList);
				exit(1);
			}

			if (flagHB == 3) {
				ProcessList[i] = gcnew Process();
				ProcessList[i]->StartInfo->FileName = names[i];
				ProcessList[i]->Start();
			}

		}
		System::Threading::Thread::Sleep(50);
		system("CLS");
	}

	PMObjPtr->Shutdown.status = 0xFF;
	checkProcessesExited(ProcessList);
	return 0;
}

