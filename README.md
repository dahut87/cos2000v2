![logo](https://github.com/dahut87/cos2000v2/raw/develop/Graphisme/logo.png)

## Présentation du projet

### Qu'est ce que COS2000 ?

COS2000 est un système d'exploitation minimaliste qui vise essentiellement un objectif pédagogique.
Il s'agit avant tout du travail d'un passionné de programmation système.

Le système est développé essentiellement en langage C mais il comporte aussi un peu d'assembleur X86.

#### Qu'est ce qu'un système d'exploitation ?

Il s'agit du logiciel principal qui anime votre ordinateur. Il a pour fonctions principales :

* Piloter des périphériques tels que souris, clavier, écran, imprimante...etc;
* Gérer des fichiers qui sont produit et exploités par des applications; 
* Coordonner l'accès aux ressources par plusieurs utilisateurs;
* Détection et récupération des dysfonctionnements;
* Surveiller les performances et assurer une gestion optimale des ressources systèmes.

Sans système d'exploitation votre ordinateur est inopérant: c'est une boite vide !

#### Comment utiliser COS2000 ?

COS2000 n'a pas pour but d'être utilisé en production. Il s'agit d'un système en cours de développement que vous pouvez néanmoins tester sur un ordinateur physique ou de préférence sur une machine virtuelle. Il est nécessaire de compiler le système avant de démarrer celui-ci à partir d'un périphérique amovible (clé usb).

#### Sur quel ordinateur fonctionne t'il ?

COS2000 est compatible avec tout ordinateur compatible PC comprenant un processeur avec FPU intégré,
gestion de la mémoire paginée et mode protégé : Intel Pentium et supérieur.

#### Ai-je le droit de le copier, de le modifier, de le réutiliser, de le vendre ?

COS2000 est sous licence LGPL v3.0, en simplifiant un peu :

* COPIER     - OUI
* MODIFIER   - OUI
* REUTILISER - OUI en citant l'auteur
* VENDRE     - NON

Les détails se trouvent dans le fichier `LICENSE.md` 

![logo](https://www.gnu.org/graphics/lgplv3-with-text-154x68.png)

### Compilation de COS2000

#### Avant la compilation

##### Logiciels utilisés

L'OS a été développé sous Linux afin de disposer d'un maximum d'outils dont GCC/GAS/Docker et l'ensemble de BINUTILS. Je vous conseille donc d'utiliser Linux afin de procéder à la compilation même si celle-ci peut fonctionner depuis Windows grace à Cygwin. Des distributions "Live" existent si vous ne souhaitez pas installer sur votre ordinateur (http://www.linuxmint.com/edition.php?id=259).

Je conseille l'usage du debogueur GDB avec l'ajout de [GDB dashboard - Modular visual interface for GDB in Python](https://github.com/cyrus-and/gdb-dashboard) qui permet de visualiser le contenu des registres CPU en permanence. 

Le système de conteneurisation Docker est exploité afin de d'assurer une reproductibilité totale de l'environnement de compilation quelque soit la distribution linux utilisée.

##### Téléchargement de COS2000

Afin de faciliter la mise à jour et le suivi des version de COS2000, GIT est utilisé.

Si vous n'avez pas GIT, installez le avec la commande suivante :

`sudo apt-get install git`

Puis cloner le source chez vous avec la commande :

`git clone https://github.com/dahut87/cos2000.git`

#### Compilation
  
Docker est utilisé afin de permettre le fonctionnement correcte de la compilation. Au premier lancement de make.sh, si vous utilisez une distribution basée sur le système de paquet Debian, l'installation sera réalisée automatiquement. Dans le cas contraire, installez Docker et relancez le script. Une image docker est produite afin de procéder ensuite à la compilation du système.

Commande de compilation de base

* `./make.sh all` compile tout le projet dans sa version disque dur et disque dur en UEFI

Autres commandes de compilation de COS2000

* `./make.sh harddisk` compile la version disque dur
* `./make.sh uefi` compile la version disque dur UEFI
* `./make.sh bochstest` lance l'émulation BOCHS en 32 bits sur disque dur
* `./make.sh test` lance l'émulation QEMU en 32 bits sur disque dur
* `./make.sh test64` lance l'émulation QEMU en 64 bits sur disque dur en UEFI
* `./make.sh clean` supprime les fichers compilés
* `./make.sh syscall` réactualise les librairies du domaine utilisateur
* `./make.sh programs` compile les programmes du domaine utilisateur
* `VESA=no ./make.sh test` préfixe à utiliser (VESA=no) pour faire appel au pilote VGA et non pas VESA

#### Utilisation

##### Sur un ordinateur émulé

Pour tester l'OS en émulation taper donc la commande `./make.sh test` qui compilera avant de lancer Qemu.

##### Sur un ordinateur physique

Lancer une compilation du système COS2000

`./make.sh all` 

Puis, il faut copier l'image sur une clé (Attention l'opération effacera le contenu de la clé) :

`sudo dd if=./final/harddisk.img.final of=/dev/sdx bs=1M` (ou sdx est votre périphérique)

Bootez sur votre clé en mode bios (legacy).

Pour un boot en mode UEFI

`sudo dd if=./final/harddiskuefi.img.final of=/dev/sdx bs=1M` (ou sdx est votre périphérique)

Pour connaitre le numéro de votre périphérique (clé)

`lsblk`

##### Usage de COS2000

Pour l'instant quelques commandes seulement sont disponibles:

* `REBOOT` redémarre le PC,
* `CLEAR` efface l'écran,
* `MODE` change le mode video,
* `DETECTCPU` detecte et affiche les informations CPU,
* `DETECTPCI` detecte et affiche les périphériques PCI,
* `TEST2D` teste l'affichage graphique 2D,
* `TEST3D` teste l'affichage graphique 2D,
* `REGS` affiche les registres CPU,
* `GDT` affiche la table des descripteurs,
* `IDT` affiche la table des interruptions,
* `MEM` affiche les statistiques d'occupation memoire,
* `INFO` affiche des informations issues de GRUB,
* `ERR` génère une exception (ARGUMENTS),
* `VIEW` visionne la mémoire vive (ARGUMENTS),
* `LOGO` affiche le logo,
* `FONT` change la police d'affichage (ARGUMENTS),
* `HELP` affiche les commandes disponibles,
* `BPSET` met un point d'arrêt pour le débogueur (ARGUMENTS),
* `BPCLR` efface un point d'arrêt (ARGUMENTS),
* `DISASM` désassemble une portion de mémoire (ARGUMENTS),

![COS2000 le 28-09-2018](https://github.com/dahut87/cos2000v2/raw/develop/Graphisme/screenshots/28-09-2018.png)

#### Organisation du dépôt

* `debug` - fichiers configuration débogueur
* `docker` - fichiers pour la construction de containers
* `final` - img raw utilisables avec un émulateur des 3 versions
* * `harddisk.img.xz`
* * `harddiskuefi.img.xz`
* `Graphisme` - fichiers images
* *  `screenshots` - screenshots de l'évolution du système
* `include` - fichier d'entête C
* `lib` - librairies pour le noyau
* `makefile` - Makefile du projet
* `make.sh` - Script de compilation utilisant Docker
* `templates` - Modèles utilisés pour générer des libraires du domaine utilisateur
* `programs` - programmes pour le domaine utilisateur
* *  `include` - fichier d'entête C
* *  `lib` - librairies pour le domaine utilisateur
* `README.md` - ce que vous lisez
* `LICENSE.md` - la licence LGPLv3.0
* `INFORMATIONS.md` - informations sur le développement de COS2000
* `BUGS.md` - bugs rélevés dans le système
* `API.md` - Liste des APIs fournies par le système COS2000
* `system` - le noyau lui-même

### En savoir plus...

#### Avancement du projet

* démarrage tout support grâce à GRUB (disquette, disque dur/SSD, réseau...Etc),
* pilotes souris et clavier,
* pilotes VGA et VESA (framebuffer),
* bibliothèque graphique 2D,
* bibliothèque graphique 3D,
* gestion des interruptions (IDT),
* gestion de la segmentation (GDT),
* gestion de la mémoire paginée,
* débogueur et désassembleur intégré,
* interpréteur de commande,
* affichage de chaîne de caractères (prinfs,sprintf,vsprintf) avec type (bin,hexa,octal,float,double,string,char),
* mode protégé limité à 4Go de mémoire vive (32 bits),
* gestion avancée de la mémoire (vmalloc).
* chargeur ELF32,
* espace utilisateur et appels systèmes,
* ordonnanceur de tâche (par TSS),

#### En cours

* correction de bogues - libération des ressources après destruction processus,
* liste d'API automatiquement mise à jour avec intégration de librairies & header,
* gestion du système de fichier CRAMFS,
* lancement du noyau par kernel et non par multiboot afin de bénéficier de initrd, 

#### A faire

* pilote IDE/ATA (PIO mode),
* fonctions affichage image PNG,
* double buffering,
* gestion multiprocessing,
* virtualisation par VMX,
* Gestion du système de fichier EXT2FS.

![COS2000 le 29-11-2018](https://github.com/dahut87/cos2000v2/raw/develop/Graphisme/screenshots/29-11-2018.png)

#### Autres Licences (autres auteurs)

Des fichiers sources utilisés par COS2000 sont sous d'autres licences, parmis ceux-ci figurent :

* `include/queues.h` sous licence Berkeley Software Distribution License
* `tools/*` sous licence GPL V2.0, fichiers issus du noyau Linux

#### Historique du projet
* Version 2.2fr - C en mode protégé Reprise du projet
* Version 2.1fr - C en mode protégé Abandon du projet
* Version 2.0   - C en mode protégé VGA text+pmode
* Version 1.x   - Assembleur en mode réel (v1.0 - 1.4.2fr)

>  “La connaissance s'acquiert par l'expérience,
>          tout le reste n'est que de l'information..
>
>   ― Albert Einstein
>   ― Mathématicien, Physicien
