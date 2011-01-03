#include <vector>
#include <stdio.h>

#define rnd(n) (rand()%(n+1))
#define PI 3.14159265f

float frnd(float range)
{
	return (float)rnd(10000)/10000*range;
}

class sfxr
{
    public:
        enum Param
        {
            PARAM_HEAD = 0,
            ENV_ATTACK, ENV_SUSTAIN, ENV_PUNCH, ENV_DECAY,
            BASE_FREQ, FREQ_LIMIT, FREQ_RAMP, FREQ_DRAMP,
            VIB_STRENGTH, VIB_SPEED, VIB_DELAY,
            ARP_MOD, ARP_SPEED,
            DUTY, DUTY_RAMP,
            REPEAT_SPEED,
            PHA_OFFSET, PHA_RAMP,
            LPF_FREQ, LPF_RAMP, LPF_RESONANCE,
            HPF_FREQ, HPF_RAMP,
            PARAM_COUNT
        };
        
        enum Wave
        {
            WAVE_SQUARE, WAVE_SAWTOOTH, WAVE_SINE, WAVE_NOISE
        };
        
        enum Preset
        {
            PRESET_PICKUP, PRESET_LASER, PRESET_EXPLOSION, PRESET_POWERUP, PRESET_HIT, PRESET_JUMP, PRESET_BLIP
        };
        
        sfxr();
        
        bool setParam(Param key, float value);
        bool setParam(char* key, float value);
        void setWave(Wave type);
        float* getParam(char* key);
        
        void ResetParams();
        bool LoadSettings(char* filename);
        bool SaveSettings(char* filename);
        void ResetSample(bool restart);
        void SynthSample(int length, float* buffer, FILE* file);
        void PlaySample();
        bool ExportWAV(char* filename);
        
        void generate(Preset preset);
        void randomize();
        void mutate();
        
        static char* keys[];
        
    private:
        int wave_type;
        std::vector<float> params;
        
        bool filter_on;
        bool playing_sample;
        bool mute_stream;
        
        float master_vol;
        float sound_vol;
        
        int phase;
        double fperiod;
        double fmaxperiod;
        double fslide;
        double fdslide;
        int period;
        float square_duty;
        float square_slide;
        int env_stage;
        int env_time;
        int env_length[3];
        float env_vol;
        float fphase;
        float fdphase;
        int iphase;
        float phaser_buffer[1024];
        int ipp;
        float noise_buffer[32];
        float fltp;
        float fltdp;
        float fltw;
        float fltw_d;
        float fltdmp;
        float fltphp;
        float flthp;
        float flthp_d;
        float vib_phase;
        float vib_speed;
        float vib_amp;
        int rep_time;
        int rep_limit;
        int arp_time;
        int arp_limit;
        double arp_mod;
        
        int wav_bits;
        int wav_freq;
        
        int file_sampleswritten;
        float filesample;
        int fileacc;
};
