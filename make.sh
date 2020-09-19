#!/bin/bash

function tool {
	echo "*** Fabrication des outils de compilation par Docker (version ${VERSION} config ${CONFIGURATION})"
	docker build - -t $COMPILER < ./docker/dockerfile.${CONFIGURATION}
}

function configuration {
   	echo "*** Création du fichier de configuration"
	echo -en "compilation=alpine\nvideo=vesa" > ./configuration
}

echo "Autorisation de docker à se connecter à l'écran :0"
xhost +local:docker
PWD=$(pwd)
FILE=${PWD}/configuration
if [ ! -f "$FILE" ]; then
	configuration
fi
DOCKER=$(docker -v| grep build)
if [ "${DOCKER}"  == "" ]; then
	echo "Docker n'est pas installé..."
	echo "*** installation si sous gestionnaire de paquet debian (.deb)"
	sudo apt-get install docker
	exit
fi
CONFIGURATION=$(sed -rn 's/^compilation=([^\n]+)$/\1/p' ./configuration)
VERSION=$(git rev-parse --short HEAD)
COMPILER=cos2000-compiler-${CONFIGURATION}-${VERSION}
if [ "$1" == "tool" ]; then
	tool
	exit
fi
if [ "$1" == "configuration" ]; then
	configuration
	exit
fi
PRESENT=$(docker image ls| grep $COMPILER)
if [ "${PRESENT}"  == "" ]; then
	tool
fi
echo "*** lancement de la version ${VERSION}"
if [ "$1"  == "config" ]; then
	docker run --rm -it -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --privileged -v ${PWD}:/data $COMPILER make $1
else
	docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --privileged -v ${PWD}:/data $COMPILER make $1
fi
