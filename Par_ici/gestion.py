import pygame
from class_tuile import *
from class_pion import*




class PlayerGestion:

	def __init__(self, celui_qui_commence):
		self.joueur_courant = celui_qui_commence
		self.joueur_1 = celui_qui_commence
		self.joueur_2 = None 

		if self.joueur_1 == NOIR:
			self.joueur_2 = BLANC
		elif self.joueur_1 == BLANC:
			self.joueur_2 = NOIR 


	def alterner_les_rôles(self):
		if self.joueur_courant == self.joueur_1:
			self.joueur_courant = self.joueur_2
		elif self.joueur_courant == self.joueur_2:
			self.joueur_courant = self.joueur_1


	def le_joueur_courant(self):
		return self.joueur_courant 








Echiquier = []
liste_des_pions = []
Gestion = PlayerGestion(BLANC)










def init_échiquier():
	#type 1 = case blanche, type 2 = case noire. Je commencerai par une case blanche comme
	#dans la plupart des échiquiers

	type_courant = 1
	# position_courante = [0, 0]
	couleur_courante = couleur_beige

	for y in range(8):
		ligne = []

		for x in range(8):

			if type_courant == 1:
				couleur_courante = couleur_beige
				type_courant = 2

			elif type_courant == 2:
				couleur_courante = couleur_brun
				type_courant = 1

			position_courante = (x, y)

			careau = Tuile(type_courant, VIDE, position_courante, couleur_courante)

			ligne.append(careau)


		if type_courant == 1:
			type_courant = 2
		elif type_courant == 2:
			type_courant = 1

		Echiquier.append(ligne)

















def init_les_pions():

	#la tour gauche noire 
	tourGN = Tour([0, 0], NOIR); Echiquier[0][0].pion = tourGN; liste_des_pions.append(tourGN)
	Echiquier[0][0].état = NOIR

	#le cavalier gauche noir 
	cavalierGN = Cavalier([1, 0], NOIR); Echiquier[0][1].pion = cavalierGN; liste_des_pions.append(cavalierGN)
	Echiquier[0][1].état = NOIR

	#le fou gauche noire 
	fouGN = Fou([2, 0], NOIR); Echiquier[0][2].pion = fouGN; liste_des_pions.append(fouGN)
	Echiquier[0][2].état = NOIR

	#la reine noire 
	reineN = Reine([3, 0], NOIR); Echiquier[0][3].pion = reineN; liste_des_pions.append(reineN)
	Echiquier[0][3].état = NOIR

	#le roi noir 
	roiN = Roi([4, 0], NOIR); Echiquier[0][4].pion = roiN; liste_des_pions.append(roiN)
	Echiquier[0][4].état = NOIR

	#le fou droit noir 
	fouDN = Fou([5, 0], NOIR); Echiquier[0][5].pion = fouDN; liste_des_pions.append(fouDN)
	Echiquier[0][5].état = NOIR

	#le cavalier droit noir 
	cavalierDN = Cavalier([6, 0], NOIR); Echiquier[0][6].pion = cavalierDN; liste_des_pions.append(cavalierDN)
	Echiquier[0][6].état = NOIR

	#la tour droite noire 
	tourDN = Tour([7, 0], NOIR); Echiquier[0][7].pion = tourDN; liste_des_pions.append(tourDN)
	Echiquier[0][7].état = NOIR


	for x in range(8):
		guerrierN = Guerrier([x, 1], NOIR)
		Echiquier[1][x].pion = guerrierN
		liste_des_pions.append(guerrierN)
		Echiquier[1][x].état = NOIR

	for x in range(8):
		guerrierB = Guerrier([x, 6], BLANC)
		Echiquier[6][x].pion = guerrierB
		liste_des_pions.append(guerrierB)
		Echiquier[6][x].état = BLANC


	#la tour gauche blanche 
	tourGB = Tour([0, 7], BLANC); Echiquier[7][0].pion = tourGB; liste_des_pions.append(tourGB)
	Echiquier[7][0].état = BLANC

	#le cavalier gauche blanc
	cavalierGB = Cavalier([1, 7], BLANC); Echiquier[7][1].pion = cavalierGB; liste_des_pions.append(cavalierGB)
	Echiquier[7][1].état = BLANC

	#le fou gauche blanc 
	fouGB = Fou([2, 7], BLANC); Echiquier[7][2].pion = fouGB; liste_des_pions.append(fouGB)
	Echiquier[7][2].état = BLANC

	#le roi blanc
	roiB = Roi([4, 7], BLANC); Echiquier[7][4].pion = roiB; liste_des_pions.append(roiB)
	Echiquier[7][4].état = BLANC

	#la reine blanche 
	reineB = Reine([3, 7], BLANC); Echiquier[7][3].pion = reineB; liste_des_pions.append(reineB)
	Echiquier[7][3].état = BLANC

	#le fou droit blanc
	fouDB = Fou([5, 7], BLANC); Echiquier[7][5].pion = fouDB; liste_des_pions.append(fouDB)
	Echiquier[7][5].état = BLANC

	#le cavalier droit blanc 
	cavalierDB = Cavalier([6, 7], BLANC); Echiquier[7][6].pion = cavalierDB; liste_des_pions.append(cavalierDB)
	Echiquier[7][6].état = BLANC

	#la tour droite blanche 
	tourDB = Tour([7, 7], BLANC); Echiquier[7][7].pion = tourDB; liste_des_pions.append(tourDB)
	Echiquier[7][7].état = BLANC


	# #teste
	# son_x = 5-3
	# son_y = 3-1
	# teste = Guerrier([son_x, son_y], BLANC); Echiquier[son_y][son_x].pion = teste; liste_des_pions.append(teste)
	# Echiquier[son_y][son_x].état = BLANC
	# # teste.le_premier_coup_est_joué = True
	# # print(teste.mouv_dispo(Echiquier))
















