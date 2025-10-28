#ifndef _SDL3_GSlib_FONT_HPP_
#define _SDL3_GSlib_FONT_HPP_

//Version modifiée le 21/10/25 (à moins d'oubli de modifier cette ligne ou autre).

	#include "SDL3_GSlib.hpp"
	#include <SDL3/SDL_ttf.h>
	#include <unordered_map>
	#include <map>
	#include <array>
	#include <fstream>




namespace gs
{


//Sur un texte/str qui contient plusieurs lignes (ou retour à la ligne) cette fonction renvoie la dernière ligne qui n'a pas de caractère de fin de ligne. Par exemple, considérons '%' comme le caractère de fin de ligne, alors dans "abc%defgh%ijk" le return serait "ijk", et dans "abc%defgh%ijk%" le return serait "" (str vide, car ça se termine sur un caractère de fin de ligne).
//Et oui par soucis de simplicité, on autorise pas de caractère fin de ligne UTF-8, mais seulement ASCII, permettant de se servir du 'char' et du 'std::string' tranquillement.
std::string getLastTextChunk(std::string text, char end_ln_ch);


namespace te
{
	enum Font_Rendering_Type {DEFAULT, SOLID, SHADED, BLENDED, SELF_DEFAULT};
	enum Paragraph_Lines_Type {RIGHT_BEGIN, LEFT_BEGIN, CENTERED};
};

struct INTERNAL_USE_Font_Manager
{
	INTERNAL_USE_Font_Manager() = default;
	~INTERNAL_USE_Font_Manager() 
	{
		if(TTF_WasInit()) 
		{
			TTF_Quit(); 
			if(gs::show_comments)
				std::cout << "\tSDL_TTF exit success!\n";
		}
	}
};







class Font
{
	public :

		Font() = default;
		Font(SDL_Renderer* ren, const char* font_path, size_t ptsize, te::Font_Rendering_Type default_type = te::SOLID, bool close_TTF_at_my_exit=false);
		~Font(); 

		
		bool init(SDL_Renderer* ren, const char* font_path, size_t ptsize, te::Font_Rendering_Type default_type = te::SOLID, bool close_TTF_at_my_exit=false);
		bool isInit() const;


		void setShowBackground(bool val);
		bool getShowBackground() const;


		size_t getPtsize() const;
		void setPtsize(int ptsize);


		TTF_Font* getFont();
		int getFontStyle() const;
		void setFontStyle(int new_font_style);

		
		void setRenderingType(te::Font_Rendering_Type new_type);
		te::Font_Rendering_Type getRenderingType() const;
		

		void setBackgroundColor(SDL_Color bg_color);
		SDL_Color getBackgroundColor() const;
		

		void setRenderer(SDL_Renderer* ren);
		SDL_Renderer* getRenderer();
		
		
		void setBlendMode(SDL_BlendMode blend_mode);
		SDL_BlendMode getBlendMode() const; 
		
		
		void setUserInterventionCallback(void (*userInterventionCallback)(SDL_Surface*, const std::string&));
		void (*getUserInterventionCallback())(SDL_Surface* surface, const std::string& rendered_text);


		std::string lastError() const;


		SDL_Texture* render(const char* text, SDL_Color text_color, 
							te::Font_Rendering_Type rendering_type=te::SELF_DEFAULT, 
							SDL_Color* bg_color=nullptr, 
							SDL_Rect* text_size_out=nullptr);
		
		SDL_Texture* renderMultiLines(std::vector<std::string> text, SDL_Color text_color, 
							te::Paragraph_Lines_Type paragraph_lines_type = te::LEFT_BEGIN,
							SDL_Rect* text_size_out=nullptr, 
							te::Font_Rendering_Type rendering_type=te::SELF_DEFAULT, 
							SDL_Color* bg_color=nullptr);
		


	protected : 

		te::Font_Rendering_Type	m_default_type=te::SOLID;
		size_t 					m_ptsize=32;
		bool 					m_is_init=false;
		bool 					m_close_TTF_at_my_exit=false;
		bool 					m_show_background=true;
		std::string 			m_last_error="no error";
		TTF_Font* 				m_SDL_font=nullptr;
		SDL_Color 				m_bg_color={0, 0, 0, 255};
		SDL_Renderer*  			m_ren_ref=nullptr;
		int  					m_font_style{TTF_STYLE_NORMAL};
		SDL_BlendMode           m_blend_mode=SDL_BLENDMODE_BLEND;
		void (*m_userInterventionCallback)(SDL_Surface* surface, const std::string& rendered_text)=nullptr; //Optionnel - traiter les pixels du texte rendu, avant de générer la texture finale.

};






class Rendered_Str_Bank // A faire attention lors des déplacements. (Je pense que cela est dû à l'utilisation de std::unordered_map<>.)
{
	public:
		Rendered_Str_Bank() = default;
		Rendered_Str_Bank(Font* font);
		~Rendered_Str_Bank() = default; 

