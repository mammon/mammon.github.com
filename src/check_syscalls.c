#define MODULE
#define __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/unistd.h>
#include <linux/proc_fs.h>
#include <sys/syscall.h>
/*
SYS_exit
SYS_fork
SYS_read
SYS_write
SYS_open
SYS_close
SYS_creat
SYS_link
SYS_unlink
SYS_execve
SYS_chdir
SYS_time
SYS_chmod
SYS_lchown
SYS_mount
SYS_setuid
SYS_getuid
SYS_ptrace
SYS_alarm
SYS_utime
SYS_access
SYS_nice
SYS_kill
SYS_rename
SYS_mkdir
SYS_rmdir
SYS_getgid
SYS_geteuid
SYS_chroot
SYS_getrusage
SYS_uselib
SYS_ioperm
SYS_klog
SYS_create_module
SYS_init_module
SYS_get_kernel_syms
SYS__sysctl
SYS_query_module
SYS_chown
*/
extern void* sys_call_table[];
//open, write, getuid, chroot, execve, chmod, lchown
int ShowSysCalls( char *buf, char **start, off_t offset, int len, int naught){
	/*len = sprintf( buf, ":%x\n", 
			sys_call_table[SYS_getdents]);
			sys_call_table[SYS_getdents]);
			sys_call_table[SYS_getdents]);
			sys_call_table[SYS_getdents]);
	len += sprintf( buf, ":%x\n", sys_call_table[]);*/
	int x;
	len = 0;
	for (x = 0; x <= SYS_chown; x++) {
		len += sprintf( buf+len, "%x\n", sys_call_table[x] );
	}
	return len;
}
struct proc_dir_entry SysProcFile = {
   0, //low_ino
   11, //name length
   "SysCallAddr", //name
   S_IFREG | S_IRUGO, //mode
   1, 0, 0, // nlinks, owner, group
   0, //size
   NULL,    //operations
   &ShowSysCalls, //read function
};

int init_module(void)
{
	   proc_register(&proc_root, &SysProcFile);
		   return 0;
}
int cleanup_module(void) {
	   proc_unregister(&proc_root, SysProcFile.low_ino);
		   return 0;
}


