#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "4coder_default_include.cpp"

class foo_bar
{
    int x;
};

enum vim_mapid
{
    VIM_MAPID_NORMAL = default_maps_count,
    VIM_MAPID_INSERT,
    VIM_MAPID_VISUAL,
    VIM_MAPID_SHARED,
};

#define VIM_HOOK_CURR_CUSTOM_CMD(func) global_vim_state.curr_custom_cmd = func
struct vim_state_t
{
    void (*curr_custom_cmd)(Application_Links *app);
};
static vim_state_t global_vim_state;

static void
set_current_keymap( Application_Links* app, int map )
{
    unsigned int access = AccessAll;
    View_Summary view = get_active_view( app, access );
    Buffer_Summary buffer = get_buffer( app, view.buffer_id, access );
    
    if ( buffer.exists )
    {
        buffer_set_setting( app, &buffer, BufferSetting_MapID, map );
    }
}

static int32_t
vim_move_pos_to_non_ws(Stream_Chunk *stream, int32_t pos, int32_t dir)
{
    int32_t curr = 0;
    if (dir == 1) {
        curr = pos+1;
        bool32 still_looping = true;
        do{
            for (; curr < stream->end; curr += dir){
                if (!char_is_whitespace(stream->data[curr])){
                    return curr;
                }
            }
            still_looping = forward_stream_chunk(stream);
        }while(still_looping);
    } else if (dir == -1) {
        curr = pos-1;
        if (curr == -1) {
            return 0;
        }
        bool32 still_looping = true;
        do{
            for (; curr >= stream->start; curr += dir){
                if (!char_is_whitespace(stream->data[curr])){
                    return curr;
                }
            }
            still_looping = backward_stream_chunk(stream);
        }while(still_looping);
    }
    
    return curr;
}

//if (!char_is_alpha_numeric(stream->data[curr])){
static int32_t
vim_move_pos_to_non_alphanumeric(Stream_Chunk *stream, int32_t pos, int32_t dir)
{
    int32_t curr = 0;
    if (dir == 1) {
        curr = pos+1;
        bool32 still_looping = true;
        do{
            for (; curr < stream->end; curr += dir){
                if (!char_is_alpha_numeric(stream->data[curr])){
                    return curr;
                }
            }
            still_looping = forward_stream_chunk(stream);
        }while(still_looping);
    } else if (dir == -1) {
        curr = pos-1;
        if (curr == -1) {
            return 0;
        }
        bool32 still_looping = true;
        do{
            for (; curr >= stream->start; curr += dir){
                if (!char_is_alpha_numeric(stream->data[curr])){
                    return curr;
                }
            }
            still_looping = backward_stream_chunk(stream);
        }while(still_looping);
    }
    
    return curr;
}

// TODO: double direction fix!!!!!
static int32_t
vim_move_pos_to_alphanumeric(Stream_Chunk *stream, int32_t pos, int32_t dir)
{
    int32_t curr = pos+1;
    bool32 still_looping = true;
    
    do{
        for (; curr < stream->end; curr += dir){
            if (!char_is_alpha_numeric(stream->data[curr])){
                return curr;
            }
        }
        still_looping = forward_stream_chunk(stream);
    }while(still_looping);
    
    return curr;
}

static int32_t
vim_seek_next_word_pos(Application_Links *app, Buffer_Summary *buffer, int32_t pos){
    char data_chunk[1024];
    Stream_Chunk stream = {0};
    
    if (init_stream_chunk(&stream, app, buffer, pos, data_chunk, sizeof(data_chunk))) {
        if (char_is_whitespace(stream.data[pos])) {
            pos = vim_move_pos_to_non_ws(&stream, pos, 1);
        } else {
            if (!char_is_alpha_numeric(stream.data[pos]) && !char_is_whitespace(stream.data[pos+1]) && stream.data[pos+1] != '\0') {
                pos++;
                return pos;
            }
            pos = vim_move_pos_to_non_alphanumeric(&stream, pos, 1);
            if (char_is_whitespace(stream.data[pos])) {
                pos = vim_move_pos_to_non_ws(&stream, pos, 1);
            }
            if (stream.data[pos] == '\0') {
                pos--;
            }
        }
    }
    
    return(pos);
}


