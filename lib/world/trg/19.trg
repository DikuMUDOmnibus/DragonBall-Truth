#1900
rikkai greet~
0 g 100
~
wait 2 sec
say Greetings, welcome to my pay lake. We stock our lake with fish constantly so you're guaranteed to catch a huge mess of fish and it only costs 50 zenni to get in.
wait 2 sec
mecho Rikkai extends his hand like as if he's waiting for something.
~
#1901
rikkai bribe~
0 m 50
~
if (%amount% == 50)
wait 2 sec
mecho Rikkai takes the money and drops it into the front pocket of his overalls.
wait 1 sec
say Thanks, here you go!
wait 2 sec
unlock gate
open gate
wait 15 sec
close gate
lock gate
else if (%amount% >= 50)
wait 2 sec
mecho Rikkai takes the money and drops it into the front pocket of his overalls.
wait 1 sec
say Wow, thanks for the tip, good luck fishing and here you go!
wait 2 sec
unlock gate
open gate
wait 15 sec
close gate
lock gate
else if (%amount% <= 49)
wait 2 sec
say %amount% zenni isn't enough money, it costs 50 zenni to get in.
wait 1 sec
drop %amount% zenni
end
~
$~
