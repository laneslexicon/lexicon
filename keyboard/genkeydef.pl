#!/usr/bin/perl -w
use Getopt::Std;
use Encode;
use utf8;
binmode STDERR, ":utf8";
binmode STDOUT, ":utf8";
my @lines;
my @ini;
my $keyStartPos = -1;

sub test {

@lines = <<EOL =~ m/(\S.*\S)/g;
1,1,`:¬:|
1,2,0x31:!
1,3,0x32:\"
1,4,0x33:£
1,5,0x34:\$:x
1,6,0x35:%
1,7,0x36:^
1,8,0x37:%
1,9,0x38:*
1,10,0x39:(
1,11,0x30:)
1,12,-:_
1,13,=:+
1,14,<Backspace>,Backspace
#
#  row 2
#
2,1,<Tab>,Tab
2,2,q:Q,level2
2,3,a:A,level2
2,4,s:S,level2
2,5,d:D,level2
2,6,f:F,level2
2,7,g:G,level2
2,8,h:H,level2
2,9,j:J,level2
2,10,k:K,level2
2,11,l:L,level2
2,12,;:X
2,13,':@
2,14,#:~
2,15,<Enter>,Enter
EOL

}
###################################################
# this will provide a dummy header if nono is given
###################################################

my $header = <<EOH;
[Header]
name=New keyboard
levels=3
groups=1
script=Arabic
button%20width=110
button%20height=100
text color=
key down color=
key up color=
keyboard color=
EOH


getopts("o:i:");
#
# read all the lines in finding where the keys start
#
if ($opt_i) {
  my $i = 0;
  open(IN,"<:utf8",$opt_i) or die "No input file $opt_i";
  while(<IN>) {
    chomp;
    if (/START_KEYS/) {
      $keyStartPos = $i;
    }
    push @lines,$_;
    $i++;
  }
  close IN;
}
#
# if no START_KEYS found, use the dummy header
# otherwise shift the header lines from @lines
# to @ini
#
if ($keyStartPos == -1) {
  push @ini , "$header";
} else {
  push @ini , "[Header]";
  for (my $i=0;$i < $keyStartPos;$i++) {
    my $t = shift @lines;
    push @ini , "$t";
  }
  shift @lines;
}
# my $s = "4,2,ز:ظ";

# my $t ="4,1,<Shift>,Shift,[sticky,type=level2]";
# if ($t =~ /(.+),\[(.+)\]$/) {
#   push @ini , "yep $1  ---------- $2 ";
# }
# exit;

####################################################################################################
#
# A line beginning with '#' will be ignored
# A line begins with a row and column specification and then upto three further comma delimited fields
# If the final entry on the line is bracketed with '[' and ']' this specifies special properties for
# the key.
# The other two entries specify the value to output when the key is pressed and what character is to appear on the keyboard (this is termed the 'decoration'
# If the value is the same as the decoration, the decoration may be omitted.
#
# key line format
# row,col,values,optional decoration ,[properties]
#
#   values     - colon separated list of values to send when the key is pressed>
#                each element in the list represents a level, so the first
#                item is level1, the second level2, etc
#                Each value can be either the character itself or in the
#                format 0xnnnn for a hex number
#   decoration - if the character that appears on the keypad is not the same as the value, then
#                decoration is a colon separated list of characters to show
#                on the keypad, each element in the list corresponding to a level
#
#   properties  - a comma separated list properties, in the format <name>=<value> for the key
#                span=n       the key occupies n columns on the keyboard
#                sticky=true  the key stays down until a normal key is press i.e. shift key
#                type=leveln  shifts the keyboard into level n
#                type=groupn  shifts the keyboard into group n
#                vpos=nn      the vertical position of the decoration on the keypad
#                toogle=true  key stays down until it is pressed i.e. Caps Lock
#
#
#  if the value or the decoration is e.g a comma or colon, you must use their hex values in the
#  input file otherwise the script will get very confused.
#
###################################################################################################
foreach $line (@lines) {
  next unless $line !~ /^#/;
  my @values;
  my $key;
  my $props;
  if ($line =~ /(.+),\[(.+)\]$/) {
    $key = $1;
    $props = $2;
  }
  else {
    $key = $line;
  }
  @k = split ",",$key;
  my $count = scalar @k;
  my $row = $k[0];
  my $col = $k[1];
  my $v = $k[2];
  if ($v)  {
    @values = split ":",$v;
  }
  push @ini , sprintf "[Key%s-%s]",$row,$col;
  push @ini , sprintf "row=$row";
  push @ini , sprintf "col=$col";
  my $keytype = "char";
  if ($props) {
    my @p = split ",", $props;
    foreach my $k (@p) {
      if ($k =~ /type=(.+)/) {
        $keytype = $1;
      }
      else {
        push @ini, "$k";
      }

    }

  }
  push @ini , sprintf "type=%s",$keytype;
  for(my $i=0;$i <= $#values;$i++) {
    push @ini , sprintf "level%d/value=%s",$i + 1,$values[$i];
  }
  if ($count == 4) {
    if ($k[3] =~ /level/) {
      push @ini , sprintf "decoration=%s",$k[3];
    }
    else {
    my @dec = split ":",$k[3];
    for(my $i=0;$i <= $#dec;$i++) {
      push @ini , sprintf "level%d/decoration=%s",$i + 1,$dec[$i];
    }
  }
}
  push @ini , "";
}
if ($opt_o) {
  open(OUT,">:utf8",$opt_o);
  print OUT join "\n",@ini;
  close OUT
}
else {
  print STDOUT join "\n",@ini;
}
