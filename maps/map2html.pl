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

my $name = charnames::viacode(0x03A3);

my @letters;
my @data;
my @unicode;

sub load_letters {
  my $filename = shift;
  open(IN,"<$filename") or die "No file $filename";
  my $t;
  my $parse = 0;
  my $inHeader = 0;
  my %p;
  $#letters = -1;
  while (<IN>) {
    chomp;
    $t = $_;
    next unless $t !~ /^#/;
    next unless $t !~ /^;/;
    if ($inHeader) {
      if ($t =~ /^NAME\s*;\s*(\w+)/) {
        $p{NAME} = $1;
        next;
      }
      if ($t =~ /^LANGUAGE\s*;\s*(\w+)/) {
        $p{LANGUAGE} = $1;
        next;
      }
      if ($t =~ /^SCRIPT\s*;\s*(\w+)/) {
        $p{SCRIPT} = $1;
        next;
      }
      if ($t =~ /^VERSION\s*;\s*(.+)$/) {
        $p{VERSION} = $1;
        next;
      }
    }
    if ($t =~ /^HEADERDATA/) {
      $inHeader = 1;
      next;
    }
    if ($t =~ /^NONCOMBININGKEYS$/) {
      $parse = 1;
      $inHeader = 0;
      next;
    }
    if ($t =~ /^COMBININGKEYS$/) {
      $parse = 1;
      next;
    }
    if ($t =~ /^ENDDATA$/) {
      $parse = 0;
      next;
    }
    if (! $parse ) {
      next;
    }
    if ($t =~ /;/) {
      push @letters,$t;
    }
  }
  close IN;
  return \%p;
}
sub xxx {
my $row = 0;
my $col = 0;
print "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
print "<body>\n";
print "<table class=\"maptable\">\n";
foreach my $letter (@letters) {
  my @m = split /\s*;\s*/,$letter;
#  print sprintf "[%s][%s][%s][%s]\n",$m[0],
#    $m[1],charnames::string_vianame($m[0]),charnames::string_vianame($m[1]);
  print "<tr>\n";
  print sprintf "<td class=\"en\">%s</td><td class=\"ar\">%s</td>\n",charnames::string_vianame($m[0]),charnames::string_vianame($m[1]);
  print "</tr>\n";
}
print "</table>\n";

print "<table class=\"maptable\">\n";
foreach my $letter (@letters) {
  my @m = split /\s*;\s*/,$letter;
#  print sprintf "[%s][%s][%s][%s]\n",$m[0],
#    $m[1],charnames::string_vianame($m[0]),charnames::string_vianame($m[1]);
  print "<tr>\n";
  print sprintf "<td class=\"en\">%s</td><td class=\"ar\">%s</td>\n",$m[0],$m[1];
  print "</tr>\n";
}
print "</table>\n";


print "</body>\n";
print "</html>\n";
}


