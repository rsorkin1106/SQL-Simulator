#Awesome Spec Example!
CREATE 281class 3 string string bool emotion person Y/N
INSERT INTO 281class 8 ROWS
happy Darden true
stressed students false
busy office_hours true
stressed students true
stressed Paoletti true
happy Darden true
happy Sith true
victorious Sith true
DELETE FROM 281class WHERE person = Darden
GENERATE FOR 281class hash INDEX ON emotion
PRINT FROM 281class 2 person emotion WHERE Y/N = true
CREATE pets 3 string bool bool Name likes_cats? likes_dogs?
INSERT INTO pets 2 ROWS
Sith true true
Paoletti true false
JOIN pets AND 281class WHERE Name = person AND PRINT 3 Name 1 emotion 2 likes_dogs? 1
REMOVE pets
REMOVE 281class
QUIT
