#! /usr/bin/env python

import sys


class passwordComposer:
    def __init__(self, minLevel = 1, maxLevel = 3, wordList = [], joinChars = ['', '_', '-', ' '], allowDoubles = False):
        self.__wordList     = wordList
        self.__joinChars    = joinChars
        self.__minLevel     = minLevel
        self.__maxLevel     = maxLevel
        self.__allowDoubles = allowDoubles

    def __usage(self, exitCode, msg = ""):
        sys.stderr.write('''%s

Usage: ./passwordComposer.py [OPTIONS] wordLists...

OPTIONS:
--------
    -h|--help
        Print this usage and go.

    -m|--minLevel <int>
        Minimum number of concatenated words (default: 1)

    -M|--maxLevel <int>
        Maximum number of concatenated words (default: 3)

    -j|--joinChars <coma separated strings>
        Characters to use for concatenations (default: ",_,-, ")

    -D|--allowDoubles
        Allows a word to be concatenated more than once
''' % msg)
        sys.exit(exitCode)


    def readWordList(self, filename):
        try:
            f = open(filename, "r")
            data = f.readlines()
            f.close()

            for i in range(len(data)):
                data[i] = data[i].strip()
                if data[i] != "":
                    self.__wordList.append(data[i])

        except IOError as e:
            sys.stderr.write("%s\n" % e)


    def getOpt(self):
        try:
            import getopt

            opts, args = getopt.getopt(sys.argv[1:],
                                       "hm:M:j:D",
                                       ["help", "minLevel=", "maxLevel=", "joinChars=", "allowDoubles"])
        except getopt.GetoptError, err:
            self.__usage(2, err)

        if len(args) < 1:
            self.__usage(2, "No word list specified")

        for o, a in opts:
          if o in   ("-h", "--help"):
              self.__usage(0)
          elif o in ("-d", "--debug"):
              debug = True
          elif o in ("-m", "--minLevel"):
              self.__minLevel = int(a)
          elif o in ("-M", "--maxLevel"):
              self.__maxLevel = int(a)
          elif o in ("-j", "--joinChars"):
              self.__joinChars = a.split(",")
          elif o in ("-D", "--allowDoubles"):
              self.__allowDoubles = True
          else:
              self.__usage(2, "Unhandled option: '" + o + "'")

        for wordList in args:
            self.readWordList(wordList)

        if len(self.__wordList) < 2:
            sys.stderr.write("Not enough words\n")
            sys.exit(3)

        if len(self.__wordList) < self.__maxLevel:
            self.__maxLevel = len(self.__wordList)


    def __generate(self, currentLevel, currentWord):
        if currentLevel <= self.__maxLevel:
            print currentWord
            for word in self.__wordList:
                if self.__allowDoubles or not (word in currentWord):
                    for char in self.__joinChars:
                        self.__generate(currentLevel + 1, "%s%s%s" % (currentWord, char, word))


    def generate(self):
        for word in self.__wordList:
            self.__generate(1, word)


if __name__ == "__main__":
    composer = passwordComposer()
    composer.getOpt()
    composer.generate()

