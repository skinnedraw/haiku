#define NOMINMAX
#include <Windows.h>
#include <memory/memory.h>
#include <sdk/sdk.h>
#include <sdk/offsets.h>
#include <game/game.h>
#include <settings.h>
#include "tickrate.h"

namespace tickrate
{
	static bool set_tickrate(float tickrate)
	{
		std::uint64_t world = memory->read<std::uint64_t>(game::workspace.address + OFF(Workspace, World));
		if (!world) return false;

		memory->write<float>(world + OFF(World, worldStepsPerSec), tickrate * 4.f);
		return true;
	}

	void run()
	{
		static bool key_was_pressed = false;
		static bool toggle_state = false;

		for (;;)
		{
			Sleep(1);

			bool key_pressed = false;
			if (settings::tickrate::keybind_mode == 0) // Hold
			{
				key_pressed = GetAsyncKeyState(settings::tickrate::keybind) & 0x8000;
			}
			else if (settings::tickrate::keybind_mode == 1) // Toggle
			{
				bool current = GetAsyncKeyState(settings::tickrate::keybind) & 0x8000;
				if (current && !key_was_pressed)
				{
					toggle_state = !toggle_state;
					key_was_pressed = true;
				}
				else if (!current)
				{
					key_was_pressed = false;
				}
				key_pressed = toggle_state;
			}
			else if (settings::tickrate::keybind_mode == 2) // Always
			{
				key_pressed = true;
			}

			bool active = settings::tickrate::enabled && key_pressed;

			if (active)
				set_tickrate(static_cast<float>(settings::tickrate::value));
			else
				set_tickrate(60.f);
		}
	}
}