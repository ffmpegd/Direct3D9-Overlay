// Author: Sean Pesce

#include "stdafx.h"
#include "SpD3D9OTextFeed.h"



SpD3D9OTextFeed::SpD3D9OTextFeed(SpD3D9Overlay *new_overlay)
{
    overlay = new_overlay;

    // Initialize text colors
    for (int i = 0; i < _SP_D3D9O_TEXT_COLOR_COUNT_; i++)
    {
        colors[i] = SP_D3D9O_TEXT_COLORS[i];
    }
    shadow_color = _SP_D3D9O_TF_DEFAULT_SHADOW_COLOR_;
    outline_color = _SP_D3D9O_TF_DEFAULT_OUTLINE_COLOR_;


    // Initialize font
#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
    HRESULT font_hr = D3DXCreateFont(
        overlay->device->m_pIDirect3DDevice9, // D3D device
        _SP_D3D9O_TF_DEFAULT_FONT_HEIGHT_, // Height
        0,						// Width
        FW_BOLD,				// Weight
        1,						// MipLevels, 0 = autogen mipmaps
        FALSE,					// Italic
        DEFAULT_CHARSET,		// CharSet
        OUT_DEFAULT_PRECIS,		// OutputPrecision
        ANTIALIASED_QUALITY,	// Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        _SP_D3D9O_TF_DEFAULT_FONT_FAMILY_, // pFaceName
        &font);					// ppFont
    _SP_D3D9_CHECK_FAILED_(font_hr);
#else
    font = new CD3DFont(_SP_D3D9O_TF_DEFAULT_FONT_FAMILY_, font_height, D3DFONT_BOLD);
    font->InitializeDeviceObjects(overlay->device->m_pIDirect3DDevice9);
    font->RestoreDeviceObjects();
#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_

    // Initialize text feed boundaries
    RECT window_rect;
    if (GetClientRect(*overlay->game_window, &window_rect))
    {
        // Handle error
    }
    set_bounds(&window_rect);
}



SpD3D9OTextFeed::~SpD3D9OTextFeed()
{
#ifndef _SP_D3D9O_TF_USE_ID3DX_FONT_
    if (font != NULL)
    {
        delete font;
        font = NULL;
    }
#endif
}



bool SpD3D9OTextFeed::is_enabled()
{
    return (overlay->enabled_elements & SP_D3D9O_TEXT_FEED_ENABLED);
}

void SpD3D9OTextFeed::set_enabled(bool enabled)
{
    if (enabled)
    {
        overlay->enabled_elements |= SP_D3D9O_TEXT_FEED_ENABLED; // Enable text feed
    }
    else
    {
        overlay->enabled_elements &= SP_D3D9O_TEXT_FEED_DISABLED; // Disable text feed
    }
}

void SpD3D9OTextFeed::enable()
{
    overlay->enabled_elements |= SP_D3D9O_TEXT_FEED_ENABLED;
}

void SpD3D9OTextFeed::disable()
{
    overlay->enabled_elements &= SP_D3D9O_TEXT_FEED_DISABLED;
}



