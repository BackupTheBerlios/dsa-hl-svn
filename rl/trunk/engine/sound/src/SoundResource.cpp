/* SoundResource.cpp - Diese Klassse kapselt einen Sound.
 * (C) 2004. Team Pantheon. www.team-pantheon.de
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the Perl Artistic License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Perl Artistic License for more details.
 *
 *  You should have received a copy of the Perl Artistic License
 *  along with this program; if not you can get it here
 *  http://www.perldoc.com/perl5.6/Artistic.html.
 */

#include <string>
#include <iostream>
#include <math.h>
#include <OgreString.h>
#include <OgreStringConverter.h>
#include "SoundSubsystem.h"
#include "SoundManager.h"
#include "SoundResource.h"
#include "SoundPlayEvent.h"
#include "SoundFadeEvent.h"

extern "C" {
    #include <AL/al.h>
}

/// Die Callback-Struktur, die Vorbisfile braucht
ov_callbacks rl::SoundResource::mVorbisCallbacks = {
    &rl::SoundResource::VorbisRead,
    &rl::SoundResource::VorbisSeek,
    &rl::SoundResource::VorbisClose,
    &rl::SoundResource::VorbisTell
};

using namespace std;
using namespace Ogre;
using namespace boost;

namespace rl {
    
    
/**
 * @author JoSch
 * @date 02-02-2005
 */
SoundResource::FadeIn::FadeIn()
    : that(0)
{
}
    
/**
 * @author JoSch
 * @date 02-03-2005
 */
void SoundResource::FadeIn::setThat(SoundResource *that)
{
    this->that = that;
}

/**
 * @author JoSch
 * @date 02-02-2005
 */
void SoundResource::FadeIn::operator()()
{
    that->fadeIn();
} 
    
/**
 * @author JoSch
 * @date 02-02-2005
 */
SoundResource::FadeOut::FadeOut()
    : that(0)
{
}
    
/**
 * @author JoSch
 * @date 02-03-2005
 */
void SoundResource::FadeOut::setThat(SoundResource *that)
{
    this->that = that;
}


/**
 * @author JoSch
 * @date 02-02-2005
 */
void SoundResource::FadeOut::operator()()
{
    that->fadeOut();
}
 
/**
 * @author JoSch
 * @date 02-02-2005
 */
SoundResource::Streaming::Streaming()
    : that(0)
{
}
    
/**
 * @author JoSch
 * @date 02-03-2005
 */
void SoundResource::Streaming::setThat(SoundResource *that)
{
    this->that = that;
}

/**
 * @author JoSch
 * @date 02-02-2005
 */
void SoundResource::Streaming::operator()()
{
    that->runStreaming();
} 

/**
 * @author JoSch
 * @date 07-23-2004
 */
SoundResource::SoundResource(ResourceManager* creator, const String& name, ResourceHandle handle,
        const String& group, bool isManual, ManualResourceLoader* loader):
    EventListener<SoundEvent>(),
    EventSource(), 
    EventCaster<SoundEvent>(),
    Resource(creator, name, handle, group, isManual, loader),
    mBuffers(mDefaultBufferCount),
    mSource(0),
    mFadeInThread(0),
    mFadeOutThread(0),
    mStreamThread(0),
    mFadeInFunctor(),
    mFadeOutFunctor(),
    mStreamFunctor()
{
    mFadeInFunctor.setThat(this);
    mFadeOutFunctor.setThat(this);
    mStreamFunctor.setThat(this);
    
    mName = name;
	mGroup = group;
    alGenSources(1, &mSource);
    check();
    alGenBuffers(mBuffers.size(), &mBuffers[0]);
    check();

    // ein paar Verknuepfungen
    addEventListener(this);

    /// Ein paar Standardwerte setzen
    setGain(1.0f);
    setPosition(Vector3(0.0, 0.0, 0.0));
    setVelocity(Vector3(0.0, 0.0, 0.0));
    setDirection(Vector3(0.0, 0.0, 0.0));
    SoundSubsystem::log("SoundResource Name: " + mName);
}

/**
 * @author JoSch
 * @date 07-24-2004
 */
SoundResource::~SoundResource()
{
    // Listener entfernen.
    removeEventListener(this);
    
    // Threads anhalten
    delete mStreamThread;
    delete mFadeInThread;
    delete mFadeOutThread;
    
    // Resourcen aufr�umen.
    alDeleteBuffers(mBuffers.size(), &mBuffers[0]);
    alDeleteSources(1, &mSource);
    mSource = 0;
}

/**
 * @return Wenn false, wird das Ereignis nicht mehr weitergereicht.
 * @author JoSch
 * @date 10-14-2004
 */
bool SoundResource::eventRaised(SoundEvent *anEvent) const
{
    SoundSubsystem::log("Event raised. Reason: " + StringConverter::toString(anEvent->getReason()));
    SoundSubsystem::log("Event raised. Source: " + StringConverter::toString((unsigned long int)anEvent->getSource()));
    return true;
}

/**
 * @return Die aktuelle Richtung der Soundquelle
 * @author JoSch
 * @date 07-23-2004
 */
const Vector3 SoundResource::getDirection() const throw (RuntimeException)
{
    Vector3 result;
    alGetSource3f(mSource, AL_DIRECTION,
        &result.x, &result.y, &result.z);
    check();
    return result;
}

/**
 * @param direction Die neue Richtung der Soundquelle.
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::setDirection (const Vector3& direction) throw (RuntimeException)
{
    alSource3f(mSource, AL_DIRECTION,
        direction[0], direction[1], direction[2]);
    check();
}

/**
 * @return Die aktuelle Position der Soundquelle
 * @author JoSch
 * @date 07-23-2004
 */
const Vector3 SoundResource::getPosition() const throw (RuntimeException)
{
    Vector3 result;
    alGetSource3f(mSource, AL_POSITION,
        &result.x, &result.y, &result.z);
    check();
    return result;
}

/**
 * @param position Die neue Position der Soundquelle.
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::setPosition(const Vector3& position) throw (RuntimeException)
{
    alSource3f(mSource, AL_POSITION,
        position[0], position[1], position[2]);
    check();
}

/**
 * @return Die aktuelle Geschwindigkeit der Soundquelle
 * @author JoSch
 * @date 07-23-2004
 */
const Vector3 SoundResource::getVelocity() const throw (RuntimeException)
{
    Vector3 result;
    alGetSource3f(mSource, AL_VELOCITY,
        &result.x, &result.y, &result.z);
    check();
    return result;
}

/**
 * @param velocity Die neue Geschwindigkeit der Soundquelle.
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::setVelocity(const Vector3& velocity) throw (RuntimeException)
{
    alSource3f(mSource, AL_VELOCITY,
        velocity[0], velocity[1], velocity[2]);
    check();
}

/**
 * @return Die aktuelle Lautstaerke der Soundquelle
 * @author JoSch
 * @date 07-23-2004
 */
const ALfloat SoundResource::getGain() const throw (RuntimeException)
{
    ALfloat result;
    {
        boost::mutex::scoped_lock lock(mGainMutex);
        alGetSourcef(mSource, AL_GAIN, &result);
    }
    check();
    return result;
}

/**
 * @param gain Die neue Lautstarke der Soundquelle.
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::setGain(const ALfloat gain) throw (RuntimeException)
{
    {
        boost::mutex::scoped_lock lock(mGainMutex);
        alSourcef(mSource, AL_GAIN, gain);
    }
    check();
}

/** Laedt die Soundquelle und setzt die Werte.
 * @author JoSch
 * @date 10-11-2004
 */
void SoundResource::loadImpl()
{
    if (!mIsLoaded)
    {
        // Holen wir erstmal die Daten.
		mDataStream = ResourceGroupManager::getSingleton().openResource(mName, mGroup);
		size_t numBytes = mDataStream.getPointer()->size();

        if (StringUtil::endsWith(mName, ".ogg"))
        {
            mSoundDataType = OggVorbis;
        } 
		else if (StringUtil::endsWith(mName, ".wav"))
        {
            mSoundDataType = Wave;
        } 
		else 
		{
            mIsLoaded = false;
            return;
        }

        // Jetzt mal die Soundparameter ermitteln.
        switch(mSoundDataType)
        {
            vorbis_info *vorbisInfo;
            case OggVorbis:
				mOggMemoryFile.mDataStream = mDataStream;
				mOggMemoryFile.mDataSize = numBytes;
                mOggMemoryFile.mDataRead = 0;
                if (ov_open_callbacks(&mOggMemoryFile, &mOggStream, NULL, 0,
                         SoundResource::mVorbisCallbacks) != 0)
                    throw string ("Could not read Ogg file from memory");

                vorbisInfo = ov_info(&mOggStream, -1);
                if (vorbisInfo->channels == 1)
                    mFormat = AL_FORMAT_MONO16;
                else
                    mFormat = AL_FORMAT_STEREO16;
                mFrequency = vorbisInfo->rate;  
                mSize = numBytes;
                mLoop = false;
                mTime = ov_time_total(&mOggStream, -1);
                //ov_clear(&mOggStream);
                //delete vorbisInfo;
                
                break;
            case Wave:
				{	
				ALbyte* dataPtr = new ALbyte[numBytes];
				mDataStream.getPointer()->read(dataPtr, numBytes);
				alutLoadWAVMemory(dataPtr, &mFormat, &mWAVData,
					&mSize, &mFrequency, &mLoop);
				check();
				delete[] dataPtr;
				}
				break;
            default:
                // Nicht erlaubt;
                mDataStream.setNull();
                mIsLoaded = false;
                return;
        }
    }
 
}

/** Entlaedt die Soundquelle und setzt die Werte.
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::unloadImpl()
{
    if (mIsLoaded)
    {
        stop();
        empty();
        // Fehler zuruecksetzen.
        alGetError();
    }
    // Aufraeumen abh. von den Sounddaten
    switch(mSoundDataType)
    {
        case OggVorbis:
            ov_clear(&mOggStream);
            
            break;
        case Wave:
            alutUnloadWAV(mFormat, mWAVData, mSize, mFrequency);
            check();
            
            break;
    }
    
    
    if (!mDataStream.isNull())
    {
        mDataStream.setNull();
    }
    this->mIsLoaded = false;
}

size_t SoundResource::calculateSize() const
{
	return mSize;
}


/**
 * @author JoSch
 * @date 09-15-2004
 */
void SoundResource::play(unsigned int msec) throw (RuntimeException)
{
    if (!mStreamThread)
    {
        mStreamThread = new thread(SoundResource::mStreamFunctor);
        if (msec != 0)
        {
            setFadeIn(msec);
            mFadeInThread = new thread(SoundResource::mFadeInFunctor);
        }
    }
}

/**
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::pause() throw (RuntimeException)
{
    alSourcePause(mSource);
    check();
}

/**
 * Abspielen stoppen.
 * @author JoSch
 * @date 07-27-2004
 */
void SoundResource::stop(unsigned int msec) throw (RuntimeException)
{
    if (alIsSource(mSource) && playing())
    {
        setFadeOut(msec);
        mFadeOutThread = new thread(SoundResource::mFadeOutFunctor);
    } 
}

/**
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::rewind() throw (RuntimeException)
{
    alSourceRewind(mSource);
    check();
}

/**
 * @return Der momentane Zustand der Soundquelle.
 * @author JoSch
 * @date 07-23-2004
 */
const ALenum SoundResource::getState() const throw (RuntimeException)
{
    ALenum result;
    alGetSourcei(mSource, AL_SOURCE_STATE, &result);    
    check();
    return result;
}

/**
 * @return Ueberprueft, ob Fehler aufgetreten ist, ansonsten Exception.
 * @author JoSch
 * @date 07-23-2004
 */
void SoundResource::check() const throw (RuntimeException)
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
		string errormsg = (char*)alGetString(error);
        SoundSubsystem::log("Error: " + errormsg);
        Throw(RuntimeException, errormsg);
    }
}


