BLOC = 64
if BLOC%2!=0: #sécurité afin que la valeur soit toujours paire 
	BLOC+=1
DEMI_BLOC = BLOC//2
FPS = 30
VIDE = 0
BLANC = 1
NOIR = 2
CAPTURE = 3
EN_JEU = 4
INDEFINI = 5 
HAUT = 6  
BAS = 7 
GAUCHE = 8 
DROITE = 9

ROI = 10
REINE = 11
TOUR = 12
CAVALIER = 13
FOU = 14
GUERRIER = 15 # = pion classique 
NULLE = 16 

LIGNE_HAUTE = 0
LIGNE_BASSE = 7

CASE_HORS_JEU_X = 44
CASE_HORS_JEU_Y = 55 

ROQUE_BLANC_fait = False
ROQUE_NOIR_fait = False

TEMPS_DEPLACEMENT = 0.2 #second
TAILLE_FONT = 28
TAILLE_FONT2 = 22 
TAILLE_FONT_NUMERO_DES_CASES = 15
FONT_PATH = "Ress/Font/bahnschrift.ttf"

ECHEC_ET_MAT = "par échec et mat"
TEMPS_ECOULE = "au temps"
MATERIEL_INSUF = "matériel insuffisant"
PAT = "par pat" 
COMMUN_ACCORD = "par commun accord"
NULL_PAR_AUTRE = "-"

TITRE_FENETRE = "échec-python"
CHEMIN_TILES = "Ress/IMG/tiles.png"
CHEMIN_IMAGE_FOND = "Ress/IMG/fond.jpg"

NOM_BOUTTON_MATCH_NUL = "Nulle" #"Draw"
NOM_BOUTTON_COUPS_LEGAUX = "Coups légaux"

NOM_BOUTTON_REJOUER = "Rejouer"
NOM_BOUTTON_ECHIQUIER_1 = "Échiquier"
NOM_BOUTTON_ECHIQUIER_2 = "Résultats"
NOM_BOUTTON_QUITTER = "Quitter"

MESSAGE_VICTOIRE = "Victoire des "
NOM_DES_BLANCS = "BLANCS" 
NOM_DES_NOIRS = "NOIRS" 
MESSAGE_MATCH_NUL = "Match nulle"

#========================================================================================#

échiquier_dim = BLOC * 8 #c est un carré donc besoin que d'une valeur 
bonus_de_marge = DEMI_BLOC//2
écran_marge_gauche = BLOC * 2 +bonus_de_marge
écran_marge_droite = BLOC * 2 +bonus_de_marge
écran_marge_haute = BLOC * 2 
écran_marge_basse = BLOC * 2 

écran_largeur = écran_marge_gauche + échiquier_dim + écran_marge_gauche
écran_hauteur = écran_marge_haute + échiquier_dim + écran_marge_basse

échiquier_coo = (écran_marge_gauche, écran_marge_haute)



#========================================================================================#

prison_lignes = 8
prison_colonnes = 2

prison_pour_blanc_largeur = prison_lignes * DEMI_BLOC
prison_pour_blanc_hauteur = prison_colonnes * DEMI_BLOC 
_son_x_centré_sur_échiquier_en_w_ = échiquier_coo[0] + ((échiquier_dim - prison_pour_blanc_largeur)/2)
prison_pour_blanc_coo = (_son_x_centré_sur_échiquier_en_w_, DEMI_BLOC)
#rect traité simplement, puis comme cadre 
prison_pour_blanc_rect_cadre = [prison_pour_blanc_coo[0], prison_pour_blanc_coo[1], 
							prison_pour_blanc_largeur, prison_pour_blanc_hauteur]

prison_pour_noir_rect_cadre = prison_pour_blanc_rect_cadre.copy()
prison_pour_noir_rect_cadre[1] = écran_hauteur - DEMI_BLOC - prison_pour_noir_rect_cadre[3]
prison_pour_noir_largeur = prison_pour_blanc_largeur
prison_pour_noir_hauteur = prison_pour_blanc_hauteur
prison_pour_noir_coo = (prison_pour_noir_rect_cadre[0], prison_pour_noir_rect_cadre[1])

#traité comme véritable cadre maintenant 
prison_cadre_épaisseur = 3 

prison_pour_noir_rect_cadre[0] -= prison_cadre_épaisseur
prison_pour_noir_rect_cadre[1] -= prison_cadre_épaisseur 
prison_pour_noir_rect_cadre[2] += prison_cadre_épaisseur*2
prison_pour_noir_rect_cadre[3] += prison_cadre_épaisseur*2

prison_pour_blanc_rect_cadre[0] -= prison_cadre_épaisseur
prison_pour_blanc_rect_cadre[1] -= prison_cadre_épaisseur 
prison_pour_blanc_rect_cadre[2] += prison_cadre_épaisseur*2
prison_pour_blanc_rect_cadre[3] += prison_cadre_épaisseur*2





from random import randint
class CouleurDesHautsEtBas:
	def __init__(self):
		self.couleur_des_hauts = NOIR
		self.couleur_des_bas = BLANC


cdheb = CouleurDesHautsEtBas()

