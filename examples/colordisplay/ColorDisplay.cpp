/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "ColorDisplay.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include <lodepng.h>

#include <cabl/gfx/Canvas.h>

namespace
{
}

//--------------------------------------------------------------------------------------------------

namespace sl
{

using namespace std::placeholders;

//--------------------------------------------------------------------------------------------------

ColorDisplay::ColorDisplay() : Client({"ˆ(Ableton Push 2)"})
{
}

//--------------------------------------------------------------------------------------------------

void ColorDisplay::initDevice()
{
}

//--------------------------------------------------------------------------------------------------

void ColorDisplay::render()
{
}
/*
//--------------------------------------------------------------------------------------------------

void DisplayClient::buttonChanged(Device::Button button_, bool buttonState_, bool shiftState_)
{
}

//--------------------------------------------------------------------------------------------------

void DisplayClient::encoderChanged(unsigned encoder_, bool valueIncreased_, bool shiftPressed_)
{
}

//--------------------------------------------------------------------------------------------------

void DisplayClient::keyChanged(unsigned index_, double value_, bool shiftPressed)
{
}

//--------------------------------------------------------------------------------------------------

void DisplayClient::controlChanged(unsigned pot_, double value_, bool shiftPressed)
{
}
*/

//--------------------------------------------------------------------------------------------------

} // namespace sl
