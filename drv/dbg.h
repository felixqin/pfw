#pragma once

#ifdef _DEBUG
	#define DBGPRINT(str)	\
	{						\
		DbgPrint("[%d] : %s\n",PsGetCurrentProcessId(),str);	\
	}

	#define DBGPRINTVAR(var)	\
	{							\
		DbgPrint("[%d] : %s = 0x%08x\n",PsGetCurrentProcessId(),#var,var);		\
	}

	#define DBGPRINTVART(txt,var)	\
	{							\
		DbgPrint("[%d] : %s%s = 0x%08x\n",PsGetCurrentProcessId(),txt,#var,var);	\
	}

	#define DBGPRINTVAR3(var1,var2,var3)	\
	{									\
		DbgPrint("[%d] : %s = 0x%08x, %s = 0x%08x, %s = 0x%08x",		\
				PsGetCurrentProcessId(),#var1,var1,#var2,var2,#var3,var3);			\
	}

#else
	#define DBGPRINT(str)
	#define DBGPRINTVAR(var)
	#define DBGPRINTVART(var)
	#define DBGPRINTVAR3(var)
#endif