// TODO: vim fix weird bug
static int32_t
vim_seek_prev_word_pos(Application_Links *app, Buffer_Summary *buffer, int32_t pos){
    char data_chunk[1024];
    Stream_Chunk stream = {0};
    
    if (init_stream_chunk(&stream, app, buffer, pos, data_chunk, sizeof(data_chunk))) {
        if (char_is_whitespace(stream.data[pos])) {
            pos = vim_move_pos_to_non_ws(&stream, pos, -1);
            if (!char_is_alpha_numeric(stream.data[pos])) {
                return pos;
            }
            pos = vim_move_pos_to_non_alphanumeric(&stream, pos, -1);
            pos++;
        } else {
            if (!char_is_alpha_numeric(stream.data[pos-1]) && stream.data[pos-1] != '\0' && !char_is_whitespace(stream.data[pos-1])) {
                pos--;
                return pos;
            } else if (char_is_whitespace(stream.data[pos-1]) || stream.data[pos-1] == '\0') {
                pos = vim_move_pos_to_non_ws(&stream, pos, -1);
                if (!char_is_alpha_numeric(stream.data[pos]) && stream.data[pos] != '\0') {
                    return pos;
                }
            }
            pos = vim_move_pos_to_non_alphanumeric(&stream, pos, -1);
            pos++;
        }
    }
    
    return pos;
}

// asdf asd ad"s d d
static int32_t
vim_seek_end_word_pos(Application_Links *app, Buffer_Summary *buffer, int32_t pos) {
    char data_chunk[1024];
    Stream_Chunk stream = {0};
    
    if (init_stream_chunk(&stream, app, buffer, pos, data_chunk, sizeof(data_chunk))) {
        if (char_is_whitespace(stream.data[pos])) {
            pos = vim_move_pos_to_non_ws(&stream, pos, 1);
            pos = vim_move_pos_to_non_alphanumeric(&stream, pos, 1);
            pos--;
        } else {
            if (char_is_whitespace(stream.data[pos+1])) {
                pos = vim_move_pos_to_non_ws(&stream, pos, 1);
            } else if (!char_is_alpha_numeric(stream.data[pos+1]) && stream.data[pos+1] != '\0') {
                pos++;
                return pos;
            }
            pos = vim_move_pos_to_non_alphanumeric(&stream, pos, 1);
            pos--;
        }
    }
    
    return pos;
}

static int
buffer_seek_nonalphanumeric_right(Application_Links* app, Buffer_Summary* buffer, int pos) {
    char chunk[1024];
    int chunk_size = sizeof(chunk);
    Stream_Chunk stream = {};
    
    if (init_stream_chunk(&stream, app, buffer, pos, chunk, chunk_size)) {
        char cursorch = stream.data[pos];
        char nextch = cursorch;
        int still_looping = true;
        do {
            for (; pos < stream.end; ++pos) {
                // Three kinds of characters:
                //  - word characters, first of a row results in a stop
                //  - symbol characters, first of a row results in a stop
                //  - whitespace characters, always skip
                //  The distinction between the first two is only needed
                //   because word and symbol characters do not form a "row"
                //   when intermixed.
                nextch = stream.data[pos];
                if (!char_is_alpha_numeric(nextch)) {
                    return pos;
                }
            }
            still_looping = forward_stream_chunk(&stream);
        } while (still_looping);
        
        if (pos > buffer->size) {
            pos = buffer->size;
        }
    }
    
    return pos;
}

static int
buffer_seek_nonalphanumeric_left(Application_Links* app, Buffer_Summary* buffer, int pos) {
    char chunk[1024];
    int chunk_size = sizeof(chunk);
    Stream_Chunk stream = {};
    
    if (init_stream_chunk(&stream, app, buffer, pos, chunk, chunk_size)) {
        char cursorch = stream.data[pos];
        char nextch = cursorch;
        int still_looping = true;
        do {
            for (; pos >= stream.start; --pos) {
                // Three kinds of characters:
                //  - word characters, first of a row results in a stop
                //  - symbol characters, first of a row results in a stop
                //  - whitespace characters, always skip
                //  The distinction between the first two is only needed
                //   because word and symbol characters do not form a "row"
                //   when intermixed.
                nextch = stream.data[pos];
                if (!char_is_alpha_numeric(nextch)) {
                    return pos;
                }
            }
            still_looping = backward_stream_chunk(&stream);
        } while (still_looping);
        
        if (pos > buffer->size) {
            pos = buffer->size;
        }
    }
    
    return pos;
}

static Range get_word_under_cursor(struct Application_Links* app, Buffer_Summary* buffer, View_Summary* view)
{
    int pos, start, end;
    pos = view->cursor.pos;
    start = buffer_seek_nonalphanumeric_right(app, buffer, pos);
    end = buffer_seek_nonalphanumeric_left(app, buffer, pos);
    
    return make_range(start, end);
}

CUSTOM_COMMAND_SIG(vim_enter_insert_mode)
{
    set_current_keymap(app, VIM_MAPID_INSERT);
    
    Theme_Color colors[ ] =
    {
        {Stag_Cursor, 0X00FF00},
        {Stag_At_Cursor, 0x00FFFF},
        {Stag_Mark, 0xFF6F1A},
        {Stag_Margin, 0x33170B},
        {Stag_Margin_Active, 0x934420},
        {Stag_Bar_Active, 0x934420},
        {Stag_Bar, 0xCACACA}
    };
    
    set_theme_colors( app, colors, ArrayCount( colors ) );
}

