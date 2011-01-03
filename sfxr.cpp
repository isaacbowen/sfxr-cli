#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string.h>
#include <time.h>

#include "sfxr.h"

char* sfxr::keys[] = {
    "PARAM_HEAD",
    "ENV_ATTACK", "ENV_SUSTAIN", "ENV_PUNCH", "ENV_DECAY",
    "BASE_FREQ", "FREQ_LIMIT", "FREQ_RAMP", "FREQ_DRAMP",
    "VIB_STRENGTH", "VIB_SPEED", "VIB_DELAY",
    "ARP_MOD", "ARP_SPEED",
    "DUTY", "DUTY_RAMP",
    "REPEAT_SPEED",
    "PHA_OFFSET", "PHA_RAMP",
    "LPF_FREQ", "LPF_RAMP", "LPF_RESONANCE",
    "HPF_FREQ", "HPF_RAMP",
    "PARAM_COUNT"
};

sfxr::sfxr()
{
    master_vol = 0.05f;
    sound_vol = 0.5f;
    wav_bits = 16;
    wav_freq = 44100;
    filesample = 0.0f;
    fileacc = 0;
    playing_sample = false;
    
    params = std::vector<float>(sfxr::PARAM_COUNT);
}

float* sfxr::getParam(char* key)
{
    for(int i = sfxr::PARAM_HEAD; i < sfxr::PARAM_COUNT; i++)
    {
        if(strcmp(key, sfxr::keys[i]) == 0)
            return &sfxr::params[i];
    }
    return 0L;
}

bool sfxr::setParam(sfxr::Param key, float value)
{
    params[key] = value;
    return true;    
}

bool sfxr::setParam(char* key, float value)
{
    float* param = getParam(key);
    if(param == 0L)
        return false;
    *param = value;
    return true;
}

void sfxr::setWave(sfxr::Wave type)
{
    wave_type = type;
}


// the rest of these are nearly verbatim sfxr 1.1

