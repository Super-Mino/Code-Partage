import pygame
from class_tuile import *
from class_pion import*
from utils import *
from animation import *



class Prise_Passant:

	def __init__(self, pos, case_du_pion_à_prendre, numéro_coup, couleur, y_des_prenneurs):
		self.pos = pos.copy() 
		self.numéro_coup = numéro_coup
		self.case_du_pion_à_prendre = case_du_pion_à_prendre
		self.c_est_pour_qui = couleur
		self.y_des_prenneurs = y_des_prenneurs



class Gestion_Global:

	def __init__(self, celui_qui_commence):
		
		self.joueur_courant = celui_qui_commence
		self.joueur_1 = celui_qui_commence
		self.joueur_2 = None 
		self.roi_joueur_1_échec = False
		self.roi_joueur_1_mat = False
		self.roi_joueur_2_échec = False
		self.roi_joueur_2_mat = False
		self.refAnimations = False
		self.victoire_accordée = False
		self.boutton_match_nul1 = None
		self.boutton_match_nul2 = None
		self.état_boutton_match_nul1 = False
		self.état_boutton_match_nul2 = False
		self.boutton_afficher_coups_légaux = None
		self.état_boutton_afficher_coups_légaux = True
		self.rects_du_dernier_déplacement = None
		self.partie_arrêtée = False

		self.promotion_en_cours = False 
		self.pion_en_demande_de_promotion = None 

		self.liste_des_prises_en_passant = []




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




	def si_le_joueur_courant_en_échec_mat(self):

		if roi_en_mat(self.joueur_courant):
		
			if not self.victoire_accordée:

				victorieux = couleur_adverse_de(self.joueur_courant)
				self.refAnimations.active_écran_de_victoire(victorieux, ECHEC_ET_MAT)
				self.victoire_accordée = True

				return True 

		return False;




	def si_situation_de_pat(self):

		if situation_de_pat(self.joueur_courant):

			if not self.victoire_accordée:

				self.refAnimations.active_écran_de_victoire(NULLE, PAT)
				self.victoire_accordée = True

				return True 

		return False;






	def si_matériel_insuffisant(self):

		if matériel_insuffisant():
			if not self.victoire_accordée:
				self.refAnimations.active_écran_de_victoire(NULLE, MATERIEL_INSUF)
				self.victoire_accordée = True

				return True 

		return False;







	def si_match_nul_d_un_commun_accord(self):

		if self.état_boutton_match_nul1 == True:
			if self.état_boutton_match_nul2 == True: 
				
				if not self.victoire_accordée:
					
					self.refAnimations.active_écran_de_victoire(NULLE, COMMUN_ACCORD)
					self.victoire_accordée = True
					
					
					return True 

		return False;








	def afficher_bouttons_de_match_nul(self, surface_écran):

		b1_face = 1
		b2_face = 1 
		b3_face = 1 

		if self.état_boutton_match_nul1 == True:
			b1_face = 2

		if self.état_boutton_match_nul2 == True:
			b2_face = 2

		if self.état_boutton_afficher_coups_légaux == True:
			b3_face = 2

		self.boutton_match_nul1.afficher(surface_écran, b1_face)
		self.boutton_match_nul2.afficher(surface_écran, b2_face)
		self.boutton_afficher_coups_légaux.afficher(surface_écran, b3_face)







	def gestion_des_événements_autres_bouttons(self, év): 

		if (self.refAnimations != None) and (not self.refAnimations.écran_de_fin_de_partie_activé):

			if self.boutton_match_nul1.cliqué(év):
				self.état_boutton_match_nul1 = not self.état_boutton_match_nul1

			if self.boutton_match_nul2.cliqué(év):
				self.état_boutton_match_nul2 = not self.état_boutton_match_nul2

			if self.boutton_afficher_coups_légaux.cliqué(év):
				self.état_boutton_afficher_coups_légaux = not self.état_boutton_afficher_coups_légaux


			# self.partie_arrêtée = self.état_boutton_match_nul2





	







	def reinitialiser(self): 

		self.partie_arrêtée = False
		self.victoire_accordée = False #afin de reinitialisé l'état de la partie (sert particulièrement pour l'écran de victoire)  
		self.liste_des_prises_en_passant.clear()
		self.rects_du_dernier_déplacement = None 


		if self.le_joueur_courant() == NOIR:
			self.alterner_les_rôles();


		taille = (3*DEMI_BLOC, DEMI_BLOC+DEMI_BLOC/2)
		y1 = f_prison_pour_noir_rect_cadre()[1]+f_prison_pour_noir_rect_cadre()[3]/2 -taille[1]/2
		y2 = f_prison_pour_blanc_rect_cadre()[1]+f_prison_pour_blanc_rect_cadre()[3]/2 -taille[1]/2



		self.boutton_match_nul1 = Boutton([cadre_rect[0]-taille[0]-cadre_épaisseur*2, y1], taille,
			couleur_grise, NOM_BOUTTON_MATCH_NUL, couleur2=couleur_verte) 

		self.boutton_match_nul2 = Boutton([cadre_rect[0]+cadre_rect[2]+cadre_épaisseur*2, y2]#cadre_rect[1]+cadre_rect[3]-taille[1]-cadre_épaisseur]
			, taille, couleur_grise, NOM_BOUTTON_MATCH_NUL, couleur2=couleur_verte) 

		taille2 = [taille[0]*1.25, taille[1]*0.75]
		y3 = f_prison_pour_blanc_rect_cadre()[1]+f_prison_pour_blanc_rect_cadre()[3]/2 -taille2[1]/2


		self.boutton_afficher_coups_légaux = Boutton([cadre_rect[0]-taille2[0]-cadre_épaisseur*1.5, y3]#cadre_rect[1]+cadre_rect[3]-taille2[1]-cadre_épaisseur]
			, taille2, couleur_grise, NOM_BOUTTON_COUPS_LEGAUX, couleur2=couleur_verte, taille_font=int(round(TAILLE_FONT2*0.81818181))) 


		self.état_boutton_match_nul1 = False
		self.état_boutton_match_nul2 = False
		# self.état_boutton_afficher_coups_légaux = False







