CUSTOM_COMMAND_SIG(vim_enter_normal_mode)
{
    set_current_keymap(app, VIM_MAPID_NORMAL);
    
    Theme_Color colors[ ] =
    {
        {Stag_Cursor, 0xFF0000},
        {Stag_At_Cursor, 0x161616},
        {Stag_Mark, 0xFF6F1A},
        {Stag_Margin, 0x33170B},
        {Stag_Margin_Active, 0x934420},
        {Stag_Bar, 0xCACACA},
        {Stag_Ghost_Character, 0xFF0000}
    };
    
    set_theme_colors( app, colors, ArrayCount( colors ) );
}

CUSTOM_COMMAND_SIG(vim_insert_above)
{
    uint32_t access = AccessOpen;
    View_Summary view = get_active_view(app, access);
    Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    seek_beginning_of_line(app);
    refresh_view(app, &view);
    
    int32_t cursor_pos = view.cursor.pos;
    buffer_replace_range(app, &buffer, cursor_pos, cursor_pos, "\n", 1);
    
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_under)
{
    uint32_t access = AccessOpen;
    View_Summary view = get_active_view(app, access);
    Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    seek_end_of_line(app);
    refresh_view(app, &view);
    
    int32_t cursor_pos = view.cursor.pos;
    buffer_replace_range(app, &buffer, cursor_pos, cursor_pos, "\n", 1);
    view_set_cursor(app, &view, seek_pos(cursor_pos+1), true);
    
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_beginning_of_line)
{
    uint32_t access = AccessOpen;
    View_Summary view = get_active_view(app, access);
    //Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    //exec_command(app, seek_beginning_of_line);
    seek_beginning_of_line(app);
    refresh_view(app, &view);
    
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_end_of_line)
{
    uint32_t access = AccessOpen;
    View_Summary view = get_active_view(app, access);
    //Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    //exec_command(app, seek_end_of_line);
    seek_end_of_line(app);
    refresh_view(app, &view);
    
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_write_braces)
{
    uint8_t character1 = '{';
    uint8_t character2 = '}';
    write_character_parameter(app, &character1, 1);
    write_character_parameter(app, &character2, 1);
}

CUSTOM_COMMAND_SIG(vim_write_params)
{
    uint8_t character1 = '(';
    uint8_t character2 = ')';
    write_character_parameter(app, &character1, 1);
    write_character_parameter(app, &character2, 1);
    //vim_enter_normal_mode(app);
}

CUSTOM_COMMAND_SIG(vim_write_brackets)
{
    uint8_t character1 = '[';
    uint8_t character2 = ']';
    write_character_parameter(app, &character1, 1);
    write_character_parameter(app, &character2, 1);
    //vim_enter_normal_mode(app);
}

CUSTOM_COMMAND_SIG(vim_insert_space)
{
    uint8_t character = ' ';
    write_character_parameter(app, &character, 1);
}

CUSTOM_COMMAND_SIG(vim_replace_char)
{
    delete_char(app);
    vim_enter_insert_mode(app);
}

CUSTOM_COMMAND_SIG(vim_seek_next_word)
{
    uint32_t access = AccessOpen;
    View_Summary view = get_active_view(app, access);
    Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    int32_t new_pos = vim_seek_next_word_pos(app, &buffer, view.cursor.pos);
    view_set_cursor(app, &view, seek_pos(new_pos), true);
    refresh_view(app, &view);
    VIM_HOOK_CURR_CUSTOM_CMD(vim_seek_next_word);
}

CUSTOM_COMMAND_SIG(vim_seek_prev_word)
{
    uint32_t access = AccessOpen;
    View_Summary view = get_active_view(app, access);
    Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    int32_t new_pos = vim_seek_prev_word_pos(app, &buffer, view.cursor.pos);
    view_set_cursor(app, &view, seek_pos(new_pos), true);
    refresh_view(app, &view);
    VIM_HOOK_CURR_CUSTOM_CMD(vim_seek_prev_word);
}

CUSTOM_COMMAND_SIG(vim_seek_end_word)
{
    uint32_t access = AccessOpen;
    View_Summary view = get_active_view(app, access);
    Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    int32_t new_pos = vim_seek_end_word_pos(app, &buffer, view.cursor.pos);
    view_set_cursor(app, &view, seek_pos(new_pos), true);
    refresh_view(app, &view);
    VIM_HOOK_CURR_CUSTOM_CMD(vim_seek_end_word);
}

CUSTOM_COMMAND_SIG(vim_insert_after)
{
    View_Summary view = get_active_view(app, AccessOpen);
    Buffer_Summary buffer = get_buffer(app, view.buffer_id, AccessOpen);
    char nextch[2];
    int pos = view.cursor.pos;
    buffer_read_range(app, &buffer, pos, pos + 1, nextch);
    if (nextch[0] != '\n') {
        move_right(app);
    }
    vim_enter_insert_mode(app);
}