void sfxr::generate(sfxr::Preset preset)
{
    srand(time(NULL));
    
    switch(preset)
    {
		case sfxr::PRESET_PICKUP: // pickup/coin
			ResetParams();
			params[sfxr::BASE_FREQ]=0.4f+frnd(0.5f);
			params[sfxr::ENV_ATTACK]=0.0f;
			params[sfxr::ENV_SUSTAIN]=frnd(0.1f);
			params[sfxr::ENV_DECAY]=0.1f+frnd(0.4f);
			params[sfxr::ENV_PUNCH]=0.3f+frnd(0.3f);
			if(rnd(1))
			{
				params[sfxr::ARP_SPEED]=0.5f+frnd(0.2f);
				params[sfxr::ARP_MOD]=0.2f+frnd(0.4f);
			}
			break;
		case sfxr::PRESET_LASER: // laser/shoot
			ResetParams();
			wave_type=rnd(2);
			if(wave_type==2 && rnd(1))
				wave_type=rnd(1);
			params[sfxr::BASE_FREQ]=0.5f+frnd(0.5f);
			params[sfxr::FREQ_LIMIT]=params[sfxr::BASE_FREQ]-0.2f-frnd(0.6f);
			if(params[sfxr::FREQ_LIMIT]<0.2f) params[sfxr::FREQ_LIMIT]=0.2f;
			params[sfxr::FREQ_RAMP]=-0.15f-frnd(0.2f);
			if(rnd(2)==0)
			{
				params[sfxr::BASE_FREQ]=0.3f+frnd(0.6f);
				params[sfxr::FREQ_LIMIT]=frnd(0.1f);
				params[sfxr::FREQ_RAMP]=-0.35f-frnd(0.3f);
			}
			if(rnd(1))
			{
				params[sfxr::DUTY]=frnd(0.5f);
				params[sfxr::DUTY_RAMP]=frnd(0.2f);
			}
			else
			{
				params[sfxr::DUTY]=0.4f+frnd(0.5f);
				params[sfxr::DUTY_RAMP]=-frnd(0.7f);
			}
			params[sfxr::ENV_ATTACK]=0.0f;
			params[sfxr::ENV_SUSTAIN]=0.1f+frnd(0.2f);
			params[sfxr::ENV_DECAY]=frnd(0.4f);
			if(rnd(1))
				params[sfxr::ENV_PUNCH]=frnd(0.3f);
			if(rnd(2)==0)
			{
				params[sfxr::PHA_OFFSET]=frnd(0.2f);
				params[sfxr::PHA_RAMP]=-frnd(0.2f);
			}
			if(rnd(1))
				params[sfxr::HPF_FREQ]=frnd(0.3f);
			break;
		case sfxr::PRESET_EXPLOSION: // explosion
			ResetParams();
			wave_type=3;
			if(rnd(1))
			{
				params[sfxr::BASE_FREQ]=0.1f+frnd(0.4f);
				params[sfxr::FREQ_RAMP]=-0.1f+frnd(0.4f);
			}
			else
			{
				params[sfxr::BASE_FREQ]=0.2f+frnd(0.7f);
				params[sfxr::FREQ_RAMP]=-0.2f-frnd(0.2f);
			}
			params[sfxr::BASE_FREQ]*=params[sfxr::BASE_FREQ];
			if(rnd(4)==0)
				params[sfxr::FREQ_RAMP]=0.0f;
			if(rnd(2)==0)
				params[sfxr::REPEAT_SPEED]=0.3f+frnd(0.5f);
			params[sfxr::ENV_ATTACK]=0.0f;
			params[sfxr::ENV_SUSTAIN]=0.1f+frnd(0.3f);
			params[sfxr::ENV_DECAY]=frnd(0.5f);
			if(rnd(1)==0)
			{
				params[sfxr::PHA_OFFSET]=-0.3f+frnd(0.9f);
				params[sfxr::PHA_RAMP]=-frnd(0.3f);
			}
			params[sfxr::ENV_PUNCH]=0.2f+frnd(0.6f);
			if(rnd(1))
			{
				params[sfxr::VIB_STRENGTH]=frnd(0.7f);
				params[sfxr::VIB_SPEED]=frnd(0.6f);
			}
			if(rnd(2)==0)
			{
				params[sfxr::ARP_SPEED]=0.6f+frnd(0.3f);
				params[sfxr::ARP_MOD]=0.8f-frnd(1.6f);
			}
			break;
		case sfxr::PRESET_POWERUP: // powerup
			ResetParams();
			if(rnd(1))
				wave_type=1;
			else
				params[sfxr::DUTY]=frnd(0.6f);
			if(rnd(1))
			{
				params[sfxr::BASE_FREQ]=0.2f+frnd(0.3f);
				params[sfxr::FREQ_RAMP]=0.1f+frnd(0.4f);
				params[sfxr::REPEAT_SPEED]=0.4f+frnd(0.4f);
			}
			else
			{
				params[sfxr::BASE_FREQ]=0.2f+frnd(0.3f);
				params[sfxr::FREQ_RAMP]=0.05f+frnd(0.2f);
				if(rnd(1))
				{
					params[sfxr::VIB_STRENGTH]=frnd(0.7f);
					params[sfxr::VIB_SPEED]=frnd(0.6f);
				}
			}
			params[sfxr::ENV_ATTACK]=0.0f;
			params[sfxr::ENV_SUSTAIN]=frnd(0.4f);
			params[sfxr::ENV_DECAY]=0.1f+frnd(0.4f);
			break;
		case sfxr::PRESET_HIT: // hit/hurt
			ResetParams();
			wave_type=rnd(2);
			if(wave_type==2)
				wave_type=3;
			if(wave_type==0)
				params[sfxr::DUTY]=frnd(0.6f);
			params[sfxr::BASE_FREQ]=0.2f+frnd(0.6f);
			params[sfxr::FREQ_RAMP]=-0.3f-frnd(0.4f);
			params[sfxr::ENV_ATTACK]=0.0f;
			params[sfxr::ENV_SUSTAIN]=frnd(0.1f);
			params[sfxr::ENV_DECAY]=0.1f+frnd(0.2f);
			if(rnd(1))
				params[sfxr::HPF_FREQ]=frnd(0.3f);
			break;
		case sfxr::PRESET_JUMP: // jump
			ResetParams();
			wave_type=0;
			params[sfxr::DUTY]=frnd(0.6f);
			params[sfxr::BASE_FREQ]=0.3f+frnd(0.3f);
			params[sfxr::FREQ_RAMP]=0.1f+frnd(0.2f);
			params[sfxr::ENV_ATTACK]=0.0f;
			params[sfxr::ENV_SUSTAIN]=0.1f+frnd(0.3f);
			params[sfxr::ENV_DECAY]=0.1f+frnd(0.2f);
			if(rnd(1))
				params[sfxr::HPF_FREQ]=frnd(0.3f);
			if(rnd(1))
				params[sfxr::LPF_FREQ]=1.0f-frnd(0.6f);
			break;
		case sfxr::PRESET_BLIP: // blip/select
			ResetParams();
			wave_type=rnd(1);
			if(wave_type==0)
				params[sfxr::DUTY]=frnd(0.6f);
			params[sfxr::BASE_FREQ]=0.2f+frnd(0.4f);
			params[sfxr::ENV_ATTACK]=0.0f;
			params[sfxr::ENV_SUSTAIN]=0.1f+frnd(0.1f);
			params[sfxr::ENV_DECAY]=frnd(0.2f);
			params[sfxr::HPF_FREQ]=0.1f;
			break;
		default:
			break;
    }
}

