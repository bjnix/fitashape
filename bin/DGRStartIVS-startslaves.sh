#!/usr/bin/env bash
#
# This script is called by DGRStartIVS.sh and is run directly on the
# IVS computer. You should not run this script directly because it
# does not start a master copy of the program on your local computer.

set -e # exit script if any command returns a non-zero exit code.

# The hostname that we expect to be running on:
EXPECT_HOSTNAME="ivs.research.mtu.edu"

if [[ ${HOSTNAME} != ${EXPECT_HOSTNAME} ]]; then
    echo "This script ($0) only works with $EXPECT_HOSTNAME. You are INCORRECTLY running it on ${HOSTNAME}."
    exit
fi


echo "Starting slaves..."
ROCKS_BIN=/opt/rocks/bin/rocks
DGR_BIN='./strace -o /dev/null ./fitashape_S'
DIR=`pwd`

${ROCKS_BIN} run host tile-0-0 command="cd $DIR && DISPLAY=tile-0-0:0.0 ${DGR_BIN} -3.09 0 -0.28  .3  5760 1080" &
${ROCKS_BIN} run host tile-0-1 command="cd $DIR && DISPLAY=tile-0-1:0.0 ${DGR_BIN} -3.09 0 .3  .88     5760 1080" &
${ROCKS_BIN} run host tile-0-2 command="cd $DIR && DISPLAY=tile-0-2:0.0 ${DGR_BIN} -3.09 0 .88  1.46  5760 1080" &
${ROCKS_BIN} run host tile-0-3 command="cd $DIR && DISPLAY=tile-0-3:0.0 ${DGR_BIN} -3.09 0 1.46  2.04   5760 1080" &

${ROCKS_BIN} run host tile-0-4 command="cd $DIR && DISPLAY=tile-0-4:0.0 ${DGR_BIN} 0 3.09  -0.28  .3  5760 1080" &
${ROCKS_BIN} run host tile-0-5 command="cd $DIR && DISPLAY=tile-0-5:0.0 ${DGR_BIN} 0 3.09  .3  .88    5760 1080" &
${ROCKS_BIN} run host tile-0-6 command="cd $DIR && DISPLAY=tile-0-6:0.0 ${DGR_BIN} 0 3.09  .88  1.46  5760 1080" &
${ROCKS_BIN} run host tile-0-7 command="cd $DIR && DISPLAY=tile-0-7:0.0 ${DGR_BIN} 0 3.09  1.46  2.04   5760 1080" &

echo "Starting relay..."
./strace -o /dev/null ./fitashape_R 10.2.255.255 &

