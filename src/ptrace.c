//#include <sys/ptrace.h>
#include <asm/errno.h>
#include <asm/user.h>
#include <stdio.h>
#include <stdlib.h>

   main( argc, argv ) 
      char *argv[];
   {
      int err;
	  extern int errno;
      long thePID = strtol( argv[1], NULL, 10);
	  struct user_regs_struct regs;
      printf("Tracing PID: %s\n", argv[1]);
      err = ptrace( PTRACE_ATTACH, thePID, 0, 0);
      while ( err >= 0 ) {
		if ((ptrace( PTRACE_GETREGS, thePID, NULL, regs)) < 0) break;
		printf("CS:IP %08X:%08X\t SS:SP %08X:%08X FLAGS %08X\n", 
		        regs.cs, regs.eip, regs.ss, regs.esp, regs.eflags);
		printf("EAX %08X \tEBX %08X \tECX %08X \tEDX %08X\n", 
		        regs.eax, regs.ebx, regs.ecx, regs.edx );
 		printf("Press any key ...");
		getchar();	
		err = ptrace( PTRACE_SINGLESTEP, thePID, NULL, NULL);
      }
	  printf("PTRACE failed with error %d! Check /usr/include/asm/errno.h\n", errno);
      ptrace( PTRACE_DETACH, thePID, 0, 0);	    
	  return;
   }