void sfxr::randomize()
{
    srand(time(NULL));
    
    params[sfxr::BASE_FREQ]=pow(frnd(2.0f)-1.0f, 2.0f);
	if(rnd(1))
		params[sfxr::BASE_FREQ]=pow(frnd(2.0f)-1.0f, 3.0f)+0.5f;
	params[sfxr::FREQ_LIMIT]=0.0f;
	params[sfxr::FREQ_RAMP]=pow(frnd(2.0f)-1.0f, 5.0f);
	if(params[sfxr::BASE_FREQ]>0.7f && params[sfxr::FREQ_RAMP]>0.2f)
		params[sfxr::FREQ_RAMP]=-params[sfxr::FREQ_RAMP];
	if(params[sfxr::BASE_FREQ]<0.2f && params[sfxr::FREQ_RAMP]<-0.05f)
		params[sfxr::FREQ_RAMP]=-params[sfxr::FREQ_RAMP];
	params[sfxr::FREQ_DRAMP]=pow(frnd(2.0f)-1.0f, 3.0f);
	params[sfxr::DUTY]=frnd(2.0f)-1.0f;
	params[sfxr::DUTY_RAMP]=pow(frnd(2.0f)-1.0f, 3.0f);
	params[sfxr::VIB_STRENGTH]=pow(frnd(2.0f)-1.0f, 3.0f);
	params[sfxr::VIB_SPEED]=frnd(2.0f)-1.0f;
	params[sfxr::VIB_DELAY]=frnd(2.0f)-1.0f;
	params[sfxr::ENV_ATTACK]=pow(frnd(2.0f)-1.0f, 3.0f);
	params[sfxr::ENV_SUSTAIN]=pow(frnd(2.0f)-1.0f, 2.0f);
	params[sfxr::ENV_DECAY]=frnd(2.0f)-1.0f;
	params[sfxr::ENV_PUNCH]=pow(frnd(0.8f), 2.0f);
	if(params[sfxr::ENV_ATTACK]+params[sfxr::ENV_SUSTAIN]+params[sfxr::ENV_DECAY]<0.2f)
	{
		params[sfxr::ENV_SUSTAIN]+=0.2f+frnd(0.3f);
		params[sfxr::ENV_DECAY]+=0.2f+frnd(0.3f);
	}
	params[sfxr::LPF_RESONANCE]=frnd(2.0f)-1.0f;
	params[sfxr::LPF_FREQ]=1.0f-pow(frnd(1.0f), 3.0f);
	params[sfxr::LPF_RAMP]=pow(frnd(2.0f)-1.0f, 3.0f);
	if(params[sfxr::LPF_FREQ]<0.1f && params[sfxr::LPF_RAMP]<-0.05f)
		params[sfxr::LPF_RAMP]=-params[sfxr::LPF_RAMP];
	params[sfxr::HPF_FREQ]=pow(frnd(1.0f), 5.0f);
	params[sfxr::HPF_RAMP]=pow(frnd(2.0f)-1.0f, 5.0f);
	params[sfxr::PHA_OFFSET]=pow(frnd(2.0f)-1.0f, 3.0f);
	params[sfxr::PHA_RAMP]=pow(frnd(2.0f)-1.0f, 3.0f);
	params[sfxr::REPEAT_SPEED]=frnd(2.0f)-1.0f;
	params[sfxr::ARP_SPEED]=frnd(2.0f)-1.0f;
	params[sfxr::ARP_MOD]=frnd(2.0f)-1.0f;
}

