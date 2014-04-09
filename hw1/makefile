EXECS=hw1 hw1.o
MPICC?=mpicc

all: ${EXECS}

hw1: hw1.c
	@${MPICC} -o hw1 hw1.c

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

