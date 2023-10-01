#!/usr/bin/perl

my $spt = 18; # Sectors per track
my $heads = 2;

my $lba = shift or die "Usage: $0 [LBA]";

my $tmp = $lba / $spt;
my $sector = ($lba % $spt) + 1;
my $head = $tmp % $heads;
my $cyl = int($tmp / $heads);

print "Sector $lba = CHS($cyl, $head, $sector)\n";