		SDL_Texture* get(std::string str,
							uint8_t style, uint16_t ptsize, 
							uint32_t fg_color, uint32_t bg_color);


		bool setFont(Font& font);
		bool setFont(Font* font);
		Font* getFont() const;


	protected:
		std::unordered_map<uint8_t, /*Par style*/
			std::unordered_map<uint16_t, /*Par taille*/
			std::unordered_map<uint32_t, /*Par couleur (RGBA) du trait*/
			std::unordered_map<uint32_t, /*Par couleur (RGBA) de fond*/
			std::unordered_map<std::string, SDL_Texture*> >> > > m_bank;

		Font* m_font{nullptr};
};








struct Selection_Act_Record //Permet de garder une trace d'une action effectuée en lien avec une sélection.
{
	bool it_is_for_replacement=false;
	int begin_cursor_pos=-1; //La position du curseur avant l'action.
	int end_cursor_pos=-1; //La position du curseur à la fin de l'action.
	Vec2i selection_bounds{-1, -1}; // (en positions de curseur) Les limites de la sélection (avant l'action). 
	std::string origin_str; //Pour la portion de texte qui fut remplacée ou effacée.
	std::string new_str; //Pour le texte qui a remplacé.
};


class Text_Line
{
	public:

		Text_Line() = default;
		Text_Line(Rendered_Str_Bank* bank);
		~Text_Line() = default; 

		bool initAll(SDL_Renderer* ren, Font& font, Rendered_Str_Bank& bank, const char* font_path, uint16_t ptsize, uint32_t fg_rgba_col); //Initialise le font et la banque (si pas déjà fait), afin de pouvoir être utilisés par le gs::Text_Line.

		bool setBank(Rendered_Str_Bank& bank);
		bool setBank(Rendered_Str_Bank* bank);
		Rendered_Str_Bank* getBank() const;

		bool setEndLineChar(const char ch);
		char getEndLineChar() const;

		std::string setText(std::string line);
		std::string getText() const;
		std::string getCursorChar() const;
		std::string getTextFromBeginToCursor() const; //Avec le caractère au niveau du curseur NON inclus.
		std::string getTextFromCursorToEnd() const; //Avec le caractère au niveau du curseur inclus.
		std::string getChar(int cursor_pos) const; //UTF-8

		bool setPos(gs::Vec2i pos);
		gs::Vec2i getPos() const;

		//La taille actuelle de la zone de texte.
		bool setSize(gs::Vec2ui size);
		gs::Vec2ui getSize() const;
		gs::Vec2i getSize_i() const;
		
		SDL_Rect getArea() const;

		//La largeur (en pixels) requise si tous les caractères devaient être affichés.
		unsigned int getTextTotalRequiredWidth() const;

		bool setMaxSize(gs::Vec2ui max_size);
		gs::Vec2ui getMaxSize() const;

		int setCursorPosByPixels(int pixel_index);
		int getCursorPosPixelIndex() const; 
		
		bool setShowCursor(bool new_state);
		bool getShowCursor() const;

		int setCursorPos(int pos); 
		int getCursorPos() const; 
		int incrCursorPos(int val);
		int setMaxCursorPos();
		int getMaxCursorPos() const;

		bool show(SDL_Renderer* ren, int start_x=0, char tabulation_size=4, Vec2i up_down_limit_y={-1, int(0xFFFFFFFF)}); 
		gs::Vec2ui update(char tabulation_size=4); //Redimentionne en fontion du texte. 
		bool isUpToDate() const;
		
		bool addRules(int begin_index, uint8_t style, uint16_t ptsize, uint32_t fg_color, uint32_t bg_color);
		bool clearRules(uint8_t style, uint16_t ptsize, uint32_t fg_color, uint32_t bg_color);
		

		//Ces deux méthodes se réfèrent à la position du curseur. 
		bool addText(const std::string& str, std::string* out=nullptr, bool allow_cursor_to_be_moved=true);
		std::string remove(int count_to_left);
		// bool replace(int count_to_right, std::string str);


