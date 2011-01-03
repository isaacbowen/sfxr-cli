#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "sfxr.cpp"
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;

void print_help(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        print_help(argc, argv);
        return 1;
    }

    sfxr sfxr_object = sfxr();

    for(int i = 0; i < argc; i++)
    {
        // help
        if(strcmp(argv[i], "--help") == 0)
        {
            print_help(argc, argv);
            return 0;
        }
        
        // load settings
        if(strcmp(argv[i], "--load") == 0 && argc >= i+1)
        {
            if(!sfxr_object.LoadSettings(argv[i+1]))
            {
                cerr << "Failed to load settings from " << argv[i+1] << endl;
                return 1;
            }
            i++;
            continue;
        }
        
        // save settings
        if(strcmp(argv[i], "--save") == 0 && argc >= i+1)
        {
            if(!sfxr_object.SaveSettings(argv[i+1]))
            {
                cerr << "Failed to save settings to " << argv[i+1] << endl;
                return 1;
            }
            i++;
            continue;
        }
        
        // export to wav
        if(strcmp(argv[i], "--export") == 0 && argc >= i+1)
        {
            if(!sfxr_object.ExportWAV(argv[i+1]))
            {
                cerr << "Failed to export to " << argv[i+1] << endl;
                return 1;
            }
            i++;
            continue;
        }
        
        // randomize
        if(strcmp(argv[i], "--randomize") == 0)
            sfxr_object.randomize();
        
        // mutate
        if(strcmp(argv[i], "--randomize") == 0)
            sfxr_object.mutate();
        
        // generate
        if(strcmp(argv[i], "--generate-pickup") == 0)
            sfxr_object.generate(sfxr::PRESET_PICKUP);
        else if(strcmp(argv[i], "--generate-laser") == 0)
            sfxr_object.generate(sfxr::PRESET_LASER);
        else if(strcmp(argv[i], "--generate-explosion") == 0)
            sfxr_object.generate(sfxr::PRESET_EXPLOSION);
        else if(strcmp(argv[i], "--generate-powerup") == 0)
            sfxr_object.generate(sfxr::PRESET_POWERUP);
        else if(strcmp(argv[i], "--generate-hit") == 0)
            sfxr_object.generate(sfxr::PRESET_HIT);
        else if(strcmp(argv[i], "--generate-jump") == 0)
            sfxr_object.generate(sfxr::PRESET_JUMP);
        else if(strcmp(argv[i], "--generate-blip") == 0)
            sfxr_object.generate(sfxr::PRESET_BLIP);
        
        // set wave function
        if(strcmp(argv[i], "--wave-square") == 0)
            sfxr_object.setWave(sfxr::WAVE_SQUARE);
        else if(strcmp(argv[i], "--wave-sawtooth"))
            sfxr_object.setWave(sfxr::WAVE_SAWTOOTH);
        else if(strcmp(argv[i], "--wave-sine"))
            sfxr_object.setWave(sfxr::WAVE_SINE);
        else if(strcmp(argv[i], "--wave-noise"))
            sfxr_object.setWave(sfxr::WAVE_NOISE);
        
        // set param
        if(strcmp(argv[i], "--set") == 0 && argc >= i + 2)
        {
            float value;
            if(EOF == sscanf(argv[i+2], "%f", &value))
            {
                cerr << "Invalid value " << argv[i+2] << endl;
                return 1;
            }
            else if(!sfxr_object.setParam(argv[i+1], value))
            {
                cerr << "Unknown param " << argv[i+1] << endl;
                return 1;
            }
            
            i += 2;
            continue;
        }
    }
}

void print_help(int argc, char* argv[])
{
    cout << "Usage: " << argv[0] << endl;
    cout << "  [--set PARAM1 VALUE1 [--set PARAM2 VALUE2 [...]]]" << endl;
    cout << "  [--randomize]" << endl;
    cout << "  [--mutate]" << endl;
    cout << "  [--load SETTINGSFILE]" << endl;
    cout << "  [--export WAVFILE]" << endl;
    
    cout << endl << "Parameters:" << endl;
    for(int i = sfxr::PARAM_HEAD+1; i < sfxr::PARAM_COUNT; i++)
    {
        cout << "  " << sfxr::keys[i] << endl;
    }
}
