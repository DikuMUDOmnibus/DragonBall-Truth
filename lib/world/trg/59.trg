#5900
Final Flash~
0 k 100
~
final
~
#5901
Goku 2nd~
0 g 100
~
wait 1 sec
say Wow, this fight is really turning out to be intense....
wait 3 sec
say I hope Vegeta doesn't find me over here, I need more time!
wait 2 sec
say Lend me your energy...
wait 1 sec
mecho Goku raises his hands as he calls on the spirit bomb.
~
#5902
Smith~
0 g 100
~
wait 2 sec
say Barumm, hello little person....
wait 3 sec
say Barumum, do you wish to purchase my services?
wait 2 sec
say Harbanum, you need only bring and give me the Elemental Ore, then pay me 10 million zenni.
wait 3 sec
say I will then make you something nice.....
~
#5903
Smith 2~
0 j 100
~
if (%object.name% == Elemental Ore)
wait 1 sec
mpurge ore
mload mob 5912
mgoto 10669
else
wait 1 sec
say This is not what I can work with!
drop %object.name%
~
#5904
Smith load~
0 n 100
~
if (%random.10% == 5)
wait 1 sec
say Alright, give me the 10 million zenni now.
mload obj 5919
else
wait 1 sec
say Alright, give me the 10 million zenni now.
mload obj 5920
~
#5905
Smithing the ore~
0 m 100
~
if (%amount% == 10000000)
wait 1 sec
say Barumum, alright here I go....
wait 2 sec
mecho The smith takes the ore and squeezes the rocks from the elements.
wait 3 sec
The smith then takes the elements and slams them against the anvil. Picking up its hammer, it begins to hammer the ore.
wait 2 sec
mecho After several minutes the smith takes the formed ore, and dunks it in the ice water tank.
wait 2 sec
mecho The smith chants some magic words, causing the finished item to glow.
wait 1 sec
say Here it is, done and in perfect shape.
drop Element
mload mob 5911
mgoto 10669
mpurge smith
mpurge self
else
wait 1 sec
say This is not the right amount of money!
drop %amount% zenni
~
#5906
Vegeta~
0 g 100
~
wait 1 sec
say Where is Kakkarot hiding at!?
wait 2 sec
say If You know, you had better tell me....
~
$~
