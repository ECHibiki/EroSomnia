#include <vector>
#include <fstream>
#include <random>
#include <chrono>
#include <Windows.h>
#include <atlstr.h>

#include <cmath>

#include "ScreenIncludes.h"
#include "KeyboardMouseIO.h"
#include "BGM.h"


#include "SaveData.h"
#include "OpeningItems.h".
#include "MenuGraphics.h"
#include "GameGraphics.h"
#include "SFXLibrary.h"
#include "BGMLibrary.h"
#include "AnimationLibrary.h"

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>

#undef main

#define LCLICK Items::inputs->at(LEFTMOUSECLICKED) && !Items::inputs->at(LEFTMOUSEHELD)
#define LHELD  Items::inputs->at(LEFTMOUSEHELD)
#define RCLICK Items::inputs->at(RIGHTMOUSECLICKED) && !Items::inputs->at(RIGHTMOUSEHELD)
#define RHELD  Items::inputs->at(RIGHTMOUSEHELD)

//#define MAINGAMEOFF
//#define DEMO
#define BETA
#define RELEASE
//#define UNLOCK
//#define JUMPTEST
//#define SERIALIZE
#ifdef SERIALIZE
#undef JUMPTEST
#endif

/**NOTES*/
//Make Incence effect point gain.
//Reverse Incence with CHCL3 for reduce gain and bar decrease


#define ENGLISH
//#define JAPANESE
//#define MULTILANG

int ScreenWidth, ScreenHeight;

namespace CardTextures {
	Texture* card0; // used for op
	Texture* card1; // used for unlock 1
	Texture* card2;
	Texture* card3;
	Texture* card4;
}

namespace Items {
	std::vector<bool>* inputs;
#ifdef ENGLISH
	bool engFlag = true;
#endif // ENGLISH
#ifdef SERIALIZE
	bool serialFlag = true;
#endif // SERIALIZE
#ifndef SERIALIZE
	bool serialFlag = false;
#endif

	SaveData userData;
	OpeningItems openingItems;
	MenuGraphics menuGraphics;
	GameGraphics gameGraphics;
	AnimationLibrary animationLibrary;
	BGMLibrary bgmLibrary;
	SFXLibrary sfxLibrary;

	int screen;
	int gameState = -1;

	int gameMode = 0; // 1 is free		2 is reg		3 is timed		4 is hard

	KeyboardMouseIO* mio;
	SDL_Renderer* r1;
	SDL_Window* w1;

	Texture* blockX;
		Texture* msgBox;

	SDL_Rect largeBox;
	std::vector<Texture*>* textureDelivery;
	std::vector<std::string>* textDelivery;
	std::vector<int>* locationsDelivery;
	std::vector<int>* operationsDelivery;

	const int min = 0;
	const int max = 100000;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(min, max);

	std::default_random_engine generatorBinary;
	std::uniform_int_distribution<int> distributionBinary(0, 1);
}

namespace Sound {

	/*SFXLibrary sfxLibrary;*/

	//menu and ui sfx

	SFX* StartSFX;
	SFX* ClickSFX;
	SFX* SelectionSFX;
	SFX* SelectionSFX2;
	SFX* DerobeSFX;
	SFX* RobeSFX;
	SFX* NoSFX;
	SFX* OpenSFX;

	//game sfx

	SFX* IntercomSFX;
	SFX* LightningSFX;
	SFX* PassingSFX;
	SFX* StepsSFX;
	SFX* XingSFX;

	BGM* Track1;
	BGM* Track2;
	BGM* Track3;
	BGM* Track4;
	BGM* TrainBG;
}

namespace Text {
	std::vector<std::string> MenuText;
	std::vector<std::string> OpeningText;
	std::vector<std::string> GameText;
	std::vector<std::string> TipText;
	std::vector<std::string> PromptText;
	std::vector<std::string> PlotText;
	std::vector<std::string> extraText;
	std::vector<std::string> hardPlotText;
	std::vector<std::string>timePlotText;

	std::vector<std::string> badPlotText;
	std::vector<std::string> regPlotText;
	std::vector<std::string> goodPlotText;

	enum NameTitles {
		Fingers = 0,
		Hands = 1,
		Penis = 2,
		Outer = 3,
		Inner = 4,
		Experience = 5,
		Awareness = 6,
		CHCl3 = 7,
		CHCl3Tip = 8,
		Incence = 9,
		IncenceTip = 10,
	};

	enum TipTitles {
		Mo = 0, Ni = 1, Fi = 2, Ha = 3, Th = 4, Br = 5, Ma = 6, Fe = 7, Xx = 8, Bl = 9, To = 10, Sk = 11, Bra = 12, Pa = 13, Thh = 14,
		Ch = 15, In = 16, Scroll = 17
	};

	Words* backWords;
	bool clickBackWords;
}

namespace Prompts {

	int x, y;
	bool promptTab;
	SDL_Color previousColor;
	bool runPrompt(std::vector<Words*> word) {

		SDL_Rect mainBox = { Graphics::prcnt(0.28,'x',Items::w1), Graphics::prcnt(0.3,'y',Items::w1),  Graphics::prcnt(0.5,'x',Items::w1),  Graphics::prcnt(0.3,'y',Items::w1) };
		SDL_Rect OKBox = { Graphics::prcnt(0.28,'x',Items::w1), Graphics::prcnt(0.50,'y',Items::w1),  Graphics::prcnt(0.25,'x',Items::w1),  Graphics::prcnt(0.10,'y',Items::w1) };
		SDL_Rect NoBox = { Graphics::prcnt(0.53,'x',Items::w1), Graphics::prcnt(0.50,'y',Items::w1),  Graphics::prcnt(0.25,'x',Items::w1),  Graphics::prcnt(0.10,'y',Items::w1) };

		SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
		int oldElapsed = 0;
		int startTime = SDL_GetTicks();
		Uint8 r = 0, g = 0, b = 0, a = 0;
		SDL_GetRenderDrawColor(Items::r1, &r, &g, &b, &a);
		previousColor = { r,g,b,a };
		bool promptRunning = true;
		int wordSize = word.size();
		while (promptRunning) {
			int elapsed = SDL_GetTicks() - startTime;
			if (elapsed - oldElapsed > 16) {
				Items::inputs = Items::mio->processKBMInput(&x, &y);
				if (Items::inputs->at(QUIT))		return false;
				if (Items::inputs->at(RIGHTMOUSECLICKED) && !Items::inputs->at(RIGHTMOUSEHELD)) {
					promptTab = false;
					promptRunning = false;
				}

				oldElapsed = elapsed;
				
				Items::msgBox->renderTexture(mainBox);

				SDL_SetRenderDrawColor(Items::r1, 100, 100, 100, 255);
				SDL_RenderDrawRect(Items::r1, &mainBox);
				SDL_RenderDrawRect(Items::r1, &OKBox);
				SDL_RenderDrawRect(Items::r1, &NoBox);
#define MSG 0
#define YES 1
#define NO 2
#define SUB 3
				if (wordSize > SUB) {
					word.at(MSG)->renderTexture(0.30, 0.34);
					word.at(SUB)->renderTexture(0.30, 0.42);
				}
				else word.at(MSG)->renderTexture(0.30, 0.37);

				word.at(YES)->renderTexture(0.33, 0.52);
				word.at(NO)->renderTexture(0.55, 0.52);

				SDL_SetRenderDrawColor(Items::r1, 0, 100, 0, 125);
				if (GenericGameTools::checkRectHitBox(OKBox, x, y)) {
					if (LCLICK) {
						promptTab = true;
						promptRunning = false;
					}
					else {
						SDL_RenderFillRect(Items::r1, &OKBox);
					}
				}
				else if (GenericGameTools::checkRectHitBox(NoBox, x, y)) {
					if (LCLICK) {
						promptTab = false;
						promptRunning = false;
					}
					else {
						SDL_RenderFillRect(Items::r1, &NoBox);
					}
				}
				SDL_SetRenderDrawColor(Items::r1, previousColor.r, previousColor.g, previousColor.b, previousColor.a);
				SDL_RenderPresent(Items::r1);
			}
		}

		for each(Words* line in word) {
			line->clearWords();
		}
		return promptTab;
	}
}

namespace Plot {

	std::vector<Texture*> opTextureSet;
	std::vector<int> opLocations;
	std::vector<int> opOperations;

	std::vector<Texture*>hardTextureSet;
	std::vector<int>hardLocations;
	std::vector<int>hardOperations;

	std::vector<Texture*>timeTextureSet;
	std::vector<int>timeLocations;
	std::vector<int>timeOperations;

	std::vector<Texture*> badTextureSet;
	std::vector<int> badLocations;
	std::vector<int> badOperations;

	std::vector<Texture*> regularTextureSet;
	std::vector<int> regularLocations;
	std::vector<int> regularOperations;

	std::vector<Texture*> goodTextureSet;
	std::vector<int> goodLocations;
	std::vector<int> goodOperations;

	std::vector<Texture*>* textureDelivery;
	std::vector<std::string>* textDelivery;
	std::vector<int>* switchLocationDelivery;
	std::vector<int>* switchOperationDelivery;

	SDL_Rect textBoxText;

	TextBox* textBoxWords;
	KeyboardMouseIO* kbm;

	int clickCounter = 0;
	int currentLocation = 0;

	bool screenRunning = true;

	const int FPS = 30;
	DATATYPE_MILLISECONDS systemTime;

	int alpha;
	int x;
	int y;
	int* screen;
	int elapsed = -1;
	int startTime = -1;
	int tab = 1;
	int yClip = 0;

	long long initial = 0;
	long long expired = 0;

	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

	void decideAction(int loc) {
	}

	void decideSwitch(int loc) {

	}
	int oldElapsed;
	void runGame(std::vector<Texture*>* td, std::vector<std::string>* text, std::vector<int>* sw, std::vector<int>* so) {
		textureDelivery = td;
		textDelivery = text;
		switchLocationDelivery = sw;
		switchOperationDelivery = so;

		clickCounter = 0;
		currentLocation = 0;
		decideAction(switchLocationDelivery->at(currentLocation));
		decideSwitch(switchLocationDelivery->at(currentLocation));

		textBoxText = //{ Graphics::prcnt(-0.01, 'x', Items::w1), Graphics::prcnt(0.73, 'y', Items::w1), Graphics::prcnt(1.01, 'x', Items::w1), Graphics::prcnt(0.28, 'y', Items::w1) };
		{ Graphics::prcnt(0.06, 'x', Items::w1), Graphics::prcnt(0.71, 'y', Items::w1), Graphics::prcnt(0.90, 'x', Items::w1), Graphics::prcnt(0.247, 'y', Items::w1) };
		SDL_Rect marginBoxText = { Graphics::prcnt(0.12, 'x', Items::w1), Graphics::prcnt(0.74, 'y', Items::w1), Graphics::prcnt(0.84, 'x', Items::w1), Graphics::prcnt(0.243, 'y', Items::w1) };
		textBoxWords = new TextBox(	48, 0, textDelivery->at(clickCounter),	SDL_Color{ 245,245,220,255 }, textBoxText, marginBoxText, Items::r1, Items::w1);

		startTime = SDL_GetTicks();
		oldElapsed = 0;
		gameLoop();
	}

	void gameLoop() {

		screenRunning = true;
		initial = 0;
		expired = 0;

		if (switchOperationDelivery->at(0) == 0) {
			Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
			Items::bgmLibrary.Track2->playFadeIn(-1, 500);
		}
		else if (switchOperationDelivery->at(0) == -1) {
			Mix_VolumeMusic(MIX_MAX_VOLUME / 8);
			Items::bgmLibrary.Track3->playFadeIn(-1, 1000);
		}
		else if (switchOperationDelivery->at(0) == 1)
			Items::bgmLibrary.Track4->playFadeIn(-1, 500);
		else {}

		while (screenRunning) {
			systemTime = SYSTEM_MILLISECONDS;
			initial = systemTime.count();

			processInput();
			update();
			play();
			if (screenRunning)
				render();
			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}

		close();
	}

	void processInput() {
		Items::inputs = Items::mio->processKBMInput(&x, &y);
		if (Items::inputs->at(QUIT)) {
			screenRunning = false;
			Items::screen = -1;
		}
	}
	void update() {
		elapsed = SDL_GetTicks() - startTime;
		int delta = elapsed - oldElapsed;
		//std::cout << elapsed << " - " << oldElapsed << " = " << delta << "\n";
		if (delta > 10000 || (LCLICK &&  delta  > 60) || Items::inputs->at(CTRL) || Items::inputs->at(RIGHTMOUSEHELD)) {
			oldElapsed = elapsed;
			clickCounter++;
			textDelivery;
			if (switchLocationDelivery->at(currentLocation) == clickCounter) {
				currentLocation++;
				if (currentLocation == switchLocationDelivery->size()) {					//leave sequence
					--currentLocation;
					--clickCounter;
					screenRunning = false;
					if (switchOperationDelivery->at(0) == 0) {
						if (Prompts::runPrompt({
							new Words(60 ,0,Text::PromptText.at(5), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
							new Words(60,0,Text::PromptText.at(0),SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
							new Words(60 ,0,Text::PromptText.at(1), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) })) {
							Items::screen = 11;
							Items::gameState = switchOperationDelivery->at(0);
							Items::sfxLibrary.startSFX->playSFX();
						}
						else {
							int retOp = switchOperationDelivery->at(0);
							if (retOp == 0) {
								Items::screen = 01;
								Plot::runGame(&regularTextureSet, &Text::regPlotText, &regularLocations, &regularOperations);
								screenRunning = false;
							}
							else {
								std::cout << "err\n";
							}
						}
					}
					else {
						Items::gameState = switchOperationDelivery->at(0);
						Items::screen = 01;
						Items::bgmLibrary.Track1->play();
					}
					Prompts::promptTab = false;
					return;
				}
			}

			textBoxWords->changeBoxWords(textDelivery->at(clickCounter));
		}
	}
	void play() {
		if (screenRunning == false) {
		}
	}
	SDL_Color blueBase = { 39, 74, 183, 199 };
	SDL_Color PurpleTransition = { 100, 84, 190, 255 };
	SDL_Color blueBorder = { 125, 125, 255, 255 };
	void render() {
		SDL_RenderClear(Items::r1);

		textureDelivery->at(currentLocation)->renderTexture();

		textBoxWords->renderBoxWords(blueBase, PurpleTransition, 1.0, 45, 1.0, 0.3, blueBorder);
		//textBoxWords->renderBoxWords(blueBase, blueBorder);
		SDL_SetRenderDrawColor(Items::r1, 255, 255, 255, 255);
		SDL_RenderPresent(Items::r1);
	}

	void close() {
		if(Items::gameState != -1 && Items::gameState != 9)
			Mix_FadeOutMusic(200);
		delete kbm;
		oldElapsed = 0;

		opTextureSet.clear();
		opLocations.clear();
		opOperations.clear();

		hardTextureSet.clear();
		hardLocations.clear();
		hardOperations.clear();

		timeTextureSet.clear();
		timeLocations.clear();
		timeOperations.clear();

		badTextureSet.clear();
		badLocations.clear();
		badOperations.clear();

		regularTextureSet.clear();
		regularLocations.clear();
		regularOperations.clear();

		goodTextureSet.clear();
		goodLocations.clear();
		goodOperations.clear();

		while (Mix_FadingMusic() == MIX_FADING_OUT) {
			Items::inputs = Items::mio->processKBMInput(&x, &y);
			if (LCLICK)
				Mix_HaltMusic();
		}
	}

}

namespace SubMenu {

	void setPlotElements() {
		Plot::opTextureSet.push_back(Items::menuGraphics.bg1);
		Plot::opTextureSet.push_back(Items::menuGraphics.bg2);
		Plot::opTextureSet.push_back(Items::menuGraphics.card0);
		Plot::opOperations.push_back(0);
		Plot::opLocations.push_back(7);
		Plot::opLocations.push_back(17);
		Plot::opLocations.push_back(Text::PlotText.size());

		Plot::hardTextureSet.push_back(Items::menuGraphics.card0);
		Plot::hardOperations = { 0 };
		Plot::hardLocations.push_back(Text::hardPlotText.size());

		Plot::timeTextureSet.push_back(Items::menuGraphics.card0);
		Plot::timeOperations = { 0 };
		Plot::timeLocations.push_back(Text::timePlotText.size());

		Plot::badTextureSet.push_back(Items::menuGraphics.bg3);
		Plot::badTextureSet.push_back(Items::menuGraphics.bg4);
		Plot::badLocations.push_back(3);
		Plot::badLocations.push_back(Text::badPlotText.size());
		Plot::badOperations.push_back(-1);

		Plot::regularTextureSet.push_back(Items::menuGraphics.bg3);
		Plot::regularTextureSet.push_back(Items::menuGraphics.bg5);
		Plot::regularLocations.push_back(3);
		Plot::regularLocations.push_back(Text::regPlotText.size());
		Plot::regularOperations.push_back(9);

#ifdef BETA
		Plot::goodTextureSet.push_back(Items::menuGraphics.bg3);
		Plot::goodOperations = { -1 };
		Plot::goodLocations = { (int)Text::goodPlotText.size() };
#endif
#ifndef BETA
		Plot::goodTextureSet.push_back(Items::menuGraphics.bg3);
		Plot::goodTextureSet.push_back(Items::menuGraphics.bg6);
		Plot::goodOperations = { -1 };
		Plot::goodLocations = { 3, (int)Text::goodPlotText.size() };
#endif

	}

	SDL_Rect tipBoxO;
	SDL_Rect tipBoxI;

	Texture* subBG;
	Texture* subMenuIconR;
	Texture* subMenuIconL;

	Words *freeWords, *regularWords, *timeWords, *hardWords,
		*freeTip, *regularTip, *timeTip, *hardTip;
	Words** currentWords;
	bool hover;

	bool clickReg, clickTime, clickHard, clickFree,
		hoverReg, hoverTime, hoverHard, hoverFree;
	bool click;

	bool screenRunning = true;

	const int FPS = 30;
	DATATYPE_MILLISECONDS systemTime;

	int alpha;
	int x;
	int y;
	int* screen;
	int elapsed = -1;
	int tab = 1;
	int yClip = 0;

	long long initial = 0;
	long long expired = 0;

	void runGame();
	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

	void runGame() {
		Text::backWords->changeTextFont(72, 0);
		tipBoxO = {
			Graphics::prcnt(0.5, 'x', Items::w1), 0,
			Graphics::prcnt(0.5, 'x', Items::w1), Graphics::prcnt(0.5, 'y', Items::w1)
		};
		tipBoxI = {
			Graphics::prcnt(0.51, 'x', Items::w1), Graphics::prcnt(0.01, 'y', Items::w1),
			Graphics::prcnt(0.5, 'x', Items::w1), Graphics::prcnt(0.48, 'y', Items::w1)
		};
		Text::backWords->setLocation(0.05, 0.45);
		GenericGameTools::reset();


		if (Items::userData.gate2 == 1095651 && Items::userData.gate1 == 95631 && Items::userData.gate0 == -9384) {
			freeWords->changeTextColor(SDL_Color{ 0,0,0,255 });
			regularWords->changeTextColor(SDL_Color{ 0,0,0,255 });
			timeWords->changeTextColor(SDL_Color{ 0,0,0,255 });
			hardWords->changeTextColor(SDL_Color{ 0,0,0,255 });
		}
		else if (Items::userData.gate2 == 1095651 && Items::userData.gate1 == 95631) {
			freeWords->changeTextColor(SDL_Color{ 0,0,0,255 });
			regularWords->changeTextColor(SDL_Color{ 0,0,0,255 });
			timeWords->changeTextColor(SDL_Color{ 0,0,0,255 });
			hardWords->changeTextColor(SDL_Color{ 125,125,125,255 });

		}
		else if (Items::userData.gate0 == -9384) {
			freeWords->changeTextColor(SDL_Color{ 125,125,125,255 });
			regularWords->changeTextColor(SDL_Color{ 0,0,0,255 });
			timeWords->changeTextColor(SDL_Color{ 125,125,125,255 });
			hardWords->changeTextColor(SDL_Color{ 125,125,125,255 });
		}

		gameLoop();
	}

	void gameLoop() {

		screenRunning = true;
		initial = 0;
		expired = 0;

		while (screenRunning) {
			systemTime = SYSTEM_MILLISECONDS;
			initial = systemTime.count();

			processInput();
			update();
			play();
			render();
			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}

		close();
	}

	void processInput() {
		Items::inputs = Items::mio->processKBMInput(&x, &y);
		if (Items::inputs->at(QUIT)) {
			screenRunning = false;
			Items::screen = -1;
		}
	}
	void update() {
		hover = hoverReg = hoverTime = hoverHard = hoverFree = false;
#ifndef DEMO
		//-9384
		//20276
		//1095651
		//095631
		if (GenericGameTools::checkRectHitBox(hardWords->box, x, y) && Items::userData.gate2 == 1095651) {
			currentWords = &hardWords;
			hover = hoverHard = true;
			if (LCLICK) {
				setPlotElements();
				Items::textureDelivery = &Plot::hardTextureSet;
				Items::textDelivery = &Text::hardPlotText;
				Items::locationsDelivery = &Plot::hardLocations;
				Items::operationsDelivery = &Plot::hardOperations;
				clickHard = true;
				click = true;
			}
		}
		else if (GenericGameTools::checkRectHitBox(freeWords->box, x, y) && Items::userData.gate1 == 95631) {
			currentWords = &freeWords;
			hover = hoverFree = true;
			if (LCLICK) {
				clickFree = true;
				click = true;
			}
		}
		else if (GenericGameTools::checkRectHitBox(timeWords->box, x, y) && Items::userData.gate1 == 95631) {
			currentWords = &timeWords;
			hover = hoverTime = true;
			if (LCLICK) {
				setPlotElements();
				Items::textureDelivery = &Plot::timeTextureSet;
				Items::textDelivery = &Text::timePlotText;
				Items::locationsDelivery = &Plot::timeLocations;
				Items::operationsDelivery = &Plot::timeOperations;
				clickTime = true;
				click = true;
			}
		}
#endif
		/////////////////////////////////////////////////////////////////
		if (GenericGameTools::checkRectHitBox(regularWords->box, x, y) && Items::userData.gate0 == -9384) {
			currentWords = &regularWords;
			hover = hoverReg = true;
			if (LCLICK) {
				setPlotElements();
				Items::textureDelivery = &Plot::opTextureSet;
				Items::textDelivery = &Text::PlotText;
				Items::locationsDelivery = &Plot::opLocations;
				Items::operationsDelivery = &Plot::opOperations;
				clickReg = true;
				click = true;
			}
		}
		else if (GenericGameTools::checkRectHitBox(Text::backWords->box, x, y)) {
			currentWords = &Text::backWords;
			hover = true;
			if (LCLICK) {
				Text::clickBackWords = true;
				click = true;
			}
		}
	}
	void play() {
		if (click) {
			Items::sfxLibrary.ClickSFX->playSFX();
			click = false;
			if (clickReg) {
				Mix_FadeOutMusic(500);
			}
			else if (clickFree || clickHard || clickTime) {
				Mix_FadeOutMusic(400);
			}				
		}
	}


	void render() {
		SDL_RenderClear(Items::r1);

		Items::menuGraphics.subBG->renderTexture();


		Items::menuGraphics.subMenuIconR->renderTexture();
		//subMenuIconL->renderTexture();
		if (hover) {
			SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
			SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
			SDL_Rect smallBox = SDL_Rect{
				(**currentWords).box.x - Graphics::prcnt(0.025, 'x', Items::w1),
				(**currentWords).box.y - Graphics::prcnt(0.025, 'y', Items::w1),
				(**currentWords).box.w + Graphics::prcnt(0.05, 'x', Items::w1),
				(**currentWords).box.h + Graphics::prcnt(0.05, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &smallBox);
			if (hoverFree) {
				freeTip->renderTexture();
			}
			else if (hoverReg) {
				regularTip->renderTexture();
			}
			else if (hoverTime) {
				timeTip->renderTexture();
			}
			else if (hoverHard) {
				hardTip->renderTexture();
			}
		}


		freeWords->renderTexture();
		regularWords->renderTexture();
		timeWords->renderTexture();
		hardWords->renderTexture();
		Text::backWords->renderTexture();
#ifdef DEMO
		Items::blockX->renderTexture(freeWords->box);
		Items::blockX->renderTexture(timeWords->box);
		Items::blockX->renderTexture(hardWords->box);
#endif // DEMO

		SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 255);
		//SDL_RenderFillRect(Items::r1, &tipBoxO);
		//SDL_SetRenderDrawColor(Items::r1, 255, 255, 255, 255);
		//SDL_RenderFillRect(Items::r1, &tipBoxI);

		if (clickFree) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 500);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_RenderFillRect(Items::r1, &Items::largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = 11;
				Items::gameMode = 1;
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (clickReg) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 500);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_RenderFillRect(Items::r1, &Items::largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = 10;
				Items::gameMode = 2;
			}
		}
		else if (clickTime) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 500);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_RenderFillRect(Items::r1, &Items::largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = 10;
				Items::gameMode = 3;
			}
		}
		else if (clickHard) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 500);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_RenderFillRect(Items::r1, &Items::largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = 10;
				Items::gameMode = 4;
			}
		}
		else if (Text::clickBackWords) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 500);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_RenderFillRect(Items::r1, &Items::largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = 01;
			}
		}

		SDL_SetRenderDrawColor(Items::r1, 255, 255, 255, 225);
		SDL_RenderPresent(Items::r1);
	}

	void close() {
		clickFree = false;
		clickHard = false;
		clickReg = false;
		clickTime = false;
		Text::clickBackWords = false;
		alpha = 0;
	}
}

namespace ImageView {

	Words** backWords;

	bool screenRunning = true, hover;

	const int FPS = 30;
	DATATYPE_MILLISECONDS systemTime;

	int alpha;
	int x;
	int y;
	int* screen;
	int elapsed = -1;
	int tab = 1;
	int yClip = 0;

	long long initial = 0;
	long long expired = 0;

	void runView(Texture** img) {
		screenRunning = true;
		initial = 0;
		expired = 0;

		SDL_Rect backBox = { Graphics::prcnt(0.825, 'x', Items::w1), Graphics::prcnt(0.85, 'y', Items::w1)
			, Graphics::prcnt(0.2, 'x', Items::w1)  , Graphics::prcnt(0.2, 'y', Items::w1) };

		while (screenRunning) {
			systemTime = SYSTEM_MILLISECONDS;
			initial = systemTime.count();

			Items::inputs = Items::mio->processKBMInput(&x, &y);
			if (Items::inputs->at(QUIT)) {
				screenRunning = false;
				Items::screen = -1;
			}
			SDL_RenderClear(Items::r1);

			(**img).renderTexture();
			(**backWords).renderTexture(0.85, 0.9);

			if (GenericGameTools::checkRectHitBox(backBox, x, y)) {
				if (LCLICK) {
					screenRunning = false;
				}
				SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
				SDL_Rect box = {
					(backBox).x - Graphics::prcnt(0.005, 'x', Items::w1),
					(backBox).y - Graphics::prcnt(0.005, 'y', Items::w1),
					(backBox).w + Graphics::prcnt(0.01, 'x', Items::w1),
					(backBox).h + Graphics::prcnt(0.01, 'y', Items::w1)
				};
				SDL_RenderFillRect(Items::r1, &box);
				SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 225);
			}

			SDL_RenderPresent(Items::r1);

			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}
	}
}

namespace ExtraMenu {

	Texture* ExtraBG, *ExtraBGHolder;
	Texture* containerTextureEC;
	Texture* containerTextureSH;

	Texture* pendulumBase, *pendulumRod, *pendulumBase2, *pendulumRod2;
	double rodVel = 0, rodAcc = 0.3, rodAngle = 20;

	Words *nameWords, *statusWords, *link1Words, *link2Words, *backWords, *nextWords;
	Words** currentWords;
	std::vector<Words*> trackWords, imageWords;
	std::vector<Words*>* currentWordVect;
	SDL_Rect container;
	bool hover, cardView, click;

	bool ecView = true;
	bool screenRunning = true;

	const int FPS = 30;
	DATATYPE_MILLISECONDS systemTime;

	int alpha;
	int x;
	int y;
	int* screen;
	int elapsed = -1;
	int tab = 1;
	int yClip = 0;

	long long initial = 0;
	long long expired = 0;

	void runGame();
	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

	void runGame() {
		ecView = true;
		ImageView::backWords = &backWords;
		backWords->changeTextFont(60, 0);
		if (ecView) {
			currentWordVect = &trackWords;
			nameWords->changeText(Text::extraText.at(0));
			statusWords->changeText(Text::extraText.at(1));
			link1Words->changeText(Text::extraText.at(5));
		}
		container = { Graphics::prcnt(0.02, 'x', Items::w1),
			Graphics::prcnt(0.18, 'y', Items::w1),
			Graphics::prcnt(0.75, 'x', Items::w1),
			Graphics::prcnt(0.675, 'y', Items::w1) };
		backWords->setLocation(0.83, 0.1);
		ecView = true;
		gameLoop();
	}

	void gameLoop() {

		screenRunning = true;
		initial = 0;
		expired = 0;

		while (screenRunning) {
			systemTime = SYSTEM_MILLISECONDS;
			initial = systemTime.count();

			processInput();
			update();
			play();
			render();

			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}

		close();
	}

