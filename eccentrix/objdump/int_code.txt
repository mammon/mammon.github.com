#!/usr/bin/perl
# int_code.pl : Intermediate code generation based on objdump output
# Output Format:
# Code:
# INSN|addr|name|size|hex|mnemonic|type|src|stype|dest|dtype|aux|atype
# Data:
# DATA|addr|hex|ascii
# Section Definition:
# SEC|name|size|addr|file_offset|permissions

my $file = shift;
my $addr, $hex, $mnem, $size;
my $s_type, $d_type, $a_type;
my $ascii, $pa, $perm;
my $name, $section;
my @ops;

if (! $file ) {
	$file = "-";
}
open( A, $file ) || die "unable to open $file\n";

foreach (<A>) {
	# is this a symbol?
	#/^[0-9a-f]{8}\s+				# sym addr
	#([a-z])\s+([A-Z])\s+			# type/scope
	#([*.A-Za-z0-9]+)\s+			# section
	#([0-9a-f]{8})\s*			# offset
	#([A-Za-z_0-9.]+)			# name
	if ( /^[0-9a-f]{8}\s+([a-z])\s+([A-Z])\s+([*.A-Za-z0-9]+)\s+([0-9a-f]{8})\s*([A-Za-z_0-9.]+)/x) {
		$name = $5;
		$addr = $4;
		$section = $3;
		# Only print global ['g'] symbols defined in a section
		# in this file ['F']
		if ( $1 eq 'g' && $2 eq 'F'&& $section =~ /^\..*/ ) {
			print "SYM|$section|$addr|$name\n";
		}

	# is this data?
	# /^([0-9a-fA-F]{8,})\s+		# address
	#(([0-9a-fA-f]{2,}\s{1,2}){1,16})\s*	# 1-16 hex bytes
	#\|([^|]{1,16})\|			# '|'ASCII'|' 
	} elsif ( /^([0-9a-fA-F]{8,})\s+(([0-9a-fA-f]{2,}\s{1,2}){1,16})\s*\|([^|]{1,16})\|/x) {
		$addr = $1;
		$hex = $2;
		$ascii = $4;
		$hex =~ s/\s+/ /g;
		$ascii =~ s/\|/./g;
		print "DATA|$addr|$hex|$ascii\n";
	# Is this an instruction ?
	# /^\s?(0x0)?([0-9a-f]{3,8}):?\s+	# address
	#	(([0-9a-f]{2,}\s)+)\s+		# hex bytes
	#	([a-z]{2,6})\s+			# mnemonic
	#	([^\s].+)			# operands
	} elsif ( /^\s?(0x0)?([0-9a-f]{3,8}):?\s+(([0-9a-f]{2,}\s)+)\s+([a-z]{2,6})\s+([^\s].+)$/x) {
		$addr = $2;
		$hex = $3;
		$mnem = $5;

		@ops = split_ops($6);

		$src = $ops[0];
		$dest = $ops[1];
		$aux = $ops[2];

		$m_type = insn_type( $mnem );
		if ( $src ) {
			$s_type = op_type( \$src );
		}
		if ( $dest ) {
			$d_type = op_type( \$dest );
		}
		if ( $aux ) {
			$a_type = op_type( \$aux );
		}
			
		chop $hex;	# remove trailing ' '
		$size = count_bytes( $hex );
		print "INSN|";			# print line type
		print "$addr|$name|$size|$hex|";
		print "$mnem|$m_type|";
		print "$src|$s_type|$dest|$d_type|$aux|$a_type\n";
		$name = "";	# undefine name
		$s_type = $d_type = $a_type = "";
	# is this a section?
	# /^\s*[0-9]+\s				# section number
	#	([.a-zA-Z_]+)\s+		# name
	#	([0-9a-fA-F]{8,})\s+		# size
	#	([0-9a-fA-F]{8,})\s+		# VMA
	#	[0-9a-fA-F]{8,}\s+		# LMA
	#	([0-9a-fA-F]{8,})\s+		# File Offset
	} elsif ( /^\s*[0-9]+\s([.a-zA-Z_]+)\s+([0-9a-fA-F]{8,})\s+([0-9a-fA-F]{8,})\s+[0-9a-fA-F]{8,}\s+([0-9a-fA-F]{8,})\s+/x) {
		$name = $1;
		$size = $2;
		$addr = $3;
		$pa = $4;

		if ( /LOAD/ ) {
			$perm = "r";
			if ( /CODE/ ) {
				$perm .= "x";
				} else {
				$perm .= "-";
			}
			if ( /READONLY/ ) {
				$perm .= "-";
			} else {
				$perm .= "w";
			}
		} else {
			$perm = "---";
		}
		print "SEC|$name|$size|$addr|$pa|$perm\n";
	} elsif ( /^[0-9a-f]+\s+<([a-zA-Z._0-9]+)>:/) {
		# is this a name? if so, use for next addr
		$name = $1;
	} # else ignore line
}
close (A);


sub insn_in_array {
	my ($insn, $insn_list) = @_;
	my $pattern;

	foreach( @{$insn_list} ) {
		$pattern = "^$_";
		if ( $insn =~ /$pattern/ ) {
			return(1);
		}
	}
	return(0);
}