void sfxr::mutate()
{
    srand(time(NULL));
    
    if(rnd(1)) params[sfxr::BASE_FREQ]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::FREQ_RAMP]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::FREQ_DRAMP]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::DUTY]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::DUTY_RAMP]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::VIB_STRENGTH]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::VIB_SPEED]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::VIB_DELAY]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::ENV_ATTACK]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::ENV_SUSTAIN]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::ENV_DECAY]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::ENV_PUNCH]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::LPF_RESONANCE]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::LPF_FREQ]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::LPF_RAMP]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::HPF_FREQ]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::HPF_RAMP]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::PHA_OFFSET]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::PHA_RAMP]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::REPEAT_SPEED]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::ARP_SPEED]+=frnd(0.1f)-0.05f;
	if(rnd(1)) params[sfxr::ARP_MOD]+=frnd(0.1f)-0.05f;
}

void sfxr::ResetParams()
{
	wave_type=0;

	params[sfxr::BASE_FREQ]=0.3f;
	params[sfxr::FREQ_LIMIT]=0.0f;
	params[sfxr::FREQ_RAMP]=0.0f;
	params[sfxr::FREQ_DRAMP]=0.0f;
	params[sfxr::DUTY]=0.0f;
	params[sfxr::DUTY_RAMP]=0.0f;

	params[sfxr::VIB_STRENGTH]=0.0f;
	params[sfxr::VIB_SPEED]=0.0f;
	params[sfxr::VIB_DELAY]=0.0f;

	params[sfxr::ENV_ATTACK]=0.0f;
	params[sfxr::ENV_SUSTAIN]=0.3f;
	params[sfxr::ENV_DECAY]=0.4f;
	params[sfxr::ENV_PUNCH]=0.0f;

	filter_on=false;
	
	params[sfxr::LPF_RESONANCE]=0.0f;
	params[sfxr::LPF_FREQ]=1.0f;
	params[sfxr::LPF_RAMP]=0.0f;
	params[sfxr::HPF_FREQ]=0.0f;
	params[sfxr::HPF_RAMP]=0.0f;
	
	params[sfxr::PHA_OFFSET]=0.0f;
	params[sfxr::PHA_RAMP]=0.0f;

	params[sfxr::REPEAT_SPEED]=0.0f;

	params[sfxr::ARP_SPEED]=0.0f;
	params[sfxr::ARP_MOD]=0.0f;
}

