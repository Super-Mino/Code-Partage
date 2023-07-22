from utils import * 
from class_pion import *
from math import sqrt, cos, sin, tan, atan  
import pygame
pygame.init()

class Boutton:

	def __init__(self, pos, taille, couleur, texte, texte2 = None, couleur2 = None, taille_font=None):
		self.pos = pos 
		self.pos_initial = self.pos.copy()
		self.pos_sec = [0, 0]
		self.taille = taille 
		self.couleur = couleur 
		self.clique_gauche = 1 
		self.surface1 = pygame.Surface(self.taille)
		self.surface2 = None 

		self.surface1.fill(self.couleur)

		deux_textes = False
		if taille_font == None:
			taille_font = TAILLE_FONT

		font = pygame.font.Font(FONT_PATH, taille_font)
		
		if couleur2 == None :
			couleur2 = couleur
		else:
			if texte2 == None: #Si on précise deux couleurs mais qu'un seul texte, la deuxième face aura le même texte que la première
				texte2 = texte 

		if texte2 != None and type(str()) == type(texte2):
			deux_textes = True 

		
		

		surface_texte1 = font.render(texte, True, couleur_noire2)
		x = self.taille[0]/2 - surface_texte1.get_size()[0]/2
		y = self.taille[1]/2 - surface_texte1.get_size()[1]/2
		self.surface1.blit(surface_texte1, [x, y])
		pygame.draw.rect(self.surface1, couleur_noire2, [0, 0, self.taille[0], self.taille[1]], 2) #la bordure


		if deux_textes:
			self.surface2 = pygame.Surface(self.taille)
			self.surface2.fill(couleur2)

			surface_texte2 = font.render(texte2, True, couleur_noire2)
			x = self.taille[0]/2 - surface_texte2.get_size()[0]/2
			y = self.taille[1]/2 - surface_texte2.get_size()[1]/2
			self.surface2.blit(surface_texte2, [x, y])
			pygame.draw.rect(self.surface2, couleur_noire2, [0, 0, self.taille[0], self.taille[1]], 2)









	def cliqué(self, événement):
		if événement.type == pygame.MOUSEBUTTONDOWN:
			if événement.button == self.clique_gauche:

				clique_x = événement.pos[0]
				clique_y = événement.pos[1]
				x1 = self.pos[0]
				x2 = x1 + self.taille[0] 
				y1 = self.pos[1]
				y2 = y1 + self.taille[1]

				if (x1 <= clique_x < x2) and (y1 <= clique_y < y2):
					return True

		return False 




	def afficher(self, surface_écran, numero_de_texte = 1):

		# rect = [self.pos[0], self.pos[1], self.taille[0], self.taille[1]]
		# pygame.draw.rect(surface_écran, self.couleur, rect) 
		if numero_de_texte == 1:
			surface_écran.blit(self.surface1, [self.pos[0], self.pos[1]])
		else:
			surface_écran.blit(self.surface2, [self.pos[0], self.pos[1]])





	def mettre_position_secondaire(self):
		self.pos = self.pos_sec.copy() 


	def revenir_position_initial(self):
		self.pos = self.pos_initial.copy()



















