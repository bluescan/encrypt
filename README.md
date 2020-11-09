# encrypt

An implementation of the *International Data Encryption Algorithm*

Here's the most useful ever implementation of IDEA. This was written after reading the book 'Applied Cryptography' (maybe 2nd edition) by Bruce Schneier. Only source code and project files are provided because you shouldn't trust an executable.

The tool encrypts a file with a passphrase using the 128bit strong IDEA in CBC mode. It also encrypts the filename itself, so a potential attacker has nothing to go on. When run, the tool generates a (new) self-decrypting executable. Your data is your decryption software, so there's never anything to install.

Don't forget your passphrase! Only download this if you are within Canada or the USA. OK, it probably doesn't matter any more, but when this was written encryption source code was still regulated by ITAR.

When you compile this you'll need to do it twice! That sounds odd, but this explains why. At the top of IDEA.cpp you'll see:

```#define EXE_SIZE	NNNNNN // Must be kept up to date!!```

So, compile it once, look at the size (in bytes) of the exe, type that number where you see the NNNNNNs, then compile again. The program appends the encrypted data onto the end of a duplicate copy of the first EXE_SIZE bytes of itself. The reason it can't just look at its own current filesize is that there may already be data appended there. There are 2 main modes the code is compiled in, a CLI and an MFC app. They both do the same job.