bool sfxr::LoadSettings(char* filename)
{
	FILE* file=fopen(filename, "rb");
	if(!file)
		return false;

	int version=0;
	fread(&version, 1, sizeof(int), file);
	if(version!=100 && version!=101 && version!=102)
		return false;

	fread(&wave_type, 1, sizeof(int), file);

	sound_vol=0.5f;
	if(version==102)
		fread(&sound_vol, 1, sizeof(float), file);

	fread(&params[sfxr::BASE_FREQ], 1, sizeof(float), file);
	fread(&params[sfxr::FREQ_LIMIT], 1, sizeof(float), file);
	fread(&params[sfxr::FREQ_RAMP], 1, sizeof(float), file);
	if(version>=101)
		fread(&params[sfxr::FREQ_DRAMP], 1, sizeof(float), file);
	fread(&params[sfxr::DUTY], 1, sizeof(float), file);
	fread(&params[sfxr::DUTY_RAMP], 1, sizeof(float), file);

	fread(&params[sfxr::VIB_STRENGTH], 1, sizeof(float), file);
	fread(&params[sfxr::VIB_SPEED], 1, sizeof(float), file);
	fread(&params[sfxr::VIB_DELAY], 1, sizeof(float), file);

	fread(&params[sfxr::ENV_ATTACK], 1, sizeof(float), file);
	fread(&params[sfxr::ENV_SUSTAIN], 1, sizeof(float), file);
	fread(&params[sfxr::ENV_DECAY], 1, sizeof(float), file);
	fread(&params[sfxr::ENV_PUNCH], 1, sizeof(float), file);

	fread(&filter_on, 1, sizeof(bool), file);
	fread(&params[sfxr::LPF_RESONANCE], 1, sizeof(float), file);
	fread(&params[sfxr::LPF_FREQ], 1, sizeof(float), file);
	fread(&params[sfxr::LPF_RAMP], 1, sizeof(float), file);
	fread(&params[sfxr::HPF_FREQ], 1, sizeof(float), file);
	fread(&params[sfxr::HPF_RAMP], 1, sizeof(float), file);
	
	fread(&params[sfxr::PHA_OFFSET], 1, sizeof(float), file);
	fread(&params[sfxr::PHA_RAMP], 1, sizeof(float), file);

	fread(&params[sfxr::REPEAT_SPEED], 1, sizeof(float), file);

	if(version>=101)
	{
		fread(&params[sfxr::ARP_SPEED], 1, sizeof(float), file);
		fread(&params[sfxr::ARP_MOD], 1, sizeof(float), file);
	}

	fclose(file);
	return true;
}

bool sfxr::SaveSettings(char* filename)
{
	FILE* file=fopen(filename, "wb");
	if(!file)
		return false;

	int version=102;
	fwrite(&version, 1, sizeof(int), file);

	fwrite(&wave_type, 1, sizeof(int), file);

	fwrite(&sound_vol, 1, sizeof(float), file);

	fwrite(&params[sfxr::BASE_FREQ], 1, sizeof(float), file);
	fwrite(&params[sfxr::FREQ_LIMIT], 1, sizeof(float), file);
	fwrite(&params[sfxr::FREQ_RAMP], 1, sizeof(float), file);
	fwrite(&params[sfxr::FREQ_DRAMP], 1, sizeof(float), file);
	fwrite(&params[sfxr::DUTY], 1, sizeof(float), file);
	fwrite(&params[sfxr::DUTY_RAMP], 1, sizeof(float), file);

	fwrite(&params[sfxr::VIB_STRENGTH], 1, sizeof(float), file);
	fwrite(&params[sfxr::VIB_SPEED], 1, sizeof(float), file);
	fwrite(&params[sfxr::VIB_DELAY], 1, sizeof(float), file);

	fwrite(&params[sfxr::ENV_ATTACK], 1, sizeof(float), file);
	fwrite(&params[sfxr::ENV_SUSTAIN], 1, sizeof(float), file);
	fwrite(&params[sfxr::ENV_DECAY], 1, sizeof(float), file);
	fwrite(&params[sfxr::ENV_PUNCH], 1, sizeof(float), file);

	fwrite(&filter_on, 1, sizeof(bool), file);
	fwrite(&params[sfxr::LPF_RESONANCE], 1, sizeof(float), file);
	fwrite(&params[sfxr::LPF_FREQ], 1, sizeof(float), file);
	fwrite(&params[sfxr::LPF_RAMP], 1, sizeof(float), file);
	fwrite(&params[sfxr::HPF_FREQ], 1, sizeof(float), file);
	fwrite(&params[sfxr::HPF_RAMP], 1, sizeof(float), file);
	
	fwrite(&params[sfxr::PHA_OFFSET], 1, sizeof(float), file);
	fwrite(&params[sfxr::PHA_RAMP], 1, sizeof(float), file);

	fwrite(&params[sfxr::REPEAT_SPEED], 1, sizeof(float), file);

	fwrite(&params[sfxr::ARP_SPEED], 1, sizeof(float), file);
	fwrite(&params[sfxr::ARP_MOD], 1, sizeof(float), file);

	fclose(file);
	return true;
}

