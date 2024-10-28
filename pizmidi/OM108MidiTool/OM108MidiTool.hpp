/*-----------------------------------------------------------------------------
OM108Sux
by Reuben Vinal
-----------------------------------------------------------------------------*/
#ifndef __OM108TOOL_H
#define __OM108TOOL_H

#include "../common/PizMidi.h"
#include <map>
#include <vector>
#include <math.h>
#include <memory>

#define TYPE_OM108  0
#define TYPE_QCHORD 1
#define NUMSEMITONES 12


//notes are based on sharps
#define NOTE_C1  24 //
#define NOTE_CS1 25 //
#define NOTE_D1  26 //
#define NOTE_DS1 27 //
#define NOTE_E1  28 //
#define NOTE_F1  29 //
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
#define NOTE_FS4 66 //
#define NOTE_G4  67 //
#define NOTE_GS4 68 //
#define NOTE_A4  69 //
#define NOTE_AS4 70 //
#define NOTE_B4  71 //

#define NOTE_ROOT_OFFSET NOTE_C1
#define NOTE_HIGHEST 255

#define NOTE_C  0
#define NOTE_DB 1
#define NOTE_D  2
#define NOTE_E  3
#define NOTE_EB 4
#define NOTE_F  5
#define NOTE_GB 6
#define NOTE_G 7
#define NOTE_AB 8
#define NOTE_A 9
#define NOTE_BB 10
#define NOTE_B 11

#define NOTE_FS NOTE_GB


enum
{
    kPower,
    kDevice,
    kNumParams,
    kNumPrograms = 1//?
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

class IndividualSetting
{
public:
    IndividualSetting(std::string ID, float myMin, float myMax)
    {
        myID = ID;
        myValMin = myMin;
        myValMax = myMax;
        fMVal    = 0;
        fmVal    = 0;
        fM7Val   = 0;
        fm7Val   = 0;
        fAugVal  = 0;
        fDimVal  = 0;
        fSus4Val = 0;
        fAdd9Val = 0;
    }

 void setMajor(float value)
    {
        fMVal = value;
    }

    void setMinor(float value)
    {
        fmVal = value;
    }

    void setM7(float value)
    {
        fM7Val = value;
    }

    void setm7(float value)
    {
        fm7Val = value;
    }

    void setAug(float value)
    {
        fAugVal = value;
    }

    void setDim(float value)
    {
        fDimVal = value;
    }

    void setSus4(float value)
    {
        fSus4Val = value;
    }

    void setAdd9(float value)
    {
        fAdd9Val = value;
    }

     float getMajor() const
    {
        return fMVal;
    }

    float getMinor() const
    {
        return fmVal;
    }

    float getM7() const
    {
        return fM7Val;
    }

    float getm7() const
    {
        return fm7Val;
    }

    float getAug() const
    {
        return fAugVal;
    }

    float getDim() const
    {
        return fDimVal;
    }

    float getSus4() const
    {
        return fSus4Val;
    }

    float getAdd9() const
    {
        return fAdd9Val;
    }

private:
    std::string myID;
    float myValMin; //actual range for setting for this value
    float myValMax;
    float fMVal; //Major
    float fmVal; //Minor
    float fM7Val; //M7 no 5
    float fm7Val; //m7 no 5
    float fAugVal; //Aug
    float fDimVal; //Dim no 5
    float fSus4Val; //Sus4
    float fAdd9Val; //Add9
};

class instrumentChord
{
public:
    chord inputChord;
    chord outputChord;
    int bassNote;

