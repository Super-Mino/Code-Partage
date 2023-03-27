from utils import*
import pygame


class Tuile:
	# un carreau sur l'échiquier

	def __init__(self, _type, état, pos, couleur):
		
		#les traitement sur les positions seront traité dans tout le programme
		# en mode 'case par case' et non au 'pixel près'

		self.pos = list(pos)
		self.rect_sur_écran = [self.pos[0]*BLOC + échiquier_coo[0], self.pos[1]*BLOC +échiquier_coo[1], BLOC, BLOC]
		self.couleur = couleur
		self.état = état
		self.type = _type
		self.pion = None