void sfxr::ResetSample(bool restart)
{
	if(!restart)
		phase=0;
	fperiod=100.0/(params[sfxr::BASE_FREQ]*params[sfxr::BASE_FREQ]+0.001);
	period=(int)fperiod;
	fmaxperiod=100.0/(params[sfxr::FREQ_LIMIT]*params[sfxr::FREQ_LIMIT]+0.001);
	fslide=1.0-pow((double)params[sfxr::FREQ_RAMP], 3.0)*0.01;
	fdslide=-pow((double)params[sfxr::FREQ_DRAMP], 3.0)*0.000001;
	square_duty=0.5f-params[sfxr::DUTY]*0.5f;
	square_slide=-params[sfxr::DUTY_RAMP]*0.00005f;
	if(params[sfxr::ARP_MOD]>=0.0f)
		arp_mod=1.0-pow((double)params[sfxr::ARP_MOD], 2.0)*0.9;
	else
		arp_mod=1.0+pow((double)params[sfxr::ARP_MOD], 2.0)*10.0;
	arp_time=0;
	arp_limit=(int)(pow(1.0f-params[sfxr::ARP_SPEED], 2.0f)*20000+32);
	if(params[sfxr::ARP_SPEED]==1.0f)
		arp_limit=0;
	if(!restart)
	{
		// reset filter
		fltp=0.0f;
		fltdp=0.0f;
		fltw=pow(params[sfxr::LPF_FREQ], 3.0f)*0.1f;
		fltw_d=1.0f+params[sfxr::LPF_RAMP]*0.0001f;
		fltdmp=5.0f/(1.0f+pow(params[sfxr::LPF_RESONANCE], 2.0f)*20.0f)*(0.01f+fltw);
		if(fltdmp>0.8f) fltdmp=0.8f;
		fltphp=0.0f;
		flthp=pow(params[sfxr::HPF_FREQ], 2.0f)*0.1f;
		flthp_d=1.0+params[sfxr::HPF_RAMP]*0.0003f;
		// reset vibrato
		vib_phase=0.0f;
		vib_speed=pow(params[sfxr::VIB_SPEED], 2.0f)*0.01f;
		vib_amp=params[sfxr::VIB_STRENGTH]*0.5f;
		// reset envelope
		env_vol=0.0f;
		env_stage=0;
		env_time=0;
		env_length[0]=(int)(params[sfxr::ENV_ATTACK]*params[sfxr::ENV_ATTACK]*100000.0f);
		env_length[1]=(int)(params[sfxr::ENV_SUSTAIN]*params[sfxr::ENV_SUSTAIN]*100000.0f);
		env_length[2]=(int)(params[sfxr::ENV_DECAY]*params[sfxr::ENV_DECAY]*100000.0f);

		fphase=pow(params[sfxr::PHA_OFFSET], 2.0f)*1020.0f;
		if(params[sfxr::PHA_OFFSET]<0.0f) fphase=-fphase;
		fdphase=pow(params[sfxr::PHA_RAMP], 2.0f)*1.0f;
		if(params[sfxr::PHA_RAMP]<0.0f) fdphase=-fdphase;
		iphase=abs((int)fphase);
		ipp=0;
		for(int i=0;i<1024;i++)
			phaser_buffer[i]=0.0f;

		for(int i=0;i<32;i++)
			noise_buffer[i]=frnd(2.0f)-1.0f;

		rep_time=0;
		rep_limit=(int)(pow(1.0f-params[sfxr::REPEAT_SPEED], 2.0f)*20000+32);
		if(params[sfxr::REPEAT_SPEED]==0.0f)
			rep_limit=0;
	}
}

