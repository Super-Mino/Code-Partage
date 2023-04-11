import pygame
from utils import*

class Pion:
	#classe mère des pions

	def __init__(self, _type, pos, couleur):
		self.pos = pos
		self.type = _type # = quel type de pion
		self.rect_src = [0,0,0,0]
		self.couleur = couleur 
		#ici couleur est un nombre : 1 -> blanc, 2 -> noir


	def show(self, surface_écran, tileset): 

		x = self.pos[0] * BLOC + échiquier_coo[0]
		y = self.pos[1] * BLOC + échiquier_coo[1]

		surface_écran.blit(tileset, (x, y), self.rect_src)








#_____________________________________________________________#

class Roi(Pion):

	def __init__(self, pos, couleur):
		super().__init__( ROI, pos, couleur)

		niveau = 0
		if couleur == BLANC :
			niveau = 64
		elif couleur == NOIR : 
			niveau = 0

		self.rect_src = [0, niveau, 64, 64]



	def mouv_dispo(self, echiquier):

		mouv_possible = []
		x = self.pos[0]
		y = self.pos[1]

		#vérification des case autour de lui comme dans les règles 
		for i in range(-1, 2):
			if y-1 < 0 or x+i < 0 or x+i >= 8:
				continue

			if echiquier[y-1][x+i].état != self.couleur :
				mouv_possible.append([x+i, y-1])

		for i in range(-1, 2):
			if y+1 > 7 or x+i < 0 or x+i >= 8:
				continue

			if echiquier[y+1][x+i].état != self.couleur :
				mouv_possible.append([x+i, y+1])

		if x - 1 < 0 :
			pass 
		else : 
			if echiquier[y][x-1].état != self.couleur : 
				mouv_possible.append([x-1, y])

		if x + 1 >= 8 :
			pass 
		else : 
			if echiquier[y][x+1].état != self.couleur : 
				mouv_possible.append([x+1, y])

		return mouv_possible


















#_____________________________________________________________#		

