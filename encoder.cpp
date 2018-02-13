#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <queue>
#include <cmath>
#include <math.h>
#include <fstream>
#include <sstream>//ostringstream
#include <iostream>
#include <string>
#include <iomanip> //setw()

const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;

struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper
{
private:
    double v;
    std::queue<BeepObject> beeps;
public:
    Beeper();
    ~Beeper();
    void beep(double freq, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};

void audio_callback(void*, Uint8*, int);

Beeper::Beeper()
{
    SDL_AudioSpec desiredSpec;

    desiredSpec.freq = FREQUENCY;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;

    // you might want to look for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    // start play audio
    SDL_PauseAudio(0);
}

Beeper::~Beeper()
{
    SDL_CloseAudio();
}

void Beeper::generateSamples(Sint16 *stream, int length)
{
    int i = 0;
    while (i < length) {

        if (beeps.empty()) {
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        BeepObject& bo = beeps.front();

        int samplesToDo = std::min(i + bo.samplesLeft, length);
        bo.samplesLeft -= samplesToDo - i;

        while (i < samplesToDo) {
            stream[i] = AMPLITUDE * std::sin(v * 2 * M_PI / FREQUENCY);
            i++;
            v += bo.freq;
        }

        if (bo.samplesLeft == 0) {
            beeps.pop();
        }
    }
}

void Beeper::beep(double freq, int duration)
{
    BeepObject bo;
    bo.freq = freq;
    bo.samplesLeft = duration * FREQUENCY / 1000;

    SDL_LockAudio();
    beeps.push(bo);
    SDL_UnlockAudio();
}

void Beeper::wait()
{
    int size;
    do {
        SDL_Delay(20);
        SDL_LockAudio();
        size = beeps.size();
        SDL_UnlockAudio();
    } while (size > 0);

}

void audio_callback(void *_beeper, Uint8 *_stream, int _length)
{
    Sint16 *stream = (Sint16*) _stream;
    int length = _length / 2;
    Beeper* beeper = (Beeper*) _beeper;

    beeper->generateSamples(stream, length);
}

using namespace std;

int main(int argc, char* argv[]){

	//freq notes

	int mul=stoi(argv[3]); //multiplicator to change the scale

	double notes[]={
		16.35*mul,
		17.32*mul,
		18.35*mul,
		19.45*mul,
		20.60*mul,
		21.83*mul,
		23.12*mul,
		24.50*mul,
		25.96*mul,
		27.50*mul,
		29.14*mul,
		30.87*mul,
		32.70*mul,
		34.65*mul,
		36.71*mul,
		38.89*mul
	};

	//SDL and beeper config
	SDL_Init(SDL_INIT_AUDIO);

	int p;
	if(strcmp(argv[2],"f")==0){
		p=stoi(argv[5]);
	}else{
		p=50;
	}
	int l=100;
	double pp = 440;
	double ll = 340;
	double sp = 0;

	Beeper b;

	unsigned char x;
	ifstream input(argv[1], ios::binary);
	input >> noskipws;
	string dds[1000];
	int dC=0; //datacounter

	ofstream hexfile;
	string hname;
	hname.append(argv[1]);
	hname.append(".hex");
	hexfile.open(hname);
	hname.clear();
	int hC=0;
	while (input >> x) {
		//cout << hex << setw(2) << setfill('0') << (int)x;
		ostringstream dat;
		dat << hex << setw(2) << setfill('0') << (int)x;
		string dd=dat.str();
		//save to hex file
		hexfile << dd;
		if(hC>=29){
			hexfile << endl;
			hC=0;
		}else{
			hC++;
		}
		//cout << dd << endl;
		if(dds[dC].size()>=1300000){
			dC++;
		}else{
			dds[dC].append(dd);
		}
		//cout << dds[dC].size() << endl;// << dds[dC].length();
	}
	hexfile.close();

	//int s=(int)sizeof(dds)/sizeof(dds[0]);

	bool show;
	if(argc<5){
		show=true;
	}else{
		if(strcmp(argv[4],"false")==0 || strcmp(argv[4],"f")==0){
			show=false;
		}else{
			show=true;
		}
	}

	//string dataOutA[1000];

	//loop in all array elements
	//for(int h=0;h<(sizeof(dds)/sizeof(dds[0]));h++){
	for(int h=0;h<dC+1;h++){

		int s=dds[h].length();
		char data[s];

		int co=0;

		for(int i=0;i<s;i++){
			data[i]=(char)dds[h][i];
			if(show){
				cout << data[i];
				co++;
				if(co==60){
					cout << endl;
					co=0;
				}
			}
		}
		dds[h].clear();

		//string dataOut[s];

		bool pS=true;
		//enable sound
		if(strcmp(argv[6],"f")==0){
			pS=false;
		}

		for(int k=0;k<s;k++){
//cout << "Debbugger\n" << data[k];
//cin.ignore();
//return 0;
			switch(data[k]){
				case '0':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("-----");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<5;i++){
								b.beep(ll,l);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[0];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[0],p);
							b.wait();
						}
					}
				break;
				case '1':
					if(strcmp(argv[2],"m")==0){
						dds[h].append(".----");
						dds[h].append("/");
						if(pS){
							b.beep(pp,p);
							b.wait();
							for(int i=0;i<4;i++){
								b.beep(ll,l);
								b.wait();
							}
						}
				}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[1];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[1],p);
							b.wait();
						}
					}
				break;
				case '2':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("..---");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<2;i++){
								b.beep(pp,p);
								b.wait();
							}
							for(int i=0;i<3;i++){
								b.beep(ll,l);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[2];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[2],p);
							b.wait();
						}
					}
				break;
				case '3':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("...--");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<3;i++){
								b.beep(pp,p);
								b.wait();
							}
							for(int i=0;i<2;i++){
								b.beep(ll,l);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[3];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[3],p);
							b.wait();
						}
					}
				break;
				case '4':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("....-");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<4;i++){
								b.beep(pp,p);
								b.wait();
							}
							b.beep(ll,l);
							b.wait();
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[4];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[4],p);
							b.wait();
						}
					}
				break;
				case '5':
					if(strcmp(argv[2],"m")==0){
						dds[h].append(".....");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<5;i++){
								b.beep(pp,p);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[5];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[5],p);
							b.wait();
						}
					}
				break;
				case '6':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("-....");
						dds[h].append("/");
						if(pS){
							b.beep(ll,l);
							b.wait();
							for(int i=0;i<4;i++){
								b.beep(pp,p);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[2];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[2],p);
							b.wait();
						}
					}
				break;
				case '7':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("--...");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<2;i++){
								b.beep(ll,l);
								b.wait();
							}
							for(int i=0;i<3;i++){
								b.beep(pp,p);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[7];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[7],p);
							b.wait();
						}
					}
				break;
				case '8':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("---..");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<3;i++){
								b.beep(ll,l);
								b.wait();
							}
							for(int i=0;i<2;i++){
								b.beep(pp,p);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[8];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[8],p);
							b.wait();
						}
					}
				break;
				case '9':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("----.");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<4;i++){
								b.beep(ll,l);
								b.wait();
							}
							b.beep(ll,l);
							b.wait();
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[9];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[9],p);
							b.wait();
						}
					}
				break;
				case 'A': case 'a':
					if(strcmp(argv[2],"m")==0){
						dds[h].append(".-");
						dds[h].append("/");
						if(pS){
							b.beep(pp,p);
							b.wait();
							b.beep(ll,l);
							b.wait();
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[10];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[10],p);
							b.wait();
						}
					}
				break;
				case 'B': case 'b':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("-...");
						dds[h].append("/");
						if(pS){
							b.beep(ll,l);
							b.wait();
							for(int i=0;i<3;i++){
								b.beep(pp,p);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[11];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[11],p);
							b.wait();
						}
					}
				break;
				case 'C': case 'c':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("-.-.");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<2;i++){
								b.beep(ll,l);
								b.wait();
								b.beep(pp,p);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[12];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[12],p);
							b.wait();
						}
					}
				break;
				case 'D': case 'd':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("-..");
						dds[h].append("/");
						if(pS){
							b.beep(ll,l);
							b.wait();
							for(int i=0;i<2;i++){
								b.beep(pp,p);
								b.wait();
							}
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[13];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[13],p);
							b.wait();
						}
					}
				break;
				case 'E': case 'e':
					if(strcmp(argv[2],"m")==0){
						dds[h].append(".");
						dds[h].append("/");
						if(pS){
							b.beep(pp,p);
							b.wait();
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[14];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[14],p);
							b.wait();
						}
					}
				break;
				case 'F': case 'f':
					if(strcmp(argv[2],"m")==0){
						dds[h].append("..-.");
						dds[h].append("/");
						if(pS){
							for(int i=0;i<2;i++){
								b.beep(pp,p);
								b.wait();
							}
							b.beep(ll,l);
							b.wait();
							b.beep(pp,p);
							b.wait();
						}
					}else if(strcmp(argv[2],"f")==0){
						ostringstream noteStr; //convert int to string
						noteStr << notes[15];
						dds[h].append(noteStr.str());
						dds[h].append("\n");
						if(pS){
							b.beep(notes[15],p);
							b.wait();
						}
					}
				break;
				case 't':
				break;
			}
			if(pS){
				b.beep(sp,p);
				b.wait();
			}
		}

//		b.wait();

		//delete dataOutA
/*		for(int j=0;j<s;j++){
			dataOutA[h].append(dataOut[j]);
			if(strcmp(argv[2],"f")==0){
				dataOutA[h].append("\n");
			}
		}*/
	}
	//save to file
	ofstream outfile;
	if(strcmp(argv[2],"m")==0){
		outfile.open("morseout.txt");
	}else if(strcmp(argv[2],"f")==0){
		outfile.open("freqout.txt");
	}
	int co=0;
//	for(int j=0;j<(sizeof(dataOutA)/sizeof(dataOutA[0]));j++){
	for(int j=0;j<dC+1;j++){
		if(strcmp(argv[2],"m")==0){
			co++;
			outfile << dds[j];
		}else if(strcmp(argv[2],"f")==0){
			outfile << dds[j];
		}
	}
	outfile << endl;
	outfile.close();

	cout << "\nDone.\n";

	return 0;
}
