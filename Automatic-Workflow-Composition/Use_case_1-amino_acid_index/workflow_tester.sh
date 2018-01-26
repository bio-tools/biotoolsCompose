#!/usr/bin/bash
# workflow tester

# download data:
wget http://cpm.lumc.nl/export/public_datasets/E_coli_ion_trap.mzXML

# define the command line arguments of the various steps
STEP[1]="comet -Pcomet.params -NE_coli_ion_trap -Dup000000318.fasta E_coli_ion_trap.mzXML"
STEP[2]="tandem tandem_params.xml"
STEP[3]="Tandem2XML E_coli_ion_trap.tandem E_coli_ion_trap.pep.xml"
STEP[4]="xinteract -NE_coli_ion_trap.interact.pep.xml -p0.95 -l7 -O E_coli_ion_trap.pep.xml"
STEP[5]="bash pepXML2SSRCalc.sh E_coli_ion_trap.interact.pep.xml peptides.txt"
STEP[6]="RTCalc TRAIN=peptides.txt"
STEP[7]="/home/biodocker/bin/rt -i E_coli_ion_trap.interact.pep.xml -f tab"

# put the steps together in different workflows and run
${STEP[1]}; ${STEP[4]}; ${STEP[5]}; ${STEP[6]} > model_1.txt
${STEP[1]}; ${STEP[4]}; ${STEP[7]} > model_2.txt
${STEP[2]}; ${STEP[3]}; ${STEP[4]}; ${STEP[5]}; ${STEP[6]} > model_3.txt
${STEP[2]}; ${STEP[3]}; ${STEP[4]}; ${STEP[7]} > model_4.txt

# compare the results
echo "A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y" > results.csv
for x in model*; do awk '{c[$1]=$2} END {split("ACDEFGHIKLMNPQRSTVWY",aa,""); for(i=1;i<20;i++) printf("%s,",c[aa[i]]); print c[aa[20]];}' $x >> results.csv; done