void sfxr::PlaySample()
{
	ResetSample(false);
	playing_sample=true;
}

void sfxr::SynthSample(int length, float* buffer, FILE* file)
{
	for(int i=0;i<length;i++)
	{
		if(!playing_sample)
			break;

		rep_time++;
		if(rep_limit!=0 && rep_time>=rep_limit)
		{
			rep_time=0;
			ResetSample(true);
		}

		// frequency envelopes/arpeggios
		arp_time++;
		if(arp_limit!=0 && arp_time>=arp_limit)
		{
			arp_limit=0;
			fperiod*=arp_mod;
		}
		fslide+=fdslide;
		fperiod*=fslide;
		if(fperiod>fmaxperiod)
		{
			fperiod=fmaxperiod;
			if(params[sfxr::FREQ_LIMIT]>0.0f)
				playing_sample=false;
		}
		float rfperiod=fperiod;
		if(vib_amp>0.0f)
		{
			vib_phase+=vib_speed;
			rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
		}
		period=(int)rfperiod;
		if(period<8) period=8;
		square_duty+=square_slide;
		if(square_duty<0.0f) square_duty=0.0f;
		if(square_duty>0.5f) square_duty=0.5f;		
		// volume envelope
		env_time++;
		if(env_time>env_length[env_stage])
		{
			env_time=0;
			env_stage++;
			if(env_stage==3)
				playing_sample=false;
		}
		if(env_stage==0)
			env_vol=(float)env_time/env_length[0];
		if(env_stage==1)
			env_vol=1.0f+pow(1.0f-(float)env_time/env_length[1], 1.0f)*2.0f*params[sfxr::ENV_PUNCH];
		if(env_stage==2)
			env_vol=1.0f-(float)env_time/env_length[2];

		// phaser step
		fphase+=fdphase;
		iphase=abs((int)fphase);
		if(iphase>1023) iphase=1023;

		if(flthp_d!=0.0f)
		{
			flthp*=flthp_d;
			if(flthp<0.00001f) flthp=0.00001f;
			if(flthp>0.1f) flthp=0.1f;
		}

		float ssample=0.0f;
		for(int si=0;si<8;si++) // 8x supersampling
		{
			float sample=0.0f;
			phase++;
			if(phase>=period)
			{
//				phase=0;
				phase%=period;
				if(wave_type==3)
					for(int i=0;i<32;i++)
						noise_buffer[i]=frnd(2.0f)-1.0f;
			}
			// base waveform
			float fp=(float)phase/period;
			switch(wave_type)
			{
			case 0: // square
				if(fp<square_duty)
					sample=0.5f;
				else
					sample=-0.5f;
				break;
			case 1: // sawtooth
				sample=1.0f-fp*2;
				break;
			case 2: // sine
				sample=(float)sin(fp*2*PI);
				break;
			case 3: // noise
				sample=noise_buffer[phase*32/period];
				break;
			}
			// lp filter
			float pp=fltp;
			fltw*=fltw_d;
			if(fltw<0.0f) fltw=0.0f;
			if(fltw>0.1f) fltw=0.1f;
			if(params[sfxr::LPF_FREQ]!=1.0f)
			{
				fltdp+=(sample-fltp)*fltw;
				fltdp-=fltdp*fltdmp;
			}
			else
			{
				fltp=sample;
				fltdp=0.0f;
			}
			fltp+=fltdp;
			// hp filter
			fltphp+=fltp-pp;
			fltphp-=fltphp*flthp;
			sample=fltphp;
			// phaser
			phaser_buffer[ipp&1023]=sample;
			sample+=phaser_buffer[(ipp-iphase+1024)&1023];
			ipp=(ipp+1)&1023;
			// final accumulation and envelope application
			ssample+=sample*env_vol;
		}
		ssample=ssample/8*master_vol;

		ssample*=2.0f*sound_vol;

		if(buffer!=NULL)
		{
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;
			*buffer++=ssample;
		}
		if(file!=NULL)
		{
			// quantize depending on format
			// accumulate/count to accomodate variable sample rate?
			ssample*=4.0f; // arbitrary gain to get reasonable output volume...
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;
			filesample+=ssample;
			fileacc++;
			if(wav_freq==44100 || fileacc==2)
			{
				filesample/=fileacc;
				fileacc=0;
				if(wav_bits==16)
				{
					short isample=(short)(filesample*32000);
					fwrite(&isample, 1, 2, file);
				}
				else
				{
					unsigned char isample=(unsigned char)(filesample*127+128);
					fwrite(&isample, 1, 1, file);
				}
				filesample=0.0f;
			}
			file_sampleswritten++;
		}
	}
}