/**
 * Fuehrt das Fadein aus
 * @author JoSch
 * @date 09-16-2004
 * @date 02-02-2005
 */
void SoundResource::fadeIn()
{
    boost::xtime xt;

    if (mFadeIn != 0)
    {
        SoundFadeEvent event(this);
        event.setReason(SoundFadeEvent::STARTEVENT);
        dispatchEvent(&event);
    }

    try {
        if (mFadeIn != 0)
        {
            // Alte Lautstaerke speichern.
            ALfloat gain = getGain();
            
            setGain(0.0f);
            for(unsigned int time = 0; (time <= mFadeIn) && playing(); time += 10)
            {
                
                // Warten
                xtime_get(&xt, TIME_UTC);
                xt.nsec += 10 * 1000;
                thread::sleep(xt);
                ALfloat newgain = calculateFadeIn(time, gain);
                // Lautstaerke hochsetzen.
                setGain((newgain > gain)?gain:newgain);
            }
            setGain(gain);
            check();
        }
    } catch(...)
    {}
    if (mFadeIn != 0)
    {
        SoundFadeEvent event(this);
        event.setReason(SoundFadeEvent::STOPEVENT);
        dispatchEvent(&event);
    }
}

/**
 * Fuehrt das Fadeout aus
 * @author JoSch
 * @date 09-16-2004
 * @date 02-02-2005
 */