		bool selectTextPortionByPixelsPos(int min_pix_pos, int max_pix_pos);		
		bool selectTextPortionByPixelsPos_From(bool from_begin, int pix_pos); //Si 'from_begin==false', alors on selectionne à partir de la fin.
		bool selectTextPortionByCursorsPos(int left_cursor_pos, int right_cursor_pos); //Si c'est un succès, la zone sélectionnée est celle entre les deux curseurs, ce qui fait que le dernier caractère sélectionné est celui juste à gauche du curseur de droite, et non le caractère qui est au niveau du curseur de droite. Exemple: dans "ab[cde(fghij", le '[' représente le curseur de gauche avec comme indice (pos) 2, et le '(' celui de droite avec comme indice (pos) 5. Le texte qui sera sélectionné est "cde", et le 'f' qui est lui est au niveau du curseur de droite n'y est pas inclus. Note: si les arguments reçus ne correspondent pas à ce qui est attendu (c-à-d que celui de gauche est celui qui est à droite, et donc que celui de droite est celui qui est à gauche), la méthode se débrouille pour remettre les choses en place.
		bool selectTextPortionByCursorToken(const std::string& ASCII_token_separators); 
		bool selectAll();
		bool haveSelection() const;
		void unselectTextPortion();
		void setSelectionColor(uint32_t col);
		std::string getSelectedText() const;
		bool eraseSelection();
		bool replaceSelection(const std::string& str_to_replace_with, std::string* out=nullptr);
		Selection_Act_Record getLastRecordedSelectionAct() const;

		bool replace(int left_cursor_pos, int right_cursor_pos, const std::string& str, std::string* out=nullptr);


	protected: //Text_Line

		int determineCursorPosByPixelsPos(int pixel_pos); //Renvoie la position (en unité de caractère) qu'aurait le curseur si on le déterminait par rapport à une position en pixel (position relative à la 'gs::Text_Line').
		std::string applyRules(const std::string& rules);
		int getStdstringPosByVirtualCurPos(int virtual_cursor_pos) const; 
		bool dealWithSelection(bool it_is_for_replacement, const std::string& str="", std::string* out=nullptr); //Pour remplacer OU effacer le contenu sélectionné (en arg: 'true' pour remplacer, 'false' pour effacer).
		
		std::string  				m_text;
		Rendered_Str_Bank* 			m_str_bank=nullptr;
		char  						m_end_line_char='\n';

		gs::Vec2i 					m_pos{0, 0};
		gs::Vec2ui 					m_size{200,200}; //Dimensions variables "automatiquement" en fonction du texte. 
		gs::Vec2ui 					m_max_size{750, 300}; //Etablit la limite des dimentions.
		bool 						m_is_up_to_date{true};//Temoin des modifications du texte, afin d'adapter les dimensions. 
		unsigned int 				m_total_required_w=0; //La largeur requise si tous les caractères devaient être affichés.
		float 						m_italic_w_percentage_to_take=0.8f; //Le pourcentage de la largeur que l'on va considérer pour les caractère en italique (pour éviter le large espace qui peut apparaître entre les lettres).

		int  						m_virtual_cursor_pos=0; //Position du curseur par rapport aux caractères selon la perception humaine. Et si je ne m'abuse, la philosophie derrière est que la position maximale du curseur est celle juste après le dernier caractère qui n'est pas le caractère de fin de ligne (caractère de fin de ligne qui lui n'est pas affiché). Par exemple, dans la chaîne "abcdefgh%" où '%' serait le caractère de fin de ligne, la position maximale du curseur serait comme suit : "abcdefgh|%" avec le '|' qui représente le curseur (et graphiquement le texte affiché ressemblerait à : abcdefgh| (avec le '|' == représentation graphique du curseur)).
		std::vector<short> 			m_chars_width; //Largeur en pixel de chaque caractère (de leurs textures (+ ou -)) une fois généré. 
		std::vector<unsigned char> 	m_chars_byte_count; //-> combien d'octets prend chaque caractère. m_chars_width.size() ou m_chars_byte_count.size() correspondent à la quantité de caractère de la ligne, sans compter le caractère de fin de ligne s'il est présent. 
		bool 						m_show_cursor=false;		

		std::map<int, std::string>	m_rules{{0/*char index*/, "00019240240240255000000000000"}}; //Pour les différents changements.
		std::array<uint32_t, 4>  	m_cur_rules{0,19,0xF0F0F0FF,0}; //Pour les actuelles règles. -> {style, ptsize, fg, bg}.	