void SpD3D9OTextFeed::set_bounds(RECT *window_boundaries)
{
    int vertical_offset = 0;

    if (overlay->console != NULL && overlay->console->is_open())
    {
        SIZE console_char_size;
        overlay->console->font->GetTextExtent("|", &console_char_size);
        vertical_offset += ((int)overlay->console->border_width * 2) + (((int)overlay->console->output_log_displayed_lines + 1) * (int)console_char_size.cy);
    }
#ifdef _SP_DARK_SOULS_1_
    else
    {
        // Add DSPW overlay offset (only if console is not open)
        extern int user_pref_dspw_ol_offset; // Used to adjust the overlay to avoid clipping with the DSPW overlay
        vertical_offset += user_pref_dspw_ol_offset;
    }
#endif // _SP_DARK_SOULS_1_

    // Initialize plain text rect
    SetRect(&bounds.plain,
            window_boundaries->left,
            window_boundaries->top + vertical_offset,
            window_boundaries->right,
            window_boundaries->bottom);

    // Inititialize main shadowed text rect
    if (shadow_x_offset >= 0 && shadow_y_offset >= 0)
    {
        // Case: x and y offsets are both positive
        SetRect(&bounds.shadowed[0],
                window_boundaries->left,
                window_boundaries->top + vertical_offset,
                window_boundaries->right - shadow_x_offset,
                window_boundaries->bottom - shadow_y_offset);
    }
    else if (shadow_x_offset <= 0 && shadow_y_offset >= 0)
    {
        // Case: x offset is negative; y offset is positive
        SetRect(&bounds.shadowed[0],
                window_boundaries->left - shadow_x_offset,
                window_boundaries->top,
                window_boundaries->right,
                window_boundaries->bottom - shadow_y_offset);
    }
    else if (shadow_x_offset >= 0 && shadow_y_offset <= 0)
    {
        // Case: x offset is positive; y offset is negative
        SetRect(&bounds.shadowed[0],
                window_boundaries->left,
                window_boundaries->top - shadow_y_offset,
                window_boundaries->right - shadow_x_offset,
                window_boundaries->bottom);
    }
    else
    {
        // Case: x and y offsets are both negative
        SetRect(&bounds.shadowed[0],
                window_boundaries->left - shadow_x_offset,
                window_boundaries->top - shadow_y_offset,
                window_boundaries->right,
                window_boundaries->bottom);
    }


    // Initialize text shadow rect
    SetRect(&bounds.shadowed[1],
            bounds.shadowed[0].left + shadow_x_offset,
            bounds.shadowed[0].top + shadow_y_offset,
            bounds.shadowed[0].right + shadow_x_offset,
            bounds.shadowed[0].bottom + shadow_y_offset);


    // Inititialize main outlined text rect
    SetRect(&bounds.outlined[0],
            window_boundaries->left + outline_thickness,
            window_boundaries->top + outline_thickness + vertical_offset,
            window_boundaries->right - outline_thickness,
            window_boundaries->bottom - outline_thickness);

    // Initialize text outline rects:
    //		Top left outline extrusion
    SetRect(&bounds.outlined[1],
            bounds.outlined[0].left - outline_thickness,
            bounds.outlined[0].top - outline_thickness,
            bounds.outlined[0].right - outline_thickness,
            bounds.outlined[0].bottom - outline_thickness);
    //		Top right outline extrusion
    SetRect(&bounds.outlined[2],
            bounds.outlined[0].left + outline_thickness,
            bounds.outlined[0].top - outline_thickness,
            bounds.outlined[0].right + outline_thickness,
            bounds.outlined[0].bottom - outline_thickness);
    //		Bottom left outline extrusion
    SetRect(&bounds.outlined[3],
            bounds.outlined[0].left - outline_thickness,
            bounds.outlined[0].top + outline_thickness,
            bounds.outlined[0].right - outline_thickness,
            bounds.outlined[0].bottom + outline_thickness);
    //		Bottom right outline extrusion
    SetRect(&bounds.outlined[4],
            bounds.outlined[0].left + outline_thickness,
            bounds.outlined[0].top + outline_thickness,
            bounds.outlined[0].right + outline_thickness,
            bounds.outlined[0].bottom + outline_thickness);
    //		Left-side outline extrusion
    SetRect(&bounds.outlined[5],
            bounds.outlined[0].left - outline_thickness,
            bounds.outlined[0].top,
            bounds.outlined[0].right - outline_thickness,
            bounds.outlined[0].bottom);
    //		Upward outline extrusion
    SetRect(&bounds.outlined[6],
            bounds.outlined[0].left,
            bounds.outlined[0].top - outline_thickness,
            bounds.outlined[0].right,
            bounds.outlined[0].bottom - outline_thickness);
    //		Right-side outline extrusion
    SetRect(&bounds.outlined[7],
            bounds.outlined[0].left + outline_thickness,
            bounds.outlined[0].top,
            bounds.outlined[0].right + outline_thickness,
            bounds.outlined[0].bottom);
    //		Downward outline extrusion
    SetRect(&bounds.outlined[8],
            bounds.outlined[0].left,
            bounds.outlined[0].top + outline_thickness,
            bounds.outlined[0].right,
            bounds.outlined[0].bottom + outline_thickness);
}