START_HOOK_SIG(vim_custom_start)
{
    named_maps = named_maps_values;
    named_map_count = ArrayCount(named_maps_values);
    
    default_4coder_initialize(app);
    hide_scrollbar(app);
    open_panel_vsplit(app);
    hide_scrollbar(app);
    change_active_panel(app);
    
    change_theme(app, literal("Handmade Hero"));
    set_global_face_by_name(app, literal("Droid Sans Mono"), true);
    vim_enter_normal_mode(app);
    toggle_fullscreen(app);
    
    if (global_config.automatically_load_project)
    {
        load_project(app);
    }
    
    // no meaning for return
    return 0;
}

CUSTOM_COMMAND_SIG(vim_seek_panel_bottom)
{
    View_Summary view = get_active_view(app, AccessOpen|AccessProtected);
    
    float new_y = view.scroll_vars.scroll_y + (float)view.view_region.y1 - view.line_height*4.0f;
    float x = view.preferred_x;
    
    view_set_cursor(app, &view, seek_xy(x, new_y, 0, view.unwrapped_lines), 0);
}

CUSTOM_COMMAND_SIG(vim_seek_panel_top)
{
    View_Summary view = get_active_view(app, AccessOpen|AccessProtected);
    
    //float new_y = view.scroll_vars.scroll_y + (float)view.view_region.y0 + view.line_height*4.0f;
    float x = view.preferred_x;
    
    view_set_cursor(app, &view, seek_xy(x, view.scroll_vars.target_y + 20.0f, 0, view.unwrapped_lines), 0);
}

CUSTOM_COMMAND_SIG(vim_seek_panel_middle)
{
    View_Summary view = get_active_view(app, AccessOpen|AccessProtected);
    
    float new_y = view.scroll_vars.scroll_y + ((float)view.view_region.y0 + (float)view.view_region.y1) / 2.0f;
    float x = view.preferred_x;
    
    view_set_cursor(app, &view, seek_xy(x, new_y, 0, view.unwrapped_lines), 0);
}

CUSTOM_COMMAND_SIG(vim_goto_prev_error)
{
    goto_prev_error_no_skips(app);
}

CUSTOM_COMMAND_SIG(vim_goto_next_error)
{
    goto_next_error_no_skips(app);
}

