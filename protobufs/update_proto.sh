#!/bin/sh

#
# This file will checkout the latest Dota2 repo and regenerate the dota 2 protobufs, no config required
# Installation of Protobuf library required: https://code.google.com/p/protobuf/downloads/list
#
TMP_FOLDER="/tmp/Dota2"
DOTA_PROTOS_PATH="$TMP_FOLDER/Resources/Protobufs/dota"

git clone https://github.com/paralin/Dota2.git $TMP_FOLDER

# this will link the necessary google protos
ln -s $DOTA_PROTOS_PATH/../google $DOTA_PROTOS_PATH

# list of files to be generated
for FILENAME in $DOTA_PROTOS_PATH/*
do
	/usr/local/bin/protoc --cpp_out=/home/devilesk/aegis/protobufs --proto_path $DOTA_PROTOS_PATH $DOTA_PROTOS_PATH/${FILENAME##*/}
	echo "generated ${FILENAME##*/}"
done

rm -rf $TMP_FOLDER

echo "done!"

