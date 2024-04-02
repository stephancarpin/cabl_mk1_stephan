/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <future>
#include <cstdint>

#include <cabl/cabl.h>

#include "Sequence.h"

#ifdef __APPLE__
#define __MACOSX_CORE__ 1
#elif defined(_WIN32)
#define __WINDOWS_MM__ 1
#endif
#include "RtMidi.h"



namespace sl
{

using namespace cabl;


class Euklid : public Client
{
public:
  enum class EncoderState
  {
    Length,
    Pulses,
    Rotate,
    Shuffle,
    Speed,
  };

  enum class ScreenPage
  {
    Sequencer,
    Configuration,
  };

  Euklid();

  void initDevice() override;
  void render() override;

  void buttonChanged(Device::Button button_, bool buttonState_, bool shiftState_) override;
  void encoderChanged(unsigned encoder_, bool valueIncreased_, bool shiftPressed_) override;
  void keyChanged(unsigned index_, double value_, bool shiftPressed) override;
  void controlChanged(unsigned pot_, double value_, bool shiftPressed) override;

  void updateClock();

  void play();

  void updateGUI();
  void updateGroupLeds();
  void updatePads();
  void updateTouchStrips();

  void drawConfigurationPage();
  void drawSequencerPage();

  void setEncoderState(EncoderState encoderState_)
  {
    m_encoderState = encoderState_;
  }

  ScreenPage screenPage() const
  {
    return m_screenPage;
  }
  void setScreenPage(ScreenPage screenPage_)
  {
    m_screenPage = screenPage_;
  }

  void setEncoder(bool valueIncreased_, bool shiftPressed_);

  void togglePlay();

  void changeTrack(uint8_t track_ = 0xFF);
  void nextTrack();
  void prevTrack();
   std::vector<unsigned char> note_on{144,63,127};
   std::vector<unsigned char> note_off{128,63,0};
   std::vector<unsigned char> ctrl_change{176,7,0};

private:
  uint8_t encoderValue(bool valueIncreased_,
    uint8_t step_,
    uint8_t currentValue_,
    uint8_t minValue_,
    uint8_t maxValue_);
  
  Sequence<unsigned> m_sequences[3];

  std::future<void> m_clockFuture;
  EncoderState m_encoderState;
  ScreenPage m_screenPage;

  uint8_t m_lengths[3];
  uint8_t m_pulses[3];
  uint8_t m_rotates[3];
  
  std::atomic<bool> m_play;
  uint8_t m_currentTrack;

  double m_bpm;
  double m_shuffle;
  uint8_t m_encoder_konb[19];

  RtMidiOut* m_pMidiout;

  uint8_t m_quarterNote;
  unsigned m_delayEven;
  unsigned m_delayOdd;
  unsigned char valuePads;
  unsigned  char indexPads;
  unsigned  char indexbutton_;
  bool m_btn_state[100];
  double findmax[20];
  bool peak[20];
  bool peak_low[20];
  unsigned int lastEvent[20];
  int DEBOUNCE;
  bool activePads[20];

};

} // namespace sl
