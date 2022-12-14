#!/usr/bin/perl
#
# checkincludes: 查找/删除多次包含的文件
#

use strict;

sub usage {
	print "Usage: checkincludes.pl [-r]\n";
	print "By default we just warn of duplicates\n";
	print "To remove duplicated includes in place use -r\n";
	exit 1;
}

my $remove = 0;

if ($#ARGV < 0) {
	usage();
}

if ($#ARGV >= 1) {
	if ($ARGV[0] =~ /^-/) {
		if ($ARGV[0] eq "-r") {
			$remove = 1;
			shift;
		} else {
			usage();
		}
	}
}

foreach my $file (@ARGV) {
	open(my $f, '<', $file)
	    or die "Cannot open $file: $!.\n";

	my %includedfiles = ();
	my @file_lines = ();

	while (<$f>) {
		if (m/^\s*#\s*include\s*[<"](\S*)[>"]/o) {
			++$includedfiles{$1};
		}
		push(@file_lines, $_);
	}

	close($f);

	if (!$remove) {
		foreach my $filename (keys %includedfiles) {
			if ($includedfiles{$filename} > 1) {
				print "$file: $filename is included more than once.\n";
			}
		}
		next;
	}

	open($f, '>', $file)
	    or die("Cannot write to $file: $!");

	my $dups = 0;
	foreach (@file_lines) {
		if (m/^\s*#\s*include\s*[<"](\S*)[>"]/o) {
			foreach my $filename (keys %includedfiles) {
				if ($1 eq $filename) {
					if ($includedfiles{$filename} > 1) {
						$includedfiles{$filename}--;
						$dups++;
					} else {
						print {$f} $_;
					}
				}
			}
		} else {
			print {$f} $_;
		}
	}
	if ($dups > 0) {
		print "$file: removed $dups duplicate includes\n";
	}
	close($f);
}
