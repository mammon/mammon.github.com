#include <stdio.h>
#include <bfd.h>  
#include <binutils/dis-asm.h> 
//gcc -o elf_hdr elf_hdr.c -lopcodes -lbfd -liberty
////needed since ld cannot find print_insn_i386
extern int print_insn_i386_intel      PARAMS ((bfd_vma, disassemble_info*));


int main( int argc, char* argv[]) {
	bfd_vma dis_addr;
	disassemble_info info;
	disassembler_ftype disassemble_fn;
    bfd *abfd;							//BFD object
	asection *section;
    char **matching, *target = NULL;
	extern bfd_target *bfd_target_vector[];
	int x, bytes;
	bfd_byte *data = NULL;
	bfd_size_type datasize = 0;



    if ( argc < 2) {
		printf("Usage: %s filename\n", argv[0]);
		return 1;
	}

    bfd_init ();
    abfd = bfd_openr(argv[1], target);
    if ( abfd == NULL) { printf("Unable to open %s\n", argv[1]); return 1;}
	else  printf("Acquired target: %s\n", argv[1]);
	if (! bfd_check_format_matches(abfd, bfd_object, &matching)){
	  printf("Unrecognized File!\n Supported targets: ");
	  for ( x = 0; bfd_target_vector[x]; x++) {
			bfd_target *p = bfd_target_vector[x];
			printf("%s ", p->name);
	  }
	} else {
		section = bfd_get_section_by_name(abfd, ".text");
		if ( section ) {
				INIT_DISASSEMBLE_INFO(info, stdout, fprintf);
		        info.flavour = bfd_get_flavour(abfd);
	            info.arch = bfd_get_arch(abfd);
		        info.mach = bfd_get_mach(abfd);
		        info.endian = BFD_ENDIAN_LITTLE;
                datasize = bfd_get_section_size_before_reloc(section);
                data = (bfd_byte *) malloc ((size_t) datasize);
				if ( data == NULL ) return 1;
                info.buffer = data;
                info.buffer_vma = section->vma;
                info.buffer_length = datasize;
                bfd_get_section_contents (abfd, section, data, 0, datasize);


                disassemble_fn = print_insn_i386_intel;
				bytes = 0;
				printf("starting at %8X\n", section->vma + bytes);
	            while ( bytes < section->_raw_size ) {
					  printf("%8X : ", section->vma + bytes); 
					  bytes += (*disassemble_fn) (section->vma + bytes, &info);
					  //here is where you would put additional processing
				      printf("\n");
				}
		}
	}
	if (! bfd_close(abfd)) printf("Unable to close BFD!\n");
	return 0;
}
