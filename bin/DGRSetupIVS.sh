#!/usr/bin/env bash
# exit script if any command returns a non-zero exit code.

# The hostname that we expect to be running on:
EXPECT_HOSTNAME="ccsr.ee.mtu.edu"
RSYNC_IGNORE=.svn,.git,lib/irrlicht/,docs,README,*_M,*_R,*_S,*.o

if [[ ${HOSTNAME} != ${EXPECT_HOSTNAME} ]]; then
    echo "This script ($0) is intended to work only on $EXPECT_HOSTNAME. You are running it from $HOSTNAME."
	echo "If you are in the IVS lab and running this script from your laptop, press Enter."
	echo "If you are NOT in the IVS lab, press Ctrl+C."
	read
fi


IVS_HOSTNAME=ivs.research.mtu.edu
echo
ls
echo
DEST_DIR=/research/bjnix/temp-fitashape-dgr
DEST_BIN=/research/bjnix/temp-fitashape-dgr/bin

echo "I will recursively copy the above files into:"
echo "bjnix@${IVS_HOSTNAME}:${DEST_DIR}"
echo "Press Ctrl+C to cancel or any other key to continue."
read

echo "You may have to enter your password multiple times..."
# make directory in case it doesn't exist
ssh bjnix@${IVS_HOSTNAME} mkdir -p $DEST_DIR
# recursively copy files over.
#scp -r * "bjnix@${IVS_HOSTNAME}:$DEST_DIR"
cd ../
rsync -ah -e ssh --exclude={.svn,.git,lib/irrlicht/,docs,README,*_M,*_R,*_S,*.o} --checksum --partial --no-whole-file --inplace --progress . ${IVS_HOSTNAME}:$DEST_DIR
cd ./bin

#copy irrlicht library into the correct place
ssh bjnix@${IVS_HOSTNAME} "cp ${DEST_DIR}/lib/irrlicht_ivs/libIrrlicht.a ${DEST_DIR}/lib/irrlicht/"

# recompile for safety's sake
ssh bjnix@${IVS_HOSTNAME} "cd ${DEST_DIR}/bin && make"
	
cp ../lib/irrlicht_ccsr/libIrrlicht.a ../lib/irrlicht/
make