	void processInput() {
		Items::inputs = Items::mio->processKBMInput(&x, &y);
		if (Items::inputs->at(QUIT)) {
			screenRunning = false;
			Items::screen = -1;
		}
	}

	void update() {
		//#define encrypt
#define ZERO 2
		hover = false;
		if (GenericGameTools::checkRectHitBox(nextWords->box, x, y)) {
			hover = true;
			currentWords = &nextWords;
			if (LCLICK) {
				click = true;
				ecView = !ecView;
				if (!ecView) {
					currentWordVect = &imageWords;
					nameWords->changeText(Text::extraText.at(7));
					statusWords->changeText(Text::extraText.at(8));
					link1Words->changeText(Text::extraText.at(13));
				}
				else {
					currentWordVect = &trackWords;
					nameWords->changeText(Text::extraText.at(0));
					statusWords->changeText(Text::extraText.at(1));
					link1Words->changeText(Text::extraText.at(5));
				}
			}
		}
		else if (GenericGameTools::checkRectHitBox(backWords->box, x, y)) {
			hover = true;
			currentWords = &backWords;
			if (LCLICK) {
				click = true;
				screenRunning = false;
				Items::screen = 01;
			}
		}
		else if (GenericGameTools::checkRectHitBox(link1Words->box, x, y)) {
			hover = true;
			currentWords = &link1Words;
			if (LCLICK) {
				click = true;
				if (ecView) {
#ifdef encrypt
					float chara[34];
					std::fstream encryption;

					encryption.open("link.txt");
					char link[34] = "https://www.patreon.com/EroCorvid";
					int i = 0;
					std::cout << "\nAAA: " << link << "\n";
					for each(char character in link) {
						if (i != 34)
							chara[i] = ((float)(character + 4)) / 2;
						i++;
					}
					for each(float n in chara)
						encryption << n << ", ";
					encryption.close();
#endif
#ifndef encrypt
					float chara[34] = { 54, 60, 60, 58, 59.5, 31, 25.5, 25.5, 61.5, 61.5, 61.5, 25, 58, 50.5, 60, 59, 52.5, 57.5, 57, 25, 51.5, 57.5, 56.5, 25.5, 36.5, 59, 57.5, 35.5, 57.5, 59, 61, 54.5, 52, ZERO};
					char link[34];
					int i = 0;
#endif // !encrypt	
					for each(char character in link) {
						link[i] = (chara[i] * 2) - 4;
						i++;
					}

					STARTUPINFOA si;
					PROCESS_INFORMATION pi;

					ZeroMemory(&si, sizeof(si));
					si.cb = sizeof(si);
					ZeroMemory(&pi, sizeof(pi));

					CStringA command_line;
					command_line.Format("cmd.exe /c start \"link\" \"%s\"", link);

					CreateProcessA(NULL,     // No module name (use command line)
						command_line.GetBuffer(),
						NULL,           // Process handle not inheritable
						NULL,           // Thread handle not inhberitable
						FALSE,          // Set handle inheritance to FALSE
						NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,              // No creation flags
						NULL,           // Use parent's environment block
						NULL,           // Use parent's starting directory
						&si,            // Pointer to STARTUPINFO structure
						&pi);           // Pointer to PROCESS_INFORMATION structure
				}
				else {
#ifdef encrypt
					float chara[35];
					std::fstream encryption;

					encryption.open("link.txt");
					char link[35] = "https://www.patreon.com/SunsetNiva";
					int i = 0;
					std::cout << "\nAAA: " << link << "\n";
					for each(char character in link) {
						if (i != 34)
							chara[i] = ((float)(character + 4)) / 2;
						i++;
					}
					for each(float n in chara)
						encryption << n << ", ";
					encryption.close();
#endif
#ifndef encrypt
					float chara[35] = { 54, 60, 60, 58, 59.5, 31, 25.5, 25.5, 61.5, 61.5, 61.5, 25, 58, 50.5, 60, 59, 52.5, 57.5, 57, 25,
						51.5, 57.5, 56.5, 25.5, 43.5, 60.5, 57, 59.5, 52.5, 60, 41, 54.5, 61, 50.5,ZERO };
					char link[35];
					int i = 0;
#endif // !encrypt	
					for each(char character in link) {
						link[i] = (chara[i] * 2) - 4;
						i++;
					}

					STARTUPINFOA si;
					PROCESS_INFORMATION pi;

					ZeroMemory(&si, sizeof(si));
					si.cb = sizeof(si);
					ZeroMemory(&pi, sizeof(pi));

					CStringA command_line;
					command_line.Format("cmd.exe /c start \"link\" \"%s\"", link);

					CreateProcessA(NULL,     // No module name (use command line)
						command_line.GetBuffer(),
						NULL,           // Process handle not inheritable
						NULL,           // Thread handle not inhberitable
						FALSE,          // Set handle inheritance to FALSE
						NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,              // No creation flags
						NULL,           // Use parent's environment block
						NULL,           // Use parent's starting directory
						&si,            // Pointer to STARTUPINFO structure
						&pi);           // Pointer to PROCESS_INFORMATION structure
				}
			}

		}
		else if (GenericGameTools::checkRectHitBox(link2Words->box, x, y)) {
			hover = true;
			currentWords = &link2Words;
			if (LCLICK) {
				click = true;
				if (ecView) {
#ifdef encrypt
					float chara[30];
					std::fstream encryption;

					encryption.open("link.txt");
					char link[30] = "http://erocorvid.blogspot.ca/";
					int i = 0;
					std::cout << "\nAAA: " << link << "\n";
					for each(char character in link) {
						if (i != 29)
							chara[i] = ((float)(character + 4)) / 2;
						i++;
					}
					for each(float n in chara)
						encryption << n;
					encryption.close();
#endif
#ifndef encrypt
					float chara[30] = { 54,60,60,58,31,25.5,25.5,52.5,59,57.5,51.5,57.5,59,61,54.5,52,25,51,56,57.5,53.5,59.5,58,57.5,60,25,51.5,50.5,25.5,ZERO };
					char link[30];
					int i = 0;
#endif // !encrypt	
					for each(char character in link) {
						link[i] = (chara[i] * 2) - 4;
						i++;
					}

					STARTUPINFOA si;
					PROCESS_INFORMATION pi;

					ZeroMemory(&si, sizeof(si));
					si.cb = sizeof(si);
					ZeroMemory(&pi, sizeof(pi));

					CStringA command_line;
					command_line.Format("cmd.exe /c start \"link\" \"%s\"", link);

					CreateProcessA(NULL,     // No module name (use command line)
						command_line.GetBuffer(),
						NULL,           // Process handle not inheritable
						NULL,           // Thread handle not inhberitable
						FALSE,          // Set handle inheritance to FALSE
						NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,              // No creation flags
						NULL,           // Use parent's environment block
						NULL,           // Use parent's starting directory
						&si,            // Pointer to STARTUPINFO structure
						&pi);           // Pointer to PROCESS_INFORMATION structure
				}
				else {
#ifdef encrypt
					float chara[22];
					std::fstream encryption;

					encryption.open("link.txt");
					char link[22] = "http://sunsetniva.com";
					int i = 0;
					std::cout << "\nAAA: " << link << "\n";
					for each(char character in link) {
						if (i != 20)
							chara[i] = ((float)(character + 4)) / 2;
						i++;
					}
					for each(float n in chara)
						encryption << n << ", ";
					encryption.close();
#endif
#ifndef encrypt
					float chara[22] = { 54, 60, 60, 58, 31, 25.5, 25.5, 59.5, 60.5, 57, 59.5, 52.5, 60, 57, 54.5, 61, 50.5, 25, 51.5, 57.5, 56.5, ZERO };
					char link[22];
					int i = 0;
#endif // !encrypt	
					for each(char character in link) {
						link[i] = (chara[i] * 2) - 4;
						i++;
					}
					STARTUPINFOA si;
					PROCESS_INFORMATION pi;

					ZeroMemory(&si, sizeof(si));
					si.cb = sizeof(si);
					ZeroMemory(&pi, sizeof(pi));

					CStringA command_line;
					command_line.Format("cmd.exe /c start \"link\" \"%s\"", link);

					CreateProcessA(NULL,     // No module name (use command line)
						command_line.GetBuffer(),
						NULL,           // Process handle not inheritable
						NULL,           // Thread handle not inhberitable
						FALSE,          // Set handle inheritance to FALSE
						NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,              // No creation flags
						NULL,           // Use parent's environment block
						NULL,           // Use parent's starting directory
						&si,            // Pointer to STARTUPINFO structure
						&pi);           // Pointer to PROCESS_INFORMATION structure
				}
			}
		}
		else {

#ifdef DEMO
			if (GenericGameTools::checkRectHitBox(currentWordVect->at(0)->box, x, y)) {
				hover = true;
				currentWords = &currentWordVect->at(1 - 1);
				if (LCLICK) {
					if (ecView) {

					}
					else {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 225);
						//		backWords->changeTextColor(SDL_Color{ 255,255,255,255 });
						ImageView::runView(&CardTextures::card1);
						//		backWords->changeTextColor(SDL_Color{ 0,0,0,255 });
					}
				}
			}
#endif
#ifndef  DEMO
			int i = 0;
			for each(Words* w in *currentWordVect) {
				i++;
				if (GenericGameTools::checkRectHitBox(w->box, x, y)) {
					if (ecView) {
						switch (i) {
						case 1:
							if (Items::userData.gate0 == -9384) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						case 2:
							if (Items::userData.gate0 == -9384) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						case 3:
							if (Items::userData.gate1 == 95631) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						case 4:
							if (Items::userData.gate4 == 991122) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						}
						if (LCLICK) {
							switch (i) {
							case 1:
								if (Items::userData.gate0 == -9384) {
									Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
									Items::bgmLibrary.Track1->play();
									click = true;
								}
								break;
							case 2:
								if (Items::userData.gate0 == -9384) {
									Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
									Items::bgmLibrary.Track2->play();
									click = true;
								}
								break;
							case 3:
								if (Items::userData.gate1 == 95631) {
									Items::bgmLibrary.Track3->play();
									click = true;
								}
								break;
							case 4:
								if (Items::userData.gate4 == 991122) {
									Items::bgmLibrary.Track1->play();
									click = true;
								}
								break;
							}
						}
						break;
					}
					else {
						switch (i) {
						case 1:
							if (Items::userData.gate0 == -9384) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						case 2:
							if (Items::userData.gate1 == 95631) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						case 3:
							if (Items::userData.gate2 == 1095651) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						case 4:
							if (Items::userData.gate3 == 20276) {
								currentWords = &currentWordVect->at(i - 1);
								hover = true;
							}
							break;
						}

						if (LCLICK) {
							switch (i) {
							case 1:
								if (Items::userData.gate0 == -9384) {
									SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 225);
									//		backWords->changeTextColor(SDL_Color{ 255,255,255,255 });
									ImageView::runView(&Items::menuGraphics.card1);
									//		backWords->changeTextColor(SDL_Color{ 0,0,0,255 });
									click = true;
								}
								break;
							case 2:
								if (Items::userData.gate1 == 95631) {
									SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 225);
									//		backWords->changeTextColor(SDL_Color{ 255,255,255,255 });
									ImageView::runView(&Items::menuGraphics.card2);
									//		backWords->changeTextColor(SDL_Color{ 0,0,0,255 });
									click = true;
								}
								break;
							case 3:
								if (Items::userData.gate2 == 1095651) {
									SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 225);
									//		backWords->changeTextColor(SDL_Color{ 255,255,255,255 });
									ImageView::runView(&Items::menuGraphics.card3);
									//		backWords->changeTextColor(SDL_Color{ 0,0,0,255 });
									click = true;
								}
								break;
							case 4:
								if (Items::userData.gate3 == 20276) {
									SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 225);
									//		backWords->changeTextColor(SDL_Color{ 255,255,255,255 });
									ImageView::runView(&Items::menuGraphics.card4);
									//		backWords->changeTextColor(SDL_Color{ 0,0,0,255 });
									click = true;
								}
								break;
							}
						}
					}
				}
			}
#endif // ! DEMO
		}
	}
	void play() {
		if (click) {
			Items::sfxLibrary.ClickSFX->playSFX();
			click = false;
		}
	}

	void render() {
		SDL_RenderClear(Items::r1);

		Items::menuGraphics.ExtraBG->renderTexture();

		if (rodAngle < 0) rodVel += rodAcc;
		else if (rodAngle >= 0) rodVel -= rodAcc;
		rodAngle += rodVel;

		if (ecView) {
			//containerTextureEC->renderTexture();
			Items::menuGraphics.pendulumRod->renderTextureRotate(rodAngle);
			Items::menuGraphics.pendulumBase->renderTexture();
		}

		else {
			//containerTextureSH->renderTexture();
			Items::menuGraphics.pendulumRod2->renderTextureRotate(rodAngle);
			Items::menuGraphics.pendulumBase2->renderTexture();
		}

		Items::menuGraphics.ExtraBGHolder->renderTexture();

		if (hover) {
			SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
			SDL_Rect box = {
				(**currentWords).box.x - Graphics::prcnt(0.005, 'x', Items::w1),
				(**currentWords).box.y - Graphics::prcnt(0.005, 'y', Items::w1),
				(**currentWords).box.w + Graphics::prcnt(0.01, 'x', Items::w1),
				(**currentWords).box.h + Graphics::prcnt(0.01, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &box);
		}

		nameWords->renderTexture();
		statusWords->renderTexture();
		nextWords->renderTexture();
		backWords->renderTexture();

		int i = 0;
		SDL_Color grey = { 125,125,125,255 };
		for each(Words* w in *currentWordVect) {
			i++;
			if (ecView) {
				switch (i) {
				case 1:
					if (Items::userData.gate0 == -9384) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				case 2:
					if (Items::userData.gate0 == -9384) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				case 3:
					if (Items::userData.gate1 == 95631) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				case 4:
					if (Items::userData.gate4 == 991122) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				}
			}
			else {
				switch (i) {
				case 1:
					if (Items::userData.gate0 == -9384) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				case 2:
					if (Items::userData.gate1 == 95631) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				case 3:
					if (Items::userData.gate2 == 1095651) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				case 4:
					if (Items::userData.gate3 == 20276) {
						w->renderTexture();
					}
					else {
						w->renderWords(grey);
					}
					break;
				}
#ifdef DEMO
				if (i != 1)
					Items::blockX->renderTexture(w->box);
#endif
			}
		}


		link1Words->renderTexture();
		link2Words->renderTexture();

		//SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 255);
		//SDL_RenderDrawRect(Items::r1, &container);
		SDL_SetRenderDrawColor(Items::r1, 255, 255, 255, 255);

		SDL_RenderPresent(Items::r1);
	}
	void close() {

	}
}

/*
PROCESS_INFORMATION pi;
STARTUPINFO si;
CreateProcess("c:\\Program Files\\Internet Explorer\\iexplore.exe",
"iexplore.exe www.yahoo.com", NULL, NULL, FALSE,
CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


STARTUPINFOA si;
PROCESS_INFORMATION pi;

ZeroMemory(&si, sizeof(si));
si.cb = sizeof(si);
ZeroMemory(&pi, sizeof(pi));

CStringA command_line;
command_line.Format("cmd.exe /c start \"link\" \"%s\"", "http://www.google.com");

if (!CreateProcessA(NULL,     // No module name (use command line)
command_line.GetBuffer(),
NULL,           // Process handle not inheritable
NULL,           // Thread handle not inhberitable
FALSE,          // Set handle inheritance to FALSE
NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,              // No creation flags
NULL,           // Use parent's environment block
NULL,           // Use parent's starting directory
&si,            // Pointer to STARTUPINFO structure
&pi)           // Pointer to PROCESS_INFORMATION structure
)
{
TRACE("CreateProcess failed (%d).\n", GetLastError());
return;
}

*/

namespace Menu {

	bool screenRunning = true;

	bool playGenericClick;
	bool playGenericHover;
	bool playHoverDone;

	bool hoverPlay, clickPlay
		, hoverExtra, clickExtra
		, hoverQuit, clickQuit;

	const int FPS = 60;
	DATATYPE_MILLISECONDS systemTime;

	double roation = 0;

	int alpha;
	int x;
	int y;
	int* screen;
	int elapsed = -1;
	int tab = 1;
	int yClip = 0;

	long long initial = 0;
	long long expired = 0;

	Texture* MenuBG, *Title, *Z1, *Z2, *Z3;

	Words* PlayWords, *ExtraWords, *QuitWords, *versWords;

	void runGame();
	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

	void runGame() {
		alpha = 0;
		GenericGameTools::reset();
		gameLoop();
	}

	void gameLoop() {

		screenRunning = true;
		initial = 0;
		expired = 0;
		versWords = new Words(32, 0, "EroSomnia - TMP: Version 0.2.1", 0.02, 0.95, Items::r1, Items::w1);
		while (screenRunning) {
			systemTime = SYSTEM_MILLISECONDS;
			initial = systemTime.count();

			processInput();
			update();
			play();
			render();

			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}

		close();
	}

	void processInput() {
		Items::inputs = Items::mio->processKBMInput(&x, &y);
	}
	void update() {
		if (Items::inputs->at(QUIT)) {
			screenRunning = false;
			Items::screen = -1;
		}
		hoverPlay = false;
		hoverExtra = false;
		hoverQuit = false;

		if (GenericGameTools::checkRectHitBox(PlayWords->box, x, y)) {
			hoverPlay = true;
			if (LCLICK && !clickPlay) {
				GenericGameTools::reset();
				clickPlay = true;
			}
		}
		if (GenericGameTools::checkRectHitBox(ExtraWords->box, x, y)) {
			hoverExtra = true;
			if (LCLICK && !clickPlay) {
				GenericGameTools::reset();
				clickExtra = true;
			}
		}
		if (GenericGameTools::checkRectHitBox(QuitWords->box, x, y)) {
			hoverQuit = true;
			if (LCLICK && !clickPlay) {
				GenericGameTools::reset();
				clickQuit = true;
			}
		}
	}

	void play() {
		playGenericHover = false;
		playHoverDone = true;

		if (LCLICK && (clickPlay || clickExtra || clickQuit) && !playGenericClick) {
			Items::sfxLibrary.ClickSFX->playSFX();
			playGenericClick = true;
		}
	}
	double z1Rotate, z2Rotate, z3Rotate, tRotate;
	double z1Vel = 0.5, z2Vel = 0.25, z3Vel = 1, tVel = 0.1;
	double z1Acc = 0.01, z2Acc = 0.01, z3Acc = 0.01, tAcc = 0.001;
	bool rR1, rR2, rR3;
	void render() {

		SDL_RenderClear(Items::r1);

		if (z1Rotate < 0) {
			rR1 = true;
			z1Vel += z1Acc;
		}
		else if (z1Rotate >= 0) {
			rR1 = false;
			z1Vel -= z1Acc;
		}
		z1Rotate += z1Vel;

		if (z2Rotate < 0) {
			rR1 = true;
			z2Vel += z2Acc;
		}
		else if (z2Rotate >= 0) {
			rR1 = false;
			z2Vel -= z2Acc;
		}
		z2Rotate += z2Vel;

		if (z3Rotate < 0) {
			rR3 = true;
			z3Vel += z3Acc;
		}
		else if (z3Rotate >= 0) {
			rR3 = false;
			z3Vel -= z3Acc;
		}
		z3Rotate += z3Vel;

		if (tRotate < 0) {
			tVel += tAcc;
		}
		else if (tRotate >= 0) {
			tVel -= tAcc;
		}
		tRotate += tVel;

		Items::menuGraphics.MenuBG->renderTexture();
		Items::menuGraphics.Title->renderTextureRotate(tRotate);
		Items::menuGraphics.Z1->renderTextureRotate(z1Rotate);
		Items::menuGraphics.Z2->renderTextureRotate(z2Rotate);
		Items::menuGraphics.Z3->renderTextureRotate(z3Rotate);

		if (hoverPlay) {
			//Make it ambiguous through pointers
			SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
			SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
			SDL_Rect smallBox = SDL_Rect{
				PlayWords->box.x - Graphics::prcnt(0.05, 'x', Items::w1),
				PlayWords->box.y - Graphics::prcnt(0.05, 'y', Items::w1),
				PlayWords->box.w + Graphics::prcnt(0.1, 'x', Items::w1),
				PlayWords->box.h + Graphics::prcnt(0.1, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &smallBox);
		}
		if (hoverExtra) {
			//Make it ambiguous through pointers
			SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
			SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
			SDL_Rect smallBox = SDL_Rect{
				ExtraWords->box.x - Graphics::prcnt(0.05, 'x', Items::w1),
				ExtraWords->box.y - Graphics::prcnt(0.05, 'y', Items::w1),
				ExtraWords->box.w + Graphics::prcnt(0.1, 'x', Items::w1),
				ExtraWords->box.h + Graphics::prcnt(0.1, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &smallBox);
		}
		if (hoverQuit) {
			//Make it ambiguous through pointers
			SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
			SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
			SDL_Rect smallBox = SDL_Rect{
				QuitWords->box.x - Graphics::prcnt(0.05, 'x', Items::w1),
				QuitWords->box.y - Graphics::prcnt(0.05, 'y', Items::w1),
				QuitWords->box.w + Graphics::prcnt(0.1, 'x', Items::w1),
				QuitWords->box.h + Graphics::prcnt(0.1, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &smallBox);
		}

		//PlayWords->renderOutlineAndWords();
		PlayWords->renderOutlineAndWords();
		ExtraWords->renderTexture();
		QuitWords->renderTexture();

		if (clickPlay) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 500);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_Rect largeBox = SDL_Rect{
				Graphics::prcnt(0, 'x', Items::w1),
				Graphics::prcnt(0, 'y', Items::w1),
				Graphics::prcnt(1, 'x', Items::w1),
				Graphics::prcnt(1, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = 02;
			}
		}
		else if (clickExtra) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 200);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_Rect largeBox = SDL_Rect{
				Graphics::prcnt(0, 'x', Items::w1),
				Graphics::prcnt(0, 'y', Items::w1),
				Graphics::prcnt(1, 'x', Items::w1),
				Graphics::prcnt(1, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = 03;
			}
		}
		else if (clickQuit) {
			alpha = GenericGameTools::incrementAtRate(0, 255, 800);
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			SDL_Rect largeBox = SDL_Rect{
				Graphics::prcnt(0, 'x', Items::w1),
				Graphics::prcnt(0, 'y', Items::w1),
				Graphics::prcnt(1, 'x', Items::w1),
				Graphics::prcnt(1, 'y', Items::w1)
			};
			SDL_RenderFillRect(Items::r1, &largeBox);
			if (alpha >= 255) {
				screenRunning = false;
				Items::screen = -1;
			}
		}

		versWords->renderTexture();

		SDL_SetRenderDrawColor(Items::r1, 255, 255, 255, 000);
		SDL_RenderPresent(Items::r1);
	}

	void close() {
		clickPlay = false;
		clickExtra = false;
		clickQuit = false;
		playGenericClick = false;
		alpha = 0;
	}

	//Black fade in from OSeq
	//whiteflash fadinto menuBG

}


namespace DefaultGame {

	bool screenRunning = true;
	bool lOpen = false;
	bool rOpen = false;
	bool errorSound, levelSound, levelSoundHold;
	//max event probablilty
	//float sfxChance = ; //99.9% in 1800 frames
	//int sfxRange = 100000; // the mod of a rand
	int sfxCutOff = 25; // the bound
						//  random of 100,000 is less than cutoff

	int passingCutoff_4 = 25;
	int hornCutoff_3 = 19;
	int commsCutoff_2 = 13;
	int bangCutoff_1 = 7;

	int promptMsgSize = 50;
	int promptYesNoSize = 60;

	bool hoverNotThatOne, hoverLeft, hoverRight, hoverUDL, hoverUDR, hoverR, hoverL, hoverSel1, hoverSel2, hoverSel3, hoverCHCl3, hoverInce;

	bool playGenericClick;
	bool playGenericHover;
	bool rngSFXPlaying;

	bool chloroClick;
	bool inceClick;

	bool moSel = true, piSel, fiSel,
		haSel, thSel, brSel,
		maSel, feSel, xxSel,
		blSel, tieSel, shirtSel,
		skSel, paSel, thiSel;
	bool* curSel = &moSel;
	bool moAni = false, lPinchAni, rPinchAni, fiAni,
		haAni, thAni, brAni, noBrAni,
		maAni, feAni, xxAni;

	bool blOn = true, tieOn = true, shirtOn = true,
		skOn = true, paOn = true, thiOn = true;
	//boolblOn = false, tieOn = false, shirtOn = false,
	//	skOn = false, paOn = false, thiOn = false;

	int blFade = 255, tieFade = 255, tieTopFade = 0, shirtFade = 255,
		skFade = 255, paFade = 255, moisFade = 255, thiFade = 255;

	int semHaFade = 0, semMoFade = 0, semBrFade = 0, semStoFade = 0, semLegFade = 0;
	bool semHaActive = false, semMoActive = false, semBrActive = false, semStoActive = false, semLegActive = false;

	bool moBuy = false, niBuy, fiBuy,
		haBuy, thBuy, brBuy,
		maBuy, feBuy, xxBuy,
		blBuy = false, tieBuy = false, shirtBuy = false,
		skBuy, paBuy, thiBuy;

	/*bool moBuy = true, niBuy= true, fiBuy= true,
		haBuy= true, thBuy= true, brBuy= true,
		maBuy= true, feBuy= true, xxBuy= true,
		blBuy = true, tieBuy = true, shirtBuy = true,
		skBuy = true, paBuy = true, thiBuy = true;
*/
	int moPrice = 100, niPrice = 400, fiPrice = 400,
		haPrice = 200, thPrice = 200, brPrice = 400,
		maPrice = 400, fePrice = 400, xxPrice = 800,
		blPrice = 100, tiePrice = 50, shirtPrice = 300,
		skPrice = 200, paPrice = 300, thiPrice = 50;

	int FPS = 30;

	int mFad  = 0;
	double fadeRate;
	DATATYPE_MILLISECONDS systemTime;

	int alpha;
	int x;
	int y;
	int* screen;
	int elapsed = -1;
	int tab = 1;
	int yClip = 0;	

	int moHitBoxX, moHitBoxY;

	SDL_Rect haHitBox1, haHitBox2, haHitBox3, haHitBox4;
	SDL_Rect breastHitBox;

	int lpHitBoxX, lpHitBoxY;
	int rpHitBoxX, rpHitBoxY;
	int fiHitBoxX, fiHitBoxY;
	int thiHitBoxX, thiHitBoxY;

	double chary = 0;

	long long startTime;
	long long initial = 0;
	long long expired = 0;

	std::string pointsString;

	Texture* MainBG, *Character;
	Texture* UDArrowL, *UDArrowR, *RArrow, *StatsLogo, *ToolsLogo, *BoxOLH, *BoxOLV, *BoxOLSel, *BoxBGH, *BoxBGV, *BoxBGSel1, *BoxBGSel2, *BoxBGSel3
		, *BoxBGSel4, *BoxBGSel5, *BoxFullChloro, *BoxFullIncence, *BoxMainBGR, *BoxMainBGL, *TextBlazer, *TextBreast, *TextShirt, *TextFellatio, *TextHair
		, *TextMasturbation, *TextMouth, *TextNipples, *TextPanties, *TextSkirt, *TextThighHigh, *TextThigh, *TextTie, *TextVagina, *TextXXX;

	Words* Tab1Words, *Tab2Words, *Tab3Words, *Tab4Words, *Tab5Words, *ExperienceWords, *AwarenessWords, *PointsWords, *ChloroformWords, *ChloroformTipWords,
		*Incenence, *IncenceTipWords,
		*moTip, *niTip, *fiTip, *haTip, *thTip, *brTip, *maTip, *feTip, *xxTip, *blTip, *toTip, *skTip, *braTip, *paTip, *thhTip,
		*ChTip, *IncTip, *ScrollTip,
		*BuyWords, *DemoWords, *PriceWords
		;


	SDL_Rect largeBoxSel1, largeBoxSel2, largeBoxSel3;

	void runGame();
	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();
	void reset();
	bool moisture;
	int moisAlpha;
	int scrollRate;
	double scrlPrcnt = 0.08;
	double sleepModifier = 1.0, pointModifier = 1.0, expModifier = 1.0, sleepDecMod = 1.0;
	int expPoints, expCounter, expCounterGain = 1, expTimer, gainLimiter;
	double awareCounter;
	double countdownTimer, timerStart;
	void calculatePoints(int expMS, int awareMS, bool gain) {
		int expGain = 10000 / (((double)expMS / 1000.0) * 30);
		int awareGain = 10000 / (((double)awareMS / 1000.0) * 30);
		//std::cout << expGain<<" " << awareGain<< "\n" << expCounter << " " << awareCounter <<"\n";
		if (gain) expCounter += expGain * expModifier;
		if (expCounter > 10000) {
			gainLimiter++;
			if (gainLimiter > 3) {
				levelSound = true;
				expPoints += expCounterGain * 10 * pointModifier;
				gainLimiter = 0;
			}
			expTimer++;
			if (expTimer > 1250 / FPS) {
				levelSound = false;
				levelSoundHold = false;
				expTimer = 0;
				expCounter = 0;
			}
		}
		if (gain) awareCounter += awareGain * sleepModifier;
		else if (awareCounter > 0) awareCounter -= 12 * sleepDecMod;
	}

