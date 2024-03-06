
#if !defined(FCODER_DEFAULT_BINDINGS_CPP)
#define FCODER_DEFAULT_BINDINGS_CPP

#include "4coder_default_include.cpp"

CUSTOM_ID(command_map, vim_mapid_normal);
CUSTOM_ID(command_map, vim_mapid_insert);
CUSTOM_ID(command_map, vim_mapid_visual);
CUSTOM_ID(command_map, vim_mapid_shared);
CUSTOM_ID(attachment, vim_view_attachment);

#include "generated/managed_id_metadata.cpp"
#include <stdlib.h>



struct VimState
{
    Arena                arena;
    Heap                 heap;
    Base_Allocator       alloc;
    
#if 0
    Vim_Register         registers[36];        // "a-z + "0-9
    Vim_Register         unnamed_register;     // ""
    Vim_Register         clipboard_register;   // "+ (and "* for now)
    Vim_Register         last_search_register; // "/
    Vim_Register         command_register;     // for command repetition - not exposed to the user
    Vim_Register*        active_register;
    
#endif
    u8                   most_recent_macro_register;
    b32                  recording_macro;
    b32                  played_macro;
    i64                  current_macro_start_pos;
    History_Group        macro_history;
    
    b32                  recording_command;
    b32                  playing_command;
    b32                  executing_queried_motion;
    u32                  command_flags;
    i64                  command_start_pos;
    History_Group        command_history;
    //Vim_Visual_Selection command_selection;
    
    b32                  search_show_highlight;
    u32                  search_flags;
    Scan_Direction       search_direction;
    u32                  search_mode_index;
    
    b32                  character_seek_show_highlight;
    Scan_Direction       character_seek_highlight_dir;
    u8                   most_recent_character_seek_storage[8];
    String_u8            most_recent_character_seek;
    Scan_Direction       most_recent_character_seek_dir;
    b32                  most_recent_character_seek_till;
    b32                  most_recent_character_seek_inclusive;
    
    b32                  capture_queries_into_chord_bar;
    u8                   chord_bar_storage[64];
    String_u8            chord_bar;
    
    i32                  definition_stack_count;
    i32                  definition_stack_cursor;
    Tiny_Jump            definition_stack[16];
};

global VimState global_vim;

struct VimView
{
#ifndef VIM_JUMP_HISTORY_SIZE
#define VIM_JUMP_HISTORY_SIZE 256
#endif
    Buffer_ID most_recent_known_buffer;
    i64       most_recent_known_pos;
    
    Buffer_ID previous_buffer;
    i64       pos_in_previous_buffer;
    
    b32       dont_log_this_buffer_jump;
    
    i32       jump_history_min;
    i32       jump_history_one_past_max;
    i32       jump_history_cursor;
    Tiny_Jump jump_history[VIM_JUMP_HISTORY_SIZE];
};


//#if !defined(_DEBUGGER_REMEDYBG_CPP)
//#define _DEBUGGER_REMEDYBG_CPP

function void remedybg_command_under_cursor(Application_Links *app, String_Const_u8 command) {
    Scratch_Block scratch(app);
    View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Read);
    
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
    String_Const_u8 path = push_hot_directory(app, scratch);
    
    i64 pos = view_get_cursor_pos(app, view);
    i64 line_number = get_line_number_from_pos(app, buffer, pos);
    
    if(file_name.size > 0 && path.size > 0) {
        String_Const_u8 cmd =
            push_u8_stringf(scratch, "remedybg %.*s %.*s %lld",
                            string_expand(command), string_expand(file_name), line_number);
        
        if (cmd.size > 0){
            exec_system_command(app, 0, buffer_identifier(0), path, cmd, 0);
        }
    }
}

function void remedybg_command(Application_Links *app, String_Const_u8 command) {
    Scratch_Block scratch(app);
    String_Const_u8 path = push_hot_directory(app, scratch);
    if(path.size > 0) {
        String_Const_u8 cmd = push_u8_stringf(scratch, "remedybg %.*s", string_expand(command));
        
        if (cmd.size > 0){
            exec_system_command(app, 0, buffer_identifier(0), path, cmd, 0);
        }
    }
}

