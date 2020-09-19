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
   git checkout master
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
echo "2   Emulation de COS2000" 
echo "3   Débuguage de COS2000 boot en mode réel (boot)"
echo "4   Débuguage de COS2000 boot en mode réel (loader)"
echo "5   Débuguage de COS2000"
echo "6   Réalisez une sauvegarde"
echo "7   Tuer tout les processus"
echo "8   Nettoyer les sources"
echo "9   Voir le disque en hexadécimal"
echo "c   Changer la version de developpement"
echo "o   Changer les options de développement"
echo "0   Quitter"
echo "------------------------------------------"
echo "Choisissez l'action à réaliser..."
read answer
clear
case "${answer}" in
0) exit;;
1) ./make.sh all;;
2) ./make.sh test;;
3) ./make.sh debug-boot;;
4) ./make.sh debug-loader;;
5) ./make.sh debug-system;;
6) ./make.sh backup;;
7) ./make.sh killer;;
8) ./make.sh clean;;
9) ./make.sh view|more;;
o*) ./make.sh config
./make.sh tool
;; 
c*) echo "Version disponibles:"
SELECT=$(git branch|grep "*"|tr -d "* ")
NUM=1
echo "XX  Hash d'une révision particulière" 
while read LINE
do
   echo "${NUM}   ${LINE}"
   (( NUM++ )) 
done < <(git branch -lr|tr -d "* "|grep -v HEAD|sed s/".*origin\/"//)
read -p"?" ANSWER
COUNT=$(echo -n ${ANSWER}|wc -c)
if [ "${COUNT}" == "1" ]; then
	CHOOSE=$(git branch -lr|tr -d "* "|grep -v HEAD|sed s/".*origin\/"//|tr "\n" " "|cut -d " " -f${ANSWER})
	echo "vous avez selectionné une branche $CHOOSE..."
else
	EXIST=$(git show ${ANSWER})
	if [ "${EXIST}" != "" ]; then
		CHOOSE=${ANSWER}
		echo "vous avez selectionné une révision ${choose}..."
	fi
fi
if [ "${CHOOSE}" != "" ]; then
	echo "*** Application de la version ${CHOOSE}"
	git checkout $CHOOSE
	git clean -fd
	git reset --hard
	git pull -f
	./make.sh tool
fi
read
;;
esac


done