class BouttonImage(Boutton):

	"""
	Cette classe permet d'avoir des bouttons avec des images à la place de textes. 
	L'image remplira le boutton
	Le rectangle source de l'images peut être obtenu à l'aide d'un pion, soit donné directement.
	(le pion sera prioritaire si les deux se présentent)
	"""

	def __init__(self, pos, taille, tileset, pion = None, rect_src = None, taille_image=(BLOC, BLOC), couleur=None, 
				pion2 = None, rect_src2 = None, taille_image2=(BLOC, BLOC), couleur2=None): 

		self.clique_gauche = 1 
		self.pos = pos 
		self.taille = taille  
		self.surface1 = None 
		self.surface2 = None

		self.surface1 = pygame.Surface(taille_image) 
		self.surface1.fill((1, 2, 3))
		self.surface1.set_colorkey((1, 2, 3))

		if pion != None and isinstance(pion, Pion):
			rect_src = pion.rect_src 
		else:
			if rect_src == None:
				rect_src = [0, 0, 0, 0] 


		if couleur != None:
			self.surface1.fill(couleur)

		self.surface1.blit(tileset, (0, 0), rect_src) 
		
		#redimensionnement pour mettre à la taille du boutton
		self.surface1 = pygame.transform.scale(self.surface1, taille)

		#la bordure
		pygame.draw.rect(self.surface1, couleur_noire2, [0, 0, self.taille[0], self.taille[1]], 2)


		#--------------------------------------------------------#

		self.surface2 = pygame.Surface(taille_image2) 
		self.surface2.fill((1, 2, 3))
		self.surface2.set_colorkey((1, 2, 3))

		if pion2 != None and isinstance(pion2, Pion):
			rect_src2 = pion2.rect_src 
		else:
			if rect_src2 == None:
				rect_src2 = [0, 0, 0, 0] 


		if couleur2 != None:
			self.surface2.fill(couleur2)

		self.surface2.blit(tileset, (0, 0), rect_src2) 
		
		#redimensionnement pour mettre à la taille du boutton
		self.surface2 = pygame.transform.scale(self.surface2, taille)

		#la bordure
		pygame.draw.rect(self.surface2, couleur_noire2, [0, 0, self.taille[0], self.taille[1]], 2)







	# def afficher(self, surface_écran):
	# 	Boutton.afficher(self, surface_écran, 1)


















