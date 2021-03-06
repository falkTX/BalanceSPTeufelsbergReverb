/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2016 - ROLI Ltd.

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

   -----------------------------------------------------------------------------

   To release a closed-source product which uses other parts of JUCE not
   licensed under the ISC terms, commercial licenses are available: visit
   www.juce.com for more information.

  ==============================================================================
*/

#ifdef JUCE_AUDIO_DEVICES_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#include "AppConfig.h"

#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1
#define JUCE_CORE_INCLUDE_COM_SMART_PTR 1
#define JUCE_CORE_INCLUDE_JNI_HELPERS 1
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1
#define JUCE_EVENTS_INCLUDE_WIN32_MESSAGE_WINDOW 1

#include "juce_audio_devices.h"

//==============================================================================
#if JUCE_MAC
 #define Point CarbonDummyPointName
 #define Component CarbonDummyCompName
 #import <CoreAudio/AudioHardware.h>
 #import <CoreMIDI/MIDIServices.h>
 #import <AudioToolbox/AudioServices.h>
 #undef Point
 #undef Component

#elif JUCE_IOS
 #import <AudioToolbox/AudioToolbox.h>
 #import <AVFoundation/AVFoundation.h>
 #import <CoreMIDI/MIDIServices.h>

 #if TARGET_OS_SIMULATOR
  #import <CoreMIDI/MIDINetworkSession.h>
 #endif

//==============================================================================
#elif JUCE_WINDOWS
 #if JUCE_WASAPI
  #include <mmreg.h>
 #endif

 #if JUCE_ASIO
  /* This is very frustrating - we only need to use a handful of definitions from
     a couple of the header files in Steinberg's ASIO SDK, and it'd be easy to copy
     about 30 lines of code into this cpp file to create a fully stand-alone ASIO
     implementation...

     ..unfortunately that would break Steinberg's license agreement for use of
     their SDK, so I'm not allowed to do this.

     This means that anyone who wants to use JUCE's ASIO abilities will have to:

     1) Agree to Steinberg's licensing terms and download the ASIO SDK
         (see http://www.steinberg.net/en/company/developers.html).

     2) Enable this code with a global definition #define JUCE_ASIO 1.

     3) Make sure that your header search path contains the iasiodrv.h file that
        comes with the SDK. (Only about a handful of the SDK header files are actually
        needed - so to simplify things, you could just copy these into your JUCE directory).
  */
  #include <iasiodrv.h>
 #endif

//==============================================================================
#elif JUCE_LINUX
 #if JUCE_ALSA
  /* Got an include error here? If so, you've either not got ALSA installed, or you've
     not got your paths set up correctly to find its header files.

     The package you need to install to get ASLA support is "libasound2-dev".

     If you don't have the ALSA library and don't want to build Juce with audio support,
     just set the JUCE_ALSA flag to 0.
  */
  #include <alsa/asoundlib.h>
 #endif

 #if JUCE_JACK
  /* Got an include error here? If so, you've either not got jack-audio-connection-kit
     installed, or you've not got your paths set up correctly to find its header files.

     The package you need to install to get JACK support is "libjack-dev".

     If you don't have the jack-audio-connection-kit library and don't want to build
     Juce with low latency audio support, just set the JUCE_JACK flag to 0.
  */
  #include <jack/jack.h>
 #endif
 #undef SIZEOF

//==============================================================================
#elif JUCE_ANDROID

 #if JUCE_USE_ANDROID_OPENSLES
  #include <SLES/OpenSLES.h>
  #include <SLES/OpenSLES_Android.h>
  #include <SLES/OpenSLES_AndroidConfiguration.h>
 #endif

#endif

namespace juce
{

#include "audio_io/juce_AudioDeviceManager.cpp"
#include "audio_io/juce_AudioIODevice.cpp"
#include "audio_io/juce_AudioIODeviceType.cpp"
#include "midi_io/juce_MidiMessageCollector.cpp"
#include "midi_io/juce_MidiOutput.cpp"
#include "sources/juce_AudioSourcePlayer.cpp"
#include "sources/juce_AudioTransportSource.cpp"
#include "native/juce_MidiDataConcatenator.h"

//==============================================================================
#if JUCE_MAC
 #include "native/juce_mac_CoreAudio.cpp"
 #include "native/juce_mac_CoreMidi.cpp"

//==============================================================================
#elif JUCE_IOS
 #include "native/juce_ios_Audio.cpp"
 #include "native/juce_mac_CoreMidi.cpp"

//==============================================================================
#elif JUCE_WINDOWS

 #if JUCE_WASAPI
  #include "native/juce_win32_WASAPI.cpp"
 #endif

 #if JUCE_DIRECTSOUND
  #include "native/juce_win32_DirectSound.cpp"
 #endif

 #include "native/juce_win32_Midi.cpp"

 #if JUCE_ASIO
  #include "native/juce_win32_ASIO.cpp"
 #endif

//==============================================================================
#elif JUCE_LINUX
 #if JUCE_ALSA
  #include "native/juce_linux_ALSA.cpp"
 #endif

 #include "native/juce_linux_Midi.cpp"

 #if JUCE_JACK
  #include "native/juce_linux_JackAudio.cpp"
 #endif

//==============================================================================
#elif JUCE_ANDROID
 #include "native/juce_android_Audio.cpp"
 #include "native/juce_android_Midi.cpp"

 #if JUCE_USE_ANDROID_OPENSLES
  #include "native/juce_android_OpenSL.cpp"
 #endif

#endif

#if ! JUCE_SYSTEMAUDIOVOL_IMPLEMENTED
 // None of these methods are available. (On Windows you might need to enable WASAPI for this)
 float JUCE_CALLTYPE SystemAudioVolume::getGain()         { jassertfalse; return 0.0f; }
 bool  JUCE_CALLTYPE SystemAudioVolume::setGain (float)   { jassertfalse; return false; }
 bool  JUCE_CALLTYPE SystemAudioVolume::isMuted()         { jassertfalse; return false; }
 bool  JUCE_CALLTYPE SystemAudioVolume::setMuted (bool)   { jassertfalse; return false; }
#endif
}
