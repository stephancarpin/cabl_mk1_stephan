/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "Euklid.h"



#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "RtMidi.h"
// Platform-dependent sleep routines.
#if defined(_WIN32)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#include <unmidify.hpp>
#include <sys/time.h>

#include <cabl/gfx/TextDisplay.h>
#include <unistd.h>
#include <sys/time.h>
unsigned int millisz () {
  struct timeval t ;
  gettimeofday ( & t , NULL ) ;
  return t.tv_sec * 1000 + ( t.tv_usec + 500 ) / 1000 ;
}
namespace
{
const uint8_t kEuklidDefaultSteps = 16;
const uint8_t kEuklidDefaultPulses = 4;
const uint8_t kEuklidDefaultOffset = 0;
const uint8_t kEuklidNumTracks = 3;

const sl::Color kEuklidColor_Track[3] = {{60, 0, 0, 80}, {0, 60, 0, 80}, {0, 0, 60, 80}};
const sl::Color kEuklidColor_Track_CurrentStep[3]
  = {{127, 0, 0, 127}, {0, 127, 0, 127}, {0, 0, 127, 127}};

const sl::Color kEuklidColor_Black(0, 0, 0, 0);

const sl::Color kEuklidColor_Step_Empty(35, 35, 35, 20);
const sl::Color kEuklidColor_Step_Empty_Current(127, 127, 127, 50);
}

// This function should

//be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.

//--------------------------------------------------------------------------------------------------

