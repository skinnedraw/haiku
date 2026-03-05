#include "settings/functions.h"
#include <Windows.h>

void c_gui::render()
{
	if (GetAsyncKeyState(var->gui.menu_key) & 0x1)
		var->gui.menu_opened = !var->gui.menu_opened;

	var->gui.menu_alpha = ImClamp(var->gui.menu_alpha + (gui->fixed_speed(8.f) * (var->gui.menu_opened ? 1.f : -1.f)), 0.f, 1.f);

	if (var->gui.menu_alpha <= 0.01f)
		return;

	gui->set_next_window_pos(ImVec2(GetIO().DisplaySize.x / 2 - var->window.width / 2, 20));
	gui->set_next_window_size(ImVec2(var->window.width, elements->section.size.y + var->window.spacing.y * 2 - 1));
	gui->push_style_var(ImGuiStyleVar_Alpha, var->gui.menu_alpha);
	gui->begin("menu for aori", nullptr, var->window.main_flags);
	{
		const ImVec2 pos = GetWindowPos();
		const ImVec2 size = GetWindowSize();
		ImDrawList* draw_list = GetWindowDrawList();
		ImGuiStyle* style = &GetStyle();

		{
			style->WindowPadding = var->window.padding;
			style->PopupBorderSize = var->window.border_size;
			style->WindowBorderSize = var->window.border_size;
			style->ItemSpacing = var->window.spacing;
			style->WindowShadowSize = var->window.shadow_size;
			style->ScrollbarSize = var->window.scrollbar_size;
			style->Colors[ImGuiCol_WindowShadow] = { clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, var->window.shadow_alpha };
		}

		{
			draw->rect(GetBackgroundDrawList(), pos - ImVec2(1, 1), pos + size + ImVec2(1, 1), draw->get_clr({0, 0, 0, 0.5f}));
			draw->rect_filled(draw_list, pos, pos + size, draw->get_clr(clr->window.background_one));
			draw->line(draw_list, pos + ImVec2(1, 1), pos + ImVec2(size.x - 1, 1), draw->get_clr(clr->accent), 1);
			draw->line(draw_list, pos + ImVec2(1, 2), pos + ImVec2(size.x - 1, 2), draw->get_clr(clr->accent, 0.4f), 1);
			draw->rect(draw_list, pos, pos + size, draw->get_clr(clr->window.stroke));
		}

		{
			gui->set_cursor_pos(style->ItemSpacing);
			gui->begin_group();
			{
				for (int i = 0; i < IM_ARRAYSIZE(var->gui.current_section); i++)
					gui->section(var->gui.section_icons[i], &var->gui.current_section[i]);
			}
			gui->end_group();
		}

		{
			if (var->gui.current_section[0])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("main", nullptr, var->window.flags);
				{
					draw->window_decorations();

					{
						static int subtabs;
						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar));
						gui->begin_group();
						{
							gui->sub_section("Legit", 0, subtabs, 4);
							gui->sub_section("Players", 1, subtabs, 4);
							gui->sub_section("Visuals", 2, subtabs, 4);
							gui->sub_section("Misc", 3, subtabs, 4);
						}
						gui->end_group();

						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + elements->section.height - 1));
						gui->begin_content();
						{
							gui->begin_group();
							{
								gui->begin_child("Aim Assist", 2, 1);
								{
									static bool enabled = true;
									static int key;
									static int mode;
									gui->checkbox("Enabled", &enabled, &key, &mode);

									static int fov = 90;
									gui->slider_int("Field Of View", &fov, 0, 100);

									static int fov_type = 0;
									const char* fov_type_items[2] = { "Static", "Dynamic" };
									gui->dropdown("fov type", &fov_type, fov_type_items, IM_ARRAYSIZE(fov_type_items), true);

									static float horizontal = 30;
									gui->slider_float("Horizontal Smoothing", &horizontal, 0, 100);

									static float vertical = 12;
									gui->slider_float("Vertical Smoothing", &vertical, 0, 100);

									static std::vector<int> checks = { 1, 1, 1 };
									const char* checks_items[3] = { "Team Check", "Alive Check", "Enemy Check" };
									gui->multi_dropdown("Checks", checks, checks_items, IM_ARRAYSIZE(checks_items));

									static std::vector<int> hitboxes = { 1, 0, 1, 0, 1, 0 };
									const char* hitboxes_items[6] = { "Head", "Neck", "Stomach", "Body", "Arms", "Legs" };
									gui->multi_dropdown("Hitboxes", hitboxes, hitboxes_items, IM_ARRAYSIZE(hitboxes_items));

									static bool randomize = false;
									gui->checkbox("Randomize Position", &randomize);

									static int hitscan_type = 0;
									const char* hitscan_type_items[2] = { "Mouse", "Distance" };
									gui->dropdown("Hitscan Type", &hitscan_type, fov_type_items, IM_ARRAYSIZE(fov_type_items));

									static bool readjustment = false;
									static int rkey;
									static int rmode;
									gui->checkbox("Readjustment", &readjustment, &rkey, &rmode);

									static bool deadzone = true;
									gui->checkbox("Dead Zone", &deadzone);

									static float dzone = 44;
									gui->slider_float("dzone", &dzone, 0, 100, true);

									static bool stutter = true;
									gui->checkbox("Stutter", &stutter);

									static float stslider = 25;
									gui->slider_float("stslider", &stslider, 0, 100, true, "%.1ft");
								}
								gui->end_child();
							}
							gui->end_group();

							gui->sameline();

							gui->begin_group();
							{
								gui->begin_child("Trigger Assist", 2, 2);
								{
									static bool enabled = true;
									static int key;
									static int mode;
									gui->checkbox("Enabled", &enabled, &key, &mode);

									static int delay = 15;
									gui->slider_int("Delay", &delay, 0, 500, false, "%dms");

									static int interval = 75;
									gui->slider_int("Interval", &interval, 0, 1000, false, "%dms");

									static std::vector<int> checks = { 1, 1, 1 };
									const char* checks_items[3] = { "Team Check", "Alive Check", "Enemy Check" };
									gui->multi_dropdown("Checks", checks, checks_items, IM_ARRAYSIZE(checks_items));

									static std::vector<int> hitboxes = { 1, 0, 1, 0, 1, 0 };
									const char* hitboxes_items[6] = { "Head", "Neck", "Stomach", "Body", "Arms", "Legs" };
									gui->multi_dropdown("Hitboxes", hitboxes, hitboxes_items, IM_ARRAYSIZE(hitboxes_items));

									static bool readjustment = false;
									static int rkey;
									static int rmode;
									gui->checkbox("Readjustment", &readjustment, &rkey, &rmode);
								}
								gui->end_child();

								gui->begin_child("Misc", 2, 2);
								{
									static bool cursor = true;
									gui->checkbox("Cursor Offset", &cursor);

									static int x = 50;
									gui->slider_int("offsetx", &x, 0, 100, true, "%dpx");

									static int y = 50;
									gui->slider_int("offsety", &y, 0, 100, true, "%dpx");

									gui->label_keybind("MENU KEY", &var->gui.menu_key, 0);
								}
								gui->end_child();
							}
							gui->end_group();
						}
						gui->end_content();
					}

				}
				gui->end();
			}

			if (var->gui.current_section[1])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("other", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}

			if (var->gui.current_section[2])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("players", nullptr, var->window.flags);
				{
					draw->window_decorations();

					{
						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
						gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
						gui->push_style_var(ImGuiStyleVar_ItemSpacing, elements->content.spacing);
						gui->begin_def_child("table test", ImVec2(GetWindowWidth() - elements->content.window_padding.x * 2, GetContentRegionAvail().y - elements->content.window_padding.y * 2), 0, ImGuiWindowFlags_NoMove);
						{
							gui->push_font(var->font.tahoma);
							static int selected_row = -1;

							gui->push_style_color(ImGuiCol_TableBorderLight, draw->get_clr(clr->window.stroke));
							gui->push_style_color(ImGuiCol_TableBorderStrong, draw->get_clr(clr->window.stroke));
							gui->push_style_color(ImGuiCol_TableRowBg, draw->get_clr(clr->window.background_one));
							gui->push_style_color(ImGuiCol_TableRowBgAlt, draw->get_clr(clr->window.background_one));
							gui->push_style_color(ImGuiCol_Text, draw->get_clr(clr->widgets.text_inactive));
							if (gui->begin_table("Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(GetContentRegionAvail().x - 1, 0)))
							{
								for (int row = 0; row < 10; row++)
								{
									gui->table_next_row();

									gui->table_set_column_index(0);
									{
										if (selected_row == row)
											gui->push_style_color(ImGuiCol_Text, draw->get_clr(clr->accent));

										draw->text_outline((std::stringstream{} << "name " << row).str().c_str());

										if (selected_row == row)
											gui->pop_style_color();

										if (IsItemClicked())
											selected_row = row;
									}

									gui->table_set_column_index(1);
									{
										draw->text_outline((std::stringstream{} << "pos " << row).str().c_str());
									}

									gui->table_set_column_index(2);
									{
										draw->text_outline((std::stringstream{} << "info " << row).str().c_str());
									}
								}
								gui->end_table();

								Text((std::stringstream{} << "selected row - " << std::to_string(selected_row)).str().c_str());
								gui->pop_font();
							}
							gui->pop_style_color(5);
						}
						gui->end_def_child();
						gui->pop_style_var(2);
					}

				}
				gui->end();

			}

			if (var->gui.current_section[3])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("menu", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}

			if (var->gui.current_section[4])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("code", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}

			if (var->gui.current_section[5])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("style", nullptr, var->window.flags);
				{
					draw->window_decorations();

					{
						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
						gui->begin_content();
						{
							gui->begin_child("Theme", 1, 2);
							{
								static float menu_accent[4] = { clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f };
								if (gui->label_color_edit("Menu Accent", menu_accent, false))
								{
									clr->accent.Value.x = menu_accent[0];
									clr->accent.Value.y = menu_accent[1];
									clr->accent.Value.z = menu_accent[2];
								}

								static float contrast_one[4] = { clr->window.background_one.Value.x, clr->window.background_one.Value.y, clr->window.background_one.Value.z, 1.f };
								if (gui->label_color_edit("Contrast One", contrast_one, false))
								{
									clr->window.background_one.Value.x = contrast_one[0];
									clr->window.background_one.Value.y = contrast_one[1];
									clr->window.background_one.Value.z = contrast_one[2];
								}

								static float contrast_two[4] = { clr->window.background_two.Value.x, clr->window.background_two.Value.y, clr->window.background_two.Value.z, 1.f };
								if (gui->label_color_edit("Contrast Two", contrast_two, false))
								{
									clr->window.background_two.Value.x = contrast_two[0];
									clr->window.background_two.Value.y = contrast_two[1];
									clr->window.background_two.Value.z = contrast_two[2];
								}

								static float inline_c[4] = { clr->window.stroke.Value.x, clr->window.stroke.Value.y, clr->window.stroke.Value.z, 1.f };
								if (gui->label_color_edit("Inline", inline_c, false))
								{
									clr->window.stroke.Value.x = inline_c[0];
									clr->window.stroke.Value.y = inline_c[1];
									clr->window.stroke.Value.z = inline_c[2];
								}

								static float outline_c[4] = { clr->widgets.stroke_two.Value.x, clr->widgets.stroke_two.Value.y, clr->widgets.stroke_two.Value.z, 1.f };
								if (gui->label_color_edit("Outline", outline_c, false))
								{
									clr->widgets.stroke_two.Value.x = outline_c[0];
									clr->widgets.stroke_two.Value.y = outline_c[1];
									clr->widgets.stroke_two.Value.z = outline_c[2];
								}

								static float text_active[4] = { clr->widgets.text.Value.x, clr->widgets.text.Value.y, clr->widgets.text.Value.z, 1.f };
								if (gui->label_color_edit("Text Active", text_active, false))
								{
									clr->widgets.text.Value.x = text_active[0];
									clr->widgets.text.Value.y = text_active[1];
									clr->widgets.text.Value.z = text_active[2];
								}

								static float text_inactive[4] = { clr->widgets.text_inactive.Value.x, clr->widgets.text_inactive.Value.y, clr->widgets.text_inactive.Value.z, 1.f };
								if (gui->label_color_edit("Text Inctive", text_inactive, false))
								{
									clr->widgets.text_inactive.Value.x = text_inactive[0];
									clr->widgets.text_inactive.Value.y = text_inactive[1];
									clr->widgets.text_inactive.Value.z = text_inactive[2];
								}
							}
							gui->end_child();

							gui->begin_child("Style", 1, 2);
							{
								gui->checkbox("Hover Highlight", &var->window.hover_hightlight);

								static bool window_glow = false;
								gui->checkbox("Window Glow", &window_glow);
								if (window_glow)
								{
									gui->slider_float("Glow Thickness", &var->window.shadow_size, 1, 100);
									gui->slider_float("Glow Alpha", &var->window.shadow_alpha, 0, 1);
								}
								else
									var->window.shadow_size = 0;
							}
							gui->end_child();
						}
						gui->end_content();
					}

				}
				gui->end();
			}

			if (var->gui.current_section[6])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("cloud", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}
		}

		var->window.width = GetCurrentWindow()->ContentSize.x + style->ItemSpacing.x;

		if (IsMouseHoveringRect(pos, pos + size))
			SetWindowFocus();
	}
	gui->end();
	gui->pop_style_var();
}