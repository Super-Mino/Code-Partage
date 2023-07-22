#Jeu d'échec en python. Début du code 18/03/23 15h 05


"""
L'objectif était de faire un programme d'échec en Python avec un code codé au maximum possible 
en français pour une fois. 

Je ne suis pas encore un codeur professionnel, j'ai fait au mieux pour avoir un code propre 
et limpide.

En ce qui concerne ce programme : 

	- certains mots comme 'coo' et 'posisition', ou 'case' , 'carreau' et 'tuile' sont nécessairement des synonymes 
	- les coordonné des cases et de pions sont sur le même plan (si une case et un pion en jeu
		ont les mêmes coordonnées alors le pion est sur la case) 
	- ces coordonnées sont traitées comme les cases d'un tableau à double entrée. 
		Les coordonnées en pixel sont déduis uniquement pour l'affichage. 
	- grosso modo les fonctions ont des paramètres avec les mêmes noms que les arguments voulus.
		Par exemple 'surface_écran' et 'tileset' gardent nécessairement les mêmes noms dans les corps de fonction 
	- j'ai essayé de respecter les règles communes de base (c-à-d qu'il n'y a peut-être pas toutes
		les subtilités de cet art) 
	- libre à vous de l'améliorer, étudier, partager ..., et pas besoin de reconnaissance particulière (mais cela est apprécié) 
	- pour toutes utilisations douteuses, coupables ou illégales et j'en passe, toutes responsabilités 
		des faits revient sur vous et vous seul-s. 

Ce code a un objectif purement ludique ;) 

Garder ces écrits avec le programme est apprécié mais pas obligatoire.
"""




import pygame
import os
from utils import *
from gestion import*
from class_tuile import*
from class_pion import*

pygame.init()






def reinitialiser():
	global un_pion_est_déjà_séléctionné, destination_séléctionné
	tirerLesDésSurLesPlacesDeQuiEtQui()
	init_échiquier()
	init_les_pions()
	Gestion.reinitialiser()
	Prisons.reinitialiser()
	Animations.reinitialiser()
	un_pion_est_déjà_séléctionné = False
	destination_séléctionné = False
	






#----------------------------------------------------------#
#----------------------------------------------------------#

clic_gauche = 1 
clic_droit = 3  
pion_à_affichier_les_solutions = None
afficher_ses_solutions = False
src_x = 0 #case x sélectionnée
src_y = 0 #case y sélectionnée 
dest_x = 0 #destination x sélectionnée
dest_y = 0 #destination y sélectionnée
un_pion_est_déjà_séléctionné = False
destination_séléctionné = False








surface_écran = pygame.display.set_mode([écran_largeur, écran_hauteur])
clock = pygame.time.Clock()
pygame.display.set_caption(TITRE_FENETRE)

go = True
# maj_dispo = True

définition_des_tiles = 32 #pixel

tileset = pygame.image.load(CHEMIN_TILES)
tileset = pygame.transform.scale(tileset, (320*(BLOC/définition_des_tiles), 64*(BLOC/définition_des_tiles)))

Animations.init_écran_de_promotion(tileset)
mettre_tileset(tileset)


"""
Ici je double simplement le dimensions de mon sprite sheet (qui fait 320*64 pixel de base) 
car les sprite sont en 32 pixel width height et je requiert des sprite en 64 W H. 
Pour ne pas déformer l'image (la surface nouvellement simulé) qui sera donc 2 fois plus grande, 
je garde les mêmes proportions en multipliant les dimensions par un même nombre (ici 2). 
"""

icon = pygame.Surface((BLOC, BLOC))
icon.blit(tileset, (0, 0), (0, 0, BLOC, BLOC))
icon.set_colorkey((0,0,0))
icon = pygame.display.set_icon(icon)


img_fond = pygame.image.load(CHEMIN_IMAGE_FOND)
img_fond = pygame.transform.scale(img_fond, [écran_largeur, écran_hauteur])





#################

reinitialiser() 

#################




















#__________________________________________________________#
#__________________________________________________________#



def gestion_des_événements():

	global go, un_pion_est_déjà_séléctionné, destination_séléctionné
	global surface_écran, dest_x, dest_y
	global src_x, src_y, afficher_ses_solutions

	for év in pygame.event.get():

		if év.type == pygame.QUIT :
			go = False

		elif év.type == pygame.KEYDOWN :
			pass

		elif év.type == pygame.KEYUP:
			pass





		elif év.type == pygame.MOUSEBUTTONUP:
			if év.button == clic_gauche:
				if Animations.ça_glisse:
					Animations.pion_ne_glisse_plus()

					resultat = est_ce_dans_l_échiquier(év.pos) 
																			
					if resultat[0] == True:		


						if est_ce_un_pion(resultat[1]) == False:
							if un_pion_est_déjà_séléctionné:
								dest_x, dest_y = resultat[1]
								destination_séléctionné = True
								Animations.par_glissade = True

						else:
							
							p = donne_le_pion(resultat[1])
							if un_pion_est_déjà_séléctionné:

								if p.couleur != Gestion.le_joueur_courant():
									dest_x, dest_y = resultat[1]
									destination_séléctionné = True
									Animations.par_glissade = True


			elif év.button == clic_droit: #on annule l'action entrepris
				if Animations.ça_glisse:
					Animations.pion_ne_glisse_plus()

				un_pion_est_déjà_séléctionné=False
				destination_séléctionné=False
				afficher_ses_solutions = False



		elif év.type == pygame.MOUSEBUTTONDOWN:

			Animations.gestion_des_événements(év)
			Gestion.gestion_des_événements_autres_bouttons(év)
 

			if év.button == clic_gauche:

				if Animations.ça_glisse:
					Animations.pion_ne_glisse_plus()


				resultat = est_ce_dans_l_échiquier(év.pos) 
																			
				if resultat[0] == True:		


					if est_ce_un_pion(resultat[1]):
						p = donne_le_pion(resultat[1])
						
						if un_pion_est_déjà_séléctionné:

							if p.couleur != Gestion.le_joueur_courant():
								dest_x, dest_y = resultat[1]
								destination_séléctionné = True
								
							else:
								src_x, src_y = resultat[1]
								destination_séléctionné = False 

						
						else:
							if p.couleur == Gestion.le_joueur_courant():
								src_x, src_y = resultat[1]
								un_pion_est_déjà_séléctionné = True 
								afficher_ses_solutions = True 




					else:
						if un_pion_est_déjà_séléctionné:
							dest_x, dest_y = resultat[1]
							destination_séléctionné = True
			
						afficher_ses_solutions = False
						





























