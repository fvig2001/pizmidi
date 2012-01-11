#ifndef MidiChordsPLUGINFILTER_H
#define MidiChordsPLUGINFILTER_H

#include "../../common/PizAudioProcessor.h"
#include "../../common/midistuff.h"
#include "../../common/ChordFunctions.h"
#include "../../common/key.h"

enum parameters {
    kChannel, 
	kLearnChord,
	kFollowInput,
	kTranspose,
	kMode,
	kRoot,
	kGuess,
	kFlats,
	kUseProgCh,
	kLearnChannel,
	kVelocity,

    numParams,
	kVariation,
	numProgs = 128
};

enum ChordModes {Normal, Octave, Global, numModes};

struct ChordNote
{
	ChordNote(int channel, int note)
	{
		c = channel;
		n = note;
	}
	ChordNote()
	{
		c=n=-1;
	}
	~ChordNote(){}

	int c;
	int n;
};

class MidiChordsPrograms : ValueTree {
public:
	friend class MidiChords;
	MidiChordsPrograms ();
	~MidiChordsPrograms () {};

	void set(int prog, const var::identifier &name, const var &newValue)
	{
		this->getChild(prog).setProperty(name,newValue,0);
	}

	void clearNoteMatrix(int prog, int trigger)
	{
		if(getChild(prog).getChild(trigger).isValid())
			getChild(prog).getChild(trigger).removeAllProperties(0);
	}

	void setChordNote(int prog, int trigger, int channel, int note, const bool &newValue)
	{
		channel-=1;
		if (!getChild(prog).getChild(trigger).isValid())
		{
			ValueTree noteMatrix("NoteMatrix_T"+String(trigger));
			getChild(prog).addChild(noteMatrix,trigger,0);
		}
		if (note<32) {
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(channel)+"_0-31",0);
			if (newValue) 
				state |= 1 << note;
			else 
				state &= ~(1<<note);
			getChild(prog).getChild(trigger).setProperty("Ch"+String(channel)+"_0-31",state,0);
		}
		else if (note<64) {
			note -= 32;
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(channel)+"_32-63",0);
			if (newValue) state |= 1 << note;
			else state &= ~(1<<note);
			getChild(prog).getChild(trigger).setProperty("Ch"+String(channel)+"_32-63",state,0);
		}
		else if (note<96) {
			note -= 64;
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(channel)+"_64-95",0);
			if (newValue) state |= 1 << note;
			else state &= ~(1<<note);
			getChild(prog).getChild(trigger).setProperty("Ch"+String(channel)+"_64-95",state,0);
		}
		else if (note<128) {
			note -= 96;
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(channel)+"_96-127",0);
			if (newValue) state |= 1 << note;
			else state &= ~(1<<note);
			getChild(prog).getChild(trigger).setProperty("Ch"+String(channel)+"_96-127",state,0);
		}
	}

	const var get(int prog, const var::identifier &name)
	{
		return this->getChild(prog).getProperty(name);
	}

	bool getChordNote(int prog, int trigger, int ch, int note)
	{
		ch-=1;
		if (!getChild(prog).getChild(trigger).isValid())
			return false;
		if (note<32) {
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(ch)+"_0-31",0);
			return (state & (1 << note)) != 0;
		}
		else if (note<64) {
			note -= 32;
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(ch)+"_32-63",0);
			return (state & (1 << note)) != 0;
		}
		else if (note<96) {
			note -= 64;
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(ch)+"_64-95",0);
			return (state & (1 << note)) != 0;
		}
		else if (note<128) {
			note -= 96;
			int state = getChild(prog).getChild(trigger).getProperty("Ch"+String(ch)+"_96-127",0);
			return (state & (1 << note)) != 0;
		}
		return false;
	}

	bool getChordNote_OLD(int prog, int trigger, int ch, int note)
	{
		ch-=1;
		if (!getChild(prog).getChild(ch).isValid())
			return false;
		if (note<32) {
			int state = getChild(prog).getChild(ch).getProperty("T"+String(trigger)+"_0-31",0);
			return (state & (1 << note)) != 0;
		}
		else if (note<64) {
			note -= 32;
			int state = getChild(prog).getChild(ch).getProperty("T"+String(trigger)+"_32-63",0);
			return (state & (1 << note)) != 0;
		}
		else if (note<96) {
			note -= 64;
			int state = getChild(prog).getChild(ch).getProperty("T"+String(trigger)+"_64-95",0);
			return (state & (1 << note)) != 0;
		}
		else if (note<128) {
			note -= 96;
			int state = getChild(prog).getChild(ch).getProperty("T"+String(trigger)+"_96-127",0);
			return (state & (1 << note)) != 0;
		}
		return false;
	}

	MidiChordsPrograms(ValueTree& tree)
	{
		this->getChild(0).getParent() = tree.createCopy();
	}
};