		bool                        m_have_a_selection=false; //Détermine s'il y a actuellement une selection.
		gs::Vec2i                   m_selected_portion_boundaries{0,0}; //Les bornes, en indice des caractères (UTF-8) sélectionnés (grosso modo c'est des positions de curseur). Le '.x' pour l'indice du premier caractère sélectionné, et le '.y' pour le dernier sélectionné.
		uint32_t                    m_selection_col=0x008A9AFFu; //La couleur du surlignement de la sélection.
		Selection_Act_Record        m_last_recorded_selection_act;

}; //Text_Line








namespace te
{
	static const int NO_ACTIVE_LINE=-1; // précédement 'NO_SELECTED_LINE'.

	enum Text_Win_Area{FOREGROUND, BACKGROUND, SELECTED_LINE_BACKGROUND, SELECTION_BACKGROUND};
	
	enum Text_Win_Action_Type{NONE, INFO, INSERT_CHAR, REMOVE_CHAR, NEW_CURSOR_POS, NEW_LINE, 
							REMOVE_LINE, NEW_TEXT, ERASE_LINE_SELECTION, REPLACE_LINE_SELECTION,  //'REPLACE_LINE_SELECTION' pour un remplacement de texte fait sur une sélection qui ne s'étend que sur une seule ligne, et, qui n'engendre pas d'autres lignes.
                            LINE_NEW_TEXT};
	
	enum Text_Win_Action_Info{NO_INFO, END_OF_NEW_LINE_PROCESS,
							END_OF_REMOVE_LINE_PROCESS, //Lorsque le processus de retrait de ligne ne se constitue pas seulement d'un 'REMOVE_LINE'. 
							END_OF_MULTILINES_INSERT,
							END_OF_ERASE_MULTILINES_SELECTION, 
							END_OF_REPLACE_SELECTION_MULTIACTIONS};// 'END_OF_REPLACE_SELECTION_MULTIACTIONS' pour lorsqu'il y a plusieurs lignes concernées à un moment ou un autre du processus. (Voir vers la fin de l'implémentation de 'Text_Window::replaceSelection()'.)
}

//Note: le type 'NEW_TEXT' veut dire que le texte entier du 'Text_Window' a été changé (et non nécessairement seulement celui d'une seule ligne, désigné par 'LINE_NEW_TEXT').


struct Text_Print_Rules
{
	int begin_index=0; //En UTF-8, c-à-d si dans le text on a un caractère qui compte trois octets, passé ce caractère on rajoute +1 à l'indice et non +3.
	uint8_t style=TTF_STYLE_NORMAL;
	uint16_t ptsize=25u;
	uint32_t fg_color=0xEEEEEEEE; 
	uint32_t bg_color=0u; 
};

struct Line__Text_Idx
{
	int idx;
	std::string text;
};


struct Text_Win_Action
{
	//Note: dans 'Text_Window::act()' avec une action de type 'NEW_LINE', l'attribut 'Text_Win_Action::begin_cursor_pos' sert à déterminer l'argument 'bool incr_active_ln_idx' de 'Text_Window::addLine()'. Pour 'begin_cursor_pos == 1', 'incr_active_ln_idx' est mis à 'true', et pour tout autre valeur de 'begin_cursor_pos', 'incr_active_ln_idx' est mis à 'false' (afin de favoriser un 'true' intentionnel).
	//Note: pour l'enregistrement des actions de type 'REPLACE_LINE_SELECTION', l'attribut 'Text_Win_Action::str' sert à renseigner la portion de texte qui a été remplacée, et l'attribut 'Text_Win_Action::str_2' sert à renseigner ce qui a remplacé.
	//      Dans la même optique, pour les actions de type 'ERASE_LINE_SELECTION', l'attribut 'Text_Win_Action::str' sert à renseigner la portion de texte qui a été effacée.
	//Note: pour les actions de type 'NEW_TEXT', l'attribut 'Text_Win_Action::str' sert à renseigner le nouveau texte, et l'attribut 'Text_Win_Action::str_2' l'ancien texte.
	