static void
vim_list_all_functions(Application_Links *app, Partition *part, Buffer_Summary *buffer, Buffer_Summary *decls_buffer)
{
    Temp_Memory temp = begin_temp_memory(part);
    
    struct Function_Positions{
        int32_t sig_start_index;
        int32_t sig_end_index;
        int32_t open_paren_pos;
    };
    
    Function_Positions *positions_array = push_array(part, Function_Positions, (4<<10)/sizeof(Function_Positions));
    int32_t positions_count = 0;
    
    Partition extra_memory_ = partition_sub_part(part, (4<<10));
    Partition *extra_memory = &extra_memory_;
    char *str = (char*)partition_current(part);
    int32_t part_size = 0;
    int32_t size = 0;
    
    static const int32_t token_chunk_size = 512;
    Cpp_Token token_chunk[token_chunk_size];
    Stream_Tokens token_stream = {0};
    
    if (init_stream_tokens(&token_stream, app, buffer, 0, token_chunk, token_chunk_size)){
        Stream_Tokens start_position_stream_temp = begin_temp_stream_token(&token_stream);
        
        int32_t token_index = 0;
        int32_t nest_level = 0;
        int32_t paren_nest_level = 0;
        
        int32_t first_paren_index = 0;
        int32_t first_paren_position = 0;
        int32_t last_paren_index = 0;
        
        bool32 still_looping = false;
        
        // Look for the next token at global scope that might need to be printed.
        mode1: do{
            for (; token_index < token_stream.end; ++token_index){
                Cpp_Token *token = &token_stream.tokens[token_index];
                
                if (!(token->flags & CPP_TFLAG_PP_BODY)){
                    switch (token->type){
                        case CPP_TOKEN_BRACE_OPEN:
                        {
                            ++nest_level;
                        }break;
                        
                        case CPP_TOKEN_BRACE_CLOSE:
                        {
                            if (nest_level > 0){
                                --nest_level;
                            }
                        }break;
                        
                        case CPP_TOKEN_PARENTHESE_OPEN:
                        {
                            if (nest_level == 0){
                                first_paren_index = token_index;
                                first_paren_position = token->start;
                                goto paren_mode1;
                            }
                        }break;
                    }
                }
            }
            still_looping = forward_stream_tokens(&token_stream);
        }while(still_looping);
        goto end;
        
        // Look for a closing parenthese to mark the end of a function signature.
        paren_mode1:
        paren_nest_level = 0;
        do{
            for (; token_index < token_stream.end; ++token_index){
                Cpp_Token *token = &token_stream.tokens[token_index];
                
                if (!(token->flags & CPP_TFLAG_PP_BODY)){
                    switch (token->type){
                        case CPP_TOKEN_PARENTHESE_OPEN:
                        {
                            ++paren_nest_level;
                        }break;
                        
                        case CPP_TOKEN_PARENTHESE_CLOSE:
                        {
                            --paren_nest_level;
                            if (paren_nest_level == 0){
                                last_paren_index = token_index;
                                goto paren_mode2;
                            }
                        }break;
                    }
                }
            }
            still_looping = forward_stream_tokens(&token_stream);
        }while(still_looping);
        goto end;
        
        // Look backwards from an open parenthese to find the start of a function signature.
        paren_mode2: {
            Stream_Tokens backward_stream_temp = begin_temp_stream_token(&token_stream);
            int32_t local_index = first_paren_index;
            int32_t signature_start_index = 0;
            
            do{
                for (; local_index >= token_stream.start; --local_index){
                    Cpp_Token *token = &token_stream.tokens[local_index];
                    if ((token->flags & CPP_TFLAG_PP_BODY) || (token->flags & CPP_TFLAG_PP_DIRECTIVE) || token->type == CPP_TOKEN_BRACE_CLOSE || token->type == CPP_TOKEN_SEMICOLON || token->type == CPP_TOKEN_PARENTHESE_CLOSE){
                        ++local_index;
                        signature_start_index = local_index;
                        goto paren_mode2_done;
                    }
                }
                still_looping = backward_stream_tokens(&token_stream);
            }while(still_looping);
            // When this loop ends by going all the way back to the beginning set the signature start to 0 and fall through to the printing phase.
            signature_start_index = 0;
            
            paren_mode2_done:;
            {
                Function_Positions positions;
                positions.sig_start_index = signature_start_index;
                positions.sig_end_index = last_paren_index;
                positions.open_paren_pos = first_paren_position;
                positions_array[positions_count++] = positions;
            }
            
            end_temp_stream_token(&token_stream, backward_stream_temp);
            goto mode1;
        }
        
        end:;
        end_temp_stream_token(&token_stream, start_position_stream_temp);
        // Print the results
        String buffer_name = make_string(buffer->buffer_name, buffer->buffer_name_len);
        for (int32_t i = 0; i < positions_count; ++i){
            Function_Positions *positions = &positions_array[i];
            Temp_Memory extra_temp = begin_temp_memory(extra_memory);
            
            int32_t local_index = positions->sig_start_index;
            int32_t end_index = positions->sig_end_index;
            int32_t open_paren_pos = positions->open_paren_pos;
            
            do{
                for (; local_index < token_stream.end; ++local_index){
                    Cpp_Token *token = &token_stream.tokens[local_index];
                    if (!(token->flags & CPP_TFLAG_PP_BODY)){
                        if (token->type != CPP_TOKEN_COMMENT){
                            bool32 delete_space_before = false;
                            bool32 space_after = false;
                            
                            switch (token->type){
                                case CPP_TOKEN_COMMA:
                                case CPP_TOKEN_PARENTHESE_OPEN:
                                case CPP_TOKEN_PARENTHESE_CLOSE:
                                {
                                    delete_space_before = true;
                                }break;
                            }
                            
                            switch (token->type){
                                case CPP_TOKEN_IDENTIFIER:
                                case CPP_TOKEN_COMMA:
                                case CPP_TOKEN_STAR:
                                {
                                    space_after = true;
                                }break;
                            }
                            if (token->flags & CPP_TFLAG_IS_KEYWORD){
                                space_after = true;
                            }
                            
                            if (delete_space_before){
                                int32_t pos = extra_memory->pos - 1;
                                char *base = ((char*)(extra_memory->base));
                                if (pos >= 0 && base[pos] == ' '){
                                    extra_memory->pos = pos;
                                }
                            }
                            
                            char *token_str = push_array(extra_memory, char, token->size + space_after);
                            
                            buffer_read_range(app, buffer, token->start, token->start + token->size, token_str);
                            if (space_after){
                                token_str[token->size] = ' ';
                            }
                        }
                    }
                    
                    if (local_index == end_index){
                        goto finish_print;
                    }
                }
                still_looping = forward_stream_tokens(&token_stream);
            }while(still_looping);
            
            finish_print:;
            {
                int32_t sig_size = extra_memory->pos;
                String sig = make_string(extra_memory->base, sig_size);
                
                int32_t line_number = buffer_get_line_number(app, buffer, open_paren_pos);
                int32_t line_number_len = int_to_str_size(line_number);
                
                int32_t append_len = buffer_name.size + 1 + line_number_len + 1 + 1 + sig_size + 1;
                
                char *out_space = push_array(part, char, append_len);
                if (out_space == 0){
                    buffer_replace_range(app, decls_buffer, size, size, str, part_size);
                    size += part_size;
                    
                    end_temp_memory(temp);
                    temp = begin_temp_memory(part);
                    
                    part_size = 0;
                    out_space = push_array(part, char, append_len);
                }
                
                part_size += append_len;
                String out = make_string(out_space, 0, append_len);
                append(&out, buffer_name);
                append(&out, ':');
                append_int_to_str(&out, line_number);
                append(&out, ':');
                append(&out, ' ');
                append(&out, sig);
                append(&out, '\n');
            }
            
            end_temp_memory(extra_temp);
        }
        
        buffer_replace_range(app, decls_buffer, size, size, str, part_size);
        
        View_Summary view = get_active_view(app, AccessAll);
        view_set_buffer(app, &view, decls_buffer->buffer_id, 0);
        
        lock_jump_buffer(decls_buffer->buffer_name, decls_buffer->buffer_name_len);
        
        end_temp_memory(temp);
    }
    
}

