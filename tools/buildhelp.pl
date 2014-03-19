#!/usr/bin/perl
use File::Spec;
use File::Temp qw(tempfile);
use Encode;
use utf8;
my $homeDir  = "/home/andrewsg/qt5projects/LanesLexicon";
my $docsDir = "docs";
my $helpDir = "help";
my @help = (
            {
            source => "lane/preface.md",
            output => "preface.html"
            },
            {
            source => "user-guide/using.md",
            output => "user-guide/using.html"
            },
            {
            source => "user-guide/configuring.md",
            output => "user-guide/configuring.html"
            }
            );


my $header = "<html><head><meta charset=\"utf-8\"></meta></head><body>";
my $footer = "</body></html>";

binmode STDERR, ":utf8";
binmode STDOUT, ":utf8";

foreach my $h (@help) {
  print $h->{source} . "\n";
  my $infile = File::Spec->catfile(@{[$homeDir,$docsDir]},$h->{source});
#  print "$infile\n";
  my $outfile = File::Spec->catfile(@{[$homeDir,$helpDir]},$h->{output});
  if (-e $infile) {
     my $cmd = "markdown $infile";
    my $html = `$cmd`;
#     print decode("UTF8",$html);
    open(OUT,">:encoding(UTF8)",$outfile);
    print OUT $header;
    print OUT decode("UTF-8",$html);
    print OUT $footer;
    close OUT;
  }
  else {
    print STDERR "No such file $infile\n";
  }
}
my $projFile = File::Spec->catfile(@{[$homeDir,$helpDir]},"lexicon.qhcp");
my $helpFile = File::Spec->catfile(@{[$homeDir,$helpDir]},"lanedocs.qhc");
my $cmd = "qcollectiongenerator $projFile -o $helpFile";
`$cmd`
