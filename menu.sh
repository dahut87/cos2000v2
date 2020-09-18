#!/bin/bash
PWD=$(pwd)
FILE=${PWD}/README.md
if [ ! -f "$FILE" ]; then
   echo "Les sources ne sont pas complets..."
   echo "* Récupération des sources"
   VERSION=$(git --version|grep version)   
   if [ "${VERSION}" == "" ]; then
	echo "Git n'est pas installé, veuillez l'installer et relancer le script"
   fi
   git clone https://github.com/dahut87/cos2000v2.git
   cd ${PWD}/cos2000v2
   git checkout develop
   git clean -fd
   git reset --hard
   rm -rf ../menu.sh
fi
while true
do
clear
echo "**********************************************************"
echo "*          COS2000 - menu de compilation                 *"
echo "**********************************************************"
echo
echo "1   Compilation des sources"
echo "2l  Emulation de COS2000 (legacy)" 
echo "2u  Emulation de COS2000 (uefi)" 
echo "3l  Débuguage de COS2000 (legacy)"
echo "3u  Débuguage de COS2000 (uefi)"
echo "4l  Débuguage de COS2000 boot en mode réel (legacy)"
echo "4u  Débuguage de COS2000 boot en mode réel (legacy)"
echo "5   Réalisez une sauvegarde"
echo "6   Tuer tout les processus"
echo "7   Nettoyer les sources"
echo "8   Voir le disque en hexadécimal"
echo "9   Voir le système en hexadécimal"
echo "c   Changer la version de developpement"
echo "0   Quitter"
echo "------------------------------------------"
echo "Choisissez l'action à réaliser..."
read answer
clear
case "${answer}" in
0*) exit;;
1*) ./make.sh all;;
2l) ./make.sh test32;;
2*) ./make.sh test64;;
3l) ./make.sh debug32;;
3*) ./make.sh debug64;;
4l) ./make.sh debug-boot32;;
4*) ./make.sh debug-boot64;;
5*) ./make.sh backup;;
6*) ./make.sh killer;;
7*) ./make.sh clean;;
8*) ./make.sh view-dsk|more;;
9*) ./make.sh view-sys|more;;
c*) echo "Version disponibles:"
SELECT=$(git branch|grep "*"|tr -d "* ")
NUM=1
while read LINE
do
   echo "${NUM}   ${LINE}"
   (( NUM++ )) 
done < <(git branch -lr|tr -d "* "|grep -v HEAD|sed s/".*origin\/"//)
read ANSWER
CHOOSE=$(git branch -lr|tr -d "* "|grep -v HEAD|sed s/".*origin\/"//|tr "\n" " "|cut -d " " -f${ANSWER})
echo "vous avez selectionné $ALL..."
git checkout $CHOOSE
git clean -fd
git reset --hard
read
;;
esac


done


