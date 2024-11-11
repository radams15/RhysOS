#!/usr/bin/perl

use warnings;
use strict;

use File::Basename;
use File::Path qw/make_path/;
use File::Find::Rule;

use POSIX qw/ceil/;

use Data::Dumper;

use Config::Simple;

chomp(my $tag = `git describe --tags`);

my $ASM = 'nasm';
my $CC = 'ia16-elf-gcc -fno-inline -nostdlib -ffreestanding -march=i8086 -mtune=i8086 -fleading-underscore -DRHYSOS';
my $CXX = 'ia16-elf-g++ -fno-inline -nostdlib -ffreestanding -march=i8086 -mtune=i8086 -fleading-underscore -fno-unwind-tables -fno-rtti -fno-exceptions -DRHYSOS';
my $LD = 'ia16-elf-ld';

my $KERNEL_CODE_SEGMENT = '0x2000';
my $KERNEL_DATA_SEGMENT = '0x3000';

my $BOOT2_SEG = '0x9000';
my $STACK_SEG = $BOOT2_SEG;

my $HEAP_ADDR = '0x5000';

my $FLOPPY_SECTORS = 2880; # 1.44M floppy

my $KERNEL_FLAGS = "-Wall -DRELEASE_VERSION=\"$tag\" -DSTACK_SEG=$STACK_SEG -DBOOT2_SEG=$BOOT2_SEG -DHEAP_ADDRESS=$HEAP_ADDR -DKERNEL_CODE_SEGMENT=$KERNEL_CODE_SEGMENT -DKERNEL_DATA_SEGMENT=$KERNEL_DATA_SEGMENT";
my $PROGRAM_FLAGS = "-Wall -DKERNEL_CODE_SEGMENT=$KERNEL_CODE_SEGMENT -DKERNEL_DATA_SEGMENT=$KERNEL_DATA_SEGMENT";

my $KERNEL_NASM_FLAGS = '-W-gnu-elf-extensions';

sub run {
	my ($cmd) = @_;
	
	print "$cmd\n";
	my $ret = system($cmd);

	die $ret if $ret;

	$ret;
}

sub find {
	my ($rule) = @_;
	
	File::Find::Rule->name(basename($rule))->in(dirname($rule));
}

sub bootloader {
	make_path("build") if !(-e 'build/');
	
	&run("$ASM -fbin bootloader/boot.nasm $KERNEL_FLAGS -Ibootloader -o build/boot.bin");

	&run("$CC $KERNEL_FLAGS -c bootloader/boot2.c -o build/boot2.o");
	&run("$ASM -felf $KERNEL_FLAGS bootloader/boot2.nasm -o build/boot2_asm.o");
	&run("$LD -T bootloader/link.ld -o build/boot2.bin -d build/boot2.o build/boot2_asm.o");
	
	"build/boot.bin", "build/boot2.bin";
}

sub kernel {
	my @objs;
	
	for my $c_file (&find('kernel/*.c')) {
		(my $out = $c_file) =~ s:(kernel/.*)\.c:build/$1.o:;
		my $folder = dirname($out);
		make_path($folder) if !(-e $folder);

		&run("$CC -Ikernel/ $KERNEL_FLAGS -c $c_file -o $out");
		(push @objs, $out);
	}

	for my $asm_file (&find('kernel/*.nasm')) {
		(my $out = $asm_file) =~ s:(kernel/.*)\.nasm:build/$1_nasm.o:;
		my $folder = dirname($out);
		make_path($folder) if !(-e $folder);
		
		&run("$ASM -felf $KERNEL_FLAGS $KERNEL_NASM_FLAGS $asm_file -o $out");
		(push @objs, $out) unless $asm_file =~ /kernel\.nasm/;
	}
	
	&run("$LD -Tkernel/link.ld -nostdlib -o build/kernel.elf -d build/kernel/kernel_nasm.o ".(join ' ', @objs));
	
	&run("objcopy -O binary --only-section=.data build/kernel.elf build/kernel.dsb");
	&run("objcopy -O binary --only-section=.text build/kernel.elf build/kernel.csb");

	'build/kernel.dsb', 'build/kernel.csb';
}

