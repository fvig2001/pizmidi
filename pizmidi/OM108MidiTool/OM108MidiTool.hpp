/*-----------------------------------------------------------------------------
OM108Sux
by Reuben Vinal
-----------------------------------------------------------------------------*/
#ifndef __OM108TOOL_H
#define __OM108TOOL_H

#include "../common/PizMidi.h"
#include <map>
#include <vector>
enum
{
    kPower,
    kNumParams,
    kNumPrograms = 1
};

class chord
{
public:
    int note1;
    int note2;
    int note3;
    int note4;
    chord(int n1, int n2, int n3, int n4);

    chord()
    {
        note1 = -1;
        note2 = -1;
        note3 = -1;
        note4 = -1;
    }

};

class instrumentChord
{
public:
    chord inputChord;
    chord outputChord;
    int bassNote;

    instrumentChord()
    {
    }
    instrumentChord(chord input)
    {
        inputChord = input;
        outputChord = input;
    };
    
};

class incDim7
{
public:
    int notes[3];
    int missingNote;
    bool isOn;
    int myID;
    int base;
    static int id;
    
    incDim7()
    {
        myID        = -1;
        base     = -1;
        notes[0]    = -1;
        notes[1]    = -1;
        notes[2]    = -1;
        isOn        = false;
        missingNote = -1;
    }
    incDim7(int baseNote, int n1, int n2, int n3, int missing)
    {
        myID        = id++;
        base        = baseNote;
        notes[0]    = n1;
        notes[1]    = n2;
        notes[2]    = n3;
        isOn        = false;
        missingNote = missing;
        dbg("Init");
        dbg(myID);
    };

};

class OM108SuxProgram
{
    friend class OM108Sux;

public:
    OM108SuxProgram();
    ~OM108SuxProgram() {}

private:
    float fPower;
    char name[kVstMaxProgNameLen + 1];
};

class OM108Sux : public PizMidi
{
public:
    OM108Sux (audioMasterCallback audioMaster);
    ~OM108Sux();

    virtual void setProgram (VstInt32 program);
    virtual void setProgramName (char* name);
    virtual void getProgramName (char* name);
    virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);

    virtual void setParameter (VstInt32 index, float value);
    virtual float getParameter (VstInt32 index);
    virtual void getParameterDisplay (VstInt32 index, char* text);
    virtual void getParameterName (VstInt32 index, char* text);

protected:
    float fPower;
    void checkNoteOn(incDim7& chord, VstMidiEventVec* outputs, int note, VstMidiEvent tomod);
    void checkNoteOff(incDim7& chord, VstMidiEventVec* outputs, int note, VstMidiEvent tomod);
    void resetNoteMap();
    virtual void processMidiEvents (VstMidiEventVec* inputs, VstMidiEventVec* outputs, VstInt32 sampleFrames);
    OM108SuxProgram* programs;
    //chord info
    incDim7 DbDim7;
    incDim7 AbDim7;
    incDim7 EbDim7;
    incDim7 BbDim7;
    incDim7 FDim7;
    incDim7 CDim7;
    incDim7 GDim7;
    incDim7 DDim7;
    incDim7 ADim7;
    incDim7 EDim7;
    incDim7 BDim7;
    incDim7 FSDim7;
    instrumentChord iChords[16];      //instrument chords
    std::map<int, bool> playingNotes; //note played map
};

#endif
