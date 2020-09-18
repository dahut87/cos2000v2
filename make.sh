#!/bin/bash
echo "Autorisation de docker à se connecter à l'écran :0"
xhost +local:docker
DOCKER=$(docker -v| grep build)
ALPINE=$(docker image ls| grep cos2000-compiler-alpine)
COMPILER=$(docker image ls| grep cos2000-compiler)
PWD=$(pwd)
if [ "${DOCKER}"  == "" ]; then
	echo "Docker n'est pas installé..."
	echo "*** installation si sous gestionnaire de paquet debian (.deb)"
	sudo apt-get install docker
	exit 
elif [ "${ALPINE}"  != "" ]; then
	echo "*** lancement de la version Alpine Linux"
	docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --privileged -v ${PWD}:/data cos2000-compiler-alpine make $1
elif [ "${COMPILER}"  != "" ]; then
	echo "*** lancement de la version Debian"
	docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --privileged -v ${PWD}:/data cos2000-compiler make $1
else
	echo "Docker est installé mais aucune image docker n'est fonctionnelle..."
	echo "*** compilation de l'image docker"
	docker build ${PWD}/docker/alpine -t cos2000-compiler-alpine
fi
