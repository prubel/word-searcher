# word-searcher
Solve a word search, even without knowing the words in it.

Finds words in a grid, forward, backward, up, down, and on diagonals. Useful for
puzzles such as the mystery hunt or homework. Brute forces the search so it works even
if you don't know the exact words you're looking for as long as they are in your dictionary, 

Tested on mac and linux. 
Requires g++/llvm and a postscript viewer such as gv on linux or open on that mac.
You'll also need a list of words to look for. I use the words from /usr/share/dict/words.

= Usage =

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

= Compiling =

If you have a g++ you should just be able to run make.

= Thanks =
Thanks to Arash Partow for providing the bloom filter code. 
http://www.partow.net/programming/hashfunctions/index.html 