	bool hover;
	bool start = false;
	bool ctrlClick = false;
	bool lightfx = false;

	int wallX, wallY, bgX, bgY;
	int wallH, wallW, bgH, bgW;

	SDL_Rect torso;
	SDL_Rect head;
	SDL_Rect lArm;
	SDL_Rect rArm;
	SDL_Rect pelvis;
	SDL_Rect lLeg;
	SDL_Rect rLeg;
	double larmY, rArmY, lLegY, rLegY;
	bool neutralSFX = false;
//#define HBOX

	void reset() {
		neutralSFX = false;
		sleepDecMod = 1.0;
		hover = false;
		moisture = false;
		moisAlpha = 0;
		alpha = 0;
		Items::bgmLibrary.TrainBG->playFadeIn(-1, 1000);
		Items::sfxLibrary.startSFX->playSFX();
		//Mix_VolumeChunk(Items::sfxLibrary.PassingSFX->sfx, SDL_MIX_MAXVOLUME);
		Mix_VolumeChunk(Items::sfxLibrary.IntercomSFX->sfx, 80);
		Mix_VolumeChunk(Items::sfxLibrary.bangSFX->sfx, 70);

		scrollRate = Graphics::prcnt(scrlPrcnt - 0.04, 'y', Items::w1);

		screenRunning = true;
		lOpen = false;
		rOpen = false;

		moSel = true;
		piSel = false; fiSel = false;
		haSel = false; thSel = false; brSel = false;
		maSel = false; feSel = false; xxSel = false;
		blSel = false; tieSel = false; shirtSel = false;
		skSel = false; paSel = false; thiSel = false;
		moAni = false, lPinchAni = false, rPinchAni = false, fiAni = false,
			haAni = false, thAni = false, brAni = false, noBrAni = false,
			maAni = false, feAni = false, xxAni = false;

		blOn = true, tieOn = true, shirtOn = true,
			skOn = true, paOn = true, thiOn = true;

		blFade = 255, tieFade = 255, tieTopFade = 0, shirtFade = 255,
			skFade = 255, paFade = 255, moisFade = 255, mFad = 0, thiFade = 255;
		semHaFade = 0, semMoFade = 0, semBrFade = 0, semStoFade = 0, semLegFade = 0;
		semHaActive = false, semMoActive = false, semBrActive = false, semStoActive = false, semLegActive = false;

		moBuy = false, niBuy = false, fiBuy = false,
			haBuy = false, thBuy = false, brBuy = false,
			maBuy = false, feBuy = false, xxBuy = false,
			blBuy = false, tieBuy = false, shirtBuy = false,
			skBuy = false, paBuy = false, thiBuy = false;


		if (Items::gameMode == 1) {//free
			moBuy = true, niBuy = true, fiBuy = true,
				haBuy = true, thBuy = true, brBuy = true,
				maBuy = true, feBuy = true, xxBuy = true,
				blBuy = true, tieBuy = true, shirtBuy = true,
				skBuy = true, paBuy = true, thiBuy = true;
			sleepModifier = 0;
			pointModifier = 0;
			expModifier = 0;
		}
		else if (Items::gameMode == 2) {//reg
			moBuy = false, niBuy = false, fiBuy = false,
				haBuy = false, thBuy = false, brBuy = false,
				maBuy = false, feBuy = false, xxBuy = false,
				blBuy = false, tieBuy = false, shirtBuy = false,
				skBuy = false, paBuy = false, thiBuy = false;
			sleepModifier = 1.0;
			pointModifier = 1.0;
			expModifier = 1.0;
		}
		else if (Items::gameMode == 3) {//time
			moBuy = true, niBuy = false, fiBuy = false,
				haBuy = false, thBuy = false, brBuy = false,
				maBuy = false, feBuy = false, xxBuy = false,
				blBuy = false, tieBuy = false, shirtBuy = false,
				skBuy = false, paBuy = false, thiBuy = false;
			sleepModifier = 1.0;
			pointModifier = 1.5;
			expModifier = 1.0;

		}
		else if (Items::gameMode == 4) {//hard
			moBuy = false, niBuy = false, fiBuy = false,
				haBuy = false, thBuy = false, brBuy = false,
				maBuy = false, feBuy = false, xxBuy = false,
				blBuy = false, tieBuy = false, shirtBuy = false,
				skBuy = false, paBuy = false, thiBuy = false;
			sleepModifier = 1.0;
			pointModifier = 1.2;
			expModifier = 1.0;
		}


		hoverInce = hoverCHCl3 = false;

		startTime = 0;
		initial = 0;
		expired = 0;

		start = false;
		ctrlClick = false;
		lightfx = false;

		awareCounter = 0;
		expCounter = 0;
		expPoints = 0;

		Items::animationLibrary.pokeFake->clearFrames();
		Items::animationLibrary.handFake->clearFrames();
		Items::gameGraphics.TextWall->resetTimer();
		Items::gameGraphics.TextWall->setTransparency(255);
		Items::gameGraphics.TextWall->box.x = wallX;
		Items::gameGraphics.TextWall->box.y = wallY;
		Items::gameGraphics.TextWall->box.w = wallW;
		Items::gameGraphics.TextWall->box.h = wallH;
		Items::gameGraphics.TextBG->resetTimer();
		Items::gameGraphics.TextBG->box.x = bgX;
		Items::gameGraphics.TextBG->box.y = bgY;
		Items::gameGraphics.TextBG->box.w = bgW;
		Items::gameGraphics.TextBG->box.h = bgH;
		alpha = 0;
		startTime = SDL_GetTicks();
		GenericGameTools::reset();
		timerStart = SDL_GetTicks();
	}

	int bxOffset;
	int xOffSet;
	int failStartTime;
	void runGame() {
		sleepDecMod = 1.0;
		hover = false;
		moisture = false;
		moisAlpha = 0;
		alpha = 0;
		Items::bgmLibrary.TrainBG->playFadeIn(-1, 1000);
		//Mix_VolumeChunk(Items::sfxLibrary.PassingSFX->sfx, SDL_MIX_MAXVOLUME);
		Mix_VolumeChunk(Items::sfxLibrary.IntercomSFX->sfx, 60);
		Mix_VolumeChunk(Items::sfxLibrary.bangSFX->sfx, 70);
		Mix_VolumeChunk(Items::sfxLibrary.neutSFX->sfx, 40);

		bxOffset = Graphics::prcnt(0.0265, 'x', Items::w1);
		xOffSet = Graphics::prcnt(0.0425, 'x', Items::w1);

		largeBoxSel1 = { Items::gameGraphics.BoxBGSel1->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
			Items::gameGraphics.BoxBGSel1->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
			Items::gameGraphics.BoxBGSel1->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
			Items::gameGraphics.BoxBGSel1->box.h + Graphics::prcnt(0.01, 'y', Items::w1) };
		largeBoxSel2 = { Items::gameGraphics.BoxBGSel2->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
			Items::gameGraphics.BoxBGSel2->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
			Items::gameGraphics.BoxBGSel2->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
			Items::gameGraphics.BoxBGSel2->box.h + Graphics::prcnt(0.01, 'y', Items::w1) };
		largeBoxSel3 = { Items::gameGraphics.BoxBGSel3->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
			Items::gameGraphics.BoxBGSel3->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
			Items::gameGraphics.BoxBGSel3->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
			Items::gameGraphics.BoxBGSel3->box.h + Graphics::prcnt(0.01, 'y', Items::w1) };

		scrollRate = Graphics::prcnt(scrlPrcnt - 0.04, 'y', Items::w1);

		moHitBoxX = Graphics::prcnt(0.6, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.440, 'y', Items::w1) - yClip * scrollRate;

		haHitBox1.x = Graphics::prcnt(0.60, 'x', Items::w1); haHitBox1.y = Graphics::prcnt(0.4575, 'y', Items::w1) - yClip * scrollRate, haHitBox1.w = Graphics::prcnt(0.170, 'x', Items::w1); haHitBox1.h = Graphics::prcnt(0.11, 'y', Items::w1);
		haHitBox2.x = Graphics::prcnt(0.355, 'x', Items::w1); haHitBox2.y = Graphics::prcnt(0.289, 'y', Items::w1) - yClip * scrollRate, haHitBox2.w = Graphics::prcnt(0.1535, 'x', Items::w1); haHitBox2.h = Graphics::prcnt(0.22, 'y', Items::w1);
		haHitBox3.x = Graphics::prcnt(0.425, 'x', Items::w1); haHitBox3.y = Graphics::prcnt(0.12, 'y', Items::w1) - yClip * scrollRate, haHitBox3.w = Graphics::prcnt(0.35, 'x', Items::w1); haHitBox3.h = Graphics::prcnt(0.17, 'y', Items::w1);
		haHitBox4.x = Graphics::prcnt(0.7, 'x', Items::w1); haHitBox4.y = Graphics::prcnt(0.289, 'y', Items::w1) - yClip * scrollRate, haHitBox4.w = Graphics::prcnt(0.118, 'x', Items::w1); haHitBox4.h = Graphics::prcnt(0.17, 'y', Items::w1);

		breastHitBox.x = Graphics::prcnt(0.27, 'x', Items::w1); breastHitBox.y = Graphics::prcnt(0.63, 'y', Items::w1) - yClip * scrollRate;;
		breastHitBox.w = Graphics::prcnt(0.4, 'x', Items::w1);  breastHitBox.h = Graphics::prcnt(0.25, 'y', Items::w1);

		thiHitBoxX = Graphics::prcnt(0.64, 'x', Items::w1);;
		thiHitBoxY = Graphics::prcnt(1.43 - yClip * scrlPrcnt, 'y', Items::w1);;
		lpHitBoxX = Graphics::prcnt(0.32, 'x', Items::w1); lpHitBoxY = Graphics::prcnt(0.814, 'y', Items::w1) - yClip * scrollRate;
		rpHitBoxX = Graphics::prcnt(0.53, 'x', Items::w1); rpHitBoxY = Graphics::prcnt(0.852, 'y', Items::w1) - yClip * scrollRate;
		fiHitBoxX = Graphics::prcnt(0.4, 'x', Items::w1); fiHitBoxY = Graphics::prcnt(1.15, 'y', Items::w1) - yClip * scrollRate;

		head = { Graphics::prcnt(0.355, 'x', Items::w1), Graphics::prcnt(0.12,'y', Items::w1) ,Graphics::prcnt(0.455, 'x', Items::w1) ,Graphics::prcnt(1.0, 'y', Items::w1) };
		torso = { Graphics::prcnt(0.28, 'x', Items::w1), Graphics::prcnt(0.67,'y', Items::w1) ,Graphics::prcnt(0.075, 'x', Items::w1) ,Graphics::prcnt(0.45, 'y', Items::w1) };
		pelvis = { Graphics::prcnt(0.22, 'x', Items::w1), Graphics::prcnt(0.90,'y', Items::w1) ,Graphics::prcnt(0.07, 'x', Items::w1) ,Graphics::prcnt(0.23, 'y', Items::w1) };
		larmY = 1.05;
		lArm = { Graphics::prcnt(0.08, 'x', Items::w1), Graphics::prcnt(larmY,'y', Items::w1) ,Graphics::prcnt(0.142, 'x', Items::w1) ,Graphics::prcnt(0.09, 'y', Items::w1) };
		rArmY = 1.12;
		rArm = { Graphics::prcnt(0.73, 'x', Items::w1), Graphics::prcnt(rArmY,'y', Items::w1) ,Graphics::prcnt(0.215, 'x', Items::w1) ,Graphics::prcnt(0.13, 'y', Items::w1) };
		lLegY = 1.10;
		lLeg = { Graphics::prcnt(0.125, 'x', Items::w1), Graphics::prcnt(lLegY,'y', Items::w1) ,Graphics::prcnt(0.56, 'x', Items::w1) ,Graphics::prcnt(0.24, 'y', Items::w1) };
		rLegY = 1.32;
		rLeg = { Graphics::prcnt(0.07, 'x', Items::w1), Graphics::prcnt(rLegY,'y', Items::w1) ,Graphics::prcnt(0.50, 'x', Items::w1) ,Graphics::prcnt(0.18, 'y', Items::w1) };

		Items::animationLibrary.moAni->setLocations(0.4317, 0.42075 - yClip * scrlPrcnt);
		Items::animationLibrary.haAni->setLocations(0.42207, 0.10805 - yClip * scrlPrcnt);
		Items::animationLibrary.lPiAni->setLocations(0.1244150, 0.565890 - yClip * scrlPrcnt);
		Items::animationLibrary.lPiAni->currentPosition = 22;
		Items::animationLibrary.rPiAni->setLocations(0.3511625, 0.6492 - yClip * scrlPrcnt);
		Items::animationLibrary.lPiAni->currentPosition = 14;
		Items::animationLibrary.fiAni->setLocations(0.055, 0.95 - yClip * scrlPrcnt);
		Items::animationLibrary.idleAni->setLocations(0.508 - 0.004808, 0.3095 - yClip * scrlPrcnt);
	
		Items::animationLibrary.eyesAni->setLocations(0.470, 0.290 - yClip * scrlPrcnt);
		Items::animationLibrary.felAni->setLocations(0.4734, 0.4220 - yClip * scrlPrcnt);
		Items::animationLibrary.masAni->setLocations(0.0, -yClip * scrlPrcnt);

		Items::animationLibrary.noBreAni->setLocations(0.25, 0.565 - yClip * scrlPrcnt);
		Items::animationLibrary.breAni->setLocations(0.27410, 0.45265 - yClip * scrlPrcnt);

		Items::animationLibrary.thiAni->setLocations(0.508 - 0.004808, 0.3095 - yClip * scrlPrcnt);

		for each(Texture* fr in Items::animationLibrary.eyesAni->frames) {
			fr->box.w *= 1.044660194174757;
			fr->box.h *= 1.044660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.felAni->frames) {
			fr->box.w *= 1.070;
			fr->box.h *= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.masAni->frames) {
			fr->box.w *= 1.070;
			fr->box.h *= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.noBreAni->frames) {
			fr->box.w *= 1.044660194174757;
			fr->box.h *= 1.044660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.breAni->frames) {
			fr->box.w *= 1.044660194174757;
			fr->box.h *= 1.044660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.thiAni->frames) {
			fr->box.w *= 1.070;
			fr->box.h *= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.idleAni->frames) {
			fr->box.w *= 1.084660194174757;
			fr->box.h *= 1.084660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.moAni->frames) {
			fr->box.w *= 1.0;
			fr->box.h *= 0.99;
		}
		for each(Texture* fr in Items::animationLibrary.haAni->frames) {
			fr->box.w *= 1.070;
			fr->box.h *= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.lPiAni->frames) {
			fr->box.w *= 1.070;
			fr->box.h *= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.rPiAni->frames) {
			fr->box.w *= 1.0700;
			fr->box.h *= 1.0700;
		}
		for each(Texture* fr in Items::animationLibrary.fiAni->frames) {
			fr->box.w *= 1.070;
			fr->box.h *= 1.070;
		}

		screenRunning = true;
		lOpen = false;
		rOpen = false;

		moSel = true;
		piSel= false; fiSel= false;
			haSel= false; thSel= false; brSel= false;
			maSel= false; feSel= false; xxSel= false;
			blSel= false; tieSel= false; shirtSel= false;
			skSel= false; paSel= false; thiSel = false;
		moAni = false, lPinchAni = false, rPinchAni = false, fiAni = false,
			haAni = false, thAni = false, brAni = false, noBrAni = false,
			maAni = false, feAni = false, xxAni = false;


		blOn = true, tieOn = true, shirtOn = true,
			skOn = true, paOn = true, thiOn = true;


		if (Items::gameMode == 1) {//free
			moBuy = true, niBuy = true, fiBuy = true,
				haBuy = true, thBuy = true, brBuy = true,
				maBuy = true, feBuy = true, xxBuy = true,
				blBuy = true, tieBuy = true, shirtBuy = true,
				skBuy = true, paBuy = true, thiBuy = true;
			sleepModifier = 0;
			pointModifier = 0;
			expModifier = 0;
		}
		else if (Items::gameMode == 2) {//reg
			moBuy = false, niBuy = false, fiBuy = false,
				haBuy = false, thBuy = false, brBuy = false,
				maBuy = false, feBuy = false, xxBuy = false,
				blBuy = false, tieBuy = false, shirtBuy = false,
				skBuy = false, paBuy = false, thiBuy = false;
			sleepModifier = 1.0;
			pointModifier = 1.0;
			expModifier = 1.0;
		}
		else if (Items::gameMode == 3) {//time
			moBuy = true, niBuy = false, fiBuy = false,
				haBuy = false, thBuy = false, brBuy = false,
				maBuy = false, feBuy = false, xxBuy = false,
				blBuy = false, tieBuy = false, shirtBuy = false,
				skBuy = false, paBuy = false, thiBuy = false;
			sleepModifier = 1.0;
			pointModifier = 1.5;
			expModifier = 1.0;

		}
		else if (Items::gameMode == 4) {//hard
			moBuy = false, niBuy = false, fiBuy = false,
				haBuy = false, thBuy = false, brBuy = false,
				maBuy = false, feBuy = false, xxBuy = false,
				blBuy = false, tieBuy = false, shirtBuy = false,
				skBuy = false, paBuy = false, thiBuy = false;
			sleepModifier = 1.0;
			pointModifier = 1.2;
			expModifier = 1.0;
		}


		//blOn = true, tieOn = true, shirtOn = true,
		//	skOn = true, paOn = true, thiOn = true;

		bool blFade = 255, tieFade = 255, shirtFade = 255,
			skFade = 255, paFade = 255, moisFade = 255, mFad = 0 ,tieTopFade = 0, thiFade = 255;
		semHaFade = 0, semMoFade = 0, semBrFade = 0, semStoFade = 0, semLegFade = 0;
		semHaActive = false, semMoActive = false, semBrActive = false, semStoActive = false, semLegActive = false;
		//bool moBuy = true, niBuy= true, fiBuy= true,
		//	haBuy= true, thBuy= true, brBuy= true,
		//	maBuy= true, feBuy= true, xxBuy= true,
		//	blBuy = false, tieBuy = false, shirtBuy = false,
		//	skBuy, paBuy, thiBuy;

		startTime = 0;
		initial = 0;
		expired = 0;

		start = false;
		ctrlClick = false;
		lightfx = false;

		BuyWords = new Words(44, 0, "Locked", SDL_Color{ 255,0,0,255 }, Items::r1, Items::w1);
		PriceWords = new Words(44, 0, "-1", SDL_Color{ 255,0,0,255 }, Items::r1, Items::w1);
		DemoWords = new Words(44, 0, "Demo", SDL_Color{ 255,0,0,255 }, Items::r1, Items::w1);

		wallX = Items::gameGraphics.TextWall->box.x;
		wallY = Items::gameGraphics.TextWall->box.y;
		wallW = Items::gameGraphics.TextWall->box.w;
		wallH = Items::gameGraphics.TextWall->box.h;
		bgX = Items::gameGraphics.TextBG->box.x;
		bgY = Items::gameGraphics.TextBG->box.y;
		bgW = Items::gameGraphics.TextBG->box.w;
		bgH = Items::gameGraphics.TextBG->box.h;

		gameLoop();
	}

