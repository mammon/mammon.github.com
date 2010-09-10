#!/usr/bin/perl
# insn_list.pl -- demonstration of instruction linked list creation

my $file = shift;
my $insn, $prev_insn, $head;
if (! $file ) {
        $file = "-";
}
open( A, $file ) || die "unable to open $file\n";

foreach (<A>) {
	if ( /^INSN/ ) {
		chomp;
		$insn = new_insn( $_ );

		if ( $prev_insn ) { 
			$$insn{prev} = $prev_insn;
			$$prev_insn{next} = $insn; 
		} else {
			$head = $insn;
		}
		$prev_insn = $insn;
	} else {
		print;
	}
}
close (A);


$insn = $head;
while ( $insn ) {
	# insert code to manipulate list here
	 print "insn $$insn{addr} : ";
	 print "$$insn{mnem}\t$$insn{dest}\t$$insn{src}\n";
	 $insn = $$insn{next};
}

# generate new instruction struct from line
sub new_insn {
	local($line) = @_;
	local(%i, $jnk);
	# change this when input file format changes!
	( $jnk, $i{addr}, $i{name}, $i{size}, $i{bytes},
	  $i{mnem}, $i{mtype}, $i{src}, $i{stype},
	  $i{dest}, $i{dtype}, $i{arg}, $i{atype} ) = 
		split '\|', $line;
	return \%i;
}
