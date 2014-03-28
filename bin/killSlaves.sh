#!/usr/bin/env bash

set -e # exit script if any command returns a non-zero exit code.

# The hostname that we expect to be running on:

ROCKS_BIN=/opt/rocks/bin/rocks
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-0 command=\"killall DGRSlave\""
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-1 command=\"killall DGRSlave\""
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-2 command=\"killall DGRSlave\""
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-3 command=\"killall DGRSlave\""
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-4 command=\"killall DGRSlave\""
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-5 command=\"killall DGRSlave\""
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-6 command=\"killall DGRSlave\""
ssh ivs.research.mtu.edu "/opt/rocks/bin/rocks run host tile-0-7 command=\"killall DGRSlave\""