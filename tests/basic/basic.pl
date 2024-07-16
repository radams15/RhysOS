#!/usr/bin/perl

use warnings;
use strict;
use utf8;
use 5.030_000;

use constant (
	PRGM_SIZ => 2000,
	NUM_VARS => 64
);

my @prgm;
my @linestack = (0..15);
my $linestackpos = 0;

my @BCMDS = qw/ PRINT INPUT VAR IF GOTO GOSUB RET END /;

sub lnpush { $linestack[$linestackpos++] = $_[0] }
sub lnpop  { $linestack[--$linestackpos] }

sub getbcmd {
	my ($s) = @_;
	
	for my $i (0..$#BCMDS-1) {
		if($BCMDS[$i] eq $s) {
			return $i;
		}
	}
	
	return -1;
}

sub cprint 

sub berror {
	my ($linenum, $e) = @_;
	
	if ($linenum == -1) {
		print STDERR "ERROR: $e\n";
	} else {
		print STDERR "ERROR: AT $linenum: $e\n";
	}
	
	die 1;
}


sub initprgm {
	@prgm = map {[0]} (0..PRGM_SIZ-1);
}




initprgm;