void SoundResource::fadeOut()
{
    boost::xtime xt;

    if (mFadeOut != 0)
    {
        SoundFadeEvent event(this);
        event.setReason(SoundFadeEvent::STARTEVENT);
        dispatchEvent(&event);
    }

    try {
        if (mFadeOut != 0)
        {
            
            // Alte Lautstaerke speichern.
            ALfloat gain = getGain();
            
            for (unsigned int time = 0; (time <= mFadeOut) && playing(); time += 10)
            {
                // Warten
                xtime_get(&xt, TIME_UTC);
                xt.nsec += 10 * 1000;
                thread::sleep(xt);
                ALfloat newgain = calculateFadeOut(time, gain);
                // Lautstaerke hochsetzen.
                setGain((newgain > gain)?gain:newgain);
            }
            setGain(0.0f);
            check();
            
        }
        // Stoppen.
        alSourceStop(mSource);
        check();
    } catch(...)
    {}   
    if (mFadeOut != 0)
    {
        SoundFadeEvent event(this);
        event.setReason(SoundFadeEvent::STOPEVENT);
        dispatchEvent(&event);
    }
}

/**
 * @param time. Die Zeit in msek., wo der Fade In gerade ist
 * @param gain. Der aktuelle Lautstaerkewert, vom dem ausgegangen wird.
 * @author JoSch
 * @date 09-15-2004
 */
