# Encrypt

An implementation of the *International Data Encryption Algorithm*

Here's the most useful ever implementation of IDEA. This was written after reading the book 'Applied Cryptography' (maybe 2nd edition) by Bruce Schneier. Only source code and project files are provided because you shouldn't trust an executable.

# Tool

The tool encrypts a file with a passphrase using the 128bit strong IDEA in CBC mode. It also encrypts the filename itself, so a potential attacker has nothing to go on. When run, the tool generates a (new) self-decrypting executable. Your data is your decryption software, so there's never anything to install.

Don't forget your passphrase! Only download this if you are within Canada or the USA. OK, it probably doesn't matter any more, but when this was written encryption source code was still regulated by ITAR.

When you compile this you'll need to do it twice! That sounds odd, but this explains why. At the top of IDEA.cpp you'll see:

```#define EXE_SIZE	NNNNNN // Must be kept up to date!!```

So, compile it once, look at the size (in bytes) of the exe, type that number where you see the NNNNNNs, then compile again. The program appends the encrypted data onto the end of a duplicate copy of the first EXE_SIZE bytes of itself. The reason it can't just look at its own current filesize is that there may already be data appended there. There are 2 main modes the code is compiled in, a CLI and an MFC app. They both do the same job.

# Info
Here's some info I got from the web. [Here is the original source](https://www.iusmentis.com/security/passphrasefaq/practical/).

A dictionary [3] has around 74,000 words in it. Using the 128 bit key size we then need, log(2^128) / log(74,000) = 7.91,
random words from our dictionary. Rounding up, you will then need 8 random words to make the passphrase harder than the
IDEA key. A brute force dictionary attack will then take slightly longer than a brute force attack on the IDEA key. This
is a decent way to generate a passphrase except that it is kind of hard to remember sometimes. This is pretty easy to
type though.

PGP uses IDEA as the conventional cipher. The key for IDEA is 128 bits. We can calculate the brute force key space with 2^128 = 3.4E38. A special hardware based key cracker for IDEA that can try
one billion (1E9) keys per second will take 1.08E22 years to go through all possible keys. You can expect to get most keys in about half that time which will take 5.39E21 years. It is estimated that the sun
will go nova in 1E9 years. Since the algorithm is secure, the cryptanalyst has to go after other things like RSA or your passphrase. It is currently beyond our technology to crack an IDEA key by brute
force. 