class Gestion_des_Prisons:
	pass

	#la refuge des pions capturés 
	def __init__(self):
		self.prison_pour_blanc = []
		self.prison_pour_noir = []
		self.prison_pour_blanc_place_libre = [0, 0]
		self.prison_pour_noir_place_libre = [0, 0]

		for i in range(prison_colonnes):
			ligne = []
			for j in range(prison_lignes):
				ligne.append(None) 

			self.prison_pour_blanc.append(ligne)

		self.prison_pour_noir = self.prison_pour_blanc.copy()






	def place_vraiment_libre(self, pos, couleur):
		prison = None

		if couleur == BLANC:
			prison = self.prison_pour_blanc

		elif couleur == NOIR:
			prison = self.prison_pour_noir

		else:
			return False 

		if prison[pos[1]][pos[0]] == None:
			return True 

		return False 





	def mettre_en_prison(self, prisonnier):

		prison = None
		place_libre = None 

		if prisonnier.couleur == NOIR:
			prison = self.prison_pour_noir
			place_libre = self.prison_pour_noir_place_libre

		elif prisonnier.couleur == BLANC :
			prison = self.prison_pour_blanc
			place_libre = self.prison_pour_blanc_place_libre
		
		else:
			print("\nErreur d'identité du pion dans 'mettre_en_prison()'\n")
			raise 



		if self.place_vraiment_libre(place_libre, prisonnier.couleur) : 
			prisonnier.pos = place_libre.copy()
			prisonnier.état = CAPTURE 
			prisonnier.dessiner_en_petit = True 



		#mise a jour de la position libre 
		if place_libre[0] >= 7 and place_libre[1] == 0:
			place_libre[0] = 0
			place_libre[1] = 1

		elif place_libre[0] < 7:
			place_libre[0] += 1








	def reinitialiser(self):

		self.prison_pour_blanc.clear()
		self.prison_pour_noir.clear()

		self.prison_pour_blanc_place_libre = [0, 0]
		self.prison_pour_noir_place_libre = [0, 0]

		for i in range(prison_colonnes):
			ligne = []
			for j in range(prison_lignes):
				ligne.append(None) 

			self.prison_pour_blanc.append(ligne)

		self.prison_pour_noir = self.prison_pour_blanc.copy()

























































Gestion = Gestion_Global(BLANC)
Prisons = Gestion_des_Prisons() 
Animations = Gestion_des_Animations(); 
Gestion.refAnimations = Animations

Echiquier = [] #sera ensuite init tel un tableau double (en rajoutant des listes dedans)
liste_des_pions = [] #restera une liste simple 
Rois = [] 
Tours = []
Guerriers = []


tileset_ref = None 

def mettre_tileset(tileset):
	global tileset_ref
	tileset_ref = tileset





















def init_échiquier():

	Echiquier.clear()

	#type 1 = case blanche, type 2 = case noire. Je commencerai par une case blanche comme
	#dans la plupart des échiquiers

	type_courant = 1
	# position_courante = [0, 0]
	couleur_courante = couleur_beige

	abcdefgh = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']

	for y in range(8):
		ligne = []

		for x in range(8):

			if type_courant == 1:
				couleur_courante = f_couleur_cases_claires()

			elif type_courant == 2:
				couleur_courante = f_couleur_cases_foncées()


			position_courante = (x, y)


			carreau = Tuile(type_courant, VIDE, position_courante, couleur_courante) 


			if cdheb.couleur_des_bas == BLANC:
				carreau.nom_colonne = abcdefgh[x] #comptées de gauche à droite. (ps : L'ordre "concret" de l'ajout des cases dans les listes python se fait ici toujours de gauche à droite puis de haut en bas. C'est la manière de numeroter les cases qui s'adapte)
				carreau.nom_ligne = str(8-y) #comptées de bas en haut (commence par 8)
			else:
				carreau.nom_colonne = abcdefgh[7-x] #comptées de droite à gauche
				carreau.nom_ligne = str(y+1) #de haut en bas (commence par 1)


			ligne.append(carreau)
			#aleternance des couleurs après chaque tour de boucle (dans les lignes)
			if type_courant == 1:
				type_courant = 2
			elif type_courant == 2:
				type_courant = 1

		#aleternance des couleurs après chaque tour de boucle (dans les colonnes)
		if type_courant == 1:
			type_courant = 2
		elif type_courant == 2:
			type_courant = 1

		Echiquier.append(ligne)

