    instrumentChord()
    {
        //not used
    }
    instrumentChord(chord input, int bNote)
    {
        inputChord = input;
        outputChord = input;
        bassNote    = bNote;
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
        base        = -1;
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

class NoteType
{
public:

    //get note type value 24 = 0 == C
    static int getNote(int note)
    {
        return (note - NOTE_ROOT_OFFSET) % NUMSEMITONES;
    }

    //From note, get note type (e.g C) and its pitch (e.g. 4 = C4)
    static void getNoteInfo(int note, int& noteType, int& notePitch)
    {
        noteType  = getNote(note);
        notePitch = static_cast<int>((ceil((note - NOTE_ROOT_OFFSET) / (NUMSEMITONES * 1.0))));
    }

    //Get pitch of note eg C4 int value = 4
    static int getNotePitch(int note)
    {
        int myNoteBase;
        int myNoteNum;
        getNoteInfo(note, myNoteBase, myNoteNum);
        return myNoteNum;
    }

    //get effective octave higher value e.g C3 - C4
    static int getNextNotePitch(int note)
    {
        return note + NOTE_ROOT_OFFSET;
    }

    //get note position in string form
    static std::string getNoteStr(int note)
    {
        int myNoteBase;
        int myNoteNum;
        getNoteInfo(note, myNoteBase, myNoteNum);
        std::string noteStr;
        switch (myNoteBase)
        {
            case NOTE_C:
                noteStr = "C";
                break;
            case NOTE_DB:
                noteStr = "Db";
                break;
            case NOTE_D:
                noteStr = "D";
                break;
            case NOTE_E:
                noteStr = "E";
                break;
            case NOTE_EB:
                noteStr = "Eb";
                break;
            case NOTE_F:
                noteStr = "F";
                break;
            case NOTE_GB:
                noteStr = "Gb";
                break;
            case NOTE_G:
                noteStr = "G";
                break;
            case NOTE_AB:
                noteStr = "Ab";
                break;
            case NOTE_A:
                noteStr = "A";
                break;
            case NOTE_BB:
                noteStr = "Bb";
                break;
            case NOTE_B:
                noteStr = "B";
                break;
            default:
                noteStr = "Unknown Note";
                break;
        }
        noteStr = noteStr + std::to_string(myNoteNum);
        return noteStr;
    }
};

//contains chord type
class ChordType
{
public:

    ChordType()
    {
    };

    ChordType(std::vector<int> nnote,
              std::vector<int> nnoteInterval,
              int nfirstNote,
              int nfirstNoteInterval,
              int nOffset,
              int myID)
    {
        //set chord notes
        id                = myID;
        note              = nnote;
        noteInterval = nnoteInterval;
        firstNote         = nfirstNote + nOffset;
        isOn              = false;
        for (int i = 0; i < note.size(); i++)
        {
            note[i] += nOffset;
        }
        firstNoteInterval = nfirstNoteInterval;
        strumMap.clear();
        //first note and vector are using actual mapped value
        //A. Create Strum plate order
        strumMap.push_back(firstNote);
        int cur = firstNote;
        int curPtr = 0;
        //search for first note (assumption is in increasing pitch order)
        for (int i = 0; i < note.size(); i++)
        {
            
            if (NoteType::getNote(note[i]) == NoteType::getNote(firstNote))
            {
                break;
            }
            curPtr++;
        }
        int pitchOffset = NoteType::getNotePitch(firstNote)-1;
        if (curPtr == note.size() - 1)
        {
            curPtr = 0;
        }
        else if (curPtr >= note.size())
        {
            //error!
            cur = NOTE_HIGHEST+1;
        }
        else
        {
        }
        //need to support case where start note -> end note is not necessarily same pitch
        while (cur <= NOTE_HIGHEST)
        {
            int curNote = NoteType::getNote(cur);
            int newNote = NoteType::getNote(note[curPtr]);
            //e.g. 10->1 
            if (newNote < curNote)
            {
                pitchOffset++; //pitch changed
            }
            //get effective new note with correct pitch
            cur = NOTE_ROOT_OFFSET + NoteType::getNote(note[curPtr]) + pitchOffset * 12;
            strumMap.push_back(cur);
            curPtr++;
            if (curPtr >= note.size())
            {
                pitchOffset++;
                curPtr = 0;
            }
        }
    }

    bool isMatchingChord(std::vector<int> rNotes)
    {
        if (rNotes.size() != note.size())
        {
            return false;
        }

        for (int i = 0; i < rNotes.size(); i++)
        {
            //if (NoteType::getNote(rNotes[i]) != NoteType::getNote(note[i]))
            if (rNotes[i] != note[i]) //assume input and this class will have same pitch values
            {
                return false;
            }
        }
        return true;
    }

    bool getIsOn() const
    {
        return isOn;
    }

    void setIsOn(bool val)
    {
        isOn = val;
    }
    // Overload the < operator for map use
    bool operator<(const ChordType& other) const
    {
        return this->id < other.id; // Comparison logic
    }

    std::vector<int> getChord()
    {
        return note;
    }

    std::vector<int> getChordIntervals()
    {
        return noteInterval;
    }

    //gets bass note which is same as ID where 0 = C, 1 = C#/Db
    int getBassNote() const
    {
        return id;
    }

    //assumes bass note has been checked, for ostinato for cases like diminished 5ths
    bool hasSameIntervalButDifferentNote(int curNote, int curInterval, int &replacement)
    {
        for (int i = 0; i < noteInterval.size(); i++)
        {
            if (noteInterval[i] == curInterval)
            {
                if (NoteType::getNote(note[i]) != NoteType::getNote(curNote))
                {
                    replacement = note[i];
                    return true;
                }
            }
        }
        return false;
    }

    

private:
    int id; //used for bass note
    std::vector<int> note;
    std::vector<int> noteInterval; //used for ostinato, we list the actual interval positions like 1, 3, 5 for major and minor
    int firstNote; //for use with strum plate map generation
    int firstNoteInterval; //for use with strum plate map generation, used to search for first note to start from
    std::vector<int> strumMap; //used for strum replacement
    bool isOn;
};

//Tone class contains the class for each note with chords
class ToneType
{
public:
    ToneType()
    {

    }

    ToneType(int myID, int noteOffset, int dType)
    {
        deviceType = dType;
        ID = myID;
        //start generating chords here
        //use dType later to differentiate chords
        //assumes each chord follows the same pattern
        int curCnt = 0;
        tMajor     = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 3, 5 }, 80, 1, noteOffset, curCnt++);
        tMinor     = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 3, 5 }, 80, 1, noteOffset, curCnt++);
        tM7no5     = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 3, 7 }, 80, 1, noteOffset, curCnt++);
        tm7no5     = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 3, 7 }, 80, 1, noteOffset, curCnt++);
        tAug       = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 3, 5 }, 80, 1, noteOffset, curCnt++);
        tDim       = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 3, 5 }, 80, 1, noteOffset, curCnt++);
        tSus4      = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 4, 5 }, 80, 1, noteOffset, curCnt++);
        tAdd9      = ChordType(std::vector<int>{ 1, 2, 3, 4 }, std::vector<int>{ 1, 3, 5, 9 }, 80, 1, noteOffset, curCnt++);
        tM7        = ChordType(std::vector<int>{ 1, 2, 3, 4 }, std::vector<int>{ 1, 3, 5, 7 }, 80, 1, noteOffset, curCnt++);
        tm7        = ChordType(std::vector<int>{ 1, 2, 3, 4 }, std::vector<int>{ 1, 3, 5, 7 }, 80, 1, noteOffset, curCnt++);
        tSus2      = ChordType(std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 1, 2, 5 }, 80, 1, noteOffset, curCnt++);
        tM6        = ChordType(std::vector<int>{ 1, 2, 3, 4 }, std::vector<int>{ 1, 3, 5, 6 }, 80, 1, noteOffset, curCnt++);
        tm6        = ChordType(std::vector<int>{ 1, 2, 3, 4 }, std::vector<int>{ 1, 3, 5, 6 }, 80, 1, noteOffset, curCnt++);
        tPower     = ChordType(std::vector<int>{ 1, 2 }, std::vector<int>{ 1, 5 }, 80, 1, noteOffset, curCnt++);
        
        //assign map per device
        //assumes no changes
        if (deviceType == TYPE_OM108)
        {
            //assign default here
            om108ChordAssignment[tMajor] = std::make_shared<ChordType>(tMajor);
            om108ChordAssignment[tMinor] = std::make_shared<ChordType>(tMinor);
            om108ChordAssignment[tM7no5] = std::make_shared<ChordType>(tM7no5);
            om108ChordAssignment[tm7no5] = std::make_shared<ChordType>(tm7no5);
            om108ChordAssignment[tAug]   = std::make_shared<ChordType>(tAug);
            om108ChordAssignment[tDim]   = std::make_shared<ChordType>(tDim);
            om108ChordAssignment[tSus4]  = std::make_shared<ChordType>(tSus4);
            om108ChordAssignment[tAdd9]  = std::make_shared<ChordType>(tAdd9);

        }
        else if (deviceType == TYPE_QCHORD)
        {
            //assign Qchord chord assignments here
        }

    }


    bool getMatch(std::vector<int> notes, std::shared_ptr<ChordType> result)
    {
        
        //check notes that exist on the devices
        //apply deviceType later 
        bool found = false;
        if (tMajor.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tMajor];
            found  = true;
        }
        
        if (tMinor.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tMinor];
            found  = true;
        }
        if (tM7no5.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tM7no5];
            found  = true;
        }
        if (tm7no5.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tm7no5];
            found  = true;
        }
        if (tAug.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tMajor];
            found  = true;
        }
        if (tDim.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tDim];
            found  = true;
        }
        if (tSus4.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tSus4];
            found  = true;
        }
        if (tAdd9.isMatchingChord(notes))
        {
            result = om108ChordAssignment[tAdd9];
            found  = true;
        }
        
    }

