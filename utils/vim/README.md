-*- Pheaton/utils/vim/README -*-

This directory contains settings for the vim editor to work on Pheaton *.ph files.
It comes with filetype detection rules in the (ftdetect),
syntax highlighting (syntax), some minimal sensible default settings (ftplugin)
and indentation plugins (indent).

To install copy all subdirectories to your $HOME/.vim or if you prefer create
symlinks to the files here. Do not copy the vimrc file here it is only meant as an inspiration and starting point for those working on llvm c++ code.


Add the following to your ~/.vimrc:

```
  " Phaeton source file highlighting mode
  augroup filetype
    au! BufRead,BufNewFile *.ph     set filetype=c
  augroup END
```