static void
clear_search_buffer(Application_Links *app, Buffer_Summary *decls_buffer)
{
    String search_name = make_lit_string("*search*");
    *decls_buffer = get_buffer_by_name(app, search_name.str, search_name.size, AccessAll);
    if (!decls_buffer->exists){
        *decls_buffer = create_buffer(app, search_name.str, search_name.size, BufferCreate_AlwaysNew);
        buffer_set_setting(app, decls_buffer, BufferSetting_Unimportant, true);
        buffer_set_setting(app, decls_buffer, BufferSetting_ReadOnly, true);
        buffer_set_setting(app, decls_buffer, BufferSetting_WrapLine, false);
    }
    else{
        buffer_replace_range(app, decls_buffer, 0, decls_buffer->size, 0, 0);
    }
}

CUSTOM_COMMAND_SIG(vim_list_all_functions_current_buffer)
{
    uint32_t access = AccessProtected;
    View_Summary view = get_active_view(app, access);
    Buffer_Summary buffer = get_buffer(app, view.buffer_id, access);
    
    Buffer_Summary search_buffer;
    clear_search_buffer(app, &search_buffer);
    basic_change_active_panel(app);
    
    vim_list_all_functions(app, &global_part, &buffer, &search_buffer);
    basic_change_active_panel(app);
}

CUSTOM_COMMAND_SIG(vim_list_all_functions_globally)
{
    uint32_t access = AccessProtected;
    
    Buffer_Summary search_buffer;
    clear_search_buffer(app, &search_buffer);
    basic_change_active_panel(app);
    
    for (Buffer_Summary bs = get_buffer_first(app, access);
         bs.exists;
         get_buffer_next(app, &bs, access))
    {
        vim_list_all_functions(app, &global_part, &bs, &search_buffer);
    }
    
    basic_change_active_panel(app);
}

CUSTOM_COMMAND_SIG(vim_open_in_other_vsplit)
{
    open_panel_vsplit(app);
    basic_change_active_panel(app);
}

CUSTOM_COMMAND_SIG(vim_delete_line)
{
    VIM_HOOK_CURR_CUSTOM_CMD(delete_line);
    delete_line(app);
}

CUSTOM_COMMAND_SIG(vim_repeat)
{
    if (global_vim_state.curr_custom_cmd)
    {
        global_vim_state.curr_custom_cmd(app);
    }
}