bool sfxr::ExportWAV(char* filename)
{
	FILE* foutput=fopen(filename, "wb");
	if(!foutput)
		return false;
	// write wav header
	char string[32];
	unsigned int dword=0;
	unsigned short word=0;
	fwrite("RIFF", 4, 1, foutput); // "RIFF"
	dword=0;
	fwrite(&dword, 1, 4, foutput); // remaining file size
	fwrite("WAVE", 4, 1, foutput); // "WAVE"

	fwrite("fmt ", 4, 1, foutput); // "fmt "
	dword=16;
	fwrite(&dword, 1, 4, foutput); // chunk size
	word=1;
	fwrite(&word, 1, 2, foutput); // compression code
	word=1;
	fwrite(&word, 1, 2, foutput); // channels
	dword=wav_freq;
	fwrite(&dword, 1, 4, foutput); // sample rate
	dword=wav_freq*wav_bits/8;
	fwrite(&dword, 1, 4, foutput); // bytes/sec
	word=wav_bits/8;
	fwrite(&word, 1, 2, foutput); // block align
	word=wav_bits;
	fwrite(&word, 1, 2, foutput); // bits per sample

	fwrite("data", 4, 1, foutput); // "data"
	dword=0;
	int foutstream_datasize=ftell(foutput);
	fwrite(&dword, 1, 4, foutput); // chunk size

	// write sample data
	mute_stream=true;
	file_sampleswritten=0;
	filesample=0.0f;
	fileacc=0;
	PlaySample();
	while(playing_sample)
		SynthSample(256, NULL, foutput);
	mute_stream=false;

	// seek back to header and write size info
	fseek(foutput, 4, SEEK_SET);
	dword=0;
	dword=foutstream_datasize-4+file_sampleswritten*wav_bits/8;
	fwrite(&dword, 1, 4, foutput); // remaining file size
	fseek(foutput, foutstream_datasize, SEEK_SET);
	dword=file_sampleswritten*wav_bits/8;
	fwrite(&dword, 1, 4, foutput); // chunk size (data)
	fclose(foutput);
	
	return true;
}
