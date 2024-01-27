/*
	Attempt at debouncing buttons efficiently and with minimal latency at the positive edge
	This assumes "updateButton" is called at a rate signifficantly (at least an order of magnitude)
	above the haptic rate
	Shoutouts to Rob Bothof for the "using a byte as a history of states" trick
	
		Copyright (C) 2017  Kassen Oud
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

//our struct
struct Button
{
	bool state;					//current state of the button as we see it
	unsigned char history;		//history of the last 8 measured states
	unsigned char pin;			//the digital in we'll poll.
};

//takes a pointer to Button struct returns true if the state changed,
//at which point "my_button.state" can be used by your program to get the new state.
bool updateButton( struct Button *but)
{
	//note we use the negation here because we assume pullup inputs are used
	//delete the "!" if you're not using those.
	bool in = digitalRead( but->pin );
	//push back history
	but->history = but->history << 1;
	//store input in most recent history location
	but->history = but->history | (unsigned char)(in);
	if (but->state)
	{
		if (!but->history)										//last 8 values were all "off"
		{
			but->state = false;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (in)													//we assume buttons do not accidentally turn on
		{
			but->state = true;
			return true;
		}
		else
		{
			return false;;
		}
	}
}
