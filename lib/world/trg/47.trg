#4701
Bubbles~
0 g 100
~
if (%direction% == south)
emote jumps up and down and points North to the small house.
wait 1 sec
say Ragoogoo, rakiki, ratata, ragiglee!
end
~
#4702
King Kai~
0 g 100
~
if (%direction% == south)
emote smiles happily as he looks up at you.
wait 1 sec
say Hello mortal.  I have a favor to ask you.
wait 2 sec
say Tien ran off with my halo.
wait 3 sec
say He is around here somewhere.  But he keeps evading me.
wait 2 sec
say If you will, retrieve my halo and bring it to me and I will allow you to train with Pikkon and his warriors.
wait 2 sec
say But be careful, he has trained very hard.  I await your return with the halo.
end
~
#4703
King Kai trainer~
0 j 100
~
if (%object.vnum% == 4701)
wait 1 sec
say Oh, thank you for returning my halo to me.
wait 1 sec
emote puts his halo back on his head adjusting it slightly.
wait 1 sec
say As I promised, I will let you train with Pikkon and his fighters.
wait 2 sec
say Wait here until I return from summoning him.
south
mload mob 4702
north
say Thank you for waiting, go South.  Pikkon awaits you there.
end
~
#4705
Pikkon greeting~
0 g 100
~
if (%direction% == north)
wait 1 sec
emote looks up at you with dark eyes.
wait 1 sec
say So, I see you are the one that has done the favor for King Kai.
wait 1 sec
say I will help you train by letting you fight my warriors.
wait 2 sec
wait 2 sec
say But, it's not free.  It will cost you 1000 zenni
wait 1 sec
say You may start whenever you are ready.
end
~
#4706
Pikkon Trainer~
0 m 1000
~
wait 3 sec
say Alright, let's see if you can handle my warriors!
wait 1 sec
mload mob 4703
wait 2 sec
mecho The warrior screams out, "Attack me if you dare!"
~
#4711
King Yemma~
0 g 100
~
if (%direction% == south)
emote looks up from his book, drops his pen and looks at you.
wait 1 sec
say Hello %actor.name%, welcome to the afterlife...
wait 2 sec
say The entrance to snake way is behind me.  I suggest you don't fall off.
end
~
$~
