#include <stdio.h>
//#include <binutils/dis-asm.h>
#include <bfd.h>  
// gcc -o elf_bfd elf_bfd.c -lbfd -liberty


//callback, ripped from objdump...
static void hdr_summary(bfd *abfd, asection *section, PTR jnk)
{
	  printf ("%3d %-13s %08lx %08lx  \n", section->index,
             bfd_get_section_name (abfd, section), section->vma,
             (unsigned long) bfd_section_size(abfd, section));
}


int main( int argc, char* argv[]) {
	//bfd_vma dis_addr;
	//disassemble_info info;
	//disassembler_ftype disassemble_fn;
    bfd *abfd;							//BFD object
    char **matching, *target = NULL;
	extern bfd_target *bfd_target_vector[];
	int x;


    if ( argc < 2) {
		printf("Usage: %s filename\n", argv[0]);
		return 1;
	}

    bfd_init ();
    abfd = bfd_openr(argv[1], target);
    if ( abfd == NULL) { printf("Unable to open %s\n", argv[1]); return 1;}
	else  printf("Acquired target: %s\n", argv[1]);
	// -------------------------------------------BFD?
	if (! bfd_check_format_matches(abfd, bfd_object, &matching)){
	  printf("Unrecognized File!\n Supported targets: ");
	  for ( x = 0; bfd_target_vector[x]; x++) {
			bfd_target *p = bfd_target_vector[x];
			printf("%s ", p->name);
	  }
	} else {
		printf("\nMapping %s sections...\n", abfd->filename);
		//iterate through the sections, using hdr_summary as a callback
		bfd_map_over_sections(abfd, hdr_summary, (PTR) NULL);
		printf(".....done\n");
	}

	if (! bfd_close(abfd)) printf("Unable to close BFD!\n");

	return 0;
}
