/*  Secure Ptrace() Kernel Module                  ::     code 2001 per mammon_
 *  A very rudimentary anti-anti-debugging tool. When an inane virus tries
 *  to exploit the even more inane ptrace(), or when a trivial software
 *  protection checks the presence of a debugger with the even more trivial
 *  ptrace(PTRACE_TRACEME) trick, you'll need this puppy to set things
 *  straight, and to tell the bastards just what they can and can't do on
 *  YOUR system.
 *    -- Allows only legitimate applications to call ptrace
 *    -- Unauthorized apps receive a successful return value (0) on
 *       PTRACE_ATTACH, PTRACE_DETACH, and PTRACE_TRACEME
 *    -- To compile:
 *          gcc -I/usr/src/linux/include -O3 -Wall -c sec_ptrace.c
 *    -- TODO: restart the stopped process so things like a renamed gdb
 *       do not lock up (low priority: only happens with gdb)
 */
#define __KERNEL__
#define MODULE
#define LINUX

#if CONFIG_MODVERSIONS==1
 #define MODVERSIONS
 #include <linux/modversions.h>
#endif
#include <linux/kernel.h>
#include <linux/module.h>  /* module junk */
#include <linux/init.h>    /* more module junk */
#include <linux/unistd.h>  /* syscall table */
#include <linux/sched.h>   /* task struct, current() */
#include <linux/string.h>  /* in-kernel strncmp */
#include <linux/ptrace.h>  /* for the ptrace types */


/* GoodGuys: List of legitimate names for ptrace users
 * Edit this to add/remove site-specific programs and useless X UIs for 
 * gdb. It is important for NUMBER_GOOD_GUYS to be accurate, natch.  
 *   NOTE: if you set NUMBER_GOOD_GUYS to 0, then the list will not be
 *   scanned and NO-ONE will be allowed to use ptrace(). This is the 
 *   most secure option, since any exploit worth it's salt will be able
 *   to change its task name to one of the "good" ones. In such a case,
 *   you can always rmmod the sec_ptrace module when you need to run a
 *   ptrace()-based program like gdb or strace. */
#define NUMBER_GOOD_GUYS 3
char *ptrace_good_guys[3] = {
   "gdb", "strace", "ltrace"
};

/* Permanency variable. If the module is loaded with 
 *    insmod sec_ptrace.o perm=1
 * then MOD_USE_COUNT is incremented on loading and is never decremented.
 * This causes the kernel module to be resident until thenext reboot; thus
 * even a process with root priveleges cannot unload the module.
 */
int perm = 0;
MODULE_PARM(perm, "i");


/* pointer to original ptrace(), duh... */
asmlinkage int (*old_ptrace)(long request, long pid, long addr, long data);

/* the obligatory extern'ed syscall table */
extern long sys_call_table[];
   
   
asmlinkage int sec_ptrace(long request, long pid, long addr, long data){
   int x;
   char *name = current->comm;

   MOD_INC_USE_COUNT;
   /* check against authorized names */
   for (x = 0; x < NUMBER_GOOD_GUYS; x++ ) {
      if (! strncmp(name, ptrace_good_guys[x], 16) ){
         MOD_DEC_USE_COUNT;
         return((*old_ptrace)(request, pid, addr, data));
      }
   }
   /* did not match: return 0 and print console message */
   printk("Prevented pid %d (%s) from doing ptrace(%ld) on %ld\n", 
           current->pid, name, request, pid ); 
   MOD_DEC_USE_COUNT;
   if (  request == PTRACE_TRACEME || 
         request == PTRACE_ATTACH ||
         request == PTRACE_DETACH )
      return(0); /* return success for basic ptrace() calls */
   return(-EIO); /* the standard ptrace() ret val for peeks, pokes and such */
}

int __init init_sec_ptrace(void){
   EXPORT_NO_SYMBOLS;
   old_ptrace = (int (*)(long request, long pid, long addr, long data))
                (sys_call_table[__NR_ptrace]);
   sys_call_table[__NR_ptrace] = (unsigned long) sec_ptrace;
   if ( perm ) MOD_INC_USE_COUNT;
   return(0);
}
void __exit exit_sec_ptrace(void){
   if ( sys_call_table[__NR_ptrace] != (unsigned long) sec_ptrace )
      printk("Warning: someone hooked ptrace() after us. Reverting.\n");
   sys_call_table[__NR_ptrace] = (unsigned long) old_ptrace;
   return;
}

/* blah blah recognize these functions */
module_init(init_sec_ptrace);
module_exit(exit_sec_ptrace);