void SpD3D9OTextFeed::set_font_height(unsigned int new_text_height)
{
    new_font_size = new_text_height;
}



void SpD3D9OTextFeed::set_title(const char *new_title)
{
    new_info_title.clear();
    new_info_title.append(new_title);

    if (new_info_title.empty())
    {
        new_info_title.append(" ");
    }
}


void SpD3D9OTextFeed::get_title(std::string *title_buff)
{
    title_buff->clear();
    title_buff->append(info_title);
}



void SpD3D9OTextFeed::update_font_height()
{
#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_

    // Store the current font attributes
    D3DXFONT_DESC font_desc;
    HRESULT font_desc_hr = font->GetDesc(&font_desc);
    _SP_D3D9_CHECK_FAILED_(font_desc_hr);

    // Release pre-existing font resources
    if (font != NULL)
    {
        font->Release(); // Decrement reference count for ID3DXFont interface
        font = NULL;
    }

    // Check that the new font height is valid
    if (new_font_size > 0)
    {

        // Re-initialize overlay font with identical attributes other than the new font height
        HRESULT font_hr = D3DXCreateFont(
            overlay->device->m_pIDirect3DDevice9, // D3D device
            new_font_size,			// Height
            font_desc.Width,		// Width
            font_desc.Weight,		// Weight
            font_desc.MipLevels,	// MipLevels; 0 = autogen mipmaps
            font_desc.Italic,		// Italic
            font_desc.CharSet,		// CharSet
            font_desc.OutputPrecision, // OutputPrecision
            font_desc.Quality,		// Quality
            font_desc.PitchAndFamily, // PitchAndFamily
            font_desc.FaceName,		// pFaceName
            &font);					// ppFont

        _SP_D3D9_CHECK_FAILED_(font_hr);
    }
    else
    {
        // Re-initialize overlay font with identical attributes
        HRESULT font_hr = D3DXCreateFont(
            overlay->device->m_pIDirect3DDevice9, // D3D device
            font_desc.Height,		// Height
            font_desc.Width,		// Width
            font_desc.Weight,		// Weight
            font_desc.MipLevels,	// MipLevels; 0 = autogen mipmaps
            font_desc.Italic,		// Italic
            font_desc.CharSet,		// CharSet
            font_desc.OutputPrecision, // OutputPrecision
            font_desc.Quality,		// Quality
            font_desc.PitchAndFamily, // PitchAndFamily
            font_desc.FaceName,		// pFaceName
            &font);					// ppFont

        _SP_D3D9_CHECK_FAILED_(font_hr);
    }

#else

    if (font != NULL)
    {
        delete font;
        font = NULL;
    }

    font = new CD3DFont(_SP_D3D9O_TF_DEFAULT_FONT_FAMILY_, new_font_size, D3DFONT_BOLD);

    font->InitializeDeviceObjects(overlay->device->m_pIDirect3DDevice9);
    font->RestoreDeviceObjects();

    font_height = new_font_size;

#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_

    new_font_size = 0;
}


