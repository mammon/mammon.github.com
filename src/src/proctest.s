.globl __module_kernel_version
.globl ThisProcFile
.globl init_module
.globl cleanup_module
.globl ReadThisProcFile

.section	.modinfo
__module_kernel_version:
	.string	"kernel_version=2.2.14-15mdk"

.section	.rodata
strRead:
	.string	"\nThisProcFile went to the market,\nThisProcFile stayed home.\n"
strName:
	.string	"ThisProcFile"

.data
ThisProcFile:
	.value 0
	.value 12
	.long strName
	.value 33060
	.value 1
	.value 0
	.value 0
	.long 0
	.long 0
	.long ReadThisProcFile
	.zero	40

.text
init_module:
	pushl %ebp
	movl %esp,%ebp
	pushl $ThisProcFile
	pushl $proc_root
	call proc_register
	addl $16,%esp
	xorl %eax,%eax
	leave
	ret
cleanup_module:
	pushl %ebp
	movl %esp,%ebp
	movzwl ThisProcFile,%eax
	pushl %eax
	pushl $proc_root
	call proc_unregister
	addl $16,%esp
	xorl %eax,%eax
	leave
	ret
ReadThisProcFile:
	pushl %ebp
	movl %esp,%ebp
	pushl $strRead
	movl 8(%ebp),%eax
	pushl %eax
	call sprintf
	addl $16,%esp
	movl %eax,20(%ebp)
	movl %eax,%edx
	leave
	ret
