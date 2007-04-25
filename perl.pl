

use warnings;
use strict;


sub print_truth_table {
	my $regexp_code = shift(@_);
	my $regexp = "\\".$regexp_code;

	print "const bool character_class_for_".$regexp_code." [] = {\n";

	for(my $i=0; $i<256; $i++) {
		print "\t";
		my $char = chr($i);
		if ($char =~ m{$regexp}) {
			print "true";
		} else {
			print "false";
		}
		my $formatted = sprintf("%3d",$i);
		print ", \t// $formatted ";
		if(($i>32) and ($i<=126)) {
			print " $char ";
		}
		if($i==7) {print 'BELL';}
		if($i==9) {print 'TAB';}
		if($i==10) {print 'LINEFEED';}
		if($i==13) {print 'CARRIAGERETURN';}
		if($i==32) {print 'SPACE';}
		

		print "\n";
	}

	print "\t";
	if("\n" =~ m{$regexp}) {
		print "true";
	} else { 
		print "false";
	}

	print "\t// 256 (special case for \\n, which may be two characters) \n};\n\n";

}

foreach my $char qw(w W d D s S) {
	print_truth_table($char);
}

