
PPS = pps.c serverUtil.c
PQC = pqc.c clientUtil.c

program: pps pqc

pps: $(PPS)
	gcc -o pps $(PPS)

pqc: $(PQC)
	gcc -o pqc $(PQC)