def dessiner_échiquier(surface_écran, tileset):

	for ligne in Echiquier:
		for careau in ligne:

			pygame.draw.rect(surface_écran, careau.couleur, careau.rect_sur_écran)
			# if careau.pion != None :
			# 	pass
			# 	careau.pion.show(surface_écran, tileset)



	for pion in liste_des_pions:
		pion.show(surface_écran, tileset)
		# print('1')









def dessiner_les_solutions_d_un_pion(surface_écran, pion):

	# pion 
	# print("i", end ="")

	mp = pion.mouv_dispo(Echiquier)
	
	x = pion.pos[0]*BLOC+échiquier_coo[0]
	y = pion.pos[1]*BLOC+échiquier_coo[1]
	pygame.draw.rect(surface_écran, couleur_verte, [x, y, BLOC, BLOC], 2)

	for pos in mp: 
		x = pos[0]*BLOC + échiquier_coo[0] + 23
		y = pos[1]*BLOC + échiquier_coo[1] + 23

		rect = [x, y, 20, 20]
		# pygame.draw.rect(surface_écran, couleur_verte, rect)
		pygame.draw.circle(surface_écran, couleur_verte, (rect[0]+10, rect[1]+10), rect[2]-10)
		# print("u", end ="")
		# pygame.display.flip()















def la_case_de_l_échiquier_à(pos):
	# try: 
	# 	tuile = Echiquier[pos[0]][pos[1]]
	# except:
	# 	print("Erreur dans la fonction 'la_case_de_l_échiquier_à(pos)' ")
	# 	return  
	# else:
	# 	return tuile
	return Echiquier[int(pos[1])][int(pos[0])]











def est_ce_dans_l_échiquier(pos):

	#retour de la fonction en deux variable : oui ou non si c'est dans l'échiquier
	#si oui : quel case [x,y] cela correspond dans l'échiquier
	resultat = [False, None]
	x = pos[0]
	y = pos[1]

	#vérification si les coo sont dans l'échiquier
	if x >= échiquier_coo[0] and x < échiquier_coo[0] + échiquier_dim:
		if y >= échiquier_coo[1] and y < échiquier_coo[1] + échiquier_dim:
			resultat[0] = True 
			# print("dedans")

	#si dans l'échiquier : calcul de la position
	if resultat[0] == True:

		x_formaté = x - échiquier_coo[0]
		y_formaté = y - échiquier_coo[1] 

		case_x = x_formaté//BLOC
		case_y = y_formaté//BLOC

		resultat[1] = [case_x, case_y]

	return resultat
			










def est_ce_un_pion(pos):
	if la_case_de_l_échiquier_à(pos).pion != None:
		return True
	return False









def donne_le_pion(pos):
	return la_case_de_l_échiquier_à(pos).pion











def déplacer(src, dest):
	print("d ", end = '')

	case_src = Echiquier[src[1]][src[0]]

	validé = False

	if case_src.pion == None:
		return False 

	choix = case_src.pion.mouv_dispo(Echiquier)

	for pos in choix:
		
		if pos[0] == dest[0] and pos[1] == dest[1]:
			validé = True
			break 

	if validé : 
		case_dest = Echiquier[dest[1]][dest[0]]

		#pour l instant je traite pas encore la capture des pions adverses 

		case_dest.pion = case_src.pion
		case_dest.état = case_src.état
		case_dest.pion.pos = case_dest.pos

		case_src.pion = None
		case_src.état = VIDE 

		if case_dest.pion.type == GUERRIER: 
			if case_dest.pion.le_premier_coup_est_joué == False:
				case_dest.pion.le_premier_coup_est_joué = True
			

		return True

	# print("t", end='')
	return False