private:
    int ID;
    int noteOffset;
    int deviceType;
    //Put chords here
    ChordType tMajor;
    ChordType tMinor;
    ChordType tM7no5;
    ChordType tm7no5;
    ChordType tAug;
    ChordType tDim;
    ChordType tSus4;
    ChordType tAdd9;
    ChordType tM7;
    ChordType tm7;
    ChordType tSus2;
    ChordType tM6;
    ChordType tm6;
    ChordType tPower;
    //std::make_shared<std::string> test;
    std::map<ChordType, std::shared_ptr<ChordType>> om108ChordAssignment;
    std::map<ChordType, std::shared_ptr<ChordType>> qChordAssignment;
};

class InstrumentType
{
public:
    // Constructor with member initializer list
    InstrumentType(int myID, int bNote)
        : ID(myID), bassNote(bNote)
    {
                  
    };

protected: 
    int ID;
    int bassNote;
    
    ToneType C;
    ToneType D;
    ToneType E;
    ToneType F;
    ToneType G;
    ToneType A;
    ToneType B;
    ToneType Db;
    ToneType Ab;
    ToneType Eb;
    ToneType Bb;
    ToneType Fs;
};


class OM108: public InstrumentType
{
public:

    OM108(int myID, int bNote)
        : InstrumentType(myID, bNote)
    {
        //Initialize Chords here
        //initialize strumplate maps here
        //initialize replacements
    }

};

class OM108SuxProgram
{
    friend class OM108Sux;

public:
    OM108SuxProgram();
    ~OM108SuxProgram() {}

private:
    float fPower;
    float fDevice;
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
    float fDevice; //0-0.5 Omnichord 108, 0.51-1.0 - QChord
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
