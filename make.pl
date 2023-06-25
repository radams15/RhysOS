#!/usr/bin/perl

use warnings;
use strict;

use File::Basename;
use File::Path qw/make_path/;
use File::Find::Rule;

use Data::Dumper;

use Config::Simple;

my $ASM = 'nasm';
my $CC = 'ia16-elf-gcc -ffreestanding '; #-march=i8086 -mtune=i8086
my $LD = 'ia16-elf-ld';

# Must be strings for some reason
my $KERNEL_ADDR = '0x0050';
my $SHELL_ADDR = '0x4500';
my $EXE_ADDR = '0x8000';
my $HEAP_ADDR = '0x9000';
my $STACK_ADDR = '0xfff0';

my $KERNEL_SECTORS = '28';

my $FLOPPY_SECTORS = 2880; # 1.44M floppy

my $KERNEL_FLAGS = "-DHEAP_ADDRESS=$HEAP_ADDR -DEXE_ADDRESS=$EXE_ADDR -DSHELL_ADDRESS=$SHELL_ADDR -DKERNEL_ADDRESS=$KERNEL_ADDR";

sub run {
	my ($cmd) = @_;
	
	print "$cmd\n";
	my $ret = system($cmd);

	die if $ret;

	$ret;
}

sub find {
	my ($rule) = @_;
	
	File::Find::Rule->name(basename($rule))->in(dirname($rule));
}

sub bootloader {
	make_path("build") if !(-e 'build/');
	#&run("$ASM -felf bootloader/boot.nasm -DSTACK_ADDR=$STACK_ADDR -DKERNEL_ADDR=$KERNEL_ADDR -DKERNEL_SECTORS=$KERNEL_SECTORS -Ibootloader -o build/boot.o");
	#&run("$LD -Tbootloader/link.ld build/boot.o --oformat binary -o build/boot.bin");
	
	&run("$ASM -fbin bootloader/boot.nasm -DSTACK_ADDR=$STACK_ADDR -DKERNEL_ADDR=$KERNEL_ADDR -DKERNEL_SECTORS=$KERNEL_SECTORS -Ibootloader -o build/boot.bin");
	
	"build/boot.bin";
}

sub kernel {
	my @objs;
	
	for my $c_file (&find('kernel/*.c')) {
		(my $out = $c_file) =~ s:(kernel/.*)\.c:build/$1.o:;
		my $folder = dirname($out);
		make_path($folder) if !(-e $folder);

		&run("$CC -Ikernel/ -fleading-underscore $KERNEL_FLAGS -c $c_file -o $out");
		(push @objs, $out) unless $c_file =~ /kernel\.c/;
	}

	for my $asm_file (&find('kernel/*.nasm')) {
		(my $out = $asm_file) =~ s:(kernel/.*)\.nasm:build/$1_nasm.o:;
		my $folder = dirname($out);
		make_path($folder) if !(-e $folder);
		
		&run("$ASM -felf $KERNEL_FLAGS $asm_file -o $out");
		(push @objs, $out);
	}
	
	&run("$LD -Tkernel/link.ld -nostdlib -o build/kernel.bin -d build/kernel/kernel.o ".(join ' ', @objs));
	
	"build/kernel.bin";
}

sub stdlib {	
	make_path("build/stdlib/") if !(-e 'build/stdlib/');
	my @objs;
	
	for my $c_file (&find('stdlib/*.c')) {		
		(my $out = $c_file) =~ s:stdlib/(.*)\.c:build/stdlib/$1.o:;
		&run("bcc -ansi -c $c_file -Istdlib/ -o $out");
		
		push @objs, $out;
	}
	
	for my $asm_file (&find('stdlib/*.nasm')) {
		(my $out = $asm_file) =~ s:stdlib/(.*)\.nasm:build/stdlib/$1_nasm.o:;
		&run("$ASM -fas86 $asm_file -Istdlib/ -o $out");
		push @objs, $out;
	}
	
	&run("ar -rcs build/stdlib/libstdlib.a ".(join ' ', @objs));
	
	"-Lbuild/stdlib -lstdlib";
}

sub runtime {
	&run("bcc -ansi -c runtime/crt0.c -Istdlib -o build/crt0.o");
	
	"build/crt0.o";
}

sub programs {
	my ($runtime, $stdlib) = @_;
	
	make_path("build/programs/") if !(-e 'build/programs/');
	
	my @programs;
	
	for my $program (<programs/*>) {
		my $folder = "build/$program";
		make_path($folder) if !(-e $folder);
		
		next if !(-e "$program/config");
		
		my $conf = Config::Simple->import_from("$program/config");
		
		my $load_addr = $conf->param('shell') ? $SHELL_ADDR : $EXE_ADDR;
		
		my @objs;
		for my $file ( ($conf->param('main')), $conf->param('files') ) {
			if ($file =~ /\.c$/) {
				(my $out_obj = $file) =~ s:(.*)\.c:$folder/$1.o:;
				&run("bcc -ansi -c $program/$file -I$program/ -Istdlib -o $out_obj");
				
				push @objs, $out_obj;
			}
			
			if ($file =~ /\.nasm$/) {
				(my $out_obj = $file) =~ s:(.*)\.nasm:$folder/$1.o:;
				
				&run("$ASM -fas86 $program/$file -I$program/ -Istdlib -o $out_obj");
				
				push @objs, $out_obj;
			}
			
		}
		
		my $out = "$folder/".$conf->param('name');
		
		&run("ld86 -o $out -d -T$load_addr $runtime ".join(' ', @objs). ($conf->param('stdlib')?" $stdlib":'') );
		

		open FH, '<', $out;
		my $original = join '', <FH>;
		close FH;
		
		open FH, '>', $out;
		
=pod
		struct Header {
			char magic[2];
			short load_address;
		}
=cut
		
		print FH pack('A2S', 'RZ', eval($load_addr));
		print FH $original;
		close FH;
		
		push @programs, $out;
	}
	
	@programs;
}

sub initrd {
	&run("tar --format=ustar --xform s:^.*/:: -cf build/initrd.tar ".(join ' ', @_));
	
	"build/initrd.tar";
}

sub img {
	my ($bootloader, $kernel, $programs, $extra_files) = @_;
	
	&run("dd if=/dev/zero of=build/system.img bs=512 count=2880");
	
	my $initrd = &initrd($kernel, @$programs, @$extra_files);
	
	&run("dd if=$bootloader of=build/system.img bs=512 count=1 conv=notrunc");
	&run("dd if=$initrd of=build/system.img bs=512 seek=1 conv=notrunc");
	
	"build/system.img";
}

sub qemu {
	&run("qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M");
}

sub build {
	my $bootloader = &bootloader;
	my $kernel = &kernel;
	my $runtime = &runtime;
	my $stdlib = &stdlib;
	my @programs = &programs($runtime, $stdlib);
	&img($bootloader, $kernel, \@programs, ['docs/syscalls.md', 'docs/fs_spec.md']);
}

sub clean {
	&run("rm -rf build");
}

if($ARGV[0] eq 'build') {
	&build;
}

if($ARGV[0] eq 'clean') {
	&clean;
}

if($ARGV[0] eq 'run') {
	&build;
	&qemu;
}