CUSTOM_COMMAND_SIG(debugger_jump_to_cursor)
CUSTOM_DOC("Sends a signal to remedybg instance to jump to the file and line under the cursor.") {
    remedybg_command_under_cursor(app, string_u8_litexpr("open-file"));
}

CUSTOM_COMMAND_SIG(debugger_add_breakpoint_at_cursor)
CUSTOM_DOC("Sends a signal to remedybg instance to add a breakpoint at the line under the cursor.") {
    remedybg_command_under_cursor(app, string_u8_litexpr("add-breakpoint-at-file"));
}

CUSTOM_COMMAND_SIG(debugger_remove_breakpoint_at_cursor)
CUSTOM_DOC("Sends a signal to remedybg instance to remove a breakpoint at the line under the cursor.") {
    remedybg_command_under_cursor(app, string_u8_litexpr("remove-breakpoint-at-file"));
}

CUSTOM_COMMAND_SIG(debugger_start_debugging)
CUSTOM_DOC("Sends a signal to remedybg instance to start debugging") {
    remedybg_command(app, string_u8_litexpr("start-debugging"));
}

CUSTOM_COMMAND_SIG(debugger_stop_debugging)
CUSTOM_DOC("Sends a signal to remedybg instance to stop debugging") {
    remedybg_command(app, string_u8_litexpr("stop-debugging"));
}

CUSTOM_COMMAND_SIG(debugger_continue_execution)
CUSTOM_DOC("Sends a signal to remedybg instance to continue execution") {
    remedybg_command(app, string_u8_litexpr("continue-execution"));
}

CUSTOM_COMMAND_SIG(debugger_open_project_session)
CUSTOM_DOC("Starts remedybg with the default session (should be saved as session.rdbg along the project.4coder).") {
    Scratch_Block scratch(app);
    
    //if (current_project.loaded) {
    String_Const_u8 remedybg_session_file =
        push_u8_stringf(scratch, "F:/Programs/4coder414/4coder/session.rdbg", string_expand(current_project.dir));
    if(file_exists_and_is_file(app, remedybg_session_file))
	{
        String_Const_u8 cmd = push_u8_stringf(scratch, "remedybg %.*s", string_expand(remedybg_session_file));
        
        if (cmd.size > 0)
	    {
            exec_system_command(app, 0, buffer_identifier(0), current_project.dir, cmd, 0);
        }
    }
	else
	{
	    int foo = 42;
	    foo++;
	}
    //}
}

//#endif // _DEBUGGER_REMEDYBG_CPP


internal void
set_current_keymap(Application_Links* app, Command_Map_ID map)
{
    View_ID view_id = get_active_view(app, Access_Always);
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_Always);
    
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = map;
}


internal Color_Array insert_color;
internal Color_Array normal_color;

internal void
vim_set_cursor_colors(Application_Links *app)
{
    Arena *arena = &global_theme_arena;
    
    insert_color = make_colors(arena, 0xFF00FF00);
    normal_color = make_colors(arena, 0xFFFF0000);
}

CUSTOM_COMMAND_SIG(vim_enter_insert_mode)
{
    set_current_keymap(app, vim_mapid_insert);
    default_color_table.arrays[defcolor_cursor] = insert_color;
}

CUSTOM_COMMAND_SIG(vim_enter_normal_mode)
{
    set_current_keymap(app, vim_mapid_normal);
    default_color_table.arrays[defcolor_cursor] = normal_color;
}

#if 0
CUSTOM_COMMAND_SIG(vim_step_back_jump_history)
{
    View_ID view = get_active_view(app, Access_Always);
    Managed_Scope scope = view_get_managed_scope(app, view);
    VimView* vim_view = scope_attachment(app, scope, vim_view_attachment, VimView);
    
    b32 log_jump = false;
    if (vim_view->jump_history_cursor == vim_view->jump_history_one_past_max)
    {
        log_jump = true;
        vim_log_jump_history(app);
        vim_view->jump_history_cursor--;
    }
    
    if (vim_view->jump_history_cursor > vim_view->jump_history_min)
    {
        Tiny_Jump jump = vim_view->jump_history[(--vim_view->jump_history_cursor) % ArrayCount(vim_view->jump_history)];
        jump_to_location(app, view, jump.buffer, jump.pos);
        vim_view->dont_log_this_buffer_jump = true;
    }
}
#endif


