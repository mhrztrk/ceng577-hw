EXECS=hw2 hw2.o
MPICC?=mpicc

all: ${EXECS}

hw2: hw2.c
	@${MPICC} hw2.c -o hw2 -lm

submit: 
	@qsub pbsscript.sh | grep -o '[0-9]*' > id
	@echo "Job ID: `cat id`"

stat:
	@qstat `cat id`

clean:
	@rm ${EXECS}
	@if [ -a myjop* ];\
	then\
		rm myjob*;\
	fi;
	@if [ -a id ];\
	then\
		rm id;\
	fi;