ALfloat SoundResource::calculateFadeIn(unsigned RL_LONGLONG time, ALfloat gain)
{
    try {
        ALfloat x = (time * 1.0f) / getFadeIn();
        return (1.0f - exp(-x)) * gain;
    } catch(...)
    {
        return gain;
    }
}

/**
 * @param time. Die Zeit in msek., wo der Fade Out gerade ist
 * @param gain. Der aktuelle Lautstaerkewert, vom dem ausgegangen wird.
 * @author JoSch
 * @date 09-15-2004
 */
ALfloat SoundResource::calculateFadeOut(unsigned RL_LONGLONG time, ALfloat gain)
{
    try {
        ALfloat x = (time * 1.0f) / getFadeOut();
        return exp(-x) * gain;
    } catch(...)
    {
        return (ALfloat)0.0f;
    }
}

/**
 * Zeit in ns, die wir unterbechen.
 * @author JoSch
 * @date 01-19-2005
 */
int SoundResource::mSleepTime = 1000;

/**
 * Fuehrt das Streamen aus
 * @author JoSch
 * @date 10-11-2004
 */
void SoundResource::runStreaming()
{
    bool loopende = false;
    
    SoundPlayEvent event(this);
    event.setReason(SoundPlayEvent::STARTEVENT);
    dispatchEvent(&event);

    mWavIndex = 0;
    if (!playback())
    {
        // TODO: Fehlermeldung
        return;
    }    
    while(!loopende && update())
    {
        if (!playing())
        {
            if (!playback())
            {
                // TODO Fehlermeldung
                return;
            } else {
                SoundSubsystem::log("Stream was interrupted.");
            }
            loopende = true;
        }
        boost::xtime xt;
        xtime_get(&xt, TIME_UTC);
        xt.nsec += mSleepTime;
        thread::sleep(xt);
    }
    
    event.setReason(SoundPlayEvent::STOPEVENT);
    dispatchEvent(&event);
    
    delete mFadeInThread;
    mFadeInThread = 0;
    delete mFadeOutThread;
    mFadeOutThread = 0;
    alSourceStop(mSource);
    empty();
    thread *temp = mStreamThread;
    mStreamThread = 0;
    delete temp;
    return;
}

