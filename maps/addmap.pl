use strict;
use POSIX;
use XML::LibXML;
#use XML::Parser;
use Encode;
use utf8;
use HTML::Template;
use charnames ();
use Getopt::Long;
use Data::Dumper;
binmode STDERR, ":utf8";
binmode STDOUT, ":utf8";
my $help;
my $mapname;
my $mapfile;
my $htmlfile;
my $configfile;
my $overwrite=0;
my $outfile;

GetOptions(
           "help"   => \$help,
           "name=s" => \$mapname,
           "map=s" => \$mapfile,
           "config=s" => \$configfile,
           "html=s" => \$htmlfile,
           "overwrite" => \$overwrite,
           "out=s" => \$outfile

          );
if (! $mapname || ! $configfile || ! $mapfile || $help) {
  print STDERR <<EOF;

perl addmap.pl <options>
     --name    <map name>         - required,
     --map     <map file name>    - required, map file e.g buckwalter-16.js
     --html    <html file name>   - optional html file name
     --config  <file name>        - required, the full path to the settings.ini file
     --out     <file name>        - optional, if not entered the original config file will be overwritten
     --overwrite                  - if the map name is already in use overwrite otherwise cancel
EOF
  exit 0;
}
if (! -e $mapfile ) {
  print STDERR "Map file not found: $mapfile\n";
  exit 0;
}
if (! -e $configfile ) {
  print STDERR "Config file not found: $configfile\n";
  exit 0;
}
if ($htmlfile && (! -e $htmlfile )) {
  print STDERR "HTML file not found: $htmlfile\n";
  exit 0;
}
open IN, "<$configfile";
binmode IN, ":encoding(UTF-8)";
my @inifile;
my %mf;
my $mapstart = -1;
my $mapend = -1;
my $line = 0;
while(<IN>) {
  chomp;
  push @inifile,$_;
  if (($mapend == -1) && ($mapstart >= 0)) {
    if (/^\[/) {
      $mapend = $line;
    }
  }
  if (/^\[Maps\]$/) {
    $mapstart = $line + 1;
  }
  $line++;
}
close IN;
print "start $mapstart,end $mapend\n";
my @map;
my $skip;
for(my $i=$mapstart; $i < $mapend;$i++) {
  $skip = 0;
#  print sprintf "%d %s\n",$i,$inifile[$i];
  if ($inifile[$i] =~ /^$mapname/) {
    if (! $overwrite ) {
      print STDERR "Duplicate map name $mapname\n";
      exit 0;
    }
    else {
      $skip = 1;
    }
  }
  $skip = 1 if ($inifile[$i] =~ /^\s+$/);
  push @map, $inifile[$i] unless $skip;
}
push @map, sprintf "%s\\file=%s",$mapname,$mapfile;
if ($htmlfile) {
  push @map, sprintf "%s\\help=%s",$mapname,$htmlfile;
}
#print "New\n";
#print join "\n",@map;
my $fh;
my $filename;
if ($outfile) {
  $filename = $outfile;
}
else {
  $filename = $configfile;
}
open $fh, ">$filename" or die "Unable to open output file $filename : $@\n";
binmode $fh,":encoding(UTF-8)";

for(my $i=0;$i < $mapstart;$i++) {
  print $fh sprintf "%s\n",$inifile[$i];
}
foreach my $t (@map) {
  print $fh sprintf "%s\n",$t

}
my $max = scalar(@inifile);
for(my $i=$mapend;$i < $max;$i++) {
  print $fh sprintf "%s\n",$inifile[$i];
}
close $fh;

print STDERR "New configuration is $filename\n";
