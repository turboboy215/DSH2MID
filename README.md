# DSH2MID
David Shea (Coyote Developments) (GBC) to MIDI

This tool converts music (and sound effects) from Game Boy Color games using David Shea (of Coyote Developments)'s sound engine to MIDI format. This sound engine was used in the majority of games developed by Crawfish.

It works with ROM images. To use it, you must specify the name of the ROM followed by the number of the bank containing the sound data (in hex).
For games that contain multiple banks of music, you must run the program multiple times specifying where each different bank is located. However, in order to prevent files from being overwritten, the MIDI files from the previous bank must either be moved to a separate folder or renamed.

Examples:
* DSH2MID "Aladdin (E) (M6) [C][!].gbc" F
* DSH2MID "Aladdin (E) (M6) [C][!].gbc" 33
* DSH2MID "Godzilla - The Series (E) (M3) [C][a1].gbc" 21
* DSH2MID "Space Invaders (U) [C][!].gbc" 5
* DSH2MID "Space Invaders (U) [C][!].gbc" 6
* DSH2MID "Space Invaders (U) [C][!].gbc" 7
* DSH2MID "Space Invaders (U) [C][!].gbc" 8

This tool was based on my own reverse-engineering of the sound engine, partially based on disassembly of Aladdin's sound code.

Also included is another program, DSH2TXT, which prints out information about the song data from each game. This is essentially a prototype of DSH2MID.

Supported games:
  * Aladdin (GBC)
  * Cruis'n Exotica
  * Driver
  * ECW Hardcore Revolution
  * Freestyle Scooter
  * Godzilla: The Series
  * Godzilla: The Series: Monster Wars
  * LEGO Island 2: The Brickster's Revenge
  * Mary-Kate and Ashley: Get a Clue!
  * The New Adventures of Mary-Kate and Ashley
  * Ready 2 Rumble Boxing
  * Space Invaders (GBC)
  * Street Fighter Alpha: Warriors' Dreams
  * Tom Clancy's Rainbow Six
  * X-Men: Mutant Academy

## To do:
  * GBS file support
