set inpurdir="..\..\head-poses"
set ouputdir="Youtput.abc"
set NodeName=head_geo
Objs2Abc.exe -i %inpurdir% -o %ouputdir% -f 40 -n %NodeName%