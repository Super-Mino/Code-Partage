BLOC = 64
FPS = 30
VIDE = 0
BLANC = 1
NOIR = 2

ROI = 10
REINE = 11
TOUR = 12
CAVALIER = 13
FOU = 14
GUERRIER = 15 #pion classique

échiquier_dim = BLOC * 8
écran_marge_gauche = BLOC * 2
écran_marge_droite = BLOC * 2
écran_marge_haute = BLOC * 2
écran_marge_basse = BLOC * 2 

écran_largeur = écran_marge_gauche + échiquier_dim + écran_marge_gauche
écran_hauteur = écran_marge_haute + échiquier_dim + écran_marge_basse

échiquier_coo = (écran_marge_gauche, écran_marge_haute)

couleur_brun = (35, 12, 5) #(50, 17, 5)
couleur_beige = (252, 206, 162)
couleur_noire = (5, 5, 5)
couleur_brun2 = (130, 75, 50)
couleur_verte = (20, 206, 30)
couleur_rouge = (223, 15, 16)
couleur_jaune = (223, 170, 0)
couleur_bleue = (12, 2, 232)

couleur_de_fond = couleur_brun2
couleur_case_noire = couleur_brun 
couleur_case_blanche = couleur_beige
cadre_couleur = couleur_noire 

cadre_épaisseur = 6
cadre_coo = (échiquier_coo[0] - cadre_épaisseur, échiquier_coo[1]- cadre_épaisseur)

cadre_rect = list(cadre_coo)
cadre_rect.append(échiquier_dim+cadre_épaisseur*2)
cadre_rect.append(échiquier_dim+cadre_épaisseur*2)
# cadre_rect = cadre_rect.extend([échiquier_dim, échiquier_dim])
# print(cadre_rect)
# print(cadre_coo)
