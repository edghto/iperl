CFLAGS=-Wall -std=c99 -g -c 
LFAGS=-lfl
CC=gcc

.PHONY: Debug
Debug: lex.yy.c syntax.tab.c obj/HV.o obj/AV.o obj/SV.o obj/GV.o obj/utils.o obj/AST.o obj/eval.o
	$(CC) -g -o iperl lex.yy.c syntax.tab.c obj/HV.o obj/AV.o obj/SV.o obj/GV.o obj/utils.o obj/AST.o obj/eval.o $(LFAGS)

iperl: lex.yy.c syntax.tab.c obj/HV.o obj/AV.o obj/SV.o obj/GV.o obj/utils.o obj/AST.o obj/eval.o
	$(CC) -DYYDEBUG=1 -DYYERROR_VERBOSE=1 -g -o iperl lex.yy.c syntax.tab.c obj/HV.o obj/AV.o obj/SV.o obj/GV.o obj/utils.o obj/AST.o obj/eval.o $(LFAGS)

obj/eval.o: eval.c eval.h kernel.h SV.h AV.h HV.h GV.h utils.h AST.h
	$(CC) $(CFLAGS) eval.c -o obj/eval.o

obj/utils.o: utils.c utils.h kernel.h AV.h
	$(CC) $(CFLAGS) utils.c -o obj/utils.o

obj/AST.o: AST.c AST.h syntax.tab.h kernel.h 
	$(CC) $(CFLAGS) AST.c -o obj/AST.o

obj/GV.o: GV.c GV.h kernel.h
	$(CC) $(CFLAGS) GV.c -o obj/GV.o

obj/HV.o: HV.c HV.h kernel.h
	$(CC) $(CFLAGS) HV.c -o obj/HV.o

obj/AV.o: AV.c AV.h kernel.h
	$(CC) $(CFLAGS) AV.c -o obj/AV.o

obj/SV.o: SV.c SV.h kernel.h
	$(CC) $(CFLAGS) SV.c -o obj/SV.o

lex.yy.c: lex.l syntax.tab.h
	flex lex.l

syntax.tab.h syntax.tab.c: syntax.y
	bison -d --report=state syntax.y

clean:
	rm -f lex.yy.c
	rm -f syntax.tab.c
	rm -f obj/SV.o
	rm -f obj/AV.o
	rm -f obj/HV.o
	rm -f obj/GV.o
	rm -f obj/utils.o
	rm -f obj/AST.o
	rm -f obj/eval.o
	rm -f syntax.output