class Reine(Pion):

	def __init__(self, pos, couleur):
		super().__init__( REINE, pos, couleur)

		niveau = 0
		if couleur == BLANC :
			niveau = 64
		elif couleur == NOIR : 
			niveau = 0

		self.rect_src = [1*64, niveau, 64, 64]



	def mouv_dispo(self, echiquier):

		mouv_possible = []
		x = self.pos[0]
		y = self.pos[1]

		#vérification de la colone qui va en haut. Au maximum si tout est vide et qu'elle
		#est aux extrémité : elle ne peut parcourrir que jusqu'à 7 cases 
		#dans les directions + (verticales) 
		#et jusqu'à 7 cases aussi dans les directions x (diagonales) 

				#vers le haut
		for i in range(1, 8):

			_y = y-i

			if _y < 0 : # = dernière ligne haute dépassée
				break

			if echiquier[int(_y)][int(x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(x)].état != self.couleur: # = si c est vide si c'est adversair
				mouv_possible.append([int(x), int(_y)])

				if echiquier[int(_y)][int(x)].état != VIDE : # = si c était un adversair
					break # on s'arrête au premier adversair trouvé 


		#vers le bas
		for i in range(1, 8):
			_y = y+i

			if _y >= 8: # = dernière ligne basse dépassée
				break

			if echiquier[int(_y)][int(x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(x)].état != self.couleur: # = si c est vide si c'est adversair
				mouv_possible.append([int(x), int(_y)])

				if echiquier[int(_y)][int(x)].état != VIDE : # = si c était un adversair
					break # on s'arrête au premier adversair trouvé 


		#vers la gauche
		for i in range(1, 8):
			_x = x-i

			if _x < 0 : # = colonne extrême gauche dépassée 
				break

			if echiquier[int(y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(y)])

				if echiquier[int(y)][int(_x)].état != VIDE:
					break;


		#vers la droite 
		for i in range(1, 8):
			_x = x+i

			if _x >= 8: # = colonne extrême droite dépassée
				break 

			if echiquier[int(y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(y)][int(_x)].état != self.couleur :
				mouv_possible.append([int(_x), int(y)])

				if echiquier[int(y)][int(_x)].état != VIDE:
					break

		

		#diagonale haute gauche
		for i in range(1, 8):
			_x = x-i
			_y = y-i

			if _x < 0 or _y < 0:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break;
		

		#diagonale haute droite
		for i in range(1, 8):
			_x = x+i
			_y = y-i

			if _x >= 8 or _y < 0:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break;


		#diagonale basse droite
		for i in range(1, 8):
			_x = x+i
			_y = y+i

			if _x >= 8 or _y >= 8:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break;


		#diagonale basse droite
		for i in range(1, 8):
			_x = x-i
			_y = y+i

			if _x < 0 or _y >= 8:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break


		return mouv_possible


















#_____________________________________________________________#

class Cavalier(Pion):

	def __init__(self, pos, couleur):
		super().__init__( CAVALIER, pos, couleur)

		niveau = 0
		if couleur == BLANC :
			niveau = 64
		elif couleur == NOIR : 
			niveau = 0

		self.rect_src = [4*64, niveau, 64, 64]



	def mouv_dispo(self, echiquier):

		mouv_possible = []
		x = self.pos[0]
		y = self.pos[1]

		#pour le cavalier c'est simple, il n'y a que 8 spots possible. On teste les quatres
		#et c'est bouclé :)

		# cette config : ##
						  #
						  # 
		_x = x-1
		_y = y-2

		if _x < 0 or _y < 0:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])



		# cette config : ##
			  			 #
			  			 # 
		_x = x+1
		_y = y-2

		if _x >= 8 or _y < 0:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])



		# cette config : #
						 #
						 ## 
		_x = x+1
		_y = y+2

		if _x >= 8 or _y >= 8:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])



		# cette config :  #
						  #
						 ## 
		_x = x-1
		_y = y+2

		if _x < 0 or _y >= 8:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				


		# cette config : ###
						 #				  
		_x = x-2
		_y = y+1

		if _x < 0 or _y >= 8:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])



		# cette config : ###
				 		   #				  
		_x = x+2
		_y = y+1

		if _x >= 8 or _y >= 8:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])



		#cette config : #
				 		###				  
		_x = x-2
		_y = y-1

		if _x < 0 or _y < 0:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])


		#cette config :   #
		 				###				  
		_x = x+2
		_y = y-1

		if _x >= 8 or _y < 0:
			pass 
		else:
			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])


		return mouv_possible




















#_____________________________________________________________#

class Fou(Pion):

	def __init__(self, pos, couleur):
		super().__init__( FOU, pos, couleur)

		niveau = 0
		if couleur == BLANC :
			niveau = 64
		elif couleur == NOIR : 
			niveau = 0

		self.rect_src = [3*64, niveau, 64, 64]



	def mouv_dispo(self, echiquier):
		
		mouv_possible = []
		x = self.pos[0]
		y = self.pos[1]

		#pour le fou c est simble, c'est la partie diagonale de la reine. Copié collé et 
		#c'est réglé :)

		#diagonale haute gauche
		for i in range(1, 8):
			_x = x-i
			_y = y-i

			if _x < 0 or _y < 0:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break;
		

		#diagonale haute droite
		for i in range(1, 8):
			_x = x+i
			_y = y-i

			if _x >= 8 or _y < 0:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break;


		#diagonale basse droite
		for i in range(1, 8):
			_x = x+i
			_y = y+i

			if _x >= 8 or _y >= 8:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break;


		#diagonale basse droite
		for i in range(1, 8):
			_x = x-i
			_y = y+i

			if _x < 0 or _y >= 8:
				break

			if echiquier[int(_y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(_y)])

				if echiquier[int(_y)][int(_x)].état != VIDE: 
					break


		return mouv_possible




















#_____________________________________________________________#

