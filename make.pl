#!/usr/bin/perl

use warnings;
use strict;

use File::Basename;
use File::Path qw/make_path/;
use File::Find::Rule;

use POSIX qw/ceil/;

use Data::Dumper;

use Config::Simple;

my $ASM = 'nasm';
my $CC = 'ia16-elf-gcc -fno-inline -ffreestanding -march=i8086 -mtune=i8086 -fleading-underscore';
my $LD = 'ia16-elf-ld';

# Must be strings for some reason
my $DATA_SEGMENT = '0x3000';

my $SHELL_ADDR = '0x6000';
my $EXE_ADDR = '0x6000';

my $SHELL_SEGMENT = '0x8000';
my $EXE_SEGMENT = '0x5000';

my $STACK_SEGMENT = '0xfff0';

my $HEAP_ADDR = '0x9000';

my $FLOPPY_SECTORS = 2880; # 1.44M floppy

my $KERNEL_FLAGS = "-DSTACK_SEGMENT=$STACK_SEGMENT -DHEAP_ADDRESS=$HEAP_ADDR -DDATA_SEGMENT=$DATA_SEGMENT -DSHELL_SEGMENT=$SHELL_SEGMENT -DEXE_SEGMENT=$EXE_SEGMENT";

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
		(push @objs, $out) unless $c_file =~ /kernel\.c/;
	}

	for my $asm_file (&find('kernel/*.nasm')) {
		(my $out = $asm_file) =~ s:(kernel/.*)\.nasm:build/$1_nasm.o:;
		my $folder = dirname($out);
		make_path($folder) if !(-e $folder);
		
		&run("$ASM -felf $KERNEL_FLAGS $asm_file -o $out");
		(push @objs, $out);
	}
	
	&run("$LD -Tkernel/link.ld -nostdlib -o build/kernel.elf -d build/kernel/kernel.o ".(join ' ', @objs));

	#&run("objcopy -O binary --only-section=.text build/kernel.elf build/sys.txt");
	#&run("objcopy -O binary --only-section=.data build/kernel.elf build/sys.dat");
	
	&run("objcopy -O binary build/kernel.elf build/kernel.bin");

	'build/kernel.bin';
}

sub stdlib {	
	make_path("build/stdlib/") if !(-e 'build/stdlib/');
	my @objs;
	
	for my $c_file (&find('stdlib/*.c')) {		
		(my $out = $c_file) =~ s:stdlib/(.*)\.c:build/stdlib/$1.o:;
		&run("$CC -c $c_file -Istdlib/ -o $out");
		
		push @objs, $out;
	}
	
	for my $asm_file (&find('stdlib/*.nasm')) {
		(my $out = $asm_file) =~ s:stdlib/(.*)\.nasm:build/stdlib/$1_nasm.o:;
		&run("$ASM -felf $asm_file -Istdlib/ -o $out");
		push @objs, $out;
	}
	
	&run("ar -rcs build/stdlib/libstdlib.a ".(join ' ', @objs));
	
	"-Lbuild/stdlib -lstdlib";
}

sub runtime {
	&run("$ASM -felf runtime/crt0.nasm -Istdlib/ -o build/crt0_nasm.o");
	
	&run("$CC -c runtime/crt0.c -Istdlib/ -o build/crt0.o");
	
	"build/crt0_nasm.o", "build/crt0.o";
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
		
		my $load_addr = $conf->param('shell') ? $SHELL_ADDR : $EXE_ADDR;
		my $segment = $conf->param('shell') ? $SHELL_SEGMENT : $EXE_SEGMENT;
		
		my $load_script = "$folder/link.ld";
		
		# Make new linker script with desired address.
		open FH, '<', "programs/link.ld";
		my $load_script_content = join '', <FH>;
		close FH;
		$load_script_content =~ s/ADDRESS/$load_addr/g;
		
		open FH, '>', $load_script;
		print FH $load_script_content;
		close FH;
		
		my @objs;
		for my $file ( ($conf->param('main')), $conf->param('files') ) {
			if ($file =~ /\.c$/) {
				(my $out_obj = $file) =~ s:(.*)\.c:$folder/$1.o:;
				&run("$CC -c $program/$file -I$program/ -Istdlib -o $out_obj");
				
				push @objs, $out_obj;
			}
			
			if ($file =~ /\.nasm$/) {
				(my $out_obj = $file) =~ s:(.*)\.nasm:$folder/$1.o:;
				
				&run("$ASM -felf $program/$file -I$program/ -Istdlib -o $out_obj");
				
				push @objs, $out_obj;
			}
			
		}
		
		my $out = "$folder/".$conf->param('name');
		
		&run("$LD -o $out.elf -d -T$load_script ".($conf->param('stdlib')? " @$runtime " : "").join(' ', @objs). ($conf->param('stdlib')?" $stdlib":'') );

        &run("objcopy -O binary $out.elf $out.bin");

		open FH, '<', "$out.bin";
		binmode FH;
		my $text = join '', <FH>;
		close FH;

		open FH, '>', $out;
		
=pod
		struct Header {
			char magic[2];
			short load_address;
			short text_size;
			short data_size;
		}
=cut
		
		my $header = pack('A2SSSS', 'RZ', eval($load_addr), eval($segment), 0, 0);
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
	
	&run("mcopy -i build/system.img ".(join ' ', $boot2, $kernel, @$extra_files)." ::");
	
	&run("dd if=$boot1 of=build/system.img bs=512 count=1 conv=notrunc");
	
	"build/system.img";
}

sub qemu {
	&run("qemu-system-i386 -machine pc -fda build/system.img -boot a -m 1M");
}

sub build {
	my ($boot1, $boot2) = &bootloader;
	my $kernel = &kernel;
	my @runtime = &runtime;
	my $stdlib = &stdlib;
	my @programs = &programs(\@runtime, $stdlib);
	&img($boot1, $boot2, $kernel, [@programs, 'docs/syscalls.md', <root/*>]);
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