def init_les_pions():

	liste_des_pions.clear()
	Rois.clear()
	Tours.clear()
	Guerriers.clear()



	colonne_des_rois = 4 # colonne d'après la liste python
	colonne_des_dames = 3

	if cdheb.couleur_des_hauts == NOIR:
		colonne_des_rois = 4
		colonne_des_dames = 3

	elif cdheb.couleur_des_hauts == BLANC:
		colonne_des_rois, colonne_des_dames = colonne_des_dames, colonne_des_rois 

	#simples alias pour ne pas trop rallonger le code qui suit
	cdr = colonne_des_rois
	cdd = colonne_des_dames




	#la tour gauche nord 
	tourGN = Tour([0, 0], cdheb.couleur_des_hauts); Echiquier[0][0].pion = tourGN; liste_des_pions.append(tourGN)
	Echiquier[0][0].état = cdheb.couleur_des_hauts; tourGN.côté = GAUCHE; Tours.append(tourGN)

	#le cavalier gauche nord 
	cavalierGN = Cavalier([1, 0], cdheb.couleur_des_hauts); Echiquier[0][1].pion = cavalierGN; liste_des_pions.append(cavalierGN)
	Echiquier[0][1].état = cdheb.couleur_des_hauts

	#le fou gauche nord 
	fouGN = Fou([2, 0], cdheb.couleur_des_hauts); Echiquier[0][2].pion = fouGN; liste_des_pions.append(fouGN)
	Echiquier[0][2].état = cdheb.couleur_des_hauts

	#la reine nord 
	reineN = Reine([cdd, 0], cdheb.couleur_des_hauts); Echiquier[0][cdd].pion = reineN; liste_des_pions.append(reineN)
	Echiquier[0][cdd].état = cdheb.couleur_des_hauts

	#le roi nord 
	roiN = Roi([cdr, 0], cdheb.couleur_des_hauts); Echiquier[0][cdr].pion = roiN; liste_des_pions.append(roiN)
	Echiquier[0][cdr].état = cdheb.couleur_des_hauts; Rois.append(roiN)

	#le fou droit nord 
	fouDN = Fou([5, 0], cdheb.couleur_des_hauts); Echiquier[0][5].pion = fouDN; liste_des_pions.append(fouDN)
	Echiquier[0][5].état = cdheb.couleur_des_hauts

	#le cavalier droit nord 
	cavalierDN = Cavalier([6, 0], cdheb.couleur_des_hauts); Echiquier[0][6].pion = cavalierDN; liste_des_pions.append(cavalierDN)
	Echiquier[0][6].état = cdheb.couleur_des_hauts

	#la tour droite nord 
	tourDN = Tour([7, 0], cdheb.couleur_des_hauts); Echiquier[0][7].pion = tourDN; liste_des_pions.append(tourDN)
	Echiquier[0][7].état = cdheb.couleur_des_hauts; tourDN.côté = DROITE; Tours.append(tourDN)


	# for x in range(8):
	# 	guerrierN = Guerrier([x, 1], cdheb.couleur_des_hauts)
	# 	guerrierN.sens_d_attaque = BAS
	# 	Echiquier[1][x].pion = guerrierN
	# 	liste_des_pions.append(guerrierN)
	# 	Echiquier[1][x].état = cdheb.couleur_des_hauts
	# 	Guerriers.append(guerrierN)

	# for x in range(8):
	# 	guerrierS = Guerrier([x, 6], cdheb.couleur_des_bas)
	# 	guerrierS.sens_d_attaque = HAUT 
	# 	Echiquier[6][x].pion = guerrierS
	# 	liste_des_pions.append(guerrierS)
	# 	Echiquier[6][x].état = cdheb.couleur_des_bas
	# 	Guerriers.append(guerrierS)


	#la tour gauche sud 
	tourGS = Tour([0, 7], cdheb.couleur_des_bas); Echiquier[7][0].pion = tourGS; liste_des_pions.append(tourGS)
	Echiquier[7][0].état = cdheb.couleur_des_bas; tourGS.côté = GAUCHE; Tours.append(tourGS)

	#le cavalier gauche sud
	cavalierGS = Cavalier([1, 7], cdheb.couleur_des_bas); Echiquier[7][1].pion = cavalierGS; liste_des_pions.append(cavalierGS)
	Echiquier[7][1].état = cdheb.couleur_des_bas

	#le fou gauche sud 
	fouGS = Fou([2, 7], cdheb.couleur_des_bas); Echiquier[7][2].pion = fouGS; liste_des_pions.append(fouGS)
	Echiquier[7][2].état = cdheb.couleur_des_bas

	#le roi sud
	roiS = Roi([cdr, 7], cdheb.couleur_des_bas); Echiquier[7][cdr].pion = roiS; liste_des_pions.append(roiS)
	Echiquier[7][cdr].état = cdheb.couleur_des_bas; Rois.append(roiS)

	#la reine sud 
	reineS = Reine([cdd, 7], cdheb.couleur_des_bas); Echiquier[7][cdd].pion = reineS; liste_des_pions.append(reineS)
	Echiquier[7][cdd].état = cdheb.couleur_des_bas

	#le fou droit sud
	fouDS = Fou([5, 7], cdheb.couleur_des_bas); Echiquier[7][5].pion = fouDS; liste_des_pions.append(fouDS)
	Echiquier[7][5].état = cdheb.couleur_des_bas

	#le cavalier droit sud 
	cavalierDS = Cavalier([6, 7], cdheb.couleur_des_bas); Echiquier[7][6].pion = cavalierDS; liste_des_pions.append(cavalierDS)
	Echiquier[7][6].état = cdheb.couleur_des_bas

	#la tour droite sud 
	tourDS = Tour([7, 7], cdheb.couleur_des_bas); Echiquier[7][7].pion = tourDS; liste_des_pions.append(tourDS)
	Echiquier[7][7].état = cdheb.couleur_des_bas; tourDS.côté = DROITE; Tours.append(tourDS)


	# #teste
	# son_x = 5-3
	# son_y = 3-1
	# teste = Guerrier([son_x, son_y], BLANC); Echiquier[son_y][son_x].pion = teste; liste_des_pions.append(teste)
	# Echiquier[son_y][son_x].état = BLANC
	# # teste.le_premier_coup_est_joué = True
	# # print(teste.mouv_dispo(Echiquier))


	# print('à ')
	# print(liste_des_pions.index(12))
	# print('à ')

	# print('-----------\n')

	# m = dir(list())
	# for i in m:
	# 	print(i)

	# print(len(liste_des_pions))

	# print('-----------\n\n')

	# un = str(1)
	# print(un)
	# print(type(un))

	#print(len("coucou"))















def dessiner_cadre(surface_écran):
	pygame.draw.rect(surface_écran, cadre_couleur, cadre_rect, cadre_épaisseur)

















def dessiner_les_prisons(surface_écran):

	pygame.draw.rect(surface_écran, (250, 250, 250), f_prison_pour_blanc_rect_cadre(), prison_cadre_épaisseur)
	pygame.draw.rect(surface_écran, (5, 5, 5), f_prison_pour_noir_rect_cadre(), prison_cadre_épaisseur)





