/**
 * Wavedaten streamen
 * @return TRUE, wenn Streamen geklappt hat.
 * @param buffer Den Sounndbuffer zum Abspielen.
 * @author JoSch
 * @date 11-01-2004
 */
bool SoundResource::wavstream (ALuint buffer)
{
    char pcm[BUFFER_SIZE];
    ALsizei toCopy = mSize - mWavIndex;
    if (toCopy > BUFFER_SIZE)
    {
        toCopy = BUFFER_SIZE;
    }
    if (toCopy <= 0)
    {
        return false;
    }
    try {
        memcpy(pcm, (void*)((char*)mWAVData + mWavIndex), toCopy);
        alBufferData(buffer, mFormat, pcm , toCopy, mFrequency);
        check();
    } catch(...) {}
    mWavIndex += toCopy;

    return true; 
 
}

/**
 * Sounddaten streamen
 * @return TRUE, wenn das Streamen geklappt hat.
 * @param buffer Den Sounndbuffer zum Abspielen.
 * @author JoSch
 * @date 11-01-2004
 */
bool SoundResource::stream (ALuint buffer)
{
    switch(mSoundDataType)
    {
        case Wave:
            return wavstream(buffer);
            break;
        case OggVorbis:
            return oggstream(buffer);
            break;
        default:
            return false;
    }
}

/**
 * @return Die Dauer des FadeIns.
 * @author JoSch
 * @date 02-02-2005
 */
unsigned int SoundResource::getFadeIn() const
{
    return mFadeIn;
}

/**
 * @return Die Dauer des FadeOuts.
 * @author JoSch
 * @date 02-02-2005
 */
unsigned int SoundResource::getFadeOut() const
{
    return mFadeOut;
}

/**
 * @param Die Dauer des FadeIns.
 * @author JoSch
 * @date 02-02-2005
 */
void SoundResource::setFadeIn(unsigned int fade)
{
    mFadeIn = fade;
}

/**
 * @param Die Dauer des FadeOuts.
 * @author JoSch
 * @date 02-02-2005
 */
void SoundResource::setFadeOut(unsigned int fade)
{
    mFadeOut = fade;
}

SoundResourcePtr::SoundResourcePtr(const ResourcePtr& res) : SharedPtr<SoundResource>()
{
    // lock & copy other mutex pointer
    OGRE_LOCK_MUTEX(*res.OGRE_AUTO_MUTEX_NAME)
        OGRE_COPY_AUTO_SHARED_MUTEX(res.OGRE_AUTO_MUTEX_NAME)
        pRep = static_cast<SoundResource*>(res.getPointer());
    pUseCount = res.useCountPointer();
    if (pUseCount != 0)
        ++(*pUseCount);
}

SoundResourcePtr& SoundResourcePtr::operator =(const ResourcePtr& res)
{
    if (pRep == static_cast<SoundResource*>(res.getPointer()))
        return *this;
    release();

    // lock & copy other mutex pointer
    OGRE_LOCK_MUTEX(*res.OGRE_AUTO_MUTEX_NAME)
        OGRE_COPY_AUTO_SHARED_MUTEX(res.OGRE_AUTO_MUTEX_NAME)
        pRep = static_cast<SoundResource*>(res.getPointer());
    pUseCount = res.useCountPointer();
    if (pUseCount != 0)
        ++(*pUseCount);

    return *this;
}

void SoundResourcePtr::destroy()
{
    SharedPtr<SoundResource>::destroy();
}


/**
 ******************************************************************************
 * Der folgende Code wurde dem Beispielcode von Jesse Maurais und Spree Tree
 * auf DevMaster.net entliehen und den hiesigen Bed�rfnissen angepasst.
 * Der Originalcode kann von hier geladen werden:
 *  http://www.devmaster.net/articles.php?catID=6
 ******************************************************************************/


