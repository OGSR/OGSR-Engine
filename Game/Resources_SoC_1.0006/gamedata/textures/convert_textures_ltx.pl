use strict;
use IO::File;

my $r_fh = IO::File->new('textures.ltx', 'r');
my $w_fh = IO::File->new('textures_conv.ltx', 'w');
my $types = 0;
while(<$r_fh>)
{
	chomp;
	if (/(crete|grnd|mtl|prop|roof|ston|trees|wood)\\/)
	{
		if (/bump_mode\[use:(.+)\], material/)
		{
			if ($types == 0)
			{
				$_ .= ', parallax[yes]';
			}
		}
	}
	if (/\[types\]/)
	{
		$types = 1;
	}
	print $w_fh $_."\n";
}
$r_fh->close();
$w_fh->close();