#1000
hiker hello~
0 g 100
~
wait 2 sec
say Hello. Beautiful day isn't it? I was just stretching before taking a big hike in the mountains.
wait 2 sec
emote stretches from side to side.
wait 10 sec
say Hey, have you heard about that cave along the trail where some people have disappeared? Scary stuff huh?
wait 1 sec
emote grimaces.
~
#1001
load eagle~
2 g 40
~
wait 1 sec
wecho A shrill cry can be heard far overhead.
wait 3 sec
wecho &00&15An enormous &00&03G&11o&00&03l&11d&00&03e&11n&00 &03E&07a&16g&00&03l&06e&00 comes flying in from above.
wload mob 1005
~
#1002
laying an egg~
0 f 100
~
mload mob 1006
mecho The Golden Eagle plummets to the ground and with it's last ounce of strength, moves to protect it's nest.
mforce nestmob mload obj 1005
mforce nestmob unlock nest
mforce nestmob open nest
mforce nestmob mjunk key
mforce nestmob mpurge nestmob 
~
#1003
hiker shopkeeper hello~
0 h 100
~
wait 2 sec
say Good day, my friend! It's a nice day for a hike. Care to buy some supplies that could help
you along the way?
wait 2 sec
emote looks over his shoulder, then around the area suspiciously.
wait 1 sec
emote lowers his voice to a whisper.
wait 1 sec
say Oh, and I've heard there is a very rare eagle in these parts.... one of it's
eggs would be very valuable, and I know a few people who happen to be
interested. If you find one, make sure to let me know.
wait 2 sec
emote winks.
~
#1004
ithosian guardian hello~
0 h 100
~
if (%direction% == east)
wait 2 sec
say Greetings, welcome to the home of my people, Ithosia. You may enter but do not cause trouble while you visit or I may be forced to deal with you.
wait 1 sec
mecho The Supreme Guardian of Ithosia looks you over carefully.
wait 1 sec
unlock gate
open gate
wait 15 sec
close gate
lock gate
end
if (%direction% == west)
wait 2 sec
mecho The Supreme Guardian of Ithosia watches you pass out of the city.
wait 2 sec
say Leaving so soon? Well goodbye for now.
end
~
#1005
ithosian guardian speech reaction~
0 d 1
gero~
wait 1 sec
say What was that you just said? Did you mention Gero? I'd like to get my hands on that little bastard!
wait 1 sec
mecho The Supreme Guardian of Ithosia clentches his armored fist.
wait 2 sec
say That monster sends his armies of androids to capture my people and bring them back for his twisted experiments. I can hardly stand to even think of what he's been doing to my brethren.
wait 1 sec
mecho The Supreme Guardian of Ithosia grits his teeth.
wait 2 sec
say If only there was something I could do...I can't be everywhere at once and they tunnel their way into our city from various directions, attacking and running away before i can do anything.
wait 1 sec
say Bah, my rambling isn't going to solve any problems. Traveler, if you know anything about Gero, please go speak to our king. If there's anything you can do to help I'm sure he'll reward you greatly, because if we don't stop Gero soon our people will be extinct.
~
#1006
elderly ithosian gatekeeper hello~
0 h 100
~
wait 2 sec
if (%direction% == east)
say Greetings traveler. Welcome to Ithosia! Enjoy your stay and don't forget that there is a lovely selection of armor, weapons and other items in our shopping disctrict.
wait 1 sec
mecho An elderly Ithosian gatekeeper gestures to the south
wait 2 sec
say Oh, and if you need to get back out, be sure to say 'let me out'.
wait 1 sec
wave
end
if (%direction% != east)
wait 2 sec
say Don't forget, if you need out, just let me know.
wait 1 sec
mecho The Elderly Ithosian Gatekeeper returns to quietly watching those passing by.
end
~
#1007
elderly ithosian gatekeeper gate open~
0 d 0
let me out~
wait 1 sec
say Alright, here we go.
wait 1 sec
mecho The Elderly Ithosian Gatekeeper fumbles with his keys momentarily.
wait 1 sec
unlock gate
open gate
say off you go
wait 15 sec
close gate
lock gate
~
#1008
Falling from Sky~
2 g 100
~
wait 1
wforce all d
~
#1009
undefined~
2 g 100
~
Nothing.
~
#1010
undefined~
0 g 100
~
Nothing.
~
#1011
undefined~
2 g 100
~
Nothing.
~
#1012
Real Estate Lady~
0 dg 100
~
if (%direction% == east)
wait 2 s
say Talk to Sheer if you have any questions about buying a house.
wait 3 s
smile
~
#1013
Soul Trigger~
0 g 100
~
wait 1 s
say If you think you can send me to hell, you are a fool!
wait 2 s
emote looks at you, laughing insanely!
~
#1055
Tan talk~
0 g 100
~
wait 1 s
say Hello, sorry but there is no room for you to tan today.  I have appointments for the rest of the day.
~
#1074
Restaurant~
0 g 100
~
if (%direction% == south)
say Hello, follow me so you can be seated.
wait 1 sec
mforce %actor.name% north
north
emot shows you to your set and you sit down.
mforce %actor.name% sit
wait 1 sec
give menu %actor.name%
say Here is your menu your waiter will be with you shotly.
say Read your manu and figure out what you want to buy.
south
end
~
#1075
Restaurant2~
0 g 100
~
if (%direction% == north)
say Welcome back, would you please return the menu.
mforce %actor.name% give menu marry
say thank you for your serice come again soon!
emote waves to you.
end
~
#1076
Chef~
0 g 100
~
say what are you doing in here, get out!
~
#1077
Waitress2~
0 g 100
~
wait 3 sec
say Hello, my name is Pam and I'll be your waitress tonight.
say What do you want?
end
~
#1078
steak~
0 m 139
~
wait 1 sec
say Thank you, let me get you your steak.
brb
north
buy steak
wait 1 sec
south
give steak %actor.name%
say Will there be anything else?
end
~
#1079
chicken~
0 m 110
~
wait 1 sec
say Thank you, let me get you your chicken.
brb
north
buy chicken
wait 1 sec
south 
give steak %actor.name%
say Will there be anything else?
end
~
#1080
Red wine~
0 m 99
~
wait 1 sec
say Thank you, let me get you your Red wine.
brb
north
buy red
wait 1 sec
south
give red %actor.name%
say will there be anything else?
end
~
#1081
White wine~
0 m 100
~
wait 1 sec
say Thank you, let me get you your White wine.
brb
north
buy white
wait 1 sec
south
give white %actor.name%
say will there be anything else?
end
~
#1091
Alert System~
2 g 100
~
wait 1 sec
wecho Warning! Warning! Intruder Alert! Intruder Alert!
~
#1092
new trigger~
0 i 100
~
say My trigger commandlist is not complete!
~
#1093
Orb~
0 g 100
~
wait 1 sec
say Enter Password
~
#1094
Unlock Some door~
0 d 100
Goku~
wait 1 sec
unlock door
open door
~
#1095
Humming~
2 g 100
~
wecho The walls hum quietly from the whirring machines.
~
#1099
Trigger~
0 g 100
~
Nothing.
~
$~