#  print sprintf "[%s][%s]\n",charnames::string_vianame("LATIN CAPITAL LETTER A"),
#  charnames::string_vianame("ARABIC LETTER ALEF");
sub get_template {
 my $x = <<'EOF';
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Keymap</title>
<style type="text/css">
.maptable { text-align : center; }
.nametable { width : 100% ; font-size : 10px}
.col1 { text-align : center }
.col2 { text-align : center }
.col3 { text-align : center }
.col4 { text-align : center }
.name2 { width : 30% }
.name4 { width : 30% }

.striped { background-color : lightblue }
.letters  { text-align : center }
.names { font-size : 12px }
.arabic { font-family : Amiri; font-size : 14px;text-align : center }
.lang { <!-- TMPL_VAR NAME=FONT --> }
</style>
</head>
<body>
<div align="center">
<h1><!-- TMPL_VAR NAME=MAPNAME --> keymap</h1>
<h2><!-- TMPL_VAR NAME=TITLE --></h2>
<table class="lettertable" width="60%" cellspacing="0">
    <tr>
      <th class="letterhead">Key</th>
      <th class="letterhead"><!-- TMPL_VAR NAME=COLHEAD --></th>
      <th></th>
      <th class="letterhead">Key</th>
      <th class="letterhead"><!-- TMPL_VAR NAME=COLHEAD --></th>
</tr>
<!-- TMPL_LOOP NAME=ROWS -->
   <tr>
    <!-- TMPL_IF NAME="__ODD__" -->
      <td class="col1 striped" align="center" valign="middle"><!-- TMPL_VAR NAME=COL1 --></td>
      <td class="lang striped"  align="center" valign="middle"> <!-- TMPL_VAR NAME=COL2 --></td>
      <td width="1%">&nbsp;</td>
      <td class="col3 striped"  align="center" valign="middle"><!-- TMPL_VAR NAME=COL3 --></td>
      <td class="lang striped"  align="center" valign="middle"><!-- TMPL_VAR NAME=COL4 --></td>

    <!-- TMPL_ELSE -->
      <td class="col1" align="center" valign="middle" ><!-- TMPL_VAR NAME=COL1 --></td>
      <td class="lang" align="center" valign="middle" ><!-- TMPL_VAR NAME=COL2 --></td>
      <td width="1%">&nbsp;</td>
      <td class="col3" align="center" valign="middle" ><!-- TMPL_VAR NAME=COL3 --></td>
      <td class="lang" align="center" valign="middle" ><!-- TMPL_VAR NAME=COL4 --></td>

    <!-- /TMPL_IF -->

   </tr>
<!-- /TMPL_LOOP -->
</table>
<br/>
<br/>
<h2>Unicode name table</h2>
<table class="nametable" width="100%" cellspacing="0">
<!-- TMPL_LOOP NAME=UNICODE -->
   <tr>
    <!-- TMPL_IF NAME="__ODD__" -->
      <td class="name1 striped"  align="left" valign="middle" ><!-- TMPL_VAR NAME=COL1 --></td>
      <td class="name2 striped"  align="left"  width="25%" valign="middle" ><!-- TMPL_VAR NAME=COL2 --></td>
      <td width="1%">&nbsp;</td>
      <td class="name3 striped"  align="left"  valign="middle" ><!-- TMPL_VAR NAME=COL3 --></td>
      <td class="name4 striped"  align="left"  valign="middle" ><!-- TMPL_VAR NAME=COL4 --></td>

    <!-- TMPL_ELSE -->
      <td class="name1" align="left"  valign="middle" ><!-- TMPL_VAR NAME=COL1 --></td>
      <td class="name2" align="left"  valign="middle" ><!-- TMPL_VAR NAME=COL2 --></td>
      <td width="1%">&nbsp;</td>
      <td class="name3" align="left"  valign="middle" ><!-- TMPL_VAR NAME=COL3 --></td>
      <td class="name4" align="left"  valign="middle" ><!-- TMPL_VAR NAME=COL4 --></td>

    <!-- /TMPL_IF -->
   </tr>
<!-- /TMPL_LOOP -->
</table>
</div>
</body>
</html>
EOF

return $x;

}
sub parse_map {
  my $mapFile = shift;
  my $props = load_letters($mapFile);
  $#unicode = -1;
  $#data = -1;

my $sz = scalar(@letters);
my $rows = $sz/2;
if (($sz % 2) != 0) {
   $rows++;
 }
my $ix;
my $c;
my $d;
for(my $i=0; $i < $rows;$i++) {
  my %h;
  my $letterA = $letters[$i];
  my $letterB = "";
  $ix = $i + $rows;
  if ($ix < $sz) {
    $letterB = $letters[$ix]
  }
  my @m = split /\s*;\s*/,$letterA;
  $c = $m[0];
  $d = $m[1];
  $h{COL1} = $h{COL2} = $h{COL3} = $h{COL4} = "";

  $h{COL1} = charnames::string_vianame($c);
  $h{COL2} = charnames::string_vianame($d);
  if ($letterB) {
    @m = split /\s*;\s*/,$letterB;
    $c = $m[0];
    $d = $m[1];

    #  print sprintf "[%d][%s][%s][%s]\n",scalar(@m),$letterB,$c,$d;

    $h{COL3} = charnames::string_vianame($c);
    $h{COL4} = charnames::string_vianame($d);
    if ($h{COL3} eq "") {
      $h{COL3} = $c;
    }
    if (! $h{COL4} ) {
      $h{COL4} = charnames::string_vianame("GREEK $d");
#      $h{COL4} = $d;
    }
  }
  push @data,\%h;

  my %j;
  @m = split /\s*;\s*/,$letterA;
  $j{COL1} = $m[0];
  $j{COL2} = $m[1];
  @m = split /\s*;\s*/,$letterB;
  $j{COL3} = $m[0];
  $j{COL4} = $m[1];
  push @unicode,\%j;

  #  print sprintf "[%s][%s]\n",$letterA,$letterB;
}
  return $props;
}
sub generate_html {
  my $map = shift;
  my $htmlFileName = shift;
  my $templateFile = shift;

  my $p = parse_map($map);
  my $html = get_template();
  my $template = HTML::Template->new(scalarref => \$html,loop_context_vars => 1);

  #  $template->param(LANG => "ar");
  $template->param(TITLE => sprintf "Keyboard to %s character table",$p->{LANGUAGE});
  $template->param(COLHEAD => $p->{LANGUAGE});
  $template->param(MAPNAME => $p->{NAME});
#  $template->param(FONT => "font-family : Gentium; font-size : 14px;text-align : center");

  $template->param(ROWS => \@data);
  $template->param(UNICODE => \@unicode);
  if ($htmlFileName) {
    open OUT, ">:utf8",$htmlFileName;
    print OUT $template->output;
    close OUT;
  }
  else {
    print STDOUT $template->output;
  }
}
my $mapFile;
my $htmlFile;
my $templateFile;
my $help = 0;
GetOptions(
           "help"   => \$help,
           "map=s" => \$mapFile,
           "html=s" => \$htmlFile,
           "template=s" => \$templateFile
          );
if (! $mapFile  || $help) {
  print STDERR <<EOF;

perl map2html.pl <options>

     --map <map file name>     - required, map file e.g buckwalter.txt
     --html <output file>      - optional to standard output
EOF
  exit 0;
}
if (! -e $mapFile ) {
  print STDERR "Map file not found: $mapFile\n";
  exit 0;
}
generate_html($mapFile,$htmlFile);