size_t SoundResource::VorbisRead(void *ptr, size_t byteSize, size_t sizeToRead,
                                 void *datasource)
{
    size_t spaceToEOF;      // How much more we can read till we hit the EOF marker
    size_t actualSizeToRead;        // How much data we are actually going to read from memory
    SOggFile *vorbisData;   // Our vorbis data, for the typecast

    // Get the data in the right format
    vorbisData = (SOggFile *) datasource;

    // Calculate how much we need to read.  This can be sizeToRead*byteSize or less depending on how near the EOF marker we are
    spaceToEOF = vorbisData->mDataSize - vorbisData->mDataRead;
    if ((sizeToRead * byteSize) < spaceToEOF)
        actualSizeToRead = (sizeToRead * byteSize);
    else
        actualSizeToRead = spaceToEOF;

    // A simple copy of the data from memory to the datastruct that the vorbis libs will use
    if (actualSizeToRead) {
        vorbisData->mDataRead += vorbisData->mDataStream->read(ptr, actualSizeToRead);
        // Copy the data from the start of the file PLUS how much we have already read in
        // TODO: memcpy (ptr, (char *) vorbisData->mDataPtr + vorbisData->mDataRead,
        //        actualSizeToRead);
        // Increase by how much we have read by
        //vorbisData->mDataRead += (actualSizeToRead);
    }

    // Return how much we read (in the same way fread would)
    return actualSizeToRead;
}

//---------------------------------------------------------------------------------
// Function : VorbisSeek
// Purpose : Callback for the Vorbis seek function
// Info     : 
//---------------------------------------------------------------------------------
int SoundResource::VorbisSeek (void *datasource
                               /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct */
                               , ogg_int64_t offset /*offset from the point we wish to seek to */
                               , int whence /*where we want to seek to */ )
{
    size_t spaceToEOF;      // How much more we can read till we hit the EOF marker
    ogg_int64_t actualOffset;       // How much we can actually offset it by
    SOggFile *vorbisData;   // The data we passed in (for the typecast)

    // Get the data in the right format
    vorbisData = (SOggFile *) datasource;

    // Goto where we wish to seek to
    switch (whence) {
        case SEEK_SET:     // Seek to the start of the data file
            // Make sure we are not going to the end of the file
            if (vorbisData->mDataSize >= offset)
                actualOffset = offset;
            else
                actualOffset = vorbisData->mDataSize;
            // Set where we now are
            vorbisData->mDataRead = (int) actualOffset;
            break;
        case SEEK_CUR:     // Seek from where we are
            // Make sure we dont go past the end
            spaceToEOF = vorbisData->mDataSize - vorbisData->mDataRead;
            if (offset < spaceToEOF)
                actualOffset = (offset);
            else
                actualOffset = spaceToEOF;
            // Seek from our currrent location
            vorbisData->mDataRead += actualOffset;
            break;
        case SEEK_END:     // Seek from the end of the file
            vorbisData->mDataRead = vorbisData->mDataSize + 1;
            break;
        default:
            printf ("*** ERROR *** Unknown seek command in VorbisSeek\n");
            break;
    };
    vorbisData->mDataStream->seek(vorbisData->mDataRead);

    return 0;
}

//---------------------------------------------------------------------------------
// Function    : VorbisClose
// Purpose    : Callback for the Vorbis close function
// Info        : 
//---------------------------------------------------------------------------------
int SoundResource::VorbisClose (void *datasource
                                /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct */
    )
{
    SOggFile *vorbisData;

    // Get the data in the right format
    vorbisData = (SOggFile *) datasource;

    // This file is called when we call ov_close.  If we wanted, we could free our memory here, but
    // in this case, we will free the memory in the main body of the program, so dont do anything
    vorbisData->mDataStream->seek(0);
    return 1;
}

//---------------------------------------------------------------------------------
// Function    : VorbisTell
// Purpose : Classback for the Vorbis tell function
// Info        : 
//---------------------------------------------------------------------------------
long SoundResource::VorbisTell (void *datasource
                                /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct */
    )
{
    SOggFile *vorbisData;

    // Get the data in the right format
    vorbisData = (SOggFile *) datasource;

    // We just want to tell the vorbis libs how much we have read so far
    return vorbisData->mDataStream->tell();
}
/************************************************************************************************************************
   End of Vorbis callback functions
************************************************************************************************************************/


