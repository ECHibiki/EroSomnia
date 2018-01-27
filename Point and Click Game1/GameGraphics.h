#pragma once
#include "Graphics.h"

#include <fstream>
#include <stdarg.h>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\serialization/vector.hpp>

class GameGraphics {

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {

		ar & MainBG;
		ar & CharacterHair;
		ar & CharacterBody;
		ar & CharacterTorso;
		ar & CharacterRBreast;
		ar & CharacterLBreast;
		ar &CharacterLeg;

		ar & UDArrowL;
		ar & UDArrowR;

		ar & RArrow;

		ar & StatsLogo;
		ar & ToolsLogo;


		ar & BoxOLH;
		ar & BoxOLV;

		ar & BoxOLSel;

		ar & BoxBGH;
		ar & BoxBGV;
		ar & BoxBGSel1;
		ar & BoxBGSel2;
		ar & BoxBGSel3;
		ar & BoxBGSel4;
		ar & BoxBGSel5;

		ar & BoxFullChloro;
		ar & BoxFullIncence;

		ar & BoxMainBGR;
		ar & BoxMainBGL;

		ar & TextBlazer;
		ar & TextBreast;
		ar & TextShirt;
		ar & TextFellatio;
		ar & TextHair;
		ar & TextMasturbation;
		ar & TextMouth;
		ar & TextNipples;
		ar & TextPanties;
		ar & TextSkirt;
		ar & TextThighHigh;
		ar & TextThigh;
		ar & TextTie;
		ar & TextVagina;
		ar & TextXXX;

		ar & TextBG;
		ar & TextWall;
		ar & TextLights;
		ar & black;
		ar & white;
		ar & shadows;

		ar &  bla;
		ar &  tie;
		ar &  tieTop;
		ar &  shirt;
		ar &  skirt;
		ar &  thh;
		ar & pa;

		ar & mois;
		ar & paMois;


		ar & semHair;
		ar & semMo;
		ar & semBre;
		ar & semSto;
		ar & semLeg;

		ar & selTabs;
		ar & selHighlight;
	}

public:
	GameGraphics(Texture* n_args, ...) {

	}
	GameGraphics() {

	}

	void saveGameGraphics() {
		std::fstream file;
		file.open("data/game.rave", std::ios::binary | std::fstream::out);

		boost::archive::binary_oarchive archive(file);
		archive << this;
		file.close();
	}

	static GameGraphics loadGameGraphics() {
		std::fstream file;
		GameGraphics* gg;
		GameGraphics ggCopy;
		file.open("data/game.rave", std::ios::binary | std::fstream::in);
		if (file.is_open()) {
			boost::archive::binary_iarchive archive(file);
			archive >> gg;
			ggCopy = *gg;
		}
		else {
			std::cout << "GAME GRAPHICS DATA NOT FOUND";
			gg = new GameGraphics();
			ggCopy = *gg;
		}
		file.close();

		return ggCopy;
	}

	Texture* MainBG;
	Texture* CharacterBody;
	Texture* CharacterTorso;
	Texture* CharacterHair;
	Texture* CharacterRBreast;
	Texture* CharacterLBreast;
	Texture*CharacterLeg;

	Texture* UDArrowL;
	Texture* UDArrowR;

	Texture* RArrow;

	Texture* StatsLogo;
	Texture* ToolsLogo;

	Texture* BoxOLH;
	Texture* BoxOLV;

	Texture* BoxOLSel;

	Texture* BoxBGH;
	Texture* BoxBGV;
	Texture* BoxBGSel1;
	Texture* BoxBGSel2;
	Texture* BoxBGSel3;
	Texture* BoxBGSel4;
	Texture* BoxBGSel5;

	Texture* BoxFullChloro;
	Texture* BoxFullIncence;

	Texture* BoxMainBGR;
	Texture* BoxMainBGL;

	Texture* TextBlazer;
	Texture* TextBreast;
	Texture* TextShirt;
	Texture* TextFellatio;
	Texture* TextHair;
	Texture* TextMasturbation;
	Texture* TextMouth;
	Texture* TextNipples;
	Texture* TextPanties;
	Texture* TextSkirt;
	Texture* TextThighHigh;
	Texture* TextThigh;
	Texture* TextTie;
	Texture* TextVagina;
	Texture* TextXXX;

	Texture* TextBG;
	Texture* TextLights;
	//Texture* TextBTree;
	Texture* TextWall;
	Texture* black;
	Texture* white;
	Texture* shadows;


	Texture* bla;
	Texture* tie;
	Texture* tieTop;
	Texture* shirt;
	Texture* skirt;
	Texture* pa;
	Texture* thh;

	Texture * mois;
	Texture * paMois;

	Texture* semHair;
	Texture* semMo;
	Texture* semBre;
	Texture* semSto;
	Texture* semLeg;

	Texture* selTabs;
	Texture* selHighlight;

