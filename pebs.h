#ifndef _PEBS_H_
#define _PEBS_H_

#define u64 __u64

struct pebs_v1 {
	u64 flags;
	u64 ip;
	u64 ax;
	u64 bx;
	u64 cx;
	u64 dx;
	u64 si;
	u64 di;
	u64 bp;
	u64 sp;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;
	u64 status;
	u64 dla;
	u64 dse;
	u64 lat;
};

struct pebs_v2 {
	struct pebs_v1 v1;
	u64 eventing_ip;
	u64 tsx;
};

struct pebs_v3 {
	struct pebs_v2 v2;
	u64 tsc;
};

#endif//_PEBS_H_