void SoundResource::display ()
{
/*    cout << "version         " << mVorbisInfo->version << "\n"
        << "channels        " << mVorbisInfo->channels << "\n"
        << "rate (hz)       " << mVorbisInfo->rate << "\n"
        << "bitrate upper   " << mVorbisInfo->bitrate_upper << "\n"
        << "bitrate nominal " << mVorbisInfo->bitrate_nominal << "\n"
        << "bitrate lower   " << mVorbisInfo->bitrate_lower << "\n"
        << "bitrate window  " << mVorbisInfo->bitrate_window << "\n"
        << "\n" << "vendor " << mVorbisComment->vendor << "\n";

    for (int i = 0; i < mVorbisComment->comments; i++)
        cout << "   " << mVorbisComment->user_comments[i] << "\n";

     cout << endl; */
}




bool SoundResource::playback ()
{

    if (playing ())
        return true;
    
    // Fuer jeden Buffer ein Bit setzen: 2^size - 1
    unsigned int streams = (1 << mBuffers.size()) - 1; 
    for (unsigned int i = 0; i < mBuffers.size(); i++)
    {
        if (!stream (mBuffers[i]))
        {
            streams &= ~(1 << i); // Entsprechendes Bit loeschen.
        }
    }
    // Wenn kein Buffer gefuellt wurde (==0), dann h�ren wir jetzt auf.
    if (streams == 0)
    {
        return false;
    }

    for (unsigned int i = 0; i < mBuffers.size(); i++)
    {
        if ((streams & (1 << i)) != 0)
        {
            alSourceQueueBuffers (mSource, 1, &mBuffers[i]);
        }
    }
    alSourcePlay (mSource);

    return true;
}


const bool SoundResource::playing () const
{
    ALenum state;

    alGetSourcei (mSource, AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);
}

bool SoundResource::update ()
{
    int processed;
    bool active = true;

    alGetSourcei (mSource, AL_BUFFERS_PROCESSED, &processed);

    while (processed--) {
        ALuint buffer;

        alSourceUnqueueBuffers (mSource, 1, &buffer);
        check ();

        active = stream (buffer); // TODO

        alSourceQueueBuffers (mSource, 1, &buffer);
        check ();
    }

    return active;
}

bool SoundResource::oggstream (ALuint buffer)
{
    char pcm[BUFFER_SIZE*4];
    int size = 0;
    int section;
    int result;
    
    while (size < BUFFER_SIZE) {
        result =
            ov_read(&mOggStream, pcm + size, BUFFER_SIZE - size, 0, 2, 1,
                     &section);
        if (result > 0) {
            size += result;
        }
        else if (result < 0) {
            throw errorString (result);
        }
        else {
            break;
        } 
       
    }

    if (size < BUFFER_SIZE) {
        return false;
    }

    alBufferData(buffer, mFormat, pcm, size, mFrequency);
    check ();

    return true; 
}

void SoundResource::empty ()
{
    int queued;

    alGetSourcei (mSource, AL_BUFFERS_QUEUED, &queued);

    while (queued--) {
        ALuint buffer;

        alSourceUnqueueBuffers (mSource, 1, &buffer);
        check ();
    }
}

void SoundResource::check ()
{
    int error = alGetError ();

    if (error != AL_NO_ERROR) {
        cerr << alGetString (error) << endl;
        Throw (RuntimeException, (const char *) alGetString (error));
    }
}

String SoundResource::errorString (int code)
{
    switch (code) {
        case OV_EREAD:
            return string ("Read from media.");
        case OV_ENOTVORBIS:
            return string ("Not Vorbis data.");
        case OV_EVERSION:
            return string ("Vorbis version mismatch.");
        case OV_EBADHEADER:
            return string ("Invalid Vorbis header.");
        case OV_EFAULT:
            return
                string
                ("Internal logic fault (bug or heap/stack corruption.");
        default:
            return string ("Unknown Ogg error.");
    }
}


} /* End of Namespace */