namespace sl
{
//using namespace midi;
using namespace std::placeholders;

//--------------------------------------------------------------------------------------------------

Euklid::Euklid()
  : m_encoderState(EncoderState::Length)
  , m_screenPage(ScreenPage::Sequencer)
  , m_play(false)
  , m_currentTrack(0)
  , m_bpm(120.)
  , m_shuffle(60.)
  , m_pMidiout(new RtMidiOut)
  , m_delayEven(125)
  , m_delayOdd(125)
  ,valuePads(0)
  ,DEBOUNCE(80)
 
  
{
  for (uint8_t i = 0; i < kEuklidNumTracks; i++)
  {
    m_lengths[i] = kEuklidDefaultSteps;
    m_pulses[i] = kEuklidDefaultPulses;
    m_rotates[i] = kEuklidDefaultOffset;
    m_sequences[i].calculate(m_lengths[i], m_pulses[i]);
    m_sequences[i].rotate(m_rotates[i]);
  }
  
  
  

    m_pMidiout->openVirtualPort("Euklid");
  peak[0]=false;
  peak_low[0]=false;
  activePads[0]=false;
  
}

//--------------------------------------------------------------------------------------------------

void Euklid::initDevice()
{
  device()->setKeyLed(0, kEuklidColor_Track[0]);
  
}

//--------------------------------------------------------------------------------------------------

void Euklid::render()
{
  updateGUI();
  updateGroupLeds();
  updatePads();
  //updateTouchStrips();
}




//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

void Euklid::encoderChanged(unsigned encoder_, bool valueIncreased_, bool shiftPressed_)
{
  uint8_t step = (shiftPressed_ ? 10 : 1);
  for(uint8_t y=0;y<=8;y++)
  {
	  if((uint8_t)encoder_ == y){
	    
	    
	    if(shiftPressed_){//when shift pressed
         y =  y + 9;
      }
		  
     m_encoder_konb[y] = encoderValue(valueIncreased_,step,m_encoder_konb[y] , 0, 127);
    // std::cout << "encoder Knobs: "<< y <<" is: " <<  m_encoder_konb[y] << std::endl;
     //Control Change: 176, 7, 100 (volume)
     
     if(m_encoder_konb[y] >100 && valueIncreased_)
     {
       m_encoder_konb[y]=127;
     }
     
     
   
      
	 ctrl_change = {176,y,m_encoder_konb[y]};
	 m_pMidiout->sendMessage(&ctrl_change);
		  
		  
		  }
 }
  requestDeviceUpdate();
}

void Euklid::buttonChanged(Device::Button button_, bool buttonState_, bool shiftState_)
{
     
       indexbutton_ =  (unsigned char)button_ + 20;
     

     if(buttonState_ && !m_btn_state[indexbutton_])//Mute button
     {
        ctrl_change = {176,indexbutton_,0};
	      m_pMidiout->sendMessage(&ctrl_change);
	      m_btn_state[indexbutton_] = true;
	      device()->setButtonLed(button_, 255);
     }else if(buttonState_ && m_btn_state[indexbutton_])
     {
        ctrl_change = {176,indexbutton_,127};
	    m_pMidiout->sendMessage(&ctrl_change);
	      m_btn_state[indexbutton_] = false;
	      device()->setButtonLed(button_, 0);
     }
     

 
   
     


  // std::cout << "button: "<<unsigned(Device::Button::F1)<< std::endl;
  if (button_ == Device::Button::F1 || button_ == Device::Button::DisplayButton1)
  {
   // if (screenPage() == Euklid::ScreenPage::Configuration)
   // {
   //   setEncoderState(Euklid::EncoderState::Speed);
  //  }
 //   else
  //  {
 //     setEncoderState(Euklid::EncoderState::Length);
  //  }
  }
  else if (button_ == Device::Button::F2 || button_ == Device::Button::DisplayButton2)
  {
    // if (screenPage() == Euklid::ScreenPage::Configuration)
    // {
    //   setEncoderState(Euklid::EncoderState::Shuffle);
    // }
    // else
    // {
    //   setEncoderState(Euklid::EncoderState::Pulses);
    // }
  }
  else if (button_ == Device::Button::F3 || button_ == Device::Button::DisplayButton3)
  {
    // if (screenPage() == Euklid::ScreenPage::Sequencer)
    // {
    //   setEncoderState(Euklid::EncoderState::Rotate);
    // }
  }
  else if (buttonState_ && (button_ == Device::Button::Group || button_ == Device::Button::Browse))
  {
    //changeTrack();
  }
  else if (buttonState_ && button_ == Device::Button::PageLeft)
  {
   // prevTrack();
  }
  else if (buttonState_ && button_ == Device::Button::PageRight)
  {
    //nextTrack();
  }
  else if (buttonState_ && button_ == Device::Button::GroupA)
  {
   // changeTrack(0);
  }
  else if (buttonState_ && button_ == Device::Button::GroupB)
  {
    //changeTrack(1);
  }
  else if (buttonState_ && button_ == Device::Button::GroupC)
  {
   // changeTrack(2);
  }
  else if ((button_ == Device::Button::Play || button_ == Device::Button::Sync) && buttonState_)
  {
    togglePlay();
  }
  else if (button_ == Device::Button::Control && buttonState_)
  {
    setScreenPage(screenPage() == Euklid::ScreenPage::Configuration
                    ? Euklid::ScreenPage::Sequencer
                    : Euklid::ScreenPage::Configuration);
  }
  else if (button_ >= Device::Button::Pad1 && button_ <= Device::Button::Pad16 && buttonState_)
  {

    uint8_t padIndex = static_cast<uint8_t>(button_) - static_cast<uint8_t>(Device::Button::Pad1);
    
    m_sequences[m_currentTrack].toggleStep(padIndex);
  }
  else
  {
    return;
  }

  requestDeviceUpdate();
}


//--------------------------------------------------------------------------------------------------

void Euklid::keyChanged(unsigned index_, double value_, bool shiftPressed_)
{

  //std::cout << "indexPads : " << (int)index_ << "value: "<<int(value_*127) << std::endl;

  //static auto lastEvent[] = std::chrono::system_clock::now();
  // auto nowz = std::chrono::system_clock::now();
  // auto duration_in_sec= std::chrono::duration<double>(nowz.time_since_epoch());
  // double now = duration_in_sec.count;
  

               
                 if( activePads[index_] && value_== 0 ) {  
                  
                  
                 
                  
                  // if(value_ >0.002)
                  // {
                  //       valuePads    =  (value_*127.0);
                  //   //std::cout << "sends after touch" << value_  << std::endl;//160 key pressuire
                  //   	ctrl_change =  {	160,indexPads,valuePads};
                  //     m_pMidiout->sendMessage(&ctrl_change);
                     	  
                    
                  // }
                  
                  
                  
                  
                
                    
                      switch (int(index_)){
                          	  case 0:
                            	{
                              		indexPads = (unsigned char)48;
                              		break;
                            	}
                            	case 1:
                            	{
                              		indexPads = (unsigned char)49;
                              		break;
                            	}
                            	case 2:
                            	{
                              		indexPads = (unsigned char)50;
                              		break;
                            	}
                            	case 3:
                            	{
                              		indexPads = (unsigned char)51;
                              		break;
                            	}
                          	  case 4:
                            	{
                              		indexPads = (unsigned char)44;
                              		break;
                            	}
                            	case 5:
                            	{
                              		indexPads = (unsigned char)45;
                              		break;
                            	}
                            	case 6:
                            	{
                              		indexPads = (unsigned char)46;
                            	}
                            	case 7:
                            	{
                              		indexPads = (unsigned char)47;
                              		break;
                            	}
                          	  case 8:
                            	{
                              		indexPads = (unsigned char)40;
                              		break;
                            	}
                            	case 9:
                            	{
                              		indexPads = (unsigned char)41;
                              		break;
                            	}
                            	case 10:
                            	{
                              		indexPads = (unsigned char)42;
                              		break;
                            	}
                            	case 11:
                            	{
                              		indexPads = (unsigned char)43;
                              		break;
                            	}
                            	case 12:
                            	{
                              		indexPads = (unsigned char)36;
                              		
                              		break;
                            	}
                            	case 13:
                            	{
                              		indexPads = (unsigned char)37;
                              		break;
                            	}
                            	case 14:
                            	{
                              		indexPads = (unsigned char)38;
                              		break;
                            	}
                            	case 15:
                            	{
                              		indexPads = (unsigned char)39;
                              		break;
                            	}
                          	 } 
                   
                    	   
                    	 	  
                	  	ctrl_change  =  {128,indexPads,0};
                   	  m_pMidiout->sendMessage(&ctrl_change);
                 	  
                  	  findmax[index_]      = 0;
                	 
                      peak[index_]          = false;
                      peak_low[index_]      = false;
                      activePads[index_]    = false;
                      
                	    std::cout << "Send OFFFF       "<< std::endl;
                	 	  //std::cout << "peak[index_]        " << peak[index_] << std::endl;
                	 	  //std::cout << "peak_low[index_]    " << peak_low[index_] << std::endl;
                	 	  //std::cout << "activePads[index_]  " << activePads[index_] << std::endl;
                	 	  
                   
                	    lastEvent[index_] = millisz();
                    	
                 
                  
                }
               
               
                 if( !activePads[index_] && millisz() - lastEvent[index_] > DEBOUNCE ) {  
                   
                   
                    if( value_ > findmax[index_])
                    {
                      
                        findmax[index_] = value_;
                       // std::cout << "Peak high : " << findmax[index_]  << std::endl;
                        std::cout << "FindMax ------indexPads : " << (int)index_ << "  value : "<<(double)value_  << std::endl;
                        
                    
                      
                    } else {
                      
                            
                        switch (int(index_))
                        	{
                        	  case 0:
                          	{
                            		indexPads = (unsigned char)48;
                            		break;
                          	}
                          	case 1:
                          	{
                            		indexPads = (unsigned char)49;
                            		break;
                          	}
                          	case 2:
                          	{
                            		indexPads = (unsigned char)50;
                            		break;
                          	}
                          	case 3:
                          	{
                            		indexPads = (unsigned char)51;
                            		break;
                          	}
                        	  case 4:
                          	{
                            		indexPads = (unsigned char)44;
                            		break;
                          	}
                          	case 5:
                          	{
                            		indexPads = (unsigned char)45;
                            		break;
                          	}
                          	case 6:
                          	{
                            		indexPads = (unsigned char)46;
                          	}
                          	case 7:
                          	{
                            		indexPads = (unsigned char)47;
                            		break;
                          	}
                        	  case 8:
                          	{
                            		indexPads = (unsigned char)40;
                            		break;
                          	}
                          	case 9:
                          	{
                            		indexPads = (unsigned char)41;
                            		break;
                          	}
                          	case 10:
                          	{
                            		indexPads = (unsigned char)42;
                            		break;
                          	}
                          	case 11:
                          	{
                            		indexPads = (unsigned char)43;
                            		break;
                          	}
                          	case 12:
                          	{
                            		indexPads = (unsigned char)36;
                            		
                            		break;
                          	}
                          	case 13:
                          	{
                            		indexPads = (unsigned char)37;
                            		break;
                          	}
                          	case 14:
                          	{
                            		indexPads = (unsigned char)38;
                            		break;
                          	}
                          	case 15:
                          	{
                            		indexPads = (unsigned char)39;
                            		break;
                          	}
                        	 } 
                     
                        //Control Change: 176, 7, 100 (volume)
                        valuePads    =  ((value_+0.2)*127.0);
                        
                        if(valuePads >= 127)
                        {
                            valuePads = 127;
                        }
                         
                	       ctrl_change  =  {144,indexPads,valuePads};
                	       
                	        m_pMidiout->sendMessage(&ctrl_change);
                	
                      	
                      	   //  std::cout <<"------------------------ " << (int)index_  << std::endl;
                      	   std::cout << "Midi sent on: " << (int)index_ << ":"<< (int)valuePads  <<" value: "<<(double)value_  << std::endl;
                      	 //	 std::cout << "valuePads " << valuePads << std::endl;
                      	 	 // std::cout << "peak_low[index_] " << peak_low[index_] << std::endl;
                      	 	//  std::cout << "activePads[index_]" << activePads[index_] << std::endl;
                      	 
                      	 // findmax[index_]       = 0;
                      	 
                          activePads[index_]    = true;
                         
                      	  lastEvent[index_]     = millisz();
                      	
                        
                    }
                    
               
                
                
                
                 
                
    
           
           
        
      }
 // requestDeviceUpdate();
  

   
}

//--------------------------------------------------------------------------------------------------

void Euklid::controlChanged(unsigned pot_, double value_, bool shiftPressed_)
{
  switch (pot_)
  {
    case 0:
    {
      m_lengths[m_currentTrack] = std::max<uint8_t>(1, static_cast<uint8_t>((value_ * 16) + 0.5));
      m_sequences[m_currentTrack].calculate(m_lengths[m_currentTrack], m_pulses[m_currentTrack]);
      m_sequences[m_currentTrack].rotate(m_rotates[m_currentTrack]);
      break;
    }
    case 1:
    {
      m_pulses[m_currentTrack]
        = std::max<uint8_t>(0, static_cast<uint8_t>((m_lengths[m_currentTrack] * value_) + 0.5));
      m_sequences[m_currentTrack].calculate(m_lengths[m_currentTrack], m_pulses[m_currentTrack]);
      m_sequences[m_currentTrack].rotate(m_rotates[m_currentTrack]);
      break;
    }
    case 2:
    {

      m_rotates[m_currentTrack]
        = std::max<uint8_t>(0, static_cast<uint8_t>((m_lengths[m_currentTrack] * value_) + 0.5));
      m_sequences[m_currentTrack].rotate(m_rotates[m_currentTrack]);
      break;
    }
    case 3:
    {
      m_bpm = (value_ * 195) + 60;
      updateClock();
      break;
    }
    case 4:
    {
      m_shuffle = value_ * 100;
      updateClock();
      break;
    }
    default:
      break;
  }
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void Euklid::updateClock()
{
  double quarterDuration = 60000.0f / m_bpm;
  double delayQuarterNote = quarterDuration / 4.0f;
  double shuffleDelay = delayQuarterNote * (m_shuffle / 300.0f);
  m_delayEven = static_cast<unsigned>(delayQuarterNote + shuffleDelay);
  m_delayOdd = static_cast<unsigned>(delayQuarterNote - shuffleDelay);
}

//--------------------------------------------------------------------------------------------------

void Euklid::play()
{
  
 
  m_quarterNote = 0;
  updateClock();
 
  while (m_play)
  {
    requestDeviceUpdate();
    unsigned delay = m_delayEven;
    if (m_quarterNote % 2 > 0)
    {
      delay = m_delayOdd;
    }

    if (++m_quarterNote > 3)
    {
      m_quarterNote = 0;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    for (uint8_t i = 0; i < kEuklidNumTracks; i++)
    {
      note_on= {144,36,120};
      note_off= {128,36,0};
         
      if (i == 1)
      {
       note_on= {144,44,120};
       note_off= {128,44,0};
        //note.setNote(MidiNote::Name::D);
      }
      else if (i == 2)
      {
         note_on = {144,37,120};
         note_off= {144,37,0};
        //note.setNote(MidiNote::Name::FSharp);
      }
      if (m_sequences[i].next())
      {
      
        
          m_pMidiout->sendMessage(&note_on);
          
          m_pMidiout->sendMessage(&note_off);
      
           
            /*//MidiMessage* m = new NoteOn(0, note.value() , 127);
              NoteOn noteObj(0, note , 127);
              std::vector<uint8_t> msg(noteObj.data());
              m_pMidiout->sendMessage(&msg);
           //   device()->sendMidiMsg(msg);*/
             // Note On: 144, 64, 90
  			
           
           
           
           
           
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Euklid::updateGUI()
{
  for (unsigned j = 0; j < device()->graphicDisplay(0)->width(); j++)
  {
    for (unsigned i = 0; i < device()->graphicDisplay(0)->height(); i++)
    {
      device()->graphicDisplay(0)->setPixel(j, i, {static_cast<uint8_t>(j), 0, 0});
    }
  }
  static Color s_colorWhite{0xff};
  static Alignment s_alignCenter = Alignment::Center;

  std::string strTrackName = "TRACK " + std::to_string(m_currentTrack + 1);

  device()->graphicDisplay(0)->black();
  device()->graphicDisplay(1)->putText(32, 52, "Turnado", s_colorWhite, "normal");
  device()->graphicDisplay(0)->rectangleFilled(0, 52, 28, 6, s_colorWhite, s_colorWhite);
  device()->graphicDisplay(0)->rectangleFilled(100, 52, 28, 6, s_colorWhite, s_colorWhite);

  device()->textDisplay(0)->putText("AB", 0);


  device()->textDisplay(0)->putText(strTrackName, 1);
  device()->textDisplay(0)->putText("{EUKLID}", 2, s_alignCenter);

  device()->textDisplay(1)->putText("Length", 1, s_alignCenter);
  device()->textDisplay(1)->putValue(
    static_cast<float>(m_lengths[m_currentTrack]) / kEuklidDefaultSteps, 0);
  device()->textDisplay(1)->putText(static_cast<int>(m_lengths[m_currentTrack]), 2, s_alignCenter);

  device()->textDisplay(2)->putText("Density", 1);
  device()->textDisplay(2)->putValue(
    static_cast<float>(m_pulses[m_currentTrack]) / kEuklidDefaultSteps, 0);
  device()->textDisplay(2)->putText(
    static_cast<double>(m_pulses[m_currentTrack]) / kEuklidDefaultSteps, 2, s_alignCenter);

  device()->textDisplay(3)->putText("Rotation", 1);
  device()->textDisplay(3)->putValue(
    static_cast<float>(m_rotates[m_currentTrack]) / kEuklidDefaultSteps, 0);
  device()->textDisplay(3)->putText(static_cast<int>(m_rotates[m_currentTrack]), 2, s_alignCenter);

  device()->textDisplay(4)->putText("BPM", 1, s_alignCenter);
  device()->textDisplay(4)->putValue(static_cast<double>(m_bpm) / 255.0, 0);
  device()->textDisplay(4)->putText(static_cast<int>(m_bpm), 2, s_alignCenter);

  device()->textDisplay(5)->putText("Shuffle", 1, s_alignCenter);
  device()->textDisplay(5)->putValue(static_cast<float>(m_shuffle) / 100, 0);
  device()->textDisplay(5)->putText(static_cast<int>(m_shuffle), 2, s_alignCenter);

  //  device()->textDisplay(3)->putText(m_rotates[m_currentTrack], 2);

  switch (m_screenPage)
  {
    case ScreenPage::Configuration:
    {
      drawConfigurationPage();
      break;
    }
    case ScreenPage::Sequencer:
    default:
    {
      drawSequencerPage();
      break;
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Euklid::updateGroupLeds()
{
  switch (m_currentTrack)
  {
    case 0:
      device()->setButtonLed(Device::Button::Group, kEuklidColor_Track_CurrentStep[0]);
      device()->setButtonLed(Device::Button::GroupA, kEuklidColor_Track_CurrentStep[0]);
      device()->setButtonLed(Device::Button::GroupB, kEuklidColor_Black);
      device()->setButtonLed(Device::Button::GroupC, kEuklidColor_Black);
      break;
    case 1:
      device()->setButtonLed(Device::Button::Group, kEuklidColor_Track_CurrentStep[1]);
      device()->setButtonLed(Device::Button::GroupA, kEuklidColor_Black);
      device()->setButtonLed(Device::Button::GroupB, kEuklidColor_Track_CurrentStep[1]);
      device()->setButtonLed(Device::Button::GroupC, kEuklidColor_Black);
      break;
    case 2:
      device()->setButtonLed(Device::Button::Group, kEuklidColor_Track_CurrentStep[2]);
      device()->setButtonLed(Device::Button::GroupA, kEuklidColor_Black);
      device()->setButtonLed(Device::Button::GroupB, kEuklidColor_Black);
      device()->setButtonLed(Device::Button::GroupC, kEuklidColor_Track_CurrentStep[2]);
      break;
  }
}

//--------------------------------------------------------------------------------------------------

void Euklid::updatePads()
{
  for (uint8_t t = 0; t < kEuklidNumTracks; t++)
  {
    uint8_t pos = (m_sequences[t].getPos()) % m_lengths[t];

    unsigned pulses = m_sequences[t].getBits();
    for (uint8_t i = 0, j = m_rotates[t]; i < 16; i++, j++)
    {
      if (m_currentTrack == t)
      {

        if (i >= m_lengths[t])
        {
          device()->setKeyLed(i, kEuklidColor_Black);
        }
        else if (pulses & (1 << i))
        {
          if (pos == (j % m_lengths[t]) && m_play)
          {
            device()->setKeyLed(i, kEuklidColor_Track_CurrentStep[m_currentTrack]);
          }
          else
          {
            device()->setKeyLed(i, kEuklidColor_Track[m_currentTrack]);
          }
        }
        else
        {
          if (pos == (j % m_lengths[t]) && m_play)
          {
            device()->setKeyLed(i, kEuklidColor_Step_Empty_Current);
          }
          else
          {
            device()->setKeyLed(i, kEuklidColor_Step_Empty);
          }
        }
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

void Euklid::updateTouchStrips()
{
  device()->ledArray(0)->setValue(m_lengths[m_currentTrack] / 16.0,
    kEuklidColor_Track_CurrentStep[m_currentTrack],
    Alignment::Left);
  device()->ledArray(1)->setValue(
    m_pulses[m_currentTrack] / static_cast<float>(m_lengths[m_currentTrack]),
    kEuklidColor_Track_CurrentStep[m_currentTrack],
    Alignment::Left);
  device()->ledArray(2)->setValue(
    m_rotates[m_currentTrack] / static_cast<float>(m_lengths[m_currentTrack]),
    kEuklidColor_Track_CurrentStep[m_currentTrack],
    Alignment::Left);
  device()->ledArray(3)->setValue((m_bpm - 60) / 195.0, {0xff}, Alignment::Left);
  device()->ledArray(4)->setValue(m_shuffle / 100.0, {0xff}, Alignment::Left);
}

//--------------------------------------------------------------------------------------------------

void Euklid::drawConfigurationPage()
{
  if (m_encoderState != EncoderState::Speed && m_encoderState != EncoderState::Shuffle)
  {
    m_encoderState = EncoderState::Speed;
  }


  device()->graphicDisplay(0)->putText(5, 2, " BPM   Shuffle", {0xff}, "normal");
  device()->graphicDisplay(0)->putText(10, 12, std::to_string(m_bpm).c_str(), {0xff}, "normal");
  device()->graphicDisplay(0)->putText(59, 12, std::to_string(m_shuffle).c_str(), {0xff}, "normal");

  device()->setButtonLed(Device::Button::F1, 0);
  device()->setButtonLed(Device::Button::F2, 0);
  device()->setButtonLed(Device::Button::F3, 0);
  device()->setButtonLed(Device::Button::DisplayButton1, 0);
  device()->setButtonLed(Device::Button::DisplayButton2, 0);
  device()->setButtonLed(Device::Button::DisplayButton3, 0);
  device()->setButtonLed(Device::Button::Control, 255);


  switch (m_encoderState)
  {
    case EncoderState::Shuffle:
    {
      device()->graphicDisplay(0)->rectangleFilled(
        41, 0, 52, 20, {BlendMode::Invert}, {BlendMode::Invert});
      device()->setButtonLed(Device::Button::F2, 255);
      device()->setButtonLed(Device::Button::DisplayButton2, 255);
      break;
    }
    case EncoderState::Speed:
    {
      device()->graphicDisplay(0)->rectangleFilled(
        0, 0, 40, 20, {BlendMode::Invert}, {BlendMode::Invert});
      device()->setButtonLed(Device::Button::F1, 255);
      device()->setButtonLed(Device::Button::DisplayButton1, 255);
      break;
    }
    default:
      break;
  }
}

//--------------------------------------------------------------------------------------------------

void Euklid::drawSequencerPage()
{
  if (m_encoderState != EncoderState::Length && m_encoderState != EncoderState::Pulses
      && m_encoderState != EncoderState::Rotate)
  {
    m_encoderState = EncoderState::Length;
  }

  device()->graphicDisplay(0)->putText(5, 2, "Length Pulses Rotate", {0xff}, "normal");
  for (uint8_t i = 0; i < kEuklidNumTracks; i++)
  {
    for (uint8_t n = 0; n < m_sequences[i].getLength(); n++)
    {
      device()->graphicDisplay(0)->rectangle(n * 8, 15 + (12 * i), 7, 7, {0xff});
    }
  }

  device()->setButtonLed(Device::Button::F1, 0);
  device()->setButtonLed(Device::Button::F2, 0);
  device()->setButtonLed(Device::Button::F3, 0);
  device()->setButtonLed(Device::Button::DisplayButton1, 0);
  device()->setButtonLed(Device::Button::DisplayButton2, 0);
  device()->setButtonLed(Device::Button::DisplayButton3, 0);
  device()->setButtonLed(Device::Button::Control, 0);

  switch (m_encoderState)
  {
    case EncoderState::Pulses:
    {
      device()->graphicDisplay(1)->rectangleFilled(
        43, 0, 42, 10, {BlendMode::Invert}, {BlendMode::Invert});
      device()->setButtonLed(Device::Button::F2, 255);
      device()->setButtonLed(Device::Button::DisplayButton2, 255);
      break;
    }
    case EncoderState::Rotate:
    {
      device()->graphicDisplay(0)->rectangleFilled(
        86, 0, 40, 10, {BlendMode::Invert}, {BlendMode::Invert});
      device()->setButtonLed(Device::Button::F3, 255);
      device()->setButtonLed(Device::Button::DisplayButton3, 255);
      break;
    }
    case EncoderState::Length:
    {
      device()->graphicDisplay(0)->rectangleFilled(
        0, 0, 42, 10, {BlendMode::Invert}, {BlendMode::Invert});
      device()->setButtonLed(Device::Button::F1, 255);
      device()->setButtonLed(Device::Button::DisplayButton1, 255);
      break;
    }
    default:
      break;
  }

  for (uint8_t t = 0; t < kEuklidNumTracks; t++)
  {
    uint8_t pos = (m_sequences[t].getPos()) % m_lengths[t];

    unsigned pulses = m_sequences[t].getBits();
    for (uint8_t i = 0, k = m_rotates[t]; i < 16; i++, k++)
    {
      if (pulses & (1 << i))
      {
        device()->graphicDisplay(0)->rectangleFilled(
          (k % m_lengths[t]) * 8, 15 + (12 * t), 7, 7, {0xff}, {0xff});
      }
    }
    device()->graphicDisplay(0)->rectangle((pos * 8) + 1, 16 + (12 * t), 5, 5, {BlendMode::Invert});
  }
}

//--------------------------------------------------------------------------------------------------

void Euklid::setEncoder(bool valueIncreased_, bool shiftPressed_)
{
  uint8_t step = (shiftPressed_ ? 5 : 1);
  switch (m_encoderState)
  {
    case EncoderState::Pulses:
    {
      m_pulses[m_currentTrack] = encoderValue(
        valueIncreased_, step, m_pulses[m_currentTrack], 0, m_lengths[m_currentTrack]);
      m_sequences[m_currentTrack].calculate(m_lengths[m_currentTrack], m_pulses[m_currentTrack]);
      m_sequences[m_currentTrack].rotate(m_rotates[m_currentTrack]);
      break;
    }
    case EncoderState::Rotate:
    {
      m_rotates[m_currentTrack] = encoderValue(
        valueIncreased_, step, m_rotates[m_currentTrack], 0, m_lengths[m_currentTrack]);
      m_sequences[m_currentTrack].rotate(m_rotates[m_currentTrack]);
      break;
    }
    case EncoderState::Length:
    {
      m_lengths[m_currentTrack]
        = encoderValue(valueIncreased_, step, m_lengths[m_currentTrack], 1, 16);
      m_sequences[m_currentTrack].calculate(m_lengths[m_currentTrack], m_pulses[m_currentTrack]);
      m_sequences[m_currentTrack].rotate(m_rotates[m_currentTrack]);
      break;
    }
    case EncoderState::Shuffle:
    {
      m_shuffle = encoderValue(valueIncreased_, step, m_shuffle, 0, 100);
      updateClock();
      break;
    }
    case EncoderState::Speed:
    {
      m_bpm = encoderValue(valueIncreased_, step, m_bpm, 60, 255);
      updateClock();
      break;
    }
    default:
      break;
  }

  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void Euklid::togglePlay()
{
  m_play = !m_play;
  if (m_play)
  {
    device()->setButtonLed(Device::Button::Play, 255);
    m_clockFuture = std::async(std::launch::async, std::bind(&Euklid::play, this));
  }
  else
  {
    device()->setButtonLed(Device::Button::Play, 0);
    m_clockFuture.get();
    for (uint8_t t = 0; t < kEuklidNumTracks; t++)
    {
      m_sequences[t].reset();
    }
    m_quarterNote = 0;
    requestDeviceUpdate();
  }
}

//--------------------------------------------------------------------------------------------------

void Euklid::changeTrack(uint8_t track_)
{
  if (track_ == 0xFF)
  {
    m_currentTrack++;
    if (m_currentTrack >= kEuklidNumTracks)
    {
      m_currentTrack = 0;
    }
  }
  else
  {
    m_currentTrack = track_;
  }
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void Euklid::nextTrack()
{
  if (m_currentTrack >= (kEuklidNumTracks - 1))
  {
    m_currentTrack = 0;
  }
  else
  {
    m_currentTrack++;
  }
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void Euklid::prevTrack()
{
  if (m_currentTrack > 0)
  {
    m_currentTrack--;
  }
  else
  {
    m_currentTrack = (kEuklidNumTracks - 1);
  }
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

uint8_t Euklid::encoderValue(
  bool valueIncreased_, uint8_t step_, uint8_t currentValue_, uint8_t minValue_, uint8_t maxValue_)
{
  if (valueIncreased_ && ((currentValue_ + step_) <= maxValue_))
  {
    return currentValue_ + step_;
  }
  else if (!valueIncreased_ && ((currentValue_ - step_) >= minValue_))
  {
    return currentValue_ - step_;
  }
  return currentValue_;
}

//--------------------------------------------------------------------------------------------------

} // namespace sl
