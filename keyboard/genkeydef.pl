#!/usr/bin/perl -w
use Getopt::Std;
use Encode;
use utf8;
binmode STDERR, ":utf8";
binmode STDOUT, ":utf8";
my @lines;
my @ini;
my $keyStartPos = -1;
getopts("o:i:");
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
if ($keyStartPos == -1) {
  push @ini , "$header";
}
else {
  push @ini , "[Header]";
for(my $i=0;$i < $keyStartPos;$i++) {
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
