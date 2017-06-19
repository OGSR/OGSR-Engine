typedef	signed		int		s32;
typedef	unsigned	int		u32;
typedef	signed		short	s16;
typedef	unsigned	short	u16;

#define IC __forceinline

extern void DbgOut( const char *format, ... );
extern void ConvOut(FILE* pFile, const char *format, ... );
extern	void	ReadName(char* Name, FILE* pFile);

#define WUS_IDENT	(('S'<<24)+('U'<<16)+('W'<<8)+' ')
#define WUS_VERSION	2


typedef	char	string1024	[1024];
typedef float	Fvector		[3];

IC float	Magnitude (Fvector v0, Fvector v1)
{
	Fvector vr;
	
	vr[0] = v1[0] - v0[0];
	vr[1] = v1[1] - v0[1];
	vr[2] = v1[2] - v0[2];

	return sqrt(vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2]);
}

#define DEF_VECTOR(N,T)				typedef std::vector<T> N;		typedef N::iterator N##_it;