OPEN_FILE_HOOK_SIG(vim_file_settings)
{
    // NOTE(allen|a4.0.8): The get_parameter_buffer was eliminated
    // and instead the buffer is passed as an explicit parameter through
    // the function call.  That is where buffer_id comes from here.
    //vim_enter_normal_mode(app);
    change_theme(app, literal("Handmade Hero"));
    Buffer_Summary buffer = get_buffer(app, buffer_id, AccessAll);
    Assert(buffer.exists);
    
    bool32 treat_as_code = false;
    bool32 treat_as_todo = false;
    bool32 wrap_lines = true;
    bool32 lex_without_strings = false;
    
    CString_Array extensions = get_code_extensions(&global_config.code_exts);
    
    Parse_Context_ID parse_context_id = 0;
    
    if (buffer.file_name != 0 && buffer.size < (16 << 20)){
        String name = make_string(buffer.file_name, buffer.file_name_len);
        String ext = file_extension(name);
        for (int32_t i = 0; i < extensions.count; ++i){
            if (match(ext, extensions.strings[i])){
                treat_as_code = true;
                
                if (match(ext, "cs")){
                    if (parse_context_language_cs == 0){
                        init_language_cs(app);
                    }
                    parse_context_id = parse_context_language_cs;
                }
                
                if (match(ext, "java")){
                    if (parse_context_language_java == 0){
                        init_language_java(app);
                    }
                    parse_context_id = parse_context_language_java;
                }
                
                if (match(ext, "rs")){
                    if (parse_context_language_rust == 0){
                        init_language_rust(app);
                    }
                    parse_context_id = parse_context_language_rust;
                    lex_without_strings = true;
                }
                
                if (match(ext, "cpp") || match(ext, "h") || match(ext, "c") || match(ext, "hpp") || match(ext, "cc")){
                    if (parse_context_language_cpp == 0){
                        init_language_cpp(app);
                    }
                    parse_context_id = parse_context_language_cpp;
                }
                
                // TODO(NAME): Real GLSL highlighting
                if (match(ext, "glsl")){
                    if (parse_context_language_cpp == 0){
                        init_language_cpp(app);
                    }
                    parse_context_id = parse_context_language_cpp;
                }
                
                // TODO(NAME): Real Objective-C highlighting
                if (match(ext, "m")){
                    if (parse_context_language_cpp == 0){
                        init_language_cpp(app);
                    }
                    parse_context_id = parse_context_language_cpp;
                }
                
                break;
            }
        }
        
        if (!treat_as_code){
            String lead_name = front_of_directory(name);
            if (match_insensitive(lead_name, "todo.txt")){
                treat_as_todo = true;
            }
        }
    }
    
    if (treat_as_code){
        wrap_lines = false;
    }
    if (buffer.file_name == 0){
        wrap_lines = false;
    }
    
    //int32_t map_id = (treat_as_code)?((int32_t)default_code_map):((int32_t)mapid_file);
    
    buffer_set_setting(app, &buffer, BufferSetting_WrapPosition, global_config.default_wrap_width);
    buffer_set_setting(app, &buffer, BufferSetting_MinimumBaseWrapPosition, global_config.default_min_base_width);
    buffer_set_setting(app, &buffer, BufferSetting_MapID, VIM_MAPID_NORMAL);
    buffer_set_setting(app, &buffer, BufferSetting_ParserContext, parse_context_id);
    
    if (treat_as_todo){
        buffer_set_setting(app, &buffer, BufferSetting_WrapLine, true);
        buffer_set_setting(app, &buffer, BufferSetting_LexWithoutStrings, true);
        buffer_set_setting(app, &buffer, BufferSetting_VirtualWhitespace, true);
    }
    else if (treat_as_code && buffer.size < (128 << 10)){
        if (global_config.enable_virtual_whitespace){
            // NOTE(allen|a4.0.12): There is a little bit of grossness going on here.
            // If we set BufferSetting_Lex to true, it will launch a lexing job.
            // If a lexing job is active when we set BufferSetting_VirtualWhitespace, the call can fail.
            // Unfortunantely without tokens virtual whitespace doesn't really make sense.
            // So for now I have it automatically turning on lexing when virtual whitespace is turned on.
            // Cleaning some of that up is a goal for future versions.
            if (lex_without_strings){
                buffer_set_setting(app, &buffer, BufferSetting_LexWithoutStrings, true);
            }
            if (global_config.enable_code_wrapping){
                buffer_set_setting(app, &buffer, BufferSetting_WrapLine, true);
            }
            buffer_set_setting(app, &buffer, BufferSetting_VirtualWhitespace, true);
        }
        else if (global_config.enable_code_wrapping){
            if (lex_without_strings){
                buffer_set_setting(app, &buffer, BufferSetting_LexWithoutStrings, true);
            }
            buffer_set_setting(app, &buffer, BufferSetting_Lex, true);
            buffer_set_setting(app, &buffer, BufferSetting_WrapLine, true);
        }
    }
    else{
        buffer_set_setting(app, &buffer, BufferSetting_WrapLine, wrap_lines);
        buffer_set_setting(app, &buffer, BufferSetting_Lex, treat_as_code);
    }
    
    // no meaning for return
    return(0);
}

