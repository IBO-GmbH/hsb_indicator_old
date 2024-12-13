#!/bin/bash

set -e

REMOTE_SERVER=m3
#REMOTE_SERVER=10.8.0.22
REMOTE_PATH=/opt/mold

#rsync --progress -v mold/mold_de.qm $REMOTE_SERVER:$REMOTE_PATH
#rsync --progress -v tests/tests $REMOTE_SERVER:$REMOTE_PATH/bin
rsync --progress -v mold/mold $REMOTE_SERVER:$REMOTE_PATH/bin
rsync --progress -v mold/libmold.so wolf/libwolf.so $REMOTE_SERVER:$REMOTE_PATH/lib
#rsync --progress -v tone_test/tone_test $REMOTE_SERVER:$REMOTE_PATH
#rsync --progress -v enocean_test/enocean_serial_test enocean_test/enocean_link_test $REMOTE_SERVER:$REMOTE_PATH
#rsync --progress -v co2_test/co2_test $REMOTE_SERVER:$REMOTE_PATH



