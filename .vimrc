set exrc
set secure
set nocompatible
filetype off
set noswapfile
set encoding=utf-8
set renderoptions=type:directx,level:0.75,gamma:1.25,contrast:0.25,geom:1,renmode:5,taamode:1
syntax on

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim

call vundle#begin()

Plugin 'VundleVim/Vundle.vim'

Plugin 'tpope/vim-fugitive.git'
Plugin 'skywind3000/asyncrun.vim'
Plugin 'itchyny/lightline.vim.git'

" Can remove this in future.
"Plugin 'itchyny/vim-gitbranch.git'

Plugin 'ycm-core/YouCompleteMe.git'
Plugin 'CreaturePhil/vim-handmade-hero.git'
Plugin 'andreasvc/vim-256noir.git'
Plugin 'junegunn/fzf.git'
Plugin 'junegunn/fzf.vim.git'
Plugin 'drmikehenry/vim-headerguard.git'
Plugin 'stsewd/fzf-checkout.vim'
Plugin 'simnalamburt/vim-mundo.git'
"Plugin 'ervandew/supertab.git'
"Plugin 'neoclide/coc.nvim', {'branch': 'release'}

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required

let g:BuildToggle = 1
let g:BuildOld = 0

let g:EnginePath = "~/Old/ui/Engine/"
let g:PlatformPath = "configs/platforms/linux_x11_egl/"
"let g:PluginPath = "~/Old/ui/Engine/plugins/qmw/Application/configs/platforms/qnx700_screen_x86_64/"
"let g:PlatformPath = "configs/platforms/linux_x11_glx_cpp98/"
"let g:PlatformPath = "configs/platforms/qnx700_screen_x86_64/"
"let g:PlatformPath = "configs/platforms/qnx700_screen_aarch64_cxx/"
"let g:PlatformPath = "configs/platforms/qnx710_screen_aarch64_cxx/"
"let g:PlatformPath = "configs/platforms/qnx710_screen_x86_64_cxx/"
"let g:PlatformPath = "configs/platforms/linux_drm_gbm_x86_64/"
"let g:PlatformPath = "configs/platforms/linux_drm_gbm_aarch64_gcc113/"
"let g:PlatformPath = "configs/platforms/linux_nogl/"
"let g:PlatformPath = "configs/platforms/linux_x11_armhf/"
"let g:PlatformPath = "configs/platforms/linux_x11_glx/"
"let g:PlatformPath = "configs/platforms/linux_wayland_x86_64_clang_ng/"
"let g:PlatformPath = "configs/platforms/linux_wayland_x86_64_clang/"

" Auto format code
"autocmd BufWritePre *.h,*.hpp,*.c,*.cpp,*.vert,*.frag :call FormatBuffer()

let g:KZBPlayerPath = EnginePath . "applications/kzb_player/" . PlatformPath
let g:IntegrationPath = "~/Old/Old-integration/prototypes/windowing/"
let g:CustomPath = "~/Old/Old-integration/testing/windowing/"

if g:BuildOld == 1
    "let g:BuildProgramPath = EnginePath . PlatformPath
    let g:BuildProgramPath = KZBPlayerPath
else
    let g:BuildProgramPath = IntegrationPath
endif

function! CleanProgram()
    if g:BuildToggle == 1
        let g:BuildToggle = 0
        copen 30
        exe 'cd ' . g:PlatformPath
        silent AsyncRun(/usr/bin/scons -c)
    else
        let g:BuildToggle = 1
        cclose
    endif
endfunction

function! AbortProgram()
    call AsyncStop
endfunction

if g:BuildOld == 1
    function! BuildProgram()
    if g:BuildToggle == 1
        let g:BuildToggle = 0
        copen 30
        cbottom
        exe 'cd ' . g:BuildProgramPath
        silent AsyncRun(python /usr/bin/scons -j 6 debug)
    else
        let g:BuildToggle = 1
        cclose
    endif
    endfunction
    function! BuildProgramDebug()
    if g:BuildToggle == 1
        let g:BuildToggle = 0
        copen 30
        cbottom
        exe 'cd ' . g:BuildProgramPath
        silent AsyncRun(/usr/bin/env python2 /usr/bin/scons -j 6)
    else
        let g:BuildToggle = 1
        cclose
    endif
    endfunction
