JCC    = javac
JFLAGS =
RM     = rm -f
SRC0   = TokenCount.java
SRC1   = TokenCount1.java
SRC2   = TokenCount2.java
SRC3   = TokenCount3.java
SRC4   = TokenCount4.java
TC0    = TokenCount
TC1    = TokenCount1
TC2    = TokenCount2
TC3    = TokenCount3
TC4    = TokenCount4

all: $(TC0) $(TC1) $(TC2) $(TC3) $(TC4)

$(TC0):
	-$(JCC) $(JFLAGS) $(SRC0)

$(TC1):
	-$(JCC) $(JFLAGS) $(SRC1)

$(TC2):
	-$(JCC) $(JFLAGS) $(SRC2)

$(TC3):
	-$(JCC) $(JFLAGS) $(SRC3)

$(TC4):
	-$(JCC) $(JFLAGS) $(SRC4)

clean:
	-$(RM) *.class *~

fclean: clean

re: clean all
