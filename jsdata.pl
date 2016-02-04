#!/usr/bin/perl
use JSON;
use File::Spec;
use File::Basename;
use File::Temp qw/ tempfile tempdir /;
use Getopt::Long;
my %kc = ();
my %uc = ();
my %bc = ();                    # base set of characters
my %dc = ();                    # diacritics
my @props;
my @ki;
my $outfile;
my $unicodeMaster = "data/UnicodeData.txt";
my $offset;
my $indir = ".";
my $outdir = File::Spec->tmpdir();
my $inputfile;
my $debugrun = 1;
my $echo = 0;
my $overwrite = 0;
my $help = 0;
my %options = (
               "debug!" => \$debugrun,
               "map=s" => \$inputfile,
               "out=s" => \$outfile,
               "outdir" => \$outdir,
               "unicode=s" => \$unicodeMaster,
               "print!" => \$echo,
               "overwrite!" => \$overwrite,
               "help!" => \$help
              );
&GetOptions(%options);
if ($help) {
  print STDERR <<EOF;
perl jdata.pl <options>

Where options are:

   --map <map file name>       Full path to the map file. Required.
   --out <output file name>    Name of the ouput file. If not supplied the input file name is used with the suffix
                               replaced by 'js' (if present) or with '.js' appended (if not present)
   --outdir <output directory> Where the output file is created. Optional.
                               If not supplied the systems tmp directory is used
   --unicode <file name>       The full path to UnicodeData.txt. If not supplied assumes ../data/UnicodeData.txt
   --overwrite                 Overwrite an existing output file\n";


For example,

perl jsdata.pl --map buckwalter.txt --out buckwalter-1.7.js --unicode UnicodeData.txt

EOF
  exit 1;
}

if (! $inputfile ) {
  print STDERR "No map file name given.\n";
  $help = 1;
}
elsif (! -e $inputfile) {
  print STDERR "No such file: $inputfile\n";
  $help = 1;
}
if (! $outfile ) {
  my @suffixes;
  if ($inputfile =~ /\.(\w+)$/) {
    push @suffixes, $1;
  }
  my ($name,$path,$s) = fileparse($inputfile,@suffixes);
  $outfile = $name . "js";
}

my ($f,$js) = doFile($inputfile);

my $fh;

