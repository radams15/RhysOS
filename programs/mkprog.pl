#!/usr/bin/env perl

use warnings;
use strict;

use Cwd qw/ abs_path /;
use File::Path qw/make_path/;
use File::Basename;
use Config::Simple;


my $usage = "Usage: $0 PROGRAM_DIRECTORY OUTPUT_BINARY\n";
sub unset_env_err { sprintf "Error, %s is unset\n", $_[0] }

my $program = abs_path(shift or die $usage);
my $folder = shift or die $usage;
my @runtime = ('../build/crt0.a', '../build/crt0.pmode.a');
my $stdlib = '../build/stdlib/real/libstdlib.a';

my $CC = $ENV{CC} or die unset_env_err "CC";
my $CXX = $ENV{CXX} or die unset_env_err "CXX";
my $LD = $ENV{LD} or die unset_env_err "LD";
my $ASM = $ENV{ASM} or die unset_env_err "ASM";
my $PROGRAM_FLAGS = $ENV{PROGRAM_FLAGS} or die unset_env_err "PROGRAM_FLAGS";

sub run {
	my ($cmd) = @_;

	print "$cmd\n";
	my $ret = system($cmd);

	die $ret if $ret;

	$ret;
}

make_path($folder) if !(-e $folder);

next if !(-e "$program/config");

my $conf = Config::Simple->import_from("$program/config");

next if $conf->param('ignore');

my @objs;
for my $file ( ($conf->param('main')), $conf->param('files') ) {
    if ($file =~ /\.c$/) {
        (my $out_obj = $file) =~ s:(.*)\.c:$folder/$1.o:;
        &run("$CC -c $program/$file -I$program/ -I../stdlib/real -o $out_obj $PROGRAM_FLAGS");

        push @objs, $out_obj;
    }

    if ($file =~ /\.cpp$/) {
        (my $out_obj = $file) =~ s:(.*)\.cpp:$folder/$1.o:;
        &run("$CXX -c $program/$file -I$program/ -I../stdlib/real -o $out_obj $PROGRAM_FLAGS");

        push @objs, $out_obj;
    }

    if ($file =~ /\.nasm$/) {
        (my $out_obj = $file) =~ s:(.*)\.nasm:$folder/$1.o:;

        &run("$ASM -felf $program/$file -I$program/ -I../stdlib/real -o $out_obj");

        push @objs, $out_obj;
    }

}

my @selected_runtime = ($conf->param('protected_mode')? $runtime[1] : $runtime[0]);
$stdlib = $conf->param('pmode')? '' : $stdlib;

my $out = "$folder/".$conf->param('name');

&run("$LD -o $out.elf -d -Tlink.ld ".($conf->param('runtime')? " @selected_runtime " : "").join(' ', @objs). ($conf->param('stdlib')?" $stdlib":'') );

&run("objcopy -O binary $out.elf $out.bin");

open FH, '<', "$out.bin";
binmode FH;
my $text = join '', <FH>;
close FH;

open FH, '>', $out;

=pod
struct Header {
    char magic[2];
    char protected_mode;
    char unused_1;
    short unused_2;
    short unused_3;
    short unused_4;
}
=cut

my $header = pack('A2SSSS', 'RZ', !!$conf->param('protected_mode'), 0, 0, 0);
print FH $header;
print FH $text;
close FH;