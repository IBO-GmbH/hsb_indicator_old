#!/bin/bash

REMOTE_SERVER=m0
REMOTE_PATH=/home/mlanner/mold_sshfs

ssh $REMOTE_SERVER pkill -cn mold
set -e
ssh $REMOTE_SERVER $REMOTE_PATH/exec-mold $@

