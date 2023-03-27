#jeu d'échec en python. Début du code 18/03/23 15h 05

import pygame
import os
from utils import *
from gestion import*
from class_tuile import*
from class_pion import*

pygame.init()
init_échiquier()
init_les_pions()


#----------------------------------------------------------#
#----------------------------------------------------------#

clic_gauche = 1 
clic_droit = 3  
pion_à_affichier_les_solutions = None
afficher_ses_solutions = False
cx = 0 #case x sélectionnée
cy = 0 #case y sélectionnée 
dx = 0 #destination x
dy = 0 #destination y
pion_sélectionné = False
destination_sélectionné = False

def gestion_des_événements():

	global go, pion_sélectionné, destination_sélectionné
	global surface_écran
	global cx, cy, afficher_ses_solutions

	for év in pygame.event.get():

		if év.type == pygame.QUIT :
			go = False

		elif év.type == pygame.KEYDOWN :
			pass

		elif év.type == pygame.KEYUP:
			pass



		elif év.type == pygame.MOUSEBUTTONDOWN:

			if év.button == clic_gauche:
				resultat = est_ce_dans_l_échiquier(év.pos) 
																			
				if resultat[0] == True:		


					if est_ce_un_pion(resultat[1]):
						p = donne_le_pion(resultat[1])
						
						if pion_sélectionné:

							if p.couleur != Gestion.le_joueur_courant():
								dx, dy = resultat[1]
								destination_sélectionné = True
							else:
								destination_sélectionné = False

						
						else:
							if p.couleur == Gestion.le_joueur_courant():
								cx, cy = resultat[1]
								pion_sélectionné = True 
								afficher_ses_solutions = True 




					else:
						if pion_sélectionné:
							dx, dy = resultat[1]
							destination_sélectionné = True

						else:
							pion_sélectionné = False
						





																			
				# 	case = Echiquier[resultat[1][0]][resultat[1][1]] 											
				# 	cx, cy = resultat[1]  

				# 	if case.pion != None:

				# 		afficher_ses_solutions = True
				# 		pion_sélectionné = True 
				# 		# destination_sélectionné = False

					
				# 	# elif pion_sélectionné :
				# 	else:
				# 		print("- ", end='')
				# 		destination_sélectionné = True 
				# 		dx, dy = cx, cy 
				# 	# else:
				# 		# pion_sélectionné = False

				# 		# afficher_ses_solutions = False


																	
						

#----------------------------------------------------------#
#----------------------------------------------------------#





surface_écran = pygame.display.set_mode([écran_largeur, écran_hauteur])
clock = pygame.time.Clock()
pygame.display.set_caption("échec-python")

go = True
maj_dispo = True



tileset = pygame.image.load("Ress/IMG/tiles.png")
tileset = pygame.transform.scale(tileset, (320*2, 64*2))

icon = pygame.Surface((64, 64))
icon.blit(tileset, (0, 0), (0, 0, 64, 64))
icon.set_colorkey((0,0,0))
icon = pygame.display.set_icon(icon)


déplacer([1, 7], [2, 5])
déplacer([2, 5], [0, 4])




#__________________________________________________________#
#__________________________________________________________#

rond  = lambda coo: pygame.draw.circle(surface_écran, couleur_rouge, [coo[0], coo[1]], 12)
rond1 = lambda coo: pygame.draw.circle(surface_écran, couleur_verte, [coo[0], coo[1]], 12)

def mise_à_jour():
	pass

#__________________________________________________________#
#__________________________________________________________#

#__________________________________________________________#
#__________________________________________________________#
def dessiner_le_tout():

	global maj_dispo, afficher_ses_solutions, couleur_verte
	global destination_sélectionné, pion_sélectionné	




	surface_écran.fill(couleur_de_fond)

	pygame.draw.rect(surface_écran, cadre_couleur, cadre_rect, cadre_épaisseur)

	dessiner_échiquier(surface_écran, tileset)




	if pion_sélectionné : 
		rond1((50, 50))
	else:
		rond((50, 50))
	if destination_sélectionné : 
		rond1((710, 50))
	else:
		rond((710, 50))
					



	pion = Echiquier[cy][cx].pion  											
	if afficher_ses_solutions:
																			
		if pion != None:
			#pour plus tard : if pion.couleur == joueur_courant:
			dessiner_les_solutions_d_un_pion(surface_écran, pion)
																			# mp = pion.mouv_dispo(Echiquier)
																			# # print("i", end ="")

																			# for pos in mp: 
																			# 	x = pos[0]*BLOC + échiquier_coo[0] + 23
																			# 	y = pos[1]*BLOC + échiquier_coo[1] + 23

																			# 	rect = [x, y, 20, 20]
																			# 	# pygame.draw.rect(surface_écran, couleur_verte, rect)
																			# 	pygame.draw.circle(surface_écran, couleur_verte, (rect[0]+10, rect[1]+10), rect[2]-10)


	if pion_sélectionné and destination_sélectionné:
		res = déplacer([cx, cy], [dx, dy])
		# print("j", end = '')

		if res == True or res == False:
			pion_sélectionné = False
			destination_sélectionné = False
			# couleur_verte = couleur_bleue




	#---------------------------------#
	for ligne in Echiquier:
		for c in ligne:
			if c.état == BLANC:

				x = c.pos[0]*BLOC + échiquier_coo[0] + 35
				y = c.pos[1]*BLOC + échiquier_coo[1] + 35
				rect = [x, y, 10, 10]
				pygame.draw.rect(surface_écran, couleur_rouge, rect)
	for ligne in Echiquier:
		for c in ligne:
			if c.état == NOIR:

				x = c.pos[0]*BLOC + échiquier_coo[0] + 35
				y = c.pos[1]*BLOC + échiquier_coo[1] + 35
				rect = [x, y, 10, 10]
				pygame.draw.rect(surface_écran, couleur_jaune, rect)
	#---------------------------------#




	




	pygame.display.flip()
	maj_dispo = False



#__________________________________________________________#
#__________________________________________________________#











#----------------------------------------------------------#
#----------------------------------------------------------#
while go:

	gestion_des_événements()
	dessiner_le_tout()
	
	clock.tick(FPS)

#----------------------------------------------------------#
#----------------------------------------------------------#



"""

Bilan des jours :

- j1 : dessin de l'échiquier avec tous les pions
- j2 : les règle de déplacement des différents pions sont mises en place
- j3 : résolution d'un problème technique rencontré 
- j4 : module de déplacement 
- j5 : repos 
- j6 : impossiblité de d'avancer sur le projet 

"""