internal void
vim_write_text_and_auto_indent_internal(Application_Links* app, String_Const_u8 text)
{
    if (text.str != 0 && text.size > 0)
    {
        b32 do_auto_indent = false;
        for (u64 i = 0; !do_auto_indent && i < text.size; i += 1)
        {
            switch (text.str[i])
            {
                case ';': case ':':
                case '{': case '}':
                case '(': case ')':
                case '[': case ']':
                case '#':
                case '\n': case '\t':
                {
                    do_auto_indent = true;
                }break;
            }
        }
        if (do_auto_indent)
        {
            View_ID view = get_active_view(app, Access_ReadWriteVisible);
            Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
            Range_i64 pos = {};
            pos.min = view_get_cursor_pos(app, view);
            write_text(app, text);
            pos.max= view_get_cursor_pos(app, view);
            auto_indent_buffer(app, buffer, pos, 0);
            move_past_lead_whitespace(app, view, buffer);
        }
        else
        {
            write_text(app, text);
        }
    }
}

CUSTOM_COMMAND_SIG(vim_replace)
{
    delete_char(app);
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_null)
{
}

CUSTOM_COMMAND_SIG(vim_insert_below)
{
    seek_end_of_textual_line(app);
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("\n"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_above)
{
    seek_beginning_of_textual_line(app);
    move_left(app);
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("\n"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_braces)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("{}"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_parentheses)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("()"));
    vim_enter_insert_mode(app);
}
CUSTOM_COMMAND_SIG(vim_insert_brackets)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("[]"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_asterisk)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("*"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_and)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("&"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_or)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("|"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_arrow)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("->"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_less)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("<"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_greater)
{
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr(">"));
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_append)
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    
    i64 start_pos = view_get_cursor_pos(app, view);
    u8 range_result = ' ';
    buffer_read_range(app, buffer, Ii64(start_pos, start_pos + 1), &range_result);
    if(range_result != '\n')
    {
        move_right(app);
    }
    
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_append_line)
{
    seek_end_of_line(app);
    vim_append(app);
}

CUSTOM_COMMAND_SIG(vim_insert_line)
{
    seek_beginning_of_line(app);
    vim_enter_insert_mode(app);
}
CUSTOM_COMMAND_SIG(vim_uncomment)
{
    seek_beginning_of_line(app);
    delete_char(app);
    delete_char(app);
}

CUSTOM_COMMAND_SIG(vim_comment)
{
    vim_insert_line(app);
    vim_write_text_and_auto_indent_internal(app, string_u8_litexpr("//"));
    vim_enter_normal_mode(app);
}


internal i64
vim_move_pos_to_non_ws(Application_Links *app, Buffer_ID buffer, i64 buffer_size, i64 start_pos, i64 dir)
{
    i64 curr_pos = 0;
    i64 result = curr_pos;
    
    if(dir == 1)
    {
        curr_pos = start_pos+1;
        for(; curr_pos < buffer_size; curr_pos += dir)
        {
            u8 range_result = ' ';
            buffer_read_range(app, buffer, Ii64(curr_pos, curr_pos + 1), &range_result);
            if(!character_is_whitespace(range_result))
            {
                result = curr_pos;
                break;
            }
        }
    }
    else if(dir == -1)
    {
        curr_pos = start_pos-1;
        if(curr_pos == -1)
        {
            result = 0;
            return result;
        }
        for(; curr_pos >= 0; curr_pos += dir)
        {
            u8 range_result = ' ';
            buffer_read_range(app, buffer, Ii64(curr_pos, curr_pos + 1), &range_result);
            if(!character_is_whitespace(range_result))
            {
                result = curr_pos;
                break;
            }
        }
    }
    
    return result;
}

