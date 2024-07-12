# Game of guess the number
10 PRINT "Guess the number!"
20 PRINT "Written in mbasic by matthilde"
30 VAR X RANDOM%100
50 INPUT NUMBER
60 IF X>NUMBER GOTO 100
70 IF X<NUMBER GOTO 110
80 PRINT "You win!"
90 END
100 PRINT "More."
101 GOTO 50
110 PRINT "Less."
111 GOTO 50
