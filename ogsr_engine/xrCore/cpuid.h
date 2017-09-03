#ifndef _INC_CPUID
#define _INC_CPUID

//#define _CPU_FEATURE_3DNOW  0x0001
//#define _CPU_FEATURE_MMX    0x0002
#define _CPU_FEATURE_SSE    0x0004
#define _CPU_FEATURE_SSE2   0x0008
#define _CPU_FEATURE_SSE3   0x0010
#define _CPU_FEATURE_SSE41  0x0020
#define _CPU_FEATURE_SSE42  0x0040
#define _CPU_FEATURE_SSSE3  0x0080
#define _CPU_FEATURE_MWAIT  0x0100
#define _CPU_FEATURE_AVX	0x0200
#define _CPU_FEATURE_HTT	0x0400

struct _processor_info {
	string32	v_name;								// vendor name
	string64	model_name;							// Name of model eg. Intel_Pentium_Pro
	int			family;								// family of the processor, eg. Intel_Pentium_Pro is family 6 processor
	int			model;								// model of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
	int			stepping;							// Processor revision number
	int			feature;							// processor Feature ( same as return value).
	int			os_support;							// does OS Support the feature
	unsigned int n_threads;							// number of available threads, both physical and logical
	unsigned int n_cores;							// number of processor cores
	unsigned int affinity_mask;						// recommended affinity mask
													// all processors available to process
													// except 2nd (and upper) logical threads
													// of the same physical core
};

int _cpuid(_processor_info *);

#endif