	te::Text_Win_Action_Type  		type=te::NONE;
	int 							line_index=-1; //L'indice de la ligne concernée par l'opération.
	std::string 					str;
	int  							new_cursor_pos=-1; //La position obtenue (ou à obtenir) du curseur après l'opération.
	int  							begin_cursor_pos=-1; //La position à la quelle était le curseur avant l'opération, OU, doit commencer le curseur lors d'une demande de suppression avec Text_Window::act(), ['begin_cursor_pos' - 'new_cursor_pos' == nombre de caractères à supprimer, si toutefois 'new_cursor_pos' <= 'begin_cursor_pos'].
	Vec2i                           selection_bounds; // Concernant les actions liées à de la sélection de texte : (en positions de curseur) Les limites de la sélection (avant l'action). 
	int 							prev_line_index=-1; //L'indice de la ligne concernée avant l'opération si celle-ci diffère de la ligne concernée après opération (attribut ajouté particulièrement pour l'action 'NEW_CURSOR_POS').
	std::string                     str_2; //Sert à mentionner un autre texte concerné par l'opération. Rajouté particulièrement pour l'action 'INSERT_CHAR', où l'on pourrait avoir besoin du texte allant du curseur à la fin de ligne, si le texte inséré contiendait un caractère de fin de ligne (et donc rejetant (possiblement) une partie du texte de la ligne). C'est particulièrement là en vue d'une marche arrière (remettre la ligne à son état avant cet action).
	
	int action_idx=0; //Dans le cas d'une action/évènement qui serait composé d'une suite de plusieurs actions, cette attribut indique la position de l'action courante dans la chaîne d'actions.
                      //Il est important que 'action_idx' soit par défaut initialisé à 0, afin que les actions uniques "n'aient" pas à s'en occuper. 
	
	te::Text_Win_Action_Info info=te::NO_INFO;
};


struct Rendered_Line_Info
{
	int line_number=0; //Numéro de la ligne.
	int y_pos=0;
	bool is_a_block_header=false;
	bool is_folded_block=false;
};


class Blocks_Folder 
{
	public:
		struct Code_Block {
			int header_line_index=0; //indice de la première ligne du bloc de code (l'en-tête). Elle n'est pas incluse dans la partie "cachable".
			int header_line_depth=0; //Profondeur de la ligne d'en-tête, en espaces.
			int lines_count=0; //Combien de lignes sont incluses dans ce bloc (en ne comptant pas l'en-tête).
			bool is_folded_block=false; //Rétracté ('true'), ou visible/ouvert ('false').
		}; 
		
		//Exemple:
		//
		//    Cette ligne est l'en-tête de ce bloc (référencée par  'Blocks_Folder::Code_Block::header_line_index').
		//        Et il y a en dessous
		//        trois lignes, de ce fait
		//        'Blocks_Folder::Code_Block::lines_count' aura une valeur de 3.
		
		
		struct Blocks_By_Depth
		{
			int header_depth=0;
			std::vector<Code_Block> blocks;
		};
		
		static const int INVALID_DEPTH = -1; 
		
		bool isLineBlockHeader(int line_index, bool* out__is_folded_block=nullptr);
		bool isLineHidden(int line_index);
		int getHiddenLinesCount(int from_begin_to__ln_idx=-1); // Avec 'from_begin_to__ln_idx < 0' c'est la totalité des lignes repliées qui est comptée, sinon, seules les lignes repliées qui se trouvent avant 'from_begin_to__ln_idx' seront comptabilisées).
		int getLineDepth(const std::string& line_text, char end_line_char, int tab_size); 

		
		void processLinesIndentsMapping(const std::vector<Text_Line>& lines, int tab_size); //Mappe les profondeurs des différentes lignes.
		void updateByIndents(); //Détermine les différents blocs dans le texte.
		void takeEdit(te::Text_Win_Action_Type processed_edit, int line_index, const Text_Line* line_ptr, int tab_size);
	
		void setCodeBlockFoldedState(int block_header_line_idx, bool inverse_state, bool otherwise__folded_state=false);
		float adjustYScroll(float y_scroll, int lines_delta) const;
		int adjustClickYPos(int t_win_relative__click_y, float y_scroll, int lines_delta) const;
		void unhideLine(int line_index);
		void setAllBlocksWithSameState(bool folded_state, int target_depth=INVALID_DEPTH); // Lorsque 'target_depth==INVALID_DEPTH', l'opération est faite sur l'ensemble des blocs, sans viser un niveau de profondeur en particulier.
		
		
	
	protected:
	
		struct Block_Range {int min=0, max=0; bool is_folded_block=false;}; // Note: min et max inclus.
		void mapBlocksRanges();
		bool _isLineBlockHeader(int line_index, Code_Block* out) const;
		void insertCodeBlock(const Code_Block& block);
		
