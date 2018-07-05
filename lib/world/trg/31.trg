#3100
Holo machine~
0 g 100
~
wait 1 sec
mecho The holo machine begins to hum loudly.
wait 1 sec
say Greetings warrior! If you would like to train off of a holo, give me 5000 zenni!
~
#3101
Holo 2~
0 m 0
~
if (%amount% == 5000)
wait 1 sec
mecho The holo machine hums as it generates a Holo Buu.
mload mob 3108
else
wait 1 sec
say You have not inserted the correct amount of money, please try again...
drop %amount% zenni
~
#3102
Holo~
0 n 100
~
wait 1 sec
scream
~
$~