internal i64
vim_move_pos_to_non_alphanumeric(Application_Links *app, Buffer_ID buffer, i64 buffer_size, i64 start_pos, i64 dir)
{
    i64 curr_pos = 0;
    i64 result = curr_pos;
    
    if (dir == 1)
    {
        curr_pos = start_pos+1;
        for (; curr_pos < buffer_size; curr_pos += dir)
        {
            u8 range_result = ' ';
            buffer_read_range(app, buffer, Ii64(curr_pos, curr_pos + 1), &range_result);
            if (!character_is_alpha_numeric(range_result))
            {
                result = curr_pos;
                break;
            }
        }
    }
    else if (dir == -1)
    {
        curr_pos = start_pos-1;
        if (curr_pos == -1)
        {
            result = 0;
        }
        for (; curr_pos >= 0; curr_pos += dir)
        {
            u8 range_result = ' ';
            buffer_read_range(app, buffer, Ii64(curr_pos, curr_pos + 1), &range_result);
            if (!character_is_alpha_numeric(range_result))
            {
                result = curr_pos;
                break;
            }
        }
    }
    
    return result;
}

internal i64
vim_seek_prev_word_pos(Application_Links *app, Buffer_ID buffer, i64 start_pos)
{
    i64 result = start_pos;
    i64 current_pos = start_pos;
    u8 current_char = buffer_get_char(app, buffer, current_pos);
    u8 previous_char = buffer_get_char(app, buffer, current_pos-1);
    i64 buffer_size = buffer_get_size(app, buffer);
    
    if(character_is_whitespace(current_char))
    {
        current_pos = vim_move_pos_to_non_ws(app, buffer, buffer_size, current_pos, -1);
        current_char = buffer_get_char(app, buffer, current_pos);
        if (!character_is_alpha_numeric(current_char))
        {
            result = current_pos;
            return result;
        }
        current_pos = vim_move_pos_to_non_alphanumeric(app, buffer, buffer_size, current_pos, -1);
        if(current_pos)
        {
            current_pos++;
        }
        result = current_pos;
    }
    else
    {
        if (!character_is_alpha_numeric(previous_char) && previous_char != '\0' && !character_is_whitespace(previous_char))
        {
            current_pos--;
            result = current_pos;
            return result;
        }
        else if (character_is_whitespace(previous_char) || previous_char == '\0') {
            current_pos = vim_move_pos_to_non_ws(app, buffer, buffer_size, current_pos, -1);
            if (!character_is_alpha_numeric(current_char) && current_char != '\0')
            {
                result = current_pos;
                return result;
            }
        }
        current_pos = vim_move_pos_to_non_alphanumeric(app, buffer, buffer_size, current_pos, -1);
        if(current_pos)
        {
            current_pos++;
        }
        result = current_pos;
    }
    
    return result;
}

internal i64
vim_seek_next_word_pos(Application_Links *app, Buffer_ID buffer, i64 start_pos)
{
    i64 result = start_pos;
    i64 current_pos = start_pos;
    u8 current_char = buffer_get_char(app, buffer, current_pos);
    u8 next_char = buffer_get_char(app, buffer, current_pos+1);
    i64 buffer_size = buffer_get_size(app, buffer);
    if (character_is_whitespace(current_char))
    {
        current_pos = vim_move_pos_to_non_ws(app, buffer, buffer_size, current_pos, 1);
        result = current_pos;
    }
    else
    {
        if (!character_is_alpha_numeric(current_char) && !character_is_whitespace(next_char) && (next_char != '\0'))
        {
            current_pos++;
            result = current_pos;
            return result;
        }
        current_pos = vim_move_pos_to_non_alphanumeric(app, buffer, buffer_size, result, 1);
        current_char = buffer_get_char(app, buffer, current_pos);
        if (character_is_whitespace(current_char))
        {
            current_pos = vim_move_pos_to_non_ws(app, buffer, buffer_size, current_pos, 1);
        }
        current_char = buffer_get_char(app, buffer, current_pos);
        if (current_char == '\0')
        {
            current_pos--;
        }
        result = current_pos;
    }
    
    return result;
}

CUSTOM_COMMAND_SIG(vim_seek_next_word)
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    i64 start_pos = view_get_cursor_pos(app, view);
    i64 end_pos = vim_seek_next_word_pos(app, buffer, start_pos);
    
    view_set_cursor(app, view, seek_pos(end_pos));
}