class Tour(Pion):

	def __init__(self, pos, couleur):
		super().__init__( TOUR, pos, couleur)

		niveau = 0
		if couleur == BLANC :
			niveau = 64
		elif couleur == NOIR : 
			niveau = 0

		self.rect_src = [2*64, niveau, 64, 64]



	def mouv_dispo(self, echiquier):
		
		mouv_possible = []
		x = self.pos[0]
		y = self.pos[1]

		#Et pour la tour c'est la même chose mais du côté vertical/horizontal

		#vers le haut
		for i in range(1, 8):

			_y = y-i

			if _y < 0 : # = dernière ligne haute dépassée
				break

			if echiquier[int(_y)][int(x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(x)].état != self.couleur: # = si c est vide si c'est adversair
				mouv_possible.append([int(x), int(_y)])

				if echiquier[int(_y)][int(x)].état != VIDE : # = si c était un adversair
					break # on s'arrête au premier adversair trouvé 

		#vers le bas
		for i in range(1, 8):
			_y = y+i

			if _y >= 8: # = dernière ligne basse dépassée
				break

			if echiquier[int(_y)][int(x)].état == self.couleur:
				break

			if echiquier[int(_y)][int(x)].état != self.couleur: # = si c est vide si c'est adversair
				mouv_possible.append([int(x), int(_y)])

				if echiquier[int(_y)][int(x)].état != VIDE : # = si c était un adversair
					break # on s'arrête au premier adversair trouvé 


		#vers la gauche
		for i in range(1, 8):
			_x = x-i

			if _x < 0 : # = colonne extrême gauche dépassée 
				break

			if echiquier[int(y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(y)][int(_x)].état != self.couleur:
				mouv_possible.append([int(_x), int(y)])

				if echiquier[int(y)][int(_x)].état != VIDE:
					break;


		#vers la droite 
		for i in range(1, 8):
			_x = x+i

			if _x >= 8: # = colonne extrême droite dépassée
				break 

			if echiquier[int(y)][int(_x)].état == self.couleur:
				break

			if echiquier[int(y)][int(_x)].état != self.couleur :
				mouv_possible.append([int(_x), int(y)])

				if echiquier[int(y)][int(_x)].état != VIDE:
					break

		return mouv_possible


















#_____________________________________________________________#

class Guerrier(Pion):

	def __init__(self, pos, couleur):
		super().__init__( GUERRIER, pos, couleur)

		niveau = 0
		if couleur == BLANC :
			niveau = 64
		elif couleur == NOIR : 
			niveau = 0

		self.rect_src = [5*64, niveau, 64, 64]
		self.le_premier_coup_est_joué = False



	def mouv_dispo(self, echiquier):
		
		mouv_possible = []
		x = self.pos[0]
		y = self.pos[1]


		#Par souci de simplicité je vais garder le même schéma tout le temps.
		#C'est à dire les noirs en haut, les blancs en bas ; ce qui fait que le devant 
		#des noirs est toujours vers le bas et le devant des blancs vers le haut.
		#Ainsi le mouvement des geurriers sera plus simple à coder il me semble. 

		direction = 1

		if self.couleur == NOIR:
			direction = 1
		elif self.couleur == BLANC:
			direction = -1


		_y = y + direction

		if _y >= 8 or _y < 0:
			pass 
		else:
			if echiquier[_y][x].état == VIDE: #et seulement vide, car ne capture qu'en diagonale 
				mouv_possible.append([x, _y])

				if self.le_premier_coup_est_joué == False:
					if echiquier[_y+direction][x].état == VIDE: #la deuxième case
						mouv_possible.append([x, _y+direction])

		#maintenant la capture
		_x = x-1
		if _x < 0 or _y >= 8 or _y < 0:
			pass 
		else:
			if echiquier[_y][_x].état != VIDE and echiquier[_y][_x].état != self.couleur:
			# = si c est un adversair sur la diagonale 1 devant soit
				 mouv_possible.append([_x, _y])

		_x = x+1
		if _x >= 8 or _y >= 8 or _y < 0:
			pass 
		else:
			if echiquier[_y][_x].état != VIDE and echiquier[_y][_x].état != self.couleur:
			# = si c est un adversair sur la diagonale 2 devant soit
				 mouv_possible.append([_x, _y])




		return mouv_possible



		













# #_____________________________________________________________#		

# class Reine(Pion):

# 	def __init__(self, pos, couleur):
# 		super().__init__( REINE, pos, couleur)

# 		niveau = 0
# 		if couleur == BLANC :
# 			niveau = 64
# 		elif couleur == NOIR : 
# 			niveau = 0

# 		self.rect_src = [1*64, niveau, 64, 64]



# 	def mouv_dispo(self, echiquier):

# 		mouv_possible = []
# 		x = self.pos[0]
# 		y = self.pos[1]

# 		#vérification de la colone qui va en haut. Au maximum si tout est vide et qu'elle
# 		#est aux extrémité : elle ne peut parcourrir que jusqu'à 7 cases 
# 		#dans les directions + (verticales) 
# 		#et jusqu'à 7 cases aussi dans les directions x (diagonales) 

# 		for i in range(1, 8):
# 			_y = y-i

# 			if _y < 0 : # = dernière ligne haute dépassée
# 				break

# 			if echiquier[int(_y)][int(x)].état != self.couleur: # = si c est vide si c'est adversair
# 				mouv_possible.append([int(x), int(_y)])

# 				if echiquier[int(_y)][int(x)].état != VIDE : # = si c était un adversair
# 					break # on s'arrête au premier adversair trouvé 

# 		#vers le bas
# 		for i in range(1, 8):
# 			_y = y+i

# 			if _y >= 8: # = dernière ligne basse dépassée
# 				break

# 			if echiquier[int(_y)][int(x)].état != self.couleur: # = si c est vide si c'est adversair
# 				mouv_possible.append([int(x), int(_y)])

# 				if echiquier[int(_y)][int(x)].état != VIDE : # = si c était un adversair
# 					break # on s'arrête au premier adversair trouvé 


# 		#vers la gauche
# 		for i in range(1, 8):
# 			_x = x-i

# 			if _x < 0 : # = colonne extrême gauche dépassée 
# 				break

# 			if echiquier[int(y)][int(_x)].état != self.couleur:
# 				mouv_possible.append([int(_x), int(y)])

# 				if echiquier[int(y)][int(_x)].état != VIDE:
# 					break;


# 		#vers la droite 
# 		for i in range(1, 8):
# 			_x = x+i

# 			if _x >= 8: # = colonne extrême droite dépassée
# 				break 

# 			if echiquier[int(y)][int(_x)].état != self.couleur :
# 				mouv_possible.append([int(_x), int(y)])

# 				if echiquier[int(y)][int(_x)].état != VIDE:
# 					break


# 		#diagonale haute gauche
# 		for i in range(1, 8):
# 			_x = x-i
# 			_y = y-i

# 			if _x < 0 or _y < 0:
# 				break

# 			if echiquier[int(_y)][int(_x)].état != self.couleur:
# 				mouv_possible.append([int(_x), int(_y)])

# 				if echiquier[int(_y)][int(_x)].état != VIDE: 
# 					break;
		

# 		#diagonale haute droite
# 		for i in range(1, 8):
# 			_x = x+i
# 			_y = y-i

# 			if _x >= 8 or _y < 0:
# 				break

# 			if echiquier[int(_y)][int(_x)].état != self.couleur:
# 				mouv_possible.append([int(_x), int(_y)])

# 				if echiquier[int(_y)][int(_x)].état != VIDE: 
# 					break;


# 		#diagonale basse droite
# 		for i in range(1, 8):
# 			_x = x+i
# 			_y = y+i

# 			if _x >= 8 or _y >= 8:
# 				break

# 			if echiquier[int(_y)][int(_x)].état != self.couleur:
# 				mouv_possible.append([int(_x), int(_y)])

# 				if echiquier[int(_y)][int(_x)].état != VIDE: 
# 					break;


# 		#diagonale basse droite
# 		for i in range(1, 8):
# 			_x = x-i
# 			_y = y+i

# 			if _x < 0 or _y >= 8:
# 				break

# 			if echiquier[int(_y)][int(_x)].état != self.couleur:
# 				mouv_possible.append([int(_x), int(_y)])

# 				if echiquier[int(_y)][int(_x)].état != VIDE: 
# 					break


# 		return mouv_possible