		std::vector<Blocks_By_Depth> m_depths;
		std::vector<int> m_lines_depth; 
		bool m_blocks_folder_is_up_to_date=false;
		bool m_need_to_map_blocks_ranges=true;
		std::vector<Block_Range> m_blocks_ranges; //Dans ce 'std::vector', chaque 'Block_Range' représente les bornes en indices (les deux incluses) des lignes incluses dans un bloc (les lignes "cachables"). De ce fait l'indice de la ligne d'en-tête est 'Block_Range::min'-1. 
		
		
};	



class Text_Window
{
	public:

		Text_Window() = default;
		~Text_Window() = default;

		bool initByText(Font& font, SDL_Rect pos_size, const std::string& text, char end_line_char='\n');

		bool initByFile(Font& font, SDL_Rect pos_size, const char* file_path, char end_line_char='\n');
		bool initByFile(Font& font, SDL_Rect pos_size, const std::string& file_path, char end_line_char='\n');

		bool openFile(const char* file_path);

		bool setText(const std::string& text, bool record_this_action=true, Text_Win_Action* out__done_action=nullptr);
		bool insertText(const std::string& str, bool when_not_have_selection__process_multilines_completion=true, bool record_this_action=true); //Au niveau du curseur de la ligne courante (si présente).
		std::string getText(int line_index=-1); //`line_index' négative = pour toute les lignes.
		std::string getCursorChar();
// 
		bool show(SDL_Renderer* ren, bool highlight_selected_line=false, std::vector<Rendered_Line_Info>* rendered_lines_info=nullptr);
		
		bool processEvent(SDL_Event& event, SDL_Window* win);
		void update(double dt); 
		bool mouseClickedAt(const Vec2i& pos); 

		bool save(const char* file_path) const;

		bool addLine(int index, const std::string& text, std::string* out=nullptr, bool incr_active_ln_idx=false, bool record_this_action=true, Text_Win_Action* out__done_action=nullptr);
		bool removeLine(int index, std::string* out__rmvd_line_text=nullptr, bool record_this_action=true, Text_Win_Action* out__done_action=nullptr);
		bool act(const Text_Win_Action& action, bool record_this_action=false, Text_Win_Action* out__done_action=nullptr);

		bool setCurrLineOfEdit(int line_index);
		int getCurrLineOfEditIndex() const; //getSelectedLineIndex() const;
		bool haveCurrActiveLine() const;
		bool setCursorPos(int cursor_index);
		int getCursorPos() const;

		bool setPos(gs::Vec2i pos);
		gs::Vec2i getPos() const;

		bool setSize(gs::Vec2ui size);
		gs::Vec2ui getSize() const;

		bool setMarginX(short val);
		short getMarginX() const;

		float setScrollY(float val);
		float getScrollY(); //const;

		float setScrollX(float val);
		float getScrollX() const;

		bool setTabulationSize(char val);
		char getTabulationSize() const;
		
		bool isPointInTextArea(gs::Vec2i pos) const;
		bool isPointInFriendArea(gs::Vec2i pos) const;

		bool setColor(te::Text_Win_Area area, SDL_FColor color);
		SDL_FColor getColor(te::Text_Win_Area area) const;

		bool setMouseClickedIn(); //Faire considérer que le gs::Text_Window est cliqué par la souris même si ce n'est pas le cas.  
		bool getMouseClickedIn();

		//Pour les modifications personnalisé du texte (les thèmes de couleur par exemple).
		bool setTextColorEditor(int (*textColoring_)(const std::vector<Line__Text_Idx>& lines,
													std::map<int, std::vector<Text_Print_Rules>> &out_rules, Text_Print_Rules default_rules));
		bool haveTextColorEditor() const;

		bool moveCursor(int on_x_axis, int on_y_axis); 
		bool setLookAtMouse(bool new_state);

		std::string setTitle(std::string title);
		std::string getTitle() const;

		size_t getLinesCount() const;
		short getLinesDelta() const;

		void clearLastActions(); 
		std::vector<Text_Win_Action> getLastActions() const;
		bool setResetLastActionsObserverWhenShow(bool new_state);

		std::vector<SDL_Rect> setFriendAreas(const std::vector<SDL_Rect>& areas);
		std::vector<SDL_Rect> getFriendAreas() const;

		unsigned int getMaxLineW() const;

		bool setPtsize(short ptsize);
		short getPtsize() const;
		
		void unselect();
		bool haveSelection() const;
		std::string getSelection() const;
		bool eraseSelection(bool record_this_action=true);
		bool replaceSelection(const std::string& str_to_place, bool record_this_action=true);
		