sub stdlib {	
	make_path("build/stdlib/real/") if !(-e 'build/stdlib/real');
	my @objs;
	
	for my $c_file (&find('stdlib/real/*.c')) {		
		(my $out = $c_file) =~ s:stdlib/(.*)\.c:build/stdlib/$1.o:;
		&run("$CC -c $c_file -Istdlib/real -o $out $PROGRAM_FLAGS");
		
		push @objs, $out;
	}
	
	for my $c_file (&find('stdlib/real/*.cpp')) {		
		(my $out = $c_file) =~ s:stdlib/(.*)\.cpp:build/stdlib/$1.mm.o:;
		&run("$CXX -c $c_file -Istdlib/real -o $out $PROGRAM_FLAGS");
		
		push @objs, $out;
	}
	
	for my $asm_file (&find('stdlib/real/*.nasm')) {
		(my $out = $asm_file) =~ s:stdlib/(.*)\.nasm:build/stdlib/$1_nasm.o:;
		&run("$ASM -felf $asm_file -Istdlib/real -o $out $PROGRAM_FLAGS -W-gnu-elf-extensions");
		push @objs, $out;
	}
	
	&run("ar -rcs build/stdlib/real/libstdlib.a ".(join ' ', @objs));
	
	"-Lbuild/stdlib/real -lstdlib";
}

sub runtime {
	&run("$ASM -felf runtime/crt0.nasm -Istdlib/real/ -o build/crt0_nasm.o $PROGRAM_FLAGS -W-gnu-elf-extensions");
	&run("$ASM -felf runtime/crt0.pmode.nasm -Istdlib/protected/ -o build/crt0_pmode_nasm.o $PROGRAM_FLAGS -W-gnu-elf-extensions");
	
	&run("$CC -c runtime/crt0.c -Istdlib/real -o build/crt0.o $PROGRAM_FLAGS");
	
	["build/crt0_nasm.o", "build/crt0.o"], ["build/crt0_pmode_nasm.o"];
}

sub padding {
        my ($size, $to) = @_;
        
        (ceil($size/$to)*$to)-$size;
}

sub filesize {
        (stat $_[0])[7];
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
		
		next if $conf->param('ignore');
		
		my $load_script = "$folder/link.ld";
		
		my @objs;
		for my $file ( ($conf->param('main')), $conf->param('files') ) {
			if ($file =~ /\.c$/) {
				(my $out_obj = $file) =~ s:(.*)\.c:$folder/$1.o:;
				&run("$CC -c $program/$file -I$program/ -Istdlib/real -o $out_obj $PROGRAM_FLAGS");
				
				push @objs, $out_obj;
			}
			
			if ($file =~ /\.cpp$/) {
				(my $out_obj = $file) =~ s:(.*)\.cpp:$folder/$1.o:;
				&run("$CXX -c $program/$file -I$program/ -Istdlib/real -o $out_obj $PROGRAM_FLAGS");
				
				push @objs, $out_obj;
			}
			
			if ($file =~ /\.nasm$/) {
				(my $out_obj = $file) =~ s:(.*)\.nasm:$folder/$1.o:;
				
				&run("$ASM -felf $program/$file -I$program/ -Istdlib/real -o $out_obj $PROGRAM_FLAGS");
				
				push @objs, $out_obj;
			}
			
		}

        my @runtime = @{ ($conf->param('protected_mode')? $runtime->[1] : $runtime->[0]) };
        $stdlib = $conf->param('pmode')? '' : $stdlib;
		
		my $out = "$folder/".$conf->param('name');
		
		&run("$LD -o $out.elf -d -Tprograms/link.ld ".($conf->param('runtime')? " @runtime " : "").join(' ', @objs). ($conf->param('stdlib')?" $stdlib":'') );

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
		
		push @programs, $out;
	}
	
	@programs;
}

sub initrd {
        &run("dd if=/dev/zero of=build/initrd.img bs=1M count=1");
        &run("mkdosfs -F12 build/initrd.img");
        
	&run("mcopy -i build/initrd.img ".(join ' ', @_)." ::");
	
	"build/initrd.img";
}

sub img {
	my ($boot1, $boot2, $kernel, $extra_files) = @_;
	
	&run("dd if=/dev/zero of=build/system.img bs=512 count=2880");
        &run("mkdosfs -F12 build/system.img");
	
	&run("mcopy -i build/system.img ".(join ' ', $boot2, @$kernel, @$extra_files)." ::");
	
	&run("dd if=$boot1 of=build/system.img bs=512 count=1 conv=notrunc");
	
	"build/system.img";
}

sub qemu {
	&run("qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M");
}

sub build {
	my ($boot1, $boot2) = &bootloader;
	my @kernel = &kernel;
	my @runtime = &runtime;
	my $stdlib = &stdlib;
	my @programs = &programs(\@runtime, $stdlib);
	&img($boot1, $boot2, \@kernel, [@programs, 'docs/syscalls.md', <root/*>]);
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
