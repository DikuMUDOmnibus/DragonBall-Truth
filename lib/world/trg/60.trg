#6000
new trigger~
0 g 100
~
growl
~
#6096
Isodeath~
0 f 100
~
&16As the final blow is dealt, the beast and his aide sink beneath the surface of the water, a roar of defeat and defiance following them. As the water closes behind them, a voice of rage calls out in singular triumph.&00
Wait 2 sec
&15"The Riders are still finished... the one true beast holds the tomes!"&00
~
#6097
Samehade~
0 k 15
~
hellspear
disk
hellspear
wait 5 sec
~
#6098
Isonade2~
0 l 50
~
if (%have_activated%)
return 0
halt
else
say Samehade! I need more! I need more energy!!!
wait 1 sec
mecho &16Letting loose a roar, Isonade sinks beneath the depths as another being rises up in an explosion of water.
wait 1 sec
mload mob 6098
mecho &16As water and &09b&00&01l&09o&00&01o&09d&16 begin to rain down, the aide looks at those who thought they could kill his master.
Wait 2 sec
Mecho &15"Recharge you energy master, I will handle this peons!"
goto 6000
wait 1 sec
2heal
2heal
2heal
2heal
2heal
2heal
2heal
2heal
wait 10 sec
goto samehade
set have_activated 1
global have_activated
~
#6099
Isonade1~
0 k 12
~
disk
hellspear
wait 2 sec
2heal
2heal
2heal
2heal
disk
disk
wait 5 sec
disk
disk
hellspear
wait 10 sec
~
$~
