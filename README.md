# Tairafune's papillon daq and ana tools
Structure
- analysis ... analysis tools in Google Drive
- build
- documents
    - beamtest
    - design
    - reference
- include
- Makefile
- mini.cc
- mini.d
- obj
- output
- README.md
- scripts
- test

# How to run DAQ
See Tairafune's [wiki](https://github.com/316higgs/PAPILLON_FW/wiki/DAQ-scripts).

# Data process
convert binary output to txt, then root.
`makeTree.C` can store waveform of each trigger in TTree.
TTree structure:
```
root [1] papillon->Print()
******************************************************************************
*Tree    :papillon  : papillon                                               *
*Entries :       10 : Total =         1264072 bytes  File  Size =     132446 *
*        :          : Tree compression factor =   9.60                       *
******************************************************************************
*Br    0 :TrigID    : TrigID/I                                               *
*Entries :       10 : Total  Size=        606 bytes  File Size  =        117 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.00     *
*............................................................................*
*Br    1 :head      : head[7]/I                                              *
*Entries :       10 : Total  Size=        842 bytes  File Size  =        114 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.11     *
*............................................................................*
*Br    2 :channel   : channel[24]/I                                          *
*Entries :       10 : Total  Size=       1539 bytes  File Size  =        155 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   6.70     *
*............................................................................*
*Br    3 :sample    : sample[640]/I                                          *
*Entries :       10 : Total  Size=      26176 bytes  File Size  =       3682 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   6.97     *
*............................................................................*
*Br    4 :waveform  : waveform[24][640]/I                                    *
*Entries :       10 : Total  Size=     615725 bytes  File Size  =      62774 *
*Baskets :       10 : Basket Size=      32000 bytes  Compression=   9.80     *
*............................................................................*
*Br    5 :adcwave   : adcwave[24][640]/I                                     *
*Entries :       10 : Total  Size=     615711 bytes  File Size  =      63535 *
*Baskets :       10 : Basket Size=      32000 bytes  Compression=   9.68     *
*............................................................................*
*Br    6 :Qstrip    : Qstrip[24]/I                                           *
*Entries :       10 : Total  Size=       1534 bytes  File Size  =        615 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.69     *
*............................................................................*
*Br    7 :Pedestal  : Pedestal[24]/F                                         *
*Entries :       10 : Total  Size=       1544 bytes  File Size  =        260 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   4.00     *
*............................................................................*
```
