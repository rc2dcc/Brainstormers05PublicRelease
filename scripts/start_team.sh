#!/bin/sh

HOST=localhost
AGENTDIR=bs2k
COACHDIR=sputcoach
AGENT=artagent/BS2kAgent
COACH=SputCoach
AGENTOPT="-log_dir /tmp -host $HOST"
COACHOPT="-server_9.4 0 -host $HOST"

# sleep in between agent starting if available
USLEEP=`which usleep`
if [ "$USLEEP" != "" ]; then
  USLEEP=$USLEEP\ 300000
else
  echo "WARNING: No usleep found on this system!"
fi

cd ..
BASEDIR=`pwd`

######## START AGENTS ###########

cd $AGENTDIR || exit 1

./$AGENT -goalie $AGENTOPT $DBG &
sleep 1

I=0
while [ "$I" != "10" ] ; do
  ./$AGENT $AGENTOPT $DBG &
  $USLEEP
  I=`expr $I + 1`
done

######## START COACH ##########
cd $BASEDIR
cd $COACHDIR || exit 1

./$COACH $COACHOPT $DBG &
