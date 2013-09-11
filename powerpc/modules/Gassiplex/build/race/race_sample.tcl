# Type: Int
# Description: "cardbase" fixed. VME base address of the board race0.
set race0(cardbase) 0x00000000

# Type: String
# Description: "verb" fixed. Verbosity in case of using racectrl.
set race0(verb) info

# Type: Int
# Description: "se_id" fixed. SubEvtId of the data of board race0.
set race0(se_id) 100

# Type: Int
# Description: "irq_on" fixed. Whether or not using interrupts in readout.
set race0(irq_on) 1

# Type: Int
# Description: "irq_vector" fixed. Interrupt vector in case.
set race0(irq_vector) 100

# Type: IntArray
# Description: "poff" fixed. Switching off single ports by putting 1 here.
set race0(poff0) 0
set race0(poff1) 0
set race0(poff2) 0
set race0(poff3) 0
set race0(poff4) 0
set race0(poff5) 0
set race0(poff6) 0
set race0(poff7) 0

# Type: IntArray
# Description: "err_off" fixed. Switching off error checking for single ports.
set race0(err_off0) 0
set race0(err_off1) 0
set race0(err_off2) 0
set race0(err_off3) 0
set race0(err_off4) 0
set race0(err_off5) 0
set race0(err_off6) 0
set race0(err_off7) 0

# Type: Filename
# Description: "rcfpgafile" fixed. rbt-File to load to race0.
set race0(rcfpgafile) slow/race/rbt/rc00.rbt

# Type: Filename
# Description: "allfefile" fixed. rbt-File to load to the frontends on race0.
set race0(allfefile) slow/race/rbt/5frontendv8r10.rbt

# Type: Filename
# Description: "dprfile" fixed. Adress mapping in race0's dual ported memory.
set race0(dprfile) slow/race/dpr/dpr0ipu.dat

# Type: Int
# Description: "thr_id" fixed. Id of the Threshold set downloaded to race0.
set race0(thr_id) 100

# Type: FilenameArray
# Description: "thrfile" fixed. Threshold data files for the ports of race0.
set race0(thrfile0) slow/race/101/s1r0p0.dat
set race0(thrfile1) slow/race/101/s1r0p1.dat
set race0(thrfile2) slow/race/101/s1r0p2.dat
set race0(thrfile3) slow/race/101/s1r0p3.dat
set race0(thrfile4) slow/race/101/s1r0p4.dat
set race0(thrfile5) slow/race/101/s1r0p5.dat
set race0(thrfile6) slow/race/101/s6r0p6.dat
set race0(thrfile7) slow/race/101/s6r0p7.dat