CUSTOM_COMMAND_SIG(vim_seek_prev_word)
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    i64 start_pos = view_get_cursor_pos(app, view);
    i64 end_pos = vim_seek_prev_word_pos(app, buffer, start_pos);
    
    view_set_cursor(app, view, seek_pos(end_pos));
}

CUSTOM_COMMAND_SIG(vim_build)
{
    internal b32 is_build_panel_open = false;
    if(!is_build_panel_open)
    {
        build_in_build_panel(app);
        is_build_panel_open = true;
        //goto_first_jump(app);
    }
    else
    {
        close_build_panel(app);
        is_build_panel_open = false;
    }
}

CUSTOM_COMMAND_SIG(vim_list_all_locations)
{
    list_all_substring_locations(app);
    view_jump_list_with_lister(app);
}

BUFFER_HOOK_SIG(jonze_begin_buffer){
    ProfileScope(app, "begin buffer");

    Scratch_Block scratch(app);

    b32 treat_as_code = false;
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);
    if (file_name.size > 0){
        String_Const_u8_Array extensions = global_config.code_exts;
        String_Const_u8 ext = string_file_extension(file_name);
        for (i32 i = 0; i < extensions.count; ++i){
            if (string_match(ext, extensions.strings[i])){

                if (string_match(ext, string_u8_litexpr("cpp")) ||
                        string_match(ext, string_u8_litexpr("h")) ||
                        string_match(ext, string_u8_litexpr("c")) ||
                        string_match(ext, string_u8_litexpr("hpp")) ||
                        string_match(ext, string_u8_litexpr("cc"))){
                    treat_as_code = true;
                }


                break;
            }
        }
    }

    Command_Map_ID map_id = vim_mapid_normal;
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = map_id;
    if(treat_as_code)
    {

        Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
        Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
        *eol_setting = setting;

        // NOTE(allen): Decide buffer settings
        b32 wrap_lines = true;
        b32 use_lexer = false;
        if (treat_as_code){
            wrap_lines = global_config.enable_code_wrapping;
            use_lexer = true;
        }

        String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
        if (string_match(buffer_name, string_u8_litexpr("*compilation*"))){
            wrap_lines = false;
        }

        if (use_lexer){
            ProfileBlock(app, "begin buffer kick off lexer");
            Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
            *lex_task_ptr = async_task_no_dep(&global_async_system, do_full_lex_async, make_data_struct(&buffer_id));
        }

        {
            b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
            *wrap_lines_ptr = wrap_lines;
        }

        if (use_lexer){
            buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
        }
        else{
            if (treat_as_code){
                buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
            }
            else{
                buffer_set_layout(app, buffer_id, layout_generic);
            }
        }

    }

    // no meaning for return
    return(0);
}


