#!/usr/bin/perl
# insn_xref.pl -- generate xrefs for data from int_code.pl
# NOTE: this changes the file format to
# INSN|addr|name|size|bytes|mem|mtyp|src|styp|dest|dtype|arg|atyp|xrefs
# DATA|addr|xrefs|hex|ascii

my $file = shift;
my $insn, $prev_insn, $head;
my $data, @data_addr;
my %xrefs;

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
	} elsif ( /^DATA/ ) {
		chomp;
		$data = new_data( $_ );
		push @data_addr, $data
	} else {
		print;
	}
}
close (A);


# new version of while (insn) loop
$insn = $head;
while ( $insn ) {
	 gen_xrefs( $insn, $$insn{src}, $$insn{stype} );
	 gen_xrefs( $insn, $$insn{dest}, $$insn{dtype} );
	 gen_xrefs( $insn, $$insn{arg}, $$insn{atype} );
	 $insn = $$insn{next};
}

# print all instructions
$insn = $head;
while ( $insn ) {
	if ( $xrefs{$$insn{addr}} ) {
		chop $xrefs{$$insn{addr}};	# remove trailing colon
	}
	print "INSN|";				# print line type
	print "$$insn{addr}|$$insn{name}|$$insn{size}|$$insn{bytes}|";
	print "$$insn{mnem}|$$insn{mtype}|$$insn{src}|$$insn{stype}|";
	print "$$insn{dest}|$$insn{dtype}|$$insn{arg}|$$insn{atype}|";
	print "$xrefs{$$insn{addr}}\n";
	$insn = $$insn{next};
}

# print all data
foreach ( @data_addr ) {
	print "DATA|$$_{addr}|";
	print "$xrefs{$$_{addr}}|";
	print "$$_{bytes}|$$_{ascii}\n";
}

sub gen_xrefs {
	local($i, $op, $op_type) = @_;
	local $addr, $type;
	if ( $op_type eq "OP_ADDR" and $op =~ /0[xX]([0-9a-fA-F]+)/ ) {
		if ( $$i{mtype} eq "INSN_BRANCH" or
		     $$i{mtype} eq "INSN_BRANCHCC" or
		     $$i{mtype} eq "INSN_CALL" ) {
			$type = "code";
		} else {
			$type = "data";
		}
		$addr = $1;
		$xrefs{$addr} .= "$$i{addr}($type):";
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
	  $i{dest}, $i{dtype}, $i{arg}, $i{atype} ) = 
		split '\|', $line;
	return \%i;
}

# generate new data struct from line
sub new_data {
	local($line) = @_;
	local(%d, $jnk);

	( $jnk, $d{addr}, $d{bytes}, $d{ascii} ) =
		split '\|', $line;
	return \%d;
}
