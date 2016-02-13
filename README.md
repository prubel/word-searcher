# word-searcher
Solve a word search, even without knowing the words in it.

Finds words in a grid, forward, backward, up, down, and on diagonals. Useful for
puzzles such as the mystery hunt or homework. Brute forces the search so it works even
if you don't know the exact words you're looking for as long as they are in your dictionary, 

Tested on mac and linux. 
Requires g++/llvm and a postscript viewer such as gv on linux or open on that mac.
You'll also need a list of words to look for. I use the words from /usr/share/dict/words.

## Usage 
    
    Usage: ./solve {-n min-word-length} {-d dictionary-file} {-o output-file} {-f found-words-file}
    default minimum word length is 5. 
    default dictionary is words.txt, one word per line
    default output is res.ps
    default found-words-file is found_words.txt
       use -f to limit hits by editing it to remove unwanted
       words and then passing it in via -d 

Look at your results by opening output-file using a postscript/pdf viewer

Input is taken from standard in, first the number of rows and then each row.
You can type in the rows or use some OCR software to take them from an image.

## Compiling 

If you have a g++ you should just be able to run make.


## Example

     $ ./solve -d /usr/share/dict/words
      How many rows are there? 
      15
      Enter a row
      BDISOFTENINGYIT
      ZZVPXBEUTIRFMEO
      FZGGOIPARALLAXR
      DJEVGNILEVOHSIA
      EEGOPAYXGVZYTPT
      FAJNKSBSWNJAHGI
      NEWTONSQUELCHES
      DEVITAICOSSAMDV
      DSERITERTWAUYEB
      TNEMHSINABZCYIZ
      AJNSTNEMTAERTPS
      TRAWYRROWWFGCXJ
      GRADIOTELEPHONE
      NTJHHLOYALISTUQ
      YREPELLENTSSMEX
      Searching 
      Searching 
       Found soften at 0:3 of length 6 find at 98:278 to 173:278
       Found softening at 0:3 of length 9 find at 98:278 to 218:278
       Found often at 0:4 of length 5 find at 113:278 to 173:278
       Found parallax at 2:6 of length 8 find at 143:248 to 248:248
       Found newton at 6:0 of length 6 find at 53:188 to 128:188
       Found newtons at 6:0 of length 7 find at 53:188 to 143:188
       Found squelch at 6:6 of length 7 find at 143:188 to 233:188
       Found squelches at 6:6 of length 9 find at 143:188 to 263:188
       Found evita at 7:1 of length 5 find at 68:173 to 128:173
       Found radio at 12:1 of length 5 find at 68:98 to 128:98
       Found radiotelephone at 12:1 of length 14 find at 68:98 to 263:98
       Found telephone at 12:6 of length 9 find at 143:98 to 263:98
       Found phone at 12:10 of length 5 find at 203:98 to 263:98
       Found loyal at 13:5 of length 5 find at 128:83 to 188:83
       Found loyalist at 13:5 of length 8 find at 128:83 to 233:83
       Found repel at 14:1 of length 5 find at 68:68 to 128:68
       Found repellent at 14:1 of length 9 find at 68:68 to 188:68
       Found repellents at 14:1 of length 10 find at 68:68 to 203:68
       Found ellen at 14:4 of length 5 find at 113:68 to 173:68
       Found lents at 14:6 of length 5 find at 143:68 to 203:68
       Found shove at 3:2 of length 5 find at 233:233 to 173:233
       Found shovel at 3:2 of length 6 find at 233:233 to 158:233
       Found shoveling at 3:2 of length 9 find at 233:233 to 113:233
       Found hovel at 3:3 of length 5 find at 218:233 to 158:233
       Found apogee at 4:9 of length 6 find at 128:218 to 53:218
       Found associative at 7:3 of length 11 find at 218:173 to 68:173
       Found retire at 8:7 of length 6 find at 158:158 to 83:158
       Found retires at 8:7 of length 7 find at 158:158 to 68:158
       Found tires at 8:9 of length 5 find at 128:158 to 68:158
       Found banish at 9:5 of length 6 find at 188:143 to 113:143
       Found banishment at 9:5 of length 10 find at 188:143 to 53:143
       Found treat at 10:2 of length 5 find at 233:128 to 173:128
       Found treatment at 10:2 of length 9 find at 233:128 to 113:128
       Found treatments at 10:2 of length 10 find at 233:128 to 98:128
       Found worry at 11:6 of length 5 find at 173:113 to 113:113
       Found worrywart at 11:6 of length 9 find at 173:113 to 53:113
       Found leper at 14:9 of length 5 find at 128:68 to 68:68
       Found sitar at 14:8 of length 5 find at 263:188 to 263:248
       Found tarot at 14:10 of length 5 find at 263:218 to 263:278
       Found rents at 12:3 of length 5 find at 128:113 to 188:173
     $
    
    You can put your puzzle in a file and call it like so:
     $ cat puzzle.txt
     15
     BDISOFTENINGYIT
     ZZVPXBEUTIRFMEO
     FZGGOIPARALLAXR
     DJEVGNILEVOHSIA
     EEGOPAYXGVZYTPT
     FAJNKSBSWNJAHGI
     NEWTONSQUELCHES
     DEVITAICOSSAMDV
     DSERITERTWAUYEB
     TNEMHSINABZCYIZ
     AJNSTNEMTAERTPS
     TRAWYRROWWFGCXJ
     GRADIOTELEPHONE
     NTJHHLOYALISTUQ
     YREPELLENTSSMEX
    
     $ ./solve -d /usr/share/dict/words < puzzle.txt

 Should show the same output as above.
    
 We can then look at the found words in found_words.txt
 
    $ head found_words.txt 
     soften
     softening
     often
     parallax
     newton
     newtons
     squelch
    
    And see the answers 
    $ evince res.ps
    
![Example output](/images/example-output.png "Example")
    
If you wanted to remove some unwanted words from the output try using found_words.txt as your dictionary, removing the words you don't want, and running again. 

    $ ./solve -d found_words.txt  < puzzle.txt

 
## Thanks 
Thanks to Arash Partow for providing the bloom filter code. 
http://www.partow.net/programming/hashfunctions/index.html 



