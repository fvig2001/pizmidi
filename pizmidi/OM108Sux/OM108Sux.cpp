#include "OM108Sux.hpp"

//notes are based on sharps 
#define NOTE_FS1 30 //
#define NOTE_G1  31 //
#define NOTE_GS1 32 //root Abdim7
#define NOTE_A1  33 //
#define NOTE_AS1 34 //root Bbdim7
#define NOTE_B1  35 //
#define NOTE_C2  36 //
#define NOTE_CS2 37 //root dbdim7
#define NOTE_D2  38
#define NOTE_DS2 39 //root EbDim7
#define NOTE_E2  40
#define NOTE_F2  41
#define NOTE_FS2 42
#define NOTE_G2  43

//Chord notes are based on midi monitor names
#define NOTE_F3  53 //
#define NOTE_FS3 54 //Ebdim7
#define NOTE_G3  55 //
#define NOTE_GS3 56 //dbdim7, abdim7, FDim7
#define NOTE_A3  57 //
#define NOTE_AS3 58 //EbDim7, BbDim7
#define NOTE_B3  59 //abdim7
#define NOTE_C4  60 //Fdim7
#define NOTE_CS4 61 //dbdim7, BbDim7
#define NOTE_D4  62 //
#define NOTE_DS4 63 //Ebdim7
#define NOTE_E4  64 //dbdim7
#define NOTE_F4  65 //abdim7, BbDim7, Fdim7
#define NOTE_FS4  66 //
#define NOTE_G4 67 //
#define NOTE_GS4 68 //
#define NOTE_A4  69 //
#define NOTE_AS4 70 //
#define NOTE_B4   71 //

#define BASS_CHANNEL 2
#define CHORD_CHANNEL 3
int incDim7::id = 0;

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new OM108Sux(audioMaster);
}