#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
void SpD3D9OTextFeed::cycle_color()
{
    if (current_rgb_cycle_vals[0] == 0x00FF0000 && current_rgb_cycle_vals[1] != 0x0000FF00 && current_rgb_cycle_vals[2] == 0x00000000)
    {
        current_rgb_cycle_vals[1] += 0x00000100;
        if (current_rgb_cycle_vals[1] == 0x0000FF00)
        {
            current_rgb_cycle_vals[0] = 0x00FE0000;
        }
    }
    else if (current_rgb_cycle_vals[0] != 0x00FF0000 && current_rgb_cycle_vals[1] == 0x0000FF00 && current_rgb_cycle_vals[2] == 0x00000000)
    {
        current_rgb_cycle_vals[0] -= 0x00010000;
        if (current_rgb_cycle_vals[0] == 0x00000000)
        {
            current_rgb_cycle_vals[2] = 0x00000001;
        }
    }
    else if (current_rgb_cycle_vals[0] == 0x00000000 && current_rgb_cycle_vals[1] == 0x0000FF00 && current_rgb_cycle_vals[2] != 0x000000FF)
    {
        current_rgb_cycle_vals[2]++;
        if (current_rgb_cycle_vals[2] == 0x000000FF)
        {
            current_rgb_cycle_vals[1] = 0x0000FE00;
        }
    }
    else if (current_rgb_cycle_vals[0] == 0x00000000 && current_rgb_cycle_vals[1] != 0x0000FF00 && current_rgb_cycle_vals[2] == 0x000000FF)
    {
        current_rgb_cycle_vals[1] -= 0x00000100;
        if (current_rgb_cycle_vals[1] == 0x00000000)
        {
            current_rgb_cycle_vals[0] = 0x00010000;
        }
    }
    else if (current_rgb_cycle_vals[0] != 0x00000000 && current_rgb_cycle_vals[1] == 0x00000000 && current_rgb_cycle_vals[2] == 0x000000FF)
    {
        current_rgb_cycle_vals[0] += 0x00010000;
        if (current_rgb_cycle_vals[0] == 0x00FF0000)
        {
            current_rgb_cycle_vals[2] = 0x000000FE;
        }
    }
    else if (current_rgb_cycle_vals[0] == 0x00FF0000 && current_rgb_cycle_vals[1] == 0x00000000 && current_rgb_cycle_vals[2] != 0x00000000)
    {
        current_rgb_cycle_vals[2]--;
    }
    colors[SP_D3D9O_TEXT_COLOR_CYCLE_ALL] = D3DXCOLOR(0xFF000000 + current_rgb_cycle_vals[0] + current_rgb_cycle_vals[1] + current_rgb_cycle_vals[2]);
}
#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_


void SpD3D9OTextFeed::update_info_header()
{
    info_string.clear();

    if ((show_info_bar & SP_D3D9O_INFO_BAR_DATE)
        || (show_info_bar & SP_D3D9O_INFO_BAR_TIME))
    {
        info_string.append("[");
    }

    if (show_info_bar & SP_D3D9O_INFO_BAR_DATE)
    {
        // Insert current date to text feed info string
        append_current_date_string(&info_string, false, SP_DATE_MMDDYYYY);
        if (show_info_bar & SP_D3D9O_INFO_BAR_TIME)
        {
            info_string.append("  ");
        }
        else
        {
            info_string.append("]  ");
        }
    }

    if (show_info_bar & SP_D3D9O_INFO_BAR_TIME)
    {
        // Append current timestamp to text feed info string
        append_current_timestamp_string(&info_string, false);
        info_string.append("]  ");
    }

    if (show_info_bar & SP_D3D9O_INFO_BAR_FPS)
    {
        // Insert FPS counter into text feed info string
        info_string.append("[");

        int current_fps = overlay->fps_count; // Get FPS count

        if (current_fps < 999)
        {
            info_string.append(std::to_string(current_fps));
        }
        else
        {
            info_string.append("999");
        }

        info_string.append(" FPS]  ");
    }

    if (show_info_bar & SP_D3D9O_INFO_BAR_FRAME_COUNT)
    {
        // Insert global frame count into text feed info string
        info_string.append("[Frame count: ");
        unsigned long long current_frame_count = overlay->frame_count; // Get FPS count
        info_string.append(std::to_string(current_frame_count));
        info_string.append("]  ");
    }


    // Add live info bar elements from plugins
    info_string.append(info_bar_plugin_elements);


    if (show_info_bar & SP_D3D9O_INFO_BAR_TITLE)
    {
        // Insert title into text feed info string
        info_string.append(info_title.c_str());
    }
}


void SpD3D9OTextFeed::update_title()
{
    info_title.clear();
    info_title.append(new_info_title.c_str());
    new_info_title.clear();
}


