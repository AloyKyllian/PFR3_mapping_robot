# PFR3

# ROBOT

## COMMUNICATION BLUETOOTH

## CONTROLE AUTOMATIQUE

## ASSERVISSEMENT DES MOTEUR

# TELECOMMANDE

## COMMUNICATION BLUETOOTH

## AFFICHEUR OLED

## JOYSTICK

# connectique

## PIN Digitale

3 -> moteur lidar
18 19 -> communication lidar
17 16 -> communication bluetooth

# FONCTIONNEMENT

la telecommande se connecte au robot et un appui du joystick fais passer le robot en mode automatique.
une fois en automatique le robot va suivre le mur a sa droite (ou sa gauche) sa position d'origine est le (0,0).
Le lidar va detecter entre 45° et 90° le mur et va envoyé ces information a la telecommande.
Puis le robot va avancer de XXX centimetre et le lidar reprend les information de distance et d'angle avec le mur suivi et les renvoie a la telecommande.
le robot tourne en boucle tant qu'un appui sur le joystick de la telecommande n'a pas ete effectué

La teleccommande peut passer le robot en automatique avec un appui sur le joystick si on réappuie on passe en mode controlable donc la telecommande permet de diriger le robot grace au joystick
Quand on recois des information on les affiche directement sur l'écran OLED

petit rappel

il y a deux mode automatique et manuel.
Dans les deux mode le bot envoye les informations du lidar tout les XXX centimetres parcourue.

En automatique le robot suit un mur et tourne quand il detecte un mur devant lui.

En manuel le robot est controlé par le joystick.

# TACHE A FAIRE

(KYLLIAN et YOUSS) [] faire rouler le robot en fonction du joystick
(KYLLIAN)[x] recuperer les valeur de joystick
(KYLLIAN et YOUSS)[x] envoier et recevoir les valeurs de joystick
#(KYLLIAN)[] interpreter les valeurs de joystick afin de controler le robot

(KYLLIAN et YOUSS)[] faire la prise d'information du lidar
(KYLLIAN)[x] recuperer une donne d'angle et de distance par rapport a un mur
#(YOUSS)[] recuperation de plusieurs valeurs de lidar
(YOUSS)[] interpretation en fonction de la position du robot
(YOUSS)[] adaptation en fonction de l'afficheur

#()[] recuperation des valeurs d'encodeur (distance parcourue)
#()[] {alternative} recuperation de valeur d'accelerometre et derivation

(LOIC)[] automatisation de la conduite du robot
(LOIC)[x] recuperation des valeurs de distance avec les capteurs ultrasons
#()[] interpretation de ses valeurs
#()[] faire tourner le robot s'il y a un mur devant

OPTIONNELLE

(HAYTAM)[] asservissement des moteurs