	void clearLibrary() {

		 MainBG->clearTexture();
		 CharacterHair->clearTexture();
		 CharacterBody->clearTexture();
		 CharacterRBreast->clearTexture();
		 CharacterLBreast->clearTexture();
		CharacterLeg->clearTexture();
		CharacterTorso->clearTexture();

		 UDArrowL->clearTexture();
		 UDArrowR->clearTexture();

		 RArrow->clearTexture();

		 StatsLogo->clearTexture();
		 ToolsLogo->clearTexture();

		 BoxOLH->clearTexture();
		 BoxOLV->clearTexture();

		 BoxOLSel->clearTexture();

		 BoxBGH->clearTexture();
		 BoxBGV->clearTexture();
		 BoxBGSel1->clearTexture();
		 BoxBGSel2->clearTexture();
		 BoxBGSel3->clearTexture();
		 BoxBGSel4->clearTexture();
		 BoxBGSel5->clearTexture();

		 BoxFullChloro->clearTexture();
		 BoxFullIncence->clearTexture();

		 BoxMainBGR->clearTexture();
		 BoxMainBGL->clearTexture();

		 TextBlazer->clearTexture();
		 TextBreast->clearTexture();
		 TextShirt->clearTexture();
		 TextFellatio->clearTexture();
		 TextHair->clearTexture();
		 TextMasturbation->clearTexture();
		 TextMouth->clearTexture();
		 TextNipples->clearTexture();
		 TextPanties->clearTexture();
		 TextSkirt->clearTexture();
		 TextThighHigh->clearTexture();
		 TextThigh->clearTexture();
		 TextTie->clearTexture();
		 TextVagina->clearTexture();
		 TextXXX->clearTexture();

		 TextBG->clearTexture();
		 TextLights->clearTexture();
		 TextWall->clearTexture();
		 black->clearTexture();
		 shadows->clearTexture();

		 //TextBTree->clearTexture();
		 white->clearTexture();

		 bla->clearTexture();
		 tie->clearTexture();
		 tieTop->clearTexture();
		shirt->clearTexture();
		skirt->clearTexture();
		 thh->clearTexture();
		 pa->clearTexture();

		 mois->clearTexture();
		 paMois->clearTexture();

		 semHair->clearTexture();
		 semMo->clearTexture();
		 semBre->clearTexture();
		 semSto->clearTexture();
		 semLeg->clearTexture();

		 selTabs->clearTexture();
		 selHighlight->clearTexture();
	}

	void configureLibrary(SDL_Renderer* r, SDL_Window* w) {

		 MainBG->loadFromByteArray(r, w);
		 CharacterHair->loadFromByteArray(r, w);
		 CharacterBody->loadFromByteArray(r, w);
		 CharacterTorso->loadFromByteArray(r, w);
		 CharacterRBreast->loadFromByteArray(r, w);
		 CharacterLBreast->loadFromByteArray(r, w);
		CharacterLeg->loadFromByteArray(r, w);

		 UDArrowL->loadFromByteArray(r, w);
		 UDArrowR->loadFromByteArray(r, w);
		 RArrow->loadFromByteArray(r, w);

		 StatsLogo->loadFromByteArray(r, w);
		 ToolsLogo->loadFromByteArray(r, w);

		 BoxOLH->loadFromByteArray(r, w);
		 BoxOLV->loadFromByteArray(r, w);

		 BoxOLSel->loadFromByteArray(r, w);

		 BoxBGH->loadFromByteArray(r, w);
		 BoxBGV->loadFromByteArray(r, w);
		 BoxBGSel1->loadFromByteArray(r, w);
		 BoxBGSel2->loadFromByteArray(r, w);
		 BoxBGSel3->loadFromByteArray(r, w);
		 BoxBGSel4->loadFromByteArray(r, w);
		 BoxBGSel5->loadFromByteArray(r, w);

		 BoxFullChloro->loadFromByteArray(r, w);
		 BoxFullIncence->loadFromByteArray(r, w);

		 BoxMainBGR->loadFromByteArray(r, w);
		 BoxMainBGL->loadFromByteArray(r, w);

		 TextBlazer->loadFromByteArray(r, w);
		 TextBreast->loadFromByteArray(r, w);
		 TextShirt->loadFromByteArray(r, w);
		 TextFellatio->loadFromByteArray(r, w);
		 TextHair->loadFromByteArray(r, w);
		 TextMasturbation->loadFromByteArray(r, w);
		 TextMouth->loadFromByteArray(r, w);
		 TextNipples->loadFromByteArray(r, w);
		 TextPanties->loadFromByteArray(r, w);
		 TextSkirt->loadFromByteArray(r, w);
		 TextThighHigh->loadFromByteArray(r, w);
		 TextThigh->loadFromByteArray(r, w);
		 TextTie->loadFromByteArray(r, w);
		 TextVagina->loadFromByteArray(r, w);
		 TextXXX->loadFromByteArray(r, w);

		 TextBG->loadFromByteArray(r, w);
		 TextLights->loadFromByteArray(r, w);
		 TextWall->loadFromByteArray(r, w);
		 black->loadFromByteArray(r, w);
		 shadows->loadFromByteArray(r, w);

		// TextBTree->loadFromByteArray(r, w);
		 white->loadFromByteArray(r, w);

		 bla->loadFromByteArray(r, w);
		 tie->loadFromByteArray(r, w);
		 tieTop->loadFromByteArray(r, w);
		 shirt->loadFromByteArray(r, w);
		 skirt->loadFromByteArray(r, w);
		 thh->loadFromByteArray(r, w);
		 pa->loadFromByteArray(r, w);

		 mois->loadFromByteArray(r, w);
		 paMois->loadFromByteArray(r, w);


		 semHair->loadFromByteArray(r, w);
		 semMo->loadFromByteArray(r, w);
		 semBre->loadFromByteArray(r, w);
		 semSto->loadFromByteArray(r, w);
		 semLeg->loadFromByteArray(r, w);

		 selTabs->loadFromByteArray(r, w);
		 selHighlight->loadFromByteArray(r, w);
	}
};
