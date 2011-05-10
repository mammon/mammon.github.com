#!/bin/bash

if [ $# -ne 1 ]
then
   echo "Usage: elf.sh target"
   exit 1
fi
TARGET=$1

ENTRY_POINT=`od -A n -t x -j 0x18 -N 4 $TARGET |sed 's/ //g'|tr '[a-z]' '[A-Z]'`
PROG_HDR=`od -A n -t u -j 0x1C -N 4  $TARGET | sed 's/ //g'`
PH_SIZE=`od -A n -t u -j 0x2A -N 2  $TARGET | sed 's/ //g'`
PH_COUNT=`od -A n -t u -j 0x2C -N 2  $TARGET | sed 's/ //g'`

while [ $PH_COUNT -gt 0 ]
do
  TMP=`od -A n -t x -j $PROG_HDR -N  4 $TARGET |sed 's/ //g'`
  if [ $TMP -eq 1 ]
  then
    FLAGS=`expr $PROG_HDR + 24`
    TMP=`od -A n -t x -j $FLAGS -N 4 $TARGET |sed 's/ //g'`
    if [ $TMP -eq 5 ]
    then
      TEXT_OFF=`expr $PROG_HDR + 4`
      TEXT_OFF=`od -A n -t x -j $TEXT_OFF  -N 4 $TARGET|sed 's/ //g'|tr '[a-z]' '[A-Z]'`
      TEXT_VADDR=`expr $PROG_HDR + 8`
      TEXT_VADDR=`od -A n -t x -j $TEXT_VADDR -N 4 $TARGET|sed 's/ //g'|tr '[a-z]' '[A-Z]'`
      TEXT_PADDR=`expr $PROG_HDR + 12`
      TEXT_PADDR=`od -A n -t x -j $TEXT_PADDR -N 4 $TARGET|sed 's/ //g'|tr '[a-z]' '[A-Z]'`
      TEXT_SIZE=`expr $PROG_HDR + 16`
      TEXT_SIZE=`od -A n -t x -j $TEXT_SIZE  -N 4 $TARGET|sed 's/ //g'|tr '[a-z]' '[A-Z]'`
      PH_COUNT=0 
    fi
  fi
  PH_COUNT=`expr $PH_COUNT - 1`
  PROG_HDR=`expr $PROG_HDR + $PH_SIZE`
done



echo " Target : $TARGET"
echo "          Entry Point: $ENTRY_POINT"
echo "          .text segment"
echo "                offset: $TEXT_OFF         size: $TEXT_SIZE"
echo "                p_addr: $TEXT_PADDR       v_addr: $TEXT_VADDR"
echo " ------------------------------------------------------------------"
echo " Beginning Disassembly..."

EOT=` echo "ibase = 16; obase = 10;$TEXT_VADDR + $TEXT_SIZE" | bc`
CURR_INST=$ENTRY_POINT
IN_TEXT=1

while [ $IN_TEXT -eq 1 ]
do
  TMP=`echo "ibase = 16; $EOT - $CURR_INST" | bc`
  if [ $TMP -lt 1 ]
  then
     IN_TEXT=0
  else
    # ----- Here you can call a lookup function for the next 16 byes
    #       the max size of one Intel instruction
    #       This function should return size:mnemonic, so one can
    #         INST_SIZE=`echo $RESULT | cut -d: -f1`
    #         INST_MNEM= `echo $RESULT | cut -d: -f2`
    LOC=` echo "ibase = 16; $CURR_INST - $TEXT_VADDR" | bc`
    INST_SIZE=16
    INST_MNEM=`od -An -tx -j $LOC -N $INST_SIZE $TARGET`
    echo "$CURR_INST : $INST_MNEM"
    CURR_INST=` echo "ibase = 16; obase = 10; $CURR_INST + $INST_SIZE" | bc `
  fi
done
