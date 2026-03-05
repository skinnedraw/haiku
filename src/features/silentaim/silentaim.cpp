#define NOMINMAX
#include <Windows.h>
#include <memory/memory.h>
#include <sdk/sdk.h>
#include <sdk/offsets.h>
#include <game/game.h>
#include <cache/cache.h>
#include <settings.h>
#include "silentaim.h"

namespace rbx::silentaim
{
	cache::entity_t* get_closest_player_near_mouse(float max_pixels)
	{
		POINT cursor;
		GetCursorPos(&cursor);
		ScreenToClient(game::wnd, &cursor);

		math::vector2 cursor_pos = { static_cast<float>(cursor.x), static_cast<float>(cursor.y) };

		cache::entity_t* closest_player = nullptr;
		float closest_distance = max_pixels;

		math::vector2 dims = game::visengine.get_dimensions();
		math::matrix4 view = game::visengine.get_viewmatrix();

		std::lock_guard<std::mutex> lock(cache::mtx);

		for (auto& player : cache::cached_players)
		{
			if (player.instance.address == game::local_player.address)
				continue;

			auto head_it = player.parts.find("Head");
			if (head_it == player.parts.end())
				continue;

			rbx::primitive_t head_prim = head_it->second.get_primitive();
			if (!head_prim.address)
				continue;

			math::vector3 head_pos = head_prim.get_position();
			math::vector2 head_screen;

			if (!game::visengine.world_to_screen(head_pos, head_screen, dims, view))
				continue;

			math::vector2 diff = { head_screen.x - cursor_pos.x, head_screen.y - cursor_pos.y };
			float distance = diff.magnitude();

			if (distance < closest_distance)
			{
				closest_distance = distance;
				closest_player = &player;
			}
		}

		return closest_player;
	}

	void run()
	{
		// High priority dedicated thread — critical for consistent viewport writes
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

		// Also boost the whole process so Windows doesn't deschedule us mid-shot
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

		static bool key_was_pressed = false;
		static bool toggle_state = false;

		// Wait for camera to be valid
		while (!game::workspace.address)
			Sleep(100);

		rbx::camera_t camera = { game::workspace.find_first_child("Camera").address };
		while (!camera.address)
		{
			Sleep(100);
			camera = { game::workspace.find_first_child("Camera").address };
		}

		std::uint64_t viewport_addr = camera.address + OFF(Camera, Viewport);
		math::vector2int16 default_viewport = memory->read<math::vector2int16>(viewport_addr);

		for (;;)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(5));

			bool key_pressed = false;
			if (settings::silentaim::keybind_mode == 0) // Hold
			{
				key_pressed = GetAsyncKeyState(settings::silentaim::keybind) & 0x8000;
				if (!key_pressed)
					sticky_target_address = 0;
			}
			else if (settings::silentaim::keybind_mode == 1) // Toggle
			{
				bool current_key_state = GetAsyncKeyState(settings::silentaim::keybind) & 0x8000;
				if (current_key_state && !key_was_pressed)
				{
					toggle_state = !toggle_state;
					key_was_pressed = true;
					if (!toggle_state)
						sticky_target_address = 0;
				}
				else if (!current_key_state)
				{
					key_was_pressed = false;
				}
				key_pressed = toggle_state;
			}
			else if (settings::silentaim::keybind_mode == 2) // Always
			{
				key_pressed = true;
			}

			bool should_silent_aim = settings::silentaim::enabled && key_pressed;

			if (!should_silent_aim)
			{
				memory->write<math::vector2int16>(viewport_addr, default_viewport);
				has_target = false;
				sticky_target_address = 0;
				continue;
			}

			// Find target
			cache::entity_t* target = nullptr;

			if (settings::silentaim::sticky_aim && sticky_target_address != 0)
			{
				std::lock_guard<std::mutex> lock(cache::mtx);
				for (auto& player : cache::cached_players)
				{
					if (player.instance.address == sticky_target_address)
					{
						target = &player;
						break;
					}
				}
				if (!target)
					sticky_target_address = 0;
			}

			if (!target)
			{
				target = get_closest_player_near_mouse(settings::silentaim::fov);
				if (target && settings::silentaim::sticky_aim)
					sticky_target_address = target->instance.address;
			}

			if (!target)
			{
				memory->write<math::vector2int16>(viewport_addr, default_viewport);
				has_target = false;
				continue;
			}

			auto head_it = target->parts.find("Head");
			if (head_it == target->parts.end())
			{
				has_target = false;
				continue;
			}

			rbx::primitive_t head_prim = head_it->second.get_primitive();
			if (!head_prim.address)
			{
				has_target = false;
				continue;
			}

			math::vector3 head_pos = head_prim.get_position();

			current_target_position = head_pos;
			has_target = true;

			math::vector2 dims = game::visengine.get_dimensions();
			math::matrix4 view = game::visengine.get_viewmatrix();

			math::vector2 target_screen;
			if (!game::visengine.world_to_screen(head_pos, target_screen, dims, view))
			{
				has_target = false;
				continue;
			}

			math::vector2int16 viewport_offset;
			viewport_offset.x = static_cast<std::int16_t>(2 * (dims.x - target_screen.x));
			viewport_offset.y = static_cast<std::int16_t>(2 * (dims.y - target_screen.y));

			// Spam write viewport — more writes = less chance of a missed shot window
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
			memory->write<math::vector2int16>(viewport_addr, viewport_offset);
		}
	}
}