		bool userResponsabilityMethode__replace_inCurrLineOfEdit(int left_cursor_pos, int right_cursor_pos, const std::string& str, std::string* out=nullptr, bool update_the_line=true);
		
		std::string copy(bool cut=false); //Si sélection renvoie sélection, sinon si ligne active renvoie ligne active, sinon chaîne vide. Si erreur renvoie chaîne vide.
		std::string cut(); //Fait appel à 'Text_Window:copy()' avec 'cut == true'.
		bool past(const std::string& str); //Fait appel à 'Text_Window::insertText()', et pour résumer le comportement (si je ne m'abuse) : si sélection remplace la sélection, sinon si ligne active insert au niveau du curseur de la ligne active.
		
		bool inverseCodeBlockFoldedState(int block_header_line_idx);
		bool foldAll(bool fold_all=true, int otherwise__target_depth=0); //'otherwise__target_depth' en 'espaces'.
		bool unfoldAll(bool unfold_all=true, int otherwise__target_depth=0); //'otherwise__target_depth' en 'espaces'.
		int getFoldedLinesCount();

	protected: //Text_Window

		// void clearEvents();
		// void setInput(std::string key);
		
		struct Wheel_Details {bool wheeled=false; int val=0; bool is_on_ending_fade=false; float fading_timer=0.225f; float speed=120.f;};
		
		void examineEvents(double dt); 
		void updateXScroll();
		void updateYScroll();
		void textColoring();
		Text_Win_Action& pushAction(te::Text_Win_Action_Type type, int line_index=-1, const std::string& str="", int new_cursor_pos=-1, int begin_cursor_pos=-1, Vec2i sel_bounds={-1,-1}, int prev_line_index=-1, const std::string& str_2="");
		Text_Win_Action makeAction(te::Text_Win_Action_Type type, int line_index=-1, const std::string& str="", int new_cursor_pos=-1, int begin_cursor_pos=-1, Vec2i sel_bounds={-1,-1}, int prev_line_index=-1, const std::string& str_2="");
		void setMaxLineW();
		void updateMaxLineW(int before_updating__last_actions_count);
		void processSelection(int anchor_line_idx); 
		bool processTabOnSelection(bool record_this_action);
		bool processBackTabOnSelection(bool record_this_action);
		bool setASelectedLine(Text_Line& line, int index, bool is_anchor); //"Enregistrer" une ligne comme ayant une partie sélectionnée (ou totalement sélectionnée).
		
		struct Line_In_Selection { //Récupère les données d'une lignes qui fait partie d'une/la sélection courante.
			Text_Line* ptr=nullptr;
			int index=-1;
			bool is_anchor=false;
		};
		bool checkLineInSelValidity(const Line_In_Selection& l_i_s) const;
		gs::Vec_Bool_Int completeTextInsertion(int start_line_idx, std::string str, bool record_this_action, int curr_action_idx, int cursor_pos_of_the_latest_compl_ln); //Utilitaire qui permet d'insérer les lignes complétant une insertion qui introduit plusieurs lignes. Cette méthode rajoute les lignes nécessaires. Elle renvoie {état de résussite, sa dernière valeur de 'curr_action_idx'};
		

		std::string    					m_title="Text Window"; //Utilité sur l'identification externe. 

		bool 							m_is_init{false};
		std::vector<Text_Line> 			m_lines; 
		Rendered_Str_Bank 				m_common_bank; //|  
		char  							m_end_line_char='\n';
		short  							m_lines_delta=1; //Espace entre deux coo de 'gs::Text_Line'.
		short  							m_common_ptsize=-1;//19; //Ptsize pour toutes les lignes. '-1' pour indiquer que ça n'a pas encore été initialisé; c'est pour permettre (par un 'Text_Window::setPtsize()' avant initialisation) d'initialiser avec un ptsize autre que celui du font reçu, et ne pas avoir à re-générer (en interne) le texte avec le ptsize voulu, après avoir déjà généré avec le ptsize du font reçu.
		char  							m_tab_size=4;
		
