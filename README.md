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
