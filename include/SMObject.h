#ifndef SMOBJECT_H
#define SMOBJECT_H
#include <Windows.h>
#include <tchar.h>
#include <string>


// Shared memory section
struct ModuleFlags {    // Flags for each important module
    unsigned char proc_mgmt : 1,
        gps_mgmt : 1,
        laser_mgmt : 1,
        xbox_mgmt : 1,
        vehicle_mgmt : 1,
        unused : 3;

};

// Overlay on the flags for status (8 bits)
union ExecFlags {
    unsigned char status;
    ModuleFlags Flags;
};

// Shared memory for all modules to monitor heartbeats.
struct ProcMgmtMemory {
    ExecFlags Heartbeats;
    ExecFlags Shutdown;
};

// Structure for GPS Data
#pragma pack(push, 4)
struct GPS{
	unsigned int header;
	unsigned char ignore_first[40];
	double northing;
	double easting;
	double height;
	unsigned char ignore_last[40];
	unsigned int checksum;
};

#pragma pack(pop)

// Structure for Laser Data
struct laser_data {
	int num_data;
	double resolution;
	double start_angle;
	double x_coords[361];
	double y_coords[361];
}; 



class SMObject
{
	HANDLE CreateHandle;
	HANDLE AccessHandle;
	TCHAR *szName;
	int Size;
public:
	void *pData;
	bool SMCreateError;
	bool SMAccessError;
public:
	SMObject();
	SMObject(TCHAR* szname, int size);

	~SMObject();
	int SMCreate();
	int SMAccess();
	void SetSzname(TCHAR* szname);
	void SetSize(int size);
};
#endif