	double deadlineTime = 5 * 1000;
	int nonHoverAlphaL = 140;
	int nonHoverAlphaR = 65;
	void gameLoop() {

		screenRunning = true;
		initial = 0;
		expired = 0;

		startTime = SDL_GetTicks();

#ifdef MAINGAMEOFF
		screenRunning = false;
		Items::screen = 01;awareCounter
#endif 

		unsigned seed = SDL_GetTicks();
		Items::generator.seed(seed);
		Items::generatorBinary.seed(seed);

		timerStart = SDL_GetTicks();

		while (screenRunning) {
			//float a = 0;
			//float b = 0;
			//std::cout << (a = SDL_GetTicks()) << "\n";

			systemTime = SYSTEM_MILLISECONDS;
			initial = systemTime.count();
			countdownTimer = SDL_GetTicks();

			processInput();
			update();
			play();
			render();
			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}

		close();

#ifdef MAINGAMEOFF
		Items::screen = 01;
#endif 

	}
	SDL_Rect growRect;
	double masYStore, semYLocation;
	void processInput() {
		Items::inputs = Items::mio->processKBMInput(&x, &y);
		if (LCLICK) {
			//std::cout << x / (1076 * Graphics::scale) << "\n";
			//std::cout << y / (1100 * Graphics::scale) << "\n";
			//std::cout << "aaaa";
		}
			/*if (Items::inputs->at(RIGHTMOUSECLICKED) && !Items::inputs->at(RIGHTMOUSEHELD))
				std::cout << "bbbb";*/
		//std::cout << Items::animationLibrary.lPiAni->frames.at(0)->box.x << " " << Items::animationLibrary.lPiAni->frames.at(0)->box.y  << "\n";
		//Items::animationLibrary.thiAni->currentPosition = 0;
			if (Items::inputs->at(MOUSEWHEELDOWN)) {
				if (yClip < 6) {
					yClip += 1;
					Items::animationLibrary.moAni->setLocations(0.4317, 0.42075 - yClip * scrlPrcnt);
					Items::animationLibrary.haAni->setLocations(0.42207, 0.10805 - yClip * scrlPrcnt);
					Items::animationLibrary.lPiAni->setLocations(0.1244150, 0.565890 - yClip * scrlPrcnt);
					Items::animationLibrary.lPiAni->currentPosition = 22;
					Items::animationLibrary.rPiAni->setLocations(0.3511625, 0.6492 - yClip * scrlPrcnt);
					Items::animationLibrary.lPiAni->currentPosition = 14;
					Items::animationLibrary.fiAni->setLocations(0.055, 0.95 - yClip * scrlPrcnt);
					Items::animationLibrary.idleAni->setLocations(0.508 - 0.004808, 0.3095 - yClip * scrlPrcnt);

					Items::animationLibrary.eyesAni->setLocations(0.470, 0.290 - yClip * scrlPrcnt);
					Items::animationLibrary.felAni->setLocations(0.4745, 0.42305 - yClip * scrlPrcnt);
					masYStore -= scrlPrcnt;

					Items::animationLibrary.masAni->setLocations(0.0, masYStore);
					std::cout << masYStore << "\n";

					Items::animationLibrary.noBreAni->setLocations(0.25, 0.565 - yClip * scrlPrcnt);
					Items::animationLibrary.breAni->setLocations(0.27410, 0.45265 - yClip * scrlPrcnt);

					Items::animationLibrary.thiAni->setLocations(0.38, 1.18 - yClip * scrlPrcnt);

					Items::animationLibrary.pokeFake->updateAssignedLocations(0, -scrlPrcnt);
					Items::animationLibrary.handFake->updateAssignedLocations(0, -scrlPrcnt);

					haHitBox1.y = Graphics::prcnt(0.4575 - yClip * scrlPrcnt, 'y', Items::w1);
					haHitBox2.y = Graphics::prcnt(0.289 - yClip * scrlPrcnt, 'y', Items::w1);
					haHitBox3.y = Graphics::prcnt(0.12 - yClip * scrlPrcnt, 'y', Items::w1);
					haHitBox4.y = Graphics::prcnt(0.289 - yClip * scrlPrcnt, 'y', Items::w1);

					breastHitBox.y = Graphics::prcnt(0.63 - yClip * scrlPrcnt, 'y', Items::w1);

					thiHitBoxX = Graphics::prcnt(0.67, 'x', Items::w1);;
					thiHitBoxY = Graphics::prcnt(1.45 - yClip * scrlPrcnt, 'y', Items::w1);;

					lpHitBoxY = Graphics::prcnt(0.814 - yClip * scrlPrcnt, 'y', Items::w1);
					rpHitBoxY = Graphics::prcnt(0.852 - yClip * scrlPrcnt, 'y', Items::w1);
					fiHitBoxY = Graphics::prcnt(1.15 - yClip * scrlPrcnt, 'y', Items::w1);

					torso.y = Graphics::prcnt(0.67 - yClip * scrlPrcnt, 'y', Items::w1);
					head.y = Graphics::prcnt(0.12 - yClip * scrlPrcnt, 'y', Items::w1);
					pelvis.y = Graphics::prcnt(0.90 - yClip * scrlPrcnt, 'y', Items::w1);
					lArm.y = Graphics::prcnt(larmY - yClip * scrlPrcnt, 'y', Items::w1);
					rArm.y = Graphics::prcnt(rArmY - yClip * scrlPrcnt, 'y', Items::w1);
					lLeg.y = Graphics::prcnt(lLegY - yClip * scrlPrcnt, 'y', Items::w1);
					rLeg.y = Graphics::prcnt(rLegY - yClip * scrlPrcnt, 'y', Items::w1);
					Items::gameGraphics.semHair->renderTextureFadeIn(1000);

					Items::gameGraphics.semHair->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;
					Items::gameGraphics.semBre->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;
					Items::gameGraphics.semLeg->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;;
					Items::gameGraphics.semSto->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;;
					Items::gameGraphics.semMo->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;;
				}
			}
			if (Items::inputs->at(MOUSEWHEELUP)) {
				if (yClip > 0) {
					yClip -= 1;
					Items::animationLibrary.moAni->setLocations(0.4317, 0.42075 - yClip * scrlPrcnt);
					Items::animationLibrary.haAni->setLocations(0.42207, 0.10805 - yClip * scrlPrcnt);
					Items::animationLibrary.lPiAni->setLocations(0.1244150, 0.565890 - yClip * scrlPrcnt);
					Items::animationLibrary.lPiAni->currentPosition = 22;
					Items::animationLibrary.rPiAni->setLocations(0.3511625, 0.6492 - yClip * scrlPrcnt);
					Items::animationLibrary.lPiAni->currentPosition = 14;
					Items::animationLibrary.fiAni->setLocations(0.055, 0.95 - yClip * scrlPrcnt);
					Items::animationLibrary.idleAni->setLocations(0.508 - 0.004808, 0.3095 - yClip * scrlPrcnt);

					Items::animationLibrary.eyesAni->setLocations(0.470, 0.290 - yClip * scrlPrcnt);

					masYStore += scrlPrcnt;
					Items::animationLibrary.felAni->setLocations(0.4745, 0.42305 - yClip * scrlPrcnt);
					Items::animationLibrary.masAni->setLocations(0.0, masYStore);
					std::cout << masYStore<< "\n";

					Items::animationLibrary.noBreAni->setLocations(0.25, 0.565 - yClip * scrlPrcnt);
					Items::animationLibrary.breAni->setLocations(0.27410, 0.45265 - yClip * scrlPrcnt);

					Items::animationLibrary.thiAni->setLocations(0.38, 1.18 - yClip * scrlPrcnt);

					Items::animationLibrary.pokeFake->updateAssignedLocations(0, scrlPrcnt);
					Items::animationLibrary.handFake->updateAssignedLocations(0, scrlPrcnt);

					haHitBox1.y = Graphics::prcnt(0.4575 - yClip * scrlPrcnt, 'y', Items::w1) ;
					haHitBox2.y = Graphics::prcnt(0.289 - yClip * scrlPrcnt, 'y', Items::w1);
					haHitBox3.y = Graphics::prcnt(0.12 - yClip * scrlPrcnt, 'y', Items::w1);
					haHitBox4.y = Graphics::prcnt(0.289 - yClip * scrlPrcnt, 'y', Items::w1);

					breastHitBox.y = Graphics::prcnt(0.63 - yClip * scrlPrcnt, 'y', Items::w1);

					thiHitBoxY = Graphics::prcnt(1.45 - yClip * scrlPrcnt, 'y', Items::w1);;
					lpHitBoxY = Graphics::prcnt(0.814 - yClip * scrlPrcnt, 'y', Items::w1);
					rpHitBoxY = Graphics::prcnt(0.852 - yClip * scrlPrcnt, 'y', Items::w1);
					fiHitBoxY = Graphics::prcnt(1.15 - yClip * scrlPrcnt, 'y', Items::w1);

					torso.y = Graphics::prcnt(0.67 - yClip * scrlPrcnt, 'y', Items::w1);
					head.y = Graphics::prcnt(0.12 - yClip * scrlPrcnt, 'y', Items::w1);
					pelvis.y = Graphics::prcnt(0.90 - yClip * scrlPrcnt, 'y', Items::w1);
					lArm.y = Graphics::prcnt(larmY - yClip * scrlPrcnt, 'y', Items::w1);
					rArm.y = Graphics::prcnt(rArmY - yClip * scrlPrcnt, 'y', Items::w1);
					lLeg.y = Graphics::prcnt(lLegY - yClip * scrlPrcnt, 'y', Items::w1);
					rLeg.y = Graphics::prcnt(rLegY - yClip * scrlPrcnt, 'y', Items::w1);

					Items::gameGraphics.semHair->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;
					Items::gameGraphics.semBre->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;
					Items::gameGraphics.semLeg->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;;
					Items::gameGraphics.semSto->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;;
					Items::gameGraphics.semMo->box.y = Graphics::prcnt(0 - yClip * scrlPrcnt, 'y', Items::w1);;;
				}
				//std::cout << "ddddd";	std::cout << yClip;
			}

			if (true) {
				if (Items::inputs->at(NUM1) && !Items::inputs->at(NUM1_H)) {
					*curSel = false;
					curSel = &moSel;
					moSel = true;
					feSel = false;
				}
				else if (Items::inputs->at(NUM2) && !Items::inputs->at(NUM2_H)) {
					if (niBuy) {
						*curSel = false;
						curSel = &piSel;
						piSel = true;
						brSel = false;
						brAni = false;
						noBrAni = false;
					}
				}

				if (Items::inputs->at(NUM3) && !Items::inputs->at(NUM3_H)) {
					if (fiBuy) {
						*curSel = false;
						curSel = &fiSel;
						fiSel = true;
					}
				}
				else if (Items::inputs->at(NUM4) && !Items::inputs->at(NUM4_H)) {
					if (haBuy) {
						*curSel = false;
						curSel = &haSel;
						haSel = true;
					}
				}
				else if (Items::inputs->at(NUM5) && !Items::inputs->at(NUM5_H)) {
					if (thBuy) {
						*curSel = false;
						curSel = &thSel;
						thSel = true;
					}
				}
				else if (Items::inputs->at(NUM6) && !Items::inputs->at(NUM6_H)) {
					if (brBuy) {
						*curSel = false;
						curSel = &brSel;
						brSel = true;
						piSel = false;
						lPinchAni = false;
						rPinchAni = false;
					}
				}
				else if (Items::inputs->at(NUM7) && !Items::inputs->at(NUM7_H)) {
					if (maBuy) {
						*curSel = false;
						curSel = &maSel;
						maSel = true;
					}
				}

				else if (Items::inputs->at(NUM8) && !Items::inputs->at(NUM8_H)) {
					if (feBuy) {
						*curSel = false;
						curSel = &feSel;
						feSel = true;
						moSel = false;
					}
				}
				//else if (Items::inputs->at(NUM9) && !Items::inputs->at(NUM9_H)) {
				//	if (xxBuy) {
				//		*curSel = false;
				//		curSel = &xxSel;
				//		xxSel = true;
				//	}
				//}
				else if (Items::inputs->at(KEYQ) && !Items::inputs->at(KEYQ_H)) {
					if (blBuy) {
						if (blOn) {
							blOn = false;
							blSel = true;
							lPinchAni = false;
							rPinchAni = false;
							noBrAni = false;
							brAni = false;
						}
						else {
							lPinchAni = false;
							rPinchAni = false;
							blOn = true;
							blSel = false;
							noBrAni = false;
							brAni = false;
						}
					}
				}
				if (Items::inputs->at(KEYW) && !Items::inputs->at(KEYW_H)) {
					if (tieBuy) {
						if (tieOn) {
							tieOn = false;
							tieSel = true;
						}
						else {
							tieOn = true;
							tieSel = false;
						}
					}
				}
				else if (Items::inputs->at(KEYE) && !Items::inputs->at(KEYE_H)) {
					if (shirtBuy) {
						if (shirtOn) {
							shirtOn = false;
							shirtSel = true;
							lPinchAni = false;
							rPinchAni = false;
							noBrAni = false;
							brAni = false;
						}

						else {
							shirtOn = true;
							shirtSel = false;
							lPinchAni = false;
							rPinchAni = false;
							noBrAni = false;
							brAni = false;
						}
					}
				}
				else if (Items::inputs->at(KEYR) && !Items::inputs->at(KEYR_H)) {
					if (skBuy) {
						if (skOn) {
							skOn = false;
							skSel = true;
						}
						else {
							fiAni = false;
							skOn = true;
							skSel = false;
						}
					}
				}
				else if (Items::inputs->at(KEYT) && !Items::inputs->at(KEYT_H)) {
					if (paBuy) {
						if (paOn) {
							paOn = false;
							paSel = true;
						}
						else {
							paOn = true;
							paSel = false;
						}
					}
				}
				else if (Items::inputs->at(KEYY) && !Items::inputs->at(KEYY_H)) {
					if (thiBuy) {
						if (thiOn) {
							thiSel = true;
							thiOn = false;
						}
						else {
							thiSel = false;
							thiOn = true;
						}
					}
				}
			}
	}	
	bool hoverMo, hoverHa, hoverFi, hoverLPinch, hoverRPinch, hoverBre, hoverThi;
	bool moveMo, moveHa, moveFi, moveLPi, moveRPi;
	bool openEyes, stopMouth;
	float aRadius = 0.10, neutralRadius = 0.1, bRadius = 0.05, cRadius = 0.20;
	float j;
	int graphicPosition = -1;
	SDL_Rect leftBox;
	void update() {
		screenRunning = !Items::inputs->at(QUIT);

		leftBox = { Graphics::prcnt(0.23, 'x', Items::w1), Graphics::prcnt(0.55, 'y', Items::w1), Items::gameGraphics.RArrow->box.w, Items::gameGraphics.RArrow->box.h };
		Items::gameGraphics.RArrow->box.y = Graphics::prcnt(0.3, 'y', Items::w1);
		hover = false;
		hoverLeft = false;
		hoverRight = false;
		if (GenericGameTools::checkRectHitBox(Items::gameGraphics.UDArrowL->box, x, y)) {
			hoverLeft = true;
			hoverUDL = true;

			playGenericHover = true;
			if (LCLICK) {
				lOpen = !lOpen;
				playGenericClick = true;
				//std::cout << "111";
			}
		}
		else
			hoverUDL = false;
		if (GenericGameTools::checkRectHitBox(Items::gameGraphics.UDArrowR->box, x, y)) {
			hoverNotThatOne = true;
			hoverRight = true;
			if (LCLICK) {
				rOpen = !rOpen;
				playGenericClick = true;
			//	std::cout << "222";
			}
			hoverUDR = true;
		}
		else
			hoverUDR = false;
		if (Items::inputs->at(CTRL) && !Items::inputs->at(CTRLHELD)) {
			GenericGameTools::reset();
			if (start)
				if (!ctrlClick ) ctrlClick = true;
				else ctrlClick = false;
		}

		if (lOpen) {
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxMainBGL->box, x, y)) {
				int transpa = 255;
				int offTraspa = 0;
				if (tab == 1) {
					Tab1Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel1->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextNipples->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextMouth->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextVagina->setTransparency(transpa + offTraspa);
				}
				else if (tab == 2) {
					Tab2Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel2->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextBreast->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextThigh->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextHair->setTransparency(transpa + offTraspa);
				}
				else if (tab == 3) {
					Tab3Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel3->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextFellatio->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextMasturbation->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextXXX->setTransparency(transpa + offTraspa);
				}
				else if (tab == 4) {
					Tab4Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel4->setTransparency(transpa);
					Items::gameGraphics.TextBlazer->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextShirt->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextTie->setTransparency(transpa + offTraspa);
				}
				else if (tab == 5) {
					Tab5Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel5->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextPanties->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextThighHigh->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextSkirt->setTransparency(transpa + offTraspa);

				}
				DemoWords->setTransparency(transpa);
				BuyWords->setTransparency(transpa);
				Items::gameGraphics.BoxMainBGL->setTransparency(transpa);
				Items::gameGraphics.selHighlight->setTransparency(transpa);
				Items::gameGraphics.selTabs->setTransparency(transpa);
				Items::gameGraphics.UDArrowL->setTransparency(transpa);
				Items::gameGraphics.RArrow->setTransparency(transpa);
				Items::gameGraphics.ToolsLogo->setTransparency(transpa);
			}
			else {
				int transpa = nonHoverAlphaR - 0;
				int offTraspa = 20;
				hover = true;
				if (tab == 1) {
					Tab1Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel1->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextNipples->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextMouth->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextVagina->setTransparency(transpa + offTraspa);
				}
				else if (tab == 2) {
					Tab2Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel2->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextBreast->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextThigh->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextHair->setTransparency(transpa + offTraspa);
				}
				else if (tab == 3) {
					Tab3Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel3->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextFellatio->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextMasturbation->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextXXX->setTransparency(transpa + offTraspa);
				}
				else if (tab == 4) {
					Tab4Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel4->setTransparency(transpa);
					Items::gameGraphics.TextBlazer->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextShirt->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextTie->setTransparency(transpa + offTraspa);
				}
				else if (tab == 5) {
					Tab5Words->setTransparency(transpa);
					Items::gameGraphics.BoxBGSel5->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextPanties->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextThighHigh->setTransparency(transpa + offTraspa);
					Items::gameGraphics.TextSkirt->setTransparency(transpa + offTraspa);

				}
				DemoWords->setTransparency(transpa);
				BuyWords->setTransparency(transpa);
				Items::gameGraphics.BoxMainBGL->setTransparency(transpa);
				Items::gameGraphics.selHighlight->setTransparency(transpa);
				Items::gameGraphics.selTabs->setTransparency(transpa);
				Items::gameGraphics.UDArrowL->setTransparency(transpa);
				Items::gameGraphics.RArrow->setTransparency(transpa);
				Items::gameGraphics.ToolsLogo->setTransparency(transpa);
			}
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.RArrow->box, x, y)) {
				hoverNotThatOne = true;
				hoverR = true;
				playGenericHover = true;
				if (hoverR && LCLICK) {
					//	std::cout << "333";
					playGenericClick = true;
					tab++;
					if (tab > 5) {
						tab = 1;
					}
				}
			}
			else
				hoverR = false;
			if (GenericGameTools::checkRectHitBox(leftBox, x, y)) {
				hoverNotThatOne = true;
				hoverL = true;
				playGenericHover = true;
				if (hoverL && LCLICK) {
					//	std::cout << "333";
					playGenericClick = true;
					tab--;
					if (tab < 1) {
						tab = 5;
					}
				}
			}
			else
				hoverL = false;
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxBGSel1->box, x, y)) {
				hoverNotThatOne = true;
				hoverSel1 = true;
				playGenericHover = true;
				if (LCLICK) {
				//	std::cout << "444";

					if (tab == 1) {
						if (moBuy) {
							*curSel = false;
							curSel = &moSel;
							moSel = true;
							feSel = false;
						}
						else {
							if (expPoints >= moPrice) {
								moBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(20), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2),SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Nothing", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (moBuy) {
									expPoints -= moPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
					}
#ifndef BETA
					else if (tab == 2) {
						if (haBuy) {
							*curSel = false;
							curSel = &haSel;
							haSel = true;
						}
						else {
							if (expPoints >= haPrice) {
								haBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(6), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2),SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Nothing", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (haBuy) {
									expPoints -= haPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}

						}
					}
#endif
					else if (tab == 3) {
						if (maBuy) {
							*curSel = false;
							curSel = &maSel;
							maSel = true;
						}
						else {
							if (expPoints >= maPrice) {
								maBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(7), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Nothing", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (maBuy) {
									expPoints -= maPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}

						}
					}

					else if (tab == 4) {
						if (blBuy) {
							if (blOn) {
								blOn = false;
								blSel = true;
								noBrAni = false;
								brAni = false;
								lPinchAni = false;
								rPinchAni = false;
							}
							else {
								noBrAni = false;
								brAni = false;
								lPinchAni = false;
								rPinchAni = false;
								blOn = true;
								blSel = false;
							}
						}
						else {
							if (expPoints >= blPrice) {
								blBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(8), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
								});
								if (blBuy) {
									expPoints -= blPrice;
								}else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
					}
#ifndef BETA
					else if (tab == 5) {
						if (skBuy) {
							if (skOn) {
								skOn = false;
								skSel = true;
							}
							else {
								skOn = true;
								skSel = false;
							}
						}
						else {
							if (expPoints >= skPrice) {
								skBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(9), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
								new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
								new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (skBuy) {
									expPoints -= skPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}

						}
					}
#endif
				}
			}
			else
				hoverSel1 = false;
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxBGSel2->box, x, y)) {
				hoverSel2 = true;
				hoverNotThatOne = true;
				playGenericHover = true;
				if (LCLICK) {
			//		std::cout << "555";
#ifndef BETA
					if (tab == 1) {
						if (niBuy) {
							*curSel = false;
							curSel = &piSel;
							piSel = true;
							brSel = false;
							brAni = false;
							noBrAni = false;
						}
						else {
							if (expPoints >= niPrice) {				
								niBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(10), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Topless", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (niBuy) {
									expPoints -= niPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
					}
					else
						#endif
						if (tab == 2) {
						if (thBuy) {
							*curSel = false;
							curSel = &thSel;
							thSel = true;
						}
						else {
							if (expPoints >= thPrice) {
								thBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(11), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Nothing", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (thBuy) {
									expPoints -= thPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}			
						}
					}
#ifndef BETA
					else if (tab == 3) {
						if (feBuy) {
							*curSel = false;
							curSel = &feSel;
							feSel = true;
							moSel = false;
						}
						else {
							if (expPoints >= fePrice) {
								feBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(12), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Nothing", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (feBuy) {
									expPoints -= fePrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}

						}
					}
					else 
#endif		
						if (tab == 4) {
						if (tieBuy) {
							if (tieOn) {
								tieOn = false;
								tieSel = true;
							}
							else {
								tieOn = true;
								tieSel = false;
							}
						}
						else {
							if (expPoints >= tiePrice) {
								tieBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(13), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (tieBuy) {
									expPoints -= tiePrice;
								}	else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
					}
#ifndef BETA
						else if (tab == 5) {
						if (paBuy) {
							if (paOn) {
								paOn = false;
								paSel = true;
							}
							else {
								paOn = true;
								paSel = false;
							}
						}
						else {
							if (expPoints >= paPrice) {
								paBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(14), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (paBuy) {
									expPoints -= paPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
		
					}
#endif
				}
			}
			else
				hoverSel2 = false;

			if 	(GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxBGSel3->box, x, y)) {
				hoverSel3 = true;
				hoverNotThatOne = true;
				playGenericHover = true;
				if (LCLICK) {
		//			std::cout << "666";
#ifndef BETA
					if (tab == 1) {
						if (fiBuy) {
							*curSel = false;
							curSel = &fiSel;
							fiSel = true;
						}
						else {
							if (expPoints >= fiPrice) {
								fiBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(15), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Bottomless", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (fiBuy) {
									expPoints -= fiPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
					}
					else
#endif
						if (tab == 2) {
						if (brBuy) {
							*curSel = false;
							curSel = &brSel;
							brSel = true;
							piSel = false;
							lPinchAni = false;
							rPinchAni = false;
						}
						else {
							if (expPoints >= brPrice) {
								brBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(16), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize-9,0, "Required: Topless/Shirt Only", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (brBuy) {
									expPoints -= brPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
					}
					else if (tab == 3) {
						if (xxBuy) {
							*curSel = false;
							curSel = &xxSel;
							xxSel = true;
							Items::gameState = -1;
							screenRunning = false;
						}
						else {
							if (expPoints >= xxPrice) {
								xxBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(17), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptMsgSize,0, "Required: Fully Nude", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (xxBuy) {
									expPoints -= xxPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}

						}

					}

#ifndef BETA 
					else if (tab == 4) {
						if (shirtBuy) {
							if (shirtOn) {
								shirtOn = false;
								shirtSel = true;
								noBrAni = false;
								brAni = false;
								lPinchAni = false;
								rPinchAni = false;
							}

							else {
								lPinchAni = false;
								rPinchAni = false;
								noBrAni = false;
								brAni = false;
								shirtOn = true;
								shirtSel = false;
							}
						}
						else {
							if (expPoints >= shirtPrice) {
								shirtBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(18), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (shirtBuy) {
									expPoints -= shirtPrice;
								}	else 	errorSound = true;
							}
							else {
								errorSound = true;
							}
						}
					}
#endif
#ifndef BETA
					else if (tab == 5) {
						if (thiBuy) {
							if (thiOn){
								thiSel = true;
								thiOn = false;
							}
							else {
								thiSel = false;
								thiOn = true;
							}
						}
						else {
							if (expPoints >= thiPrice) {
								thiBuy = Prompts::runPrompt({ new Words(promptMsgSize ,0,Text::PromptText.at(19), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
									new Words(promptYesNoSize ,0,Text::PromptText.at(2), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) ,
									new Words(promptYesNoSize ,0,Text::PromptText.at(3), SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) });
								if (thiBuy) {
									expPoints -= thiPrice;
								}
								else 	errorSound = true;
							}
							else {
								errorSound = true;
							}

						}

					}
#endif
				}

			}
			else
				hoverSel3 = false;

		}
		if (rOpen) {
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxMainBGR->box, x, y)) {
				int transpa = 255;

				pointsString = GenericGameTools::createPointString(expPoints, 4);
				PointsWords->changeText(pointsString);

				Items::gameGraphics.BoxFullIncence->setTransparency(transpa);
				Items::gameGraphics.BoxFullChloro->setTransparency(transpa);
				Items::gameGraphics.BoxBGV->setTransparency(transpa);
				Items::gameGraphics.BoxBGH->setTransparency(transpa);
				Items::gameGraphics.BoxOLV->setTransparency(transpa);
				Items::gameGraphics.BoxOLH->setTransparency(transpa);
				ExperienceWords->setTransparency(transpa);
				PointsWords->setTransparency(transpa);
				Incenence->setTransparency(transpa);
				ChloroformWords->setTransparency(transpa);
				AwarenessWords->setTransparency(transpa);
				ChloroformTipWords->setTransparency(transpa);
				IncenceTipWords->setTransparency(transpa);
				Items::gameGraphics.StatsLogo->setTransparency(transpa);
				Items::gameGraphics.UDArrowR->setTransparency(transpa);
				Items::gameGraphics.BoxMainBGR->setTransparency(transpa);
			}
			else {
				int transpa = nonHoverAlphaR - 20;

				pointsString = GenericGameTools::createPointString(expPoints, 4);
				PointsWords->changeText(pointsString);

				Items::gameGraphics.BoxFullIncence->setTransparency(transpa);
				Items::gameGraphics.BoxFullChloro->setTransparency(transpa);
				Items::gameGraphics.BoxBGV->setTransparency(transpa);
				Items::gameGraphics.BoxBGH->setTransparency(transpa);
				Items::gameGraphics.BoxOLV->setTransparency(transpa);
				Items::gameGraphics.BoxOLH->setTransparency(transpa);
				ExperienceWords->setTransparency(transpa);
				PointsWords->setTransparency(transpa);
				Incenence->setTransparency(transpa);
				ChloroformWords->setTransparency(transpa);		
				AwarenessWords->setTransparency(transpa);
				ChloroformTipWords->setTransparency(transpa);
				IncenceTipWords->setTransparency(transpa);
				Items::gameGraphics.StatsLogo->setTransparency(transpa);
				Items::gameGraphics.UDArrowR->setTransparency(transpa);
				Items::gameGraphics.BoxMainBGR->setTransparency(transpa);
			}

			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxFullChloro->box, x, y)) {
				hoverCHCl3 = true;
				hoverNotThatOne = true;
				playGenericHover = true;
				if (LCLICK) {
					//std::cout << "888";
					playGenericClick = true;
					if (sleepModifier > 0.0 && expPoints >= 100) {
						sleepModifier *= 0.80;
						sleepDecMod *= 1.06;
						awareCounter -= 2500 * 1;
						if (awareCounter < 0) awareCounter = 0;
						expPoints -= 100;
						chloroClick = true;
					}
					else errorSound = true;
				}
			}
			else
				hoverCHCl3 = false;
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxFullIncence->box, x, y)) {
				hoverInce = true;
				hoverNotThatOne = true;
				playGenericHover = true;
				if (LCLICK) {
					//std::cout << "888";
					playGenericClick = true;
					if (expPoints >= 150) {
						pointModifier *= 1.10;
						expPoints -= 150;
						inceClick = true;
					}
					else errorSound = true;
				}
			}
			else
				hoverInce = false;
		}

		hoverMo = false;
		hoverHa = false;
		hoverFi = false;
		hoverLPinch = false;
		hoverRPinch = false;
		hoverThi = false;
		hoverBre = false;
		bool poke = true;

		/*std::cout << GenericGameTools::checkRectHitBox(haHitBox1, x, y) << " " <<
			GenericGameTools::checkRectHitBox(haHitBox2, x, y) << " " <<
			GenericGameTools::checkRectHitBox(haHitBox3, x, y) << " " <<
			GenericGameTools::checkRectHitBox(haHitBox4, x, y) << "\n";*/

		/////////////////MOUTH
		if (moveMo) hoverMo = true;
		else if (moveHa) hoverHa = true;
		else if (moveFi) hoverFi = true;
		else if (moveLPi) lPinchAni = true;
		else if (moveRPi) rPinchAni = true;
		else if (moAni && (GenericGameTools::checkRectHitBox(Items::animationLibrary.moAni->frames.at(Items::animationLibrary.moAni->currentPosition)->box, x, y))) {
			hoverMo = true;
			poke = false;
		}
		else if (!moAni && GenericGameTools::checkCircleHitBox(moHitBoxX, moHitBoxY - yClip * scrollRate, Graphics::prcnt(aRadius, 'x', Items::w1), x, y)) {
			hoverMo = true;
			poke = false;
			Items::animationLibrary.moAni->currentPosition = 0;
		}
		///////////////////HAIR
		else if (haAni && (GenericGameTools::checkRectHitBox(Items::animationLibrary.haAni->frames.at(Items::animationLibrary.haAni->currentPosition)->box, x, y))
			|| (GenericGameTools::checkRectHitBox(haHitBox1, x, y) ||
				GenericGameTools::checkRectHitBox(haHitBox2, x, y) ||
				GenericGameTools::checkRectHitBox(haHitBox3, x, y) ||
				GenericGameTools::checkRectHitBox(haHitBox4, x, y))) {
			hoverHa = true;
			poke = false;
		}
		else if (!haAni &&
			(GenericGameTools::checkRectHitBox(haHitBox1, x, y) ||
			GenericGameTools::checkRectHitBox(haHitBox2, x, y) ||
			GenericGameTools::checkRectHitBox(haHitBox3, x, y) ||
			GenericGameTools::checkRectHitBox(haHitBox4, x, y))) {
			hoverHa = true;
			poke = false;
			Items::animationLibrary.haAni->currentPosition = 0;
		}
		///////////////////FING
		else if (fiAni && !skOn && (GenericGameTools::checkRectHitBox(Items::animationLibrary.fiAni->frames.at(Items::animationLibrary.fiAni->currentPosition)->box, x, y)
			)) {
			poke = false;
			hoverFi = true;
		}
		else if (!fiAni && !skOn && (GenericGameTools::checkCircleHitBox(fiHitBoxX, fiHitBoxY - yClip * scrollRate, Graphics::prcnt(aRadius, 'x', Items::w1), x, y))) {
			hoverFi = true;
			poke = false;
		}
		///////////////////LPINCH
		else if (lPinchAni && (GenericGameTools::checkRectHitBox(Items::animationLibrary.lPiAni->frames.at(Items::animationLibrary.lPiAni->currentPosition)->box, x, y)
			&& !shirtOn && !blOn && !tieOn)) {
			poke = false;
			hoverLPinch = true;
		}
		else if (!lPinchAni&& (GenericGameTools::checkCircleHitBox(lpHitBoxX, lpHitBoxY - yClip * scrollRate, Graphics::prcnt(aRadius, 'x', Items::w1), x, y)
			&& !shirtOn && !blOn&& piSel && !tieOn)) {
			poke = false;
			hoverLPinch = true;
			Items::animationLibrary.lPiAni->currentPosition = 14;
		}
		///////////////////RPINCH
		else if (rPinchAni && (GenericGameTools::checkRectHitBox(Items::animationLibrary.rPiAni->frames.at(Items::animationLibrary.rPiAni->currentPosition)->box, x, y)
			&& !shirtOn && !blOn && !tieOn)) {
			poke = false;
			hoverRPinch = true;
		}
		else if (!rPinchAni && GenericGameTools::checkCircleHitBox(rpHitBoxX, rpHitBoxY - yClip * scrollRate, Graphics::prcnt(aRadius, 'x', Items::w1), x, y) 
			&& !shirtOn && !blOn&& piSel && !tieOn) {
			poke = false;
			hoverRPinch = true;
			Items::animationLibrary.rPiAni->currentPosition = 22;
		}
		////////////////MASAGE
		else if (noBrAni && shirtOn && !tieOn && !blOn && GenericGameTools::checkRectHitBox(breastHitBox, x, y)) {
			//std::cout << "A\n"; 
			hoverBre = true;
			poke = false;
		}
		else if (brAni && !shirtOn && !tieOn && !blOn && GenericGameTools::checkRectHitBox(breastHitBox, x, y)) {
			//std::cout << "B\n"; 
			hoverBre = true;
			poke = false;
		}
		else if (!noBrAni && shirtOn && !tieOn && !blOn && GenericGameTools::checkRectHitBox(breastHitBox, x, y)) {
			//std::cout << "C\n";
			hoverBre = true;
			Items::animationLibrary.noBreAni->currentPosition = 0;
			poke = false;
		}
		else if (!brAni && !shirtOn && !tieOn && !blOn && GenericGameTools::checkRectHitBox(breastHitBox, x, y)) {
			//std::cout << "D\n";
			hoverBre = true;
			Items::animationLibrary.breAni->currentPosition = 0;
			poke = false;
		}
		//THIGH
		else if (thAni && (GenericGameTools::checkRectHitBox(Items::animationLibrary.thiAni->frames.at(Items::animationLibrary.thiAni->currentPosition)->box, x, y))) {
			hoverThi = true;
			poke = false;
			//std::cout << "E\n";
		}
		else if (!thAni && (GenericGameTools::checkCircleHitBox(thiHitBoxX, thiHitBoxY - yClip * scrollRate, Graphics::prcnt(cRadius, 'x', Items::w1), x, y))) {
			hoverThi = true;
			poke = false;
			//std::cout << "F\n";
		}
		if (hoverUDR || hoverUDL || hoverR || hoverL || hoverCHCl3 || hoverInce || hoverSel1 || hoverSel2 || hoverSel3) {}
		else if (LCLICK) {
			bool del = false;
			graphicPosition = Items::animationLibrary.pokeFake->searchFramesByCoordinates(x, y);
			int graphicPosition2 = Items::animationLibrary.handFake->searchFramesByCoordinates(x, y);
			if (graphicPosition >= 0) {
				Items::animationLibrary.pokeFake->assignedFrameCopies.erase(Items::animationLibrary.pokeFake->assignedFrameCopies.begin() + graphicPosition);
				del = true;
			}
			else if (graphicPosition2 >= 0) {
				Items::animationLibrary.handFake->assignedFrameCopies.erase(Items::animationLibrary.handFake->assignedFrameCopies.begin() + graphicPosition2);
				del = true;
			}
			else if (maAni && GenericGameTools::checkRectHitBox(Items::animationLibrary.masAni->frames.at(Items::animationLibrary.masAni->currentPosition)->box, x, y)) {
				maAni = false;
				Items::animationLibrary.masAni->currentPosition = 0;
				del = true;
			}
			if (del) {}
			else if (poke) {
				if (tab == 1 || tab == 5) {
					Items::animationLibrary.pokeFake->assignFrameToLocation(x - Graphics::prcnt(0.15, 'x', Items::w1), y - Graphics::prcnt(0.00, 'y', Items::w1));
					Items::animationLibrary.pokeFake->assignedFrameCopies.back()->setMovementPattern(
						x - Graphics::prcnt(0.15, 'x', Items::w1), y - Graphics::prcnt(0.00, 'y', Items::w1),
						x - Graphics::prcnt(0.135, 'x', Items::w1), y - Graphics::prcnt(0.015, 'y', Items::w1),
						1.0);
				}
				else if (tab == 2 || tab == 4) {
					Items::animationLibrary.handFake->assignFrameToLocation(x - Graphics::prcnt(0.08, 'x', Items::w1), y - Graphics::prcnt(0.09, 'y', Items::w1));
					Items::animationLibrary.handFake->assignedFrameCopies.back()->setMovementPattern(
						x - Graphics::prcnt(0.08, 'x', Items::w1), y - Graphics::prcnt(0.09, 'y', Items::w1),
						x - Graphics::prcnt(0.08, 'x', Items::w1), y - Graphics::prcnt(0.09, 'y', Items::w1),
						1.0);
				}
				else if (maBuy == true){
					int i, j;
					SDL_GetWindowSize(Items::w1, &i, &j);
					masYStore = ((double)y) / j;
					semYLocation = masYStore + scrlPrcnt * yClip;
					std::cout << semYLocation << "\n";
					if (semYLocation > 1.15) {
						masYStore = masYStore + (1.15 - semYLocation);
						std::cout << masYStore << " YStoreOFF\n";
					}
					else std::cout << masYStore << " YStoreON\n";

					Items::animationLibrary.masAni->setLocations(Graphics::prcnt(0.0, 'x', Items::w1), Graphics::prcnt(masYStore, 'x', Items::w1));
					maAni = true;
				}
			}
			else if (moAni && hoverMo) {
				moAni = false;
				Items::mio->resetDistanceCounter();
			}
			else if (feAni && hoverMo) {
				feAni = false;
				Items::animationLibrary.felAni->currentPosition = 0;
			}
			else if (feSel && hoverMo) {
				feAni = true;
			}
			else if (moSel && hoverMo) {
				moAni = true;
			}
			else if (haAni && hoverHa) {
				haAni = false;
				Items::mio->resetDistanceCounter();
			}
			else if (haSel && hoverHa) {
				haAni = true;
			}
			else if (fiAni && hoverFi) {
				fiAni = false;
				Items::mio->resetDistanceCounter();
			}
			else if (fiSel && hoverFi) {
				fiAni = true;
			}
			else if (lPinchAni && hoverLPinch) {
				lPinchAni = false;
				Items::mio->resetDistanceCounter();
			}
			else if (piSel && hoverLPinch) {
				lPinchAni = true;
			}
			else if (rPinchAni && hoverRPinch) {
				rPinchAni = false;
				Items::mio->resetDistanceCounter();
			}
			else if (piSel && hoverRPinch) {
				rPinchAni = true;
			}
			else if (brAni || noBrAni && hoverBre) {
				noBrAni = false;
				brAni = false;
			}
			else if (brSel && hoverBre && shirtOn) {
				noBrAni = true;
			}
			else if (brSel && hoverBre && !shirtOn) {
				brAni = true;
			}
			else if (thAni && hoverThi) {
				thAni = false;
			}
			else if (thSel && hoverThi) {
				thAni = true;
			}
			else {
				if (tab == 1 || tab == 5) {
					Items::animationLibrary.pokeFake->assignFrameToLocation(x - Graphics::prcnt(0.15, 'x', Items::w1), y - Graphics::prcnt(0.00, 'y', Items::w1));
					Items::animationLibrary.pokeFake->assignedFrameCopies.back()->setMovementPattern(
						x - Graphics::prcnt(0.15, 'x', Items::w1), y - Graphics::prcnt(0.00, 'y', Items::w1),
						x - Graphics::prcnt(0.135, 'x', Items::w1), y - Graphics::prcnt(0.015, 'y', Items::w1),
						1.0);

				}
				else if (tab == 2 || tab == 4) {
					Items::animationLibrary.handFake->assignFrameToLocation(x - Graphics::prcnt(0.08, 'x', Items::w1), y - Graphics::prcnt(0.09, 'y', Items::w1));
					Items::animationLibrary.handFake->assignedFrameCopies.back()->setMovementPattern(
						x - Graphics::prcnt(0.08, 'x', Items::w1), y - Graphics::prcnt(0.09, 'y', Items::w1),
						x - Graphics::prcnt(0.08, 'x', Items::w1), y - Graphics::prcnt(0.09, 'y', Items::w1),
						1.0);
				}
				else if (maBuy == true){
					int i, j;
					SDL_GetWindowSize(Items::w1, &i, &j);
					masYStore = ((double)y) / j;
					semYLocation = masYStore + scrlPrcnt * yClip;
					std::cout << semYLocation << "\n";
					if (semYLocation > 1.15) {
						masYStore = masYStore + (1.15 - semYLocation);
						std::cout << masYStore << " YStoreOFF\n";
					}
					else std::cout << masYStore << " YStoreON\n";

					Items::animationLibrary.masAni->setLocations(Graphics::prcnt(0.0, 'x', Items::w1), Graphics::prcnt(masYStore, 'x', Items::w1));
					maAni = true;
				}
			}
		}

		moSel = moBuy && !feSel; feSel = feBuy && !moSel;
		piSel = niBuy && !brSel; brSel = brBuy && !piSel;
		haSel = haBuy; fiSel = fiBuy;
		maSel = maBuy;
		thSel = thBuy;

	//	std::cout << hoverHa << "\n";

	}
#include <iterator>
	int maxRand;
	bool evntHappening;
	int now;
	int timer;
	bool moSFX, feSFX, fiSFX, masSFX, moanSFX, moanOn;
	bool heartSlow, heartMed, heartFast, fail;
	bool heartStopped = true;
	void checkChannelDone(int i) {
		heartStopped = true;
	}
	void play() {
		if (awareCounter > 10000) {
			Mix_FadeOutChannel(2, 500);
			Mix_FadeOutChannel(3, 500);
			if (!fail) {
				Items::sfxLibrary.failSFX->playSFX();
				fail = true;
			}
			heartSlow = false;
			heartMed = false;
			heartFast = false;
			heartStopped = false;
		}
		else if (awareCounter > 9500) {
			//if (!heartFast) {
				//Mix_FadeOutChannel(2, 50);
				Mix_ChannelFinished(checkChannelDone);
				if (heartStopped) {
					Items::sfxLibrary.heartFastSFX->playSFX(2, 0);
					heartFast = true;
					heartMed = false;
					heartSlow = false;
					heartStopped = false;
				}
			//}		
		}
		else if (awareCounter > 9000) {
			//if (!heartMed) {
				//Mix_FadeOutChannel(2, 50);
				Mix_ChannelFinished(checkChannelDone);
				if (heartStopped) {
					Items::sfxLibrary.heartMediumSFX->playSFX(2, 0);
					heartFast = false;
					heartMed = true;
					heartSlow = false;
					heartStopped = false;
				}
			//}
		}
		else if (awareCounter > 8000) {
		//	if (!heartSlow) {
				//Mix_FadeOutChannel(2, 50);
				Mix_ChannelFinished(checkChannelDone);
				if (heartStopped) {
					Items::sfxLibrary.heartSlowSFX->playSFX(2, 0);
					heartFast = false;
					heartMed = false;
					heartSlow = true;
					heartStopped = false;
				}
			//}
		}
		else {
			Mix_FadeOutChannel(2, 1000);		
			heartSlow = false;
			heartMed = false;
			heartFast = false;
			fail = false;
		}

		if (!neutralSFX) {
			Items::sfxLibrary.neutSFX->playSFX(3, -1);
			neutralSFX = true;
		}
		if (lPinchAni || rPinchAni || brAni || noBrAni) {
			moanOn = true;
			Mix_FadeOutChannel(3, 100);
		}
		if (moAni) {
			moanOn = true;
			if (!moSFX) {
				moSFX = true;
				Mix_FadeOutChannel(3, 100);
				Items::sfxLibrary.moSFX->playSFX(4,-1);
			}
		}
		else if (feAni) {
			moanOn = true;
			if (!feSFX) {
				feSFX = true;
				Mix_FadeOutChannel(3, 100);
				Items::sfxLibrary.feSFX->playSFX(4,-1);
			}
		}
		else {
			if (moSFX || feSFX) Mix_FadeOutChannel(4,100);
			moSFX = false;
			feSFX = false;
		}
		if (fiAni) {
			moanOn = true;
			if (!fiSFX) {
				fiSFX = true;
				Mix_FadeOutChannel(3, 100);
				Items::sfxLibrary.fiSFX->playSFX(5, -1);
			}
		}
		else {
			if(fiSFX) Mix_FadeOutChannel(5, 100);
			fiSFX = false;
		}
		if (maAni) {
			//moanOn = true;
			if (!masSFX) {
				masSFX = true;
				Mix_FadeOutChannel(3, 100);
				Items::sfxLibrary.masSFX->playSFX(6, -1);
			}
		}
		else {
			if (masSFX) Mix_FadeOutChannel(6, 100);
			masSFX = false;
		}
		if (moanOn) {
			moanOn = false;
			if (!moanSFX) {
				moanSFX = true;
				Items::sfxLibrary.moan1SFX->playSFX(7, -1);
			}
		}
		else {
			if (moanSFX) {
				Mix_FadeOutChannel(7, 100);
				neutralSFX = false;
			}
			moanSFX = false;
		}

		if (Prompts::promptTab) {
			Items::sfxLibrary.SelectionSFX2->playSFX();
			Prompts::promptTab = false;
		}

		if (levelSound && !levelSoundHold) {
			levelSoundHold = true;
			Items::sfxLibrary.pointsSFX->playSFX();
		}
		if (errorSound) {
			errorSound = false;
			Items::sfxLibrary.errorSFX->playSFX();
		}
		else if (LCLICK) {
			if ((hoverSel1 || hoverSel2 || hoverSel3) && tab < 4) {
				Items::sfxLibrary.SelectionSFX->playSFX();
			}
			else if (tab == 4) {
				if (hoverSel1) {
					if (blSel)
						Items::sfxLibrary.DerobeSFX->playSFX();
					else
						Items::sfxLibrary.RobeSFX->playSFX();
				}
				else if (hoverSel2) {
					if (tieSel)
						Items::sfxLibrary.DerobeSFX->playSFX();
					else
						Items::sfxLibrary.RobeSFX->playSFX();
				}
				else if (hoverSel3) {
					if (shirtSel)
						Items::sfxLibrary.DerobeSFX->playSFX();
					else
						Items::sfxLibrary.RobeSFX->playSFX();
				}
			}
			else if (tab == 5) {
				if (hoverSel1) {
					if (skSel)
						Items::sfxLibrary.DerobeSFX->playSFX();
					else
						Items::sfxLibrary.RobeSFX->playSFX();
				}
				else if (hoverSel2) {
					if (paSel)
						Items::sfxLibrary.DerobeSFX->playSFX();
					else
						Items::sfxLibrary.RobeSFX->playSFX();
				}
				else if (hoverSel3) {
					if (thiSel)
						Items::sfxLibrary.DerobeSFX->playSFX();
					else
						Items::sfxLibrary.RobeSFX->playSFX();
				}
			}
			if (hoverR || hoverL|| hoverUDL || hoverUDR) {
				Items::sfxLibrary.OpenSFX->playSFX();
			}
			else if (hoverCHCl3 || hoverInce) {
				Items::sfxLibrary.SelectionSFX2->playSFX();
			}
		}
		else if (Items::inputs->at(KEYPRESSED) && !Items::inputs->at(KEYHELD)) {
			if (true) {
				if (Items::inputs->at(NUM1) && !Items::inputs->at(NUM1_H)) {
					Items::sfxLibrary.SelectionSFX->playSFX();
				}
				else if (Items::inputs->at(NUM2) && !Items::inputs->at(NUM2_H)) {
					if (niBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}

				if (Items::inputs->at(NUM3) && !Items::inputs->at(NUM3_H)) {
					if (fiBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}
				else if (Items::inputs->at(NUM4) && !Items::inputs->at(NUM4_H)) {
					if (haBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}
				else if (Items::inputs->at(NUM5) && !Items::inputs->at(NUM5_H)) {
					if (thBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}
				else if (Items::inputs->at(NUM6) && !Items::inputs->at(NUM6_H)) {
					if (brBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}
				else if (Items::inputs->at(NUM7) && !Items::inputs->at(NUM7_H)) {
					if (maBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}
				else if (Items::inputs->at(NUM8) && !Items::inputs->at(NUM8_H)) {
					if (feBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}
				else if (Items::inputs->at(NUM9) && !Items::inputs->at(NUM9_H)) {
					if (xxBuy) {
						Items::sfxLibrary.SelectionSFX->playSFX();
					}
				}
				else if (Items::inputs->at(KEYQ) && !Items::inputs->at(KEYQ_H)) {
					if (blBuy) {
						if (blOn) 
							Items::sfxLibrary.DerobeSFX->playSFX();
						else
							Items::sfxLibrary.RobeSFX->playSFX();
					}
				}
				if (Items::inputs->at(KEYW) && !Items::inputs->at(KEYW_H)) {
					if (tieBuy) {
						if (tieOn) 
							Items::sfxLibrary.DerobeSFX->playSFX();
						else
							Items::sfxLibrary.RobeSFX->playSFX();
					}
				}
				else if (Items::inputs->at(KEYE) && !Items::inputs->at(KEYE_H)) {
					if (shirtBuy) {
						if (shirtOn)	
							Items::sfxLibrary.DerobeSFX->playSFX();
						else
							Items::sfxLibrary.RobeSFX->playSFX();
					}
				}
				else if (Items::inputs->at(KEYR) && !Items::inputs->at(KEYR_H)) {
					if (skBuy) {
						if (skOn)		
							Items::sfxLibrary.DerobeSFX->playSFX();
						else
							Items::sfxLibrary.RobeSFX->playSFX();
					}
				}
				else if (Items::inputs->at(KEYT) && !Items::inputs->at(KEYT_H)) {
					if (paBuy) {
						if (paOn)		
							Items::sfxLibrary.DerobeSFX->playSFX();
						else
							Items::sfxLibrary.RobeSFX->playSFX();
					}
				}
				else if (Items::inputs->at(KEYY) && !Items::inputs->at(KEYY_H)) {
					if (thiBuy) {
						if (thiOn)	
							Items::sfxLibrary.DerobeSFX->playSFX();
						else
							Items::sfxLibrary.RobeSFX->playSFX();
					}
				}
			}
		}

		int diceRoll = Items::distribution(Items::generator);

		if (diceRoll < sfxCutOff && !evntHappening && start) {
		//	std::cout << "\n\n" << SDL_GetTicks() / 1000 << " seconds: " << diceRoll << "\n\n";
			if (diceRoll < bangCutoff_1)
				Items::sfxLibrary.bangSFX->playSFX();
			else if (diceRoll < commsCutoff_2 )
				Items::sfxLibrary.IntercomSFX->playSFX();
			else if (diceRoll < hornCutoff_3)
				Items::sfxLibrary.HornSFX->playSFX();
			else if (diceRoll < passingCutoff_4)
				Items::sfxLibrary.PassingSFX->playSFX();
			//else if (diceRoll < lightningCutoff_5)
			//	Items::sfxLibrary.LightningSFX->playSFX();

			evntHappening = true;
			now = SDL_GetTicks();
		}
		if (evntHappening && (timer - now) > 15000) {
			evntHappening = false;
		}
	}
	int boxAlphaR = 120;

	int sub;
	int tStart;
	int tRepeat;
	int xv, yv;
	bool staydark;

	int rate = 10;
	bool pinch;
	int eyeOpenStart;
	int time;
	int eyeOpenTime = 1500;
	void render() {

		SDL_RenderClear(Items::r1);
		SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);

		timer = SDL_GetTicks();
		if (!start) {
			//std::cout << timer - startTime << " " << timer << " " << startTime << "\n";
			//sub = timer - startTime;
			if (sub < 3000)
				Items::gameGraphics.TextBG->renderTextureHScrolling(-5, 3, 0, Graphics::prcnt(-0.2, 'y', Items::w1));
			if ((sub) > 7000)
				start = Items::gameGraphics.TextWall->renderTextureFadeOut(1000);
			else
				start = Items::gameGraphics.TextWall->renderTextureScaleScrolling(-15, 2, ScreenWidth, Graphics::prcnt(-0.6, 'y', Items::w1), 10);
		}
		else {
			Items::gameGraphics.TextLights->renderTextureScaleScrollingLoop(-10, 3, 0, 0.002, ScreenWidth, Graphics::prcnt(-0.80, 'y', Items::w1) - Graphics::prcnt((float)yClip * 0.05, 'y', Items::w1), 2, &growRect);
		}

		Items::gameGraphics.MainBG->renderTextureClip(
			0, (float)yClip / 10,
			1.0, 1.0);
		//BODY
		Items::gameGraphics.CharacterBody->renderTextureClip(
			0, (float)yClip / 10,
			1.0, 1.0);
		//BREASTS
		//std::cout << lPinchAni << "lpia\n";
		if (!noBrAni)
			Items::gameGraphics.CharacterTorso->renderTexture(0.000, -scrlPrcnt * yClip);
		if(!lPinchAni && !brAni && !noBrAni)
			Items::gameGraphics.CharacterLBreast->renderTexture(0.000, -scrlPrcnt * yClip);
		if(!rPinchAni && !brAni && !noBrAni)
			Items::gameGraphics.CharacterRBreast->renderTexture(0.000, -scrlPrcnt * yClip);


		//FACE
		if (openEyes) {
			Items::animationLibrary.eyesAni->frames.at(4)->renderTexture();
			time = SDL_GetTicks();
		}
		if (awareCounter > 10000 || (Items::gameMode == 3 && (countdownTimer - timerStart) > deadlineTime)) {
			if (!openEyes && Items::animationLibrary.eyesAni->itterateOnce(30)) {
				openEyes = true;
				eyeOpenStart = SDL_GetTicks();
			}
		}
		else Items::animationLibrary.idleAni->itterateAnimation(24);


		//SDL_RenderDrawRect(Items::r1, &Items::animationLibrary.idleAni->frames.at(0)->box);

		//used to keep points cycling when overtime
		if (Items::gameMode != 4)
			if(awareCounter <= 10000)
				calculatePoints(0, 0, false);
		else{
			//calculatePoints(0, 0, falseac);
		}
		
		//CLOTHING
		if (true) {
			//LEG
			Items::gameGraphics.CharacterLeg->renderTexture(0.000, -scrlPrcnt * yClip);
			//THH
			if (thiOn) {
				if (thiFade < 255) {
					thiFade += rate;
					if (thiFade > 255) thiFade = 255;
				}
				Items::gameGraphics.thh->setTransparency(thiFade);
				Items::gameGraphics.thh->renderTexture(0.0000, -scrlPrcnt * yClip);
			}
			else {
				if (thiFade > 0) {
					thiFade -= rate;
					if (thiFade < 0) thiFade = 0;
				}
				Items::gameGraphics.thh->setTransparency(thiFade);
				Items::gameGraphics.thh->renderTexture(0.0000, -scrlPrcnt * yClip);
			}
			if (semLegActive) {
				Items::gameGraphics.semLeg->renderTextureFadeIn(1000);
			}
			if (paOn) {
				if (paFade < 255) {
					paFade += rate;
					if (paFade > 255) paFade = 255;
				}
				if (moisFade > 0) {
					moisFade -= rate;
					if (moisFade  < 0) moisFade = 0;
				}
				Items::gameGraphics.pa->setTransparency(paFade);
				Items::gameGraphics.pa->renderTexture(0.0, -scrlPrcnt * yClip);
				Items::gameGraphics.paMois->setTransparency(paFade * ((double) moisAlpha / 255.0));
				Items::gameGraphics.paMois->renderTexture(0.0, -scrlPrcnt * yClip);
				Items::gameGraphics.mois->setTransparency(moisFade * ((double)moisAlpha / 255.0));
				Items::gameGraphics.mois->renderTexture(0.0, -scrlPrcnt * yClip);
			}
			else {
				if (paFade > 0) {
					paFade -= rate;
					if (paFade < 0) paFade = 0;
					mFad = abs(moisAlpha / (paFade - 254));
				}
				if (moisFade < 255) {
					moisFade += rate;
					if (moisFade > 255) moisFade = 255;
				}
				Items::gameGraphics.pa->setTransparency(paFade);
				Items::gameGraphics.pa->renderTexture(0.0, -scrlPrcnt * yClip);
				Items::gameGraphics.paMois->setTransparency(paFade * ((double)moisAlpha / 255.0));
				Items::gameGraphics.paMois->renderTexture(0.0, -scrlPrcnt * yClip);
				Items::gameGraphics.mois->setTransparency(moisFade * ((double)moisAlpha / 255.0));
				Items::gameGraphics.mois->renderTexture(0.0, -scrlPrcnt * yClip);
			}
			if (skOn) {
				if (skFade < 255) {
					skFade += rate;
					if (skFade > 255) skFade = 255;
				}
				Items::gameGraphics.skirt->setTransparency(skFade);
				Items::gameGraphics.skirt->renderTexture(0.0, -scrlPrcnt * yClip);
			}
			else {
				if (skFade > 0) {
					skFade -= rate;
					if (skFade < 0) skFade = 0;
				}
				Items::gameGraphics.skirt->setTransparency(skFade);
				Items::gameGraphics.skirt->renderTexture(0.0, -scrlPrcnt * yClip);
			}
			//Breasts open

			if (brAni) {
				if (semBrActive) {
					Items::gameGraphics.semBre->renderTextureFadeIn(1000);
				}
				bool frameMove = Items::animationLibrary.noBreAni->itterateAnimation(35);
				if (Items::gameMode != 4)
					calculatePoints(3000, 8200, frameMove);
				else
					calculatePoints(1, 2, frameMove);
				//Use method to calculate points
				moveMo = false;
				aRadius = 0.10;
				moHitBoxX = Graphics::prcnt(0.6, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.440, 'y', Items::w1) - yClip * scrollRate;
			}
			else if(false){
			}
			//breclosed
			else if (noBrAni) {
				if (semBrActive) {
					Items::gameGraphics.semBre->renderTextureFadeIn(1000);
				}
				bool frameMove = Items::animationLibrary.breAni->itterateAnimation(35);
				if (Items::gameMode != 4)
					calculatePoints(4000, 2000, frameMove);
				else
					calculatePoints(1, 2, frameMove);
				//Use method to calculate points
				moveMo = false;
				aRadius = 0.10;
				moHitBoxX = Graphics::prcnt(0.6, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.440, 'y', Items::w1) - yClip * scrollRate;
				}
				else if (false) {
				}
				else {
					if (shirtOn) {
					if (blOn) {
						if (shirtFade < 255) {
							shirtFade += rate;
							if (shirtFade > 255) shirtFade = 255;
						}
						Items::gameGraphics.shirt->setTransparency(shirtFade);
						Items::gameGraphics.shirt->renderTextureClip(0.000, (double)-scrlPrcnt * yClip, 0.0, 0.0, 0.76, 1.0);
					}
					else {
						if (shirtFade < 255) {
							shirtFade += rate;
							if (shirtFade > 255) shirtFade = 255;
						}
						Items::gameGraphics.shirt->setTransparency(shirtFade);
						Items::gameGraphics.shirt->renderTexture(0.000, -scrlPrcnt * yClip);
					}
				}
				else {
				if (blOn) {
					if (shirtFade > 0) {
						shirtFade -= rate;
						if (shirtFade < 0) shirtFade = 0;
					}
					Items::gameGraphics.shirt->setTransparency(shirtFade);
					Items::gameGraphics.shirt->renderTextureClip(0.000, (double)-scrlPrcnt * yClip, 0.0, 0.0, 0.76, 1.0);
				}
				else {
					if (shirtFade > 0) {
						shirtFade -= rate;
						if (shirtFade < 0) shirtFade = 0;
					}
					Items::gameGraphics.shirt->setTransparency(shirtFade);
					Items::gameGraphics.shirt->renderTexture(0.000, -scrlPrcnt * yClip);
				}
			}
				if (tieOn) {
					if (!shirtOn) {
						if (tieTopFade < 255) {
							tieTopFade += rate;
							if (tieTopFade > 255) tieTopFade = 255;
						}
						Items::gameGraphics.tieTop->setTransparency(tieTopFade);
						Items::gameGraphics.tieTop->renderTexture(0.0000, -scrlPrcnt * yClip);
					}
					else {
						if (tieTopFade > 0) {
							tieTopFade -= rate;
							if (tieTopFade < 0) tieTopFade = 0;
						}
						Items::gameGraphics.tieTop->setTransparency(tieTopFade);
						Items::gameGraphics.tieTop->renderTexture(0.0000, -scrlPrcnt * yClip);
					}

					if (tieFade < 255) {
						tieFade += rate;
						if (tieFade > 255) tieFade = 255;
					}
					Items::gameGraphics.tie->setTransparency(tieFade);
					Items::gameGraphics.tie->renderTexture(0.0000, -scrlPrcnt * yClip);
				}
				else {
					if (tieTopFade > 0) {
						tieTopFade -= rate;
						if (tieTopFade < 0) tieTopFade = 0;
					}
					Items::gameGraphics.tieTop->setTransparency(tieTopFade);
					Items::gameGraphics.tieTop->renderTexture(0.0000, -scrlPrcnt * yClip);

					if (tieFade > 0) {
						tieFade -= rate;
						if (tieFade < 0) tieFade = 0;
					}
					Items::gameGraphics.tie->setTransparency(tieFade);
					Items::gameGraphics.tie->renderTexture(0.0000, -scrlPrcnt * yClip);

				}
				//ANIS
				if (blOn) {
					if (blFade < 255) {
						blFade += rate;
						if (blFade > 255) blFade = 255;
					}
					Items::gameGraphics.bla->setTransparency(blFade);
					Items::gameGraphics.bla->renderTexture(0.0, -scrlPrcnt * yClip);
				}
				else {
					if (blFade > 0) {
						blFade -= rate;
						if (blFade < 0) blFade = 0;
					}
					Items::gameGraphics.bla->setTransparency(blFade);
					Items::gameGraphics.bla->renderTexture(0.0, -scrlPrcnt * yClip);
				}
				if (semBrActive) {
					Items::gameGraphics.semBre->renderTextureFadeIn(1000);
				}
			}
	


			if (true) {
				//LNI
				pinch = false;
				if (lPinchAni && !shirtOn && !blOn) {
					if (LHELD && hoverLPinch) {
						Items::mio->trackDestopMouse(&x, &y);

						bool frameMove = Items::animationLibrary.lPiAni->itterateAnimation(Items::mio->calculateMouseDistance(x, y, 10), 0, 1);
						pinch = true;
						moveLPi = true;
						if(Items::gameMode != 4)
							calculatePoints(4000, 8200, frameMove);
						else
							calculatePoints(1, 2, frameMove);
						//Use method to calculate points

						aRadius = 1.5;
						lpHitBoxX = Graphics::prcnt(0.0, 'x', Items::w1); lpHitBoxY = Graphics::prcnt(0.0, 'y', Items::w1);
						Items::mio->trackGameMouse(&x, &y);
					}
					else {
						bool frameMove = Items::animationLibrary.lPiAni->itterateAnimation(35);
						//Items::animationLibrary.lPiAni->frames.at(0)->renderTexture();
						if (Items::gameMode != 4)
							calculatePoints(4000, 8200, frameMove);
						else
							calculatePoints(1, 2, frameMove);
						//Use method to calculate points
						pinch = true;
						moveLPi = false;
						aRadius = 0.10;
						lpHitBoxX = Graphics::prcnt(0.32, 'x', Items::w1); lpHitBoxY = Graphics::prcnt(0.814, 'y', Items::w1) - yClip * scrollRate;
					}
				}
				else {
					moveLPi = false;
					aRadius = 0.10;;
					Items::animationLibrary.lPiAni->currentPosition = 0;
					lpHitBoxX = Graphics::prcnt(0.32, 'x', Items::w1); lpHitBoxY = Graphics::prcnt(0.814, 'y', Items::w1) - yClip * scrollRate;
				}

				//RNI
				if (rPinchAni && !shirtOn && !blOn) {
					if (LHELD && hoverRPinch) {
						Items::mio->trackDestopMouse(&x, &y);
						bool frameMove = Items::animationLibrary.rPiAni->itterateAnimation(Items::mio->calculateMouseDistance(x, y, 10), 0, 1);
						if (Items::gameMode != 4)
							calculatePoints(4000, 8000, frameMove);
						else
							calculatePoints(1, 2, frameMove);
						//Use method to calculate points
						pinch = true;
						moveRPi = true;

						aRadius = 1.5;
						rpHitBoxX = Graphics::prcnt(0.0, 'x', Items::w1); rpHitBoxY = Graphics::prcnt(0.0, 'y', Items::w1);
						Items::mio->trackGameMouse(&x, &y);
					}
					else {
						bool frameMove = Items::animationLibrary.rPiAni->itterateAnimation(35);
						//Items::animationLibrary.rPiAni->frames.at(0)->renderTexture();
						if (Items::gameMode != 4)
							calculatePoints(4000, 8000, frameMove);
						else
							calculatePoints(1, 2, frameMove);
						//Use method to calculate points
						pinch = true;
						moveRPi = false;

						aRadius = 0.10;
						rpHitBoxX = Graphics::prcnt(0.53, 'x', Items::w1); rpHitBoxY = Graphics::prcnt(0.852, 'y', Items::w1) - yClip * scrollRate;
					}
				}
				else {
					moveRPi = false;
					aRadius = 0.10;;
					rpHitBoxX = Graphics::prcnt(0.53, 'x', Items::w1); rpHitBoxY = Graphics::prcnt(0.852, 'y', Items::w1) - yClip * scrollRate;
					Items::animationLibrary.rPiAni->currentPosition = 0;
				}
				//FI
				if (fiAni && !skOn) {
					if (LHELD && hoverFi) {
						Items::mio->trackDestopMouse(&x, &y);
						moveFi = true;
						bool frameMove = Items::animationLibrary.fiAni->itterateAnimation(Items::mio->calculateMouseDistance(x, y, 10), 0, 1);
						if (Items::gameMode != 4)
							calculatePoints(3000, 8000, frameMove);
						else
							calculatePoints(1, 2, frameMove);
						//Use method to calculate points

						if (moisAlpha < 255) {
							moisAlpha++;
						}

						aRadius = 1.5;
						fiHitBoxX = Graphics::prcnt(0.0, 'x', Items::w1); fiHitBoxY = Graphics::prcnt(0.0, 'y', Items::w1);
						Items::mio->trackGameMouse(&x, &y);
					}
					else {
						bool frameMove = Items::animationLibrary.fiAni->itterateAnimation(35);
						if (Items::gameMode != 4)
							calculatePoints(3000, 8000, frameMove);
						else
							calculatePoints(1, 2, frameMove);
						//Use method to calculate points

						if (moisAlpha < 255) {
							moisAlpha++;
						}

						moveFi = false;
						aRadius = 0.10;
						fiHitBoxX = Graphics::prcnt(0.4, 'x', Items::w1); fiHitBoxY = Graphics::prcnt(1.15, 'y', Items::w1) - yClip * scrollRate;
					}
					Items::gameGraphics.CharacterLeg->renderTexture(0.000, -scrlPrcnt * yClip);
					if (thiOn) {
						if (thiFade < 255) {
							thiFade += rate;
							if (thiFade > 255) thiFade = 255;
						}
						Items::gameGraphics.thh->setTransparency(thiFade);
						Items::gameGraphics.thh->renderTexture(0.0000, -scrlPrcnt * yClip);
						if (semLegActive) {
							Items::gameGraphics.semLeg->renderTextureFadeIn(1000);
						}
					}
					else {
						if (thiFade > 0) {
							thiFade -= rate;
							if (thiFade < 0) thiFade = 0;
						}
						Items::gameGraphics.thh->setTransparency(thiFade);
						Items::gameGraphics.thh->renderTexture(0.0000, -scrlPrcnt * yClip);
					}
				}
				else {
					moveFi = false;
					aRadius = 0.10;;
					fiHitBoxX = Graphics::prcnt(0.4, 'x', Items::w1); fiHitBoxY = Graphics::prcnt(1.15, 'y', Items::w1) - yClip * scrollRate;
				}
			}
		}
		//HAIR&MOUTH&&POKES
		if (true) {
			if (thAni) {
				bool frameMove = Items::animationLibrary.thiAni->itterateAnimation(35);
				if (Items::gameMode != 4)
					calculatePoints(6000, 4000, frameMove);
				else
					calculatePoints(1, 2, frameMove);
				//Use method to calculate points
				moveMo = false;
				aRadius = 0.10;
				moHitBoxX = Graphics::prcnt(0.6, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.440, 'y', Items::w1) - yClip * scrollRate;
			}
			else {
			}

			//HAIR  L half
			Items::gameGraphics.CharacterHair->renderTexture(0.0, -yClip * scrlPrcnt);

			if (semHaActive) {
				Items::gameGraphics.semHair->renderTextureFadeIn(1000);
			}
			if (semStoActive) {
				Items::gameGraphics.semSto->renderTextureFadeIn(1000);
			}
			if (semMoActive) {
				Items::gameGraphics.semMo->renderTextureFadeIn(1000);
			}
			if (feAni) {	
				bool frameMove = Items::animationLibrary.felAni->itterateAnimation(35);
				if (Items::animationLibrary.felAni->currentPosition > 94) {
					if (!semMoActive) { 
						expPoints += 50; 
						if(Items::gameMode != 1)
							Items::sfxLibrary.pointsSFX->playSFX();
					}
					semMoActive = true;
				}
				if (Items::gameMode != 4)
					calculatePoints(4000, 8000, frameMove);
				else
					calculatePoints(1, 2, frameMove);
				//Use method to calculate points
				moveMo = false;
				aRadius = 0.10;
				moHitBoxX = Graphics::prcnt(0.6, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.440, 'y', Items::w1) - yClip * scrollRate;
			}
			//MOUTH
			else if (moAni) {
				if (LHELD && hoverMo) {
					Items::mio->trackDestopMouse(&x, &y);

					bool frameMove = Items::animationLibrary.moAni->itterateAnimation(Items::mio->calculateMouseDistance(x, y, 10), 0, 1);
					if (Items::gameMode != 4)
						calculatePoints(8000, 5000, frameMove);
					else
						calculatePoints(1, 2, frameMove);
					//Use method to calculate points
					moveMo = true;
					aRadius = 1.5;
					//moHitBoxX = Graphics::prcnt(0.0, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.0, 'y', Items::w1);
					Items::mio->trackGameMouse(&x, &y);
				}
				else {
					bool frameMove = Items::animationLibrary.moAni->itterateAnimation(35);
					if (Items::gameMode != 4)
						calculatePoints(8000, 5000, frameMove);
					else
						calculatePoints(1, 2, frameMove);
					//Use method to calculate points
					moveMo = false;
					aRadius = 0.10;
					moHitBoxX = Graphics::prcnt(0.6, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.440, 'y', Items::w1) - yClip * scrollRate;
				}
			}
			else {
				aRadius = 0.10;;
				moveMo = false;
				moHitBoxX = Graphics::prcnt(0.6, 'x', Items::w1); moHitBoxY = Graphics::prcnt(0.440, 'y', Items::w1) - yClip * scrollRate;
				Items::animationLibrary.moAni->currentPosition = 0;
			}
			bool hair = false;
			//Hair r half
			if (haAni) {
				if (LHELD && hoverHa) {
					Items::mio->trackDestopMouse(&x, &y);
					hair = true;
					moveHa = true;
					bool frameMove = Items::animationLibrary.haAni->itterateAnimation(Items::mio->calculateMouseDistance(x, y, 10), 0, 1, 1, 48);
					calculatePoints(8000, 8000, frameMove);
					//Use method to calculate points
					Items::mio->trackGameMouse(&x, &y);
				}
				else {
					bool frameMove = Items::animationLibrary.haAni->itterateAnimationStartEnd(35, 1, 48);
					calculatePoints(8000, 8000, frameMove);
					//Use method to calculate points
					hair = true;
					moveHa = false;
					//Items::animationLibrary.haAni->frames.at(0)->renderTexture();

					aRadius = 0.10;
				}
			}
			else {
				aRadius = 0.10;
				moveHa = false;
				Items::animationLibrary.haAni->currentPosition = 0;
			}
			if (!hair) {
				Items::animationLibrary.haAni->frames.at(0)->renderTexture();
			}
			for each(Graphics* g in Items::animationLibrary.pokeFake->assignedFrameCopies) {
				bool frameMove = g->singleFrameAnimate(35);
				g->renderSingleFrame();
				if (
					GenericGameTools::checkRectHitBox(torso, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(head, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(lArm, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(rArm, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(pelvis, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(lLeg, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(rLeg, g->box.x + g->box.w, g->box.y))
					if (Items::gameMode != 4)
						calculatePoints(10000, 7000, frameMove);
					else
						calculatePoints(1, 2, frameMove);
#ifdef HBOX
				SDL_RenderFillRect(Items::r1, &g->box);
#endif // HBOX	
			}
			for each(Graphics* g in Items::animationLibrary.handFake->assignedFrameCopies) {
				bool frameMove = g->singleFrameAnimate(35);
				g->singleFrameRotate(35, 1);
				g->renderSingleFrame();
				if (
					GenericGameTools::checkRectHitBox(torso, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(head, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(lArm, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(rArm, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(pelvis, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(lLeg, g->box.x + g->box.w, g->box.y) ||
					GenericGameTools::checkRectHitBox(rLeg, g->box.x + g->box.w, g->box.y))
					if (Items::gameMode != 4)
						calculatePoints(10000, 7000, frameMove);
					else
						calculatePoints(1, 2, frameMove);
#ifdef HBOX
				SDL_RenderFillRect(Items::r1, &g->box);
#endif // HBOX	
			}
			if (maAni) {
				Items::animationLibrary.masAni->itterateAnimation(35);
				if (Items::animationLibrary.masAni->currentPosition > 98) {
					if (semYLocation < 0.55) { 
						if (!semHaActive) {
							expPoints += 50;
							if (Items::gameMode != 1)
								Items::sfxLibrary.pointsSFX->playSFX();
						}
						semHaActive = true; 
					}
					else if (semYLocation < 0.86) {
						if (!semBrActive){ 
							expPoints += 50;
							if (Items::gameMode != 1)
								Items::sfxLibrary.pointsSFX->playSFX();
						}
						semBrActive = true;
					}
					else if (semYLocation < 1.05) {
						if (!semStoActive) {
							expPoints += 50;
							if (Items::gameMode != 1)
								Items::sfxLibrary.pointsSFX->playSFX();
						}
						semStoActive = true; 			
					}
					else { 
						if (!semLegActive) {
							expPoints += 50;
							if (Items::gameMode != 1)
								Items::sfxLibrary.pointsSFX->playSFX();
						}
						semLegActive = true;
					}
				}
			}	
		}
	
		if (LHELD && (haAni || moAni || fiAni || pinch || fiAni )) {
			aRadius = 1.5;
		}
		else	aRadius = 0.1;


#ifdef HBOX
		SDL_SetRenderDrawColor(Items::r1, 200, 0, 200, 180);
		SDL_Rect r1 = { moHitBoxX, moHitBoxY - yClip * scrollRate,-Graphics::prcnt(aRadius , 'x', Items::w1) , Graphics::prcnt(aRadius , 'y', Items::w1) };
		SDL_Rect r3 = { lpHitBoxX, lpHitBoxY - yClip * scrollRate,-Graphics::prcnt(aRadius , 'x', Items::w1) , Graphics::prcnt(aRadius , 'y', Items::w1) };
		SDL_Rect r4 = { rpHitBoxX, rpHitBoxY - yClip * scrollRate,-Graphics::prcnt(aRadius , 'x', Items::w1) , Graphics::prcnt(aRadius , 'y', Items::w1) };
		SDL_Rect r5 = { fiHitBoxX, fiHitBoxY - yClip * scrollRate,-Graphics::prcnt(aRadius , 'x', Items::w1) , Graphics::prcnt(aRadius , 'y', Items::w1) };
		SDL_Rect r6 = { thiHitBoxX, thiHitBoxY - yClip * scrollRate,-Graphics::prcnt(cRadius , 'x', Items::w1) , Graphics::prcnt(cRadius , 'y', Items::w1) };
		SDL_RenderFillRect(Items::r1, &r1);
		SDL_RenderFillRect(Items::r1, &haHitBox1);
		SDL_RenderFillRect(Items::r1, &haHitBox2);
		SDL_RenderFillRect(Items::r1, &haHitBox3);
		SDL_RenderFillRect(Items::r1, &haHitBox4);
		SDL_RenderFillRect(Items::r1, &breastHitBox);

		SDL_RenderFillRect(Items::r1, &r3);
		SDL_RenderFillRect(Items::r1, &r4);
		SDL_RenderFillRect(Items::r1, &r5);	
		SDL_SetRenderDrawColor(Items::r1, 200, 200, 0, 180);
		SDL_RenderFillRect(Items::r1, &r6);
#endif
		if (!ctrlClick) {
			if (start) {
				SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, GenericGameTools::incrementAtRate(0, 190, 500));
				SDL_Rect fullBox = SDL_Rect{ 0,0, ScreenWidth, ScreenHeight };
				SDL_RenderFillRect(Items::r1, &fullBox);
			}
		}
		else {
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, GenericGameTools::decrementAtRate(190, 500));
			SDL_Rect fullBox = SDL_Rect{ 0,0, ScreenWidth, ScreenHeight };
			SDL_RenderFillRect(Items::r1, &fullBox);
		}

		//Character->renderTextureClip(
		//0, (float)yClip / 10 - 0.0945454545,
		//1.0 , 1.0);
	//FPS = 120;
		int transpar = 60;
		if (lOpen) {
			Items::gameGraphics.BoxMainBGL->renderTexture();
			double moreX = 0.0015;
			double slot = (((double)Items::gameGraphics.selTabs->box.w - (Graphics::prcnt(moreX, 'x', Items::w1))) / 5 * (tab - 1) / ScreenWidth);
			double selHeight = 0.894;
			double xOffSetHighlight = 0.01;
			Items::gameGraphics.selTabs->renderTexture(xOffSetHighlight, 0.0);
			if (tab == 1) { Items::gameGraphics.selHighlight->renderTexture(slot + xOffSetHighlight + moreX, selHeight ); }
			else if (tab == 2) { Items::gameGraphics.selHighlight->renderTexture(slot + xOffSetHighlight + moreX, selHeight ); }
			else if (tab == 3) { Items::gameGraphics.selHighlight->renderTexture(slot + xOffSetHighlight + moreX, selHeight ); }
			else if (tab == 4) { Items::gameGraphics.selHighlight->renderTexture(slot + xOffSetHighlight + moreX, selHeight ); }
			else if (tab == 5) { Items::gameGraphics.selHighlight->renderTexture(slot + xOffSetHighlight + moreX, selHeight); }
			if (hoverUDL)
				Items::gameGraphics.UDArrowL->renderTextureSizeFlip(
					Items::gameGraphics.UDArrowL->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
					Items::gameGraphics.UDArrowL->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
					Items::gameGraphics.UDArrowL->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
					Items::gameGraphics.UDArrowL->box.h + Graphics::prcnt(0.01, 'y', Items::w1),
					SDL_FLIP_VERTICAL);
			else
				Items::gameGraphics.UDArrowL->renderTextureFlip(SDL_FLIP_VERTICAL);

			if (hoverR)
				Items::gameGraphics.RArrow->renderTextureSize(
					Items::gameGraphics.RArrow->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
					Items::gameGraphics.RArrow->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
					Items::gameGraphics.RArrow->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
					Items::gameGraphics.RArrow->box.h + Graphics::prcnt(0.01, 'y', Items::w1)
				);
			else
				Items::gameGraphics.RArrow->renderTexture();
			if (hoverL)
				Items::gameGraphics.RArrow->renderTextureSizeFlip(
					leftBox.x - Graphics::prcnt(0.005, 'x', Items::w1),
					leftBox.y - Graphics::prcnt(0.005, 'y', Items::w1),
					Items::gameGraphics.RArrow->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
					Items::gameGraphics.RArrow->box.h + Graphics::prcnt(0.01, 'y', Items::w1), SDL_FLIP_HORIZONTAL);
			else
				Items::gameGraphics.RArrow->renderTextureSizeFlip(leftBox.x, leftBox.y,
					Items::gameGraphics.RArrow->box.w, Items::gameGraphics.RArrow->box.h, SDL_FLIP_HORIZONTAL);

			if (tab == 1) {
				if (moSel)
					hoverSel1 = true;
				if (piSel)
					hoverSel2 = true;
				if (fiSel)
					hoverSel3 = true;

				if (hoverSel1) {
					Items::gameGraphics.BoxBGSel1->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.TextMouth->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);

					if (!moBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel1);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(moPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}

				else {
					Items::gameGraphics.BoxBGSel1->renderTexture(0.05, 0.27);
					Items::gameGraphics.TextMouth->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture();

					if (!moBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));(largeBoxSel1.x, largeBoxSel1.y);
						PriceWords->changeText(std::to_string(moPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#ifndef BETA
				if (hoverSel2) {
					Items::gameGraphics.BoxBGSel1->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.TextNipples->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					if (!niBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel2);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));(largeBoxSel2.x, largeBoxSel2.y);
						PriceWords->changeText(std::to_string(niPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel1->renderTexture(0.05, 0.47);
					Items::gameGraphics.TextNipples->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.47);
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextNipples->box);
#endif

#ifndef BETA
					if (!niBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(niPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				if (hoverSel3) {
					Items::gameGraphics.BoxBGSel1->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.TextVagina->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					if (!fiBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel3);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(fiPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));

					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel1->renderTexture(0.05, 0.67);
					Items::gameGraphics.TextVagina->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.67);
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextVagina->box);
#endif
#ifndef BETA
					if (!fiBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(fiPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#endif
				SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
				if (moSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
				if (piSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
				if (fiSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);

				Tab1Words->renderTexture();
			}

			if (tab == 2) {
				if (haSel)
					hoverSel1 = true;
				if (thSel)
					hoverSel2 = true;
				if (brSel)
					hoverSel3 = true;
#ifndef BETA
				if (hoverSel1) {
					Items::gameGraphics.BoxBGSel2->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.TextHair->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					if (!haBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel1);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(haPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel2->renderTexture(0.05, 0.27);
					Items::gameGraphics.TextHair->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture();
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextHair->box);
#endif
#ifndef BETA
					if (!haBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(haPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#endif
				if (hoverSel2) {
					Items::gameGraphics.BoxBGSel2->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.TextThigh->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					if (!thBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel2);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(thPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
					Items::gameGraphics.BoxBGSel2->renderTexture(0.05, 0.47);
					Items::gameGraphics.TextThigh->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.47);

					if (!thBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(thPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}

			if (hoverSel3) {
					Items::gameGraphics.BoxBGSel2->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.TextBreast->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					if (!brBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel3);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(brPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture( largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
					Items::gameGraphics.BoxBGSel2->renderTexture(0.05, 0.67);
					Items::gameGraphics.TextBreast->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.67);
					if (!brBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(brPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
				if (haSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
				if (thSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
				if (brSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);
				Tab2Words->renderTexture();

			}
			if (tab == 3) {

				if (maSel)
					hoverSel1 = true;
				if (feSel)
					hoverSel2 = true;
				if (xxSel)
					hoverSel3 = true;

				if (hoverSel1) {
					Items::gameGraphics.BoxBGSel3->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.TextMasturbation->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					if (!maBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel1);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(maPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
					Items::gameGraphics.BoxBGSel3->renderTexture(0.05, 0.27);
					Items::gameGraphics.TextMasturbation->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture();

					if (!maBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(maPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#ifndef BETA
				if (hoverSel2) {
					Items::gameGraphics.BoxBGSel3->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.TextFellatio->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					if (!feBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel2);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(fePrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel3->renderTexture(0.05, 0.47);
					Items::gameGraphics.TextFellatio->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.47);
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextFellatio->box);
#endif
#ifndef BETA
					if (!feBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(fePrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#endif
				if (hoverSel3) {
					Items::gameGraphics.BoxBGSel3->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.TextXXX->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					if (!xxBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel3);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(xxPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {

					Items::gameGraphics.BoxBGSel3->renderTexture(0.05, 0.67);
					Items::gameGraphics.TextXXX->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.67);
					if (!xxBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(xxPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
				if (maSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
				if (feSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
				if (xxSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);
				Tab3Words->renderTexture();

			}
			if (tab == 4) {
				if (blSel)
					hoverSel1 = true;
				if (tieSel)
					hoverSel2 = true;
				if (shirtSel)
					hoverSel3 = true;

				if (hoverSel1) {
					Items::gameGraphics.BoxBGSel4->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.TextBlazer->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					if (!blBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel1);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(blPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
					Items::gameGraphics.BoxBGSel4->renderTexture(0.05, 0.27);
					Items::gameGraphics.TextBlazer->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture();

					if (!blBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(blPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				if (hoverSel2) {

					Items::gameGraphics.BoxBGSel4->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.TextTie->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					if (!tieBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel2);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(tiePrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
					Items::gameGraphics.BoxBGSel4->renderTexture(0.05, 0.47);
					Items::gameGraphics.TextTie->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.47);

					if (!tieBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(tiePrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#ifndef BETA
				if (hoverSel3) {

					Items::gameGraphics.BoxBGSel4->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.TextShirt->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);

					if (!shirtBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel3);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(shirtPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel4->renderTexture(0.05, 0.67);
					Items::gameGraphics.TextShirt->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.67);
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextShirt->box);
#endif
#ifndef BETA
					if (!shirtBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(shirtPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#endif
				SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
				if (blSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
				if (tieSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
				if (shirtSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);

				Tab4Words->renderTexture();

			}
			if (tab == 5) {
				if (skSel)
					hoverSel1 = true;
				if (paSel)
					hoverSel2 = true;
				if (thiSel)
					hoverSel3 = true;
#ifndef BETA
				if (hoverSel1) {
					Items::gameGraphics.BoxBGSel5->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.TextSkirt->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel1.x, largeBoxSel1.y, largeBoxSel1.w, largeBoxSel1.h);
					if (!skBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel1);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->renderWords(std::to_string(skPrice) + "p", largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel5->renderTexture(0.05, 0.27);
					Items::gameGraphics.TextSkirt->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture();
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextSkirt->box);
#endif
#ifndef BETA
					if (!skBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
						BuyWords->renderTexture(largeBoxSel1.x + bxOffset, largeBoxSel1.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(skPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel1.x + xOffSet, largeBoxSel1.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				if (hoverSel2) {
					Items::gameGraphics.BoxBGSel5->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.TextPanties->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel2.x, largeBoxSel2.y, largeBoxSel2.w, largeBoxSel2.h);
					if (!paBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel2);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(skPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel5->renderTexture(0.05, 0.47);
					Items::gameGraphics.TextPanties->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.47);
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextPanties->box);
#endif
#ifndef BETA
					if (!paBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
						BuyWords->renderTexture(largeBoxSel2.x + bxOffset, largeBoxSel2.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(paPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel2.x + xOffSet, largeBoxSel2.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				if (hoverSel3) {
					Items::gameGraphics.BoxBGSel5->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.TextThighHigh->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					Items::gameGraphics.BoxOLSel->renderTextureSize(largeBoxSel3.x, largeBoxSel3.y, largeBoxSel3.w, largeBoxSel3.h);
					if (!thiBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &largeBoxSel3);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(thiPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
				else {
#endif
					Items::gameGraphics.BoxBGSel5->renderTexture(0.05, 0.67);
					Items::gameGraphics.TextThighHigh->renderTexture();
					Items::gameGraphics.BoxOLSel->renderTexture(0.05, 0.67);
#ifdef BETA
					Items::blockX->renderTexture(Items::gameGraphics.TextThighHigh->box);
#endif
#ifndef BETA
					if (!thiBuy) {
						SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 200);
						SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);
						BuyWords->renderTexture(largeBoxSel3.x + bxOffset, largeBoxSel3.y + Graphics::prcnt(0.05, 'y', Items::w1));
						PriceWords->changeText(std::to_string(thiPrice) + "p");
						if (hover) PriceWords->setTransparency(transpar);
						else PriceWords->setTransparency(255);
						PriceWords->renderTexture(largeBoxSel3.x + xOffSet, largeBoxSel3.y + Graphics::prcnt(0.09, 'y', Items::w1));
					}
				}
#endif
				SDL_SetRenderDrawColor(Items::r1, 249, 211, 120, 125);
				if (skSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel1->box);
				if (paSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel2->box);
				if (thiSel)
					SDL_RenderFillRect(Items::r1, &Items::gameGraphics.BoxBGSel3->box);

				Tab5Words->renderTexture();
			}
		}
		else {
			//if (!hoverLeft)
			//	BoxMainBGL->renderTextureClip(0, 0, 0.3168, 0.25, nonHoverAlpha - boxMod);
			//else
			//	BoxMainBGL->renderTextureClip(0, 0, 0.3168, 0.25);

			if (hoverUDL)
				Items::gameGraphics.UDArrowL->renderTextureSize(
					Items::gameGraphics.UDArrowL->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
					Items::gameGraphics.UDArrowL->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
					Items::gameGraphics.UDArrowL->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
					Items::gameGraphics.UDArrowL->box.h + Graphics::prcnt(0.01, 'y', Items::w1)
				);
			else {
				if (!hoverLeft)
					Items::gameGraphics.UDArrowL->renderTextureTransparent(nonHoverAlphaL);
				else
					Items::gameGraphics.UDArrowL->renderTexture();
			}

			}
		if (rOpen) {
			Items::gameGraphics.BoxMainBGR->renderTexture();
			if (hoverUDR)
				Items::gameGraphics.UDArrowR->renderTextureSizeFlip(
					Items::gameGraphics.UDArrowR->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
					Items::gameGraphics.UDArrowR->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
					Items::gameGraphics.UDArrowR->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
					Items::gameGraphics.UDArrowR->box.h + Graphics::prcnt(0.01, 'y', Items::w1),
					SDL_FLIP_VERTICAL);
			else
				Items::gameGraphics.UDArrowR->renderTextureFlip(SDL_FLIP_VERTICAL);

			int x = Items::gameGraphics.BoxBGV->box.x; int w = Items::gameGraphics.BoxBGV->box.w; int h = Items::gameGraphics.BoxBGV->box.h;
			int y1 = h + Items::gameGraphics.BoxBGV->box.y; int n = ((double)expCounter / 10000) * h;
			if ((double)expCounter / 10000 > 1.0) n = h;
			
			Items::gameGraphics.BoxBGV->renderTextureClip(x, y1 - n, 0, h - n, w, n);

			x = Items::gameGraphics.BoxBGH->box.x; w = Items::gameGraphics.BoxBGH->box.w; h = Items::gameGraphics.BoxBGH->box.h; int y2 = Items::gameGraphics.BoxBGH->box.y; 
			n = ((double)awareCounter / 10000) * w;
			if ((double)awareCounter / 10000 > 1.0) n = w;
			
			Items::gameGraphics.BoxBGH->renderTextureClip(x, y2, 0, 0, n, h);

			Items::gameGraphics.BoxOLH->renderTexture();
			Items::gameGraphics.BoxOLV->renderTexture();

				if (hoverCHCl3) {
					Items::gameGraphics.BoxFullChloro->renderTextureSizeFlip(
						Items::gameGraphics.BoxFullChloro->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
						Items::gameGraphics.BoxFullChloro->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
						Items::gameGraphics.BoxFullChloro->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
						Items::gameGraphics.BoxFullChloro->box.h + Graphics::prcnt(0.01, 'y', Items::w1),
						SDL_FLIP_VERTICAL);
					ChloroformWords->renderTextureSizeRotate(
						ChloroformWords->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
						ChloroformWords->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
						ChloroformWords->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
						ChloroformWords->box.h + Graphics::prcnt(0.01, 'y', Items::w1),
						45);
				}
				else {
					Items::gameGraphics.BoxFullChloro->renderTexture();
					ChloroformWords->renderTextureRotate(45);
				}
				if (chloroClick) {
					SDL_Rect flash = {
						Items::gameGraphics.BoxFullChloro->box.x - Graphics::prcnt(0.01, 'x', Items::w1),
						Items::gameGraphics.BoxFullChloro->box.y - Graphics::prcnt(0.01, 'y', Items::w1),
						Items::gameGraphics.BoxFullChloro->box.w + Graphics::prcnt(0.02, 'x', Items::w1),
						Items::gameGraphics.BoxFullChloro->box.h + Graphics::prcnt(0.02, 'y', Items::w1)
					};
					SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
					bool cont = Items::gameGraphics.white->renderTextureFadeOut(300, flash);
					if (cont) {
						Items::gameGraphics.white->resetTimer();
						chloroClick = false;
					}
				}

				if (hoverInce) {
					Items::gameGraphics.BoxFullIncence->renderTextureSizeFlip(
						Items::gameGraphics.BoxFullIncence->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
						Items::gameGraphics.BoxFullIncence->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
						Items::gameGraphics.BoxFullIncence->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
						Items::gameGraphics.BoxFullIncence->box.h + Graphics::prcnt(0.01, 'y', Items::w1),
						SDL_FLIP_VERTICAL);
					Incenence->renderTextureSizeRotate(
						Incenence->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
						Incenence->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
						Incenence->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
						Incenence->box.h + Graphics::prcnt(0.01, 'y', Items::w1),
						45);
				}
				else {
					Items::gameGraphics.BoxFullIncence->renderTexture();
					Incenence->renderTextureRotate(45);

				}
				if (inceClick) {
					SDL_Rect flash = {
						Items::gameGraphics.BoxFullIncence->box.x - Graphics::prcnt(0.01, 'x', Items::w1),
						Items::gameGraphics.BoxFullIncence->box.y - Graphics::prcnt(0.01, 'y', Items::w1),
						Items::gameGraphics.BoxFullIncence->box.w + Graphics::prcnt(0.02, 'x', Items::w1),
						Items::gameGraphics.BoxFullIncence->box.h + Graphics::prcnt(0.02, 'y', Items::w1)
					};
					SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
					bool cont = Items::gameGraphics.white->renderTextureFadeOut(300, flash);
					if (cont) {
						Items::gameGraphics.white->resetTimer();
						inceClick = false;
					}
				}

				ChloroformTipWords->renderTexture();
				IncenceTipWords->renderTexture();

				ExperienceWords->renderTextureRotate(90);
				AwarenessWords->renderTexture();

				PointsWords->renderTexture();


			}
			else {
				/*if(!hoverRight)
					BoxMainBGR->renderTextureClip(0.0, 0.0, 1.0, 0.25, nonHoverAlpha - boxMod);
				else
					BoxMainBGR->renderTextureClip(0.0, 0.0, 1.0, 0.25);*/
				if (hoverUDR)
					Items::gameGraphics.UDArrowR->renderTextureSize(
						Items::gameGraphics.UDArrowR->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
						Items::gameGraphics.UDArrowR->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
						Items::gameGraphics.UDArrowR->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
						Items::gameGraphics.UDArrowR->box.h + Graphics::prcnt(0.01, 'y', Items::w1)
					);
				else {
					if (!hoverRight)
						Items::gameGraphics.UDArrowR->renderTextureTransparent(nonHoverAlphaR);
					else
						Items::gameGraphics.UDArrowR->renderTexture();
				}

			}

			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxBGSel1->box, x, y) && lOpen) {
				hoverSel1 = true;
				hover = true;
			}
			else hoverSel1 = false;
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxBGSel2->box, x, y) && lOpen) {
				hoverSel2 = true;
				hover = true;
			}
			else hoverSel2 = false;
			if (GenericGameTools::checkRectHitBox(Items::gameGraphics.BoxBGSel3->box, x, y) && lOpen) {
				hoverSel3 = true;
				hover = true;
			}
			else hoverSel3 = false;

			if (!hoverSel1 && !hoverSel2 && !hoverSel3 && start)
				ScrollTip->renderTextureFadeOut(7000);

			if (hoverCHCl3)
				ChTip->renderTexture();
			else if (hoverInce)
				IncTip->renderTexture();
			else if (tab == 1) {
				if (hoverSel1)
					moTip->renderTexture();
				else if (hoverSel2)
					niTip->renderTexture();
				else if (hoverSel3)
					fiTip->renderTexture();
			}
			else if (tab == 2) {
				if (hoverSel1)
					haTip->renderTexture();
				else if (hoverSel2)
					thTip->renderTexture();
				else if (hoverSel3)
					brTip->renderTexture();
			}
			else if (tab == 3) {
				if (hoverSel1)
					maTip->renderTexture();
				else if (hoverSel2)
					feTip->renderTexture();
				else if (hoverSel3)
					xxTip->renderTexture();
			}
			else if (tab == 4) {
				if (hoverSel1)
					blTip->renderTexture();
				else if (hoverSel2)
					toTip->renderTexture();
				else if (hoverSel3)
					skTip->renderTexture();
			}
			else if (tab == 5) {
				if (hoverSel1)
					braTip->renderTexture();
				else if (hoverSel2)
					paTip->renderTexture();
				else if (hoverSel3)
					thhTip->renderTexture();
			}

			if (!hoverRight && !rOpen)
				Items::gameGraphics.StatsLogo->renderTextureTransparent(nonHoverAlphaR);
			else
				Items::gameGraphics.StatsLogo->renderTexture();

			if (!hoverLeft && !lOpen)
				Items::gameGraphics.ToolsLogo->renderTextureTransparent(nonHoverAlphaL);
			else
				Items::gameGraphics.ToolsLogo->renderTexture();


#ifdef HBOX
			SDL_SetRenderDrawColor(Items::r1, 255, 255, 255, 150);
			SDL_RenderFillRect(Items::r1, &head);
			SDL_RenderFillRect(Items::r1, &lArm);
			SDL_RenderFillRect(Items::r1, &rArm);
			SDL_RenderFillRect(Items::r1, &torso);
			SDL_RenderFillRect(Items::r1, &pelvis);
			SDL_RenderFillRect(Items::r1, &lLeg);
			SDL_RenderFillRect(Items::r1, &rLeg);
#endif
			if (time - eyeOpenStart > eyeOpenTime) {
				std::vector <Words*> failMsg = { new Words(58,0,"Game Over. Retry ?", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
					new Words(60,0,"  Yes", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1),
					new Words(60,0,"    No", SDL_Color{ 0,0,10,255 }, Items::r1, Items::w1) };
				Items::animationLibrary.idleAni->currentPosition = 0;
				bool choice = Prompts::runPrompt(failMsg);
				Items::animationLibrary.eyesAni->done = false;
				openEyes = false;
				time = 0;
				eyeOpenStart = 0;
				if (choice == false) {
					screenRunning = false;
					Items::gameState = 1;
				}
				else {
					//Items::gameState = -1;
					reset();
				}
			}

			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 255);
			SDL_RenderPresent(Items::r1);		
	}

	void close() {
		Mix_HaltChannel(-1);
		awareCounter = 0;
		expCounter = 0;
		expPoints = 0;

		//Items::screen = -1;

		Items::animationLibrary.pokeFake->clearFrames();
		Items::animationLibrary.handFake->clearFrames();

		for each(Texture* fr in Items::animationLibrary.eyesAni->frames) {
			fr->box.w /= 1.044660194174757;
			fr->box.h /= 1.044660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.felAni->frames) {
			fr->box.w /= 1.070;
			fr->box.h /= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.masAni->frames) {
			fr->box.w /= 1.070;
			fr->box.h /= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.noBreAni->frames) {
			fr->box.w /= 1.044660194174757;
			fr->box.h /= 1.044660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.breAni->frames) {
			fr->box.w /= 1.044660194174757;
			fr->box.h /= 1.044660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.thiAni->frames) {
			fr->box.w /= 1.070;
			fr->box.h /= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.idleAni->frames) {
			fr->box.w /= 1.084660194174757;
			fr->box.h /= 1.084660194174757;
		}
		for each(Texture* fr in Items::animationLibrary.moAni->frames) {
			fr->box.w /= 1.0;
			fr->box.h /= 0.99;
		}
		for each(Texture* fr in Items::animationLibrary.haAni->frames) {
			fr->box.w /= 1.070;
			fr->box.h /= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.lPiAni->frames) {
			fr->box.w /= 1.070;
			fr->box.h /= 1.070;
		}
		for each(Texture* fr in Items::animationLibrary.rPiAni->frames) {
			fr->box.w /= 1.0700;
			fr->box.h /= 1.0700;
		}
		for each(Texture* fr in Items::animationLibrary.fiAni->frames) {
			fr->box.w /= 1.070;
			fr->box.h /= 1.070;
		}

		Items::screen = -1;
		if (Items::gameState == 1) {
			Items::gameGraphics.clearLibrary();
			//Items::animationLibrary.clearLibrary();
			Items::menuGraphics = MenuGraphics::loadMenuGraphics();
			Items::menuGraphics.configureLibrary(Items::r1, Items::w1);
			SubMenu::setPlotElements();
			Items::textureDelivery = &Plot::badTextureSet;
			Items::textDelivery = &Text::badPlotText;
			Items::locationsDelivery = &Plot::badLocations;
			Items::operationsDelivery = &Plot::badOperations;
			Items::screen = 10;
		}
		else if(Items::gameState == -1){

			Items::gameGraphics.clearLibrary();



			///START FINAL ANIMATIONS HERE


			//Items::animationLibrary.clearLibrary();
			Items::menuGraphics = MenuGraphics::loadMenuGraphics();
			Items::menuGraphics.configureLibrary(Items::r1, Items::w1);
			SubMenu::setPlotElements();
			Items::textureDelivery = &Plot::goodTextureSet;
			Items::textDelivery = &Text::goodPlotText;
			Items::locationsDelivery = &Plot::goodLocations;
			Items::operationsDelivery = &Plot::goodOperations;
			Items::screen = 10;
		}
		Mix_FadeOutMusic(100);
	}
}

namespace OpeningSequence
{
	void runSequence();
	void loadProfile();

	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

	bool endTransitionFlag;
	bool langClicked, langPlayed;
	bool logo1Flag;
	bool logo2Flag;
	bool screenRunning;
	bool skippedFlag;

	bool hoverEN;
	bool hoverJP;

	const int FPS = 30;

	double fadeRate;
	DATATYPE_MILLISECONDS systemTime;

	int alpha;
	int x;
	int y;

	int elapsed = -1;

	//decay for flags 
	int langTime = 2000;

	//stay for l1 || in for l2
	int logo1Time = 6500;

	//decay for l1
	int logo2TransitionTime = 10000;

	//stay for l2
	int logo2Time = 11500;

	//decay for l2
	int endTransitionTime = 12120;

	long long initial = 0;
	long long expired = 0;
	long long start = 0;
	long long timer = 0;

#ifdef MULTILANG
	Texture* engSymbol;
	Texture* jpSymbol;
#endif
	Texture* logoEC;
	Texture* logo2;
	Texture* startMenu;
	Texture* white;

	Words* hs;
	Words* hsRole;

	bool bgmPlay;
	bool effectDone;
	void chanDone(int chan) {
		effectDone = true;
	}

	void OpeningSequence::runSequence()
	{
		alpha = 0;

#ifdef MULTILANG
		OpeningSequence::engSymbol = new Texture(0.10, 0.33, "Graphics/Icons/EN.png", Items::r1, Items::w1);
		OpeningSequence::jpSymbol = new Texture(0.55, 0.33, "Graphics/Icons/JP.png", Items::r1, Items::w1);
#endif
		//float a = 0;
		//float b = 0;
		//a = SDL_GetTicks();



		//b = (SDL_GetTicks() - a);
		//float c = 1.0 / 30 * 1000;
		//std::cout << b << " millisenconds with " + std::to_string(c) << " seconds per frames\n\n";

		systemTime = SYSTEM_MILLISECONDS;
#if defined ENGLISH
		hoverEN = false;
		Items::engFlag = true;
		langClicked = true;

		alpha = 0;
		start = systemTime.count();
#endif // ENGLISH
		gameLoop();
	}

	void OpeningSequence::gameLoop() {
		screenRunning = true;
		initial = 0;
		expired = 0;

		while (screenRunning) {
			initial = systemTime.count();
			systemTime = SYSTEM_MILLISECONDS;
			processInput();
			update();
			play();
			render();

			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}

		close();
	}

	void OpeningSequence::processInput() {
		Items::inputs = Items::mio->processKBMInput(&x, &y);

		if (Items::inputs->at(QUIT)) {
			screenRunning = false;
			Items::screen = -1;
		}
	}

	void OpeningSequence::update() {

		hoverEN = false;
		hoverJP = false;
		if (langClicked) {
			if (LCLICK) {
				//decay for flags 
				langTime = 500;

				//stay for l1 || in for l2
				logo1Time = 1000;

				//decay for l1
				logo2TransitionTime = 1500;

				//stay for l2
				logo2Time = 2000;

				//decay for l2
				endTransitionTime = 2500;
			}
		}
#if defined MULTILANG
		if (GenericGameTools::checkRectHitBox(engSymbol->box, x, y) && !langClicked) {
			hoverEN = true;
			if (LCLICK) {

				engSymbol->box.x = engSymbol->box.x - Graphics::prcnt(0.005, 'x', Items::w1);
				engSymbol->box.y = engSymbol->box.y - Graphics::prcnt(0.005, 'y', Items::w1);
				engSymbol->box.w = engSymbol->box.w + Graphics::prcnt(0.01, 'x', Items::w1);
				engSymbol->box.h = engSymbol->box.h + Graphics::prcnt(0.01, 'y', Items::w1);

				hoverEN = false;
				Items::engFlag = true;
				langClicked = true;

				alpha = 0;
				start = systemTime.count();
			}
		}
#ifndef DEMO
		else if (GenericGameTools::checkRectHitBox(jpSymbol->box, x, y) && !langClicked) {
			hoverJP = true;
			if (LCLICK) {

				jpSymbol->box.x = jpSymbol->box.x - Graphics::prcnt(0.005, 'x', Items::w1);
				jpSymbol->box.y = jpSymbol->box.y - Graphics::prcnt(0.005, 'y', Items::w1);
				jpSymbol->box.w = jpSymbol->box.w + Graphics::prcnt(0.01, 'x', Items::w1);
				jpSymbol->box.h = jpSymbol->box.h + Graphics::prcnt(0.01, 'y', Items::w1);

				hoverJP = false;
				Items::engFlag = false;
				langClicked = true;

				alpha = 0;
				start = systemTime.count();
			}
		}
#endif
#endif
		if (langClicked) {
			timer = systemTime.count();
			elapsed = timer - start;
			//std::cout << elapsed << "\n";
		}

		if (skippedFlag) {
			screenRunning = false;
		}
		//else if (elapsed < langTime && elapsed > -1) {
		//	std::cout << "AAA\n";
		//}
		//else if (elapsed < logo1Time && elapsed > -1) {
		//	std::cout << "BBB\n";
		//}
		//else if (elapsed < logo2Time && elapsed > -1) {
		//	std::cout << "CCC\n";
		//}
		//else if (elapsed < endTransitionTime && elapsed > -1) {
		//	std::cout << "DDD\n";
		//}
		else if (elapsed > endTransitionTime && effectDone) {
			screenRunning = false;
			Items::screen = 1;
		//	std::cout << "EEE\n";
		}
	}
	int vol = 128;
	void OpeningSequence::play() {
		Mix_VolumeMusic(MIX_MAX_VOLUME / 3);
		Mix_ChannelFinished(chanDone);
		if (langClicked && langPlayed && LCLICK && !bgmPlay) {
			bgmPlay = true;
			Mix_FadeOutChannel(0, 500);
			Items::bgmLibrary.Track1->playFadeIn(-1, 500);
		}
		//if (bgmPlay &&  vol != 0) {
		//	vol -= 2;
		//	Mix_VolumeChunk(Items::bgmLibrary.StartSFX, vol--);

		//}
		if (!bgmPlay && elapsed > logo1Time) {
			bgmPlay = true;
			Items::bgmLibrary.Track1->play();
		}

		if (langClicked && !langPlayed) {
			langPlayed = true;
			Items::openingItems.StartSFX->playSFX();
		}
	}
	void OpeningSequence::render() {
		SDL_RenderClear(Items::r1);

		if (elapsed == -1) {
#ifdef MULTILANG
			if (hoverEN) {
				engSymbol->renderTextureSize(
					engSymbol->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
					engSymbol->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
					engSymbol->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
					engSymbol->box.h + Graphics::prcnt(0.01, 'y', Items::w1));
			}
			else {
				engSy0mbol->renderTexture();
			}
#ifdef DEMO
			jpSymbol->renderTexture();
			Items::blockX->renderTexture(jpSymbol->box);
#endif // !DEMO
#ifndef DEMO
			if (hoverJP) {
				jpSymbol->renderTextureSize(
					jpSymbol->box.x - Graphics::prcnt(0.005, 'x', Items::w1),
					jpSymbol->box.y - Graphics::prcnt(0.005, 'y', Items::w1),
					jpSymbol->box.w + Graphics::prcnt(0.01, 'x', Items::w1),
					jpSymbol->box.h + Graphics::prcnt(0.01, 'y', Items::w1));
			}
			else {
				jpSymbol->renderTexture();
			}
#endif
#endif
		}

		else if (elapsed + 65< langTime  && elapsed > -1) {
			//std::cout << "111\n";
#ifdef MULTILANG
			engSymbol->renderTextureFadeOut(langTime);
			jpSymbol->renderTextureFadeOut(langTime);
#endif
			alpha = GenericGameTools::decrementAtRate(255, langTime - 100);
			SDL_SetRenderDrawBlendMode(Items::r1, SDL_BLENDMODE_BLEND);
#ifdef MULTILANG
			if (Items::engFlag) {
				SDL_RenderFillRect(Items::r1, &engSymbol->box);
			}
			else {
				SDL_RenderFillRect(Items::r1, &jpSymbol->box);
			}
#endif
			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, alpha);
			Items::openingItems.logoEC->renderTexture();
			SDL_RenderFillRect(Items::r1, NULL);

		}
		else if (elapsed < logo1Time && elapsed > -1) {
			//std::cout << "222\n";
#ifdef MULTILANG
			engSymbol->clearTexture();
			jpSymbol->clearTexture();
#endif
			//fadeout slow
			if (elapsed > (langTime + 2500))
				Items::openingItems.logoEC->renderTextureFadeOut(logo1Time - langTime - 2500);
			else
				Items::openingItems.logoEC->renderTexture();

			Items::openingItems.white->renderTextureFadeOut(3000);

			SDL_SetRenderDrawColor(Items::r1, 0, 0, 0, 0);
		}
		else if (elapsed < logo2TransitionTime  && elapsed > -1) {
			//std::cout << "333\n";

			//fadein
			Items::openingItems.logo2->renderTextureFadeIn(logo2TransitionTime - logo1Time);
			hs->renderTextureFadeIn(logo2TransitionTime - logo1Time);
			hsRole->renderTextureFadeIn(logo2TransitionTime - logo1Time);
		}
		else if (elapsed < logo2Time  && elapsed > -1) {
			//std::cout << "444\n";

			//fade out slow
			Items::openingItems.logo2->renderTextureFadeOut(logo2Time - logo2TransitionTime);
			hs->renderTextureFadeOut(logo2Time - logo2TransitionTime);
			hsRole->renderTextureFadeOut(logo2Time - logo2TransitionTime);
		}
		else {//if (elapsed < endTransitionTime  && elapsed > -1) {
			//std::cout << "555\n";
			//fadein
			//startMenu->renderTextureFadeIn(endTransitionTime - logo2TransitionTime);
		}
		//else if (elapsed > endTransitionTime  && elapsed > -1) {
		//	std::cout << "666\n";
		//	//fadeOut

		//}
		SDL_RenderPresent(Items::r1);
	}
	void loadProfile() {
#ifdef RELEASE
		SaveData::loadProfile(&Items::userData);
#endif
#ifdef DEMO
		Items::userData = SaveData();
#endif // !Demo
#ifdef UNLOCK
		Items::userData = SaveData('u');
#endif
	}
	void OpeningSequence::close() {
		hs->clearWords();
		hsRole->clearWords();
		Items::openingItems.clearLibrary();
		loadProfile();
	}
};

namespace LoadSequence {
	void loadText() {
		std::ifstream gameWordFile;
		std::ifstream menuWordFile;
		std::ifstream tipFile;
		std::ifstream promptFile;
		std::ifstream extraFile;
		std::ifstream PlotFile;
		std::ifstream hPlotFile;
		std::ifstream tPlotFile;

		std::ifstream goodEndFile;
		std::ifstream regEndFile;
		std::ifstream badEndFile;
		if (Items::engFlag) {
			extraFile.open("data/text/eEng.txt");
			gameWordFile.open("data/text/gEng.txt");
			hPlotFile.open("data/text/hPlotEng.txt");
			menuWordFile.open("data/text/mEng.txt");
			promptFile.open("data/text/pEng.txt");
			PlotFile.open("data/text/PlotEng.txt");
			tPlotFile.open("data/text/tPlotEng.txt");
			tipFile.open("data/text/ttipEng.txt");

			badEndFile.open("data/text/badplot.txt");
			regEndFile.open("data/text/mehplot.txt");
#ifdef BETA
			goodEndFile.open("data/text/falseGoodPlot.txt");
#endif
#ifndef BETA
			goodEndFile.open("data/text/goodplot.txt");
#endif

		}
		else {
			extraFile.open("data/text/eJp.txt");
			gameWordFile.open("data/text/gJp.txt");
			hPlotFile.open("data/text/hPlotJp.txt");
			menuWordFile.open("data/text/mJp.txt");
			promptFile.open("data/text/pJp.txt");
			PlotFile.open("data/text/PlotJp.txt");
			tPlotFile.open("data/text/tPlotJp.txt");
			tipFile.open("data/text/ttipJp.txt");
		}
		std::string line;
		while (std::getline(gameWordFile, line))
			Text::GameText.push_back(line);
		while (std::getline(menuWordFile, line))
			Text::MenuText.push_back(line);
		while (std::getline(tipFile, line))
			Text::TipText.push_back(line);
		while (std::getline(promptFile, line))
			Text::PromptText.push_back(line);
		while (std::getline(PlotFile, line))
			Text::PlotText.push_back(line);
		while (std::getline(extraFile, line))
			Text::extraText.push_back(line);
		while (std::getline(hPlotFile, line))
			Text::hardPlotText.push_back(line);
		while (std::getline(tPlotFile, line))
			Text::timePlotText.push_back(line);
		while (std::getline(badEndFile, line))
			Text::badPlotText.push_back(line);
		while (std::getline(regEndFile, line))
			Text::regPlotText.push_back(line);
		while (std::getline(goodEndFile, line))
			Text::goodPlotText.push_back(line);

		std::vector<std::string> badPlotText;
		std::vector<std::string> regPlotText;
		std::vector<std::string> goodPlotText;

		float tabwordX = 0.070;
		float tabwordY = 0.84;

		double tipX = 0.3175, tipY = 0.95;
		double tipFont = 40;
		double pendulumTopX = 0.52, pendulumTopY = 0.07;
		double selectionFont = 74;

		SDL_Color tipCol = { 75,75, 75,255 };

		int itemFont = 90;
		int itemFontClass = 0;

		Text::backWords = new Words(selectionFont, 0, Text::MenuText.at(5), 0, 0, Items::r1, Items::w1);
		Menu::PlayWords = new Words(itemFont, itemFontClass, Text::MenuText.at(0), SDL_Color{ 0,0,0,255 }, 0.17, 0.37, Items::r1, Items::w1);
		//Menu::PlayWords->createOutline(SDL_Color{ 255,0,0,255 });

		SubMenu::freeWords = new Words(selectionFont, 0, Text::MenuText.at(1), 0.05, 0.05, Items::r1, Items::w1);
		SubMenu::freeTip = new Words(tipFont, 0, Text::TipText.at(19), tipCol, pendulumTopX, pendulumTopY, Items::r1, Items::w1);
		SubMenu::regularWords = new Words(selectionFont, 0, Text::MenuText.at(2), 0.05, 0.15, Items::r1, Items::w1);
		SubMenu::regularTip = new Words(tipFont, 0, Text::TipText.at(20), tipCol, pendulumTopX, pendulumTopY + 0.1, Items::r1, Items::w1);
		SubMenu::timeWords = new Words(selectionFont, 0, Text::MenuText.at(3), 0.05, 0.25, Items::r1, Items::w1);
		SubMenu::timeTip = new Words(tipFont, 0, Text::TipText.at(21), tipCol, pendulumTopX, pendulumTopY + 0.2, Items::r1, Items::w1);
		SubMenu::hardWords = new Words(selectionFont, 0, Text::MenuText.at(4), 0.05, 0.35, Items::r1, Items::w1);
		SubMenu::hardTip = new Words(tipFont, 0, Text::TipText.at(22), tipCol, pendulumTopX - 0.05, pendulumTopY + 0.3, Items::r1, Items::w1);

		Menu::ExtraWords = new Words(itemFont, itemFontClass, Text::MenuText.at(6), 0.42, 0.49, Items::r1, Items::w1);

		double lAlign = 0.07;
		double vectAlign = 0.28, vectSpace = 0.125, start = 0.3;
		float chloroX = 0.76;
		float chloroY = 0.80;
		float inceX = 0.88;
		float inceY = 0.80;

		SDL_Color white = { 255,255,255,255 };
		SDL_Color woodColor = { 245,245,220,255 };

		ExtraMenu::nameWords = new Words(80, 0, Text::extraText.at(0), woodColor, lAlign - 0.0025, 0.0300, Items::r1, Items::w1);
		ExtraMenu::statusWords = new Words(50, 0, Text::extraText.at(1), woodColor, lAlign, 0.10, Items::r1, Items::w1);
		int i = 0;
		ExtraMenu::trackWords = { new Words(70, 0, Text::extraText.at(2),white, vectAlign, start + vectSpace * i++,Items::r1, Items::w1),
			new Words(70, 0, Text::extraText.at(3),white, vectAlign,  start + vectSpace * i++,Items::r1, Items::w1) ,
			new Words(70, 0, Text::extraText.at(4),white, vectAlign,  start + vectSpace * i++,Items::r1, Items::w1),
			new Words(70, 0, Text::extraText.at(18),white, vectAlign,  start + vectSpace * i++,Items::r1, Items::w1) };
		i = 0;
		ExtraMenu::imageWords = { new Words(70, 0, Text::extraText.at(9), white,vectAlign, start + vectSpace * i++,Items::r1, Items::w1),
			new Words(70, 0, Text::extraText.at(10),white, vectAlign,  start + vectSpace * i++,Items::r1, Items::w1) ,
			new Words(70, 0, Text::extraText.at(11), white,vectAlign, start + vectSpace * i++,Items::r1, Items::w1),
			new Words(70, 0, Text::extraText.at(12), white,vectAlign,  start + vectSpace * i++,Items::r1, Items::w1) };
		ExtraMenu::link1Words = new Words(85, 0, Text::extraText.at(5), woodColor, lAlign, 0.875, Items::r1, Items::w1);
		ExtraMenu::link2Words = new Words(85, 0, Text::extraText.at(15), woodColor, 0.78, 0.875, Items::r1, Items::w1);
		ExtraMenu::backWords = new Words(selectionFont, 0, Text::MenuText.at(5), woodColor, 0, 0, Items::r1, Items::w1);
		ExtraMenu::nextWords = new Words(60, 0, Text::extraText.at(16), woodColor, 0.83, 0.02, Items::r1, Items::w1);

		Menu::QuitWords = new Words(itemFont, itemFontClass, Text::MenuText.at(7), 0.17, 0.62, Items::r1, Items::w1);

		SDL_Color defaultColor = SDL_Color{ 200,200,200,255 };
		DefaultGame::Tab1Words = new Words(48, 0, Text::GameText.at(Text::Fingers), defaultColor, tabwordX, tabwordY, Items::r1, Items::w1);
		DefaultGame::Tab2Words = new Words(48, 0, Text::GameText.at(Text::Hands), defaultColor, tabwordX, tabwordY, Items::r1, Items::w1);
		DefaultGame::Tab3Words = new Words(48, 0, Text::GameText.at(Text::Penis), defaultColor, tabwordX, tabwordY, Items::r1, Items::w1);
		DefaultGame::Tab4Words = new Words(48, 0, Text::GameText.at(Text::Outer), defaultColor, tabwordX+0.005f, tabwordY, Items::r1, Items::w1);
		DefaultGame::Tab5Words = new Words(48, 0, Text::GameText.at(Text::Inner), defaultColor, tabwordX, tabwordY, Items::r1, Items::w1);

		DefaultGame::ExperienceWords = new Words(52, 1, Text::GameText.at(Text::Experience), defaultColor, 0.7395, 0.395, Items::r1, Items::w1);

		DefaultGame::AwarenessWords = new Words(36, 1, Text::GameText.at(Text::Awareness), defaultColor, 0.78, 0.69, Items::r1, Items::w1);
		DefaultGame::PointsWords = new Words(48, 2, "err", defaultColor, 0.905, 0.62, Items::r1, Items::w1);
		DefaultGame::ChloroformWords = new Words(36, 1, Text::GameText.at(Text::CHCl3), chloroX - 0.0045, chloroY + 0.0265, Items::r1, Items::w1);
		DefaultGame::ChloroformTipWords = new Words(26, 0, Text::GameText.at(Text::CHCl3Tip), defaultColor, chloroX - 0.003, chloroY + 0.11, Items::r1, Items::w1);
		DefaultGame::Incenence = new Words(32, 1, Text::GameText.at(Text::Incence), inceX - 0.005550, inceY + 0.0315, Items::r1, Items::w1);
		DefaultGame::IncenceTipWords = new Words(26, 0, Text::GameText.at(Text::IncenceTip), defaultColor, inceX - 0.009, inceY + 0.11, Items::r1, Items::w1);

		DefaultGame::moTip = new Words(36, 0, Text::TipText.at(Text::Mo),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::niTip = new Words(36, 0, Text::TipText.at(Text::Ni),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::fiTip = new Words(36, 0, Text::TipText.at(Text::Fi),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::haTip = new Words(36, 0, Text::TipText.at(Text::Ha),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::thTip = new Words(36, 0, Text::TipText.at(Text::Th),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::brTip = new Words(36, 0, Text::TipText.at(Text::Br),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::maTip = new Words(36, 0, Text::TipText.at(Text::Ma),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::feTip = new Words(36, 0, Text::TipText.at(Text::Fe),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::xxTip = new Words(36, 0, Text::TipText.at(Text::Xx),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::blTip = new Words(36, 0, Text::TipText.at(Text::Bl),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::toTip = new Words(36, 0, Text::TipText.at(Text::To),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::skTip = new Words(36, 0, Text::TipText.at(Text::Sk),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::braTip = new Words(36, 0, Text::TipText.at(Text::Bra),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::paTip = new Words(36, 0, Text::TipText.at(Text::Pa),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::thhTip = new Words(36, 0, Text::TipText.at(Text::Thh),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::ChTip = new Words(36, 0, Text::TipText.at(Text::Ch),defaultColor, tipX, tipY, Items::r1, Items::w1);
		DefaultGame::IncTip = new Words(36, 0, Text::TipText.at(Text::In),defaultColor, tipX, tipY, Items::r1, Items::w1);

		DefaultGame::ScrollTip = new Words(52, 0, Text::TipText.at(Text::Scroll), defaultColor, tipX - 0.1f, tipY-0.05, Items::r1, Items::w1);
	}
	void loadX() { 

	}
	void loadAll() {

		Items::menuGraphics.MenuBG = Menu::MenuBG = new Texture("art/SubBG.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.Title = Menu::Title = new Texture("art/title.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.Z3 = Menu::Z3 = new Texture(0.67, 0.7, "art/Z3.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.Z2 = Menu::Z2 = new Texture(0.78, 0.52, "art/Z2.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.Z1 = Menu::Z1 = new Texture(0.7325, 0.29, "art/Z1.png", Items::r1, Items::w1, Items::serialFlag);

		  Items::menuGraphics.subBG = SubMenu::subBG = new Texture("art/SubBG.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.subMenuIconR = SubMenu::subMenuIconR = new Texture(0.0, 0.6, "art/iconR.png", Items::r1, Items::w1, Items::serialFlag);
		//  Items::menuGraphics.subMenuIconL = new Texture(-1, 0.6, "art/iconL.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.ExtraBG = ExtraMenu::ExtraBG = new Texture("art/ExtraBG.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.ExtraBGHolder = ExtraMenu::ExtraBGHolder = new Texture("art/ExtraBGHolder.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.pendulumBase = ExtraMenu::pendulumBase = new Texture(0.10, 0.16, "art/base.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.pendulumRod = ExtraMenu::pendulumRod = new Texture(0.297, -0.215, "art/arm.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.pendulumBase2 = ExtraMenu::pendulumBase2 = new Texture(0.10, 0.16, "art/base2.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.pendulumRod2 = ExtraMenu::pendulumRod2 = new Texture(0.297, -0.215, "art/arm2.png", Items::r1, Items::w1, Items::serialFlag);

		  //Items::menuGraphics.containerTextureEC = ExtraMenu::containerTextureEC = new Texture(0.02, 0.18, 1.0025, 1.0, "art/-ECT.jpg", Items::r1, Items::w1, Items::serialFlag);
		 // Items::menuGraphics.containerTextureSH = ExtraMenu::containerTextureSH = new Texture(0.02, 0.18, "art/-SHT.jpg", Items::r1, Items::w1, Items::serialFlag);

		  //Items::menuGraphics.offTrain = offTrain = new Texture("art/2.jpg", Items::r1, Items::w1, Items::serialFlag);
		//0.1533457249070632
		  Items::menuGraphics.card0 = CardTextures::card0 = new Texture(0, 0, "art/card0.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.card1 = CardTextures::card1 = new Texture(0, 0, "art/card1.png", Items::r1, Items::w1, Items::serialFlag);
#ifndef BETA
		  Items::menuGraphics.card2 = CardTextures::card1 = new Texture(0, 0, "art/card1.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.card3 = CardTextures::card1 = new Texture(0, 0, "art/card1.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.card4 = CardTextures::card1 = new Texture(0, 0, "art/card1.png", Items::r1, Items::w1, Items::serialFlag);
#endif
#ifdef BETA
		  Items::menuGraphics.card2 = CardTextures::card1 = new Texture(0, 0, "art/white.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.card3 = CardTextures::card1 = new Texture(0, 0, "art/white.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.card4 = CardTextures::card1 = new Texture(0, 0, "art/white.png", Items::r1, Items::w1, Items::serialFlag);
#endif

		  Items::menuGraphics.bg1 = new Texture("art/1.jpg", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.bg2 = new Texture("art/2.png", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.bg3 = new Texture("art/3.jpg", Items::r1, Items::w1, Items::serialFlag);


		  Items::menuGraphics.bg4 = new Texture("art/4.jpg", Items::r1, Items::w1, Items::serialFlag);
		  Items::menuGraphics.bg5 = new Texture("art/5.jpg", Items::r1, Items::w1, Items::serialFlag);
#ifndef BETA
		  Items::menuGraphics.bg6 = new Texture("art/6.jpg", Items::r1, Items::w1, Items::serialFlag);
#endif	 

#ifdef BETA
		  Items::menuGraphics.bg6 = new Texture("art/white.png", Items::r1, Items::w1, Items::serialFlag);
#endif
		//Menu::MenuBG = PlotBG;

		float chloroX = 0.76;
		float chloroY = 0.80;
		float inceX = 0.88;
		float inceY = 0.80;
	
		Items::gameGraphics.MainBG = DefaultGame::MainBG = new Texture("art/gameBG.png", Items::r1, Items::w1 , Items::serialFlag);
		Items::gameGraphics.CharacterHair = DefaultGame::Character = new Texture("art/artHairHalf.png", Items::r1, Items::w1 , Items::serialFlag);
		Items::gameGraphics.CharacterBody = DefaultGame::Character = new Texture("art/artBody.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.CharacterTorso = DefaultGame::Character = new Texture("art/artTorso.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.CharacterRBreast = new Texture("art/artRBre.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.CharacterLBreast = new Texture("art/artLBre.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.CharacterLeg = new Texture("art/artLeg.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.shadows = new Texture("art/shadows.png", Items::r1, Items::w1, Items::serialFlag);
		//Items::gameGraphics.TextBTree = new Texture("art/art-.png", Items::r1, Items::w1, Items::serialFlag);

		Items::gameGraphics.UDArrowL = DefaultGame::UDArrowL = new Texture(0.10, 0.135, "Graphics/Icons/DArrow.png", Items::r1, Items::w1 , Items::serialFlag);
		Items::gameGraphics.UDArrowR = DefaultGame::UDArrowR = new Texture(0.81, 0.135, "Graphics/Icons/DArrow.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.RArrow = DefaultGame::RArrow = new Texture(0.23, 0.35, "Graphics/Icons/RArrow.png", Items::r1, Items::w1 , Items::serialFlag);

		Items::gameGraphics.StatsLogo = DefaultGame::StatsLogo = new Texture(0.75, 0.01, "Graphics/Icons/Stats.png", Items::r1, Items::w1 , Items::serialFlag);
		Items::gameGraphics.ToolsLogo = DefaultGame::ToolsLogo = new Texture(0.01, 0.01, "Graphics/Icons/Tools.png", Items::r1, Items::w1, Items::serialFlag);

		float awaX = 0.748;
		float awaY = 0.73;
		float expX = 0.9;
		float expY = 0.25;

		Items::gameGraphics.BoxOLH = DefaultGame::BoxOLH = new Texture(awaX, awaY, "Graphics/boxs/boxol/HBoxO.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxOLV = DefaultGame::BoxOLV = new Texture(expX, expY, "Graphics/boxs/boxol/VBoxO.png", Items::r1, Items::w1 ,Items::serialFlag);

		Items::gameGraphics.BoxOLSel = DefaultGame::BoxOLSel = new Texture(0.05, 0.27, "Graphics/boxs/boxol/BGO.png", Items::r1, Items::w1 ,Items::serialFlag);

		Items::gameGraphics.BoxBGH = DefaultGame::BoxBGH = new Texture(awaX - 0.0005, awaY - 0.0005, "graphics/boxs/boxbg/HBoxBG.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxBGV = DefaultGame::BoxBGV = new Texture(expX-0.0005, expY - 0.0005, "graphics/boxs/boxbg/VBoxBG.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxBGSel1 = DefaultGame::BoxBGSel1 = new Texture(0.05, 0.27, "graphics/boxs/boxbg/LBG1.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxBGSel2 = DefaultGame::BoxBGSel2 = new Texture(0.05, 0.47, "graphics/boxs/boxbg/LBG2.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxBGSel3 = DefaultGame::BoxBGSel3 = new Texture(0.05, 0.67, "graphics/boxs/boxbg/LBG3.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxBGSel4 = DefaultGame::BoxBGSel4 = new Texture(0.05, 0.27, "graphics/boxs/boxbg/LBG4.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxBGSel5 = DefaultGame::BoxBGSel5 = new Texture(0.05, 0.27, "graphics/boxs/boxbg/LBG5.png", Items::r1, Items::w1 ,Items::serialFlag);

		Items::gameGraphics.BoxFullChloro = DefaultGame::BoxFullChloro = new Texture(chloroX, chloroY, "Graphics/boxs/fullbox/CHC3.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxFullIncence = DefaultGame::BoxFullIncence = new Texture(inceX, inceY, "Graphics/boxs/fullbox/Ince.png", Items::r1, Items::w1 ,Items::serialFlag);

		Items::gameGraphics.BoxMainBGR = DefaultGame::BoxMainBGR = new Texture(0.74, 0.0, "Graphics/boxs/fullbox/BGR.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.BoxMainBGL = DefaultGame::BoxMainBGL = new Texture("Graphics/boxs/fullbox/BGL.png", Items::r1, Items::w1 ,Items::serialFlag);

		Items::gameGraphics.TextBlazer = DefaultGame::TextBlazer = new Texture(0.05, 0.27, "Graphics/Letters/bl.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextBreast = DefaultGame::TextBreast = new Texture(0.05, 0.67, "Graphics/Letters/br.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextShirt = DefaultGame::TextShirt = new Texture(0.05, 0.67, "Graphics/Letters/bra.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextFellatio = Items::gameGraphics.TextFellatio = new Texture(0.05, 0.47, "Graphics/Letters/fel.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextHair = Items::gameGraphics.TextHair = new Texture(0.05, 0.27, "Graphics/Letters/ha.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextMasturbation = new Texture(0.05, 0.27, "Graphics/Letters/ma.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextMouth= DefaultGame::TextMouth = new Texture(0.05, 0.27, "Graphics/Letters/mo.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextNipples = DefaultGame::TextNipples = new Texture(0.05, 0.47, "Graphics/Letters/ni.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextPanties = DefaultGame::TextPanties = new Texture(0.05, 0.47, "Graphics/Letters/pa.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextSkirt = DefaultGame::TextSkirt = new Texture(0.05, 0.27, "Graphics/Letters/sk.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextThighHigh = DefaultGame::TextThighHigh = new Texture(0.05, 0.67, "Graphics/Letters/th.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextThigh = DefaultGame::TextThigh = new Texture(0.05, 0.47, "Graphics/Letters/thi.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextTie = DefaultGame::TextTie = new Texture(0.05, 0.47, "Graphics/Letters/to.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextVagina = DefaultGame::TextVagina = new Texture(0.05, 0.67, "Graphics/Letters/va.png", Items::r1, Items::w1 ,Items::serialFlag);
		Items::gameGraphics.TextXXX = DefaultGame::TextXXX = new Texture(0.05, 0.67, "Graphics/Letters/Xx.png", Items::r1, Items::w1 ,Items::serialFlag);

		Items::gameGraphics.TextBG = new Texture(0, 0, "art/BG.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.TextWall= new Texture(0, 0, "art/Wall.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.TextLights = new Texture(0, 0, "art/Lights.png", Items::r1, Items::w1, Items::serialFlag);

		Items::gameGraphics.bla = new Texture(0, 0, "clothing/bla.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.shirt = new Texture(0, 0, "clothing/shirt.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.skirt = new Texture(0, 0, "clothing/skir.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.thh = new Texture(0, 0, "clothing/thh.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.tie = new Texture(0, 0, "clothing/tie.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.tieTop = new Texture(0, 0, "clothing/tieTop.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.pa = new Texture(0, 0, "clothing/pa.png", Items::r1, Items::w1, Items::serialFlag);

		Items::gameGraphics.paMois = new Texture(0, 0, "clothing/paMois.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.mois = new Texture(0, 0, "art/artmois.png", Items::r1, Items::w1, Items::serialFlag);
	
		Items::gameGraphics.semHair = new Texture(0, 0, "art/semha.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.semMo = new Texture(0, 0, "art/semmo.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.semBre = new Texture(0, 0, "art/sembre.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.semSto = new Texture(0, 0, "art/semsto.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.semLeg = new Texture(0, 0, "art/semleg.png", Items::r1, Items::w1, Items::serialFlag);

		Items::gameGraphics.selTabs = new Texture(0, 0, "Graphics/boxs/fullbox/selBox.png", Items::r1, Items::w1, Items::serialFlag);
		Items::gameGraphics.selHighlight = new Texture(0, 0, "Graphics/boxs/fullbox/selectedBox.png", Items::r1, Items::w1, Items::serialFlag);
	}
	void loadAni() {
		std::vector<Texture*> mo;
		for (int i = 1; i <= 95; i++) {
			mo.push_back(new Texture("Animations/mo/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> ha;
		for (int i = 1; i <= 58; i++) {
			ha.push_back(new Texture("Animations/hair/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> lPin;
		for (int i = 1; i <= 47; i++) {
			lPin.push_back(new Texture("Animations/lPin/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> rPin;
		for (int i = 1; i <= 82; i++) {
			rPin.push_back(new Texture("Animations/rPin/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> fing;
		for (int i = 1; i <= 96; i++) {
			fing.push_back(new Texture("Animations/fing/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> idle;
		for (int i = 1; i <= 120; i++) {
			idle.push_back(new Texture("Animations/idle/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}

		std::vector<Texture*> eyes;
		for (int i = 1; i <= 5; i++) {
			eyes.push_back(new Texture("Animations/eyes/(" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> fel;
		for (int i = 1; i <= 96; i++) {
			fel.push_back(new Texture("Animations/fel/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> mas;
		for (int i = 1; i <= 131; i++) {
			mas.push_back(new Texture("Animations/mastu/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> noBre;
		for (int i = 1; i <= 96; i++) {
			noBre.push_back(new Texture("Animations/noshirfond/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> bre;
		for (int i = 1; i <= 96; i++) {
			bre.push_back(new Texture("Animations/shirfond/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}
		std::vector<Texture*> thi;
		for (int i = 1; i <= 96; i++) {
			thi.push_back(new Texture("Animations/thi/ (" + std::to_string(i) + ").png", Items::r1, Items::w1, Items::serialFlag));
		}

		Items::animationLibrary.idleAni = new Animation(idle, Items::serialFlag);
		Items::animationLibrary.moAni = new Animation(mo, Items::serialFlag);
		Items::animationLibrary.lPiAni = new Animation(lPin, Items::serialFlag);
		Items::animationLibrary.rPiAni = new Animation(rPin, Items::serialFlag);
		Items::animationLibrary.fiAni = new Animation(fing, Items::serialFlag);
		Items::animationLibrary.haAni = new Animation(ha, Items::serialFlag);

		Items::animationLibrary.eyesAni = new Animation(eyes, Items::serialFlag);
		Items::animationLibrary.felAni = new Animation(fel, Items::serialFlag);
		Items::animationLibrary.masAni = new Animation(mas, Items::serialFlag);
		Items::animationLibrary.noBreAni = new Animation(noBre, Items::serialFlag);
		Items::animationLibrary.breAni = new Animation(bre, Items::serialFlag);
		Items::animationLibrary.thiAni = new Animation(thi, Items::serialFlag);

		//Items::animationLibrary.moAni = new Animation(new Texture("Animations/mo/mou_spritesheet.png", Items::r1, Items::w1, Items::serialFlag), 1076,1100, Items::serialFlag);
		Items::animationLibrary.pokeFake = new Texture("Animations/Poke/Poke.png", Items::r1, Items::w1, Items::serialFlag);
		Items::animationLibrary.handFake = new Texture("Animations/hand/hand.png", Items::r1, Items::w1, Items::serialFlag);

	}
	void loadMusic() {
		printf("Mix_LoadMUS(\"DEFAULT\"): %s\n", Mix_GetError());
		Sound::ClickSFX = Items::sfxLibrary.ClickSFX = new SFX("Audio/SFXClick.mp3", 98,Items::serialFlag);
		Sound::NoSFX = Items::sfxLibrary.NoSFX = new SFX("Audio/SFXNo.mp3", 98,Items::serialFlag);
		Sound::SelectionSFX = Items::sfxLibrary.SelectionSFX = new SFX("Audio/SFXSelection.mp3", 80,Items::serialFlag);
		Sound::SelectionSFX2 = Items::sfxLibrary.SelectionSFX2 = new SFX("Audio/SFXSelect2.mp3", 24, Items::serialFlag);
		Sound::OpenSFX = Items::sfxLibrary.OpenSFX = new SFX("Audio/SFXOpen.mp3", 54, Items::serialFlag);
		Sound::DerobeSFX = Items::sfxLibrary.DerobeSFX = new SFX("Audio/SFXDerobe.mp3", 64 ,Items::serialFlag);
		Sound::RobeSFX = Items::sfxLibrary.RobeSFX = new SFX("Audio/SFXRobe.mp3", 64 ,Items::serialFlag);

		Sound::IntercomSFX = Items::sfxLibrary.IntercomSFX = new SFX("Audio/GameSFX/dead.mp3", 64 ,Items::serialFlag);
		Sound::LightningSFX = Items::sfxLibrary.HornSFX = new SFX("Audio/GameSFX/BGMSFX-Horn.mp3", 64 ,Items::serialFlag);
		Sound::PassingSFX = Items::sfxLibrary.PassingSFX = new SFX("Audio/GameSFX/BGMSFX-Passing.mp3", 64 ,Items::serialFlag);
		Sound::StepsSFX = Items::sfxLibrary.bangSFX = new SFX("Audio/GameSFX/BGMSFX-bang.mp3", 64 ,Items::serialFlag);
		Sound::XingSFX = Items::sfxLibrary.XingSFX = new SFX("Audio/GameSFX/BGMSFX-Xing.mp3", 64 , Items::serialFlag);

		Items::sfxLibrary.moSFX = new SFX("Audio/actionSFX/moSFX.mp3", 64, Items::serialFlag);
		Items::sfxLibrary.feSFX = new SFX("Audio/actionSFX/feSFX.mp3", 64, Items::serialFlag);
		Items::sfxLibrary.fiSFX = new SFX("Audio/actionSFX/fiSFX.mp3", 64, Items::serialFlag);
		Items::sfxLibrary.masSFX = new SFX("Audio/actionSFX/masSFX.mp3", 64, Items::serialFlag);
		Items::sfxLibrary.moan1SFX = new SFX("Audio/actionSFX/moan1.mp3", 45, Items::serialFlag);
		Items::sfxLibrary.neutSFX = new SFX("Audio/actionSFX/neutral.mp3", 64, Items::serialFlag);

		Items::sfxLibrary.failSFX = new SFX("Audio/specialSFX/failSound.mp3", 64, Items::serialFlag);
		Items::sfxLibrary.heartFastSFX= new SFX("Audio/specialSFX/HeartBeatFast.wav", 64, Items::serialFlag);
		Items::sfxLibrary.heartMediumSFX= new SFX("Audio/specialSFX/HeartBeatMedium.wav", 64, Items::serialFlag);
		Items::sfxLibrary.heartSlowSFX= new SFX("Audio/specialSFX/HeartBeatSlow.wav", 64, Items::serialFlag);
		Items::sfxLibrary.startSFX= new SFX("Audio/specialSFX/StartGame.mp3", 64, Items::serialFlag);

		Items::sfxLibrary.errorSFX = new SFX("Audio/SFXError.mp3", 64, Items::serialFlag);
		Items::sfxLibrary.pointsSFX = new SFX("Audio/SFXPoints.mp3", 64, Items::serialFlag);

		Items::bgmLibrary.Track1 = Sound::Track1 = new BGM("Audio/Track1.wav", Items::serialFlag);
		printf("Mix_LoadMUS(\"Track1\"): %s\n", Mix_GetError());
		Items::bgmLibrary.Track2 = Sound::Track2 = new BGM("Audio/Track2.mp3", Items::serialFlag);
		printf("Mix_LoadMUS(\"Track2\"): %s\n", Mix_GetError());
		Items::bgmLibrary.Track3 = Sound::Track4 = new BGM("Audio/Track3.wav", Items::serialFlag);
		printf("Mix_LoadMUS(\"Track3\"): %s\n", Mix_GetError());
#ifndef BETA
		Items::bgmLibrary.Track4 = Sound::Track3 = new BGM("Audio/Track2.wav", Items::serialFlag);
		printf("Mix_LoadMUS(\"Track4\"): %s\n", Mix_GetError());
#endif
#ifdef BETA
	Items::bgmLibrary.Track4 = Sound::Track3 = new BGM("Audio/dead.mp3", Items::serialFlag);
	printf("Mix_LoadMUS(\"Track4\"): %s\n", Mix_GetError());
#endif
		 Items::bgmLibrary.TrainBG = Sound::TrainBG = new BGM("Audio/GameSFX/BGMSFX-TrainBG.wav", Items::serialFlag);
		printf("Mix_LoadMUS(\"TrainBG\"): %s\n", Mix_GetError());

	}
	/*void setPlotElements() {
		Plot::opTextureSet.push_back(Items::menuGraphics.bg1);
		Plot::opTextureSet.push_back(Items::menuGraphics.bg2);
		Plot::opTextureSet.push_back(Items::menuGraphics.card0);
		Plot::opOperations.push_back(7);
		Plot::opOperations.push_back(17);
		Plot::opOperations.push_back(Text::PlotText.size());
		Plot::opLocations.push_back(1);


		Plot::hardTextureSet.push_back(CardTextures::card1);
		Plot::hardOperations = { 2 };
		Plot::hardLocations.push_back(1);

		Plot::timeTextureSet.push_back(CardTextures::card1);
		Plot::timeLocations.push_back(2);
		Plot::timeOperations = { 2 };

		Plot::badTextureSet.push_back(Items::menuGraphics.bg3);
		Plot::badTextureSet.push_back(Items::menuGraphics.bg4);
		Plot::badOperations.push_back(0);
		Plot::badLocations.push_back(0);

		Plot::regularTextureSet.push_back(Items::menuGraphics.bg3);
		Plot::regularTextureSet.push_back(Items::menuGraphics.bg5);
		Plot::regularOperations.push_back(0);
		Plot::regularLocations.push_back(0);

		Plot::goodTextureSet.push_back(Items::menuGraphics.bg3);
		Plot::goodTextureSet.push_back(Items::menuGraphics.bg6);
		Plot::goodOperations.push_back(0);
		Plot::goodLocations.push_back(0);
	}*/
}

double setScale(int y, int yMax) {
	if (y > yMax)
		return 1.0;
	else
		return ((double)y - 200.0) / yMax;
}

//int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance,	LPSTR lpCmdLine,	int nCmdShow) {
int main(int argc, char* argv){
	if (!Graphics::initialize(2)) std::cout << "GRAPHICS INIT FAILED\n";

	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 500);

	int monitorX = GetSystemMetrics(SM_CXSCREEN);
	int monitorY = GetSystemMetrics(SM_CYSCREEN);

	float scale = setScale(monitorY, 1100);
	Graphics::scale = scale;
#ifdef SERIALIZE
	Graphics::scale = 1.0;
#endif

	std::cout << scale << " Scale\n";
	std::cout << std::to_string(TTF_Linked_Version()->major) << " MajVer\n";
	std::cout << std::to_string(TTF_Linked_Version()->minor) << " MinVer\n";
	std::cout << std::to_string(TTF_Linked_Version()->patch) << " PatchVer\n\n";


	Items::w1 = SDL_CreateWindow("EroSomnia - TMP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * 1076, scale * 1100, SDL_WINDOW_OPENGL);
	Items::r1 = SDL_CreateRenderer(Items::w1, -1, SDL_RENDERER_ACCELERATED);

	SDL_GetWindowSize(Items::w1, &ScreenWidth, &ScreenHeight);

	SDL_Surface *icon = SDL_LoadBMP("icon.bmp");
	SDL_SetWindowIcon(Items::w1,
		icon);
	SDL_FreeSurface(icon);

	Items::largeBox = {
		0,0,
		Graphics::prcnt(1.0,'x', Items::w1),Graphics::prcnt(1.0,'y', Items::w1)
	};


	bool gameRunning = true;
	Items::screen = 0;

	Items::msgBox = new Texture("msgbox.png", Items::r1, Items::w1);
	Items::blockX = new Texture("xLarge.png", Items::r1, Items::w1);

#ifdef JUMPTEST
	using namespace Items;
	screen = 11;
	gameMode = 1;
	bgmLibrary = BGMLibrary::loadBGMLibrary();
	bgmLibrary.configureLibrary();
	sfxLibrary = SFXLibrary::loadSFXLibrary();
	sfxLibrary.configureLibrary();
	LoadSequence::loadText();
	animationLibrary = AnimationLibrary::loadAnimationLibrary();
	animationLibrary.configureLibrary(Items::r1, Items::w1);
#endif

#ifdef  BETA
	LoadSequence::loadX();
#endif //  BETA

	Items::mio = new KeyboardMouseIO();
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	while (gameRunning) {
		switch (Items::screen) {
		case -1:
			gameRunning = false;
			break;
		case 00: {
#ifdef SERIALIZE
			Items::openingItems.StartSFX = Sound::StartSFX = new SFX("Audio/SFXOP.mp3", 64, Items::serialFlag);
			Items::openingItems.logoEC = OpeningSequence::logoEC = new Texture("Art/ph1.png", Items::r1, Items::w1, Items::serialFlag);
			Items::openingItems.logo2 = OpeningSequence::logo2 = new Texture("Art/ph2.png", Items::r1, Items::w1, Items::serialFlag);
			Items::openingItems.white = OpeningSequence::white = new Texture("Art/white.png", Items::r1, Items::w1, Items::serialFlag);
			Items::gameGraphics.white = new Texture("Art/white.png", Items::r1, Items::w1, Items::serialFlag);
			Items::gameGraphics.black = OpeningSequence::white = new Texture("Art/black.png", Items::r1, Items::w1, Items::serialFlag);
			Items::openingItems.saveOpeningItems();

			Items::openingItems.clearLibrary();
			Items::openingItems = OpeningItems::loadOpeningItems();
			Items::openingItems.configureLibrary(Items::r1, Items::w1);
			Items::openingItems.clearLibrary();

			LoadSequence::loadAll();
			Items::gameGraphics.saveGameGraphics();

			Items::gameGraphics.clearLibrary();
			Items::gameGraphics = GameGraphics::loadGameGraphics();
			Items::gameGraphics.configureLibrary(Items::r1, Items::w1);
			Items::gameGraphics.clearLibrary();

			LoadSequence::loadAni();
			Items::animationLibrary.saveAnimationLibrary();

			Items::animationLibrary.clearLibrary();
			Items::animationLibrary = AnimationLibrary::loadAnimationLibrary();
			Items::animationLibrary.configureLibrary(Items::r1, Items::w1);
			Items::animationLibrary.clearLibrary();

			Items::menuGraphics.saveMenuGraphics();

			Items::menuGraphics.clearLibrary();
			Items::menuGraphics = MenuGraphics::loadMenuGraphics();
			Items::menuGraphics.configureLibrary(Items::r1, Items::w1);
			Items::menuGraphics.clearLibrary();


			LoadSequence::loadMusic();
			Items::sfxLibrary.saveSFXLibrary();

			Items::sfxLibrary.clearLibrary();
			Items::sfxLibrary = SFXLibrary::loadSFXLibrary();
			Items::sfxLibrary.configureLibrary();
			Items::sfxLibrary.clearLibrary();


			Items::bgmLibrary.saveBGMLibrary();
			Items::bgmLibrary.clearLibrary();
			Items::bgmLibrary = BGMLibrary::loadBGMLibrary();
			Items::bgmLibrary.configureLibrary();
			Items::bgmLibrary.clearLibrary();

			//LoadSequence::loadText();
			//Items::openingItems.StartSFX->playSFX();
			//Items::sfxLibraryOpenSFX->playSFX();
			//Items::bgmLibrary.Track2->play();*/
			//while (Items::screen < 8000) {
				//Items::openingItems.logoEC->renderTexture();
				//SDL_RenderPresent(Items::r1);
				//Items::screen = SDL_GetTicks();
			//}
			Items::userData.saveProfile();
			return 0;

#endif
#ifdef NOSERIAL
			Items::openingItems.StartSFX = Items::sfxLibrary.StartSFX = new SFX("Audio/SFXOP.mp3", 64);
			Items::openingItems.logoEC = OpeningSequence::logoEC = new Texture("Art/ph1.png", Items::r1, Items::w1);
			Items::openingItems.logo2 = OpeningSequence::logo2 = new Texture(-0.15, 0.2, "Art/ph2.png", Items::r1, Items::w1);
			Items::openingItems.startMenu = OpeningSequence::startMenu = new Texture("Art/white.png", Items::r1, Items::w1);
			Items::openingItems.white = OpeningSequence::white = new Texture("Art/white.png", Items::r1, Items::w1);
			Items::openingItems.hs = OpeningSequence::hs = new Words(64, 1, "Sunset & Hariet", 0.0, 0.1, Items::r1, Items::w1);
			OpeningSequence::hs->changeTextColor(SDL_Color{ 255,255,255,255 });
			Items::openingItems.hsRole = OpeningSequence::hsRole = new Words(64, 1, "Animation & Art", 0.0, 0.55, Items::r1, Items::w1);
			Items::openingItems.hsRole->changeTextColor(SDL_Color{ 255,255,255,255 });
			OpeningSequence::white->changeImageSize(1.0, 1.0);
#endif
#ifndef SERIALIZE
			int optime = SDL_GetTicks();
			std::cout << optime << "\n";
			Items::openingItems = OpeningItems::loadOpeningItems();
			Items::openingItems.configureLibrary(Items::r1, Items::w1);
			Items::openingItems.white->changeImageSize(1.0, 1.0);

			OpeningSequence::hs = new Words(52, 1, " ", 0.0, 0.1, Items::r1, Items::w1);
			OpeningSequence::hs->changeTextColor(SDL_Color{ 255,255,255,255 });
			OpeningSequence::hsRole = new Words(52, 1, " ", 0.0, 0.55, Items::r1, Items::w1);
			OpeningSequence::hsRole->changeTextColor(SDL_Color{ 255,255,255,255 });

			Items::bgmLibrary = BGMLibrary::loadBGMLibrary();
			Items::bgmLibrary.configureLibrary();

			Items::menuGraphics = MenuGraphics::loadMenuGraphics();
			Items::menuGraphics.configureLibrary(Items::r1, Items::w1);
			Items::sfxLibrary = SFXLibrary::loadSFXLibrary();
			Items::sfxLibrary.configureLibrary();

			LoadSequence::loadText();
			//LoadSequence::setPlotElements();
		
			int edtime = SDL_GetTicks();
			std::cout << "MENU, SFX, TEXT, PLOT: " << (double)(edtime - optime) / 1000 << "\n";
			std::cout << "Frames: " << (double)(edtime - optime) / 30 << "\n";

			Items::animationLibrary = AnimationLibrary::loadAnimationLibrary();
			Items::animationLibrary.configure1Third(Items::r1, Items::w1);
			Items::animationLibrary.configure2Third(Items::r1, Items::w1);
			OpeningSequence::runSequence();
			Items::animationLibrary.configure3Third(Items::r1, Items::w1);
			Items::openingItems.clearLibrary();

			/*************************/
			//Items::screen = 11;


#endif // !SERIALIZE
		}
			break;
		case 01:
			Menu::runGame();
			break;
		case 02:
			SubMenu::runGame();
			break;
		case 03:
			ExtraMenu::runGame();
			break;
		case 10:
			Plot::runGame(Items::textureDelivery, Items::textDelivery, Items::locationsDelivery, Items::operationsDelivery);
			break;
		case 11:{
			int optime = SDL_GetTicks();
			std::cout << optime << "\n";
#ifndef JUMPTEST
			Items::menuGraphics.clearLibrary();
#endif // !JUMPTEST
			GenericGameTools::reset();
			Items::gameGraphics = GameGraphics::loadGameGraphics();
			Items::gameGraphics.configureLibrary(Items::r1, Items::w1);
			Items::gameGraphics.black->changeImageSize(1.0, 1.0);

			int edtime = SDL_GetTicks();
			std::cout << "GAME: " << (double)(edtime - optime) / 1000 << "\n";
			std::cout << "Frames: " << (double)(edtime - optime) / 30<< "\n";
			DefaultGame::runGame();
			}break;
		}
	}

	Items::userData.saveProfile();

#ifdef SERIALIZE
#endif

	return 0;
}