OM108SuxProgram::OM108SuxProgram()
{
    // default Program Values
    fPower   = 1.f;
    //initialize constants


    vst_strncpy(name, "OM-108 Dim7 Fixer", kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------
OM108Sux::OM108Sux(audioMasterCallback audioMaster)
    : PizMidi(audioMaster, kNumPrograms, kNumParams), programs(0)
{
    dbg("OM108Sux: init");
    programs = new OM108SuxProgram[numPrograms];
    
    DbDim7 = incDim7(NOTE_CS2, NOTE_CS4, NOTE_E4, NOTE_AS3, NOTE_G4);
    AbDim7 = incDim7(NOTE_GS1, NOTE_GS3, NOTE_B3, NOTE_F4, NOTE_D4);
    EbDim7 = incDim7(NOTE_DS2, NOTE_DS4, NOTE_FS3, NOTE_C4, NOTE_A4);
    BbDim7 = incDim7(NOTE_AS1, NOTE_AS3, NOTE_CS4, NOTE_G3, NOTE_E4);
    FDim7  = incDim7(NOTE_F2, NOTE_F4, NOTE_GS3, NOTE_D4, NOTE_B3);
    CDim7  = incDim7(NOTE_C2, NOTE_C4, NOTE_DS4, NOTE_A3, NOTE_FS4);
    GDim7  = incDim7(NOTE_G1, NOTE_G3, NOTE_AS3, NOTE_E4, NOTE_CS4);
    DDim7  = incDim7(NOTE_D2, NOTE_D4, NOTE_F4, NOTE_B3, NOTE_GS4);
    ADim7  = incDim7(NOTE_A1, NOTE_A3, NOTE_C4, NOTE_FS3, NOTE_DS4);
    EDim7  = incDim7(NOTE_E2, NOTE_E4, NOTE_G3, NOTE_CS4, NOTE_AS4);
    BDim7  = incDim7(NOTE_B1, NOTE_B3, NOTE_D4, NOTE_GS3, NOTE_F4);
    FSDim7 = incDim7(NOTE_FS1, NOTE_FS3, NOTE_A3, NOTE_DS4, NOTE_C4);
    
    if (programs)
    {
        CFxBank* defaultBank = new CFxBank(numPrograms, numParams);
        if (readDefaultBank(PLUG_NAME, defaultBank))
        {
            if ((VstInt32) defaultBank->GetFxID() == PLUG_IDENT)
            {
                for (int i = 0; i < numPrograms; i++)
                {
                    programs[i].fPower   = defaultBank->GetProgParm(i, 1);
                    strcpy(programs[i].name, defaultBank->GetProgramName(i));
                }
            }
        }
        else
        {
            // built-in programs
            for (int i = 0; i < numPrograms; i++)
            {
                sprintf(programs[i].name, "Program %d", i + 1);
            }
        }
        setProgram(0);
    }

    init();
}

//-----------------------------------------------------------------------------------------
OM108Sux::~OM108Sux()
{
    if (programs)
    {
        delete[] programs;
    }
}

//------------------------------------------------------------------------
void OM108Sux::setProgram(VstInt32 program)
{
    OM108SuxProgram* ap = &programs[program];

    curProgram = program;
    setParameter(kPower, ap->fPower);
}

//------------------------------------------------------------------------
void OM108Sux::setProgramName(char* name)
{
    vst_strncpy(programs[curProgram].name, name, kVstMaxProgNameLen);
}

//------------------------------------------------------------------------
void OM108Sux::getProgramName(char* name)
{
    vst_strncpy(name, programs[curProgram].name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
bool OM108Sux::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text)
{
    if (index < numPrograms)
    {
        vst_strncpy(text, programs[index].name, kVstMaxProgNameLen);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------
void OM108Sux::setParameter(VstInt32 index, float value)
{
    OM108SuxProgram* ap = &programs[curProgram];

    switch (index)
    {
        case kPower:
            fPower = ap->fPower = value;
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------------------
float OM108Sux::getParameter(VstInt32 index)
{
    float v = 0;

    switch (index)
    {
        case kPower:
            v = fPower;
            break;
        default:
            break;
    }
    return v;
}

//-----------------------------------------------------------------------------------------
void OM108Sux::getParameterName(VstInt32 index, char* label)
{
    switch (index)
    {
        case kPower:
            vst_strncpy(label, "Power", kVstMaxParamStrLen);
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------------------
void OM108Sux::getParameterDisplay(VstInt32 index, char* text)
{
    switch (index)
    {
        case kPower:
            if (fPower < 0.5f)
            {
                vst_strncpy(text, "Off", kVstMaxParamStrLen);
            }
            else
            {
                vst_strncpy(text, "On", kVstMaxParamStrLen);
            }
            break;
        default:
            break;
    }
}

void OM108Sux::checkNoteOn(incDim7& chord, VstMidiEventVec* outputs, int note, VstMidiEvent tomod)
{
    dbg("turning On:");
    dbg(note);
    dbg(chord.myID);
    playingNotes[note] = true;
    if (chord.notes[0] == note || chord.notes[1] == note || chord.notes[2] == note || chord.base == note)
    {
        dbg("note on is in this chord:");
        dbg(note);
        dbg("ID of chord is: ");
        dbg(chord.myID);
        if (playingNotes[chord.notes[0]] && playingNotes[chord.notes[1]] && playingNotes[chord.notes[2]] && playingNotes[chord.base])
        {
            chord.isOn = true;
            VstMidiEvent tomodNew = tomod; // copy
            tomodNew.midiData[1] = chord.missingNote & 0x7f; //add missing note
            outputs->push_back(tomodNew);
            dbg("checkNoteOn:");
            dbg(chord.missingNote);
        }
    }

}

void OM108Sux::checkNoteOff(incDim7& chord, VstMidiEventVec* outputs, int note, VstMidiEvent tomod)
{

    dbg("turning Off:");
    dbg(note)
    playingNotes[note] = false;
    if (!chord.isOn)
    {
        return;
    }
    if (chord.notes[0] == note || chord.notes[1] == note || chord.notes[2] == note || chord.base == note)
    {
        
        if (!(playingNotes[chord.notes[0]] && playingNotes[chord.notes[1]] && playingNotes[chord.notes[2]] && playingNotes[chord.base]))
        {

            dbg("note off is in this chord:");
            dbg(note);
            dbg("ID of chord is: ");
            dbg(chord.myID);
            chord.isOn            = false;
            VstMidiEvent tomodNew = tomod;                    // copy
            tomodNew.midiData[1]  = chord.missingNote & 0x7f; //remove missing note
            outputs->push_back(tomodNew);
        }
    }
}

void OM108Sux::resetNoteMap()
{
    for (int i = NOTE_G1; i < NOTE_B4; i++)
    {
        playingNotes[i] = false;
    }
    //reset state
    DbDim7.isOn = false;
    AbDim7.isOn = false;
    EbDim7.isOn = false;
    BbDim7.isOn = false;
    FDim7.isOn  = false;
    CDim7.isOn  = false;
    GDim7.isOn  = false;
    DDim7.isOn  = false;
    ADim7.isOn  = false;
    EDim7.isOn  = false;
    BDim7.isOn  = false;
    FSDim7.isOn = false;
}

void OM108Sux::processMidiEvents(VstMidiEventVec* inputs, VstMidiEventVec* outputs, VstInt32 sampleFrames)
{
    
    if (fPower >= 0.5f)
    {
        
        // process incoming events
        for (unsigned int i = 0; i < inputs[0].size(); i++)
        {
            //copying event "i" from input (with all its fields)
            VstMidiEvent tomod     = inputs[0][i];
            unsigned char status   = tomod.midiData[0] & 0xf0; // scraping  channel
            const char in_channel  = tomod.midiData[0] & 0x0f; // isolating channel
            const char data1       = tomod.midiData[1] & 0x7f; //pitch
            const char data2       = tomod.midiData[2] & 0x7f; //velocity
            if ((status == MIDI_NOTEON) && (data2 == 0))
            {
                dbg("MIDI_NOTEON data2 = 0");
				dbg((int)data1);
                status = MIDI_NOTEOFF;
            }
            if (status == MIDI_NOTEON)
            {
                //dbg("MIDI_NOTEON");
                //tomod.midiData[0]                = out_channel | MIDI_NOTEON;
                tomod.midiData[0] = MIDI_NOTEON;
                tomod.midiData[1]                = data1 & 0x7f;
                //outputs[0].push_back(tomod);
                //check each chord if it should play the 5th
                checkNoteOn(DbDim7, outputs, data1, tomod);
                checkNoteOn(AbDim7, outputs, data1, tomod);
                checkNoteOn(EbDim7, outputs, data1, tomod);
                checkNoteOn(BbDim7, outputs, data1, tomod);
                checkNoteOn(FDim7, outputs, data1, tomod);
                checkNoteOn(CDim7, outputs, data1, tomod);
                checkNoteOn(GDim7, outputs, data1, tomod);
                checkNoteOn(DDim7, outputs, data1, tomod);
                checkNoteOn(ADim7, outputs, data1, tomod);
                checkNoteOn(EDim7, outputs, data1, tomod);
                checkNoteOn(BDim7, outputs, data1, tomod);
                checkNoteOn(FSDim7, outputs, data1, tomod);
                //check if the right chord was pressed and activate as needed
            }
            else if (status == MIDI_NOTEOFF)
            {
                //dbg("MIDI_NOTEOFF/STOP");
                //tomod.midiData[0] = out_channel | MIDI_NOTEOFF;
                tomod.midiData[0] = MIDI_NOTEOFF;
                tomod.midiData[1] = data1 & 0x7f;
                //outputs[0].push_back(tomod);

                //check if any of the current playing chords are not playing anymore
                
                checkNoteOff(DbDim7, outputs, data1, tomod);
                checkNoteOff(AbDim7, outputs, data1, tomod);
                checkNoteOff(EbDim7, outputs, data1, tomod);
                checkNoteOff(BbDim7, outputs, data1, tomod);
                checkNoteOff(FDim7, outputs, data1, tomod);
                checkNoteOff(CDim7, outputs, data1, tomod);
                checkNoteOff(GDim7, outputs, data1, tomod);
                checkNoteOff(DDim7, outputs, data1, tomod);
                checkNoteOff(ADim7, outputs, data1, tomod);
                checkNoteOff(EDim7, outputs, data1, tomod);
                checkNoteOff(BDim7, outputs, data1, tomod);
                checkNoteOff(FSDim7, outputs, data1, tomod);
            }
            else if (status == MIDI_SYSEX)
            {
                dbg("SYSEX");
                dbg((int) data1);

                //tomod.midiData[0] = out_channel | status;
                tomod.midiData[0] = status;
                //outputs[0].push_back(tomod);
                if (data1 == 0)
                {
                    dbg("SYSEX Stop Playing");
                    //flag all extras as off
                    checkNoteOff(DbDim7, outputs, DbDim7.missingNote, tomod);
                    checkNoteOff(AbDim7, outputs, AbDim7.missingNote, tomod);
                    checkNoteOff(EbDim7, outputs, EbDim7.missingNote, tomod);
                    checkNoteOff(BbDim7, outputs, BbDim7.missingNote, tomod);
                    checkNoteOff(FDim7, outputs, FDim7.missingNote, tomod);
                    checkNoteOff(CDim7, outputs, CDim7.missingNote, tomod);
                    checkNoteOff(GDim7, outputs, GDim7.missingNote, tomod);
                    checkNoteOff(DDim7, outputs, DDim7.missingNote, tomod);
                    checkNoteOff(ADim7, outputs, ADim7.missingNote, tomod);
                    checkNoteOff(EDim7, outputs, EDim7.missingNote, tomod);
                    checkNoteOff(BDim7, outputs, BDim7.missingNote, tomod);
                    checkNoteOff(FSDim7, outputs, FSDim7.missingNote, tomod);
					resetNoteMap();
                }
                //tomod.midiData[0] = out_channel | status;
                tomod.midiData[0] = status;
                outputs[0].push_back(tomod);
            }
            else
            {
                dbg("Other");
                dbg((int)status);
                //pass through
                tomod.midiData[0] = status;
                //tomod.midiData[0] = out_channel | status;
                outputs[0].push_back(tomod);
            }
        }
    }
}