#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
void SpD3D9OTextFeed::build_feed()
{
    // Iterate through overlay text feed message list for each color
    std::list<SP_D3D9O_TEXT_FEED_ENTRY>::const_iterator iterator;
    for (iterator = messages.begin(); iterator != messages.end(); iterator++)
    {
        if (iterator == messages.begin())
        {
            feed_string_full.clear(); // Erase text feed shadow/outline strings from last-rendered frame
            if (show_info_bar)
            {
                feed_string[0].clear();
                update_info_header();
                feed_string[0].append(info_string).append("\n");
                feed_string_full.append(info_string).append("\n");
                for (int c = 1; c < _SP_D3D9O_TEXT_COLOR_COUNT_; c++)
                {
                    feed_string[c].clear(); // Erase strings from last-rendered frame
                    feed_string[c].append(" \r\n");
                }
            }
            else
            {
                for (int c = 0; c < _SP_D3D9O_TEXT_COLOR_COUNT_; c++)
                {
                    feed_string[c].clear(); // Erase strings from last-rendered frame
                }
            }
        }

        for (int c = 0; c < _SP_D3D9O_TEXT_COLOR_COUNT_; c++)
        {
            if (c == (*iterator).color)
            {
                // Construct line for the specified color
                feed_string[c].append((*iterator).message).append("\r\n");
                feed_string_full.append((*iterator).message).append("\r\n");
            }
            else
            {
                // Create a blank line for all other colors
                feed_string[c].append(" \r\n");
            }
        }
    }
}
#else

void SpD3D9OTextFeed::build_feed()
{
    feed_string_full.clear();

    if (show_info_bar)
    {
        update_info_header();
        feed_string_full += '^';
        feed_string_full += SP_D3D9O_TEXT_COLOR_TABLE_CHARS[_SP_D3D9O_TF_DEFAULT_COLOR_];
        feed_string_full.append(info_string).append("\n");
    }

    std::list<SP_D3D9O_TEXT_FEED_ENTRY>::const_iterator iterator;
    for (iterator = messages.begin(); iterator != messages.end(); iterator++)
    {
        feed_string_full += '^';
        feed_string_full += SP_D3D9O_TEXT_COLOR_TABLE_CHARS[iterator->color];
        feed_string_full.append(iterator->message).append("\n");
    }
}