//==============================================================================
class MidiChords  : public PizAudioProcessor,
                    public ChangeBroadcaster
{
public:
    //==============================================================================
    MidiChords();
    ~MidiChords();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

	void processBlock (AudioSampleBuffer& buffer,
                       MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();

    //==============================================================================
    const String getName() const;
	bool hasEditor() const {return true;}

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

	bool acceptsMidi() const {return true;}
    bool producesMidi() const {return true;}

    //==============================================================================
    int getNumPrograms()                                        { return numProgs; }
    int getCurrentProgram()                                     { return curProgram; }
    void setCurrentProgram (int index);
    const String getProgramName (int index)                     { return programs->get(index,"Name"); }
    void changeProgramName (int index, const String& newName)   { programs->set(index,"Name",newName); }

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    void getCurrentProgramStateInformation (MemoryBlock& destData);
    void setCurrentProgramStateInformation (const void* data, int sizeInBytes);

    //==============================================================================
	MidiKeyboardState progKbState[numProgs][128];
	MidiKeyboardState chordKbState;
	MidiKeyboardState triggerKbState;
	void selectTrigger(int index);
	void selectChordNote(int index, int note, bool on, int ch=-1);
	int getCurrentTrigger() {return curTrigger;}
	bool isTriggerNotePlaying(int channel, int note) {return notePlaying[channel][note];}
	bool isTriggerNotePlaying(int note) {
		for (int i=0;i<16;i++)
			if (notePlaying[i][note]) return true;
		return false;
	}
	void clearAllChords();
	void resetAllChords();
	void clearChord(int trigger);
	void resetChord(int trigger);
	void transposeAll(bool up);
	void transposeChord(int trigger, bool up);
	void transposeCurrentChordByOctave(bool up);
	String getCurrentChordName();
	void savePreset(String name);
	void playCurrentChord(bool on) 
	{
		if (playingFromGUI!=on)
		{
			playFromGUI=on;
		}
	}
	bool isPreviewChordPlaying() {return playFromGUI;}

	bool readKeyFile(File file=File::nonexistent);
	bool demo;

	int lastUIWidth, lastUIHeight;
    //==============================================================================
    juce_UseDebuggingNewOperator

private:
	int curProgram;
	int curTrigger;
	//StringArray chordNames;

	MidiChordsPrograms* programs;
	bool init;

    int channel;
	int learnchan;
	bool learn;
	bool follow;
	bool usepc;
	int transpose;
	int previewVel;
	int mode;
	int root;
	bool guess;
	bool flats;

	bool playingFromGUI, playFromGUI;
	int playButtonTrigger;
	int learning;
	bool notePlaying[16][128]; //trigger note
	int outputNote[16][128];
	int numChordNotes[numProgs][128];

	Array<ChordNote> playingChord[128];
	bool chordNotePlaying[16][128];
	Array<int> ignoreNextNoteOff[16];

	void copySettingsToProgram(int index);
};

#endif