static void
vim_bind_keys(Bind_Helper *context)
{
    
    begin_map(context, VIM_MAPID_SHARED);
    {
        bind(context, 'n', MDFR_NONE, goto_next_jump_sticky);
        bind(context, 'N', MDFR_NONE, goto_prev_jump_sticky);
        bind(context, 'o', MDFR_CTRL, interactive_open_or_new);
        bind(context, '\n', MDFR_ALT, toggle_fullscreen);
        bind(context, key_f4, MDFR_ALT, exit_4coder);
        bind(context, 'c', MDFR_ALT, open_color_tweaker);
        bind(context, 's', MDFR_NONE, basic_change_active_panel);
        bind(context, 's', MDFR_CTRL, save);
        bind(context, L'Å', MDFR_NONE, execute_any_cli);
        bind(context, L'å', MDFR_NONE, execute_previous_cli);
        bind(context, 't', MDFR_NONE, interactive_switch_buffer);
        bind(context, 'T', MDFR_NONE, interactive_kill_buffer);
        bind(context, 'c', MDFR_NONE, close_panel);
        bind(context, 'Q', MDFR_NONE, open_in_other);
        bind(context, 'Z', MDFR_NONE, vim_open_in_other_vsplit);
        bind(context, 'w', MDFR_CTRL, toggle_virtual_whitespace);
    }
    end_map(context);
    
    begin_map(context, VIM_MAPID_NORMAL);
    {
        inherit_map(context, VIM_MAPID_SHARED);
        bind(context, 'h', MDFR_NONE, move_left);
        bind(context, 'l', MDFR_NONE, move_right);
        bind(context, 'k', MDFR_NONE, move_up);
        bind(context, 'j', MDFR_NONE, move_down);
        bind(context, 'w', MDFR_NONE, vim_seek_next_word);
        bind(context, 'b', MDFR_NONE, vim_seek_prev_word);
        bind(context, 'e', MDFR_NONE, vim_seek_end_word);
        bind(context, 'u', MDFR_NONE, undo);
        bind(context, 'U', MDFR_NONE, redo);
        bind(context, 'y', MDFR_NONE, copy);
        bind(context, 'Y', MDFR_NONE, duplicate_line);
        bind(context, 'p', MDFR_NONE, paste_and_indent);
        bind(context, ' ', MDFR_NONE, set_mark);
        bind(context, 'q', MDFR_NONE, cursor_mark_swap);
        bind(context, 'm', MDFR_NONE, build_in_build_panel);
        bind(context, 'd', MDFR_NONE, delete_range);
        bind(context, 'x', MDFR_NONE, delete_char);
        bind(context, 'D', MDFR_NONE, vim_delete_line);
        bind(context, 'i', MDFR_NONE, vim_enter_insert_mode);
        bind(context, 'a', MDFR_NONE, vim_insert_after);
        bind(context, L'ö', MDFR_NONE, vim_enter_insert_mode);
        bind(context, 'o', MDFR_NONE, vim_insert_under);
        bind(context, 'O', MDFR_NONE, vim_insert_above);
        bind(context, 'A', MDFR_NONE, vim_insert_end_of_line);
        bind(context, 'I', MDFR_NONE, vim_insert_beginning_of_line);
        bind(context, 'g', MDFR_CTRL, goto_line);
        bind(context, '/', MDFR_NONE, search);
        bind(context, 'v', MDFR_NONE, list_all_substring_locations_case_insensitive);
        bind(context, 'V', MDFR_NONE, list_all_locations_of_identifier);
        bind(context, 'f', MDFR_NONE, vim_list_all_functions_globally);
        bind(context, 'L', MDFR_NONE, vim_seek_panel_bottom);
        bind(context, 'M', MDFR_NONE, vim_seek_panel_middle);
        bind(context, 'H', MDFR_NONE, vim_seek_panel_top);
        bind(context, 'g', MDFR_NONE, goto_beginning_of_file);
        bind(context, 'G', MDFR_NONE, goto_end_of_file);
        bind(context, 'r', MDFR_NONE, vim_replace_char);
        bind(context, '.', MDFR_NONE, vim_repeat);
        bind(context, 'z', MDFR_NONE, center_view);
        bind(context, key_page_down, MDFR_NONE, page_down);
        bind(context, key_page_up, MDFR_NONE, page_up);
    }
    end_map(context);
    
    begin_map(context, VIM_MAPID_INSERT);
    {
        inherit_map(context, VIM_MAPID_SHARED);
        bind_vanilla_keys(context, write_character);
        bind(context, L'ö', MDFR_NONE, vim_enter_normal_mode);
        bind(context, L'Ö', MDFR_NONE, backspace_char);
        bind(context, key_back, MDFR_NONE, backspace_char);
        bind(context, '\t', MDFR_NONE, word_complete);
        bind(context, 'K', MDFR_NONE, vim_write_braces);
        bind(context, 'J', MDFR_NONE, vim_write_params);
        bind(context, 'L', MDFR_NONE, vim_write_brackets);
        bind(context, ' ', MDFR_SHIFT, vim_insert_space);
    }
    end_map(context);
}

extern "C" int32_t
get_bindings(void *data, int32_t size)
{
    Bind_Helper context = begin_bind_helper(data, size);
    set_hook(&context, hook_exit, default_exit);
    set_hook(&context, hook_view_size_change, default_view_adjust);
    
    set_start_hook(&context, vim_custom_start);
    set_open_file_hook(&context, vim_file_settings);
    
    set_new_file_hook(&context, default_new_file);
    set_save_file_hook(&context, default_file_save);
    set_end_file_hook(&context, end_file_close_jump_list);
    
    set_command_caller(&context, default_command_caller);
    set_input_filter(&context, default_suppress_mouse_filter);
    set_scroll_rule(&context, smooth_scroll_rule);
    set_buffer_name_resolver(&context, default_buffer_name_resolution);
    vim_bind_keys(&context);
    
    int32_t result = end_bind_helper(&context);
    return(result);
}
