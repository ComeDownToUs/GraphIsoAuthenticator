# GraphIsoAuthenticator

+++++++++++++MIRACL++++++++++++++
This system currently uses the Certivox MIRACL library, found at https://www.certivox.com/miracl for multiple precision arithmetic. I am honestly unsure whether that is okay to host here but the files mirdef.h, miracl.h and miracl.a were compiled through the libraries simple config system and used all of the default settings to be built.
+++++++++++++++++++++++++++++++++

The code represents a prototype authetication protocol for use in small embedded systems within the Internet of Things. The idea is that by using the graph isomorphism problem to autheticate devices, the computational overhead can be significantly lower than that used by existing standard. To increase functionality, this version of the prototype also includes a key exchange mechanism which uses piecewise encoding to transmit public keys within the authentication process.

For a detailed rundown of the code, please read the design and implementation portions of the following paper: https://drive.google.com/file/d/0B9rY91M6v3IxWFp5a1JlWVpnY0k/view?usp=sharing The literature review contains some pretty simple to understand stuff if you want a quick rundown of ZKPs and the like

Let me know anything you would like better comments on and I will add them.


The Generate folder contains all the code with which the graphs and individual user info accounts are created with
The code seemed to run into some kind of issue with graphs with fewer than 32 nodes, would love to hear what I'm doing wrong there. It also crashes at sizes greater than 158 nodes, which I don't entirely understand, there were previous issues in a few spots with unsigned chars but I'm lost here.

=======================
AREAS TO IMPROVE UPON
=======================
1. Reduce usage of "decompress_graph"
The compression algorithm for digital representations of graphs significantly cuts down upon the bandwitdh and storage overheads involved by reducing the code to a bit level and removing what amounts to near 50% repetition of data. However, the current processes to verify isomorphisms requires decompressing these graphs, I am pretty confident alternative versions of verify_graphs and permGraph within gmw.h could be designed which achieve all of this without the need decompress these graphs at all.

2. Use an alternative arbitrary precision arithmetic library
MIRACL caused me numerous problems throughout, most importantly it kept running into issues with key sizes of 2048-bit and larger. While it worked an absolute treat with ECDH (which I also coded but is less stable than this version so I haven't uploaded it), the licensing restrictions of the library seriously call into question whether it's worth persisting with so if someone wants to try a version with GMP, I'd love to see it!

3. Combine gmwCli and gmwServ, miracl_extensions and miracl_ext_sev
For testing purposes, I found a client server model much easier to work with at this level of development but perhaps someone else may see differently? Any kind of real implementation would require a P2P setup.
Also, the inclusion of a respected third party might be helpful? The current system depends on all existing deviced to be stored already within the Users folder.

4. Arduino and Contiki(!) versions
I managed to adapt the pre-key exhange version of this code quite easily to arduino and contiki but I'm definitely not an expert on either. If someone would like to help me out with a Contiki version in particular it would be fantastic!




RUN PROGRAM
=====================
COMPILE SERVER: $ gcc -pthread -o [name] gmwServ.c miracl.a 
COMPILE CLIENT: $ gcc -o [name2] gmwCli.c miracl.a
RUN SERVER: $ [location+name] [portNo]
RUN CLIENT: $ [location+name2] [ServerIP] [server portNo]

The server is multithreaded.
