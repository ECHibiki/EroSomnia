class VNFormat {
public:
	bool screenRunning = true;
	std::vector<bool>* inputs;

	const int FPS = 30;
	std::chrono::milliseconds systemTime;

	int alpha;
	int x;
	int y;
	int* screen;
	int slideCounter;

	long long initial = 0;
	long long expired = 0;

	MouseIO* mio;

	SDL_Renderer* r1;
	SDL_Window* w1;

	Texture** currentTexture;
	TextBox* textBox;

	std::vector<Texture*> plotImages;
	std::vector<std::string> plotText;
	std::vector<int> imageSwitchLocations;
	std::vector<int> operationLocations;

	VNFormat(std::vector<Texture*>* images, std::vector<std::string>* text, std::vector<int>* switchLocations, std::vector<int>* switchOperations, int* scrn);

	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

};


/****************EDIT ZONE********************/

VNFormat::VNFormat(std::vector<Texture*>* images, std::vector<std::string>* text, std::vector<int>* switchLocations, std::vector<int>* switchOperations, int* scrn)
	:plotImages(*images), plotText(*text), imageSwitchLocations(*switchLocations), operationLocations(*switchOperations), screen(screen) {

	gameLoop();
}

/****************EDIT ZONE********************/


void VNFormat::gameLoop() {

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

void VNFormat::processInput() {
	inputs = mio->processMouseInput(&x, &y);
	if (inputs->at(QUIT)) {
		screenRunning = false;
		*screen = -1;
	}
}
void VNFormat::update() {

}

void VNFormat::play() {

}

void VNFormat::render() {
	SDL_RenderClear(r1);



	SDL_RenderPresent(r1);
}

void VNFormat::close() {
}