def dessiner_échiquier_et_pions(surface_écran, tileset, pion_est_select, pos):

	#dessin des différents carreaux
	for ligne in Echiquier:
		for careau in ligne:

			pygame.draw.rect(surface_écran, careau.couleur, careau.rect_sur_écran)
	


	#mise en relief du carreau du pion séléctionné (avec une couleur différente)
	if pion_est_select:
		careau = Echiquier[pos[1]] [pos[0]]
		pygame.draw.rect(surface_écran, COULEUR_CASE_SELECT, careau.rect_sur_écran) 



	#mise en relief du dernier déplacement
	if Gestion.rects_du_dernier_déplacement != None :

		rect1 = Gestion.rects_du_dernier_déplacement[0].copy()
		rect2 = Gestion.rects_du_dernier_déplacement[1].copy()
		
		couleur1 = COULEUR_CASES_DERNIER_DEPLACEMENT_1
		couleur2 = COULEUR_CASES_DERNIER_DEPLACEMENT_2
		
		if Echiquier[rect1[1]][rect1[0]].type == 1: # 1 = case clair
			couleur1 = COULEUR_CASES_DERNIER_DEPLACEMENT_1
		else:
			couleur1 = COULEUR_CASES_DERNIER_DEPLACEMENT_2

		if Echiquier[rect2[1]][rect2[0]].type == 1: # 1 = case clair
			couleur2 = COULEUR_CASES_DERNIER_DEPLACEMENT_1
		else:
			couleur2 = COULEUR_CASES_DERNIER_DEPLACEMENT_2

		#conversion coordonnées pixels
		rect1[0] = rect1[0]*BLOC  +échiquier_coo[0]
		rect1[1] = rect1[1]*BLOC  +échiquier_coo[1]
		rect2[0] = rect2[0]*BLOC  +échiquier_coo[0]
		rect2[1] = rect2[1]*BLOC  +échiquier_coo[1]

		pygame.draw.rect(surface_écran, couleur1, rect1)

		pygame.draw.rect(surface_écran, couleur2, rect2)








	#mise en relief de la case du roi si est en échec 
	roi = None 
	un_roi_est_en_échec = False 
	
	
	if roi_en_échec(BLANC): 
		roi = roi_blanc()
		un_roi_est_en_échec = True  


	elif roi_en_échec(NOIR):
		roi = roi_noir()
		un_roi_est_en_échec = True


	if un_roi_est_en_échec: 
		careau = Echiquier[roi.Y()][roi.X()] 
		pygame.draw.rect(surface_écran, COULEUR_CASE_ROI_ECHEC, careau.rect_sur_écran)





	#dessin des numéros 
	couleur = COULEUR_ZONE_DES_NUMEROS  
	couleur_texte = couleur_noire2
	font = pygame.font.Font(FONT_PATH, TAILLE_FONT_NUMERO_DES_CASES)

	taille = [DEMI_BLOC//2, BLOC]
	pos = [cadre_rect[0]-taille[0], échiquier_coo[1]]
	

	for y in range(8):
		zone = [pos[0], pos[1]+y*BLOC, taille[0], taille[1]] #Représente dans chaque tour de boucle un rectangle servant d'arrière plan à ce qui est écrit. A la fin les huits forment une bande.
		
		mot = Echiquier[y][0].nom_ligne
		mot = font.render(mot, True, couleur_texte)
		centrée = [zone[0]+taille[0]/2 -mot.get_size()[0]/2, zone[1]+taille[1]/2 -mot.get_size()[1]/2]

		pygame.draw.rect(surface_écran, couleur, zone)
		surface_écran.blit(mot, centrée)


	taille[0], taille[1] = taille[1], taille[0] 
	pos = [échiquier_coo[0], cadre_rect[1]+cadre_rect[3]]

	for x in range(8):
		zone = [pos[0]+x*BLOC, pos[1], taille[0], taille[1]] 

		mot = Echiquier[0][x].nom_colonne
		mot = font.render(mot, True, couleur_texte)
		centrée = [zone[0]+taille[0]/2 -mot.get_size()[0]/2, zone[1]+taille[1]/2 -mot.get_size()[1]/2]

		pygame.draw.rect(surface_écran, couleur, zone)
		surface_écran.blit(mot, centrée)




	#affichage de tous les pions
	for pion in liste_des_pions:
		pion.show(surface_écran, tileset)


	dessiner_cadre(surface_écran) #la bordure du plateau de jeu
		

















def dessiner_les_solutions_d_un_pion(surface_écran, pion): 

	#Cette fonction filtre et dessine les solutions pertinentes du pion 
	#(ici, pertinentes = qui ne laissent ou mettent le roi en échec) 

	md = pion.mouv_dispo(Echiquier)
	
	#ajout de la prise en passant si disponible
	if pion.type == GUERRIER:
		if pion.potentiel_prise_en_passant != None:
			md.append(pion.potentiel_prise_en_passant[0]) #au zéro est la postion, au un est la case du passant, au deux est le numéro du coup depuis lequel ça a été lancé
			

	for pos in md: 

		case_src = Echiquier[pion.Y()][pion.X()]
		case_dest = Echiquier[pos[1]][pos[0]]
		if met_il_roi_en_échec(case_src, case_dest) == True:
			continue

		x = pos[0]*BLOC + échiquier_coo[0] + BLOC//2
		y = pos[1]*BLOC + échiquier_coo[1] + BLOC//2

		pygame.draw.circle(surface_écran, couleur_verte_t, (x, y), BLOC//5)
		

	if pion.type == ROI:
		md = donne_roque_si_possible(pion.couleur)

		for pos in md :

			x = pos[0]*BLOC+échiquier_coo[0]+BLOC//2
			y = pos[1]*BLOC+échiquier_coo[1]+BLOC//2
			pygame.draw.circle(surface_écran, couleur_verte, (x, y), BLOC//5)













def la_case_de_l_échiquier_à(pos):
	return Echiquier[int(pos[1])][int(pos[0])]










def roi_blanc():
	if Rois[0].couleur == BLANC:
		return Rois[0]
	elif Rois[1].couleur == BLANC:
		return Rois[1]
	else:
		print("Erreur de l'obtention dans 'roi_blanc()'")
		raise 









def roi_noir():
	if Rois[0].couleur == NOIR:
		return Rois[0]
	elif Rois[1].couleur == NOIR:
		return Rois[1]
	else:
		print("Erreur de l'obtention dans 'roi_noir()'")
		raise 









def donne_roi(couleur):
	if couleur == NOIR:
		return roi_noir()
	elif couleur == BLANC:
		return roi_blanc()
	else:
		print("\nErreur de couleur dans 'roi(couleur)' \n\n")






def tours_noires():
	T = []
	for t in Tours:
		if t.couleur == NOIR:
			T.append(t) 
	return T 









def tours_blanches():
	T = []
	for t in Tours:
		if t.couleur == BLANC: 
			T.append(t) 
	return T 










def donne_les_tours(couleur):
	if couleur != NOIR and couleur != BLANC:
		print("dans 'donne_les_tours()' : arg 'couleur' invalid")
		raise 

	if couleur == NOIR :
		return tours_noires() 
	else:
		return tours_blanches() 












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










def couleur_adverse_de(couleur):
	if couleur == BLANC:
		return NOIR 
	elif couleur == NOIR:
		return BLANC 
	else:
		print("dans 'couleur_adverse_de()' : arg 'couleur' invalid")
		return False 










def donne_case_à(colonne=None, ligne=None, nom=None): 
	
	if colonne==None and ligne==None and nom==None:
		return None 

	if nom != None and len(nom) == 2:
		colonne = nom[0]
		ligne = nom[1] 
	else:
		if colonne == None or ligne == None:
			return None 


	for ligne_de_échiquier in Echiquier:

		if ligne_de_échiquier[0].nom_ligne != ligne:
			continue 

		for case_ in ligne_de_échiquier:

			if case_.nom_ligne == ligne:
				if case_.nom_colonne == colonne:
					return case_ 


	return None 






















def déplacer(src, dest):

	réussite = False

	case_src = Echiquier[src[1]][src[0]]

	validé = False

	if case_src.pion == None: #si la case ne possède pas de pion, on quitte
		return False 

	pion = case_src.pion

	#récupération des coups possibles pour ce pion
	choix = pion.mouv_dispo(Echiquier)

	for pos in choix:

		#si la case choisie fait partie de ses coups possibles 
		if dest[0] == pos[0] and dest[1] == pos[1]: 
			validé = True
			break 

	#traitement du cas d'une prise en passant si c'en est un 
	if not validé and pion.type == GUERRIER:
		
		if pion.potentiel_prise_en_passant != None:
			
			pos = pion.potentiel_prise_en_passant[0]

			if dest[0] == pos[0] and dest[1] == pos[1]: 
				
				réussite = faire_le_déplacement_de_la_prise_en_passant(pion
							, pion.potentiel_prise_en_passant[0]
							, pion.potentiel_prise_en_passant[1])
			# pion.potentiel_prise_en_passant = None




	#récupération de la case de destination  
	case_dest = Echiquier[dest[1]][dest[0]]

	côté = [] #pour le potentiel roque


	if validé and met_il_roi_en_échec(case_src, case_dest) == False: 

		#si possédait un pion, on le capture
		if case_dest.pion != None:
			Prisons.mettre_en_prison(case_dest.pion)
		

		case_dest.pion = case_src.pion
		case_dest.état = case_src.état
		case_dest.pion.pos = case_dest.pos.copy()

		case_src.pion = None
		case_src.état = VIDE 


		vérifier_les_prises_en_passant(case_dest.pion, case_src, case_dest)


		#Gestion du premier mouvement 
		son_type = case_dest.pion.type
		if son_type == GUERRIER or son_type == ROI or son_type == TOUR: 
			if case_dest.pion.le_premier_coup_est_joué == False:
				case_dest.pion.le_premier_coup_est_joué = True
			

		if not Animations.par_glissade:
			Animations.mettre_à_l_animation(case_dest.pion, src, dest, tileset_ref)

		réussite = True



	#si le déplacement n'est un pas un mouvement classique mais un roque 
	else:
		if réussite == False:
			if c_est_un_roque(src, dest, côté):
				réussite = faire_le_roque(src, dest, côté[0])

	if réussite:
		Animations.nombre_de_coups+=1

	if réussite:
		Gestion.rects_du_dernier_déplacement = [[src[0], src[1], BLOC, BLOC], 
												[dest[0], dest[1], BLOC, BLOC]]

	return réussite
















def roi_en_échec(couleur):

	#Cette fonction vérifie si avec l'état actuel de l'échiquier, le roi est en échec 

	roi = None 
	if couleur == NOIR:
		roi = roi_noir()
	elif couleur == BLANC:
		roi = roi_blanc()
	else:
		print("dans 'roi_en_échec()' : arg 'couleur' invalide ")
		return False 


	#maintenant on va parcourir tous les pions adverses et voir leurs 
	#solutions : si les coo du roi en font parti alors il est en échec 

	x = roi.pos[0]
	y = roi.pos[1] 

	en_échec = False 

	couleur_adverse = couleur_adverse_de(roi.couleur)  

	for p in liste_des_pions:

		if p.couleur == couleur_adverse and p.état == EN_JEU: 

			#on check ses solutions 
			solutions = p.mouv_dispo(Echiquier) 
			for pos in solutions: 

				if pos[0] == x and pos[1] == y:
					en_échec = True 
					break 

		#on casse aussi la premiere boucle
		if en_échec : 
			break 


	return en_échec
















def met_il_roi_en_échec(case_src, case_dest, case_du_passant=None): 

	#Cette fonction indique si le déplacement souhaité met (ou laisse) le roi en échec ou non 
	#mais ne réalise pas le déplacement 

	#On va simuler le déplacement puis remttre à l'état initial. 
	#Pendant la simulation, si après le mouvement le roi tombe en échec alors le coup sera interdit

	#Sauvegarde des infos pour les restituer à la fin 
	pion_dest = case_dest.pion  
	pion_src = case_src.pion  
	état_dest = case_dest.état
	état_src = case_src.état 
	pion_src_pos = pion_src.pos.copy()  
	pion_dest_pos = []
	if pion_dest != None:
		pion_dest_pos = pion_dest.pos.copy()

		#si on teste pour une prise en passant
	pion_qui_passe = None 
	pion_passé_pos = []
	if case_du_passant != None:
		pion_qui_passe = case_du_passant.pion 
		pion_passé_pos = pion_qui_passe.pos.copy()
		état_case_passant = case_du_passant.état






	#déplacement 
	if pion_dest != None:
		pion_dest.pos = (CASE_HORS_JEU_X, CASE_HORS_JEU_Y) 
	pion_src.pos = case_dest.pos.copy() 
	case_dest.pion = pion_src
	case_dest.état = état_src

	case_src.pion = None
	case_src.état = VIDE 

	if case_du_passant != None:
		case_du_passant.pion = None
		case_du_passant.état = VIDE 
		pion_qui_passe.pos = (CASE_HORS_JEU_X, CASE_HORS_JEU_Y) 




	#teste de l'échec 
	échec = False 
	échec = roi_en_échec(pion_src.couleur) 



	#restitution 
	if pion_dest != None:
		pion_dest.pos = pion_dest_pos
	pion_src.pos = pion_src_pos
	case_dest.pion = pion_dest
	case_dest.état = état_dest

	case_src.pion = pion_src 
	case_src.état = état_src 

	if case_du_passant != None:
		case_du_passant.pion = pion_qui_passe
		case_du_passant.état = état_case_passant
		pion_qui_passe.pos = pion_passé_pos
		 

	return échec 














def roi_en_mat(couleur): 

	#Cette fonction vérifie si le roi mentionné est vient de perdre 

	#Il ne peut être en mat que si c'est son tour (= l'autre joueur vient de jouer) 
	if couleur != Gestion.le_joueur_courant(): 
		return False 

	#Il faut également qu'il soit au moins en échec  
	if roi_en_échec(couleur) == False:
		return False 

	#L'algorithme qui m'est venu est de vérifier les solutions 
	#de tous les alliers en jeu. S'il y en a un qui annule l'échec alors pas de mat 

	sauvé = False  

	for p in liste_des_pions:
		if p.couleur == couleur and p.état == EN_JEU: # = si est un allier en jeu

			md = p.mouv_dispo(Echiquier)
			for pos in md:

				case_src = Echiquier[p.Y()][p.X()]
				case_dest = Echiquier[pos[1]][pos[0]] 

				if met_il_roi_en_échec(case_src, case_dest) == False:
					sauvé = True 
					break 

		#Si on en trouve un, on casse aussi cette boucle car un seul mouvement annulant 
		#l'échec suffit 
		if sauvé:
			break 


	#Retour de l'opposé pour que la fonction soit cohérente : si sauvé alors pas de mat donc 
	#on doit retourner False, et inversément pour non sauvé.  
	return not sauvé 














#----------------------------------------------------#
#----------------------------------------------------#
#-----------autres règles des échecs-----------------#
#----------------------------------------------------#
#----------------------------------------------------#















def donne_roque_si_possible(couleur_du_roi):

	#Cette fonction est comme 'mouv_dispo()', mais pour le roque
	
	if ROQUE_BLANC_fait and ROQUE_NOIR_fait:
		# print("hallo")
		return []

	
	roi = None
	tour = None  
	côté = INDEFINI
	mouv_possible = [] 

	cases_entre_roi_et_tour_droite = 2
	cases_entre_roi_et_tour_gauche = 3

	if cdheb.couleur_des_hauts == NOIR:
		cases_entre_roi_et_tour_droite = 2
		cases_entre_roi_et_tour_gauche = 3
	
	elif cdheb.couleur_des_hauts == BLANC:
		cases_entre_roi_et_tour_droite, cases_entre_roi_et_tour_gauche = cases_entre_roi_et_tour_gauche, cases_entre_roi_et_tour_droite

	#les alias raccourcis
	ceretd = cases_entre_roi_et_tour_droite
	ceretg = cases_entre_roi_et_tour_gauche

	
	if couleur_du_roi == BLANC:
		roi = roi_blanc()
	elif couleur_du_roi == NOIR: 
		roi = roi_noir() 
	else:
		print("dans 'roque()' : arg 'couleur_du_roi' invalid")
		return [] 


	# 1: vérifier que le roi en question est encore dans sa position initial 
	if roi.le_premier_coup_est_joué:
		return [] 


	# 2: on teste un côté (droit)
	# récuperons la tour 
	for t in donne_les_tours(couleur_du_roi):
		if t.côté == DROITE:
			tour = t 
			break  

	# a: est il aussi dans sa position initial 
	if tour.le_premier_coup_est_joué == False: 
		
		# b: voir si la voie est dégagée avec le roi 
		voie_dégagée = True 
		voie_libre = True 

		for i in range(1, ceretd+1): #Parcours des cases entre les deux. Il y a deux ou trois cases entre roi et tour droite selon le point de vue de notre position (si noirs en haut, blancs en bas et inversément) 
			x = roi.X()+i #on va de plus en plus à sa droite
			y = roi.Y()
			if Echiquier[y][x].état != VIDE:
				voie_dégagée = False 
				break

		# c: voir si aucune des cases de la voie n'est attaquée par un adversaire :) 
		if voie_dégagée:
			for i in range(1, 2): #ceretd+1):
				x = roi.X()+i
				y = roi.Y()

				for p in liste_des_pions:
					if p.couleur == couleur_adverse_de(couleur_du_roi) and p.état == EN_JEU:

						md = p.mouv_dispo(Echiquier) 
						for pos in md:
							if pos[0] == x and pos[1] == y : 
								voie_libre = False 
								break 

		#si tout cela est bon, on rajoute le mouvement et voila 
		if voie_dégagée and voie_libre: 
			mouv_possible.append([roi.X()+2, roi.Y()]) 

		

	# 3: même chose pour l'autre côté (gauche)
	for t in donne_les_tours(couleur_du_roi):
		if t.côté == GAUCHE:
			tour = t 
			break  

	if tour.le_premier_coup_est_joué == False: 
		
		voie_dégagée = True 
		voie_libre = True 

		for i in range(1, ceretg+1): #il y a deux ou trois cases entre roi et tour droite selon la disposition et le point de vue
			x = roi.X()-i
			y = roi.Y()
			if Echiquier[y][x].état != VIDE:
				voie_dégagée = False 
				break


		if voie_dégagée:
			for i in range(1, 2): #ceretg+1):
				x = roi.X()-i
				y = roi.Y()

				for p in liste_des_pions:
					if p.couleur == couleur_adverse_de(couleur_du_roi) and p.état == EN_JEU:

						md = p.mouv_dispo(Echiquier) 
						for pos in md:
							if pos[0] == x and pos[1] == y : 
								voie_libre = False 
								break 


		if voie_dégagée and voie_libre: 
			mouv_possible.append([roi.X()-2, roi.Y()]) 

	return mouv_possible















def c_est_un_roque(src, dest, côté):

	pion = Echiquier[src[1]][src[0]].pion 
	if pion.type != ROI:
		return False 

	roque = False

	md = donne_roque_si_possible(pion.couleur) 
	for pos in md:
		if pos[0] == dest[0] and pos[1] == dest[1]:
			roque = True 

	if not roque:
		return False 


	#Détermination du côté 
	if dest[0] < src[0]:
		côté.append(GAUCHE)
	elif src[0] < dest[0]:
		côté.append(DROITE)

	return True 







def faire_le_roque(src, dest, côté):

	

	c = []

	#vérifications 
	if c_est_un_roque(src, dest, c) == False:
		return False

	if côté != c[0]: #si le côté choisi ne correspond pas au côté dispo
		return  False

	roi = Echiquier[src[1]][src[0]].pion 
	tour = None 


	if côté == GAUCHE:
		tours = donne_les_tours(roi.couleur) 
		for t in tours:
			if t.côté == côté:
				tour = t
				break 

		case_src_tour = Echiquier[tour.Y()][tour.X()]
		case_src_roi = Echiquier[roi.Y()][roi.X()] 

		case_dest_tour = Echiquier[dest[1]] [dest[0]+1]
		case_dest_roi = Echiquier[dest[1]] [dest[0]] 
		#-----------------------------------------------# 
		tour.pos = case_dest_tour.pos.copy() 
		case_dest_tour.pion = tour 
		case_dest_tour.état = tour.couleur 

		roi.pos = case_dest_roi.pos.copy()
		case_dest_roi.pion = roi 
		case_dest_roi.état = roi.couleur 
		#-----------------------------------------------# 
		case_src_tour.pion = None 
		case_src_tour.état = VIDE 
		case_src_roi.pion = None 
		case_src_roi.état = VIDE 

		tour.le_premier_coup_est_joué = True 
		roi.le_premier_coup_est_joué = True 
		




	elif côté == DROITE:
		tours = donne_les_tours(roi.couleur) 
		for t in tours:
			if t.côté == côté:
				tour = t
				break 

		case_src_tour = Echiquier[tour.Y()][tour.X()]
		case_src_roi = Echiquier[roi.Y()][roi.X()] 

		case_dest_tour = Echiquier[dest[1]] [dest[0]-1]
		case_dest_roi = Echiquier[dest[1]] [dest[0]] 
		#-----------------------------------------------# 
		tour.pos = case_dest_tour.pos.copy() 
		case_dest_tour.pion = tour 
		case_dest_tour.état = tour.couleur 

		roi.pos = case_dest_roi.pos.copy()
		case_dest_roi.pion = roi 
		case_dest_roi.état = roi.couleur 
		#-----------------------------------------------# 
		case_src_tour.pion = None 
		case_src_tour.état = VIDE 
		case_src_roi.pion = None 
		case_src_roi.état = VIDE 

		tour.le_premier_coup_est_joué = True 
		roi.le_premier_coup_est_joué = True
		

	return True

	








def situation_de_pat(couleur):

	if couleur != Gestion.le_joueur_courant(): 
		return False 

	if roi_en_échec(couleur):
		return False 

	#Memê idée que pour l'algorithme du mat sauf qu'ici le roi n'est pas encore en échec
	for p in liste_des_pions:

		if p.couleur != couleur:
			continue 

		if p.état == EN_JEU:
			
			md = p.mouv_dispo(Echiquier)

			for pos in md:

				case_src = Echiquier[p.Y()][p.X()]
				case_dest = Echiquier[pos[1]][pos[0]] 

				if met_il_roi_en_échec(case_src, case_dest) == False: 
					return False 


	return True 











def matériel_insuffisant():

	#parcours tous les pions, si seuls les deux rois sont en jeux (ou les autres cas) alors manque de matériel


	#normalement les deux rois sont toujours au grand toujours sur le terrain
	if roi_noir().état != EN_JEU or roi_blanc().état != EN_JEU:
		print("Fatal Error trouvé dans 'matériel_insuffisant()' : un des rois si ce n'est les deux, est considéré comme hors jeu (hors du plateau) comme s'il était capturé.")
		exit()

	cavaliers_team_x = 0
	cavaliers_team_y = 0
	fous_team_x = 0
	fous_team_y = 0

	for pion in liste_des_pions: 

		if pion.état == EN_JEU:
			if pion.type == GUERRIER: #s'il y a au moins un guerrier il une chance de promouvoir en tour ou reine pour tenter faire échec et mat. Donc pas de matériel insuffisant.
				return False 

			elif pion.type == CAVALIER:
				if pion.couleur == NOIR:
					cavaliers_team_x+=1

				elif pion.couleur == BLANC:
					cavaliers_team_y+=1

			elif pion.type == FOU :
				if pion.couleur == NOIR:
					fous_team_x+=1

				elif pion.couleur == BLANC:
					fous_team_y+=1
					
			else : #c'est soit une tour soit une reine. Dans les deux cas et peu importe sa couleur il y a possibilité de faire échec et mat.
				if pion.type != ROI:
					return False 


	"""
	D'après chess.com voici les cas de figures possibles :
	1 : Roi contre roi (R vs R)
	2 : Roi contre roi et fou (R vs R+F)
	3 : Roi et fou contre roi et fou (R+F vs R+F)
	4 : Roi contre roi et cavalier (R vs R+C)
	5 : Roi et cavalier contre roi et cavalier (R+C vs R+C)
	6 : + une exception : Roi et deux cavaliers contre un roi seul
	"""

	#1:
	if (cavaliers_team_x+cavaliers_team_y+fous_team_x+fous_team_y) == 0:
		return True

	#2:
	if fous_team_x == 1 and (cavaliers_team_x+cavaliers_team_y+fous_team_y) == 0:
		return True
	if fous_team_y == 1 and (cavaliers_team_x+cavaliers_team_y+fous_team_x) == 0:
		return True

	#3:
	if fous_team_x == 1 and fous_team_y == 1 and (cavaliers_team_x+cavaliers_team_y) == 0:
		return True

	#4:
	if cavaliers_team_y == 1 and (cavaliers_team_x+fous_team_x+fous_team_y) == 0:
		return True
	if cavaliers_team_x == 1 and (cavaliers_team_y+fous_team_x+fous_team_y) == 0:
		return True

	#5:
	if cavaliers_team_y == 1 and cavaliers_team_x == 1 and (fous_team_x+fous_team_y) == 0:
		return True

	#6:
	if cavaliers_team_x == 2 and (cavaliers_team_y+fous_team_x+fous_team_y) == 0:
		return True
	if cavaliers_team_y == 2 and (cavaliers_team_x+fous_team_x+fous_team_y) == 0:
		return True


	return False












def vérifier_les_promotions():

	for pion in Guerriers:

		if pion.type == GUERRIER: #on est jamais trop prudent
			if pion.état == EN_JEU:
				if pion.Y() == LIGNE_HAUTE or pion.Y() == LIGNE_BASSE:
					pion.demande_promotion = True 










def traiter_les_promotions(): 


	if Gestion.promotion_en_cours == False:

		for pion in Guerriers:

			if pion.type == GUERRIER: #on est jamais trop prudent
				if pion.état == EN_JEU:
					if pion.demande_promotion : 

						Gestion.partie_arrêtée = True 
						Gestion.promotion_en_cours = True
						Animations.demande_de_promotion(pion.couleur)
						Gestion.pion_en_demande_de_promotion = pion 
						break 


	if Gestion.promotion_en_cours:

		réponse = Animations.choix_de_promotion 
		
		if réponse == None:
			return 


		if promouvoir(Gestion.pion_en_demande_de_promotion, réponse):

			Gestion.promotion_en_cours = False
			Gestion.pion_en_demande_de_promotion = None 
			Gestion.partie_arrêtée = False
			Animations.fin_de_la_demande_de_promotion()











def promouvoir(pion, réponse):
	

	#le supprimer de la liste des guerriers 
	index = None 
	try:
		index = Guerriers.index(pion)
	except ValueError: 
		print("Erreur lors de la promotion : impossible d enlever de 'Guerriers'\n")
		quit() 
	else:
		del Guerriers[index] 




	#créer son remplaçant
	nouveau = None 
	
	if réponse == TOUR:
		nouveau = Tour(pion.pos, pion.couleur)

	elif réponse == CAVALIER:
		nouveau = Cavalier(pion.pos, pion.couleur)

	elif réponse == FOU:
		nouveau = Fou(pion.pos, pion.couleur)

	elif réponse == REINE:
		nouveau = Reine(pion.pos, pion.couleur)

	else:
		print("Erreur lors de la promotion : choix du noueau pion invalide\n")
		quit() 




	#le remplacer dans la listes des pions 
		#reperer sa position
		#supprimer cette pos
		#inserer le nouveau 

	index = None 
	
	try:
		index = liste_des_pions.index(pion)
	except ValueError: 
		print("Erreur lors de la promotion : impossible de le remplacer dans 'liste_des_pions'\n")
		quit() 
	else:
		del liste_des_pions[index] 

		liste_des_pions.insert(index, nouveau)

		


	#le remplacer sur sa cace 
	Echiquier[pion.Y()][pion.X()].pion = nouveau


	return True 











def vérifier_les_prises_en_passant(pion, case_src, case_dest): 

	#cette fonction vérifie si un pion est déplacé de deux cases. Si oui alors
	#déclanche une possiblité de prise en passant

	if pion.type != GUERRIER or pion.état != EN_JEU:
		return 

	if pion.le_premier_coup_est_joué: #cette fonction est lancée juste avant la mise à jour cette variable afin d'avoir un résultat représentatif ici 
		return 


	bougé_de_deux_cases = False

	src_y = case_src.pos[1]
	dest_y = case_dest.pos[1]

	if abs(dest_y - src_y) == 2:
		bougé_de_deux_cases = True 

	if not bougé_de_deux_cases:
		return 


	#la case entre la source et la destination, la case sautée 
	cible_y = (src_y + dest_y)//2 
	cible_x = case_src.pos[0] 

		#normalement cette cible est forcément une case libre, laissons tout de même la vérification
	if Echiquier[cible_y][cible_x].état != VIDE:
		return 

	prise_enpassant = Prise_Passant([cible_x, cible_y], case_dest, Animations.nombre_de_coups
					, couleur_adverse_de(pion.couleur), case_dest.pos[1]) 

	Gestion.liste_des_prises_en_passant.append(prise_enpassant)












def traiter_les_prises_en_passant(): 

	# 1 : nétoyages des prises plus à jours (ils ne sont valables que le coup qui suit leur activation) 
	
	coup_courant = Animations.nombre_de_coups 
	les_expirés = []

		
		#en commançant par ceux des pions 
	for pion in Guerriers:
		if pion.type != GUERRIER:
			continue 

		if pion.potentiel_prise_en_passant != None:

			if abs(coup_courant - pion.potentiel_prise_en_passant[2]) > 1:
				pion.potentiel_prise_en_passant = None 


	
	for prise in Gestion.liste_des_prises_en_passant:

		if abs(coup_courant - prise.numéro_coup) > 1:
			les_expirés.append(prise) 


	for ex in les_expirés:

		try:
			index = Gestion.liste_des_prises_en_passant.index(ex)
			del Gestion.liste_des_prises_en_passant[index]

		except: 
			print("Erreur dans traiter_les_prises_en_passant(), taille de la liste : ", len( Gestion.liste_des_prises_en_passant))
			quit() 

	les_expirés.clear()


	for prise in Gestion.liste_des_prises_en_passant: #vérification que tous les non valables ne sont plus là

		if abs(coup_courant - prise.numéro_coup) > 1:
			les_expirés.append(prise) 

	if len(les_expirés) != 0:
		print("Erreur dans traiter_les_prises_en_passant(), au niveau des prises non valables")
		quit() 



	# 2 : attribution des prises au pions disponibles  

	for prise in Gestion.liste_des_prises_en_passant:

		pos_pion_à_gauche = None
		pos_pion_à_droite = None

		if prise.pos[0] > 0: 
			pos_pion_à_gauche = [prise.pos[0]-1, prise.y_des_prenneurs]

		if prise.pos[0] < 7:
			pos_pion_à_droite = [prise.pos[0]+1, prise.y_des_prenneurs] 



		ppàg = pos_pion_à_gauche
		ppàd = pos_pion_à_droite

		if ppàg != None:
			pion = Echiquier[ppàg[1]][ppàg[0]].pion

			if pion != None and pion.type == GUERRIER: 

				if prise.c_est_pour_qui == pion.couleur:
					pion.potentiel_prise_en_passant = [prise.pos.copy(), prise.case_du_pion_à_prendre
													  , prise.numéro_coup]

		if ppàd != None:
			pion = Echiquier[ppàd[1]][ppàd[0]].pion

			if pion != None and pion.type == GUERRIER: 

				if prise.c_est_pour_qui == pion.couleur:
					pion.potentiel_prise_en_passant = [prise.pos.copy(), prise.case_du_pion_à_prendre
													  , prise.numéro_coup]













def faire_le_déplacement_de_la_prise_en_passant(pion_à_déplacer, dest, case_du_passant):

	if pion_à_déplacer == None or dest == None or case_du_passant == None:
		return False 

	pion_qui_passe = case_du_passant.pion

	if pion_qui_passe == None:
		return False 



	src = pion_à_déplacer.pos.copy() 

	case_src = Echiquier[src[1]][src[0]]  
	case_dest = Echiquier[dest[1]][dest[0]] 


	if met_il_roi_en_échec(case_src, case_dest, case_du_passant): 
		return False



	#transfert des données de case_src à case_dest 
	case_dest.pion = case_src.pion # égal à 'pion_à_déplacer'
	case_dest.état = case_src.état 
	case_dest.pion.pos = case_dest.pos.copy() 

	#virginisation de la case_src 
	case_src.pion = None 
	case_src.état = VIDE 

	#capture du pion passé 
	Prisons.mettre_en_prison(pion_qui_passe)
	case_du_passant.état = VIDE 
	case_du_passant.pion = None 

	if not Animations.par_glissade:
		Animations.mettre_à_l_animation(pion_à_déplacer, src, dest, tileset_ref)

	return True 










