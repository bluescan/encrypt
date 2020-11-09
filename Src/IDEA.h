//
// IDEA.h
//
// Copyright (c) 2000 Tristan Grimmer.
// Please read the accompanying licence file.
//

// Returns success.
bool DoEncrypt(const char* fileName, const char* passPhrase);
bool DoDecrypt(const char* passPhrase);
const char* GetErrorMessage();
