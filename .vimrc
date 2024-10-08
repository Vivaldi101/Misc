﻿filetype off

set exrc
set secure
set nocompatible
set shortmess+=A
set renderoptions=type:directx,level:0.75,gamma:1.25,contrast:0.25,geom:1,renmode:5,taamode:1
set t_Co=256
set encoding=utf-8
set fileencoding=utf-8
set hlsearch
set ignorecase
set cindent
set autoindent
set smartindent
set tabstop=4
set softtabstop=4
set shiftwidth=4
set fillchars=
set number
set incsearch
set laststatus=2
set title

let mapleader="-"
inoremap jk <esc>
vnoremap jk <esc>
onoremap jk <esc>
nnoremap gd gd<C-o>
nnoremap <leader>i viB=<C-o>
nnoremap <leader>s [[
nnoremap <leader>d ]]
map <leader>b %
nnoremap S <C-w>r
nnoremap f :Files<cr>
nnoremap <leader>f :Rg<space>
nnoremap <leader>b :Buffers<cr>
nnoremap <C-c> I//<esc>
nnoremap <C-h> :nohl <cr>
nnoremap <leader>w :e .<cr>
nnoremap <leader>d :LspDefinition <cr>
inoremap <S-j> ()
inoremap <S-k> {}
inoremap <S-l> []
inoremap <leader>s *
inoremap <leader>r &
inoremap <leader>b <bar>
inoremap <leader>t ~
inoremap <leader>c ""
inoremap ö <BS>
nnoremap <C-s> :wa<cr>
map <space> mf
nnoremap d d`f
nnoremap vv v$
nnoremap s <C-w>w


" Install vim-plug
"curl -fLo ~/.vim/autoload/plug.vim --create-dirs \
    "https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim


call plug#begin()

" lsp
Plug 'prabirshrestha/vim-lsp'
Plug 'mattn/vim-lsp-settings'

" fzf
Plug 'junegunn/fzf', { 'do': { -> fzf#install() } }
Plug 'junegunn/fzf.vim'

" autocomplete
" install bear to generate compile_commands.json
Plug 'prabirshrestha/asyncomplete.vim'
Plug 'prabirshrestha/asyncomplete-lsp.vim'

" whitespace
Plug 'ntpeters/vim-better-whitespace'

" colors
Plug 'AlessandroYorba/Alduin'
Plug 'andreasvc/vim-256noir'

" grep replacement
Plug 'jremmen/vim-ripgrep'

" async tasks
Plug 'skywind3000/asyncrun.vim'

call plug#end()


inoremap <expr> <Tab>   pumvisible() ? "\<C-n>" : "\<Tab>"
inoremap <expr> <S-Tab> pumvisible() ? "\<C-p>" : "\<S-Tab>"
inoremap <expr> <cr>    pumvisible() ? asyncomplete#close_popup() : "\<cr>"

colorscheme darkblue

let g:BuildToggle = 1

function! BuildProgram()
	if g:BuildToggle == 1
		let g:BuildToggle = 0
		copen 30
		cbottom
		"exe 'cd ' . g:BuildProgramPath
		silent AsyncRun(./build.sh)
	else
		let g:BuildToggle = 1
		cclose
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

nnoremap <silent> , :wa<CR> :call BuildProgram() <cr>
nnoremap <silent> ö :call ToggleBuildPanel() <cr>