def tirerLesDésSurLesPlacesDeQuiEtQui():
	global prison_pour_blanc_rect_cadre, prison_pour_noir_rect_cadre
	global prison_pour_noir_coo, prison_pour_blanc_coo

	rd = randint(0, 100)
	if rd >= 50:
		cdheb.couleur_des_hauts, cdheb.couleur_des_bas = cdheb.couleur_des_bas, cdheb.couleur_des_hauts

		prison_pour_blanc_rect_cadre, prison_pour_noir_rect_cadre = prison_pour_noir_rect_cadre, prison_pour_blanc_rect_cadre 
		prison_pour_noir_coo, prison_pour_blanc_coo = prison_pour_blanc_coo, prison_pour_noir_coo

tirerLesDésSurLesPlacesDeQuiEtQui()


def f_prison_pour_blanc_rect_cadre():
	return prison_pour_blanc_rect_cadre

def f_prison_pour_noir_rect_cadre():
	return prison_pour_noir_rect_cadre


def prison_coo(couleur):
	if couleur == NOIR:
		return prison_pour_noir_coo
	elif couleur == BLANC:
		return prison_pour_blanc_coo
	else:
		return (0, 0)

#========================================================================================#




couleur_brun = (35, 12, 5) #(50, 17, 5)
couleur_beige = (252, 206, 162)
couleur_noire = (5, 5, 5)
couleur_noire2 = (32, 32, 32)
couleur_brun2 = (130, 75, 50)
couleur_verte = (20, 186, 25, 100)
couleur_rouge = (223, 15, 16)
couleur_jaune = (253, 170, 0)
couleur_bleue = (12, 120, 232) 
couleur_blanche = (250, 250, 250)
couleur_bleue2 = (84, 116, 162)#(85, 119, 168)
couleur_grise = (121, 126, 133)
couleur_rouge2 = (217, 55, 44)

from pygame import Color
#couleurs transparentes (de moitié)

couleur_brun_t = Color(couleur_brun[0], couleur_brun[1], couleur_brun[2], 122)
couleur_beige_t = Color(couleur_beige[0], couleur_beige[1], couleur_beige[2], 122)
couleur_noire_t = Color(couleur_noire[0], couleur_noire[1], couleur_noire[2], 122)
couleur_noire2_t = Color(couleur_noire2[0], couleur_noire2[1], couleur_noire2[2], 122)
couleur_brun2_t = Color(couleur_brun2[0], couleur_brun2[1], couleur_brun2[2], 122)
couleur_verte_t = Color(couleur_verte[0]+150, couleur_verte[1], couleur_verte[2], 122)
couleur_rouge_t = Color(couleur_rouge[0], couleur_rouge[1], couleur_rouge[2], 122)
couleur_jaune_t = Color(couleur_jaune[0], couleur_jaune[1], couleur_jaune[2], 122)
couleur_bleue_t = Color(couleur_bleue[0], couleur_bleue[1], couleur_bleue[2], 122) 
couleur_blanche_t = Color(couleur_blanche[0], couleur_blanche[1], couleur_blanche[2], 122) 
couleur_bleue1_t = Color(couleur_bleue2[0], couleur_bleue2[1], couleur_bleue2[2], 122) 
couleur_grise_t = Color(couleur_grise[0], couleur_grise[1], couleur_grise[2], 122) 
couleur_rouge2_t = Color(couleur_rouge2[0], couleur_rouge2[1], couleur_rouge2[2], 122)




couleur_de_fond = couleur_brun2
couleur_case_noire = couleur_brun 
couleur_case_blanche = couleur_beige
cadre_couleur = couleur_noire 

COULEUR_CASE_SELECT = (59, 180, 152)  #couleur_verte 
COULEUR_CASE_ROI_ECHEC = couleur_rouge
COULEUR_ZONE_DES_NUMEROS = (221, 178, 118)
COULEUR_CASES_DERNIER_DEPLACEMENT_1 = (100, 180, 100)
COULEUR_CASES_DERNIER_DEPLACEMENT_2 = [i-50 for i in COULEUR_CASES_DERNIER_DEPLACEMENT_1]


cadre_épaisseur = 6
cadre_coo = (échiquier_coo[0] - cadre_épaisseur, échiquier_coo[1]- cadre_épaisseur)

cadre_rect = list(cadre_coo)
cadre_rect.append(échiquier_dim+cadre_épaisseur*2)
cadre_rect.append(échiquier_dim+cadre_épaisseur*2)
# cadre_rect = cadre_rect.extend([échiquier_dim, échiquier_dim])
# print(cadre_rect)
# print(cadre_coo)




#========================================================================================#

from pygame.time import get_ticks

last_ticks = get_ticks()
dt_ = 0 
def delta_time():
	global dt_, last_ticks
	naw_ticks = get_ticks()
	dt_ = (naw_ticks - last_ticks)/1000
	last_ticks = naw_ticks 

def dt():
	return dt_ 




def rad(deg):
	return (deg*2*3.14159265)/360



def degré(rad):
	return (rad*360)/(2*3.14159265)




#========================================================================================#

#couleurs dynamiques 

couleur_cases_foncées = (20, 107, 112) #(21, 114, 119)
couleur_cases_claires = (182, 194, 194) 

def f_couleur_cases_foncées(): 
	return list(couleur_cases_foncées).copy()
def f_couleur_cases_claires():
	return list(couleur_cases_claires).copy()