void SpD3D9OTextFeed::update_position()
{
    coordinates.flags = 0;

    int message_count = 0;
    if (show_info_bar)
    {
        message_count++;
    }

    switch (style) {
        case SP_D3D9O_SHADOWED_TEXT:
            // Set text feed X position
            if (position & DT_CENTER)
            {
                coordinates.shadowed[0].x = (float)bounds.shadowed[0].right / 2.0f;
                coordinates.shadowed[1].x = (float)bounds.shadowed[1].right / 2.0f;
                coordinates.flags |= D3DFONT_CENTERED;
            }
            else if (position & DT_RIGHT)
            {
                coordinates.shadowed[0].x = (float)bounds.shadowed[0].right;
                coordinates.shadowed[1].x = (float)bounds.shadowed[1].right;
                coordinates.flags |= D3DFONT_RIGHT;
            }
            else
            {
                coordinates.shadowed[0].x = (float)bounds.shadowed[0].left;
                coordinates.shadowed[1].x = (float)bounds.shadowed[1].left;
            }
            // Set text feed Y position
            if (position & DT_VCENTER)
            {
                coordinates.shadowed[0].y = ((float)bounds.shadowed[0].bottom / 2.0f) - (((messages.size() + message_count) * (font_height * 1.5f)) / 2.0f);
                coordinates.shadowed[1].y = ((float)bounds.shadowed[1].bottom / 2.0f) - (((messages.size() + message_count) * (font_height * 1.5f)) / 2.0f);
            }
            else if (position & DT_BOTTOM)
            {
                coordinates.shadowed[0].y = (float)bounds.shadowed[0].bottom - ((messages.size() + message_count) * (font_height * 1.5f));
                coordinates.shadowed[1].y = (float)bounds.shadowed[1].bottom - ((messages.size() + message_count) * (font_height * 1.5f));
            }
            else
            {
                coordinates.shadowed[0].y = (float)bounds.shadowed[0].top;
                coordinates.shadowed[1].y = (float)bounds.shadowed[1].top;
            }
            break;

        case SP_D3D9O_PLAIN_TEXT:
            // Set text feed X position
            if (position & DT_CENTER)
            {
                coordinates.plain.x = (float)bounds.plain.right / 2.0f;
                coordinates.flags |= D3DFONT_CENTERED;
            }
            else if (position & DT_RIGHT)
            {
                coordinates.plain.x = (float)bounds.plain.right;
                coordinates.flags |= D3DFONT_RIGHT;
            }
            else
            {
                coordinates.plain.x = (float)bounds.plain.left;
            }
            // Set text feed Y position
            if (position & DT_VCENTER)
            {
                coordinates.plain.y = ((float)bounds.plain.bottom / 2.0f) - (((messages.size() + message_count) * (font_height * 1.5f)) / 2.0f);
            }
            else if (position & DT_BOTTOM)
            {
                coordinates.plain.y = (float)bounds.plain.bottom - ((messages.size() + message_count) * (font_height * 1.5f));
            }
            else
            {
                coordinates.plain.y = (float)bounds.plain.top;
            }
            break;

        case SP_D3D9O_OUTLINED_TEXT:
        default:
            for (int i = 0; i < 9; i++)
            {
                // Set text feed X position
                if (position & DT_CENTER)
                {
                    coordinates.outlined[i].x = (float)bounds.outlined[i].right / 2.0f;
                    coordinates.flags |= D3DFONT_CENTERED;
                }
                else if (position & DT_RIGHT)
                {
                    coordinates.outlined[i].x = (float)bounds.outlined[i].right;
                    coordinates.flags |= D3DFONT_RIGHT;
                }
                else
                {
                    coordinates.outlined[i].x = (float)bounds.outlined[i].left;
                }
                // Set text feed Y position
                if (position & DT_VCENTER)
                {
                    coordinates.outlined[i].y = ((float)bounds.outlined[i].bottom / 2.0f) - (((messages.size() + message_count) * (font_height * 1.5f)) / 2.0f);
                }
                else if (position & DT_BOTTOM)
                {
                    coordinates.outlined[i].y = (float)bounds.outlined[i].bottom - ((messages.size() + message_count) * (font_height * 1.5f));
                }
                else
                {
                    coordinates.outlined[i].y = (float)bounds.outlined[i].top;
                }
            }
            break;
    }
}

#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_


void SpD3D9OTextFeed::clean_feed()
{
    // Get current time (in milliseconds since epoch)
    unsigned long long ms_since_epoch = std::chrono::system_clock::now().time_since_epoch() /
        std::chrono::milliseconds(1);

    // Iterate through overlay text feed message list
    std::list<SP_D3D9O_TEXT_FEED_ENTRY>::const_iterator iterator = messages.begin();
    while (iterator != messages.end())
    {
        if ((*iterator).expire_time != 0 && ms_since_epoch >= (*iterator).expire_time)
        {
            // Remove expired message
            iterator = messages.erase(iterator);
        }
        else
        {
            iterator++;
        }
    }
}



void SpD3D9OTextFeed::print(const char *message, unsigned long long duration, bool include_timestamp, SP_D3D9O_TEXT_COLOR_ENUM text_color)
{
    // Get current time (in milliseconds since epoch)
    unsigned long long ms_since_epoch = std::chrono::system_clock::now().time_since_epoch() /
        std::chrono::milliseconds(1);

    // Create new overlay text feed message data structure
    SP_D3D9O_TEXT_FEED_ENTRY new_message;

    // Calculate the expiration time for the message
    if (duration == 0)
    {
        // Messages with an expire time of 0 never expire
        new_message.expire_time = 0;
    }
    else
    {
        new_message.expire_time = (ms_since_epoch + duration);
    }

    // Set the text color for the message
    if (text_color < _SP_D3D9O_TEXT_COLOR_COUNT_ && text_color >= 0)
    {
        new_message.color = text_color;
    }
    else
    {
        // Invalid color specified, set to default color
        new_message.color = _SP_D3D9O_TF_DEFAULT_COLOR_;
    }

    // Add timestamp to message
    if (include_timestamp)
    {
        // Build timestamp string
        if (get_current_timestamp_string(&new_message.message, true))
        {
            // Handle error
        }

        new_message.message.append(" ");
    }

    // Store the new message in the text feed message structure
    new_message.message.append(message);


    // Also print to the console
    if (is_enabled() && !overlay->console->is_open())
    {
        overlay->console->print(std::string("[TEXT FEED] ").append(new_message.message).c_str());
    }
    else
    {
        overlay->console->print(message);
    }


    // Add the constructed message to the overlay text feed message queue
    messages.push_back(new_message);
}



