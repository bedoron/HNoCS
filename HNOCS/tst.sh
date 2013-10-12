#!/bin/bash
EMAIL="sbedoron@t2.technion.ac.il"

################################ NOTIFY WERE DONE ##############
echo "Mail someone simulation is done"
SUBJECT="HNoCs Simulation on `hostname` is done"
EMAILMESSAGE="/tmp/emailmessagesimulation.txt"
MSG="""
HNoCs Simulation on `hostname` is done in:
`pwd`
"""
echo "$MSG" 	> $EMAILMESSAGE

scp $EMAILMESSAGE t2.technion.ac.il:$EMAILMESSAGE
ssh sbedoron@t2.technion.ac.il /bin/mail -s \"$SUBJECT\" \"$EMAIL\" < $EMAILMESSAGE