function void
jonze_set_bindings(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id)
{
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(vim_mapid_normal);
    ParentMap(global_id);
    Bind(move_up,		    KeyCode_K);
    Bind(move_up_10,		    KeyCode_K, KeyCode_Shift);
    Bind(move_down,                  KeyCode_J);
    Bind(move_down_10,                  KeyCode_J, KeyCode_Shift);
    Bind(move_line_up,		    KeyCode_K, KeyCode_Control);
    Bind(move_line_down,                  KeyCode_J, KeyCode_Control);
    Bind(move_left,                  KeyCode_H);
    Bind(move_right,                 KeyCode_L);
    Bind(vim_seek_next_word,         KeyCode_W);
    Bind(vim_seek_prev_word,         KeyCode_B);
    Bind(seek_beginning_of_line,     KeyCode_1);
    Bind(seek_end_of_line,           KeyCode_0);
    Bind(set_mark,                   KeyCode_M);
    Bind(vim_enter_insert_mode,      KeyCode_I);
    Bind(vim_insert_line,      KeyCode_I, KeyCode_Shift);
    Bind(vim_append_line,      KeyCode_A, KeyCode_Shift);
    Bind(vim_append,		    KeyCode_A);
    Bind(command_lister,             KeyCode_Minus);
    Bind(interactive_switch_buffer,  KeyCode_T);
    Bind(interactive_open_or_new,    KeyCode_O, KeyCode_Control);
    Bind(change_active_panel,	    KeyCode_S);
    Bind(swap_panels,		    KeyCode_S, KeyCode_Shift);
    Bind(vim_insert_below,	    KeyCode_O);
    Bind(vim_insert_above,	    KeyCode_O, KeyCode_Shift);
    Bind(set_mark,		    KeyCode_Space);
    Bind(delete_range,		    KeyCode_D);
    Bind(delete_line,		    KeyCode_D, KeyCode_Shift);
    Bind(delete_char,		    KeyCode_X);
    Bind(undo,			    KeyCode_U);
    Bind(redo,			    KeyCode_U, KeyCode_Shift);
    Bind(save,			    KeyCode_S, KeyCode_Control);
    Bind(close_panel,		    KeyCode_C);
    Bind(open_panel_vsplit,	    KeyCode_V);
    Bind(jump_to_function_definition,	    KeyCode_F);
    Bind(jump_to_type_definition,	    KeyCode_F, KeyCode_Shift);
    Bind(jump_to_field_definition,	    KeyCode_R, KeyCode_Shift);
    Bind(center_view,		    KeyCode_Z);
    Bind(goto_beginning_of_file,	    KeyCode_G);
    Bind(goto_end_of_file,	    KeyCode_G, KeyCode_Shift);
    Bind(vim_build, KeyCode_Comma);
    Bind(goto_first_jump, KeyCode_N);
    Bind(copy, KeyCode_Y);
    Bind(duplicate_line, KeyCode_Y, KeyCode_Shift);
    Bind(paste_and_indent, KeyCode_P);
    Bind(page_down, KeyCode_PageDown);
    Bind(page_up, KeyCode_PageUp);
    Bind(search, KeyCode_7);
    Bind(vim_list_all_locations, KeyCode_L, KeyCode_Shift);
    Bind(theme_lister, KeyCode_T, KeyCode_Control);
    Bind(view_buffer_other_panel, KeyCode_V, KeyCode_Shift);
    Bind(vim_replace, KeyCode_R);
    Bind(vim_comment, KeyCode_C, KeyCode_Control);
    Bind(vim_uncomment, KeyCode_U, KeyCode_Control);
    Bind(replace_in_all_buffers, KeyCode_R, KeyCode_Control);
    
    
    SelectMap(vim_mapid_insert);
    ParentMap(code_id);
    BindTextInput(write_text_and_auto_indent);
    Bind(vim_insert_null,	    KeyCode_Quote);
    Bind(vim_insert_braces,	    KeyCode_K, KeyCode_Shift);
    Bind(vim_insert_parentheses,	    KeyCode_J, KeyCode_Shift);
    Bind(vim_insert_brackets,	    KeyCode_L, KeyCode_Shift);
    Bind(vim_enter_normal_mode,	    KeyCode_Tick);
    Bind(vim_insert_asterisk,	    KeyCode_S, KeyCode_Quote);
    Bind(vim_insert_and,	    KeyCode_R, KeyCode_Quote);
    Bind(vim_insert_or,	    KeyCode_B, KeyCode_Quote);
    Bind(vim_insert_arrow,	    KeyCode_A, KeyCode_Quote);
    Bind(vim_insert_greater,	    KeyCode_E, KeyCode_Quote);
    Bind(vim_insert_less,	    KeyCode_V, KeyCode_Quote);
    Bind(backspace_char,		    KeyCode_Tick, KeyCode_Shift);
}

internal void
jonze_init(Application_Links* app)
{
    vim_set_cursor_colors(app);
    toggle_fullscreen(app);
}

void
custom_layer_init(Application_Links *app)
{
    Thread_Context *tctx = get_thread_context(app);
    
    // NOTE(allen): setup for default framework
    default_framework_init(app);
    
    // NOTE(allen): default hooks and command maps
    set_all_default_hooks(app);
    set_custom_hook(app, HookID_BeginBuffer, jonze_begin_buffer);
    mapping_init(tctx, &framework_mapping);
    jonze_init(app);
    
    setup_default_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
    jonze_set_bindings(&framework_mapping, mapid_global, mapid_file, mapid_code);
}

#endif //FCODER_DEFAULT_BINDINGS


