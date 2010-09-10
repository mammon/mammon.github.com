#!/usr/bin/perl
# insn_output.pl -- print disassembled listing
#                   NOTE: this ignores SEC, SYM and DATA lines

my $file = shift;
my %insn, $i;
my @xrefs, $xrefstr;
if (! $file ) {
        $file = "-";
}
open( A, $file ) || die "unable to open $file\n";

foreach (<A>) {
	if ( /^INSN|/ ) {
		chomp;
		$i = new_insn( $_ );
		$insn{$$i{addr}} = $i;
	} else {
		; # ignore other lines
	}
}
close (A);


foreach ( sort keys %insn ) {
	$i = $insn{$_};
	$xrefstr = "";
	@xrefs = undef;
	if ($$i{name}) {	
		print "\n$$i{name}:\n";	
	} elsif ( $$i{xrefs} ) {
		# generate fake name
		print "\nloc_$$i{addr}:\n";
		@xrefs = split ':', $$i{xrefs};
		foreach ( @xrefs ) {
			$xrefstr .= " $_";
		}
	}
	print "\t$$i{mnem}\t";
	if ( $$i{src} ) {
		print_op( $$i{src}, $$i{stype} );
		if ( $$i{dest} ) {
			print ", ";
			print_op( $$i{dest}, $$i{dtype} );
			if ( $$i{arg} ) {
				print ", ";
				print_op( $$i{arg}, $$i{atype} );
			}
		}
	}
	print "\t\t(Addr: $$i{addr})";
	if ( $xrefstr ne "" ) {
		print " References:$xrefstr";
	}
	print "\n";
}

sub print_op {
	local($op, $op_type) = @_;
	local $addr, $i;
	if ( $op_type eq "OP_ADDR" && $op =~ /0[xX]([0-9a-fA-F]+)/ ) {
		# replace addresses with their names
		$addr = $1;
		$i = $insn{$addr};
		if ( $$i{name} ) {
			print "$$i{name}";
		} else {
			print "loc_$addr";
		}
	} else {
		print "$op";
	}
	return;
}

# generate new instruction struct from line
sub new_insn {
	local($line) = @_;
	local(%i, $jnk);
	# change this when input file format changes!
	( $jnk, $i{addr}, $i{name}, $i{size}, $i{bytes},
	  $i{mnem}, $i{mtype}, $i{src}, $i{stype},
	  $i{dest}, $i{dtype}, $i{arg}, $i{atype}, $i{xrefs} ) = 
		split '\|', $line;
	return \%i;
}