void SpD3D9OTextFeed::draw()
{

    if (new_font_size)
    {
        update_font_height();
    }

    if (!new_info_title.empty())
    {
        update_title();
    }

#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_
    cycle_color(); // Calculate the next ARGB color value for text whose color cycles through all colors
#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_

    build_feed(); // Build text feed string

#ifdef _SP_D3D9O_TF_USE_ID3DX_FONT_

    switch (style) {
        case SP_D3D9O_SHADOWED_TEXT:
            font->DrawText(NULL, feed_string_full.c_str(), -1, &bounds.shadowed[1], position, shadow_color); // Draw text shadow
            for (int c = 0; c < _SP_D3D9O_TEXT_COLOR_COUNT_; c++)
            {
                font->DrawText(NULL, feed_string[c].c_str(), -1, &bounds.shadowed[0], position, colors[c]); // Draw text
            }
            break;
        case SP_D3D9O_PLAIN_TEXT:
            for (int c = 0; c < _SP_D3D9O_TEXT_COLOR_COUNT_; c++)
            {
                font->DrawText(NULL, feed_string[c].c_str(), -1, &bounds.plain, position, colors[c]); // Draw text
            }
            break;
        case SP_D3D9O_OUTLINED_TEXT:
        default:
            // Draw outlined text
            for (int o = 1; o <= 8; o++) // 8 = number of boundary rects for outlined text style
            {
                font->DrawText(NULL, feed_string_full.c_str(), -1, &bounds.outlined[o], position, outline_color); // Draw text outline
            }
            for (int c = 0; c < _SP_D3D9O_TEXT_COLOR_COUNT_; c++)
            {
                font->DrawText(NULL, feed_string[c].c_str(), -1, &bounds.outlined[0], position, colors[c]); // Draw text
            }
            break;
    }

#else

    update_position();

    font->BeginDrawing();

    switch (style) {
        case SP_D3D9O_SHADOWED_TEXT:
            font->DrawText(coordinates.shadowed[1].x, coordinates.shadowed[1].y, shadow_color, feed_string_full.c_str(), coordinates.flags, 0); // Draw text shadow
            font->DrawText(coordinates.shadowed[0].x, coordinates.shadowed[0].y, colors[_SP_D3D9O_TF_DEFAULT_COLOR_], feed_string_full.c_str(), (coordinates.flags | D3DFONT_COLORTABLE), 0); // Draw text
            break;
        case SP_D3D9O_PLAIN_TEXT:
            font->DrawText(coordinates.plain.x, coordinates.plain.y, colors[_SP_D3D9O_TF_DEFAULT_COLOR_], feed_string_full.c_str(), (coordinates.flags | D3DFONT_COLORTABLE), 0); // Draw text
            break;
        case SP_D3D9O_OUTLINED_TEXT:
        default:
            for (int o = 1; o <= 8; o++) // 8 = number of boundary rects for outlined text style
            {
                font->DrawText(coordinates.outlined[o].x, coordinates.outlined[o].y, outline_color, feed_string_full.c_str(), coordinates.flags, 0); // Draw text outline
            }
            font->DrawText(coordinates.outlined[0].x, coordinates.outlined[0].y, colors[_SP_D3D9O_TF_DEFAULT_COLOR_], feed_string_full.c_str(), (coordinates.flags | D3DFONT_COLORTABLE), 0); // Draw text
            break;
    }

    font->EndDrawing();

#endif // _SP_D3D9O_TF_USE_ID3DX_FONT_

}