if (! $f ) {
  $fh = File::Temp->new(DIR => $outdir,SUFFIX => ".js",TEMPLATE => "mappingXXXX",UNLINK => 0);
  $f =  $fh->filename;
} else {
  $f = File::Spec->catfile($outdir,$f);
  if (! $overwrite && -e $f ) {
    print STDERR "Output file $f exists and overwrite not specified\n";
    exit 0;
  }
  open $fh, ">$f" or die "Cannot create output file $f\n";
}
binmode($fh, ":utf8");
print $fh $js;
close $fh;
print STDERR "Created $f\n";
exit 1;
#-------------------------------------------
#
#-------------------------------------------
sub unicode_lookup {
  open(IN,"<$unicodeMaster") or die "No data file";
  local ($k,$v);
  while (<IN>) {
    @a = split ";";
    $k = strip($a[1]);
    $v = hex(strip($a[0]));
    $uc{$k} = $v;
  }
}
# 40  : { type : 2, s : "LEFT PARENTHESIS", t : "DASIA" },
# 41  : { type : 2, s : "RIGHT PARENTHESIS", t : "PSILI" },
# 42  : { type : 3, s : "ASTERISK",t : function() { this.capitalise = 1; } } ,
# 61  : { type : 2, s : "EQUALS SIGN" , t : "PERISPOMENI" },
# 92  : { type : 2, s : "REVERSE SOLIDUS" , t : "VARIA" },
# 97  : { type : 1, s : "LATIN SMALL LETTER A",t : "GREEK SMALL LETTER ALPHA"}
#-------------------------------------------
#
#-------------------------------------------
sub get_props {
  local *FH  = shift;
  my $k;
  my $v;
  #  push @code, "\"properties\" : { ";

  while (<FH>) {
    if (/ENDDATA/) {
      last;
    } else {
      chomp;
      my $t = $_;
      if (($t =~ /;/) && ($t !~ /^;/)) {
	@a = split ";",$t;
        if ($a[0] =~ /NAME/) {
          $k = "name";
          $v = strip($a[1]);
          push @props,sprintf "\"%s\" : \"%s\"",$k,$v;
        }
        if ($a[0] =~ /LANGUAGE/) {
          $k = "language";
          $v = strip($a[1]);
          push @props,sprintf "\"%s\" : \"%s\"",$k,$v;
        }
        if ($a[0] =~ /SCRIPT/) {
          $k = "script";
          $v = strip($a[1]);
          push @props,sprintf "\"%s\" : \"%s\"",$k,$v;
        }
        # if ($a[0] =~ /OUTPUT/) {
        #   $k = "jsfile";
        #   $v = strip($a[1]);
        #   $outfile = $v;
        #   $outfile =~ s/'//g;
        #   $outfile =~ s/"//g;
	#   print STDERR "Output file set to $outfile\n";
        #   push @props,sprintf "\"%s\" : \"%s\"",$k,$v;
        # }
        if ($a[0] =~ /VERSION/) {
          $k = "version";
          $v = strip($a[1]);
          push @props,sprintf "\"%s\" : \"%s\"",$k,$v;
        }
        if ($a[0] =~ /DISCARD/) {
          $k = "discard";
          $v = strip($a[1]);
          push @props,sprintf "\"%s\" : \"%s\"",$k,$v;
        }
      }
    }
  }
  return;
}
#-------------------------------------------
#
#-------------------------------------------
sub gen_keys_of_interest {
  local *FH  = shift;
  local $type = shift;
  my @code;

  while (<FH>) {
    if (/ENDDATA/) {
      last;
    } else {
      chomp;
      my $t = $_;
      if (($t =~ /;/) && ($t !~ /^;/)) {
	@a = split ";",$t;
	if ($#a == 1) {
	  my $k = strip($a[0]);
	  my $key_code = -1;
	  if (exists $uc{$k} ) {
	    $key_code = $uc{$k};
	  }
	  my $b = strip($a[1]);
	  my $text =sprintf "\"%d\" : { \"type\" : %d, \"s\": \"%s\", \"t\" : \"%s\" }",$key_code, $type,$k,$b;
	  if ($type == 1) {
	    $bc{$b} = 1;  # these are the base characters we are using
	  }
	  if ($type == 2) {
	    $dc{$b} = 1;
	  }
	  # print $text . "\n";
	  push @code, $text;
	}
      }
    }
  }
  # my $x = pop @code;
  # $x =~ s/,$//;
  # push @code, $x;
  # push @code,"};";
  return @code;
}
#    "combinations" :  {
#	"945"  : { base: "GREEK SMALL LETTER ALPHA", diacritics : [] , combined : [7936,7937,7938,7939]},
#	"7936" : { base: "GREEK SMALL LETTER ALPHA", parent : 945, diacritics : ["PSILI"] },
#	"7937" : { base: "GREEK SMALL LETTER ALPHA", parent : 945, diacritics : ["DASIA"] },
#	"7938" : { base: "GREEK SMALL LETTER ALPHA", parent : 945, diacritics : ["PSILI","VARIA"] },
#	"7939" : { base: "GREEK SMALL LETTER ALPHA", parent : 945, diacritics : ["DASIA","VARIA"] },
sub gen_combinations {
  my @child_codes;
  my $parent;
  my $keycode;
  my $base;
  my @combinations;
  my @code;

  foreach my $k (keys %bc) {
    #    print $k . "\n";
    $base = $k;
    $parent = strip($uc{$base});
    foreach my $u (keys %uc) {
      if ($u =~ /WITH/) {
	if ($u =~ /^$k/) {
	  my @dt = split /WITH|AND/, $u;
	  shift @dt;
	  my @dx;
	  foreach my $x (@dt) {
	    push @dx, sprintf "\"%s\"",strip($x);
	  }
	  push @child_codes, $uc{$u};
	  my $t =sprintf "\"%d\" : { \"base\" : \"%s\",\"parent\" : %d, \"diacritics\" : [%s] }",$uc{$u},$base,$parent,join ",",@dx;
	  push @combinations, $t;
	}
      }
    }
    if ($#child_codes >= 0) {
      my $t = sprintf "\"%d\" : { \"base\" : \"%s\",\"diacritics\" : [],\"combined\" : [%s]}",$parent,$base,join ",",@child_codes;
      push @code,$t;
      push (@code,@combinations);
      $#combinations = -1;
      $#child_codes = -1;
    }
  }
  #
  return @code;
}
#-------------------------------------------
#
#-------------------------------------------
sub strip {
  my $t = shift;
  if ($t) {
    $t =~ s/^\s+//;
    $t =~ s/\s+$//;
  }
  return $t;
}
#-------------------------------------------
#
#-------------------------------------------
sub gen_unicode {
  my @code;
  my $v;
  foreach my $k (keys %bc) {
    if (! exists $uc{$k}) {
      print STDERR "gen_unicode missing value for key :[$k]\n";
      $v = -1;
    } else {
      $v = $uc{$k};
    }
    push @code, sprintf "\"%s\" : %d",$k,$v;
  }
  return @code;
}
# 92  : { type : 2, s : "REVERSE SOLIDUS" , t : "VARIA" },
#-------------------------------------------
#
#-------------------------------------------
sub gen_combining {
  local *FH  = shift;
  my @code;
  push @code, "var c_keys = {";
  while (<FH>) {
    if (/ENDDATA/) {
      last;
    } else {
      chomp;
      my $t = $_;
      if (($t =~ /;/) && ($t !~ /^;/)) {
        @a = split ";",$t;
        if ($#a == 1) {
          my $k = strip($a[0]);
          my $x = sprintf "\"%s\" : \"%s\",",$k,strip($a[1]);
          if (exists $kc{$k} ) {
            push @ki, sprintf "%s : 1,", $kc{$k};
          }
          push @code, $x;
        }
      }
    }
  }
  my $x = pop @code;
  $x =~ s/,$//;
  push @code, $x;
  push @code,"};";
  return @code;

}
#-------------------------------------------
#
#-------------------------------------------
sub gen_input_mapping {
  my $f = shift;
  open(IN,"<$f") or die "No data file";
  my @code;
  while (<IN>) {
    chomp;
    if (/NONCOMBINING/) {
      push (@code,gen_keys_of_interest(*IN,1));
    }
    if (/COMBININGKEYS/) {
      push (@code,gen_keys_of_interest(*IN,2));
    }
    if (/HEADERDATA/) {
      get_props(*IN);
    }
    #  print $_;
  }
  #  print join "\n",@code;
  return @code;
}
#-------------------------------------------
#
#-------------------------------------------
sub doFile {
  my $file = shift;
  unicode_lookup();
  push @ki, "var input_keys = {";
  my @koi = gen_input_mapping($file);
  my @cmb = gen_combinations();
  my @uni = gen_unicode();
  my @json;
  my $v = localtime;
  unless ($debugrun == 1) {
    push @props,sprintf "\"%s\" : \"%s\"","parse data",$v;
  }
  ;

  push @props,sprintf "\"%s\" : \"%s\"","type","mapping";


  my $js = '{';
  if ($#props > 0) {
    $js .= sprintf "%s , ",join ",",@props;
  }
  $js .= sprintf "\"unicode\" : { %s },",join ",",@uni;

  $js .= sprintf "\"koi\" : { %s },",join ",",@koi;

  $js .= sprintf "\"combinations\" : { %s }",join ",",@cmb;

  $js .= "}";
  return ($outfile,$js);
}
#open(JSFILE,">$outfile");
#print JSFILE $js;
#close JSFILE;
#}
