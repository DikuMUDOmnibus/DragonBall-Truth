#2200
Destructo Disk~
0 k 10
~
destructo
~
#2240
Glog~
0 g 100
~
wait 1 sec
say HAHAHA, rip his head offs! I likes to sees the blood!
wait 2 sec
say Who da hell are you!?
wait 1 sec
say Ah well, you die now.....
wait 1
kill %actor.name%
~
#2241
Garn recieve 2~
0 j 100
~
if (%object.name% == Glog Plasma Light)
wait 1 sec
say Wow, you got Glug's brother!?
wait 2 sec
say Hmm, well alright. Here take this, it should do you well....
wait 1 sec
mload obj 2241
drop vulcan
mpurge %object.name%
~
#2242
Glog's memory~
0 g 100
~
wait 1
say Damn you! Coward!
kill %actor.name%
~
$~