else
    function! BuildProgram()
    if g:BuildToggle == 1
        let g:BuildToggle = 0
        copen 30
        cbottom
        exe 'cd ' . g:BuildProgramPath
        silent AsyncRun(./build.sh x11_egl debug -std=c++14)
    else
        let g:BuildToggle = 1
        cclose
    endif
    endfunction
endif

"-------------------------------------------

"autocmd CursorMoved ControlP
            "\ resize

colorscheme handmade-hero
"colorscheme 256_noir
"colorscheme mountaineer-grey

"let &winheight=&lines * 8 / 10


let mapleader = "-"
let g:AutoPairsShortcutFastWrap=''
let g:netrw_localcopydircmd = 'cp -r'
let g:netrw_localrmdircmd = 'rm -r'
let g:netrw_keepdir=0
let g:netrw_list_hide = '\(^\|\s\s\)\zs\.\S\+'
autocmd FileType netrwset setl bufhidden=delete

" Key Mappings
"nnoremap <esc> :q<CR>
noremap <A-u> [m
noremap <A-i> ]m
inoremap ä <S-*>
nnoremap <leader>i viB=<C-o>
nnoremap <leader>b %
nnoremap S <C-w>r
nnoremap <leader>z 0I//<esc>
nnoremap <leader>u 0xx
nnoremap <leader>n gea!<esc>
nnoremap <leader>s i""<esc>
"nnoremap <leader>c ]c
"nnoremap <leader>C [c
nnoremap <leader>. @:
nnoremap <C-u> 0wxx
nnoremap <leader>v F(wvf,ldf)i, <esc>pF,vf)hd
nnoremap <leader>q :call CopyToVM()<CR>
inoremap jk <esc>
vnoremap jk <esc>
onoremap jk <esc>
inoremap <S-j> ()
inoremap <S-k> {}
inoremap <S-l> []
inoremap <leader>s *
inoremap <leader>r &
inoremap <leader>b <bar>
inoremap <leader>t ~
nnoremap <leader>x $x
inoremap ö <BS>
inoremap Ö <C-n>
map <space> mf
nnoremap d d`f
nnoremap vv v$
nnoremap z zz
inoremap <leader>v <
inoremap <leader>e >
inoremap <leader>a ->
inoremap <leader>d // TODO:

nnoremap <leader>c "_yiwi/*<esc>ea*/<esc>
nnoremap <leader>j viw<esc>a)<esc>bi(<esc>lel
nnoremap <leader>k viw<esc>a}<esc>bi{<esc>lel
nnoremap <leader>l viw<esc>a]<esc>bi[<esc>lel
nnoremap <leader>i ^iif (<esc>$a)<esc>
nnoremap <leader>1 I#if 0<esc>
nnoremap <leader>2 I#endif<esc>

"nnoremap <leader>f viB=
nnoremap <leader>g ggVG=

nnoremap <C-h> :nohl<CR>

nnoremap <leader>w :e .<CR>

"nnoremap <leader>s f}i<space><esc>F{a<space><esc>

"Window switching
nnoremap s <C-w>w

nnoremap <C-s> :wa<CR>

nnoremap <C-c> :AsyncStop <CR>
nnoremap <silent> ; :wa<CR> :call CleanProgram() <CR>
nnoremap <silent> , :wa<CR> :call BuildProgram() <CR>
nnoremap <silent> ö :call ToggleBuildPanel() <CR>

"Go to previous error
nnoremap <F3> :cp<CR>

"Go to next error
nnoremap <F4> :cn<CR>

" Line moving

nnoremap <silent><A-k> :move -2<CR>
nnoremap <silent><A-j> :move +1<CR>

" Vimspector
nnoremap <F5> :call vimspector#Launch()<CR>
nnoremap <F6> :call vimspector#Continue()<CR>
nnoremap <F7> :call vimspector#Reset()<CR>
nnoremap <F9> :call vimspector#ToggleBreakpoint()<CR>
nnoremap <F10> :call vimspector#StepOver()<CR>
nnoremap <F11> :call vimspector#StepInto()<CR>

func! VSAddToWatch()
    let word = expand("<cexpr>")
    call vimspector#AddWatch(word)
endfunction

"func! VSCustomUI()
"call win_gotoid( g:vimspector_session_windows.output )
"q
"endfunction

"func! VSSetupTerminal()
"call win_gotoid( g:vimspector_session_windows.terminal )
"endfunction

"augroup VSCustomisation
"autocmd!
"autocmd User VimspectorUICreated call s:VSCustomUI()
"autocmd User VimspectorTerminalOpened call s:VSSetupTerminal()
"augroup end


" Headerguard
nnoremap <silent><leader>h :call InsertNewHppFileInfo()<CR>

" Maximizer
"nnoremap <silent><A-h> :MaximizerToggle<CR>

" Lightline
let g:lightline = {
            \ 'colorscheme' : 'simpleblack',
            \ 'active' : {
                \ 'left' : [ ['mode', 'paste'],
                \            [ 'gitbranch', 'readonly', 'filename', 'modified'] ]
                \ },
                \ 'component_function' : {
                    \ 'gitbranch' : 'gitbranch#name'
                    \ },
                    \ }

" Some funky status bar code its seems
" https://stackoverflow.com/questions/9065941/how-can-i-change-vim-status-line-colour
set laststatus=2            " set the bottom status bar

function! ModifiedColor()
    if &mod == 1
        hi statusline guibg=White ctermfg=8 guifg=Red ctermbg=15
    else
        hi statusline guibg=White ctermfg=8 guifg=DarkSlateGray ctermbg=15
    endif
endfunction

"au InsertLeave,InsertEnter,BufWritePost   * call ModifiedColor()

highlight VertSplit cterm=NONE ctermbg=NONE ctermfg=NONE
" Formats the statusline
set statusline=%f                           " file name
"set statusline+=[%{strlen(&fenc)?&fenc:'none'}, "file encoding
"set statusline+=%{&ff}] "file format
"set statusline+=%y      "filetype
"set statusline+=%h      "help file flag
"set statusline+=[%{getbufvar(bufnr('%'),'&mod')?'MODIFIED':'SAVED'}]
"modified flag

"set statusline+=%r      "read only flag

set statusline+=\ %=                        " align left
set statusline+=Line:%l/%L[%p%%]            " line X of Y [percent of file]
set statusline+=\ Col:%c                    " current column
set statusline+=\ Buf:%n                    " Buffer number
"set statusline+=\ [%b][0x%B]\               " ASCII and byte code under cursor
" default the statusline when entering Vim
"hi statusline guibg=White ctermfg=8 guifg=DarkSlateGray ctermbg=15
set showtabline=0
set number
set autoread
set history=1000
set nosol
set hlsearch
set hidden
set cindent
set autoindent
set smartindent
set tabstop=4
set softtabstop=4
set shiftwidth=4
set expandtab
"set guifont=Noto\ Mono\ Bold\ 16
"set guifont=Monospace\ Bold\ 16
"set guifont=Liberation\ Mono\ Bold\ 16
set guifont=Liberation\ Mono\ 18
set backspace=indent,eol,start
set incsearch
set guioptions-=m
set guioptions-=T
set guioptions-=r
set guioptions-=L
"set guioptions+=k
set cmdheight=2
set ignorecase
set smartcase
set noshowmode
set showmatch
set antialias
set switchbuf-=split

set maxmempattern=2000000
set shortmess-=S
set fillchars=eob:\ ,
set path+=**
set wildmenu

" deoplete
"let g:deoplete#enable_at_startup = 1

" vim-plug; for other plugin managers, use what's appropriate
" if you don't want to trust a prebuilt binary, skip the 'do' part
" and build the binaries yourself. Instructions are further down
" and place them in the /path/to/plugin/rplugin/python3 folder

"Plug 'raghur/fruzzy', {'do': { -> fruzzy#install()}}

" optional - but recommended - see below
"let g:fruzzy#usenative = 1

" When there's no input, fruzzy can sort entries based on how similar they are to the current buffer
" For ex: if you're on /path/to/somefile.h, then on opening denite, /path/to/somefile.cpp
" would appear on the top of the list.
" Useful if you're bouncing a lot between similar files.
" To turn off this behavior, set the variable below  to 0

"let g:fruzzy#sortonempty = 1 " default value

" AsyncRun
"let g:asyncrun_open = 10

" YCM
"let g:ycm_global_ycm_extra_conf = 'C:\Code\Repos\feature\ui\Engine\.ycm_extra_conf.py'
let g:ycm_seed_identifiers_with_syntax=1
let g:ycm_confirm_extra_conf=1
"let g:ycm_collect_identifiers_from_tag_files = 1
let g:ycm_always_populate_location_list = 1
let g:ycm_clangd_args = ['--clang-tidy']
let g:ycm_clangd_args=['--header-insertion=never']
set completeopt=longest,menu

nnoremap <F12> :YcmCompleter GoTo<CR>
"nnoremap <leader>fi :YcmCompleter FixIt<CR>

" Airline
let g:airline_theme = 'minimalist'

" FZF
" Default fzf layout
" - Popup window (center of the screen)
"let g:fzf_layout = { 'window': { 'width': 1.0, 'height': 1.0 } }

" - Popup window (center of the current window)
"let g:fzf_layout = { 'window': { 'width': 0.9, 'height': 0.6, 'relative': v:true } }

" - Popup window (anchored to the bottom of the current window)
"let g:fzf_layout = { 'window': { 'width': 0.9, 'height': 0.6, 'relative': v:true, 'yoffset': 1.0 } }

"nnoremap<silent> f :Files ~/Old/ui/Engine<CR>
nnoremap<silent> f :call fzf#run(fzf#wrap({'source': 'find ~/Old/Old-integration -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.py" -or -name "*.json" -or -name "*.sh" -or -name "*.rst" -or -name "*.cmake"'}))<CR>
"nnoremap<silent> f :Files!<CR>
"nnoremap<silent> f :call fzf#run(fzf#wrap({'source': 'find ~ -name "*.cpp" -or -name "*.c" -or -name "*.hpp" -or -name "*.h" -or -name "*.py"'}))<CR>
nnoremap<silent> t :Buffers<CR>
nnoremap<silent><A-l> :BLines<CR>
nnoremap<silent><A-h> :Tags<CR>
nnoremap<silent><A-f> :Rg<CR>
" MRU list
nnoremap<silent><A-o> :call fzf#run({ 'source': v:oldfiles, 'sink': 'e', 'options': '-m -x +s' })<CR>

"command! FZFMru call fzf#run({ 'source': v:oldfiles, 'sink': 'e', 'options': '-m -x +s' })

" Overwrite :Rg from fzf.vim
" Hit '?' to toggle the preview
command! -bang -nargs=* Rg call fzf#vim#grep(
  \   'rg
        \ --column
        \ --line-number
        \ --no-heading
        \ --fixed-strings
        \ --ignore-case
        \ --hidden
        \ --follow
        \ --glob "!.git/*"
        \ --color "always" '.shellescape(<q-args>),
  \   fzf#vim#with_preview('right:50%:hidden', '?'),
  \   <bang>0)

nmap <F2> :MundoToggle<CR>


" Search pattern across repository files
function! FzfExplore(...)
    let inpath = substitute(a:1, "'", '', 'g')
    if inpath == "" || matchend(inpath, '/') == strlen(inpath)
        execute "cd" getcwd() . '/' . inpath
        let cwpath = getcwd() . '/'
        call fzf#run(fzf#wrap(fzf#vim#with_preview({'source': 'ls -1ap', 'dir': cwpath, 'sink': 'FZFExplore', 'options': ['--prompt', cwpath]})))
    else
        let file = getcwd() . '/' . inpath
        execute "e" file
    endif
endfunction

command! -nargs=* FZFExplore call FzfExplore(shellescape(<q-args>))

"vimrc mappings
nnoremap <leader>e :e $MYVIMRC<CR>

" Highlight all instances of word under cursor, when idle.
" Useful when studying strange source code.
"nnoremap <leader>h :if AutoHighlightToggle()<Bar>set hls<Bar>endif<CR>


hi clear SignColumn

"caret color
highlight Cursor guifg=white guibg=green
highlight iCursor guifg=white guibg=red

set guicursor=n-v-c:block-Cursor
set guicursor+=i:ver100-iCursor
set guicursor+=a:blinkon0

"hi CursorLine   cterm=NONE ctermbg=Brown ctermfg=white

" old
"
"
set cursorline
highlight CursorLine cterm=NONE ctermbg=121212 ctermfg=NONE guibg=#121312 guifg=NONE

"autocmd InsertLeave * highlight CursorLine cterm=NONE ctermfg=NONE ctermbg=133 guibg=#00ab00 guifg=NONE
"autocmd InsertEnter * highlight CursorLine cterm=NONE ctermfg=NONE ctermbg=204 guibg=#301212 guifg=NONE
"
autocmd InsertLeave * highlight CursorLine cterm=NONE ctermfg=NONE ctermbg=233 guibg=#122212 guifg=NONE
autocmd InsertEnter * highlight CursorLine cterm=NONE ctermfg=NONE ctermbg=204 guibg=#2c1c1c guifg=NONE

highlight EndOfBuffer ctermfg=NONE ctermbg=NONE

hi NonText guifg=bg

" Thanks to https://forums.handmadehero.org/index.php/forum?view=topic&catid=4&id=704#3982
" error message formats
" Microsoft MSBuild
"set errorformat+=\\\ %#%f(%l\\\,%c):\ %m
" Microsoft compiler: cl.exe
"set errorformat+=\\\ %#%f(%l)\ :\ %#%t%[A-z]%#\ %m
" Microsoft HLSL compiler: fxc.exe
"set errorformat+=\\\ %#%f(%l\\\,%c-%*[0-9]):\ %#%t%[A-z]%#\ %m


function! AutoHighlightToggle()
    let @/ = ''
    if exists('#auto_highlight')
        au! auto_highlight
        augroup! auto_highlight
        setl updatetime=50
        echo 'Highlight current word: off'
        return 0
    else
        augroup auto_highlight
            au!
            au CursorHold * let @/ = '\V\<'.escape(expand('<cword>'), '\').'\>'
        augroup end
        setl updatetime=50
        echo 'Highlight current word: ON'
        return 1
    endif
endfunction



"function! BuildProgram()
"    if g:BuildToggle == 1
"        OpenBuildPanel(1)
"        g:BuildToggle = 0
"        exe 'cd' . g:BuildProgramPath
"       "silent AsyncRun(./build_wayland.sh debug)
"        silent AsyncRun(scons2 -j 8)
"    else
"        g:BuildToggle = 1
"        OpenBuildPanel(0)
"    endif
"endfunction

function! OpenBuildPanel(PanelToggle)
    if a:PanelToggle == 1
        copen 30
    else
        close
    endif
endfunction

function! ToggleBuildPanel()
    if g:BuildToggle == 1
        let g:BuildToggle = 0
        copen 30
        cbottom
    else
        let g:BuildToggle = 1
        close
    endif
endfunction

function! RunProgram()
    exe 'cd' . g:BuildDirectory
    silent AsyncRun ./run_drm_gbm_debug.sh
endfunction

function! InsertNewHppFileInfo()
    call HeaderguardAdd()
    const copyright = "// Copyright 2008-2024 by Rightware. All rights reserved."

    call append(0, copyright)
    call append(1, "")
    call cursor(6, 2)

    wa
endfunction

function! InsertNewCppFileInfo()
    const copyright = "// Copyright 2008-2024 by Rightware. All rights reserved."

    call append(0, copyright)
    call append(1, "")
    call cursor(2, 1)

    wa
endfunction

function! CopyToVM()
    silent AsyncRun(cp /home/joni/Old/ui/Engine/applications/kzb_player/bin/kzb_player ~/Qnx700VM/kzb_player_custom)
endfunction

" Remove trailing whitespaces
autocmd BufWritePre *.c,*.h,*.cpp,*.hpp,*.sh :%s/\s\+$//e

autocmd BufNewFile *.hpp call InsertNewHppFileInfo()
autocmd BufNewFile *.cpp call InsertNewCppFileInfo()

"autocmd BufWritePre *.hpp call InsertNewHppFileInfo()
"autocmd BufWritePre *.cpp call InsertNewHppFileInfo()

hi NonText guifg=bg

"autocmd FileType c,cpp,hpp,h ClangFormatAutoEnable
function FormatBuffer()
  if !empty(findfile('_clang-format', expand('%:p:h') . ';'))
    let cursor_pos = getpos('.')
    :%!clang-format
    call setpos('.', cursor_pos)
  endif
endfunction

" Fugitive
autocmd BufReadPost fugitive://* set bufhidden=delete