		//Si je ne me trompe pas, les valeurs de ce bloc-ci d'attributs sont exprimées en pixels. 
		gs::Vec2i 						m_pos{0,0};
		gs::Vec2ui 						m_size{100,100}; 
		short  							m_x_margin=0; //Concerne les gs::Text_Line, et non les textes inclus dedans.
		float  							m_y_scroll=0.f; //Plus m_y_scroll est grand, plus l'on descend dans la page/le fichier/le texte (si je ne m'abuse).
		float  							m_x_scroll=0.f; //Concerne les textes inclus dans les gs::Text_Line. Aide pour visualiser le concept : on peut voir le 'gs::Text_Window' comme une fenêtre ouvrant sur des lignes de texte. Plus l'on déplace cette fenêtre vers la droite (en augmentant 'm_x_scroll'), plus les lignes semblent aller vers la gauche. En gros, en augmentant le scrolling horizontal, on se rapproche de la droite du texte, et pour pouvoir visualiser cela on fait "aller les lignes vers la gauche". Et lorsque l'on diminue le scrolling horizontal (pour que le 'gs::Text_Window' aille plus à gauche dans le texte), on fait "aller les lignes vers la droite".
		gs::Vec2ui 						m_max_ln_w={0, 0}; // {valeur, idx de la ligne}. La largeur requise pour afficher totalement la plus longue ligne. (Peut aider notamment à la gestion du scrolling horizontal). 


		int  							m_curr_active_line_idx=te::NO_ACTIVE_LINE; // Précédemment nommé 'm_selected_line_index', indique l'indice de la ligne sur laquelle se trouve le curseur, la ligne qui reçoit les entrés clavier.
		Input  							m_inputs;
		bool 							m_check_mouse=true;
		Vec2i 							m_mouse_clicked_pos={0,0};
		Wheel_Details       			m_wheel; //pour le scroll à la molette. 
		bool  							m_last_mouse_click_is_in=false; //Afin de savoir le gs::Text_Window est "actif". 
		std::vector<SDL_Rect>  			m_friend_areas; //Le click dans ces zones ne désélectionne le gs::Text_Window. Utile pour des zones extérieur au champ de texte, comme les scrollbars afin de garder le focus même en manipulant ces dernières. 
		gs::Mouse_Tracker               m_mouse_tracker; //Pour gérer la sélection.
		Blocks_Folder                   m_blocks_folder; //Pour permettre de cacher des blocs de code.

		SDL_FColor 						m_bg_color{200.f,200.f,200.f,255.f}; //Couleur de l'arrière plan du 'gs::Text_Window'.  
		SDL_FColor 						m_fg_color{50.f,50.f,50.f,255.f}; //Couleur du text, par défaut.
		SDL_FColor 						m_selected_line_bg_color{220.f,220.f,220.f,200.f}; //Concerne la ligne sur laquelle se trouve le curseur.
		bool 							m_text_color_editor_changed_from_ok_to_nullptr=false;

		std::vector<Text_Win_Action> 	m_last_actions; //Se réinitialise lors d'un gs::Text_Window::show() (si `m_reset_last_act_observer_when_show'==true), ou de manière forcée avec gs::Text_Window::lastActionSeen() à l'extérieur. 
		bool 							m_reset_last_act_observer_when_show=true;
		
		std::vector<Line_In_Selection>  m_selected_lines; //Celle-ci concerne la selection de texte (souvent illustrer par un "surlignement" dans les éditeurs de texte). 
		bool                            m_selection_is_up_to_down; //Permet de savoir dans quelle sens se fit la sélection, pour renvoyer le texte dans le bon ordre dans 'gs::Text_Window::getSelection()'.
		uint32_t                        m_selection_bg_color=0x008A9AFFu; //En RGBA, couleur de l'arrière plan du texte sélectionné.
		std::string                     m_ASCII_token_separators=" ,;.:\t*-+!/=\"&'()[]%{}<>|`?\\";

		int 					(*_textColoring)(const std::vector<Line__Text_Idx>& lines,
												std::map<int, std::vector<Text_Print_Rules>> &out_rules, Text_Print_Rules default_rules)=nullptr;
	
	
	private: //Attributs de communication interne entre certaines méthodes:
		
		int m_for_removeLine__action_idx=0; //Ceci permet, en interne, de "personnaliser" l'attribut 'action_idx' des actions émises par 'removeLine()'.
		int m_for_replaceSelection_from_eraseSelection__curr_action_idx=0; //Si 'replaceSelection()' fait appel à 'eraseSelection()', ceci permet de lui faire parvenir la dernière valeur de 'curr_action_idx' afin de pouvoir l'exploiter.
		int m_for_addLine__action_idx=0; //Ceci permet, en interne, de "personnaliser" l'attribut 'action_idx' des actions émises par 'addLine()'.
		
	
}; //Text_Window



}//namespace gs
#endif



//Points d'amélioration:
// - la coloration syntaxique sur une plus grande portée que sur les lignes visibles.
// - la sélection avec scrolling si arrivé aux bords.