class Gestion_des_Animations : 

	#Cette classe va gerer les animations, effets visuel et co, 
	#histoire de rendre le jeu attrayant 

	def __init__(self):

		self.pion_en_mouv = None 
		self.animation_en_cours = False 
		self.dest_coo = None
		self.src_coo = None
		self.pos_courante = None
		self.vecteur2f_dir = None #vecteur de déplacement du pion (direction)
		self.sprite = None
		self.temps_anim = TEMPS_DEPLACEMENT 
		self.timer = 0.0 #timer du déplacement des pions
		self.victorieux = None
		self.écran_de_fin_de_partie_activé = False
		self.timer_écran_de_victoire = None 
		self.temps_écran_de_victoire = 25.0 





		#-------------variables pour l affichage de l'acran de victoire----------------------# 

		self.taille_fenetre = (5*BLOC, 5*BLOC) # fenêtre de l'écran de fin et non de tout le programme 
		self.position_fenetre = (échiquier_coo[0] + (échiquier_dim/2) - (self.taille_fenetre[0]/2), échiquier_coo[1] + (échiquier_dim/2) - (self.taille_fenetre[1]/2)) #calculs pour centrer la fenêtre 
		self.afficher_fenetre = True 
		self.bordure_épaisseur = 7
		self.bordure_couleur = couleur_jaune 
		self.bordure_coo = (self.position_fenetre[0]-self.bordure_épaisseur, self.position_fenetre[1]-self.bordure_épaisseur)
		self.font = None 
		self.font2 = None 
		self.bloc_texte_taille = (self.taille_fenetre[0]-(self.bordure_épaisseur*2*2), self.taille_fenetre[1]/4)
		self.bloc_texte_coo = (self.position_fenetre[0]+self.bordure_épaisseur*2, self.position_fenetre[1]+self.bordure_épaisseur*2)
		self.bloc_texte_couleur = couleur_verte
		self.type_de_fin = "" 

		self.état_boutton_rejouer = False 
		self.état_boutton_voir_échiquier = False 
		self.état_boutton_quitter = False 

		self.bouttons_taille = (3*DEMI_BLOC+DEMI_BLOC, DEMI_BLOC+DEMI_BLOC/2)

		self.boutton_rejouer = Boutton([self.position_fenetre[0]+self.taille_fenetre[0]/2 -self.bouttons_taille[0]/2, self.position_fenetre[1]+self.taille_fenetre[1]/2]
							, self.bouttons_taille, couleur_verte, NOM_BOUTTON_REJOUER ) 

		self.boutton_voir_échiquier = Boutton([self.position_fenetre[0]+self.taille_fenetre[0]/4 -self.bouttons_taille[0]/2, self.position_fenetre[1]+self.taille_fenetre[1]*(6/8)]
							, self.bouttons_taille, couleur_jaune, NOM_BOUTTON_ECHIQUIER_1, NOM_BOUTTON_ECHIQUIER_2) 
		
		self.boutton_quitter = Boutton([self.position_fenetre[0]+self.taille_fenetre[0]*(3/4) -self.bouttons_taille[0]/2, self.position_fenetre[1]+self.taille_fenetre[1]*(6/8)]
							, self.bouttons_taille, couleur_rouge2, NOM_BOUTTON_QUITTER ) 


		#ces variables permettre de construire celles qui suivent 
		bordureX = cadre_rect[0]+cadre_rect[2] 
		interval = DEMI_BLOC/2
		étendu = self.bouttons_taille[1]*3 + interval*2

		self.boutton_rejouer.pos_sec = [bordureX, échiquier_coo[1] +échiquier_dim/2 -étendu/2]
		self.boutton_quitter.pos_sec = [bordureX, self.bouttons_taille[1]+self.boutton_rejouer.pos_sec[1]+interval]
		self.boutton_voir_échiquier.pos_sec = [bordureX, self.bouttons_taille[1]+self.boutton_quitter.pos_sec[1]+interval]





		#----------------------à propos de l écran de promotion---------------------------#
		

		self.demande_de_promotion_en_cours = False
		self.choix_de_promotion = None 
		self.couleur_de_promotion = None 

		self.boutton_tour = None
		self.boutton_cavalier = None
		self.boutton_fou = None 
		self.boutton_reine = None








		#----------------------------------divers-----------------------------------------#

		self.nombre_de_coups = 0 

		taille = [BLOC, BLOC]
		pos = [échiquier_coo[0]+échiquier_dim+DEMI_BLOC, échiquier_coo[1]+échiquier_dim-BLOC-DEMI_BLOC]

		self.zone_nombre_de_coups = [pos[0], pos[1], taille[0], taille[1]]
		self.couleur_zone_nombre_de_coups = COULEUR_ZONE_DES_NUMEROS 
	




		#drag and drop 
		self.sprite_pion_qui_glisse = None 
		self.pos_du_pion_qui_glisse = None 
		self.ref_vrai_pion_qui_glisse = None 
		self.ça_glisse = False 



















	#------------------------------------------------------------------#


	def mettre_à_l_animation(self, pion, src_coo, dest_coo, tileset):

		if self.animation_en_cours:
			return False 

		pion.cacher = True 
		self.pion_en_mouv = pion 
		self.dest_coo = dest_coo.copy()
		self.src_coo = src_coo.copy()
		self.pos_courante = list(src_coo.copy())

		# print("dest = ", self.dest_coo)

		self.animation_en_cours = True 
		self.timer = TEMPS_DEPLACEMENT 



		#amorçage du procéssus 

		#le sprite : 
		self.sprite = pygame.Surface((BLOC, BLOC))
		self.sprite.fill((222, 0, 0))
		self.sprite.set_colorkey((222, 0, 0))
		self.sprite.blit(tileset, (0, 0), pion.rect_src) 


		#OBSOLETE
		# 		#calculs sur la vitesse qu'aura le mouvement 
		# 		#v = d/t ; ps : on va travailler ici à l'unité de la case, et non au pixel, 
		# 		#la conversion sera faite après

		# #calcul de la distance du parcours (entre le src_coo et le dest_coo)
		# distance_X = dest_coo[0] - src_coo[0]
		# distance_Y = dest_coo[1] - src_coo[1] 
		# 					# distance = sqrt(coo_vecX**2 + coo_vecY**2) #simple formule des vecteurs 

		# #Comme il n'y a pas d'histoire d'angle (donc difficile de faire de la trigo) 
		# #on va calculer la vitesse de chaque axes du vecteur séparément
		# vitesseX = distance_X/self.temps_anim/BLOC/0.5
		# vitesseY = distance_Y/self.temps_anim/BLOC/0.5 

		# self.vecteur2f_dir = (vitesseX, vitesseY) 
		# # print("vec = ", self.vecteur2f_dir)


		#-------------------------------------------------#

		#1 Trouver le vecteur de direction (grâce à l'angle, donc trouver l'angle en 1er)

		#l'angle:
			#deux possibilités : soit le mouvement est sur les diagonales soit c'est sur les axes et il n'y a donc que 4 possibilités d'angle : (0° = 360°), 90°, 180°, 270°
		distX = dest_coo[0] - src_coo[0]
		distY = dest_coo[1] - src_coo[1]
		angle = 0

			#Sur les axes
		if distX == 0: #si mouvement seulement sur l axe des y
			if distY<0:
				angle = rad(270)
			else:
				angle = rad(90)
		
		elif distY == 0: #si mouvement seulement sur l axe des x
			if distX<0:
				angle = rad(180)
			else:
				angle = rad(0)
		else:

			#Sur les diagonales
			#on à deux cas, cas 1: le mouvement est vers la droite, cas 2: le mouvement est vers la gauche
		
			#cas 1 : on considère ce cas par défaut pour commencer
			adjacent = dest_coo[0] - src_coo[0] #la distance à parcourir sur les x 
			opposé   = dest_coo[1] - src_coo[1] #la distance à parcourir sur les y 
			décalage_de_l_angle = 0 
			cote = 1
			
			# cas 2 : 
			if distX < 0:
				cote = -1 #pour symboliser le côté gauche
				décalage_de_l_angle = rad(180)
				


			angle = atan(opposé/adjacent) #simple équation déduis à l'aide des propriétés de la trigonométrie (cah soh toa) 


			if cote == -1:
				angle += décalage_de_l_angle
			

			"""
			C'est vrai que les calculs ne sont pas évidents à comprendre. J'ai d'ailleurs tâtonné
			avant de trouver les bonnes opérations. Le mieux pour essayer de comprendre est d'être
			à l'aise avec la trigonométrie, faire des schémas et calculs, et avoir une bonne gymnastique
			cérébrale. 

			Les calculs pour trouver l'angle se basent sur un triangle rectangle (dans un cercle trigonométrique) 
			formé sur : 
			la trajectoire finale (l'hypoténus), 
			la distance à parcourir sur l'axes des x (traité par défaut comme l'adjacent)
			la distance à parcourir sur l'axes des y (traité par défaut comme l'opposé) 

			ce qui donne par défaut ce triangle : (avec le pion étant considéré aller vers le bas à droite, de 'A' au dernier 'y')

							
							270°





				180°		Axxxxxxxxxxxxx   0°
							  h          y
								 h       y
									h    y
									   h y
				

							90° 



			'A' étant l'angle recherché 

			ps: Il est important que le triangle considéré par défaut ait son angle droit au 0°

			"""


		vecX = cos(angle)
		vecY = sin(angle)


		#2 Appliquer la vitesse convenable à ce vecteur 

		#trouver la vitesse 

		distance = sqrt(distX**2 + distY**2) #simple théoreme de Pythagore pour trouver l'hypoténus (ici c'est la distance que va parcourir le pion)
		temps_en_sec = self.temps_anim #le temps que doit durer le déplacement 
		temps_d_une_frame_en_sec = 1/FPS #le 1 réprésentente la séconde. En gros on divise la seconde équitablement pour chaque frames qu'il doit faire passer

		vitesse = distance / temps_en_sec #simple formule de la vitesse (v=d/t)
		vitesse *= temps_d_une_frame_en_sec #on convertit maintenant cette vitesse à l'équivalent pour le temps d'une frame 

		vecX *= vitesse
		vecY *= vitesse 

		self.vecteur2f_dir=(vecX, vecY)



		










	def animer_le_mouv(self, surface_écran):
		if not self.animation_en_cours: 
			return False


		pos_à_écranX = self.pos_courante[0] * BLOC #conversion au pixel près 
		pos_à_écranY = self.pos_courante[1] * BLOC 

		pos_à_écranX += échiquier_coo[0] #application du décalage de l'échiquier par rapport
		pos_à_écranY += échiquier_coo[1] #au bord de l'écran à gauche et en haut 


		#affichage
		surface_écran.blit(self.sprite, (pos_à_écranX, pos_à_écranY)) 

		#mise à jour de la position
		self.pos_courante[0] += self.vecteur2f_dir[0]
		self.pos_courante[1] += self.vecteur2f_dir[1]
		# print('3_', self.vecteur2f_dir)

		self.timer -= dt()
		if self.timer <= 0.0: #on reinitialise tout si le timer est fini
			self.pion_en_mouv.cacher = False
			self.timer = 0.0
			self.animation_en_cours = False
			self.pion_en_mouv = None 
			self.animation_en_cours = False 
			self.dest_coo = None
			self.src_coo = None
			self.pos_courante = None
			self.vecteur2f_dir = None 
			self.sprite = None 
			return True 

		# else: 
		# 	self.timer -= dt()












	#------------------------------------------------------------------#



	def active_écran_de_victoire(self, victorieux, type_de_fin):
		if victorieux != NOIR and victorieux != BLANC and victorieux != NULLE:
			return 

		if type(type_de_fin) != type(str()):
			raise 
			

		if not self.écran_de_fin_de_partie_activé:
			self.victorieux = victorieux
			self.écran_de_fin_de_partie_activé = True 
			# self.timer_écran_de_victoire = self.temps_écran_de_victoire
			self.type_de_fin = type_de_fin





















	def écran_de_fin_de_partie(self, surface_écran):

		if not self.écran_de_fin_de_partie_activé:
			return 

		if self.victorieux != NOIR and self.victorieux != BLANC and self.victorieux != NULLE:
			return 



		if self.afficher_fenetre == False: #si la fenêtre est masquée on affiche quand-même les bouttons (qui sont mis hors de la fenêtre)
			self.boutton_rejouer.afficher(surface_écran)
			self.boutton_voir_échiquier.afficher(surface_écran, 2)
			self.boutton_quitter.afficher(surface_écran)
			return

		# self.victorieux = NULLE

		couleur = None 
		texte1 = MESSAGE_VICTOIRE
		texte2 = self.type_de_fin 


		if self.victorieux == BLANC:
			couleur = couleur_beige #couleur_blanche 
			texte1 += NOM_DES_BLANCS

		elif self.victorieux == NOIR:
			couleur = couleur_brun #couleur_noire
			texte1 += NOM_DES_NOIRS
		
		else:
			couleur = couleur_bleue2
			texte1 = MESSAGE_MATCH_NUL
			
		


		fond = [self.position_fenetre[0], self.position_fenetre[1], 
				self.taille_fenetre[0], self.taille_fenetre[1]]

		bordure = [self.bordure_coo[0], self.bordure_coo[1], 
					self.taille_fenetre[0]+(2*self.bordure_épaisseur), 
					self.taille_fenetre[1]+(2*self.bordure_épaisseur),]

		bloc_texte = [self.bloc_texte_coo[0], self.bloc_texte_coo[1],
					self.bloc_texte_taille[0], self.bloc_texte_taille[1]]


		surface_texte1 = self.font.render(texte1, True, couleur_noire2)
		surface_texte2 = self.font2.render(texte2, True, couleur_noire2)
		

		position_centré_texte1 = [(bloc_texte[2]/2) - (surface_texte1.get_size()[0]/2) + bloc_texte[0],
						(bloc_texte[3]/2) - (surface_texte1.get_size()[1]/2*2) + bloc_texte[1]]

		#Je fais /2*2 simplement pour me rappeler le procédé que j'ai utilisé : centrer puis remonter sur la partie haute
		#ainsi laisser la partie basse au texte n°2

		position_centré_texte2 = [(bloc_texte[2]/2) - (surface_texte2.get_size()[0]/2) + bloc_texte[0],
						(bloc_texte[3]/2)  + bloc_texte[1]]



		pygame.draw.rect(surface_écran, couleur, fond)
		pygame.draw.rect(surface_écran, self.bordure_couleur, bordure, self.bordure_épaisseur)
		pygame.draw.rect(surface_écran, self.bloc_texte_couleur, bloc_texte)

		surface_écran.blit(surface_texte1, position_centré_texte1)
		surface_écran.blit(surface_texte2, position_centré_texte2)
		
		self.boutton_rejouer.afficher(surface_écran)
		self.boutton_voir_échiquier.afficher(surface_écran)
		self.boutton_quitter.afficher(surface_écran)

	









	#------------------------------------------------------------------#


	def init_écran_de_promotion(self, tileset):

		if tileset == None:
			print("c est none")

		taille = [2*BLOC, 2*BLOC]
		x = échiquier_coo[0]
		y = échiquier_coo[1] + 3*BLOC
		couleur_fond = COULEUR_CASE_SELECT 

		self.boutton_tour = 	BouttonImage([x, y], taille, tileset, pion=Tour(None, BLANC), couleur=couleur_fond
								, pion2=Tour(None, NOIR), couleur2=couleur_fond )
		
		self.boutton_cavalier = BouttonImage([x+(1*2)*BLOC, y], taille, tileset, pion=Cavalier(None, BLANC), couleur=couleur_fond
								, pion2=Cavalier(None, NOIR), couleur2=couleur_fond )
		
		self.boutton_fou =  	BouttonImage([x+(2*2)*BLOC, y], taille, tileset, pion=Fou(None, BLANC), couleur=couleur_fond
								, pion2=Fou(None, NOIR), couleur2=couleur_fond )
		
		self.boutton_reine = 	BouttonImage([x+(3*2)*BLOC, y], taille, tileset, pion=Reine(None, BLANC), couleur=couleur_fond
								, pion2=Reine(None, NOIR), couleur2=couleur_fond )






	def demande_de_promotion(self, couleur):
		self.demande_de_promotion_en_cours = True 
		self.couleur_de_promotion = couleur 


	def fin_de_la_demande_de_promotion(self):
		self.demande_de_promotion_en_cours = False
		self.choix_de_promotion = None 






	def écran_de_promotion(self, surface_écran):

		if self.demande_de_promotion_en_cours == False:
			self.choix_de_promotion = None 
			return 

		if self.écran_de_fin_de_partie_activé:
			self.demande_de_promotion_en_cours = False
			return 


		face = 1 
		if self.couleur_de_promotion == NOIR:
			face = 2

		self.boutton_tour.afficher(surface_écran, face)
		self.boutton_cavalier.afficher(surface_écran, face)
		self.boutton_fou.afficher(surface_écran, face)
		self.boutton_reine.afficher(surface_écran, face)
















	#------------------------------------------------------------------#

	#le drag and drop
	def mettre_pion_à_glisser(self, pion, pos_curseur, tileset): 

		self.sprite_pion_qui_glisse = pygame.Surface((BLOC, BLOC))
		self.sprite_pion_qui_glisse.fill((1, 2, 3))
		self.sprite_pion_qui_glisse.set_colorkey((1, 2, 3))

		self.sprite_pion_qui_glisse.blit(tileset, [0, 0], pion.rect_src)#[0, 0, BLOC, BLOC]) 
		self.pos_du_pion_qui_glisse = pos_curseur

		self.ref_vrai_pion_qui_glisse = pion 
		pion.cacher = True 
		
		self.ça_glisse = True
		self.par_glissade = False 


	def pion_ne_glisse_plus(self):
		if self.ça_glisse:
			self.ça_glisse = False 
			self.ref_vrai_pion_qui_glisse.cacher = False 
			self.ref_vrai_pion_qui_glisse = None 


	def afficher_pion_qui_glisse(self, surface_écran, pos_curseur):

		if self.ça_glisse:
			
			centrée = list(pos_curseur).copy() #self.pos_du_pion_qui_glisse.copy() #

			centrée[0] -= DEMI_BLOC
			centrée[1] -= DEMI_BLOC
		
			surface_écran.blit(self.sprite_pion_qui_glisse, centrée) #self.pos_du_pion_qui_glisse)

























	#------------------------------------------------------------------#


	def gestion_des_événements(self, év):

		if self.écran_de_fin_de_partie_activé:
	
			if self.état_boutton_rejouer == False:
				self.état_boutton_rejouer = self.boutton_rejouer.cliqué(év)

			if self.état_boutton_quitter == False:
				self.état_boutton_quitter = self.boutton_quitter.cliqué(év)

			# if self.état_boutton_voir_échiquier == False:
			#  self.boutton_voir_échiquier.cliqué(év) 
			if self.boutton_voir_échiquier.cliqué(év): 
				self.état_boutton_voir_échiquier = not self.état_boutton_voir_échiquier



		if self.demande_de_promotion_en_cours:

			if self.boutton_tour.cliqué(év):
				self.choix_de_promotion = TOUR

			elif self.boutton_cavalier.cliqué(év):
				self.choix_de_promotion = CAVALIER

			elif self.boutton_fou.cliqué(év):
				self.choix_de_promotion = FOU

			elif self.boutton_reine.cliqué(év):
				self.choix_de_promotion = REINE










	def gestion(self, resultat):
		if self.état_boutton_voir_échiquier == False:
			self.afficher_fenetre = True 
		else:
			self.afficher_fenetre = False 


		if self.afficher_fenetre == False:
			self.boutton_rejouer.mettre_position_secondaire()
			self.boutton_quitter.mettre_position_secondaire()
			self.boutton_voir_échiquier.mettre_position_secondaire()
		else:
			self.boutton_rejouer.revenir_position_initial()
			self.boutton_quitter.revenir_position_initial()
			self.boutton_voir_échiquier.revenir_position_initial()
 

	
		if self.état_boutton_quitter == True:
 			resultat[0] = True  

		if self.état_boutton_rejouer: 
 			self.état_boutton_voir_échiquier = False

















	def écran_de_fin_de_partie_fini(self):
		if self.état_boutton_rejouer: 
			self.état_boutton_rejouer = False 
			return True 
		return False 













	def dessiner_diverses_choses(self, surface_écran):

		#nombre de coups 
		texte = self.font.render(str(self.nombre_de_coups), True, couleur_noire2)
		posTexte = [self.zone_nombre_de_coups[0]+ self.zone_nombre_de_coups[2]/2 -texte.get_size()[0]/2
				   ,self.zone_nombre_de_coups[1]+ self.zone_nombre_de_coups[3]/2 -texte.get_size()[1]/2] 

		pygame.draw.rect(surface_écran, self.couleur_zone_nombre_de_coups, self.zone_nombre_de_coups)
		surface_écran.blit(texte, posTexte)












	def reinitialiser(self):
		self.écran_de_fin_de_partie_activé = False
		self.font = pygame.font.Font(FONT_PATH, TAILLE_FONT) 
		self.font2 = pygame.font.Font(FONT_PATH, TAILLE_FONT2) 

		self.demande_de_promotion_en_cours = False
		self.nombre_de_coups = 0


