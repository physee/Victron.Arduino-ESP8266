/*
  config.h - config the keywords for Victron.DIRECT
*/

#define MPPT_75_10
// MPPT 75 | 10
#ifdef MPPT_75_10

	const byte buffsize = 32;
	const byte value_bytes = 33;
	const byte label_bytes = 9;
	const byte num_keywords = 18;

	char keywords[num_keywords][label_bytes] = {
	"PID",
	"FW",
	"SER#",
	"V",
	"I",
	"VPV",
	"PPV",
	"CS",
	"ERR",
	"LOAD",
	"IL",
	"H19",
	"H20",
	"H21",
	"H22",
	"H23",
	"HSDS",
	"Checksum"
	};
	#define PID 0
	#define FW 1
	#define SER 2	// Offically SER# but # does not play that well as macro
	#define V 3     // ScV
	#define I 4     // ScI
	#define VPV 5   // PVV
	#define PPV 6   // PVI = PVV / VPV
	#define CS 7    // ScS
	#define ERR 8   // ScERR
	#define LOAD 9  // SLs
	#define IL 10   // SLI
	#define H19 11
	#define H20 12
	#define H21 13
	#define H22 14
	#define H23 15
	#define HSDS 16
	#define CHECKSUM 17
#endif