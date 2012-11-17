DIRS1=lectures exercises master-solutions es131 js174 mb226
DIRS2=ab331-se33 ah367 as462 cg160 cs449 dp65-mg225 fs204 fw41 jm152 js301\
			jw218 ke23 mb381 mf205 mf228

default:
	@echo "targets: up, pepare EX=0?, ci"

up:
	@for i in $(DIRS1);\
		do echo "updating $$i";\
		cd $$i; svn up; cd ..;\
	done;
	@for i in $(DIRS2);\
		do echo "updating $$i";\
		cd $$i; svn up; cd ..;\
	done;

EX=0
prepare:
	@for i in $(DIRS2);\
		do echo "copying feedback skeleton to $$i";\
		cd $$i/exercise-sheet-*$(EX);\
		cp -n ../../feedback-tutor.txt .; svn add feedback-tutor.txt;\
		echo "creating feedback directory for $$i";\
		svn mkdir ".feedback";\
		svn cp *.h .feedback/;\
		svn cp *.cpp .feedback/;\
		cd ../..;\
	done;

MSG=Tutor(esawin): added feedack and comments.
ci:
	@for i in $(DIRS2);\
		do echo "committing $$i";\
		cd $$i; svn ci -m "$(MSG)"; cd ..;\
	done;