#__________________________________________________________#
#__________________________________________________________#


def mise_à_jour():
	global roi_noir_en_mat, roi_blanc_en_mat 
	global roi_blanc_en_échec, roi_noir_en_échec 
	global afficher_ses_solutions, un_pion_est_déjà_séléctionné
	global destination_séléctionné, go	


	if Gestion.partie_arrêtée:
		un_pion_est_déjà_séléctionné = False
		destination_séléctionné = False
		afficher_ses_solutions = False
		



	#Testes de l'état de la partie
	Gestion.si_le_joueur_courant_en_échec_mat()
	Gestion.si_situation_de_pat()
	Gestion.si_matériel_insuffisant()
	Gestion.si_match_nul_d_un_commun_accord()
		




	if un_pion_est_déjà_séléctionné: #si pion sélectionné alors on affiche ses mouvements
		afficher_ses_solutions = True
		





	if un_pion_est_déjà_séléctionné and destination_séléctionné:
		res = déplacer([src_x, src_y], [dest_x, dest_y])
		
		destination_séléctionné = False

		if res == True:
			un_pion_est_déjà_séléctionné = False
			Gestion.alterner_les_rôles() 
		else:
			Animations.par_glissade = False 



	vérifier_les_promotions()
	traiter_les_promotions()
	traiter_les_prises_en_passant()



	if un_pion_est_déjà_séléctionné:
		if not Animations.ça_glisse:
			if pygame.mouse.get_pressed()[0]:

				#vérification pour ne prendre en glissade seulement si le curseur est sur la case
				coo_case_x = échiquier_coo[0] + (src_x*BLOC)
				coo_case_y = échiquier_coo[1] + (src_y*BLOC)

				pos = pygame.mouse.get_pos()

				if coo_case_x <= pos[0] < coo_case_x+BLOC:
					if coo_case_y <= pos[1] < coo_case_y+BLOC:

						Animations.mettre_pion_à_glisser(Echiquier[src_y][src_x].pion
														, pos, tileset)





	quitter = [None,]
	Animations.gestion(quitter)

	if quitter[0] == True:
		go = False




	if Animations.écran_de_fin_de_partie_activé: 
		Gestion.partie_arrêtée = True 



	
	if Animations.écran_de_fin_de_partie_fini():
		reinitialiser();
		



#__________________________________________________________#
#__________________________________________________________#

































#__________________________________________________________#
#__________________________________________________________#


def dessiner_le_tout():

	global maj_dispo, afficher_ses_solutions, couleur_verte
	global destination_séléctionné, un_pion_est_déjà_séléctionné	


	# surface_écran.fill(couleur_de_fond)
	surface_écran.blit(img_fond, (0, 0))

	
	dessiner_échiquier_et_pions(surface_écran, tileset, un_pion_est_déjà_séléctionné, [src_x, src_y])
	dessiner_les_prisons(surface_écran)


	pion = Echiquier[src_y][src_x].pion  											
	if afficher_ses_solutions:
																			
		if pion != None:
			#pour plus tard : if pion.couleur == joueur_courant:
			if Gestion.état_boutton_afficher_coups_légaux == True:
				dessiner_les_solutions_d_un_pion(surface_écran, pion)
																		

	
	Animations.dessiner_diverses_choses(surface_écran)
	Animations.écran_de_promotion(surface_écran)
	Animations.animer_le_mouv(surface_écran)
	Animations.écran_de_fin_de_partie(surface_écran)
	


	Gestion.afficher_bouttons_de_match_nul(surface_écran)
	Animations.afficher_pion_qui_glisse(surface_écran, pygame.mouse.get_pos())






	pygame.display.flip()
	maj_dispo = False



#__________________________________________________________#
#__________________________________________________________#























#----------------------------------------------------------#
#----------------------------------------------------------#

while go:

	gestion_des_événements()
	mise_à_jour() 
	dessiner_le_tout()
	
	delta_time()
	clock.tick(FPS)



#----------------------------------------------------------#
#----------------------------------------------------------#


#développement global jusqu'à un niveau fonctionnel et agréable fini le 27/04/23
	#reste plus qu'à mettre du son

#01/07/23 : finitions de la fonction qui vérifie le matériel insuffisant