sub insn_type {
	local($insn) = @_;
	local($insn_type) = "INSN_UNK";
	my @push_insns = ("push");
	my @pop_insns = ("pop");
	my @add_insns = ("add", "inc");
	my @sub_insns = ("sub", "dec", "sbb");
	my @mul_insns = ("mul", "imul", "shl", "sal");
	my @div_insns = ("div", "idiv", "shr", "sar");
	my @rot_insns = ("ror", "rol");
	my @and_insns = ("and");
	my @xor_insns = ("xor");
	my @or_insns = ("or");
	my @jmp_insns = ("jmp", "ljmp");
	my @jcc_insns = ("ja", "jb", "je", "jn", "jo", "jl", "jg", 
			 "js", "jp");
	my @call_insns = ("call");
	my @ret_insns = ("ret");
	my @trap_insns = ("int");
	my @cmp_insns = ("cmp", "cmpl");
	my @test_insns = ("test", "bt");
	my @mov_insns = ("mov", "lea");

	if (insn_in_array($insn, \@jcc_insns) == 1) {
		$insn_type = "INSN_BRANCHCC";
	} elsif ( insn_in_array($insn, \@push_insns) == 1 ) {
		$insn_type = "INSN_PUSH";
	} elsif ( insn_in_array($insn, \@pop_insns) == 1 ) {
		$insn_type = "INSN_POP";
	} elsif ( insn_in_array($insn, \@add_insns) == 1 ) {
		$insn_type = "INSN_ADD";
	} elsif ( insn_in_array($insn, \@sub_insns) == 1 ) {
		$insn_type = "INSN_SUB";
	} elsif ( insn_in_array($insn, \@mul_insns) == 1 ) {
		$insn_type = "INSN_MUL";
	} elsif ( insn_in_array($insn, \@div_insns) == 1 ) {
		$insn_type = "INSN_DIV";
	} elsif ( insn_in_array($insn, \@rot_insns) == 1 ) {
		$insn_type = "INSN_ROT";
	} elsif ( insn_in_array($insn, \@and_insns) == 1 ) {
		$insn_type = "INSN_AND";
	} elsif ( insn_in_array($insn, \@xor_insns) == 1 ) {
		$insn_type = "INSN_XOR";
	} elsif ( insn_in_array($insn, \@or_insns) == 1 ) {
		$insn_type = "INSN_OR";
	} elsif ( insn_in_array($insn, \@jmp_insns) == 1 ) {
		$insn_type = "INSN_BRANCH";
	} elsif ( insn_in_array($insn, \@call_insns) == 1 ) {
		$insn_type = "INSN_CALL";
	} elsif ( insn_in_array($insn, \@ret_insns) == 1 ) {
		$insn_type = "INSN_RET";
	} elsif ( insn_in_array($insn, \@trap_insns) == 1 ) {
		$insn_type = "INSN_TRAP";
	} elsif ( insn_in_array($insn, \@cmp_insns) == 1 ) {
		$insn_type = "INSN_CMP";
	} elsif ( insn_in_array($insn, \@test_insns) == 1 ) {
		$insn_type = "INSN_TEST";
	} elsif ( insn_in_array($insn, \@mov_insns) == 1 ) {
		$insn_type = "INSN_MOV";
	}
	$insn_type;
}

sub op_type {
	local($op) = @_; #passed as reference to enable mods
	local($op_type) = "";

	# strip dereference operator
	if ($$op =~ /^\*(.+)/ ) {
		$$op = $1;
	}
	if ( $$op =~ /^(\%[a-z]{2,}:)?(0x[a-f0-9]+)?\([a-z\%,0-9]+\)/ ){
		# Effective Address, e.g. [ebp-8]
		$op_type = "OP_EADDR";
	} elsif ( $$op =~ /^\%[a-z]{2,3}/ ) {
		# Register, e.g. %eax
		$op_type = "OP_REG";
	} elsif ( $$op =~ /^\$[0-9xXa-f]+/ ) {
		# Immediate value, e.g. $0x1F
		$op_type = "OP_IMM";
	} elsif ( $$op =~ /^0x[0-9a-f]+/ ) {
		# Address, e.g. 0x8048000
		$op_type = "OP_ADDR";
	} elsif ( $$op =~ /^([0-9a-f]+)\s+<[^>]+>/ ) {
		$op_type = "OP_ADDR";
		$$op = "0x$1";
	} elsif ( $$op ne "" )  {
		# Unknown operand type
		$op_type = "OP_UNK";
	} 
	$op_type;
}

sub split_ops { 
	local($opstr) = @_;
	local(@op);

	# /^([^\(]*\([^\)]+\)),\s?		# effective addr
	#	(([a-z0-9\%\$_]+)(,\s?		# any operand
	#	(.+))?)?			# any operand
	if ( $opstr =~ /^([^\(]*\([^\)]+\)),\s?(([a-z0-9\%\$_]+)(,\s?(.+))?)?/x ) {
		$op[0] = $1;
		$op[1] = $3;
		$op[2] = $5;
	# $opstr =~ /^([a-z0-9\%\$_]+),\s?	# any operand
	#		([^\(]*\([^\)]+\))(,\s?	# effective addr
	#		(.+))?			# any operand
	} elsif ( $opstr =~ /^([a-z0-9\%\$_]+),\s?([^\(]*\([^\)]+\))(,\s?(.+))?/x ) {
		$op[0] = $1;
		$op[1] = $2;
		$op[2] = $4;
	} else {
		@op = split ',', $opstr;
	}
	@op;
}

sub count_bytes {
	local(@bytes) = split ' ', $_[0];
	local($len) = $#bytes + 1;
	$len;
}
