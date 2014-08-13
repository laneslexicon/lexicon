#!/usr/bin/perl
#use strict;
use POSIX;
use XML::LibXML;
use English;
#use XML::Parser;
use Encode;
use utf8;
use DBI;
use File::Spec;
use File::Basename;
use File::Find;
use Data::Dumper;
use Getopt::Long;
use Time::HiRes qw( time );
use FileHandle;
my $infile;
my $outfile;
my $safemode=0;
my $remove=0;
binmode STDERR, ":encoding(UTF-8)";
binmode STDOUT, ":encoding(UTF-8)";
###########################################################
# can be used with document to add <span class="ar"></span
# around arabic text
##########################################################
##########################
# test for arabic text
##########################
sub arabic {
  my $word = shift;

  if (! $word ) {
    return 0;
  }
  if ($word =~ /^[\p{InArabic}\p{IsSpace}\p{IsPunct}]+$/) {
    return 1;
  }
  return 0;
}
sub markup_arabic {
  my $fileName = shift;
  my $outf = shift;
  my $fh;
  open(IN,"<:encoding(UTF8)",$fileName) or die "No file $fileName";
  if (! $outf ) {
    open $fh, '>&:encoding(UTF8)', STDOUT or die "Error opening STDOUT: $!";
  }
  else {
  open($fh,">:encoding(UTF8)",$outf);
  }
  my $txt = "";
  my $inArabic = 0;
  my $inSpan = 0;
  while(<IN>) {
    my @words = split " ",$_;

    foreach my $word (@words) {
      $inArabic = arabic($word);
      if ($word =~ /^[\p{IsSpace}\p{IsPunct}]+$/) {
        $inArabic = 0;
      }
#      print STDERR "[$inArabic][$word]\n";
      if ( $inArabic && ! $inSpan) {
          print $fh "<span class=\"ar\">";
          $inSpan = 1;
      }
      else {
        if (! $inArabic && $inSpan) {
          print $fh "</span>";
          $inArabic = 0;
          $inSpan = 0;
        }
      }
      print $fh $word . " ";
    }
    print $fh "\n";
  }
  close IN;
  close $fh;
}
sub remove_markup {
  my $fileName = shift;
  my $outf = shift;
  my $fh;
  open(IN,"<:encoding(UTF8)",$fileName) or die "No file $fileName";
  if (! $outf ) {
    open $fh, '>&:encoding(UTF8)', STDOUT or die "Error opening STDOUT: $!";
  }
  else {
  open($fh,">:encoding(UTF8)",$outf);
  }
  my $txt;
  my $pos;
  my $rec;
  my $spanLength = length "<span class=\"ar\">";
  my $matchCount;
  while(<IN>) {
    $rec = $_;
    $txt = "";
    $matchCount = 0;
    $pos = 0;
    while (/<span class="ar">([^<]+)<\/span>/g) {
#      print sprintf "\n\n\nMatch at %d, length %d\n",length $PREMATCH,length $MATCH;
      $txt .= substr $rec,$pos,length($PREMATCH) - $pos;
#      print "[$pos][$txt]\n";
      $txt .= $1;
      $pos = (length $MATCH) + (length $PREMATCH);
      $matchCount++;
    }
    if ($matchCount  == 0) {
      $txt = $rec;
    }
    else {
      $txt .= substr $rec, $pos;
    }
    print $fh $txt;
  }
  close IN;
  close $fh;
}

GetOptions("in=s" => \$infile,
           "out=s" => \$outfile,
           "clean" => \$remove,
          "safe" => \$safemode);

if (! $infile) {
  print STDERR "No input file specifield\n";
  print STDERR "Usage: --in <filename> --out <filename> --safe\n";
  exit;
}
if (! -e $infile ) {
  print STDERR "Input file $infile not found\n";
  exit;
}

if ((! -e $outfile ) && ($safemode)) {
  print STDERR "Output file exists and safe mode specified: $outfile\n";
  exit;
}
if ($remove) {
  remove_markup($infile,$outfile);
}
else {
markup_arabic($infile,$outfile);
}
